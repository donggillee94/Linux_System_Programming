#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
int stat(const char *pathname, struct stat *restrict buf);
int fstat(int filedes, struct stat *buf);
int lstat(const char *pathname, struct stat *restrict buf);
//성공시 0, 실패시 -1을 리턴하며 errno가 설정됨

#include <unistd.h>
int access(const char *pathname, int mode);
//성공시 0, 실패시 -1을 리턴하며 errno가 설정됨

#include <sys/stat.h>
#include <sys/types.h>
mode_t umask(mode_t cmask);
//이전의 파일 모드 생성 마스크

#include <sys/stat.h>
int chmod(const char *pathname, mode_t mode);
int fchmod(int filedes, mode_t mode);
//성공시 0, 실패시 -1을 리턴하며 errno가 설정됨

#include <unistd.h>
int chown(const char *pathname, uid_t owner, gid_t group);
int fchown(int filedes, uid_t owner, gid_t group);
int lchown(const char *pathname, uid_r owner, gid_t group);
//성공시 0, 실패시 -1을 리턴하며 errno가 설정됨

#include <unistd.h>
int link(const char *existingpath, const char *newpath);
int unlink(const char *pathname);
//성공시 0, 에러시 -1을 리턴하며 errno가 설정됨
//unlink시 해당 파일이 가지고 있던 디스크 공간도 사라지게 된다

#include <stdio.h>
int remove(const char *pathname);
int rename(const char *oldname, const char *newname);
//성공시 0, 에러시 -1을 리턴하며 errno가 설정됨
//remove시 파일이 가지고 있던 디스크 공간은 사라지지 않음

#include <unistd.h>
int symlink(const char *actualpath, const char *sympath);
//성공시 0, 에러시 -1을 리턴하며 errno가 설정됨

#include <sys/types.h>
#include <utime.h>
int utime(const char *pathname, const struct utimbuf *times);
//성공시 0, 에러시 -1을 리턴하고 errno가 설정됨

#include <unistd.h>
int chdir(const char *pathname);
//성공시 0, 에러시 -1을 리턴하고 errno가 설정됨

#include <unistd.h>
char *getcwd(char *buf, size_t size);
//성공시 현재 작업 디렉토리의 pathname, 에러시 NULL을 리턴하고 errno가 설정됨
==========================================================================

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
int stat(const char *restrict pathname, struct stat *restrict statbuf);
int fstat(int filedes, struct stat *restrict statbuf);
int lstat(const char *restrict pathname, struct stat *restrict statbuf);
//성공시 0, 에러시 -1을 리턴하고 errno가 설정됨

#include <unistd.h>
int access(const char *pathname, int mode);
//성공시 0, 에러시 -1을 리턴하고 errno가 설정됨

#include <sys/stat.h>
#include <sys/types.h>
mode_t umask(mode_t cmask);
//이전의 파일 모드 생성 마스크

#include <sys/stat.h>
int chmod(const char *pathname, mode_t mode);
int fchmod(int filedes, mode_t mode);
//성공시 0, 에러시 -1을 리턴하고 errno가 설정됨

#include <unistd.h>
int chown(const char *pathname, uid_t owner, gid_t group);
int fchown(int filedes, uid_t owner, gid_t group);
int lchown(const char *pathname, uid_t owner, gid_t group);
//성공시 0, 에러시 -1을 리턴하고 errno가 설정됨

#include <unistd.h>
int link(const char *oldname, const char *newname);
int unlink(const char *existingpath);
//성공시 0, 에러시 -1을 리턴하고 errno가 설정됨

#include <stdio.h>
int remove(const char *pathname);
int rename(const char *oldname, const char *newname);
//성공시 0, 에러시 -1을 리턴하고 errno가 설정됨

#include <unistd.h>
int symlink(const char *actualpath, const char *sympath);
//성공시 0, 에러시 -1을 리턴하고 errno가 설정됨

#include <utime.h>
#include <sys/types.h>
int utime(const char *pathname, const struct utimbuf *times);
//성공시 0 에러시 -1을 리턴하고 errno가 설정됨
//utimbuf구조체 멤버 : time_t actime, time_t modtime

#include <sys/stat.h>
#include <sys/types.h>
int mkdir(const char *pathname, mode_t mode);
#include <unistd.h>
int rmdir(const char *pathname);
//성공시 0, 실패시 -1을 리턴하며 errno가 설정됨

#include <sys/types.h>
#include <dirent.h>
DIR *opendir(const char *pathname);
//성공시 포인터, 실패시 NULL을 리턴하며 errno가 설정됨

#include <dirent.h>
struct dirent *readdir(DIR *dp);
//성공시 포인터, 실패시 NULL을 리턴하며 errno가 설정됨

#include <unistd.h>
int chdir(const char *pathname);
int fchdir(int filedes);
//성공시 0, 실패시 -1을 리턴하며 errno가 설정됨

#include <unistd.h>
char *getcwd(char *buf, size_t size);
//성공시 현재 작업 디렉토리의 절대경로, 에러시 NULL을 리턴하고 errno가 설정됨


