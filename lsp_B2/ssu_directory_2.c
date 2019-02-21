#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ssu_do_grep(void);
void ssu_make_grep(int , char *argv[]);

static int pathmax = PATH_MAX;

#define MAX_PATH_GUESSED 1024
#define LINE_MAX 2048

char *pathname;
char command[LINE_MAX], grep_cmd[LINE_MAX];

int ssu_do_grep()
{
	struct stat statbuf;
	struct dirent *dentry;
	DIR *dirp;
	char *ptr;
	
	if(lstat(pathname, &statbuf) < 0) {
		fprintf(stderr, "lstat error for %s\n", pathname);
		exit(1);
	}

	if(!S_ISDIR(statbuf.st_mode)) {
		sprintf(command, "%s %s", grep_cmd, pathname);
		printf("%s : \n", pathname);
		system(command);
		return 0;
	}

	ptr = pathname + strlen(pathname);
	*ptr++ = '/';
	*ptr = '\0';

	if((dirp = opendir(pathname)) == NULL) {
		fprintf(stderr, "opendir error for %s\n", pathname);
		exit(1);
	}
	
	while((dentry = readdir(dirp)) != NULL) {
		if(strcmp(dentry -> d_name, ".") && strcmp(dentry -> d_name, "..")) {
			strcpy(ptr, dentry -> d_name);

			ssu_do_grep();
		}
	}
}

void ssu_make_grep(int argc, char *argv[])
{
	int i;
	for(i = 1; i < argc-1; i++) {
		strcpy(grep_cmd, "grep ");
		strcat(grep_cmd, argv[i]);
		strcat(grep_cmd, " ");
	}
}
int main(int argc, char *argv[])
{
	if(argc < 2) {
		fprintf(stderr, "usage : %s <-option> <-num> <-A num> <-B num> <-f file> <directory>\n", argv[0]);
		exit(1);
	}
	pathname = (char *)malloc(pathmax+1);
	strcpy(pathname,argv[argc-1]);
	ssu_make_grep(argc, argv);
	ssu_do_grep();
	exit(0);
}
