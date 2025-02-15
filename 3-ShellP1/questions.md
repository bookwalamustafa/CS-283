1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

   > **Answer**:  `fgets()` is great for reading user input in our shell because it safely reads an entire line, including spaces, until a newline (`\n`) or the buffer size limit is reached. Unlike `b`, it doesn’t split words, and unlike `gets()`, it prevents buffer overflow, making it a safer choice.
   >
2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

   > **Answer**:  We used `malloc()` to dynamically allocate memory for `cmd_buff` so our shell can handle different input sizes flexibly. A fixed-size array could waste memory or be too small for long commands. With `malloc()`, we can allocate exactly what we need and free it when done.
   >
3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

   > **Answer**:  Trimming spaces is necessary to avoid issues like empty commands or unexpected behavior. For example, if a user types `" ls "`, without trimming, the shell might misinterpret it as an invalid command. Spaces before and after pipes (`|`) could also cause parsing errors.
   >
4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

  > **Answer**:  Three examples of redirection and challenges in implementing them -
  >

  - Redirect output to a file -> ls > output.txt (Saves output to a file)
  - Redirect errors to a file -> ls non_existing_dir 2> errors.text (Saves errors separately)
  - Merge STDOUT and STDERR -> command > output.txt 2>&1 (Combines both into one file)

    **Challenges:** Our shell needs to correctly detect >, 2<, and 2>&1, open the correct file, and redirect file descriptors properly using dup2(). Handling different redirection types without breaking normal command execution is tricky.
- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

  > **Answer**:  Redirection (`>`, `b`, `2>`) *sends input or output to a file , while piping* (`|`) passes the output of one command as input to another. Redirection stores results, while piping connects processes in real-time.
  >
- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

  > **Answer**:  Keeping errors separate from normal output helps avoid confusion. If errors and regular output mix, it’s hard to distinguish success from failure. It also allows users to handle errors differently, like saving only errors to a file (`2> errors.txt`).
  >
- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

  > **Answer**:  Our shell should detect failed commands and return an appropriate error message (e.g., `"command not found"`). If a command outputs both STDOUT and STDERR, we should let users decide whether to merge them (`2>&1`). We can implement this by handling `dup2()` for redirection.
  >
