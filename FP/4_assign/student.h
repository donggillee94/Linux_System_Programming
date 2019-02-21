#ifndef _STUDENT_H_
#define _STUDENT_H_

#define RECORD_SIZE		127	
#define HEADER_SIZE		4	

typedef struct _STUDENT //학생 구조체
{
	char id[9];	
	char name[21];
	char dept[21];	
	char year[2];
	char addr[31];
	char phone[16];
	char email[27];
} STUDENT;

typedef struct _HEADER { //헤더 구조체
	short _records;
	short head;
} HEADER;

#endif
