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

struct timeval bgn, end;
void d_cp_dir(char *, char *);
void cp_dir(char *, char *);
void opt_i_n(void);
void opt_l(char *source, char *target);
void stat_cp(char *source, char *target);
void print_error(void);
void check_target_path(char *str1, char *str2);
void cp_file(char *source, char *target);
void reverse_str(char *str);
char *getpath_name(char *str1, char *str2);
char *timeToString(struct tm *t);
int change_dir_check(char *str1, char *str2, int flag_r, int flag_d);
char source_name[PATH_MAX], target_name[PATH_MAX], resolved_path[PATH_MAX], symlink_path[PATH_MAX];
int path_exist;
int flag_s = 0, flag_i = 0, flag_l = 0, flag_n = 0, flag_p = 0, flag_r = 0, flag_d = 0;
int d_option_num = 0;
char source[PATH_MAX], target[PATH_MAX];
char *source_path, *target_path;
int dir_flag = 0;
pid_t pid2;

int main(int argc, char *argv[])
{
	gettimeofday(&bgn, NULL);
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

	while((param_opt = getopt(argc, argv, "SsIiLlNnPpRrD:d:")) != -1) {
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
	if(opt_cnt != 1) {
		printf("Option must be used just one time\n");
		exit(1);
	}
	
	for(; optind < argc; optind++) {
		strcpy(opt[i], argv[optind]);
		i++;
	}

	if(flag_i && flag_n) {
		fprintf(stderr, "<-i> and <-n> option is duplicated\n");
		print_error();
		exit(1);
	}

	if(flag_r && flag_d) {
		fprintf(stderr, "<-r> and <-d> option is duplicated\n");
		print_error();
		exit(1);
	}

	if(flag_s) {
		if((flag_i + flag_l + flag_n + flag_p + flag_r + flag_d) != 0) {
			fprintf(stderr, "option error!\nUsage : cp [-s][source][target]\n");
			print_error();
			exit(1);
		}
	}

	realpath(argv[0], resolved_path);
	source_path = opt[0];
	source_path = getpath_name(source_path, source_name); //get source_path, reversed source_name
	reverse_str(source_name);
	target_path = opt[1];
	target_path = getpath_name(target_path, target_name); //get target_path, reversed target_name
	reverse_str(target_name);
	
	if(target_path[0] == '\0') {
			if(getcwd(pathname, PATH_MAX) == NULL) {
				;
			}
			else {
				strcat(target,pathname);
				strcat(target,slash);
				strcat(target,target_name);
			}
	}
	else {
		strcpy(target, target_path);
		strcat(target, target_name); //combine target_path & target_name
	}
	if(source_path[0] == '\0'){
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
	if(strcmp(source, target) == 0) {
		fprintf(stderr, "source & target is same!\n");
		exit(1);
	}
	checksum = change_dir_check(source_path, source_name, flag_r, flag_d); //check it is file or symlink_file & source path is exist
	check_target_path(target_path, target_name); //check target_path is exist
	if(flag_p) {
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

	if(flag_s) {
		if(symlink(source_name, target) < 0) {
			remove(target);
			symlink(source_name, target);
		}
	}

	if(flag_r) {
		if(flag_i || flag_n) {
			opt_i_n();
		}
		cp_dir(source, target);//call directory cp method & flag_l if flag_l is on
		if(flag_l) {
			opt_l(source, target);
		}
	}
	if(flag_d) {
		if(flag_i || flag_n) {
			opt_i_n();
		}
		d_cp_dir(source, target);
		if(flag_l) {
			opt_l(source,target);
		}
	}

	if(flag_i || flag_n) {
		if(flag_r == 0 && flag_d == 0)
			opt_i_n();
	}



	if(flag_n == 0 && flag_r == 0 && flag_d == 0 && flag_s == 0) { //do this when source and target is file
			cp_file(source, target);
			if(flag_l) {
				opt_l(source, target);
			}
	}

	if(flag_i == 0 && flag_n == 0) {
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
	gettimeofday(&end, NULL);
	printf("elapsed time : %ld us\n", (end.tv_sec*1000000 + end.tv_usec) - (bgn.tv_sec*1000000 + bgn.tv_usec));
	exit(0);
}

void cp_dir(char *source, char *target)
{
	DIR *dirp;
	DIR *dirp1;
	struct dirent *dentry;
	struct stat statbuf;
	char filename[PATH_MAX];
	char s_path[PATH_MAX], t_path[PATH_MAX];
	char slash[1] = "/";
	int s_len = strlen(source_path), t_len = strlen(target_path);
	
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
			cp_dir(s_path, t_path);
		}

	}
	return ;
}
void d_cp_dir(char *source, char *target)
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
			if(pid == 0 && dir_flag == 0) {
				dir_flag = 1;
				printf("process num : %d directory name : %s\n", getpid(), t_path);
				cp_dir(s_path, t_path);
				dir_flag = 0;
				exit(1);
			}
		}

	}
	if(pid2 == getpid()) {
		for(i = 0; i < d_option_num; i++){
			pid = fork();
			if(pid == 0) {
				exit(1);
			}
		}
	}
	return ;
}

void opt_i_n(void)
{
	char buf1[PATH_MAX], c;
	if(flag_i) {
		if(target_path[0] != '\0') {
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
		else {	
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

	if(flag_n) {
		if(target_path[0] != '\0') {
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
		else {	
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

void opt_l(char *source, char *target)
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

void cp_file(char *source, char *target) //file cp method
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
		if(S_ISDIR(statbuf.st_mode)) { //if target name is already in that directory but it is directory
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

void check_target_path(char *str1, char *str2)
{
	if(str1[0] != '\0') { //check the target path that is exist
		if(opendir(str1) < 0) {
			fprintf(stderr, "No Such target_path : %s\n", str1);
			print_error();
			exit(1);
		}
	}
	else
		return ;
}

int change_dir_check(char *str1, char *str2, int flag_r, int flag_d)
{
	struct stat info;
	char buf[PATH_MAX];
	char s_path[PATH_MAX];
	char s_name[PATH_MAX];
	strcpy(s_path, str1);
	strcpy(s_name, str2);
	char *test = strcat(s_path, s_name);
	
	if(str1[0] != '\0') { //ther is path to cp
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
				if(S_ISDIR(info.st_mode)) {
					if(flag_r || flag_d) { //option r or d is on
					//	printf("%s%s is directory!\n", str1, str2);
					//	printf("%s\n", target_name);
					//	exit(0);
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
	else { //ther is no path to cp
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
				if(flag_r || flag_d) { //option r or d  is on
				//	printf("%s%s is directory!\n", str1, str2);
				//	printf("%s\n", target_name);
				//	exit(0);
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

char *getpath_name(char *str1, char *str2)
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

void reverse_str(char *str)
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

void print_error(void)
{
	printf("ssu_cp error\n");
	printf("usage : in case of file\n");
	printf(" cp [-i/n][-l][-p]	[source][target]\n");
	printf(" or cp [-s][source][target]\n");
	printf(" in case of directory cp [-i/n][-l][-p][-r][-d][N]\n");
	return ;
}

char *timeToString(struct tm *t)
{
	static char s[20];
	
	sprintf(s, "%04d.%02d.%02d %02d:%02d:%02d", t->tm_year + 1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	return s;
}
