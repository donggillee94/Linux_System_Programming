#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

char *pathname;

void search_directory(char *dir)
{
	struct dirent *dentry;
	struct stat statbuf;
	DIR *dirp;
	char cwd[1024];
	
	getcwd(cwd, 1024);
	
	if(lstat(dir, &statbuf) < 0) {
		fprintf(stderr, "lstat error\n");
		exit(1);
	}
	
	if((dirp = opendir(dir)) == NULL) {
		fprintf(stderr, "readdir error\n");
		exit(1);
	}
	chdir(dir);

	while((dentry = readdir(dirp)) != NULL) {
		if(!(strcmp(dentry -> d_name, ".") && strcmp(dentry -> d_name, "..")))
			continue;
		if(lstat(dentry -> d_name, &statbuf) < 0) {
			fprintf(stderr, "lstat error for %s\n", dentry -> d_name);
			exit(1);
		}
		if(!S_ISLNK(statbuf.st_mode)) {
			if(!S_ISDIR(statbuf.st_mode)) {
				printf("%s/%s\n",cwd, dentry -> d_name);
			}
			if(S_ISDIR(statbuf.st_mode)) {
				printf("%s/%s\n", cwd, dentry -> d_name);
				search_directory(dentry -> d_name);
			}
		}
	}
	chdir(dir);
	chdir(cwd);
}
int main(int argc, char *argv[])
{
	if(argc < 2) {
		fprintf(stderr, "usage : %s filename\n", argv[0]);
		exit(1);
	}
	search_directory(argv[1]);
	exit(0);
}
