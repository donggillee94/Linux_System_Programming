#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

int ssu_do_grep(void);
void ssu_make_grep(int, char *argv[]);

static int pathmax = PATH_MAX;

#define MAX_PATH_GUESSED 1024
#define LINE_MAX 2048

char *pathname;
char command[LINE_MAX], grep_cmd[LINE_MAX];

int ssu_do_grep()
{
	struct dirent *dentry;
	struct stat statbuf;
	char *ptr;
	DIR *dirp;

	if(lstat(pathname, &statbuf) < 0) {
		fprintf(stderr, "lstat error for %s\n", pathname);
		exit(1);
	}

	if(S_ISDIR(statbuf.st_mode) == 0) {
		sprintf(command, "%s %s", grep_cmd, pathname);
		printf("%s : \n",pathname);
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

			if(ssu_do_grep() < 0)
				break;
		}
	}
	closedir(dirp);
	return 0;
		
}

void ssu_make_grep(int argc, char *argv[])
{
	int i;
	strcpy(grep_cmd, " grep");
	for(i = 1; i < argc-1; i++){
		strcat(grep_cmd, " ");
		strcat(grep_cmd, argv[i]);
	}
}

int main(int argc, char *argv[])
{
	if(argc < 2) {
		fprintf(stderr, "Usage : %s filename\n", argv[0]);
		exit(1);
	}

	pathname = (char *)malloc(pathmax+1);

	strcpy(pathname, argv[argc-1]);
	ssu_make_grep(argc, argv);
	ssu_do_grep();
	exit(0);
}
