#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>

#include "dshlib.h"

/* return true if command is found, either:
 *   - a valid fully qualified path was supplied to an existing file
 *   - the executable file was found in the system's PATH
 * false is returned otherwise */
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
