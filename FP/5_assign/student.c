#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

void makeRuns(FILE *inputfp, char *inputbuf);
void internalsort(char *inputbuf, int n);
void kwaymerge(FILE *outputfp, char *inputbuf, char *outputbuf);
void pack(char *recordbuf, const STUDENT *s);
void unpack(const char *recordbuf, STUDENT *s);
void readChunk(FILE *runfp, char *inputbuf, int chunkid);
void writeOutputbuf(FILE *outputfp, const char *outputbuf, int n);
int sort_func(const void *a, const void *b);

int run_num = 1, r_num = 0, chunksize = 0, rec_per_chunk = 0, check_read = 0;
short tot_rec_num = 0, del_rec_num = 0;

int main(int argc, char *argv[])
{
	FILE *inputfp, *outputfp;
	char inputbuf[INPUT_BUF_SIZE];
	char outputbuf[OUTPUT_BUF_SIZE];
    char buf[4];

    inputfp = fopen(argv[2], "r+");
    outputfp = fopen(argv[3], "a+");
    makeRuns(inputfp, inputbuf);
    memset(inputbuf, 0, INPUT_BUF_SIZE);
    memset(outputbuf, 0, OUTPUT_BUF_SIZE);

    chunksize = INPUT_BUF_SIZE / run_num;
    rec_per_chunk = chunksize / RECORD_SIZE;
    fwrite(&tot_rec_num, sizeof(tot_rec_num), 1, outputfp);
    fwrite(&del_rec_num, sizeof(del_rec_num), 1, outputfp);
    kwaymerge(outputfp, inputbuf, outputbuf);
	return 1;
}

void makeRuns(FILE *inputfp, char *inputbuf)
{
    FILE *fp;
    char run_name[10];

    fseek(inputfp, 4L, SEEK_SET);
    while((r_num = fread(inputbuf, RECORD_SIZE, RECORDS_NUM_INPUTBUF, inputfp)) > 0) {
        internalsort(inputbuf, r_num);
        tot_rec_num += r_num;
        sprintf(run_name, "run%d", run_num);
        fp = fopen(run_name, "a+");
        fwrite(inputbuf, r_num*RECORD_SIZE, 1, fp);
        fclose(fp);
        memset(inputbuf, 0, INPUT_BUF_SIZE);
        run_num++;
    }
    run_num--;
    fclose(inputfp);
}

void internalsort(char *inputbuf, int n)
{
    qsort(inputbuf, n, RECORD_SIZE, sort_func);
}

int sort_func(const void *a, const void *b) {
    int key1 = 0;
    int key2 = 0;
    int i = 0;

    while(*((char *)a+i) != '#') {
        char buf1[1];
        key1 *= 10;
        buf1[0] = *((char *)a+i);
        key1 += atoi(buf1);
        i++;
    }
    i = 0;
    while(*((char *)b+i) != '#') {
        char buf1[1];
        key2 *= 10;
        buf1[0] = *((char *)b+i);
        key2 += atoi(buf1);
        i++;
    }
    if(key1 < key2)
        return -1;
    if(key1 > key2)
        return 1;
    return 0;
}

void kwaymerge(FILE *outputfp, char *inputbuf, char *outputbuf)
{
    FILE *fp[run_num];
    int chunkpos[run_num];
    char buf[10]; 
    int key1 = 0, key2 = 0, min_key = 0, min_key_pos = 0, outputbuf_idx = 0, n = 0;
    int i, j ,all_null = 0;
    int test = 0;

    for(i = 0; i < run_num; i++) {
        sprintf(buf, "run%d", i+1);
        fp[i] = fopen(buf, "r+");
        chunkpos[i] = 0;
    }
    //하나의 run을 다 읽은 경우
    if(run_num == 1 && tot_rec_num <= RECORDS_NUM_INPUTBUF) { //run의 개수가1, inputbuf에 들어가는 레코드 수가 총 레코드 수보다 큰 경우
        while(!feof(fp[0])) {
            int length = 0;
            length = fread(inputbuf, RECORD_SIZE, RECORDS_NUM_INPUTBUF, fp[0]);
            for(i = 0; i < length*RECORD_SIZE; i++) {
                fputc(inputbuf[i], outputfp);
            }
        }
        fclose(fp[0]);
        remove("run1");
        fclose(outputfp);
    }
    else {
        for(i = 0; i < run_num; i++) {
            readChunk(fp[i], inputbuf+(i*chunksize), 0);
        }
        while(1) {
            if(OUTPUT_BUF_SIZE - outputbuf_idx < RECORD_SIZE){ //outputbuf가 다찬 경우
                test += 5;
                if(test > tot_rec_num) {
                    writeOutputbuf(outputfp, outputbuf, tot_rec_num % RECORDS_NUM_OUTPUTBUF);
                }
                else {
                    writeOutputbuf(outputfp, outputbuf, n);
                    outputbuf_idx = 0;
                    n = 0;
                }
                memset(outputbuf, 0, OUTPUT_BUF_SIZE);
            }
            else { //outputbuf가 다차지 않은경우
                for(i = 0; i < run_num; i++) { //해당 chunk를 다 outputbuf에 넣은경우
                    if((chunksize - chunkpos[i] < RECORD_SIZE)) {
                        memset(inputbuf+(i*chunksize), 0, chunksize);
                        readChunk(fp[i], inputbuf+(i*chunksize), 0);
                        chunkpos[i] = 0;
                    }
                }
                for(i = 0; i < 10; i++) { //첫번쨰 chunk에 있는 레코드의 key값을 가져옴
                    char buf1[1];
                    if(*(inputbuf+chunkpos[0]+i) == '#')
                        break;
                    else {
                        sprintf(buf1, "%c", *(inputbuf+chunkpos[0]+i));
                        key1 *= 10;
                        key1 += atoi(buf1);
                    }
                    min_key_pos = 0;
                }
                for(i = 1; i < run_num; i++) { //모든 chunk의 가장최근에 읽은 레코드의 key값과 비교
                    for(j = 0; j < 10; j++) {
                        char buf1[1];
                        if(*(inputbuf+(i*chunksize)+chunkpos[i]+j) == '#')
                            break;
                        else {
                            sprintf(buf1, "%c", *(inputbuf+(i*chunksize)+chunkpos[i]+j));
                            key2 *= 10;
                            key2 += atoi(buf1);
                        }
                    }
                    if(key1 != 0 && key2 != 0) {
                        if(key1 > key2){
                            key1 = key2;
                            min_key_pos = i;
                            key2 = 0;
                        }
                        else if(key1 < key2) {
                            key2 = 0;
                        }
                        else
                            ;
                    }
                    else if(key1 == 0 && key2 != 0) {
                        key1 = key2;
                        min_key_pos = i;
                        key2 = 0;
                    }
                    else
                        key2 = 0;
                }
                for(i = min_key_pos*chunksize+chunkpos[min_key_pos]; i < min_key_pos*chunksize+chunkpos[min_key_pos]+RECORD_SIZE; i++) {
                    outputbuf[outputbuf_idx] = inputbuf[i];
                    outputbuf_idx++;
                }
                chunkpos[min_key_pos] += 127;
                n++;
            }
            key1 = 0;
            if(test > tot_rec_num) {
                break;
            }
        }
        for(i = 0; i < run_num; i++) {
            char buf[10];
            sprintf(buf,"run%d", i+1);
            fclose(fp[i]);
            remove(buf);
        }
    }
}

void pack(char *recordbuf, const STUDENT *s)
{

}
void unpack(const char *recordbuf, STUDENT *s)
{

}

void readChunk(FILE *runfp, char *inputbuf, int chunkid)
{
   fread(inputbuf, RECORD_SIZE, rec_per_chunk, runfp);
}

void writeOutputbuf(FILE *outputfp, const char *outputbuf, int n)
{
    int i;
    
    for(i = 0; i < n*RECORD_SIZE; i++) {
        fputc(outputbuf[i], outputfp);
    }
}
