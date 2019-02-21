#ifndef _RECORD_H_
#define _RECORD_H_

//student record size = 200 bytes
typedef struct _Student
{
	char id[11];		//�й�: 10 bytes
	char name[21];		//�̸�: 20 bytes
	char address[51];	//�ּ�: 50 bytes
	char univ[21];		//�б�: 20 bytes
	char dept[31];		//�а�: 30 bytes
	char others[71];	//�׿�: 70 bytes
} Student;

#endif
