#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "dshlib.h"
#include "builtin.h"
#include "parse.h"


int main()
{
    char cmd_buff[ARG_MAX] = {0};
    Parse *P;

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
           printf("\n");
           break;
        }
        //remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';

        P = parse_cmdline(cmd_buff);
        if (!P)
            goto next;
        if (P->invalid_syntax) {
            fprintf(stderr, "Invalid syntax");
            goto next;
        }

        if(!strcmp(P->tasks[0].cmd, "exit")){
            exit(EXIT_SUCCESS);
        } else if(!strcmp(P->tasks[0].cmd, "cd")){
            execute_cd(&P->tasks[0]);
            goto next;
        }
        execute_tasks(P);

    next:
        parse_destroy(&P);
    }
}