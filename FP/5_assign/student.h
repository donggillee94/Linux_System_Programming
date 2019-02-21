#ifndef _STUDENT_H_
#define _STUDENT_H_

#define RECORD_SIZE		127
#define HEADER_SIZE		4

typedef struct _STUDENT
{
	char id[9];	
	char name[21];
	char dept[21];
	char year[2];
	char addr[31];
	char phone[16];
	char email[27];		
} STUDENT;

#define RECORDS_NUM_INPUTBUF	100
#define RECORDS_NUM_OUTPUTBUF 	5	
#define INPUT_BUF_SIZE	(RECORDS_NUM_INPUTBUF * RECORD_SIZE)
#define OUTPUT_BUF_SIZE	(RECORDS_NUM_OUTPUTBUF * RECORD_SIZE)

#endif
