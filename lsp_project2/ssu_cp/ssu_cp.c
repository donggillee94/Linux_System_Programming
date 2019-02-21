#include <sys/stat.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <memory.h>
#include <utime.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>

#define S_MODE 0766
#define BUFFER_SIZE 1024

struct timeval bgn, end;//프로그램 실행시간을 출력하기 위해 사용되는 timeval구조체
void d_cp_dir(char *, char *);//d옵션 사용시 호출되는 함수로 내부에 fork와 cp_dir함수를 호출하며 재귀적으로 실행된다, 매개변수로는 source와 target의 경로+디렉토리명이 들어온다
void cp_dir(char *, char *);//s옵션 사용시 호출되는 함수로 재귀적으로 실행되며 매개변수는 d_cp_dir과 같다
void opt_i_n(void);//i/n옵션사용시에 호출되는 함수 target에 대해서 access호출하여 존재여부 확인후 덮어쓰기할지 안할지 결정한다
void opt_l(char *source, char *target);//l옵션사용시에 호출되는 함수, 매개변수로는 source와 target의 경로+디렉토리명이 들어가며 stat구조체를 이용해서 source의 정보를 가져와 target에 정보 조작해줌
void print_error(void);//error발생 시 ssu_cp프로그램의 사용법을 출력해주는 함수이다
void check_target_path(char *str1, char *str2);//target의 경로가 실존하는지 확인해주는 함수이다
void cp_file(char *source, char *target);//파일 복사시 호출되는 함수로 source와 target에 대해 각각 파일디스크립터 이용하여 open후 파일 복사가 수행된다
void reverse_str(char *str);//실행시 source및 target입력할때 경로가 포함된 내용이면 경로와 파일 및 디렉토리명이 밑의 getpath_name에서 분리가되는데 문자열을 뒤에서부터 탐색하기에 거꾸로 들어가는 파일 및 디렉토리명에 대해서 reverse해주는 함수이다
char *getpath_name(char *str1, char *str2);//위의 reverse함수에서 설명했듯이 경로와 파일 및 디렉토리명을 분리해주는 함수이다
char *timeToString(struct tm *t);//p옵션 사용시에 호출되는 함수로 source의 atime,ctime,mtime을 문자열로 번역해주는 함수이다
int change_dir_check(char *str1, char *str2, int flag_r, int flag_d);//source의 경로를 파악하는 함수로서 매개변수로는 source_path, source_name, flag_r,flag_d가 들어간다 해당 옵션들이 켜져있지 않는데 파일명[source]가 디렉토리로 들어올 경우 오류출력 위함이다
char source_name[PATH_MAX], target_name[PATH_MAX], resolved_path[PATH_MAX], symlink_path[PATH_MAX];//source의 이름과 경로명, target의 이름과 경로명, symlink파일에 대한 경로 및 그거에 대한 원본 파일path(resolved_path)
int path_exist;
int flag_s = 0, flag_i = 0, flag_l = 0, flag_n = 0, flag_p = 0, flag_r = 0, flag_d = 0;//각 플래그들
int d_option_num = 0;//d옵션 사용시에 뒤에오는 숫자 [N]
char source[PATH_MAX], target[PATH_MAX];//source의 경로명과 이름, target의 경로명과 이름이 각각 합쳐진 변수들
char *source_path, *target_path;
int dir_flag = 0;//d옵션 사용시에 해당 하위디렉토리의 개수를 알기위한 변수
pid_t pid2;//d옵션 사용시 부모 프로세스의 pid를 받아주는

int main(int argc, char *argv[])
{
	gettimeofday(&bgn, NULL);//main함수 시작시에 시간 측정
	struct stat stat_buf;
	int checksum = 0, i = 0, param_opt;
	char opt[2][PATH_MAX];
	char slash[1] = "/";
	char *pathname = malloc(PATH_MAX);
	int opt_cnt = 1;
	pid2 = getpid();

	if(argc < 3) {
		print_error();
		exit(1);
	}

	while((param_opt = getopt(argc, argv, "SsIiLlNnPpRrD:d:")) != -1) {//option 처리 부분
		switch(param_opt) {
			case 's':
			case 'S':
				flag_s++;
				opt_cnt *= flag_s;
				printf(" s option is on\n");
				break;
			case 'i':
			case 'I':
				flag_i++;
				opt_cnt *= flag_i;
				printf(" i option is on\n");
				break;
			case 'l':
			case 'L':
				flag_l++;
				opt_cnt *= flag_l;
				printf(" l option is on\n");
				break;
			case 'n':
			case 'N':
				flag_n++;
				opt_cnt *= flag_n;
				printf(" n option is on\n");
				break;
			case 'p':
			case 'P':
				flag_p++;
				opt_cnt *= flag_p;
				printf(" p option is on\n");
				break;
			case 'r':
			case 'R':
				flag_r++;
				opt_cnt *= flag_r;
				printf(" r option is on\n");
				break;
			case 'd':
			case 'D':
				flag_d++;
				char *sub = optarg;
				d_option_num = atoi(sub);
				if(d_option_num <= 1 || d_option_num >= 10) {
					fprintf(stderr, "-d option is error\nUsage : -d/-D[N](1~10)\n");
					exit(1);
				}
				opt_cnt *= flag_d;
				printf(" d option is on\n");
				break;
		}
	}
	if(opt_cnt != 1) {//같은 옵션 중복사용시 에러 처리
		printf("Option must be used just one time\n");
		exit(1);
	}
	
	for(; optind < argc; optind++) {//옵션 말고는 source와 target일 것이므로 그 두개를 opt배열에다가 넣어준다
		strcpy(opt[i], argv[optind]);
		i++;
	}

	if(flag_i && flag_n) {//i와n옵션 동시사용시 에러 처리
		fprintf(stderr, "<-i> and <-n> option is duplicated\n");
		print_error();
		exit(1);
	}

	if(flag_r && flag_d) {//r과d옵션 동시사용시 에러 처리
		fprintf(stderr, "<-r> and <-d> option is duplicated\n");
		print_error();
		exit(1);
	}

	if(flag_s) {//s옵션 사용시에 다른 옵션들도 사용되면 에러 처리
		if((flag_i + flag_l + flag_n + flag_p + flag_r + flag_d) != 0) {
			fprintf(stderr, "option error!\nUsage : cp [-s][source][target]\n");
			print_error();
			exit(1);
		}
	}

	source_path = opt[0];
	source_path = getpath_name(source_path, source_name); //source_path를 가져오며 source_name은 reverse처리해준다
	reverse_str(source_name);
	target_path = opt[1];
	target_path = getpath_name(target_path, target_name); //target_path를 가져오며 target_name은 reverse처리해준다
	reverse_str(target_name);
	
	if(target_path[0] == '\0') {//target_path가 없을경우엔 지금 디렉토리의 path명을 붙여준다
			if(getcwd(pathname, PATH_MAX) == NULL) {
				;
			}
			else {
				strcat(target,pathname);
				strcat(target,slash);
				strcat(target,target_name);
			}
	}
	else {//있을경우에는 path와 name을 합쳐서 target에 넣어줌
		strcpy(target, target_path);
		strcat(target, target_name);
	}
	if(source_path[0] == '\0'){//target_path와 같은방법으로 source에 대해서도 동일하게 해준다
		if(getcwd(pathname, PATH_MAX) == NULL) {
			;
		}
		else {
			strcat(source, pathname);
			strcat(source, slash);
			strcat(source, source_name);
		}
	}
	else {
		strcpy(source, source_path);
		strcat(source, source_name);
	}
	if(strcmp(source, target) == 0) {//source와 target의 파일이나 디렉토리가 같을경우에 오류 처리
		fprintf(stderr, "source & target is same!\n");
		exit(1);
	}

	checksum = change_dir_check(source_path, source_name, flag_r, flag_d); //source가 심볼릭 파일인지 그냥 파일인지 디렉토리인지 확인
	check_target_path(target_path, target_name); //target에 대해서 경로 확인해줌 없으면 오류 처리

	if(flag_p) {//p옵션 구현부분
		struct stat statbuf;
		struct passwd *my_passwd;
		struct group *my_group;

		if(stat(source, &statbuf) < 0) {
			fprintf(stderr, "stat error for option <p>\n");
			exit(1);
		}

		my_passwd = getpwuid(statbuf.st_uid);
		my_group = getgrgid(statbuf.st_gid);
		printf("*************************file info************************\n");
		printf("file name : %s\n", source_name);
		printf("file's atime : %s\n", timeToString(localtime(&statbuf.st_atime)));
		printf("file's mtime : %s\n", timeToString(localtime(&statbuf.st_mtime)));
		printf("file's ctime : %s\n", timeToString(localtime(&statbuf.st_ctime)));
		printf("OWNER : %s\n", my_passwd->pw_name);
		printf("GROUP : %s\n", my_group->gr_name);
		printf("file size : %ld\n", statbuf.st_size);
		printf("**********************************************************\n");
	}

	if(flag_s) {//s옵션 구현부분
		if(symlink(source_name, target) < 0) {
			remove(target);
			symlink(source_name, target);
		}
	}

	if(flag_r) {//r옵션 구현부분
		if(flag_i || flag_n) {
			opt_i_n();
		}
		cp_dir(source, target);//call directory cp method & flag_l if flag_l is on
		if(flag_l) {
			opt_l(source, target);
		}
	}
	if(flag_d) {//d옵션 구현부분
		if(flag_i || flag_n) {
			opt_i_n();
		}
		d_cp_dir(source, target);
		if(flag_l) {
			opt_l(source,target);
		}
	}

	if(flag_i || flag_n) {//i와n옵션 사용시 호출되는 함수 opt_i_n
		if(flag_r == 0 && flag_d == 0)
			opt_i_n();
	}



	if(flag_n == 0 && flag_r == 0 && flag_d == 0 && flag_s == 0) { //source와 target이 파일일경우에 해주며 l옵션이 켜져있을 시 opt_l함수 호출해주면서 l옵션까지 처리가능하다
			cp_file(source, target);
			if(flag_l) {
				opt_l(source, target);
			}
	}

	if(flag_i == 0 && flag_n == 0) {//src와 target출력 문구이며 i와 n옵션 구현함수내에서 똑같이 출력해주기때문에 해당 옵션이 꺼져있을 경우엔 이렇게 출력
		if(target_path[0] != '\0'){
			printf("target : %s\n", target);
		}
		else
			printf("target : %s\n", target_name);
		if(source_path[0] != '\0') {
			printf("src : %s\n", source);
		}
		else
			printf("src : %s\n", source_name);
	}
	gettimeofday(&end, NULL);//main함수 종료시에 시간 측정
	printf("elapsed time : %ld us\n", (end.tv_sec*1000000 + end.tv_usec) - (bgn.tv_sec*1000000 + bgn.tv_usec));
	exit(0);
}

void cp_dir(char *source, char *target)//디렉토리 복사함수 구현부분 s옵션 호출시 이 함수 호출하게 된다
{
	DIR *dirp;
	DIR *dirp1;
	struct dirent *dentry;
	struct stat statbuf;
	char filename[PATH_MAX];
	char s_path[PATH_MAX], t_path[PATH_MAX];
	char slash[1] = "/";
	int s_len = strlen(source_path), t_len = strlen(target_path);
	
	if((dirp = opendir(source)) == NULL) {//source가 디렉토리인지 확인
		fprintf(stderr, "<%s> is not directory\n", source);
		exit(1);
	}

	if(mkdir(target, 0777) == -1) {//target에 대해서 mkdir해줌
		if((dirp1 = opendir(target)) == NULL) {
			fprintf(stderr, "mkdir error for %s in cp_dir\n", target);
			exit(1);
		}
	}

	while((dentry = readdir(dirp)) != NULL) {//source디렉토리에 대해서 readdir실행
		if(dentry -> d_ino == 0)
			continue;

		memcpy(filename, dentry -> d_name, PATH_MAX);
		sprintf(s_path,"%s/%s", source,filename);
		if(stat(s_path, &statbuf) == -1) {//source디렉토리 내부에 존재하는 파일들에 대해서 stat구조체 만들어줌
			fprintf(stderr, "stat error for %s to directory cp\n", filename);
		}

		if((statbuf.st_mode & S_IFMT) == S_IFREG) {//일반파일인지 확인
			sprintf(t_path, "%s/%s", target,filename);
			cp_file(s_path, t_path);
		}
		else if(strcmp(dentry -> d_name, ".") && strcmp(dentry -> d_name, "..")) {//자기자신과 부모 디렉토리 제외하고 하위 디렉토리 발견했을 경우에는 재귀호출
			sprintf(s_path, "%s/%s", source,filename);
			sprintf(t_path, "%s/%s", target,filename);
			cp_dir(s_path, t_path);
		}

	}
	return ;
}
void d_cp_dir(char *source, char *target)//위의 r옵션 함수 구현과 비슷한 메커니즘이지만 fork와 getpid를 통해서 자식 프로세스 생성 후 복사 속도 향상
{
	DIR *dirp;
	DIR *dirp1;
	struct dirent *dentry;
	struct stat statbuf;
	char filename[PATH_MAX];
	char s_path[PATH_MAX], t_path[PATH_MAX];
	char slash[1] = "/";
	int s_len = strlen(source_path), t_len = strlen(target_path);
	int i;
	pid_t pid;

	if((dirp = opendir(source)) == NULL) {
		fprintf(stderr, "<%s> is not directory\n", source);
		exit(1);
	}

	if(mkdir(target, 0777) == -1) {
		if((dirp1 = opendir(target)) == NULL) {
			fprintf(stderr, "mkdir error for %s in cp_dir\n", target);
			exit(1);
		}
	}

	while((dentry = readdir(dirp)) != NULL) {
		if(dentry -> d_ino == 0)
			continue;

		memcpy(filename, dentry -> d_name, PATH_MAX);
		sprintf(s_path,"%s/%s", source,filename);
		if(stat(s_path, &statbuf) == -1) {
			fprintf(stderr, "stat error for %s to directory cp\n", filename);
		}

		if((statbuf.st_mode & S_IFMT) == S_IFREG) {
			sprintf(t_path, "%s/%s", target,filename);
			cp_file(s_path, t_path);
		}
		else if(strcmp(dentry -> d_name, ".") && strcmp(dentry -> d_name, "..")) {
			sprintf(s_path, "%s/%s", source,filename);
			sprintf(t_path, "%s/%s", target,filename);
			
			if(d_option_num) {
				d_option_num--;
				pid = fork();
			}
			if(pid == 0 && dir_flag == 0) {//부모 프로세스일경우와 하위 디렉토리 갯수가 0개일 경우
				dir_flag = 1;
				printf("process num : %d directory name : %s\n", getpid(), t_path);
				cp_dir(s_path, t_path);
				dir_flag = 0;
				exit(1);
			}
		}

	}
	if(pid2 == getpid()) {//돌아가지 않는 자식 프로세스들에 대해서 돌려줌
		for(i = 0; i < d_option_num; i++){
			pid = fork();
			if(pid == 0) {
				exit(1);
			}
		}
	}
	return ;
}

void opt_i_n(void)//i옵션과 n옵션 구현부분
{
	char buf1[PATH_MAX], c;
	if(flag_i) {
		if(target_path[0] != '\0') {//target_path가 있을경우
			if(opendir(target) < 0) {
				fprintf(stderr, "No Such path to opendir <%s> in option 'i'\n", buf1);
				print_error();
				exit(1);
			}
			if(access(target, F_OK) == 0) {	
				if(target_path[0] != '\0'){
					printf("target : %s\n", target);
				}
				else
					printf("target : %s\n", target_name);
				if(source_path[0] != '\0') {
					printf("src : %s\n", source);
					printf("%s\n", source);
				}
				else {
					printf("src : %s\n", source_name);
					printf("%s\n", source_name);
				}
				printf("overwrite %s (y/n)? ", target);
				while(1)
				{
					scanf("%c", &c);
					if(c == 'y')
						break;
				}
			}
			else
				flag_i = 0;
		}
		else {//target_path가 없을경우
			if(opendir(target_name) < 0) {
				fprintf(stderr, "No Such path to opendir <%s> in option 'i'\n", buf1);
				print_error();
				exit(1);
			}
			if(access(target_name, F_OK) == 0) {
				if(target_path[0] != '\0'){
					printf("target : %s\n", target);
				}
				else
					printf("target : %s\n", target_name);
				if(source_path[0] != '\0') {
					printf("src : %s\n", source);
					printf("%s\n", source);
				}
				else {
					printf("src : %s\n", source_name);
					printf("%s\n", source_name);
				}
				printf("overwrite %s (y/n)? ", target_name);
				while(1)
				{
					scanf("%c", &c);
					if(c == 'y')
						break;
				}
			}
			else
				flag_i = 0;
		}
	
	}

	if(flag_n) {//n옵션
		if(target_path[0] != '\0') {//target_path가 있을경우
			if(opendir(target) < 0) {
				fprintf(stderr, "No Such path to opendir <%s> in option 'n'\n", buf1);
				print_error();
				exit(1);
			}		
			if(access(target, F_OK) == 0) {
				if(target_path[0] != '\0'){
					printf("target : %s\n", target);
				}
				else
					printf("target : %s\n", target_name);
				if(source_path[0] != '\0') {
					printf("src : %s\n", source);
					printf("%s\n", source);
				}
				else {
					printf("src : %s\n", source_name);
					printf("%s\n", source_name);
				}
				printf("overwrite %s (y/n)? ", target);
				while(1)
				{
					scanf("%c", &c);
					if(c == 'n'){
						break;
					}
				}
			}
			else
				flag_n = 0;
		}
		else {//target_path가 없을경우
			if(opendir(target_name) < 0) {
				fprintf(stderr, "No Such path to opendir <%s> in option 'n'\n", buf1);
				print_error();
				exit(1);
			}
			if(access(target_name, F_OK) == 0) {
				if(target_path[0] != '\0'){
					printf("target : %s\n", target);
				}
				else
					printf("target : %s\n", target_name);
				if(source_path[0] != '\0') {
					printf("src : %s\n", source);
					printf("%s\n", source);
				}
				else {
					printf("src : %s\n", source_name);
					printf("%s\n", source_name);
				}			
				printf("overwrite %s (y/n)? ", target_name);
				while(1)
				{
					scanf("%c", &c);
					if(c == 'n'){
						break;
					}
				}
			}
			else
				flag_n = 0;
		}
	}
}

void opt_l(char *source, char *target)//l옵션 구현부분
{
	struct utimbuf time_buf;
	struct stat source_info;
	
	if(lstat(source, &source_info) < 0) {
		fprintf(stderr, "stat error for %s in option 'l'\n", source);
		exit(1);
	}
	chown(target, source_info.st_uid, source_info.st_gid);
	chmod(target, source_info.st_mode);
	time_buf.actime = source_info.st_atime;
	time_buf.modtime = source_info.st_mtime;

	if(utime(target, &time_buf) < 0) {
		fprintf(stderr, "utime error for %s in option 'l'\n", target_name);
		exit(1);
	}
}

void cp_file(char *source, char *target) //파일에 대해서 복사해주는 함수이다
{
	char buf[BUFFER_SIZE], buf2[PATH_MAX];
	char source_buf[PATH_MAX], target_buf[PATH_MAX];
	int fd1, fd2;
	int length, i, slash_exist = 0;
	
	for(i = strlen(resolved_path); i >= 0; i--) {
		if(resolved_path[i] == '/'){
			resolved_path[i+1] = '\0';
			break;
		}
	}

	strcpy(buf2, resolved_path);
	strcpy(source_buf, source);
	strcpy(target_buf, target);

	for(i = 0; i < strlen(target); i++) {
		if(target[i] == '/')
			slash_exist++;
	}

	if(slash_exist == 0) {
		target = strcat(buf2, target);
		strcpy(target_buf, target);
		strcpy(buf2, resolved_path);
	}

	slash_exist = 0;

	for(i = 0; i < strlen(source); i++) {
		if(source[i] == '/')
			slash_exist++;
	}
	
	if(slash_exist == 0) {
		source = strcat(buf2, source);
		strcpy(source_buf, source);
		strcpy(buf2, resolved_path);
		slash_exist = 0;
	}

	if((fd1 = open(source_buf, O_RDONLY)) < 0) {
		fprintf(stderr, "No Such file :  %s\n", source);
		print_error();
		exit(1);
	}
	if((fd2 = open(target_buf, O_WRONLY | O_CREAT | O_TRUNC, S_MODE)) < 0) {
		struct stat statbuf;
		stat(target_buf, &statbuf);
		if(S_ISDIR(statbuf.st_mode)) {//해당 target명이 이미 내가 원하는 target쪽에 다른 디렉토리명으로 있을경우에는 오류출력해준다
			fprintf(stderr, "already have same named <%s>directory in target path\n", target_name);
			exit(1);
		}
		fprintf(stderr, "Target path <%s> is strange\n", target);
		print_error();
		exit(1);
	}

	while((length = read(fd1, buf, BUFFER_SIZE)) > 0)
		write(fd2, buf, length);

	return;
}

void check_target_path(char *str1, char *str2)//target_path에 대해서 확인해주는 함수
{
	if(str1[0] != '\0') {
		if(opendir(str1) < 0) {
			fprintf(stderr, "No Such target_path : %s\n", str1);
			print_error();
			exit(1);
		}
	}
	else
		return ;
}

int change_dir_check(char *str1, char *str2, int flag_r, int flag_d)//source에 대해서 경로가 존재하는지 그리고 해당 파일이 일반파일인지 심볼릭파일인지 디렉토리인지 확인하는 함수이다.
{
	struct stat info;
	char buf[PATH_MAX];
	char s_path[PATH_MAX];
	char s_name[PATH_MAX];
	strcpy(s_path, str1);
	strcpy(s_name, str2);
	char *test = strcat(s_path, s_name);
	
	if(str1[0] != '\0') { //source_path가 존재할 경우
		if(opendir(str1) < 0) {
			fprintf(stderr, "No Such source_path : %s\n", str1);
			print_error();
			exit(1);
		}
		else {
			if(lstat(test, &info) < 0) {
			}
			if(access(test, F_OK) < 0) {
				fprintf(stderr, "No Such file or directory : %s\n", test);
				print_error();
				exit(1);
			}
			else {
				if(S_ISREG(info.st_mode))
					return 1;
				if(S_ISLNK(info.st_mode)) 
					return 0;
				if(S_ISDIR(info.st_mode)) {//해당 source가 디렉토리인 경우
					if(flag_r || flag_d) { //r옵션과 d옵션이 켜져있을 경우
					}
					else {
						fprintf(stderr, "Must use <-r> or <-d>[N] Option to copy <%s>directory\n", str2);
						print_error();
						exit(1);
					}
				}
			}
		}
	}
	else { //source_path가 없는경우
		if(lstat(str2, &info) < 0) {
		}
		if(access(str2, F_OK) < 0) {
			fprintf(stderr, "No Such file or directory : %s\n", test);
			print_error();
			exit(1);
		}
		else {
			if(S_ISREG(info.st_mode)) 
				return 1;
			if(S_ISLNK(info.st_mode))
				return 0;
			if(S_ISDIR(info.st_mode)) {
				if(flag_r || flag_d) { //r과 d옵션이 켜져있을 경우
				}
				else {
					fprintf(stderr, "Must use <-r> or <-d>[N] Option to copy <%s>directory\n", str2);
					print_error();
					exit(1);
				}
			}
		}
	}
}

char *getpath_name(char *str1, char *str2)//source와 target에 대해서 경로명과 파일 및 디렉토리명을 분리해주는 함수이다
{
	int i, j = 0;
	
	for(i = strlen(str1); i >= 0; i--) {
		if(str1[i] == '/'){
			path_exist = 1;
			str1[i+1] = '\0';
			break;
		}
		else {
			if(str1[i] != '\0') {
				str2[j] = str1[i];
				j++;
			}
		}
	}
	if(path_exist == 0) {
		str1[0] = '\0';
	}
	path_exist = 0;
	return str1;
}

void reverse_str(char *str)//위의getpath_name함수를 실행하고나면 파일명은 거꾸로 들어가게 되므로 이름을 다시 거꾸로 되돌려주기위한 함수
{
	size_t size = strlen(str);
	char temp;

	for(size_t i = 0; i < size/2 ; i++) {
		temp = str[i];
		str[i] = str[(size-1) - i];
		str[(size-1) - i] = temp;
	}
	return ;
}

void print_error(void)//오류 문구 출력 후 사용법 출력예시이다.
{
	printf("ssu_cp error\n");
	printf("usage : in case of file\n");
	printf(" cp [-i/n][-l][-p]	[source][target]\n");
	printf(" or cp [-s][source][target]\n");
	printf(" in case of directory cp [-i/n][-l][-p][-r][-d][N]\n");
	return ;
}

char *timeToString(struct tm *t)//해당 파일의 atime, ctime, mtime에 대해서 시간정보 받은 후 문자열로 출력해주는 함수
{
	static char s[20];
	
	sprintf(s, "%04d.%02d.%02d %02d:%02d:%02d", t->tm_year + 1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	return s;
}
