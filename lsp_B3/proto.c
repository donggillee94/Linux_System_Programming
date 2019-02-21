#include <stdio.h>
void setbuf(FILE *fp, char *buf);
int setvbuf(FILE *fp, char *buf, int mode, size_t size);
//성공시 0, 실패시 -1을 리턴하며 errno가 설정됨

#include <stdio.h>
FILE *fopen(const char *pathname, const char *mode);
FILE *fdopen(int filedes, const char *mode);
FILE *freopen(const char *pathname, const char *mode, FILE *fp);
//성공시 파일 포인터, 실패시 NULL을 리턴하며 errno가 설정됨

#include <stdio.h>
int fclose(FILE *fp);
//성공시 0, 실패시 EOF를 리턴하고 errno가 설정됨

#include <stdio.h>
int getc(FILE *fp);
int fgetc(FILE *fp);
int getchar(FILE *fp);//얘는 표준입력 함수
//성공시 다음 문자, 파일의 끝이나 에러시 EOF를 리턴함

#include <stdio.h>
int ferror(FILE *fp);
int feof(FILE *fp);
//조건이 참이면 0이 아닌값(true), 거짓이면 0(false)

void clearerr(FILE *fp);
//FILE구조체 내의 _flag를 초기화해줌


#include <stdio.h>
int ungetc(int c, FILE *fp);
//성공시 c, 실패시 EOF

#include <stdio.h>
int putc(int c, FILE *fp);
int fputc(int c, FILE *fp);
int putchar(int c);//얘는 표준 출력 함수
//성공시 c 실패시 EOF

#include <stdio.h>
char *fgets(char *buf, int n, FILE *fp);
char *gets(char *buf);//얘는 표준입력으로부터 개행문자까지 입력을 받음
//성공시 buf 파일의 끝이나 에러시 NULL

#include <stdio.h>
int fputs(const char *str, FILE *fp);
int puts(const char *str);
//성공시 음이 아닌 값, 실패시 EOF

#include <stdio.h>
size_t fread(void *ptr, size_t size, size_t nobj, FILE *fp);
size_t fwrite(const void *ptr, size_t size, size_t nobj, FILE *fp);
//성공적으로 읽거나 쓴 객체들의 개수

#include <stdio.h>
long ftell(FILE *fp);
//성공시 파일의 현재 오프셋, 에러시 -1L을 리턴하고 errno가 설정됨

int fseek(FILE *fp, long offset, int whence);
//성공시 0, 에러시 0이 아닌 값

void rewind(FILE *fp);
//파일의 오프셋으로 처음르로 되돌려줌

#include <stdio.h>
int printf(const char *format, ...);
int fprintf(FILE *fp, const char *format, ...);
//성공시 출력된 문자 개수, 출력 에러시 음의 값

int sprintf(char *buf, const char *format, ...);
int snprintf(char *buf, size_t n, const char *format, ...);
//성공시 배열에 저장된 문자 개수, 부호화 에러 시 음의 값

int scanf(const char *format, ...);
int fscanf(FILE *fp, const char *format, ...);
int sscanf(char *buf, const char *format, ...);
//성공시 배정된 입력 항목들의 개수 리턴, 어떠한 변화도 일어나기 전에 파일 끝에 도달한 경우에는 EOF리턴
//읽기 에러 발생시 EOF를 리턴하고 errno가 설정됨

==========================================================================

#include <stdio.h>
void setbuf(FILE *fp, char *buf);
int setvbuf(FILE *fp, char *buf, int mode, size_t size);
//성공시 0, 실패시 0이 아닌 값을 리턴하고 errno가 설정됨
//mode 인자 : _IOFBF, _IOLBF, _IONBF

#include <stdio.h>
FILE *fopen(const char *pathname, const char *mode);
FILE *fdopen(int filedes, const char *mode);
FILE *freopen(const char *pathname, const char *mode, FILE *fp);
//성공시 파일 포인터, 에러시 NULL을 리턴하고 errno가 설정됨

#include <stdio.h>
int fclose(FILE *fp);
//성공시 0, 실패시 EOF를 리턴하고 errno가 설정됨

#include <stdio.h>
int getc(FILE *fp);
int fgetc(FILE *fp);
int getchar(void);
//성공시 다음 문자, 실패시 EOF

#include <stdio.h>
int ferror(FILE *fp);
int feof(FILE *fp);
//조건이 참이면 0이 아닌 값, 거짓이면 0을 리턴
void clearerr(FILE *fp);

#include <stdio.h>
int ungetc(int c, FILE *fp);
//성공시 c, 실패시 EOF 리턴

#include <stdio.h>
int putc(int c, FILE *fp);
int fputc(int c, FILE *fp);
void putchar(FILE *fp);
//성공시 c, 실패시 EOF 리턴

#include <stdio.h>
char *fgets(char *buf, FILE *fp);
char *gets(char *buf);
//성공시 buf, 실패시 NULL

#include <stdio.h>
char *fputs(const char *buf, FILE *fp);
char *puts(const char *buf);
//성공시 음이 아닌 값, 에러시 EOF 리턴

#include <stdio.h>
size_t fread(void *ptr, size_t size, size_t nobj, FILE *fp);
size_t fwrite(const void *ptr, size_t size, size_t nobj, FILE *fp);
//성공적으로 읽거나 쓴 객체들의 수

#include <stdio.h>
long ftell(FILE *fp);
//파일의 현재 오프셋, 실패시 -1L을 리턴하고 errno가 설정됨

#include <stdio.h>
int fseek(FILE *fp, long offset, int whence);
//성공시 0, 에러시 0이 아닌 값

#include <stdio.h>
int printf(const char *format, ...);
int fprintf(FILE *fp, const char *format, ...);
//성공시 출력된 문자 개수, 에러시 음의 값
int sprintf(char *buf, const char *format, ...);
int snprintf(char *buf, size_t size, const char *format, ...);
//성공시 배열에 저장된 문자 개수, 부호화 에러시 음의 값

#include <stdio.h>
int scanf(const char *format, ...);
int fscanf(FILE *fp, const char *format);
//성공적으로 읽어들인 인자의 수, 에러시 EOF를 리턴하고 errno가 설정됨
int sscanf(const char *buf, const char *format, ...);
