#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */

char *trim(char *str) {
    while (isspace((unsigned char)*str)) str++; 
    if (*str == '\0') return str;              

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0'; 

    return str;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    char *cmd_token, *arg_token;
    char *cmd_saveptr, *arg_saveptr;
    int cmd_count = 0;

    memset(clist, 0, sizeof(command_list_t)); 

    cmd_token = strtok_r(cmd_line, PIPE_STRING, &cmd_saveptr);
    while (cmd_token != NULL) {
        cmd_token = trim(cmd_token);

        if (cmd_count >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        if (strlen(cmd_token) == 0) {
            return WARN_NO_CMDS;
        }

        arg_token = strtok_r(cmd_token, " ", &arg_saveptr);
        if (arg_token == NULL) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        if (strlen(arg_token) >= EXE_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        strcpy(clist->commands[cmd_count].exe, arg_token);

        char *arg_ptr = strtok_r(NULL, "", &arg_saveptr);
        if (arg_ptr) {
            arg_ptr = trim(arg_ptr);
            if (strlen(arg_ptr) >= ARG_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            strcpy(clist->commands[cmd_count].args, arg_ptr);
        } else {
            clist->commands[cmd_count].args[0] = '\0';
        }

        cmd_count++;
        cmd_token = strtok_r(NULL, PIPE_STRING, &cmd_saveptr);
    }

    clist->num = cmd_count;
    return (cmd_count > 0) ? OK : WARN_NO_CMDS;
}