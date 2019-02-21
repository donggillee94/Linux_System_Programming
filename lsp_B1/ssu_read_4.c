#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024
#define TABLE_SIZE 128

int main(int argc, char *argv[])
{
	static struct{
		long offset;
		int length;
	}table[TABLE_SIZE];
	char buf[BUFFER_SIZE];
	int entry;
	int i;
	int length;
	int fd;
	long offset;

	if(argc < 2) {
		fprintf(stderr, "Usage : %s file\n", argv[0]);
		exit(1);
	}

	if((fd = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}
	
	entry = 0;
	offset = 0;

	while((length = read(fd, &buf, BUFFER_SIZE)) > 0) {
		for(i = 0; i < length; i++) {
			table[entry].length++;
			offset++;

			if(buf[i] == '\n') {
				entry++;
				table[entry].offset = offset;
			}

		}
	}

	while(1) {
		printf("Enter line number : ");
		scanf("%d", &length);

		if(length == -1)
			break;

		lseek(fd, table[length].offset, SEEK_SET);
		read(fd, buf, table[length].length);
		buf[table[length].length-1] = 0;
		printf("%s\n", buf);
	}
	close(fd);
	exit(0);
}
