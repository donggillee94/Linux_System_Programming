#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	struct stat file_info;
	char *str;
	int i;
	
	for(i = 1; i < argc; i++) {

		printf("name = %s, type = ", argv[i]);

		if(lstat(argv[i], &file_info) < 0) {
			fprintf(stderr, "lstat error for %s\n", argv[i]);
			exit(1);
		}

		if(S_ISREG(file_info.st_mode))
			str = "regular";
		else if(S_ISDIR(file_info.st_mode))
			str = "directory";
		else if(S_ISCHR(file_info.st_mode))
			str = "character special";
		else if(S_ISBLK(file_info.st_mode))
			str= "block special";
		else if(S_ISLNK(file_info.st_mode))
			str = "symbolic link";
		else if(S_ISFIFO(file_info.st_mode))
			str = "FIFO";
		else if(S_ISSOCK(file_info.st_mode))
			str = "socket";
		else
			str = "unknown mode";

		printf("%s\n", str);
	}
	exit(0);
}
