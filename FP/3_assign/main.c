#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "blkmap.h"
#include "ftlmgr.c"

extern FILE *devicefp;

int main(int argc, char *argv[])
{
	FILE *workloadfp;
	char sectorbuf[SECTOR_SIZE];
	char pagebuf[PAGE_SIZE];
	int lsn;
	int flag;

	devicefp = fopen("flashmemory", "w+b");			// 가상 flash memory의 파일명은 'flashmemory'으로 사용하기 바람
	if(devicefp == NULL)
	{
		printf("file open error\n");
		exit(1);
	}

	initialize_flash_memory();
	ftl_open();
	//
	// ftl_write() 및 ftl_read() 테스트를 위한 코드를 자유자재로 만드세요
	//
	while(1) {
		scanf("%d", &flag);
		if(flag == -1) break;
		else {
			ftl_write(flag, "test");
		}
		for(int i = 0; i < BLOCKS_PER_DEVICE; i++) {
			print_block(i);
		}
		print_blkmaptbl();
	}
	printf("read\n");
	scanf("%d", &flag);
	ftl_read(flag, pagebuf);
	printf("%s\n", pagebuf);
	fclose(devicefp);

	return 0;
}
