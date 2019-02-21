#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	printf("Process ID         = %d\n", getpid()); //프로세스 id
	printf("Parent Process ID  = %d\n", getppid());//부모프로세스 id
	printf("Real user ID       = %d\n", getuid()); //user id
	printf("Effective user ID  = %d\n", geteuid());// effective user id
	printf("Real group ID      = %d\n", getgid());//group id
	printf("Effective group ID = %d\n", getegid());//effective group id
	exit(0);
}
