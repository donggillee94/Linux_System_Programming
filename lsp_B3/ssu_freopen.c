#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	char *fname = "ssu_test.txt";
	int fd;

	printf("First printf : Hello Oslab!!\n");

	if((fd = open(fname, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	if((freopen(fname, "w", stdout)) == NULL) {
		fprintf(stderr, "freopen error for %s\n", fname);
		exit(1);
	}
	else
		printf("Second Printf : Hello Oslab!!\n");

	exit(0);
}
