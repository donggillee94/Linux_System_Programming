#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "record.h"

struct timeval bgn, end;

int main(int argc, char **argv)
{
	Student student;
	struct stat statbuf;
	int total_record, input_record;
	int i = 0,j;
	int fd;
	char *fname = "student.dat";
	if((fd = open(fname, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}
	if(stat(fname, &statbuf) < 0) {
		fprintf(stderr, "stat error\n");
		exit(1);
	}
	total_record = statbuf.st_size / 200;
	input_record = atoi(argv[1]);
	if(input_record > total_record) {
		fprintf(stderr, "input error!!!\ntotal_record is %d\n", total_record);
		exit(1);
	}
	else {
		gettimeofday(&bgn, NULL);
		while(1) {
			if(read(fd, (char *)&student.id, sizeof(student.id)-1) < 0) {
				fprintf(stderr, "read error\n");
				exit(1);
			}
			else if(read(fd, (char *)&student.name, sizeof(student.name)-1) < 0) {
				fprintf(stderr, "read error\n");
				exit(1);
			}
			else if(read(fd, (char *)&student.address, sizeof(student.address)-1) < 0) {
				fprintf(stderr, "read error\n");
				exit(1);
			}
			else if(read(fd, (char *)&student.univ, sizeof(student.univ)-1) < 0) {
				fprintf(stderr, "read error\n");
				exit(1);
			}
			else if(read(fd, (char *)&student.dept, sizeof(student.dept)-1) < 0) {
				fprintf(stderr, "read error\n");
				exit(1);
			}
			else if(read(fd, (char *)&student.others, sizeof(student.others)-1) < 0) {
				fprintf(stderr, "read error\n");
				exit(1);
			}
			i++;
			if(i > total_record) {
				gettimeofday(&end, NULL);
				break;
			}
		}
	}
	printf("elapsed time: %ld us\n", end.tv_usec - bgn.tv_usec);
	return 0;
}
