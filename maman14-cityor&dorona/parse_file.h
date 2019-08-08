#ifndef PARSE_FILE_H
#define PARSE_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define OP_CODE 16
#define AS_FILE ".as"
#define LINE_LENGTH 31
#define FIRST_ADDRESS 100
#define NO 0
#define YES 1

/* extra*/
#define KRED "\x1B[31m"

typedef enum{OUT_OF_MEMORY,LABEL_NOT_DECLARED,UNDEFIEND_COMMAND,UNDEFIEND_PARAMETER,UNDEFIEND_REGISTER,
MISSING_PARAMETER,MISSING_COMMA,TOO_MANY_COMMAS,ARGUMENT_ILLEGAL,
SAME_COMMAND_NAME,TOO_MANY_OPERANDS,TOO_MANY_PARAMETERS,LABEL_ALREADY_EXISTS,ECXESSIVE_TEXT}error_text;

typedef struct error_info{
       int type; /* The type of the error text by error_text*/
       int line; /* The number of the line in the file*/
}error_info;

typedef struct error_node *error_p;
typedef struct error_node{
         error_info data;
         error_p next;
}error_node;

typedef struct command *cmd_p;
typedef struct command{
        char *name; /*option label*/
        int opCode;
        int operand;/*opearnd source/destiniton*/
        cmd_p next;
}command;

/*------------------------------------------------------------------------------------------------------------ */
/*------------------------------------------------------------------------------------------------------------ */



/*
--------------------------------
function from parse_file.c
--------------------------------
*/

int add_err(error_p *head, int error_text, int line);

void free_err_list(error_p *err_head);

void print_err_list(error_p *err_head);

int skip_spaces(const char *line);

int check_label(const char *word,error_p *err_head,int line);

int check_data(const char *word);

command *check_inst(const char *word);
int check_arr(char *word,error_p *err_head,int line);

int check_comma(const char *line, error_p *err_head, int _line);

int check_param(char *param, error_p *err_head, int line);

int check_end(const char *word);

FILE *open_file(char *name, char *end,const char *temp);

int check_arr(char *word,error_p *err_head,int line);
#endif /* PARSE_FILE_H */
