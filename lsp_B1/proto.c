#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int open(const char *pathname, int oflag, mode_t mode);
//성공시 새 파일의 파일디스크립터, 실패시 -1을 리턴하고 errno가 설정됨


#include <unistd.h>
int close(int filedes);
//성공시 0, 에러시 -1을 리턴하고 errno가 설정됨

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int creat(const char *pathname, mode_t mode);
//성공시 쓰기전용으로 열린 파일의 디스크립터, 에러시 -1을 리턴하고 errno가 설정됨

#include <sys/types.h>
#include <unistd.h>
off_t lseek(int filedes, off_t offset, int whence);
//성공시 파일의 오프셋, 에러시 -1을 리턴하고 errno가 설정됨

#include <unistd.h>
ssize_t read(int filedes, void *buf, ssize_t nbytes);
//성공시 읽어들인 바이트 수, EOF시 0, 에러시 -1을 리턴하며 errno가 설정됨

#include <unistd.h>
ssize_t write(int filedes, const void *buf, ssize_t nbytes);
//성공시 쓴 바이트 수, 에러시 -1을 리턴하며 errno가 설정됨

#include <unistd.h>
int dup(int filedes);
int dup2(int filedes1, int filedes2);
//성공시 새로운 파일디스크립터 리턴, 실패시 -1을 리턴하고 errno가 설정됨
=========================================================================
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int open(const char *pathname, int oflag, mode_t mode);
//성공시 파일의 디스크립터, 실패시 -1을 리턴하며 errno가 설정됨

#include <unistd.h>
int close(int filedes);
//성공시 0, 실패시 -1을 리턴하며 errno가 설정됨

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
int creat(const char *pathname, mode_t mode);
//성공시 쓰기전용으로 열린 파일의 디스크립터, 실패시 -1을 리턴하며 errno가 설정됨

#include <sys/types.h>
#include <unistd.h>
off_t lseek(int filedes, off_t offset, int whence);
//성공시 파일의 오프셋, 실패시 -1을 리턴하며 errno가 설정됨

#include <unistd.h>
ssize_t read(int filedes, void *buf, ssize_t nbytes);
//성공시 읽은 바이트 수, 실패시 -1을 리턴하며 errno가 설정됨

#include <unistd.h>
ssize_t write(int filedes, void *buf, ssize_t nbytes);
//성공시 쓴 바이트 수, 실패시 -1을 리턴하며 errno가 설정됨

#include <unistd.h>
int dup(int filedes);
int dup2(int filedes1, int filedes2);
//성공시 새로운 파일디스크립터 리턴, 실패시 -1을 리턴하며 errno가 설정됨
