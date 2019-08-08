#include "check_file.h"
#include "second_tran.h"
#include "first_tran.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

extern int IC,DC;
extern const char *data_arr[];
extern const char *reg_arr[];
extern command op_code[OP_CODE];
extern enum{STRING,DATA}data_type;
extern enum{LBL_LINE = 0,NUM_LINE = 1,INST_LINE = 2,ERR_LINE = 3}line_opt;
extern enum {A, E, R} ARE;
extern enum{MOV,CMP,ADD,SUB,NOT,CLR,LEA,INC,DEC,JMP,BNE,RED,PRN,JSR,RTS,STOP}op_code_dec;
extern enum {NONE_PRM = 0, NUM_PRM = 0, LBL_PRM = 1,ARR_PRM=2, REG_PRM = 3}prm_type;

void check_file(char *name)
{

  
/*var for saving the location in the file*/ 
int num = 1;
unsigned int IC = FIRST_ADDRESS;/*100 */
unsigned int DC = 0;
data_p curr;
/* make pointers for all the linked list*/
label_p lbl_head;
error_p err_head;
data_p data_head;
inst_p inst_head;

int extern_flag =  NO;
int entry_flag = NO;

char str[LINE_LENGTH]; /*/31 chars/ */

FILE *ptr = open_file(name,".as","r");
  if(!ptr)
  {
     printf("%sError! The file's name does not excist\n",KRED);
    return;
  }

 printf("%sFile \"%s.as\" is excist.\n",KYEL,name);
 
/*initialize the linked list */
 lbl_head = NULL;
 err_head = NULL;
 data_head = NULL;
 inst_head = NULL;

/*/loop fro read each line from the file/ */
  while(fgets(str,LINE_LENGTH,ptr) != NULL)
  {
     parse_line(str,num,&lbl_head,&err_head,&data_head,&inst_head,&IC,&DC,&extern_flag,
&entry_flag);
    num++;
   }
 if(err_head)
   {
      print_err_list(&err_head);
      return;
   }

   update_data_ads(&IC,&data_head);
   if(err_head)
   {
       print_err_list(&err_head);
       return;
   }
  
   curr = data_head;
   while(curr)
   {
       fin_data(curr);
       curr = curr->next;
   }
 
   update_label(&lbl_head,&IC);
   
   update_inst_list(&lbl_head,&err_head,&inst_head);

   if(err_head)
   {
       print_err_list(&err_head);
      return;
   }
  
/*  creat all the files */
   
  create_ob(&inst_head,&data_head,name,&DC,&IC);
  if(entry_flag)
   {
      create_ent(&lbl_head,name);
   }
  if(extern_flag)
   {
      create_ext(&inst_head,name);
   }
   fclose(ptr);
  
 /*free memory of all the lists label,data,instruction AND error */
    free_lists(&lbl_head,&data_head,&inst_head,&err_head);
    entry_flag = NO;
  
 }/*end of function */


void parse_line(char *line, int num,label_p *lbl_head,error_p *err_head,data_p *data_head,inst_p *inst_head,unsigned int *IC,unsigned int *DC, int *extern_flag, int *entry_flag)
{
 int i,w;
 char *label = NULL;
 label_info *curr = NULL;
 int sts = ERR_LINE;
 char first_word[MAX_LABEL_LENGTH];
 char sec_word[MAX_LABEL_LENGTH];
 command *cmd_p = NULL;

 /*/skip to the next line if the line is comment,empty, or it is end of the line/ */
 i = skip_spaces(line);
 
  if(line[i] == ';' || line[i] == '\n' || line[i] == EOF)
  {
    return;
  }
  /*/decide if the line declarition is from type data,inst or label/ */
  else if(sscanf(line+i,"%s",first_word))
  {
    if(first_word[check_label(first_word,err_head,num)] == ':')
    {
        sts = LBL_LINE;
    }
    else if(check_data(first_word))
    {
        sts = NUM_LINE;
    }
    else if(check_inst(first_word))
    { 
         sts = INST_LINE;
    }
  }
  
  switch(sts)
  {
   case LBL_LINE:
   {
    i += strlen(first_word);
    i += skip_spaces(line+i);

    /*/delet the next word/ */
     if(sscanf(line+i, "%s",sec_word))
     {
        w = check_data(sec_word);
     switch(w)
      {
      
        case 0: 
        {
          strcpy(first_word,strtok(first_word,":"));/*find the label in the label's list and update the address */
          if(curr == find(lbl_head,first_word))
           {


             if(curr->address == 0)
              {
                curr->address = *DC;
              parse_data(line+i+strlen(sec_word),w,err_head,data_head, num,IC,DC); 
               return;
              }
             else
              {
                add_err(err_head,LABEL_ALREADY_EXISTS,num);
                return;
              }
           }/*/end of if/    */
 /*/The label does not exists and we need to add a new label/ */
          else
          {
              add_label(lbl_head,label,*DC,0,0);
              parse_data(line+i+strlen(sec_word),w,err_head,data_head,num,IC,DC);
            return;
          }
     }/*/end of case 0/ */

    /*/.string/ */
     case 1: 
     {
       label = strtok(first_word,":");
 /*find the label in the label's list and update the address */
          if(curr == find(lbl_head,label))
           {
              if(curr->address == 0)
              {
                 curr->address = *DC;
              parse_data(line+i+strlen(sec_word),w,err_head,data_head, num,IC,DC); 
               return;
              }
             else
              {
                add_err(err_head,LABEL_ALREADY_EXISTS ,num);
                return;
              }
           }
 /*/The label does not exists and we need to add a new label/ */
          else
          {
              add_label(lbl_head,label,*DC,0,0);
              parse_data(line+i+strlen(sec_word),w,err_head,data_head,num,IC,DC);
             return;
          }
   
     }/*/end of case 1/ */

    /*/.entry/ */
     case 2:
     {
        parse_entry(lbl_head,err_head,num,line+i+strlen(sec_word),DC);
        *entry_flag = YES;
        return;
     }
    /*.extern*/
     case 3:
     {
        parse_extern(lbl_head,err_head,num,line+i+strlen(sec_word));
        *extern_flag = YES;
     return;
     }
   default:break;
  }/*end of switch(w) */
/*/if e arrived here, so the line must be a command/ */

if(cmd_p == check_inst(sec_word))
    {
     add_label(lbl_head,first_word,*IC,1,0);
      i += strlen(sec_word);
      i += skip_spaces(line+i);
      line += i;

  /*/add the command to the instruction list/ */
      add_inst(IC,DC,cmd_p->opCode,cmd_p->operand,line,err_head,inst_head,num);
      return;

    }/*end if*/
  }/*end of case LBL_LINE */   


  case NUM_LINE:
  {

   w = check_data(first_word);
  
   switch(w)
   {
  
 /* /.data/ */
    case 0: 
     {
        parse_data(line+i+strlen(first_word),w,err_head,data_head,num,IC,DC);
         return;
     }
/*/.string/ */
    case 1:
    {
        parse_data(line+i+strlen(first_word),w,err_head,data_head,num,IC,DC);
       return;
    }
  /*/.entry/ */
    case 2:
    {
       parse_entry(lbl_head,err_head,num,line+i+strlen(first_word),DC);
       *entry_flag = YES;
       return;
    }
 /* /.extern/ */
    case 3:
    {
       parse_extern(lbl_head,err_head,num,line+i+strlen(first_word));
       *extern_flag = YES;
       return;
    }
    default:break;
  
   }/*end of switch(w) */
 }/*end of case NUM_LINE*/
 case INST_LINE:
 {
   cmd_p = check_inst(first_word);
   i += strlen(first_word);
   i += skip_spaces(line + i);
   line += i;
   
  add_inst(IC,DC,cmd_p->opCode,cmd_p->operand,line,err_head,inst_head,num);
   return;
  }
 } 
 default:break;
}/*/end of the switch(sts)/ */
}/*end of func*/

/* function for read and process a data line in the file */
int parse_data(char *line, int type,error_p *err_head,data_p *data_head,int line_A,unsigned int *IC,unsigned int *DC)
{

 int i,cnt,curr;
 char number[MAX_DATA];
 char *str;

/*check which "treatment" to give for the inst_opt */
 
  switch(type)
  {
  /*/.data/ */
   case 0:
   {
     cnt = 0;
    strcpy(number,"\0");
/* cut the numbers from the line */
     str = strtok(line,"");
 
/* loop for checking the number values in the line */
 while(str != NULL)
 {
   i = skip_spaces(str);
   if(check_param(str+i,err_head,line_A))
    {/*delet the numbers withput spaces only*/
     sscanf(str,"%d",&curr);
     if(add_data(IC,DC,curr,data_head,err_head,line_A))
      {
        cnt++;
        (*DC)++;
      }
     else
      {
        return 0;
        str = strtok(NULL,"");
      }
    }
   else
     {
       return 0;
     }
   }/*/end of loop/ */
    break;
 }/*/end of loop/ */

/* /.string/ */
 case 1:
 {
    cnt = 0;
   i = skip_spaces(line);
 /*string start and end with " ". exp: "Hello World"/ */
  if(line[i] == '"')
    i++;
  if(line[i] == '\"')
    i++;
 
 /*add each char to the data list */
  while(line[i] != '\"')
  {
    if(add_char(IC,DC,line[i],data_head,err_head,line_A))
     {
       (*DC)++;
       i++;
       cnt++;
     }
    else
     {
      return 0;
     }
  } 
 /*we added at least one char so we have to add the end of a string sign '\0') */
   if(cnt > 0)
   {
     if(add_char(IC,DC,'\0',data_head,err_head,line_A))
      {
        (*DC)++;
        i++;
        cnt++;
      }
     else
      {
        return 0;
      }
   }
  
   i += skip_spaces(line+i);
   /*only if its the end of the line - return the number of char that added to the data list else, return error/ */
   if(line[i] == '\n')
   {
     return cnt;
   }
   else 
   {
     add_err(err_head,UNDEFIEND_PARAMETER, line_A);
     return 0;
   }
  }
 default:break;

 }
return 0;
}

int parse_entry(label_p *lbl_head, error_p *err_head, int line_A,char *line, unsigned int *DC)
{
  char word[MAX_LABEL_LENGTH];
  
  if(sscanf(line,"%s",word))
   {
 /*/if the label is already exists, update the parameter for positive value/ */
     if(find(lbl_head,word))
     {
       find(lbl_head,word)->data.entries = 1;
       return 1;
     }
     else
     {
  /*/add the new label with a positive value for entry/ */
        return(add_label(lbl_head,word,*DC,0,1));
     }
   }
   else
   {
     add_err(err_head,MISSING_PARAMETER,line_A);
     return 0;
   }
}/*/end of func parse_entry/ */

int parse_extern(label_p *lbl_head,error_p *err_head,int line_A,char *line)
{
     
char word[MAX_LABEL_LENGTH];

 if(sscanf(line,"%s",word))
 {
 /*/add a new label tagges as a extern/ */
    return add_label(lbl_head,word,0,1,0);
 }
 else 
 {
   add_err(err_head,MISSING_PARAMETER,line_A);
   return 0;
 }
}/*end of func parse_extern*/
