
#ifndef __SSU_CP_H__
#define __SSU_CP_H__

#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0
#define ERROR -1
#define OK 1 

#define MAX_NUM 16
#define MAX_SIZE 1024
#define OPT_MAX_NUM 10
#define USE_OPT 5

#define flag_t int

#define RETAINBITS		(S_ISUID|S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO)
#define STRIP_TRAILING_SLASH(p) { \
	while ((p)->p_end > (p)->p_path && (p)->p_end[-1] == '/') \
	*--(p)->p_end = 0;\
}
typedef struct {
	char *p_end;					// pointer to NULL at end of path
	char p_path[PATH_MAX + 1];		// pointer to the start of a path
}PATH_T;

extern char *progname;

int path_set(PATH_T *p, char *string);
char *path_append (PATH_T *p, char *name, int len);
char *path_basename(PATH_T *p);
void path_restore(PATH_T *p, char *old);

int ssu_cp(int argc, char *argv[]);
int copy();
void copy_file(struct stat *fs , int dne);
void copy_dir();
void copy_dir_fork();
void copy_link(int exists);
void copy_tolink();
void copy_fifo(struct stat *from_stat, int exists);
void copy_special(struct stat *from_stat, int exists);
void setfile(struct stat *fs, int fd);
void print_fileinfo(PATH_T *p,struct stat *from_stat);
void init_variable(flag_t flag);// 헤더 파일에 처리할것 
char *formatdate(char *str, time_t val);

int check_flag(char opt_c);
int check_s_onlyopt();
int check_multiplecount(int multiple_count);
int get_opt(int argc, char *argv[]);

int able_access(char *path_name, int flag);
void error(char *s);
void usage();

#endif

/* 있는지 없는지 판단하는 함수의 경우 리턴값 0,1 로 구분*/
/* 인지 아닌지 판단하는 함수의 경우 리턴값 1, -1 로 구분*/

