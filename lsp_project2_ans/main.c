#include <stdio.h>
#include "ssu_cp.h"

int main(int argc, char *argv[])
{
	if (ssu_cp(argc, argv) < 0)
	{
		fprintf(stderr, "ssu_cp error \n");
		usage();
		exit(1);
	}

	exit(0);
}
