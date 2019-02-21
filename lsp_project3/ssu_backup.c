#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <signal.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <ctype.h>
#include <pthread.h>

#define MAX_LENGTH 255
#define BUFFER_SIZE 1024

typedef struct f_info { //d옵션 내애서 사용된 디렉토리내의 모든 파일에 대한 정보 저장할 구조체
	int ino;
	int mtime;
	int exist;
	char name[BUFFER_SIZE];
}F_INFO;

int str_compare_sort(const void *a, const void *b); //퀵정렬에 사용될 함수
int ssu_daemon_unit(void); //디몬 프로세스 수행 함수
int getProcName(char *pPath, char *pname); //현재 수행중인 프로세스들 이름 가져오는 함수
void ssu_cp(char *src); //백업 수행하는 함수
void backup_log(char *src); //백업 로그 입력하는 함수
void daemon_exist(char *fname); //이미 실행중인 디몬 있는지 확인하는 함수
void not_exist_log(char *fname); //파일이 존재하지 않을때 존재하지 않는다는 로그메세지 띄워주는 함수
void n_opt_func(char *src); //n옵션 함수
void c_opt_func(char *src); //c옵션 함수
void r_opt_func(char *src); //r옵션 함수
void d_opt_func(char *src); //d옵션 함수
void fcnt(char *src); //d옵션 초기에 디렉토리내의 파일개수 측정하는 함수
void *th_func(void *fname); //쓰레드가 수행할 함수 포인터
static void daemon_kill(int signo); //SIGUSR1들어왔을 시 수행할 핸들러
static void sys(int signo); //디몬 프로세스 종료되었을 시(SIGUSR1 제외)의 핸들러
char *timeToString(struct tm *t); //파일의 mtime을 구해오는 함수1
char *timeToString2(struct tm *t); //파일의 mtime을 구해오는 함수2j
char *get_fname(char *pathname); //경로 포함 파일명에서 파일명만 추출해오는 함수

pthread_t p_thread[BUFFER_SIZE];
F_INFO info[BUFFER_SIZE];
char working_dir[BUFFER_SIZE];
char org_files[BUFFER_SIZE][BUFFER_SIZE];
time_t org_mtime, timer;
struct tm *t;
int flag_d = 0, flag_r = 0, flag_m = 0, flag_n = 0, flag_c = 0, n_option_num = 0, d_opt_num = 0;
int tot_fcnt = 0, d_handle = 0, sub_fcnt = 0, sub2_fcnt = 0, d_cnt;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[])
{//-c -r옵션의 경우 period입력받으면 오류처리
	struct stat statbuf;
	int param_opt, i = 0, j = 0;
	char opt[2][BUFFER_SIZE];
	pid_t pid;
	daemon_exist(argv[0]);
	getcwd(working_dir, sizeof(working_dir));

	while((param_opt = getopt(argc, argv, "drmcn:")) != -1) {
		switch(param_opt) {
			case 'd':
				flag_d++;
				break;
			case 'r':
				flag_r++;
				break;
			case 'm':
				flag_m++;
				break;
			case 'c':
				flag_c++;
				break;
			case 'n':
				flag_n++;
				char *ch = optarg;
				float f = atof(ch);
				n_option_num = atoi(ch); //n옵션 뒤에 오는 숫자 받아줌
				if(n_option_num <= 0 || isalpha(*ch) != 0 || (f - n_option_num) != 0.0) {//N에 대한 오류처리
					fprintf(stderr, "must use natural number after -n option\n");
					exit(1);
				}
				break;
		}
	}
	for(; optind < argc; optind++) { //옵션 제외 입력받은 값들에 대해서 정렬 해줌
		strcpy(opt[i], argv[optind]);
		i++;
	}
	for(j = 0; j < strlen(opt[0]); j++) { //파일명이 영어로만 이루어져있는지 확인 후 아닐시 오류출력후 종료
		if(!isalpha(opt[0][i])) {
			fprintf(stderr, "filename is not composed entirely in English\n");
			exit(1);
		}
	}
	if(!flag_d) { //d옵션 제외 모든 옵션들에 대해
		if((lstat(opt[0], &statbuf)) < 0) { //백업파일본 없을 경우
			fprintf(stderr, "file is not exist\n");
			exit(1);
		}
		else {
			if(!S_ISREG(statbuf.st_mode)) { //백업대상 파일이 정규파일이 아닐경우 에러문 출력하고 종료
				fprintf(stderr, "%s is no regular file\n", opt[0]); 
				exit(1);
			}
			org_mtime = statbuf.st_mtime; //백업대상 파일의 mtime을 구해놓음
		}
	}
	if(flag_d) {//d옵션 들어왔을 경우 디렉토리인지 확인
		if((lstat(opt[0], &statbuf)) < 0) {
			fprintf(stderr, "directory is not exist\n");
			exit(1);
		}
		else {
			if(!S_ISDIR(statbuf.st_mode)) {
				fprintf(stderr, "%s is not directory\n", opt[0]);
				exit(1);
			}
		}
	}
	if(flag_c) { //c옵션 들어왔을 경우 다른 옵션 같이 들어왔는지 확인
		if((flag_d || flag_r || flag_m || flag_n) != 0) {
			fprintf(stderr, "-c option must used single\n");
			exit(1);
		}
		else { //c옵션만 사용된 경우
			if(i > 1) { //period 같이 입력한 경우에는 에러 출력 후 종료
				fprintf(stderr, "you input PERIOD[%d] with -c option\n", atoi(opt[1]));
				exit(1);
			}
			char buf[BUFFER_SIZE];
			struct stat r_statbuf;
			strcpy(buf, working_dir);
			strcat(buf, "/backup_dir");
			if(stat(buf, &r_statbuf) < 0) { //백업 디렉토리가 존재하지 않을경우에 종료
				fprintf(stderr, "backup directory is not exist\n");
				exit(0);
			}
			daemon_exist(argv[0]);
			c_opt_func(realpath(opt[0], NULL));
			exit(0);
		}
	}
	if(flag_r) { 
		if((flag_d || flag_c || flag_m || flag_n) != 0) { //r옵션이 다른 옵션이랑 사용되었는지 확인
			fprintf(stderr, "-r option must used single\n");
			exit(1);
		}
		else { //r옵션만 사용된 경우
			if(i > 1) {  //period 같이 입력한 경우에는 에러 출력 후 종료
				fprintf(stderr, "you input PERIOD[%d] with -r option\n", atoi(opt[1]));
				exit(0);
			}
			char buf[BUFFER_SIZE];
			struct stat r_statbuf;
			strcpy(buf, working_dir);
			strcat(buf, "/backup_dir");
			if(stat(buf, &r_statbuf) < 0) {  //백업 디렉토리가 존재하지 않을경우에 종료
				fprintf(stderr, "backup directory is not exist\n");
				exit(0);
			}
			daemon_exist(argv[0]);
			r_opt_func(realpath(opt[0], NULL));
			exit(0);
		}
	}
	if(atoi(opt[1]) < 3 || atoi(opt[1]) > 10) { //preiod 확인 부분
		fprintf(stderr, "period input error\nperiod usage : 3 ~ 10\n");
		exit(1);
	}
	if(n_option_num == 0 && flag_n && !flag_d) {//n옵션 뒤의 숫자가 0인데 n옵션 들어왔을시
		fprintf(stderr, "must input [N] after -n option\n-n option usage : %s <FILENAME> [PERIOD] -n [N]\n", argv[0]);
		exit(1);
	}
	pid = getpid();
	printf("parent process : %d\n", pid);
	printf("daemon process initialization\n");
	
	if(ssu_daemon_unit() < 0) { //디몬 프로세스 수행
		fprintf(stderr, "ssu_daemon_init failed\n");
		exit(1);
	}

	while(1) {
		signal(SIGKILL, sys); //kill시그널에 대해 핸들러 
		signal(SIGSEGV, sys); //세그멘트시그널에 대해 핸들러
		signal(SIGUSR1, daemon_kill);
		daemon_exist(argv[0]);
		if((flag_d || flag_r || flag_m || flag_c || flag_n) == 0) { //아무 옵션도 안들어온 경우
			signal(SIGUSR1, daemon_kill);
			struct stat or_statbuf;
			if(stat(opt[0], &or_statbuf) < 0)  //파일이 없을 경우에는 존재하지않는다는 함수 call함
				not_exist_log(opt[0]);
			ssu_cp(realpath(argv[1], NULL));
			backup_log(argv[1]);
			sleep(atoi(argv[2]));
		}
		else if(flag_m && !flag_n && !flag_d) { //m옵션만 들어온 경우
			struct stat m_statbuf;
			daemon_exist(argv[0]);
			if(stat(opt[0], &m_statbuf) < 0){
				not_exist_log(opt[0]);
				exit(1);
			}
			else {
				if(org_mtime != m_statbuf.st_mtime) { //mtime이 바뀐경우만 백업 수행
					ssu_cp(realpath(opt[0], NULL));
					backup_log(opt[0]);
				}
				else
					;
			}
			sleep(atoi(opt[1]));
		}
		else if(flag_n && !flag_d) { //n옵션 들어왔지만 d옵션은 안들어온 경우
			signal(SIGUSR1, daemon_kill);
			if(!flag_m) { //m옵션이 들어오지 않은경우
				n_opt_func(realpath(opt[0], NULL));
				ssu_cp(realpath(opt[0], NULL));
				backup_log(opt[0]);
			}
			else { //m옵션이 들어온 경우에는 
				struct stat m_statbuf;
				if(stat(opt[0], &m_statbuf) < 0) {
					not_exist_log(opt[0]);
					exit(1);
				}
				else {
					if(org_mtime != m_statbuf.st_mtime) { //시간이 변경된 경우에만 백업수행
						n_opt_func(realpath(opt[0], NULL));
						ssu_cp(realpath(opt[0], NULL));
						backup_log(opt[0]);
					}
				}
			}
			sleep(atoi(opt[1]));
		}
		else if(flag_d){ //d랑 m같이 쓰이는 경우엔 쓰레드 생성하되 백업파일이 안만들어지는 것이다
			signal(SIGUSR1, daemon_kill);
			if(tot_fcnt >= sub2_fcnt) { //파일이 더 추가된 경우 포함
				d_opt_func(opt[0]);
				sub2_fcnt = 0;
			}
			else if(tot_fcnt < sub2_fcnt) {//디렉토리내에 파일이 삭제된 경우
				tot_fcnt = 0;
				d_opt_num = 0;
				d_opt_func(opt[0]);
				sub2_fcnt = 0;
			}
			if(d_opt_num == 1) { //d 옵션이 한번 돈 이후
				int cnt;
				for(cnt = 0; cnt < tot_fcnt; cnt++) {
					if(info[cnt].exist == 1) {
						not_exist_log(info[cnt].name);
						d_handle = 1;
					}
				}
				for(cnt = 0; cnt < tot_fcnt; cnt++) {
					info[cnt].exist = 1;
				}
			}
			sleep(atoi(opt[1]));
			signal(SIGUSR1, daemon_kill);
			fcnt(opt[0]);
			sub_fcnt = 0;
			d_opt_num = 1;	
		}
		else{
			;
		}
		signal(SIGSEGV, sys);
		signal(SIGKILL, sys);
	}
	exit(0);
}
void fcnt(char *src) { //d옵션 수행시 sleep전에 디렉토리 내의 파일들 갯수 초기에 세는 함수
	DIR *dir;
	struct dirent *dirp;
	struct stat statbuf;
	char fname[BUFFER_SIZE];
	int  status;
	int cnt = 1;
	pthread_t thread_t;

	strcpy(fname ,src);
	dir = opendir(fname);
	while((dirp = readdir(dir)) != NULL) {
		if((strcmp(dirp->d_name, ".") != 0) && (strcmp(dirp->d_name, "..") != 0)) {
			char buf[BUFFER_SIZE];
			
			strcpy(buf, fname);
			strcat(buf, "/");
			strcat(buf, dirp -> d_name);
			stat(buf, &statbuf);
		
			if(S_ISDIR(statbuf.st_mode))
				fcnt(buf);
			else {
				if(S_ISREG(statbuf.st_mode)) {
					strcpy(org_files[sub2_fcnt], buf);
					sub2_fcnt++;
				}
			}
		}
	}
	if(flag_m) { //m옵션이 들어왔을 경우
		int i, j;
		if(sub2_fcnt > tot_fcnt) { //sleep이전에 수행한 d옵션 함수에서 센 파일들 숫자보다 지금의 파일 숫자들이 더 많았을 경우에는 파일이 추가된 경우이므로 수행
			for(i = 0; i < sub2_fcnt; i++) { //추가된 파일명 가져오는 반복문
				for(j = 0; j < sub2_fcnt; j++) {
					if(strcmp(org_files[i], info[j].name) == 0) {
						cnt *= 0;
					}
				}
				if(cnt == 1) {  //있을경우에는 쓰레그 생성 후 백업 수행후 쓰레드 종료
					int k = strlen(org_files[i]);
					if(org_files[i][k-1] == 'p' && org_files[i][k-2] == 'w' && org_files[i][k-3] == 's')//스왑파일을 만나게될 경우 무시
						continue;
					pthread_create(&thread_t, NULL, th_func, (void *)org_files[i]);
					pthread_join(thread_t, (void **)&status);
					backup_log(org_files[i]);
				}

				cnt = 1;
			}
		}
	}

}
void d_opt_func(char *src) {
	DIR *dir;
	struct dirent *dirp;
	struct stat statbuf;
	char fname[BUFFER_SIZE];
	int thr_id, status;
	pthread_t thread_t, thread_t1;

	strcpy(fname ,src);
	dir = opendir(fname);
	while((dirp = readdir(dir)) != NULL) {
		if((strcmp(dirp->d_name, ".") != 0) && (strcmp(dirp->d_name, "..") != 0)) {
			char buf[BUFFER_SIZE];
			
			strcpy(buf, fname);
			strcat(buf, "/");
			strcat(buf, dirp -> d_name);
			stat(buf, &statbuf);
		
			if(S_ISDIR(statbuf.st_mode))
				d_opt_func(buf);
			else {
				if(S_ISREG(statbuf.st_mode)) {
					if(d_opt_num == 0) { //d옵션 초기상태 수행 경우
						int k = strlen(buf);
						if(buf[k-1] == 'p' && buf[k-2] == 'w' && buf[k-3] == 's')//스왑파일명을 만나게 될 경우 무시
							continue;
						info[tot_fcnt].ino = dirp -> d_ino; //각 파일들의 정보를 구조체에 담아줌
						info[tot_fcnt].mtime = statbuf.st_mtime;
						info[tot_fcnt].exist = 1;
						strcpy(info[tot_fcnt].name, buf);
						if(!flag_m && !flag_n) { //d옵션만 들어온 경우
							org_mtime = info[tot_fcnt].mtime; //쓰레드 생성 후 백업 수행
							if(stat(info[tot_fcnt].name, &statbuf) < 0) {
								if(tot_fcnt == 0) {
									d_opt_num = 0;
								}
								continue;
							}
							pthread_create(&thread_t, NULL, th_func, (void *)info[tot_fcnt].name);
							pthread_join(thread_t, (void **)&status); //쓰레드 종료
							backup_log(info[tot_fcnt].name);
							info[tot_fcnt].mtime = org_mtime;
						}	
						else if(flag_n && !flag_m) { //n옵션과 같이 들어온 경우
							org_mtime = info[tot_fcnt].mtime; 
							n_opt_func(realpath(info[tot_fcnt].name, NULL)); //n옵션 함수 수행
							pthread_create(&thread_t, NULL, th_func, (void *)info[tot_fcnt].name); //쓰레드 생성하여 백업 수행
							pthread_join(thread_t, (void **)&status); //쓰레드 종료
							backup_log(info[tot_fcnt].name);
						}
						else
							;
						tot_fcnt++;
					}
					else {
						if(d_handle == 1) {	 //디렉토리내에 파일이 지워졌을경우
							int k = strlen(buf);
							if(buf[k-1] == 'p' && buf[k-2] == 'w' && buf[k-3] == 's')//스왑파일명을 만나게 될 경우 무시
								continue;
							info[sub_fcnt].ino = dirp -> d_ino;
							info[sub_fcnt].mtime = statbuf.st_mtime;
							info[sub_fcnt].exist = 1;
							strcpy(info[sub_fcnt].name, buf);	
							sub_fcnt++;
							tot_fcnt = sub_fcnt;
						}
						for(d_cnt = 0; d_cnt < tot_fcnt; d_cnt++) {
							int k = strlen(buf);
							if(buf[k-1] == 'p' && buf[k-2] == 'w' && buf[k-3] == 's')//스왑파일명을 만나게 될 경우 무시
								continue;
							if(stat(info[d_cnt].name, &statbuf) < 0) {
								if(tot_fcnt == 0)
									d_opt_num = 0;
								continue;
							}
							if(strcmp(info[d_cnt].name, buf) == 0) {
								if(!flag_m && !flag_n) { //d옵션만 들어온 경우
									org_mtime = info[d_cnt].mtime; //모든 파일들에 대해서 org_mtime을 넣어주고 쓰레드 생성 후 백업 수행
									
									pthread_create(&thread_t, NULL, th_func, (void *)info[d_cnt].name);
									pthread_join(thread_t, (void **)&status);
									backup_log(info[d_cnt].name);
									info[d_cnt].mtime = org_mtime;
								}
								else if(flag_n && !flag_m) { //n옵션만 들어온 경우
									org_mtime = info[d_cnt].mtime;
									n_opt_func(realpath(info[d_cnt].name, NULL)); //n옵션 함수 수행
									pthread_create(&thread_t, NULL, th_func, (void *)info[d_cnt].name); //쓰레드 생성 후 백업 수행
									pthread_join(thread_t, (void **)&status);
									backup_log(info[d_cnt].name);
									info[d_cnt].mtime = org_mtime;
								}
								else
									;
								info[d_cnt].exist = 0; //디렉토리 내의 모든 파일정보 구조체에 대해 exist멤버 0으로 초기화
							}
						}
					}
				}
			}
		}
	}
	if(!flag_n&&flag_m && d_opt_num == 0 && d_handle == 1) {
	}
	else if(!flag_n && flag_m && d_opt_num == 1 && d_handle == 0) {//m옵션만 들어온 경우
		for(d_cnt = 0; d_cnt < tot_fcnt; d_cnt++) {
			int k = strlen(info[d_cnt].name);
			if(info[d_cnt].name[k-1] == 'p' && info[d_cnt].name[k-2] == 'w' && info[d_cnt].name[k-3] == 's')//스왑파일명을 만나게 될 경우 무시
				continue;

			struct stat m_statbuf;
			if(stat(info[d_cnt].name, &m_statbuf) < 0) {
				if(tot_fcnt == 0)
					tot_fcnt = 0;
				continue;
			}
			if(info[d_cnt].mtime != m_statbuf.st_mtime) { //각 파일들의 mtime을 비교 후 다를 경우에만
				org_mtime = info[d_cnt].mtime;
				pthread_create(&thread_t, NULL, th_func, (void *)info[d_cnt].name);  //쓰레드 생성 후 백업 수행
				pthread_join(thread_t, (void **)&status); //백업 수행 후 쓰레드 종료
				backup_log(info[d_cnt].name);
				info[d_cnt].mtime = m_statbuf.st_mtime; //mtime 변경된거 다시 적용시켜줌
			}
		}
	}
	else if(flag_n && flag_m && d_opt_num == 1 && d_handle == 0) { //d, n, m옵션 다같이 사용했을경우
		for(d_cnt = 0; d_cnt < tot_fcnt; d_cnt++) {
			int k = strlen(info[d_cnt].name);
			if(info[d_cnt].name[k-1] == 'p' && info[d_cnt].name[k-2] == 'w' && info[d_cnt].name[k-3] == 's')//스왑파일명을 만나게될 경우 무시
				continue;
			struct stat mn_statbuf;
			if(stat(info[d_cnt].name, &mn_statbuf) < 0) {
				if(tot_fcnt == 0)
					tot_fcnt = 0;
				continue;
			}//디렉토리 내 파일들 순차적으로 stat구조체 에 넣어줌
			n_option_num++; //n_option_num을 1로해줌
			n_opt_func(realpath(info[d_cnt].name, NULL)); //n_option함수 호출
			n_option_num--; //n_option_num을  0으로 다시 해줌

			if(info[d_cnt].mtime != mn_statbuf.st_mtime) { //mtime시간이 다를경우엔
				org_mtime = info[d_cnt].mtime;
				pthread_create(&thread_t, NULL, th_func, (void *)info[d_cnt].name); //쓰레드 생성 후 백업 수행
				n_opt_func(realpath(info[d_cnt].name, NULL)); //n_option함수 다시 수행
				pthread_join(thread_t, (void **)&status); //해당 쓰레드 종료
				backup_log(info[d_cnt].name);
				info[d_cnt].mtime = mn_statbuf.st_mtime;//다시 갱신된 mtime을 구조체 배열에 넣어줌
			}
		}
	}
	else
		;
	d_handle = 0;
}
void *th_func(void *fname) {
	ssu_cp(realpath((char *)fname, NULL));
}
void r_opt_func(char *src) { //r옵션에 대해서 수행해주는 함수
	DIR *dir;
	struct dirent *dirp;
	struct stat statbuf;
	int i, j = 0, k = 0, l = 0, flag = 1, num = 0, input, ch;
	char backup_files[BUFFER_SIZE][BUFFER_SIZE];
	char backup_name[BUFFER_SIZE];
	char backup_time[BUFFER_SIZE];
	char backup_dir[BUFFER_SIZE];
	char buf[BUFFER_SIZE], buf2[BUFFER_SIZE];
	char fname[BUFFER_SIZE];
	pid_t pid;

	strcpy(buf, src);
	strcpy(fname, get_fname(buf));

	for(i = 0; i < strlen(buf); i++) { //백업파일명으로 변환
		sprintf(buf2, "%x", buf[i]);
		strcat(backup_name, buf2);
	}
	sprintf(backup_dir, "%s/%s", working_dir, "backup_dir");
	dir = opendir(backup_dir);
	while((dirp = readdir(dir)) != NULL) { //백업 디렉토리 내에 모든 백업 파일들에 대해
		int test;
		char buf3[BUFFER_SIZE];
		strcpy(buf3, dirp -> d_name);
		if(strlen(buf3) > 3) {
			for(i = 0; i < strlen(buf3); i++) {
				if(buf3[i] == '_') {
					j = i-1;
					break;
				}
			}
			for(i = 0; i < j; i++) {
				if(buf3[i] == backup_name[i])
					flag *= 1;
				else
					flag *= 0;
			}
			if(flag) { //파일명 부분만 다 비교해서 일치할 경우에는 backup_files배열에 추가해줌
				strcpy(backup_files[num], working_dir);
				strcat(backup_files[num], "/");
				strcat(backup_files[num], "backup_dir/");
				strcat(backup_files[num], buf3);
				num++;
			}
		}
		flag = 1;
	}
	if(num == 0) { //백업파일 존재하지 않을경우에는 해당 문구 출력하고 종료
		printf("backupfile is not exist\n");
		exit(1);
	}
	else { //존재할 경우에는
		qsort((void *)backup_files, num, sizeof(backup_files[0]), str_compare_sort); //모든 일치한 파일들에 대해서 정렬해줌
		printf("<%s backup list>\n", fname); //백업 리스트 출력
		printf("0. exit\n");
		for(i = 0; i < num; i++) { 
			stat(backup_files[i], &statbuf);
			for(j = strlen(backup_files[i]); j >= 0; j--) {
				if(backup_files[i][j] == '_') {
					k = j+1;
					break;
				}
			}
			for(j = k; j < strlen(backup_files[i]); j++) { //일치한 백업파일들의 백업시간을 다 구해줌
				backup_time[l] = backup_files[i][j];
				l++;
				backup_time[l] = '\0';
			}
			printf("%d. %s_%s [size:%ld]\n", i+1, fname, backup_time, statbuf.st_size); //구한 백업시간과 사이즈를 구해 문구 출력
			l = 0;
			backup_time[l] = '\0';
		}
		while(1) {
			printf("input : "); //입력받는 걸 대기
			scanf("%d", &input);
			if(input == 0)
				exit(0);
			else if(input >= 1 && input <= num+1) {
				printf("Recovery backup file...\n");
				printf("[%s]\n", fname);
				FILE *fp1, *fp2;
				fp1 = fopen(backup_files[input-1], "r");
				fp2 = fopen(src, "w+");
				if(fp1 == NULL || fp2 == NULL) {
					fprintf(stderr, "fopen error for %s or %s\n", backup_files[input-1], src);
					exit(1);
				}
				while((ch = fgetc(fp1)) != EOF) { //입력받은 숫자에 대한 백업파일을 원본파일로 불러들여온다
					fputc(ch, fp2);
					fputc(ch, stdout);
				}
				fclose(fp1);
				fclose(fp2);
				exit(0);
			}
			else {
				printf("there is no selection %d\n", input);
			}
		}
	}
}
void c_opt_func(char *src) { //c옵션 함수
	DIR *dir;
	struct dirent *dirp;
	struct stat statbuf;
	int i, j = 0, flag = 1, num = 0, status;
	char backup_files[BUFFER_SIZE][BUFFER_SIZE];
	char backup_name[BUFFER_SIZE];
	char backup_dir[BUFFER_SIZE];
	char buf[BUFFER_SIZE], buf2[BUFFER_SIZE];
	char fname[BUFFER_SIZE];
	pid_t pid;

	strcpy(buf, src);
	strcpy(fname, get_fname(buf));

	for(i = 0; i < strlen(buf); i++) {
		sprintf(buf2, "%x", buf[i]);
		strcat(backup_name, buf2);
	}
	sprintf(backup_dir, "%s/%s", working_dir, "backup_dir");
	dir = opendir(backup_dir);
	while((dirp = readdir(dir)) != NULL) { //백업 디렉토리내에서 모든 백업파일들 읽어들임
		char buf3[BUFFER_SIZE];
		strcpy(buf3, dirp -> d_name);
		if(strlen(buf3) > 3) {
			for(i = 0; i < strlen(buf3); i++) {
				if(buf3[i] == '_') {
					j = i-1;
					break;
				}
			}
			for(i = 0; i < j; i++) { //백업파일명(시간부분 제외)을 다 비교한다 같은 경우엔 flag1로
				if(buf3[i] == backup_name[i])
					flag *= 1;
				else
					flag *= 0;
			}
			if(flag) {
				strcpy(backup_files[num], working_dir);
				strcat(backup_files[num], "/");
				strcat(backup_files[num], "backup_dir/");
				strcat(backup_files[num], buf3);
				num++;
			}
		}
		flag = 1;
	}
	if(num == 0) {
		printf("backupfile is not exist\n");
		exit(1);
	}
	qsort((void *)backup_files, num, sizeof(backup_files[0]), str_compare_sort); //해당 일치하는 백업파일명들 퀵정렬해줌
	if(stat(backup_files[num-1], &statbuf) < 0) { //해당 백업파일에 대해 stat오류가 날 경우
		fprintf(stderr, "file is no exist\n");
		exit(1);
	}
	printf("<Compare with backup file[%s_%s]>\n", fname, timeToString2(localtime(&statbuf.st_mtime))); //해당 백업파일과 비교시작
	if((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid == 0) { //자식 프로세스 생성 후 자식 프로세스에게 diff수행하게함
		char *args[] = {"diff", src, backup_files[num-1], NULL}; //가장 최근의 백업파일에 대해 diff명령어 수행
		if(execv("/usr/bin/diff", args) < 0) {
			fprintf(stderr, "exec error\n");
			exit(1);
		}
		exit(3);
	}

	if(wait(&status) != pid) { //부모 프로세스는 자식 프로세스가 모든걸 수행하고 종료할때까지 대기
		fprintf(stderr, "wait error\n");
		exit(1);
	}
	exit(0);
}
void n_opt_func(char *src) { //n옵션 들어왔을시 수행하는 함수
	FILE *fp;
	char buf[BUFFER_SIZE];
	char buf2[BUFFER_SIZE];
	char backup_dir[BUFFER_SIZE];
	char backup_fname[BUFFER_SIZE] = "";
	char backup_files[BUFFER_SIZE][BUFFER_SIZE];
	char backup_time[BUFFER_SIZE];
	char log_msg[BUFFER_SIZE] = "";
	char fname[BUFFER_SIZE] = "";
	char log_fname[BUFFER_SIZE] = "";
	DIR *dir;
	struct dirent *dirp;
	struct stat statbuf;
	int flag = 1;
	int num = 0, i, j;
	
	strcpy(log_fname, working_dir);
	strcat(log_fname, "/backup_log.txt");
	strcpy(buf, src);
	strcpy(fname, get_fname(buf));
	fp = fopen(log_fname, "a+"); //입력받은 파일명을 백업파일명으로 변환
	for(i = 0; i < strlen(buf); i++) {
		sprintf(buf2, "%x", buf[i]);
		strcat(backup_fname, buf2);
	}
	sprintf(backup_dir, "%s/%s", working_dir,"backup_dir");
	mkdir(backup_dir, 0777);//백업 디렉토리 없을경우 생성
	dir = opendir(backup_dir);
	while((dirp = readdir(dir)) != NULL) { //백업디렉토리내의 모든 파일을 다 읽어옴
		char buf3[BUFFER_SIZE];
		strcpy(buf3, dirp -> d_name);
		if(strlen(buf3) > 3) {
			for( i = 0; i < strlen(buf3); i++) {//백업파일명에서의 _이전까지를 가져옴
				if(buf3[i] == '_') {
					j = i-1;
					break;
				}
			}
			for(i = 0; i < j; i++) { 
				if(buf3[i] == backup_fname[i])
					flag *= 1;
				else
					flag *= 0;
			}

			if(flag == 1){ //같을 경우에는 num 증가(현재 백업할 파일과 같은 백업파일명을 다 가져와서 갯수세줌
				strcpy(backup_files[num], working_dir);
				strcat(backup_files[num], "/");
				strcat(backup_files[num], "backup_dir/");
				strcat(backup_files[num], buf3);
				num++;
			}
		}
		flag = 1;
	}
	qsort((void *)backup_files, num, sizeof(backup_files[0]), str_compare_sort); //해당 백업파일들을 퀵정렬해줌
	if(num - n_option_num >= 0) { //백업파일들에 수에서 n옵션 숫자를 빼서 for문을 돌려줌
		for(i = 0; i <= num-n_option_num; i++) {
			stat(backup_files[i], &statbuf);
			timer = time(NULL);
			t = localtime(&timer);
			sprintf(backup_time, "[%02d%02d %02d:%02d:%02d]", t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
			sprintf(log_msg, "%s Delete backup [%s, size:%ld, btime:%s]\n", backup_time, fname, statbuf.st_size, timeToString(localtime(&statbuf.st_mtime)));
			fputs(log_msg, fp);
			remove(backup_files[i]); //n옵션 숫자제외 나머지 백업파일 다 지워줌
		}
	}

	fclose(fp);
}
int str_compare_sort(const void *a, const void *b) {
	return (strcmp((char *)a, (char *)b));
}
void not_exist_log(char *src) { //원본 파일이 존재하지 않는경우에 수행하는 함수
	FILE *fp;
	char log_fname[BUFFER_SIZE] = "";
	char log_msg[BUFFER_SIZE];
	char pathname[BUFFER_SIZE] = "";
	char backup_time[BUFFER_SIZE];
	char fname[BUFFER_SIZE];

	strcpy(pathname, src);
	strcpy(fname, get_fname(pathname));
	timer = time(NULL);
	t = localtime(&timer);
	strcat(log_fname, working_dir);
	strcat(log_fname, "/");
	strcat(log_fname, "backup_log.txt");
	sprintf(backup_time, "[%02d%02d %02d:%02d:%02d] ", t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec); //해당 파일이 존재하지 않는다는 로그 문구 생성
	
	fp = fopen(log_fname, "a+");
	sprintf(log_msg, "%s%s is deleted\n", backup_time, fname);
	fputs(log_msg, fp);
	fclose(fp);
}

void daemon_exist(char *fname) { //데몬 프로세스가 이미 돌아가고있는지 확인
	DIR *dir;
	struct dirent *dirp;
	struct stat statbuf;
	int pid, ret_val;
	char pname[MAX_LENGTH], pPath[MAX_LENGTH];
	char org_proc[BUFFER_SIZE];

	strcpy(org_proc, fname); 
	dir = opendir("/proc");
	while((dirp = readdir(dir)) != NULL) { // '/proc'디렉토리 내에 돌아가고있는 실행 파일명들 다 가져옴
		stat(dirp -> d_name, &statbuf);

		if(!S_ISDIR(statbuf.st_mode))
			continue;
		pid = atoi(dirp -> d_name);
		if(pid <= 0)
			continue;

		sprintf(pPath, "/proc/%d/cmdline", pid);  
		ret_val = getProcName(pPath, pname);
		if(ret_val == -1)
			continue;
		else {
			if(!strcmp(pname, fname) && (pid != getpid())) { //파일명 같은것을 찾았는데 해당 pid가 다를경우
				if(flag_c || flag_r) {
					printf("Send signal to ssu_backup process<%d>\n", pid); //해당 프로세스에 SIGUSR1보냄
					kill(pid, SIGUSR1);
				}
				else {
					kill(pid, SIGUSR1);
				}
				break;
			}
		}
	}

}
static void daemon_kill(int signo) { //SIGUSR1발생하였을 시 수행해주는 핸들러
	FILE *fp;
	char log_fname[BUFFER_SIZE] = "";
	char kill_msg[BUFFER_SIZE];
	pid_t pid;
	
	pid = getpid();
	timer = time(NULL);
	t = localtime(&timer);
	strcat(log_fname, working_dir);
	strcat(log_fname, "/");
	strcat(log_fname, "backup_log.txt");

	fp = fopen(log_fname, "a+");
	sprintf(kill_msg, "[%02d%02d %02d:%02d:%02d] ssu_backup<pid:%d> exit\n", t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, pid); //로그문구 생성
	fputs(kill_msg, fp);
	fclose(fp);
	exit(1);
}
static void sys(int signo) { //SIGSEGV랑 SIGKILL발생시 시그널 핸들러
	openlog("./ssu_backup", LOG_PID, LOG_LPR);
	syslog(LOG_ERR, "open failed ./ssu_backup %m");
	closelog();
	exit(0);
}
int getProcName(char *pPath, char *pname) { //원래 백그라운드로 돌아가고있던 프로세스의 이름을 가져옴
	FILE *Pfp;

	Pfp = fopen(pPath, "r");
	if(Pfp == NULL) {
		fclose(Pfp);
		return -1;
	}
	else {
		memset(pname, 0, sizeof(pname));
		fgets(pname, 256, Pfp);
		fclose(Pfp);
		return 0;
	}
}
char *get_fname(char *pathname) { //전달받은 경로포함 파일명에서 파일명만 추출해주는 함수
	int i, j = 0, k = 0;
	char buf[BUFFER_SIZE];
	char *str = buf;

	for(i = strlen(pathname); i >= 0; i--) { //경로포함 파일명에 대해 뒤에서부터 for문 돌림
		j = i;
		if(pathname[i] == '/')  //'/'를 만났을 경우 for문 탈출
			break;
	}
	if(j == 0)
		;
	else
		j++;
	for(i = j; i < strlen(pathname); i++) { //'/'를 만난 위치부터 경로포함 파일명 끝까지 for문 돌림 곧 파일명 추출
		buf[k] = pathname[i];
		k++;
		buf[k] = '\0';
	}
	return str;
}
void backup_log(char *src) { //백업수행 후 로그파일 작성하는 함수
	FILE *fp;
	struct stat statbuf;
	char log_fname[BUFFER_SIZE] = "";
	char log_msg[BUFFER_SIZE];
	char backup_time[BUFFER_SIZE];
	char pathname[BUFFER_SIZE];
	char fname[BUFFER_SIZE];
	time_t rec_mtime;
	int handle, i;
	
	strcpy(pathname, src); //전달받은 src를 pathname에 저장
	strcpy(fname, get_fname(pathname)); //해당 파일의 경로제외 파일명만 가져오는 함수 호출하여 파일명 추출
	timer = time(NULL); //현재 시간 측정
	t = localtime(&timer);
	strcat(log_fname, working_dir); //백업로그파일의 경로 구함
	strcat(log_fname, "/");
	strcat(log_fname, "backup_log.txt");
	sprintf(backup_time, "[%02d%02d %02d:%02d:%02d] ", t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec); //백업 로그 시간 측정 후 저장
	fp = fopen(log_fname, "a+");

	if((stat(src, &statbuf)) < 0) { //해당 파일이 존재하지 않을경우
		handle = 1;
		sprintf(log_msg, "%s%s is deleted\n", backup_time, fname);
	}
	else { //해당 파일이 존재할 경우 
		rec_mtime = statbuf.st_mtime; //mtime비교하기 위해 파일의 mtime을 구한다
		handle = 0;
	}

	if(!handle) {
		if(org_mtime != rec_mtime) {//전역변수로 선언된 org_mtime(main에서 백업할 파일명 입력받았을 시에 측정한 mtime)과 데몬 프로세스 수행중 측정한 파일의 mtime을 비교후 다를시
			sprintf(log_msg, "%s%s is modified [size:%ld/mtime:%s]\n", backup_time, fname, statbuf.st_size, timeToString(localtime(&statbuf.st_mtime)));
			org_mtime = rec_mtime;
		}
		else //같을시
			sprintf(log_msg, "%s%s [size:%ld/mtime:%s]\n", backup_time, fname, statbuf.st_size, timeToString(localtime(&statbuf.st_mtime)));
		fputs(log_msg, fp);
		fclose(fp);
	}
	else { //파일이 지워진 경우
		fputs(log_msg, fp);
		fclose(fp);
	}
}

char *timeToString(struct tm *t) { //파일의 mtime을 구하는 함수1
	static char s[50];

	sprintf(s, "%02d%02d  %02d:%02d:%02d", t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	return s;
}
char *timeToString2(struct tm *t) { //파일의 mtime을 구하는 함수2
	static char s[50];
	
	sprintf(s, "%02d%02d%02d%02d%02d", t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	return s;
}
void ssu_cp(char *src) { //백업을 수행하는 함수
	FILE *fp1, *fp2;
	int ch, i;
	char buf[BUFFER_SIZE];
	char buf2[BUFFER_SIZE];
	char buf3[BUFFER_SIZE];
	char backup_dir[BUFFER_SIZE];
	char backup_fname[BUFFER_SIZE] = "";
	char backup_time[BUFFER_SIZE];
	char trg[BUFFER_SIZE] = "";
	
	strcpy(backup_dir, working_dir);
	strcat(backup_dir, "/backup_dir");
	mkdir(backup_dir, 0777);
	timer = time(NULL); //현재 시간을 측정한다
	t = localtime(&timer);
	strcpy(buf, src);

	for(i = 0; i < strlen(buf); i++) { //백업 파일명 생성
		sprintf(buf3, "%x", buf[i]);
		strcat(backup_fname, buf3);
	}
	
	sprintf(backup_time, "%02d%02d%02d%02d%02d", t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec); //측정한 백업시간 구해놓음
	strcat(backup_fname, "_");
	strcat(backup_fname, backup_time);

	if(strlen(backup_fname) > MAX_LENGTH) { //백업파일의 이름이 255이상일 경우 syslog발생
		openlog("./ssu_backup", LOG_PID, LOG_LPR);
		syslog(LOG_ERR, "open failed ./ssu_backup %m");
		closelog();
		exit(1);
	}
	strcat(trg, working_dir); //백업 디렉토리 경로와 백업파일명 합침
	strcat(trg, "/backup_dir/");
	strcat(trg, backup_fname);
	
	fp1 = fopen(src, "r");
	fp2 = fopen(trg, "w+"); //합쳐진 총 백업파일 절대경로를 fp2로 fopen해줌

	if(fp1 == NULL | fp2 == NULL) { //원본파일이 존재하지 않을경우
	}
	else {
		while((ch = fgetc(fp1)) != EOF) //백업 수행
			fputc(ch, fp2);
	}
	fclose(fp1); //원본 파일 닫아줌
	fclose(fp2); //백업 파일 닫아줌
}
int ssu_daemon_unit(void) { //디몬 프로세스 생성 함수 
	pid_t pid;
	int fd, maxfd;

	if((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid != 0) //고아 프로세스 생성
		exit(0);
	
	pid = getpid();
	printf("process %d running as daemon\n", pid);
	umask(0);
	setsid();
	maxfd = getdtablesize();

	for(fd = 0; fd < maxfd; fd++) //부모 프로세스의 모든 파일 디스크립터 닫음
		close(fd);

	signal(SIGTTIN, SIG_IGN); //표준 입출력, 에러에 대해 시그널 무시
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);

	fd = open("/dev/null", O_RDWR); //표준 입출력, 에러 파일 디스크립터를 /dev/null로 설정
	dup(0);
	dup(0);
	
	return 0;
}
