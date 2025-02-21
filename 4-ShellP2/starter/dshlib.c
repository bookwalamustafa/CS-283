#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

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

// Removes leading & trailing spaces from a string
char *trim_whitespace(char *str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;  

    char *end = str + strlen(str) - 1;
    while (end > str && *end == ' ') end--;

    *(end + 1) = '\0';
    return str;
}

// Parses input into the cmd_buff_t structure
void parse_input(char *input, cmd_buff_t *cmd) {
    cmd->argc = 0;
    bool in_quotes = false;
    char *ptr = input;

    while (*ptr) {
        while (*ptr == ' ' && !in_quotes) ptr++;

        if (*ptr == '\0') break;

        if (*ptr == '"') { 
            in_quotes = !in_quotes;
            ptr++;
        }

        cmd->argv[cmd->argc++] = ptr;

        while (*ptr && (in_quotes || (*ptr != ' ' && *ptr != '\n'))) {
            if (*ptr == '"') {  
                in_quotes = !in_quotes;
                *ptr = '\0';
                ptr++;
                break;
            }
            ptr++;
        }

        if (*ptr == ' ' || *ptr == '\0') {
            *ptr = '\0';
            ptr++;
        }

        if (cmd->argc >= CMD_ARGV_MAX - 1) break;
    }

    cmd->argv[cmd->argc] = NULL;
}

// Handles built-in commands like cd and exit
int handle_builtin(cmd_buff_t *cmd) {
    if (cmd->argc == 0) return BI_NOT_BI; 

    if (strcmp(cmd->argv[0], "exit") == 0) {
        exit(0); 
    }

    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc > 1) {
            if (chdir(cmd->argv[1]) != 0) {
                perror("cd"); 
            }
        }
        return BI_EXECUTED;
    }

    return BI_NOT_BI; 
}

// Executes external commands using fork & execvp
int execute_external(cmd_buff_t *cmd) {
    pid_t pid = fork(); 

    if (pid == 0) { 
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp");  
        exit(ERR_EXEC_CMD);  
    } else if (pid > 0) {  
        int status;
        waitpid(pid, &status, 0); 
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 0) {
                return OK;
            }
            return ERR_EXEC_CMD;
        }
    } else {
        perror("fork");  
        return ERR_EXEC_CMD;
    }

    return OK; 
}


int exec_local_cmd_loop()
{
    // char *cmd_buff;
    int rc = OK;
    cmd_buff_t cmd;

    // TODO IMPLEMENT MAIN LOOP

    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"

    char input[SH_CMD_MAX]; 

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(input, SH_CMD_MAX, stdin) == NULL) {
            printf("\n"); 
            break;
        }

        input[strcspn(input, "\n")] = '\0'; 
        char *trimmed_input = trim_whitespace(input);
        
        if (strlen(trimmed_input) == 0) {
            printf(CMD_WARN_NO_CMD);
            continue; 
        }

        parse_input(trimmed_input, &cmd);

        if (handle_builtin(&cmd) == BI_NOT_BI) {
            rc = execute_external(&cmd); 
        }
    }

    return rc;
}