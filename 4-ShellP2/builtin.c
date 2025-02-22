#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "parse.h"
#include <linux/limits.h>

void print_dragon();

static char *builtin[] = {
    "exit",   /* exits the shell */
    //"which",  /* displays full path to command */
    "cd",     /* changes the working directory */
    "dragon",
    NULL
};

static int execute_which(Task *T){
    char *dir;
    char *tmp;
    char *PATH;
    char *state;
    char probe[PATH_MAX];

    if (access(T->argv[1], X_OK) == 0){
        getcwd(probe, (PATH_MAX - 2));
        printf("%s\n", probe);
        return 1;
    }

    // check if it is a built-in command
    if(is_builtin(T->argv[1]))
        printf("%s: shell built-in command\n", T->argv[1]);

    PATH = strdup(getenv("PATH"));

    for (tmp=PATH; ; tmp=NULL) {
        dir = strtok_r(tmp, ":", &state);
        if (!dir)
            break;

        strncpy(probe, dir, PATH_MAX-1);
        strncat(probe, "/", PATH_MAX-1);
        strncat(probe, T->argv[1], PATH_MAX-1);

        if (access(probe, X_OK) == 0) {
            printf("%s\n", probe);
            free(PATH);
            return 1;
        }
    }

    free(PATH);
    return 0;
}

int execute_cd(Task *T){
    if (T->argv[1] == NULL) {
        fprintf(stderr, "cd: path is NULL\n");
        return 0;
    }

    if (chdir(T->argv[1]) != 0) {
        perror("cd");
        return 0;
    }
    return 1;
}


int is_builtin(char *cmd)
{
    int i;

    for (i=0; builtin[i]; i++) {
        if (!strcmp(cmd, builtin[i]))
            return 1;
    }

    return 0;
}


void builtin_execute(Task *T)
{
    if (!strcmp(T->cmd, "exit")) {
        exit(EXIT_SUCCESS);
    } else if (!strcmp(T->cmd, "which")) {
        execute_which(T);
    } else if (!strcmp(T->cmd, "cd")) {
        execute_cd(T);
    } else if (!strcmp(T->cmd, "dragon")) {
        print_dragon();
    } else {
        printf("pssh: builtin command: %s (not implemented!)\n", T->cmd);
    }
    exit(EXIT_SUCCESS);
}

void print_dragon(){
    puts("                                                                        @%%%%                       ");
    puts("                                                                     %%%%%%                         ");
    puts("                                                                    %%%%%%                          ");
    puts("                                                                 % %%%%%%%           @              ");
    puts("                                                                %%%%%%%%%%        %%%%%%%           ");
    puts("                                       %%%%%%%  %%%%@         %%%%%%%%%%%%@    %%%%%%  @%%%%        ");
    puts("                                  %%%%%%%%%%%%%%%%%%%%%%      %%%%%%%%%%%%%%%%%%%%%%%%%%%%          ");
    puts("                                %%%%%%%%%%%%%%%%%%%%%%%%%%   %%%%%%%%%%%% %%%%%%%%%%%%%%%           ");
    puts("                               %%%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%     %%%            ");
    puts("                             %%%%%%%%%%%%%%%%%%%%%%%%%%%%@ @%%%%%%%%%%%%%%%%%%        %%            ");
    puts("                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%                ");
    puts("                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%              ");
    puts("                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%@              ");
    puts("      %%%%%%%%@           %%%%%%%%%%%%%%%%        %%%%%%%%%%%%%%%%%%%%%%%%%%      %%                ");
    puts("    %%%%%%%%%%%%%         %%@%%%%%%%%%%%%           %%%%%%%%%%% %%%%%%%%%%%%      @%                ");
    puts("  %%%%%%%%%%   %%%        %%%%%%%%%%%%%%            %%%%%%%%%%%%%%%%%%%%%%%%                        ");
    puts(" %%%%%%%%%       %         %%%%%%%%%%%%%             %%%%%%%%%%%%@%%%%%%%%%%%                       ");
    puts("%%%%%%%%%@                % %%%%%%%%%%%%%            @%%%%%%%%%%%%%%%%%%%%%%%%%                     ");
    puts("%%%%%%%%@                 %%@%%%%%%%%%%%%            @%%%%%%%%%%%%%%%%%%%%%%%%%%%%                  ");
    puts("%%%%%%%@                   %%%%%%%%%%%%%%%           %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%              ");
    puts("%%%%%%%%%%                  %%%%%%%%%%%%%%%          %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      %%%%  ");
    puts("%%%%%%%%%@                   @%%%%%%%%%%%%%%         %%%%%%%%%%%%@ %%%% %%%%%%%%%%%%%%%%%   %%%%%%%%");
    puts("%%%%%%%%%%                  %%%%%%%%%%%%%%%%%        %%%%%%%%%%%%%      %%%%%%%%%%%%%%%%%% %%%%%%%%%");
    puts("%%%%%%%%%@%%@                %%%%%%%%%%%%%%%%@       %%%%%%%%%%%%%%     %%%%%%%%%%%%%%%%%%%%%%%%  %%");
    puts(" %%%%%%%%%%                  % %%%%%%%%%%%%%%@        %%%%%%%%%%%%%%   %%%%%%%%%%%%%%%%%%%%%%%%%% %%");
    puts("  %%%%%%%%%%%%  @           %%%%%%%%%%%%%%%%%%        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  %%% ");
    puts("   %%%%%%%%%%%%% %%  %  %@ %%%%%%%%%%%%%%%%%%          %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    %%% ");
    puts("    %%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%           @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    %%%%%%% ");
    puts("     %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%              %%%%%%%%%%%%%%%%%%%%%%%%%%%%        %%%   ");
    puts("      @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                  %%%%%%%%%%%%%%%%%%%%%%%%%               ");
    puts("        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                      %%%%%%%%%%%%%%%%%%%  %%%%%%%          ");
    puts("           %%%%%%%%%%%%%%%%%%%%%%%%%%                           %%%%%%%%%%%%%%%  @%%%%%%%%%         ");
    puts("              %%%%%%%%%%%%%%%%%%%%           @%@%                  @%%%%%%%%%%%%%%%%%%   %%%        ");
    puts("                  %%%%%%%%%%%%%%%        %%%%%%%%%%                    %%%%%%%%%%%%%%%    %         ");
    puts("                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                      %%%%%%%%%%%%%%            ");
    puts("                %%%%%%%%%%%%%%%%%%%%%%%%%%  %%%% %%%                      %%%%%%%%%%  %%%@          ");
    puts("                     %%%%%%%%%%%%%%%%%%% %%%%%% %%                          %%%%%%%%%%%%%@          ");
    puts("                                                                                 %%%%%%%@        ");
}

