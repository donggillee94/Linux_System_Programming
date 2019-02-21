#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define BUFFER_SIZE 1024

int main()
{
	char buf[BUFFER_SIZE];
	int length;

	length = read(0, buf, sizeof(buf));
	buf[length] = '\0';
	write(1, buf, length);
	exit(0);
}
