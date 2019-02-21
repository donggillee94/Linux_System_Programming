#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() 
{
	struct stat statbuf;
	struct stat statbuf1;

	stat("test_code3.o", &statbuf);
	stat("test_code3.c", &statbuf1);
	printf("%ld %ld\n", statbuf.st_mtime, statbuf1.st_mtime);
}
