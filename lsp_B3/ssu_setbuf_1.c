#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main()
{
	char buf[BUFFER_SIZE];

	setbuf(stdout, buf);
	printf("Soongsil ");
	sleep(1);
	printf("University!");
	sleep(1);
	printf("\n");
	sleep(1);
	
	setbuf(stdout, NULL);
	printf("Computer ");
	sleep(1);
	printf("Science ");
	sleep(1);
	printf("& ");
	sleep(1);
	printf("Engineering");
	sleep(1);
	printf("\n");

	exit(0);
}
