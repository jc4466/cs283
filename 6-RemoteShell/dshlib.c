#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"
#include "builtin.h"
#include "parse.h"

/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

 static int command_found(const char *cmd)
{
    char *dir;
    char *tmp;
    char *PATH;
    char *state;
    char probe[PATH_MAX];

    int ret = 0;

    if (access(cmd, X_OK) == 0)
        return 1;

    PATH = strdup(getenv("PATH"));

    for (tmp=PATH; ; tmp=NULL) {
        dir = strtok_r(tmp, ":", &state);
        if (!dir)
            break;

        strncpy(probe, dir, PATH_MAX-1);
        strncat(probe, "/", PATH_MAX-1);
        strncat(probe, cmd, PATH_MAX-1);

        if (access(probe, X_OK) == 0) {
            ret = 1;
            break;
        }
    }

    free(PATH);
    return ret;
}

static int check_input(Parse *P){
    int t;
    for (t = 0; t < P->ntasks; t++) {
        if(!command_found(P->tasks[t].cmd) && !is_builtin(P->tasks[t].cmd)){
            fprintf(stderr, "Command not found in PATH\n");
            return 0;
        } else if(!strcmp(P->tasks[t].cmd, "exit")){
            exit(EXIT_SUCCESS);
        }
    }
    return 1;
}

static int get_infile(Parse *P){
    static int fdin;
    if(P->infile){
        fdin = open(P->infile, O_RDONLY);
        if (fdin < 0)
            fprintf(stderr, "Failed opening infile\n");
    } else {
        fdin = STDIN_FILENO;
    }
    return fdin;
}

static int get_outfile(Parse *P){
    static int fdout;
    if(P->outfile){
        fdout = open(P->outfile, O_WRONLY | O_CREAT, 0664);
        if (fdout < 0)
            fprintf(stderr, "Failed opening outfile\n");
    } else {
        fdout = STDOUT_FILENO;
    }
    return fdout;
}

// safely closes a file descriptor
void close_safe(int fd){
    if ((fd != STDIN_FILENO) && (fd != STDOUT_FILENO))
        close(fd);
}

void change_out_fd(int fdout){
    // reroute STDOUT to outfile fd (fdout)
    if (!(fdout == STDOUT_FILENO))
        if (dup2(fdout, STDOUT_FILENO) == -1)
            fprintf(stderr, "Failed to dup()\n");
}

void change_in_fd(int fdin){
    // reroute STDIN to infile fd (fdin)
    if(!(fdin == STDIN_FILENO))
        if (dup2(fdin, STDIN_FILENO) == -1)
            fprintf(stderr, "Failed to dup()\n");
}

// run builtin task or exec
static void run(Task *T, int in, int out){
    change_in_fd(in);
    change_out_fd(out);

    if(is_builtin(T->cmd)){
        builtin_execute(T);
    } else {
        execvp(T->cmd, T->argv);
    }
}


/* Called upon receiving a successful parse.
 * This function is responsible for cycling through the
 * tasks, and forking, executing, etc as necessary to get
 * the job done! */
void execute_tasks(Parse *P)
{
    int t;
    pid_t *pid;
    int fd[2];

    int fdin = get_infile(P);
    int fdout = get_outfile(P);

    pid = malloc(P->ntasks * sizeof(*pid));

    int ret = check_input(P);
    if(!ret)
        return;

    for (t = 0; t < P->ntasks - 1; t++) {
        
        // make a pipe, except on last iteration
        if( t < (P->ntasks-1)){
            if (pipe(fd) == -1){
                fprintf(stderr, "failed to create pipe\n");
                return;
            }
        }

        pid[t] = fork();
        if (pid[t] < 0)
            fprintf(stderr, "Failed forking\n");
        
        if (!pid[t]){
            /* CHILD PROCESS */
            close_safe(fd[0]);
            run(&P->tasks[t], fdin, fd[1]);
        }

        close_safe(fd[1]);
        close_safe(fdin);

        fdin = fd[0];
    }

    fdout = get_outfile(P);

    pid[t] = fork();

    if(!pid[t]){
        run(&P->tasks[t], fdin, fdout);
    }

    close_safe(fdin);
    close_safe(fdout);

    // Parent waits for all child processes
    //      down here because if child process is trying to write
    //      to a full pipe, it will block, meanwhile the parent
    //      will create the next child to read the pipe, instead
    //      of waiting for 1 child at a time.
    
    // remove for project 2, use SIGCHILD
    for (t = 0; t < P->ntasks; t++) {
        wait(NULL);
    }

    free(pid);        
}


/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{

    // THIS CODE SHOULD BE THE SAME AS PRIOR ASSIGNMENTS
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
   
    return OK;
}
