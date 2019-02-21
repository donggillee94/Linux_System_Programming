#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>

void ssu_func(int loc_var, int loc_volatile, int loc_register);

int count = 0;
static jmp_buf glob_buffer;

int main()
{
    register int loc_register;
    volatile int loc_volatile;
    int loc_var;

    int ret_val;

    loc_var = 10; loc_volatile = 11; loc_register = 12;

    if((ret_val = setjmp(glob_buffer)) != 0) {
        printf("after longjmp, loc_var = %d, loc_volatile = %d, loc_register = %d\n", loc_var, loc_volatile, loc_register);
        printf("ret_val = %d\n", ret_val);
        exit(0);
    }

    loc_var = 80; loc_volatile = 81; loc_register = 82; //컴파일시 최적화 옵션을 주지 않으면 메모리에 저장되기에 longjmp 호출 당시의 변수값을 유지함, 그러므로 그냥 컴파일하면 얘네가 출력됨
    ssu_func(loc_var, loc_volatile, loc_register);
    exit(0);
}

void ssu_func(int loc_var, int loc_volatile, int loc_register) {
    if(count == 3)
        longjmp(glob_buffer, 1);
    count++;
    printf("ssu_func, loc_var = %d, loc_volatile = %d, loc_register = %d\n", loc_var, loc_volatile, loc_register);
    ssu_func(++loc_var, ++loc_volatile, ++loc_register);
    printf("ssu_func, loc_var = %d, loc_volatile = %d, loc_register = %d\n", loc_var, loc_volatile, loc_register);
}
