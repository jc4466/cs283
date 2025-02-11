1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**: fgets() is a good choice because it doesn't require you to free the buffer like readline does. However unlike readline(), you can't enter a prompt so we must print "dsh>" beforehand.

2. You needed to use `malloc()` to allocate memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Allocating memory is better since the cmd prompt will vary in size each iteration.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  When we use execve() we must be exact. Trimming command line is part of parsing in any program.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**: Examples:
        `ls > files.txt'
        `wc < some_story.txt`
        `sort < names.txt`
        Challenges that may arise when implementing them are not using the correct file descriptors. I implemented them in another class but the redirection wasn't too bad.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection is changing the stdin or stdout file descriptor to an actual file (often a .txt). Piping is a form of inner-process communication (IPC) where two processes can communicate through a half-duplex pipe that the kernel owns. Piping is used to redirect the output of one process to the input of another in shells.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  When we pipe or redirect, it is important to distinct the two because we wouldn't want errors to go into out file, because we wouldn't see the error right away. Keeping STDERR print out to the screen helps us diagnose issues.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**: Our custom shell should print to stderr if a command fails. It is best not to merge the two command outputs.