#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int main()
{
	FILE *fp;
	int character;
	int number = 0;
	char operator;

	if((fp = fopen("ssu_expr.txt", "r")) == NULL) {
		fprintf(stderr, "fopen error\n");
		exit(1);
	}
	while(!feof(fp)) {
		while((character = fgetc(fp)) != EOF && isdigit(character)) 
			number = 10 * number + character - 48;

			fprintf(stdout, "%d\n", number);
			number = 0;

			if(character != EOF) {
				ungetc(character, fp);
				operator = fgetc(fp);
				printf("Operator => %c\n", operator);
			}
	}

	fclose(fp);
	exit(0);
}
