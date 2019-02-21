#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int param_opt;
    int flag = 0;
    char opt[10][1024];
    int i = 0;

    while((param_opt = getopt(argc, argv, "9")) != -1) {
        switch(param_opt) {
            case '9':
                for(; optind < argc; optind++) {
                    strcpy(opt[i], argv[optind]);
                    i++;
                }
                kill(atoi(opt[0]), SIGKILL);
                flag++;
                break;
        }
      }
    if(param_opt == -1 && flag == 0) {
        for(; optind < argc; optind++) {
          strcpy(opt[i], argv[optind]);
          i++;
        }
        kill(atoi(opt[0]), SIGTERM);
    }
}

