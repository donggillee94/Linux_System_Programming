#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "record.h"

#define MODE 0644

int main(int argc, char **argv)
{
	Student student;
	char *str;
	int fd;
	int record_num, i = 1;

	if (argc < 2) {
		fprintf(stderr, "Usage error!\nUsage : %s filename\n", argv[0]);
		exit(1);
	}

	if ((fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, MODE)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}
	
	printf("Enter record number : ");
	scanf("%d", &record_num);

	while (1) {
		switch(i) {
			case 1:
				str = "st";
				break;
			case 2:
				str = "nd";
				break;
			case 3:
				str = "rd";
				break;
			default:
				str = "th";
				break;
		}
		printf("Enter %d%s Student's record informations\n",i,str);
		printf("Id <SPACE> Name <SPACE> Address <SPACE> University <SPACE> Department <SPACE> Others\n");
	//	scanf("%s %s %s %s %s %s", student.id, student.name, student.address, student.univ, student.dept, student.others);

		if (write(fd, (char *)&student.id, sizeof(student.id)-1) < 0) {
			fprintf(stderr, "record %s write error\n", "Id");
			exit(1);
		}
		else if(write(fd, (char *)&student.name, sizeof(student.name)-1) < 0) {
			fprintf(stderr, "record %s write error\n", "Name");
			exit(1);
		}
		else if(write(fd, (char *)&student.address, sizeof(student.address)-1) < 0) {
			fprintf(stderr, "record %s write error\n", "Name");
			exit(1);
		}
		else if(write(fd, (char *)&student.univ, sizeof(student.univ)-1) < 0) {
			fprintf(stderr, "record %s write error\n", "Name");
			exit(1);
		}
		else if(write(fd, (char *)&student.dept, sizeof(student.dept)-1) < 0) {
			fprintf(stderr, "record %s write error\n", "Name");
			exit(1);
		}
		else if(write(fd, (char *)&student.others, sizeof(student.others)-1) < 0) {
			fprintf(stderr, "record %s write error\n", "Name");
			exit(1);
		}
		else {
			i++;
			if (i > record_num) {
				printf("Input Finished\n");
				i = 1;
				break;
			}
		}
	}
	close(fd);

	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}
	while (1) {
		if (read(fd, (char *)&student.id, sizeof(student.id)-1) < 0) {
			fprintf(stderr, "record %s write error\n", "Id");
			exit(1);
		}
		else if(read(fd, (char *)&student.name, sizeof(student.name)-1) < 0) {
			fprintf(stderr, "record %s write error\n", "Name");
			exit(1);
		}
		else if(read(fd, (char *)&student.address, sizeof(student.address)-1) < 0) {
			fprintf(stderr, "record %s write error\n", "Name");
			exit(1);
		}
		else if(read(fd, (char *)&student.univ, sizeof(student.univ)-1) < 0) {
			fprintf(stderr, "record %s write error\n", "Name");
			exit(1);
		}
		else if(read(fd, (char *)&student.dept, sizeof(student.dept)-1) < 0) {
			fprintf(stderr, "record %s write error\n", "Name");
			exit(1);
		}
		else if(read(fd, (char *)&student.others, sizeof(student.others)-1) < 0) {
			fprintf(stderr, "record %s write error\n", "Name");
			exit(1);
		}
		else {
			i++;
			if(i > record_num)
				break;
		}
	}
	exit(0);
}
