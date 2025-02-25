1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

> **Answer**: I loop through the number of child processes and call `wait(NULL)`. I do not care about the pid's but I know my process will "see" all of it's children in the zombie state if I call wait() the number of child processes I created.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

> **Answer**: Leaving pipes open causes undefined behavior. Pipes send 'EOF' if the write end closes and there is no data in the pipe. This is needed for some program that read from the pipe.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

> **Answer**: If cd was an external command, the child process would not be able to change the directory of the parent process.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

> **Answer**: My shell does support an arbitrary number of piped commands currently. I dynamically allocate the pipes. Trade-offs are risk of memory leaks if not closed.
