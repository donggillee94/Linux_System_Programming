#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if(argc != 3) {
		fprintf(stderr, "usage : %s oldname newname\n", argv[0]);
		exit(1);
	}

	if(link(argv[1], argv[2]) < 0) {
		fprintf(stderr, "link error for %s %s\n", argv[1], argv[2]);
		exit(1);
	}
	else
		printf("Step1 passed\n");

	if(remove(argv[1]) < 0) {
		fprintf(stderr, "remove error\n");
		remove(argv[2]);
		exit(1);
	}
	else
		printf("Step2 paseed\n");

	printf("Succeeded\n");
	exit(0);
}