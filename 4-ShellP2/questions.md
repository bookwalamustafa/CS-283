1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

   > **Answer**: We use `fork()` before `execvp()` because `execvp()` replaces the current process with the new program, meaning if we called it directly, our shell would disappear. `fork()` creates a child process that runs the new program, while the parent shell stays alive to handle more commands.
   >
2. What happens if the fork() system call fails? How does your implementation handle this scenario?

   > **Answer**:  If `fork()` fails, it means the system couldn't create a new process. In my implementation, I check if `fork()` returns -1, and if it does, I print an error message using `perror("fork")` and return an error code.
   >
3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

   > **Answer**:  `execvp()` searches for the command in directories listed in the `PATH` environment variable. It goes through eaech directory in `PATH`, looking for an executable file with the given name.
   >
4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

   > **Answer**:  `wait()` makes sure the parrent shell waits for the child process to finish before continuing. If we don't call it, the child process would run in the background, and we could end up spawning too many processes, leading to "zombie" processes.
   >
5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

   > **Answer**:  `WEXITSYAYUS()` extracts the exit code of the child process after it finishes. This is important because it helps us check if a command ran successfully or failed, so we can handle errors properly.
   >
6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

   > **Answer**:  It keeps spaces inside quotes instead of treating them as argument separators. This is necessary so commands like `echo "hello world"` are treated as a single arguemnt instead of splitting into multiple words.
   >
7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

   > **Answer**:  I changed the parsing ogic to properly handle qupted strings and remove extra spaces. The hardest part was making sure the quotes were removed from the arguments while still keeping spaces inside them intact.
   >
8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

  > **Answer**:  Signals let processes send mnotifications to each other to handle things like termination, stopping, or user interruptions. Unlike pipes or shared memory, signals are asynchronous - they can interrupt a process at any time.
  >
- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

  > **Answer**:
  >

  - SIGKILL(9): Immediately terminates a process, cannot be ignored or caught. Used when a process is unresponsive.
  - SIGTERM (15): Politely asks a process to terminate, allowing it to clean up first. Preferred over `SIGKILL`.
  - SIGINT (2): Sent when the user presses `CTRL+C`, usually to stop a running program.
- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

  > **Answer**:  `SIGSTOP` pauses until it gets `SIGCONT`. Unlike `SIGINT`, it cannot be caught or ignored because it's designed to be enforced by the system.
  >
