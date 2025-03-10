1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

**Ans -** After forking, my shell invokes waitpid() on every child process. This ensures that a pipeline's commands all complete before the shell continues. Zombie processes would be left running if I neglected to call waitpid(), eating up system resources and maybe leading to problems in the future.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

**Ans -** In addition to preventing resource leaks, closing empty pipe ends also keeps processes from hanging. Because read() believes there is still data coming in while pipes remian open, it may never return EOF, freezing requests.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

**Ans -** Since changing directories impacts the shell process rather than a child process, cd is build-in. The shell would remain in the same directory if cd were external; it would only alter the child process's directory.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

**Ans -** To expand the command list as necessary, I woud use realloc() in conjuction with dynamic memory allocation. Increased complexity and possible memory fragmentation are the trade-offs, although the hard restriction on pipes would be eliminated.
