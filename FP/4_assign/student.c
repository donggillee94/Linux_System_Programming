#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

void pack(char *recordbuf, const STUDENT *s);

void unpack(const char *recordbuf, STUDENT *s);

void read(FILE *fp, char *recordbuf, int rrn);

void add(FILE *fp, const STUDENT *s);

int search(FILE *fp, const char *keyval);

void delete(FILE *fp, const char *keyval);

void printRecord(const STUDENT *s, int n);

HEADER h;
STUDENT s;
char recordbuf[RECORD_SIZE];
char headerinfo[4];

int main(int argc, char *argv[])
{
	FILE *fp;
	int i;

	fp = fopen(argv[2], "a+");
	if(fp == NULL) {
		fprintf(stderr, "fopen error\n");
		exit(1);
	}

	if(!strcmp(argv[1], "-a")){
		fseek(fp, 0L, SEEK_END);

		if(ftell(fp) == 0) {
			h.head = -1;//header에 초기 data입력
			fwrite(&h, sizeof(h), 1, fp);
		}
		strcpy(s.id, argv[3]);//argv로 받은 학생에 대한 정보 학생 구조체에 넣어줌
		strcpy(s.name, argv[4]);
		strcpy(s.dept, argv[5]);
		strcpy(s.year, argv[6]);
		strcpy(s.addr, argv[7]);
		strcpy(s.phone, argv[8]);
		strcpy(s.email, argv[9]);
		add(fp, &s); //add 함수 호출
	}
	else if(!strcmp(argv[1], "-s")){ //search함수 호출
		search(fp, argv[3]);
	}
	else {
		delete(fp, argv[3]); //delete 함수 호출
	}
	return 1;
}
void pack(char *recordbuf, const STUDENT *s) {//student 구조체 내용을 저장할때 delimeter을 넣어줘서 규격맞춰주는 함수
	sprintf(recordbuf, "%s#%s#%s#%s#%s#%s#%s#", s->id, s->name, s->dept, s->year, s->addr, s->phone, s->email);
}
void unpack(const char *recordbuf, STUDENT *s) {//delimeter가 들어가있는 format의 문자열을 delimeter를 제외한 정보를 student에 넣어주는 함수
	int i;
	int flag = 0;
	int off = 0;
	for(i = 0; i <= RECORD_SIZE; i++) { //recordbuf 크기만큼 반복문 수행
		if(recordbuf[i] == '#') {//구분자 만났을시
			flag++;
			off = 0;
		}
		else {//구분자가 아닌 student에 대한 정보
			if(flag == 0) { //첫번째 정보에 대해 수행
				s->id[off] = recordbuf[i];//student 구조체 내의 학번에 대해 저장할때 끝에 널문자 삽입
				if(off+1 > sizeof(s->id)) {
					;
				}
				else
					s->id[off+1] = '\0';
			}
			else if(flag == 1) { //구조체 내의 2번쨰 멤버에 대해 저장
				s->name[off] = recordbuf[i];
			}
			else if(flag == 2) { //구조체 내의 3번째 멤버에 대해 저장
				s->dept[off] = recordbuf[i];
			}
			else if(flag == 3) { //구조체 내의 4번째 멤버에 대해 저장
				s->year[off] = recordbuf[i];
			}
			else if(flag == 4) { //구조체 내의 5번째 멤버에 대해 저장
				s->addr[off] = recordbuf[i];
			}
			else if(flag == 5) { //구조체 내의 6번째 멤버에 대해 저장
				s->phone[off] = recordbuf[i];
			}
			else if(flag == 6) { //구조체 내의 7번째 멤버에 대해 저장
				s->email[off] = recordbuf[i];
			}
			else
				;
			off++;
		}
	}

}

void read(FILE *fp, char *recordbuf, int rrn) { //record를 읽는 함수
	fseek(fp, sizeof(h)+RECORD_SIZE*rrn, SEEK_SET);
	fread(recordbuf, RECORD_SIZE, 1, fp);
}

void add(FILE *fp, const STUDENT *s) { //record 추가해주는 함수
	long size;
	int i = 0;
	short last_del;
	pack(recordbuf, s);
	fseek(fp, 0L, SEEK_SET);
	fread(&h, sizeof(h), 1, fp);
	if(h.head == -1) { //지워진 record가 없을시
		fseek(fp, 0, SEEK_END); //파일의 끝으로 오프셋 이동
		fwrite(recordbuf, RECORD_SIZE, 1, fp); //해당 위치의 오프셋에 record저장
		fseek(fp, 0, SEEK_SET); //파일의 처음으로 오프셋 이동
		fread(&h, sizeof(h), 1, fp); //파일의 처음에 있는 헤더에 접근해서 헤서 헤더에 있는 레코드 개수를 갱신해줌
		h._records++;
		fseek(fp, 0, SEEK_SET);
		fwrite(&h, sizeof(h), 1, fp);
	}
	else { //지워진 record가 있을 경우
		fseek(fp, sizeof(h)+RECORD_SIZE*(h.head-1)+1, SEEK_SET); //가장 최근에 지워진 record에 대해서 접근
		fread(&last_del, sizeof(last_del), 1, fp); //해당 지워진 record에 저장되어있는 이전에 지워진 record에 대한 정보 받아옴
		fseek(fp, sizeof(h)+RECORD_SIZE*(h.head-1), SEEK_SET); //이전에 지워진 record에 대해 정보 받을시 오프셋이 이동되었으므로 해당 record의 처음으로 오프셋을 지정해줌
		fwrite(recordbuf, RECORD_SIZE, 1, fp); //입력받은 record를 넣어줌
		h.head = last_del; //헤더에 있는 최근에 지워진 record number, 현재 record개수를 갱신해줌
		h._records++;
		fseek(fp, 0, SEEK_SET); //파일의 처음으로감
		fwrite(&h, sizeof(h), 1, fp); //헤더정보 갱신
	}
}

int search(FILE *fp, const char *keyval) { //원하는 record를 검색해서 출력해주는 함수
	int i = 0;
	long size;
	fseek(fp, 0L, SEEK_END); //파일의 끝으로 오프셋 이동
	size = ftell(fp); //파일의 총 사이즈를 받아옴
	fseek(fp, 0L, SEEK_SET); //파일의 처음으로 오프셋 이동
	fread(&h, sizeof(h), 1, fp); //헤더정보 받아와서 header구조체에 저장
	while(1) { //파일의 끝까지 record 검색
		read(fp, recordbuf, i); //record 읽음
		unpack(recordbuf, &s); //해당레코드를 구분해서 student구조체에 정보 저장
		if(!strcmp(s.id, keyval)) { //내가 원하는 record랑 읽어온 record랑 정보 일치할때
			printRecord(&s, 1); //해당 레코드 출력
			break; //반복문 종료
		}
		if(size == ftell(fp)) //파일의 끝까지 찾아봤으면
			break;	//반복문 종료
		i++;
	}
}

void delete(FILE *fp, const char *keyval) { //내가 원하는 record를 지워주는 함수
	int i = 0;
	long size;
	char del_mark = '*'; //지운 record에 넣어줄 mark
	fseek(fp, 0L, SEEK_END); //파일의 끝까지 가서 파일의 총 사이즈 구함
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET); //파일의 처음으로 오프셋 이동
	fread(&h, sizeof(h), 1, fp); //헤더의 정보를 받아옴
	while(1) { //파일의 끝까지 반복문 수행
		read(fp, recordbuf, i); //record를 읽음
		unpack(recordbuf, &s); //읽어온 record에 대해 student 구조체에 저장
		if(!strcmp(s.id, keyval)) { //내가 지울 record랑 읽어온 record랑 비교해봤을시 일치하면
			fseek(fp, -RECORD_SIZE, SEEK_CUR); //오프셋을 해당 record의 처음으로 되돌림
			fwrite(&del_mark, sizeof(del_mark), 1 ,fp); //record의 처음에 지운 record라는 mark삽입
			fwrite(&h.head, sizeof(h.head), 1, fp); //이전에 지워진 record정보도 삽입
			h.head = i+1; //header에 가장 최근에 지워진 record번호, 현재 파일에 존재하는 총 레코드 수를 갱신해줌
			h._records--;
			fseek(fp, 0L, SEEK_SET);
			fwrite(&h, sizeof(h), 1, fp);
			break;
		}
		if(size == ftell(fp)) //파일의 끝까지 찾아도 지우길 원하는 record가 없을 시 종료
			break;
		i++;
	}
}

void printRecord(const STUDENT *s, int n) //record를 출력해주는 함수
{
	int i;

	for(i=0; i<n; i++)
	{
		printf("%s#%s#%s#%s#%s#%s#%s\n", s[i].id, s[i].name, s[i].dept, s[i].year, s[i].addr, s[i].phone, s[i].email);
	}
}
