#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "ssu_employee.h"

int main(int argc, char *argv[])
{
	struct ssu_employee record;
	int fd;

	if(argc != 2) {
		fprintf(stderr, "Usage : %s file\n", argv[0]);
		exit(1);
	}
	
	if((fd = open(argv[1], O_RDWR)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	while(1) {
		printf("Enter employee name <SPACE> salary : ");
		scanf("%s", record.name);

		if(record.name[0] == '.')
			break;

		scanf("%d", &record.salary);
		record.pid = getpid();

		if(write(fd, (char *)&record, sizeof(record)) < 0) {
			fprintf(stderr, "write error for %s\n", argv[1]);
			exit(1);
		}
	}
	exit(0);
}
