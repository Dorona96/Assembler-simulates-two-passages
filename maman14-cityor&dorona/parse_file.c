#include "parse_file.h"
#include "first_tran.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define KRED "\x1B[31m"


command op_code[OP_CODE] = {
                            {"mov", 0, 2},
                            {"cmp", 1, 2},
                            {"add", 2, 2},
                            {"sub", 3, 2},
                            {"not", 4, 1},
                            {"clr", 5, 1},
                            {"lea", 6, 2},
                            {"inc", 7, 1},
                            {"dec", 8, 1},
                            {"jmp", 9, 1},
                            {"bne", 10, 1},
                            {"red", 11, 1},
                            {"prn", 12, 1},
                            {"jsr", 13, 1},
                            {"rts", 14, 0},
                            {"stop", 15, 0}
 
};/*name of the command, value of code, operand*/

extern int IC,DC;
extern const char *data_arr[];
extern const char *reg_arr[];

typedef enum{STRING,DATA}data_type;

typedef enum{LBL_LINE = 0,NUM_LINE = 1,INST_LINE = 2,ERR_LINE = 3}line_opt;
typedef enum {A, E, R} ARE;

typedef enum{MOV,CMP,ADD,SUB,NOT,CLR,LEA,INC,DEC,JMP,BNE,RED,PRN,JSR,RTS,STOP}op_code_dec;


typedef enum {NONE_PRM = 0, NUM_PRM = 0, LBL_PRM = 1,ARR_PRM=2, REG_PRM = 3}prm_type;

FILE *open_file(char *name,char *last, const char *temp)
{
  FILE *ptr;
  char *buff= (char *)malloc(strlen(name)+strlen(AS_FILE));
   if(!buff)
   {
     printf("%sThere is'nt allocate \n",KRED);
     return NULL;
   }
 
  strcpy(buff,name);
  strcat(buff,last);

ptr = fopen(buff,temp);
free(buff);

return ptr;
}

int skip_spaces(const char *line)
{
    int i = 0;
    while(line[i] == '\t' || line[i] == ' ')
    {
        i++;
    }
 return i;
}


/* ---------- function for error list -------- */

int add_err(error_p *err_head, int error_text, int line)
{
   error_p newNode;
   error_p temp;

   newNode = (error_p)malloc(sizeof(error_node));
   if(newNode == NULL)
   {
      printf("%sno memory allocation, the assigment failed\n",KRED);
     exit(0);
   }
   newNode->data.type = error_text;
   newNode->data.line = line;
   newNode->next = NULL;
  
  if(*err_head == NULL)
  {
    *err_head = newNode;
  }else{
    temp = *err_head;
    while(temp->next != NULL)
    {
       temp = temp->next;
    }
   temp->next = newNode;
  }/*end of loop*/

return 1;
}

void free_err_list(error_p *err_head)
{
  error_p curr= (error_p)malloc(sizeof(error_node));
  while(*err_head)
  {
     curr = (*err_head)->next;
     free(*err_head);
    *err_head = curr;
  }
}

void print_err_list(error_p *err_head)
{
  error_p curr;
  curr = *err_head;
  
  while(curr)
  {
    switch(curr->data.type)
     {
      case OUT_OF_MEMORY: 
     {
        printf("%sError!There is no more free space, file is too long from line %d.\n",KRED,curr->data.line);
       break;
     }
     case UNDEFIEND_PARAMETER:
    {
       printf("%sError! The parameter you enters is undefiend in line %d\n",KRED,curr->data.line);
    } 
     case MISSING_PARAMETER:
    {
       printf("%sError!missing parameter in the command in line %d.\n",KRED,curr->data.line);
      break;
    }

    case TOO_MANY_PARAMETERS:
    {
     printf("%sError! There's to many parameters in line %d.\n",KRED,curr->data.line);
    break;
   }
     case UNDEFIEND_COMMAND:
    {
       printf("%sError!The commnand you enterd is undefined in line %d\n",KRED,curr->data.line);
      break;
    }
    case SAME_COMMAND_NAME:
    {
      printf("%sThere's already command in the same name, please check it, in line %d\n",KRED,curr->data.line);
     break;
    }
     case MISSING_COMMA:
    {
       printf("%sError! missing comma between the parameters, please add ',' inline %d\n",KRED,curr->data.line);
      break;
    }
     case TOO_MANY_COMMAS:
    {
      printf("%sError! Theere is too many comma between parameters,  please delet ',,' int line %d\n",KRED,curr->data.line);
     break;
    }
     case ARGUMENT_ILLEGAL:
    {
      printf("%sThe argument does not match the incstruction in line %d\n",KRED,curr->data.line);
     break;
    }
    case TOO_MANY_OPERANDS:
    {
      printf("%sError! There's to many operands in line %d.\n",KRED,curr->data.line);
    break;
   }
   case LABEL_ALREADY_EXISTS:
   {
     printf("%sError! There label you wanted to add is already exists in the list in line %d\n",KRED,curr->data.line);
   }

     }/* end of switch*/
  }/*end of loop*/
}/*ent of program */



int check_label(const char *word,error_p *err_head,int line)
{
  int i = 0;
  if(isalpha(word[i]))
  {
    while(isdigit(word[i]) || isalpha(word[i]))
    {
       i++;
    }
  }
  if(MAX_LABEL_LENGTH < i)
  {
    return add_err(err_head,UNDEFIEND_COMMAND,line);
  }
return i;
}
/*function that check which type the data option, if it does match - 
  return where is it in the array (by index)*/
int check_data(const char *word)
{
 int i = 0;
 while(data_arr[i])
  {
    if(!strcmp(word,data_arr[i]))
     {
        return i;
     }
   i++;
  }
 return i; /*The index in the array */
}

/*The function check if the word is a label
by comparing the word with each label from the commad's list
return the op code from the list OR return larger number*/

command *check_inst(const char *word)
{
 int i =0;

  while(i < OP_CODE)
  {
    if(!strcmp(word,op_code[i].name))
     {
      return &op_code[i];
     }
   i++;
  }
 
  return NULL;
}
    
/*function that check if there's commas in the line(between the arguments (for exmple : HELLO -5,4,12,-8) if the comma is the first parameter than error.
return 1 for YES and 0 for NO*/
int check_comma(const char *line, error_p *err_head, int line_A)
{
 if(*line == ',')
 {
    add_err(err_head,MISSING_PARAMETER,line_A);
   return 1;
 }
  line++;

  while(*line)
  {
   if(*line == ',')
   {
    /*if the command line receives only a comma*/
     if(*(line+1) == '\0') 
     {
        add_err(err_head,MISSING_PARAMETER,line_A);
     }
    /*if there double comma in the command line */
     if(*(line+1) == ',')
     {
        add_err(err_head,TOO_MANY_COMMAS,line_A);
     }
   return 1;
  }/*end of if */
  line++;
 }/*end of loop */


/*if we arrived here and still didn't return nothing so there isn't commas in the line */

return 0;

}

/*function that check if the parameter in the line is lligeal
if lligeal return 1, otherwise return 0
lligeal parameter (digit) can start with '+' or '-'*/
int check_param(char *param, error_p *err_head, int line)
{

 if(*param == '\n' || *param == '\0' || *param == EOF)
 {
     return 0;
 }
 if(*param == '+' || *param == '-')
 {
    param++;
 }
  while(*param)
  {
   if(isdigit(*param))
   {
       param++;
   }else{
      add_err(err_head,UNDEFIEND_PARAMETER,line);
       return 0;
      }  
  }/*end of loop */
 return 1;
}
/*function that check the end of the line, if that line does not end return 0,
if it's end OK return 1*/
int check_end(const char *word)
{
int i = 0;
 while(isalpha(word[i]) || isdigit(word[i]))
  {
    i++;
  }
 i += skip_spaces(word+i);

  if(word[i] == '\n')
  {
     return 0;
  }
  else
  {
     return 1;
  }
 return i;
}/*end of func */

int check_arr(char *word,error_p *err_head,int line)
{return 0;}
