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

char *trim_whitespace(char *str) {
    while (*str == ' ' || *str == '\t' || *str == '\n') str++;
    if (*str == '\0') return str;

    char *end = str + strlen(str) - 1;
    while (*end == ' ' || *end == '\t' || *end == '\n') end--;
    
    *(end + 1) = '\0';
    return str;
}


void remove_surrounding_quotes(char *str) {
    int len = strlen(str);
    if (len >= 2 && str[0] == '"' && str[len - 1] == '"') {
        for (int i = 0; i < len - 2; i++) {
            str[i] = str[i + 1];
        }
        str[len - 2] = '\0';
    }
}

void handle_redirection(char **argv) {
    for (int i = 0; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "<") == 0) {
            if (argv[i+1] == NULL) {
                fprintf(stderr, "No input file specified\n");
                exit(1);
            }
            int fd_in = open(argv[i+1], O_RDONLY);
            if (fd_in < 0) {
                perror("open input file");
                exit(1);
            }
            if (dup2(fd_in, STDIN_FILENO) == -1) {
                perror("dup2 input");
                exit(1);
            }
            close(fd_in);
    
            int j = i;
            while (argv[j+2] != NULL) {
                argv[j] = argv[j+2];
                j++;
            }
            argv[j] = NULL;
            argv[j+1] = NULL;
            i--;  
        } else if (strcmp(argv[i], ">>") == 0) {
            if (argv[i+1] == NULL) {
                fprintf(stderr, "No output file specified for append\n");
                exit(1);
            }
            int fd_out = open(argv[i+1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd_out < 0) {
                perror("open output file for append");
                exit(1);
            }
            if (dup2(fd_out, STDOUT_FILENO) == -1) {
                perror("dup2 output for append");
                exit(1);
            }
            close(fd_out);
            int j = i;
            while (argv[j+2] != NULL) {
                argv[j] = argv[j+2];
                j++;
            }
            argv[j] = NULL;
            argv[j+1] = NULL;
            i--; 
        } else if (strcmp(argv[i], ">") == 0) {
            if (argv[i+1] == NULL) {
                fprintf(stderr, "No output file specified\n");
                exit(1);
            }
            int fd_out = open(argv[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out < 0) {
                perror("open output file");
                exit(1);
            }
            if (dup2(fd_out, STDOUT_FILENO) == -1) {
                perror("dup2 output");
                exit(1);
            }
            close(fd_out);
            int j = i;
            while (argv[j+2] != NULL) {
                argv[j] = argv[j+2];
                j++;
            }
            argv[j] = NULL;
            argv[j+1] = NULL;
            i--;  
        }
    }
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    clist->num = 0;
    char *saveptr;

    char *token = strtok_r(cmd_line, PIPE_STRING, &saveptr);
    while (token != NULL) {
        token = trim_whitespace(token);
        if (strlen(token) == 0) {
            return WARN_NO_CMDS;
        }
        if (clist->num >= CMD_MAX) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            return ERR_TOO_MANY_COMMANDS;
        }
        cmd_buff_t *cmd = &clist->commands[clist->num];
        cmd->_cmd_buffer = strdup(token);
        if (cmd->_cmd_buffer == NULL) {
            return ERR_MEMORY;
        }
        
        char *inner_saveptr;
        char *arg = strtok_r(cmd->_cmd_buffer, " ", &inner_saveptr);
        int argc = 0;
        while (arg != NULL && argc < CMD_ARGV_MAX - 1) {
            if (arg[0] == '"' && arg[strlen(arg) - 1] != '"') {
                char combined[ARG_MAX] = "";
                strcat(combined, arg);
                while ((arg = strtok_r(NULL, " ", &inner_saveptr)) != NULL) {
                    strcat(combined, " ");
                    strcat(combined, arg);
                    if (arg[strlen(arg) - 1] == '"')
                        break;
                }
                remove_surrounding_quotes(combined);
                cmd->argv[argc] = strdup(combined);
                if (cmd->argv[argc] == NULL)
                    return ERR_MEMORY;
                argc++;
            } else {
                remove_surrounding_quotes(arg);
                cmd->argv[argc] = arg;
                argc++;
            }
            arg = strtok_r(NULL, " ", &inner_saveptr);
        }
        cmd->argv[argc] = NULL;
        cmd->argc = argc;
        clist->num++;
        token = strtok_r(NULL, PIPE_STRING, &saveptr);
    }
    if (clist->num == 0) {
        return WARN_NO_CMDS;
    }
    return OK;
}

int free_cmd_list(command_list_t *cmd_lst) {
    for (int i = 0; i < cmd_lst->num; i++) {
        free(cmd_lst->commands[i]._cmd_buffer);
        cmd_lst->commands[i]._cmd_buffer = NULL;
    }
    cmd_lst->num = 0;
    return OK;
}

int execute_pipeline(command_list_t *clist) {
    int in_fd = STDIN_FILENO; 
    int pipe_fd[2];
    pid_t pid;
    int final_status = OK; 
    
    for (int i = 0; i < clist->num; i++) {
        if (i < clist->num - 1) {
            if (pipe(pipe_fd) == -1) {
                perror("pipe");
                return ERR_EXEC_CMD;
            }
        }
        
        pid = fork();
        if (pid < 0) {
            perror("fork");
            return ERR_EXEC_CMD;
        }
        
        if (pid == 0) {
            if (in_fd != STDIN_FILENO) {
                if (dup2(in_fd, STDIN_FILENO) == -1) {
                    perror("dup2 in_fd");
                    exit(1);
                }
                close(in_fd);
            }
            if (i < clist->num - 1) {
                if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
                    perror("dup2 pipe_fd[1]");
                    exit(1);
                }
                close(pipe_fd[0]);
                close(pipe_fd[1]);
            }

            handle_redirection(clist->commands[i].argv);

            if (execvp(clist->commands[i].argv[0], clist->commands[i].argv) == -1) {
                perror("execvp");
                exit(ERR_EXEC_CMD);
            }
        } else { 
            if (in_fd != STDIN_FILENO) {
                close(in_fd);
            }

            if (i < clist->num - 1) {
                close(pipe_fd[1]); 
                in_fd = pipe_fd[0];
            }

            int child_status;
            waitpid(pid, &child_status, 0);
            if (WIFEXITED(child_status)) {
                int exit_code = WEXITSTATUS(child_status);
                if (exit_code != 0) {
                    final_status = exit_code;
                }
            }
        }
    }
    return final_status;
}

int exec_local_cmd_loop() {
    char cmd_line[SH_CMD_MAX];
    int ret;
    int last_status = OK;
    command_list_t clist;

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_line, sizeof(cmd_line), stdin) == NULL) {
            printf("\n");
            break;
        }

        cmd_line[strcspn(cmd_line, "\n")] = '\0';
        char *trimmed_line = trim_whitespace(cmd_line);

        if (strlen(trimmed_line) == 0) {
            printf("%s", CMD_WARN_NO_CMD);
            last_status = WARN_NO_CMDS; 
            continue;
        }

        if (strcmp(trimmed_line, EXIT_CMD) == 0) {
            printf("exiting...\n");
            break;
        }

        ret = build_cmd_list(trimmed_line, &clist);
        
        if (ret == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            last_status = ret;
            continue;
        }

        ret = execute_pipeline(&clist);
        if (ret == ERR_EXEC_CMD) {
            fprintf(stderr, "Error executing pipeline\n");
            last_status = ret;
        }

        free_cmd_list(&clist);
        last_status = ret;
    }

    return last_status;
}