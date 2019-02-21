#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	char *fname = "ssu_test.txt";
	char character;
	int line_count = 0;
	int fd;

	if((fd = open(fname, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	while(1) {
		if(read(fd, &character, 1) > 0) {
			if(character == '\n')
				line_count++;
		}
		else
			break;
	}
	printf("Total line : %d\n", line_count);

	exit(0);
}
