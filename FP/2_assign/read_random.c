#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "record.h"

#define SUFFLE_NUM	10000

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
struct timeval bgn, end;

int main(int argc, char **argv)
{
	struct stat statbuf;
	Student student;
	int num_of_records, input_record;
	char *fname = "student.dat";
	int fd, i;

	if((fd = open(fname, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	if(stat(fname, &statbuf) < 0) {
		fprintf(stderr, "stat error\n");
		exit(1);
	}
	
	num_of_records = (int)statbuf.st_size / 200;
	input_record = atoi(argv[1]);
	if(input_record > num_of_records) {
		fprintf(stderr, "Input error!!!\ninput_record > num_of_records\n");
		exit(1);
	}
	int read_order_list[num_of_records];
	GenRecordSequence(read_order_list, num_of_records);
	
	gettimeofday(&bgn, NULL);
	for(i = 0; i < input_record; i++) {
		lseek(fd, (long)read_order_list[i]*(sizeof(student)-6), 0);
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
	}

	gettimeofday(&end, NULL);
	printf("elapsed time: %ld us\n", end.tv_usec - bgn.tv_usec);
	return 0;
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(NULL));

	for(i=0; i<n; i++) {
		list[i] = i;
	}
	
	for(i=0; i<SUFFLE_NUM; i++) {
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}
	return;
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

	return;
}
