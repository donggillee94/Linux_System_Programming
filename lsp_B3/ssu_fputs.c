#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
	char buf[BUFFER_SIZE];
	FILE *fp;

	if(argc < 2) {
		fprintf(stderr, "Usage : %s file\n", argv[0]);
		exit(1);
	}

	if((fp = fopen(argv[1], "w+")) == NULL) {
		fprintf(stderr, "fopen error for %s\n", argv[1]);
		exit(1);
	}

	fputs("Input String >> ", stdout);
	fgets(buf, sizeof(buf), stdin);
	fputs(buf, fp);
	rewind(fp);
	fgets(buf, sizeof(buf), fp);
	puts(buf);
	fclose(fp);
	exit(0);
}

