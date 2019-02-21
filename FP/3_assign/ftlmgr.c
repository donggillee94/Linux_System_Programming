//#define PRINT_FOR_DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include "blkmap.h"
#include "devicedriver.c"

BlockMapTbl blkmaptbl;

/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);
void initialize_flash_memory();
void print_block(int pbn);
void print_blkmaptbl();

//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다
//
void ftl_open()
{
	int i;

	// initialize the address mapping table
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		blkmaptbl.entry[i].pbn = -1;
		blkmaptbl.entry[i].first_free_page_offset = 0;
	}

	//
	// 추가적으로 필요한 작업이 있으면 수행할 것
	//

	return;
}

void ftl_write(int lsn, char *sectorbuf)
{
	int free_pbn;
	int i, invalid, empty_block_cnt = 0;
	int lbn = lsn/4;
	int pbn = blkmaptbl.entry[lbn].pbn;
	int ppn = pbn * 4;
	char buf[PAGE_SIZE];
	char page[PAGE_SIZE];
	char page_test[PAGE_SIZE];
	int previous_pbn;
	
	if(blkmaptbl.entry[lbn].pbn == -1) {
		for(int j = 0; j < BLOCKS_PER_DEVICE; j++){
			dd_read(j*4, page_test);
			if((int)*page_test == -1) {
				blkmaptbl.entry[lbn].pbn = j;
				break;
			}
		}
	}
	for(i = 0; i < PAGES_PER_BLOCK; i++) {
		dd_read(blkmaptbl.entry[lbn].pbn*4+i, buf);
		char lsn_test[4];
		sprintf(lsn_test, "%d", lsn);
		if(strcmp(lsn_test, buf+SECTOR_SIZE) == 0) {
			sprintf(buf+SECTOR_SIZE+4, "%d", TRUE);
			dd_write(blkmaptbl.entry[lbn].pbn*4+i, buf);
		}
	}
	if(blkmaptbl.entry[lbn].first_free_page_offset == 4) {//check that block is full
		for(int j = 0; j < BLOCKS_PER_DEVICE; j++) {
			dd_read(j*4, page_test);
			if((int)*page_test == -1) {
				empty_block_cnt++;
				if(empty_block_cnt == 1) {
					blkmaptbl.entry[lbn].first_free_page_offset =0;
					previous_pbn = blkmaptbl.entry[lbn].pbn;
					blkmaptbl.entry[lbn].pbn = j;
				}
			}
		}	
		for(int j = 0; j < PAGES_PER_BLOCK; j++) {
			char page2[PAGE_SIZE];
			dd_read(previous_pbn*4+j,page2);
			if(atoi(page2+SECTOR_SIZE+4) == FALSE) {
				dd_write(blkmaptbl.entry[lbn].pbn*4+(blkmaptbl.entry[lbn].first_free_page_offset++), page2);
				sprintf(page2+SECTOR_SIZE+4, "%d", TRUE);
				dd_write(previous_pbn*4+j,page2);
			}
		}
		dd_erase(previous_pbn);
		if(empty_block_cnt <= 1) {
			int cnt_valid = 0;
			char page_test[PAGE_SIZE];
			for(int j = 0; j < BLOCKS_PER_DEVICE; j ++) {
				for(int k = 0; k < PAGES_PER_BLOCK; k++) {
					dd_read(j*PAGES_PER_BLOCK+k, page_test);
					if(atoi(page_test+SECTOR_SIZE+4) == TRUE)
						cnt_valid++;
				}
				if(cnt_valid == 4) {
					dd_erase(j);
					break;
				}
				cnt_valid = 0;
			}
		}

	}
	sprintf(page, "%s", sectorbuf); 
	sprintf(page+SECTOR_SIZE, "%d", lsn); 
	sprintf(page+SECTOR_SIZE+4, "%d", FALSE);
	dd_write(blkmaptbl.entry[lbn].pbn*4+blkmaptbl.entry[lbn].first_free_page_offset, page);
	blkmaptbl.entry[lbn].first_free_page_offset++;
	
#ifdef PRINT_FOR_DEBUG				// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
	print_blkmaptbl();
#endif

	return;
}

void ftl_read(int lsn, char *sectorbuf)
{
	int lbn = lsn/4;
	int pbn = blkmaptbl.entry[lbn].pbn;
	int ppn = pbn * 4;
	int i;
	char page[PAGE_SIZE];
	
	for(i = 0; i < PAGES_PER_BLOCK; i++) {
		dd_read(ppn+i, page);
		if((lsn == atoi(page+SECTOR_SIZE)) && (TRUE != atoi(page+SECTOR_SIZE+4))) {
			dd_read(ppn+i, sectorbuf);
		}
	}
#ifdef PRINT_FOR_DEBUG				// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
	print_blkmaptbl();
#endif

	return;
}

//
// initialize flash memory where each byte are set to 'OxFF'
// 
void initialize_flash_memory()
{
	char *blockbuf;
	int i;

	blockbuf = (char *)malloc(BLOCK_SIZE);
	memset(blockbuf, 0xFF, BLOCK_SIZE);

	for(i = 0; i < BLOCKS_PER_DEVICE; i++)
	{
		fwrite(blockbuf, BLOCK_SIZE, 1, devicefp);
	}

	free(blockbuf);

	return;
}

void print_block(int pbn)
{
	char *pagebuf;
	SpareData *sdata;
	int i;
	
	pagebuf = (char *)malloc(PAGE_SIZE);
	sdata = (SpareData *)malloc(SPARE_SIZE);

	printf("Physical Block Number: %d\n", pbn);

	for(i = pbn*PAGES_PER_BLOCK; i < (pbn+1)*PAGES_PER_BLOCK; i++)
	{
		dd_read(i, pagebuf);
		memcpy(sdata, pagebuf+SECTOR_SIZE, SPARE_SIZE);
		printf("\t   %5d-[%5d][%5d]\n", i, sdata->lsn, sdata->invalid);
	}

	free(pagebuf);
	free(sdata);

	return;
}

void print_blkmaptbl()
{
	int i;

	printf("Block Mapping Table: \n");
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		if(blkmaptbl.entry[i].pbn >= 0)
		{
			printf("[%d %d %d]\n", i, blkmaptbl.entry[i].pbn, blkmaptbl.entry[i].first_free_page_offset);
		}
	}
}
