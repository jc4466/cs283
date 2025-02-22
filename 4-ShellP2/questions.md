1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  If we call `execvp` directly, the process that is running the shell will have its virtual memory wiped and will run the exec'd command. So it will exit after and our shell will be gone. When we fork we create another process, so when the exec'd function return, our shell will be safe since it is a separate process.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the fork() fails, the program will print to stderr and the command will not be executed.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  `execvp()` searches through the PATH environment variable. This is a list of colon separated directory pathnames and it will search for the command in there.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  If we didn't call `wait()`, then our shell process may be scheduled before the exec'd process so it may prompt the user for another input. There would be undefined behavior since we can't predict CPU scheduling.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  The `wait()` parameter (e.g. `wait(&status)`) stores an int that contains info. The `WEXITSTATUS()` will strip the int and give the exit status of the child's process.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**: I did not implement build_cmd_buff(). I used a parser from another class.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**: I didn't make any changes. My parsing logic in the previous assignment gave me all the tools to complete this one.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**: Signals differ from IPC because a process will receive a signal as soon as it is scheduled on the CPU. Each signal has a default action and if it is not masked, or handled differently, that signal will execute the action. 

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL will terminate a process immediately. This signal cannot be masked. SIGINT is when the user presses `CTRL+C`. This will stop the running process in the terminal immediately. This is used for foreground process when you cannot "kill" a process in the terminal. SIGCHILD is sent to the parent process whenever one of its child changes state. The default for this is to ignore it.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP it will immediately be stopped until it receives SIGCONT. This process cannot execute a custom handler unlike SIGINT. Also it doesn't terminate a process just stops it.