#include <stdio.h>
#include <stdlib.h>

int main()
{
	printf("abort terminate this program\n");
	abort(); //abort호출하여 자신에게 SIGABRT발생
	printf("this line is never reached\n");
	exit(0);
}
