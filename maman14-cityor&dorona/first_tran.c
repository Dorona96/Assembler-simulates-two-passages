
#include "first_tran.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define PRN 12
#define CMP 1
#define LEA 6

#define KRED "\x1B[31m"


const char *reg_arr[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", NULL}; 
const char *data_arr[] =  {".data", ".string", ".entry", ".extern", NULL};
int IC = FIRST_ADDRESS , DC = 0;

typedef enum{STRING,DATA}data_type;

typedef enum{LBL_LINE,NUM_LINE ,INST_LINE,ERR_LINE}line_opt;

typedef enum{NONE_PRM=0,NUM_PRM = 0,LBL_PRM = 1,ARR_PRM= 2,REG_PRM = 3}prm_type;

typedef enum {A, E, R} ARE;
/*----------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------- */

label_info *find(label_p *lbl_head,char *name)
{
  label_p temp= *lbl_head;

   while(temp)
   {
     if(strcmp(temp->data.name,name))
      {
         return &temp->data; /*return  the name of the label*/
      }
    temp = temp->next;

   }/*end of loop*/
  return NULL; /*if the name doesn't excist*/

}/*end of function*/

void free_label_list(label_p *lbl_head)
{
  label_p curr;
   while(*lbl_head)
   {
      curr = (*lbl_head)->next;
      free(*lbl_head);
  *lbl_head = curr;

   }
}/*end of function*/


int add_label(label_p *lbl_head, char *name,int address,int extrnal,int entries)
{
  label_p newNode;
  label_p curr;
   
  newNode = (label_p)malloc(sizeof(label_node));

  if(newNode == NULL)
  {
       printf("%sno memory allocation, the assigment failed\n",KRED);
     return 0;
  }
  
  name = strtok(name,"");
  strcpy(newNode->data.name,name);
  newNode->data.address = address;
  newNode->data.external = extrnal;
  newNode->data.entries = entries;
  newNode->next = NULL;

  if(*lbl_head == NULL)
  {
     *lbl_head = newNode;
  
  }else{
     curr = *lbl_head;
     while(curr->next != NULL)
     {
        curr = curr->next;
     }

   curr->next = newNode;

 }/*end of else*/

return 0;

}/*end of function*/

/*----------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------- */

/*The function add a number to the list*/
int add_data(unsigned int *IC,unsigned int *DC,int number,data_p *data_head,error_p *err_head,int line)
{

  data_p newNode;
  data_p curr;  
     
  if((*IC+*DC) > MAX_MEMORY)
  {
      add_err(err_head,OUT_OF_MEMORY,line);
     return 0;
  }
  else
  {
     newNode = (data_p)malloc(sizeof(data_node));
  }/* end of else*/

  
  if(newNode == NULL)
  {
       printf("%sno memory allocation, the assigment failed\n",KRED);
     exit(1);
  }
  
  if(number < 0)
  {
    newNode->data.type = DATA;
    newNode->data.number = number;
    newNode->data.address = *DC;
    newNode->next = NULL;
  }
  if(*data_head == NULL)
  {
     *data_head = newNode;
  
   }else{
     curr = *data_head;
     while(curr->next != NULL)
     {
        curr = curr->next;
     }

   curr->next = newNode;

 }/* end of else*/
return 1;

}/* end program*/


/*The function add a data char node to the list */
int add_char(unsigned int *IC,unsigned int *DC,char c,data_p *data_head,error_p *err_head,int line)
{
  data_p newNode;
  data_p curr;  
     
  if((*IC+*DC) > MAX_MEMORY)
  {
     add_err(err_head,OUT_OF_MEMORY,line);
     return 0;
  }else{
          newNode = (data_p)malloc(sizeof(data_node));
       }
  if(newNode == NULL)
  {
       printf("%sCan't find memory, the allocated failed\n",KRED);
     return 0;
  }
  
  newNode->data.type = STRING;
  newNode->data.c = c;
  newNode->data.address = *DC;
  newNode->next = NULL;

  if(*data_head == NULL)
   {
     *data_head = newNode;
   }
  else{
     curr = *data_head;
     while(curr->next != NULL)
     {
        curr = curr->next;
     }

   curr->next = newNode;

 }/*end of else */
return 1;

}/*end program */


/*free all the data in the linked list */
void free_data_list(data_p *data_head)
{
  data_p temp;

  while(*data_head)
  {
     temp = (*data_head)->next;
     free(*data_head);
     *data_head = temp;
  }
}

/*----------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------- */

int add_inst(unsigned int *IC, unsigned int *DC,unsigned int opCode,int parameter,char *str,error_p *err_head,inst_p *inst_head,int line)
{
 inst_p temp;
 inst_p newNode;
 char *check=NULL;
 unsigned int j = 0;
 int i = 0;
 int value;

 if((*IC+*DC) > MAX_MEMORY)
  {
    add_err(err_head,OUT_OF_MEMORY,line);
    return 0;
  }
  else
  {
    newNode = (inst_p)malloc(sizeof(inst_node));
    if(newNode == NULL)
     {
       printf("%sno memory allocation, the assigment failed\n",KRED);
       exit(1);
     }
   
    newNode->data.opCode = opCode; 
    newNode->data.ARE = A; 
    newNode->data.line_number = line; /*in case of error in file*/
    newNode->data.IC = *IC;
    newNode->next = NULL;
    (*IC)++;

/*This action check by switch all the cases that can make error in the instruction line*/

 switch(parameter)
  {
  /*The line without errors/extra words by inst_p, meaning no parm*/
   case 0: 
   {
     newNode->data.source = NONE_PRM;
     newNode->data.dest = NONE_PRM;
     newNode->data.ARE = A;
   /*if it's not the end of the line*/
     i += skip_spaces(str);
      if(str[i] != '\n') 
      {
        add_err(err_head,TOO_MANY_PARAMETERS,line);
       return 0;
      }
    break;
   }/*end of case 0*/

   
   case 1:/*one extra word meaning one parm */
   {
      (*IC)++;
       i += skip_spaces(str);
  if(str[i] == 'r')
  {
   /*The argument can't receive register as source operand*/
         if(opCode == LEA)
          { 
            add_err(err_head,ARGUMENT_ILLEGAL,line);
            return 0;
          }
      while(reg_arr[j])
       {
        if(!strcmp(reg_arr[j],str)) /*we found register at the opcode file*/
         {
             newNode->data.dest = REG_PRM;/*שיטת מיעון אוגר ישיר שלוש */
             newNode->data.source = NONE_PRM;
             newNode->data.prm2.reg.des = j;
             newNode->data.prm2.reg.sor = NONE_PRM;
             newNode->data.prm2.reg.ARE = A;
        
             i += strlen(reg_arr[j]);
             i += skip_spaces(str + i);

   /*if we reached the end of the line with the sign ','*/
             if(str[i] == ',')
             {
               add_err(err_head,MISSING_PARAMETER,line);
               return 0;
             }
             if(str[i] != '\n')
             {
               add_err(err_head,ECXESSIVE_TEXT,line);
               return 0;
             }
             break;
          }/*end of if(strcmp)*/

         j++;

        }/*end of loop*/
   
  /*j represent the number in the file, and check if it match to the reg_arr**/
        if(j == REGISTER_DIGIT)
        {
           add_err(err_head,UNDEFIEND_REGISTER,line);      
           return 0;
        } 
  }/*end of if(str[i])*/
  else
  {

      if(check_label(str,err_head,line))/*זה לא אוגר כרגע בודקים אם זו תווית */
        {
           newNode->data.dest = LBL_PRM;/*שיטת מיעון ישיר */

           if(check_end(str))
           {
               add_err(err_head,TOO_MANY_PARAMETERS,line);
               return 0;
            }
 
            newNode->data.prm2.label.name = (char *)malloc(MAX_LABEL_LENGTH);
            if(newNode->data.prm2.label.name == NULL)
            { 
               printf("%sError!no memory allocation, the assigment failed",KRED);
               exit(1);
            }
     
            sscanf(str,"%s",str);
            strcpy(newNode->data.prm2.label.name,str);
        }/*end of if(check_label)*/

/*LEA can get only label to rgeister, means, can not ger number as agrument*/
      else if(check_param(str,err_head,line))
      {
            if(opCode == LEA) 
            {
               add_err(err_head,ARGUMENT_ILLEGAL,line);
               return 0;
            }
    
            newNode->data.dest = NUM_PRM;/*מיעון מיידי */
            newNode->data.prm2.number.ARE = A;
            sscanf(str,"%d",&value);
     
      }
     else
      {
     /*The argument doesn't match to the line*/
           add_err(err_head,UNDEFIEND_PARAMETER,line);
           return 0;
      }
    }/*end of first if*/
   break;
  }/*end of case 1*/ 

 /*in case that op_code with TWO operands */

  case 2: 
  {
    if(check_comma(str,err_head,line))
    {
             check = strtok(str," "); /* "delet" the first argument*/
 /*take the argument withut any tabs OR space*/
              if(sscanf(check,"%s",check)) 
              {
               if(check[0] == 'r')
               { 
                    while(reg_arr[j])/*The arr not empty*/
                    {
          /*The arguments is from the array's register*/
                       if(!strcmp(check,reg_arr[j])) 
                       {
                             newNode->data.source = REG_PRM;
                             newNode->data.prm1.reg.ARE = A;
                             newNode->data.prm1.reg.sor = j;
                             (IC)++; /*promoting memory space*/
                             break;
                       }
                      j++;
                    }/*end of loop*/
              
                   if(j == REGISTER_DIGIT) 
                   {
                       add_err(err_head,UNDEFIEND_REGISTER,line);
                   }
               }/*end of if(check[0])*/

 /*if the argument is not a register we will check if its number OR label*/
               else
               {
                  if(check_label(check,err_head,line)) 
                  {
                       newNode->data.source = LBL_PRM;/*מיעון ישיר 1 */
                       newNode->data.prm1.label.ARE = R;
                       newNode->data.prm1.label.name = (char *)malloc(MAX_LABEL_LENGTH);
         
                       if(newNode->data.prm1.label.name == NULL)
                       {
                           printf("%sError. No space can be allocate\n",KRED);
                           exit(1);       
                       }
                       sscanf(check,"%s",check);
                       strcpy(newNode->data.prm1.label.name ,check);
                       (*IC)++;
                  }/*end of if(check_label)*/
                  else if(check_param(check,err_head,line)) 
                  {
                       newNode->data.source = NUM_PRM;
                       newNode->data.prm1.number.ARE = A;
                       (*IC)++;
                  }
                  else
                  {
                     return 0;
                  }

               }/*end of else*/
        
              check = strtok(NULL,",");
              sscanf(check,"%s",check);
              j = 0;
   
    /*check the second argument*/
     if(check[0] == 'r')
     {
        while(reg_arr[j])
        {
         /*if the argument is a register*/
           if(!strcmp(check,reg_arr[j]))
           {
               newNode->data.dest = REG_PRM;
  
               if(newNode->data.dest == REG_PRM) 
               {
                 newNode->data.prm2.reg.des= j;
               }
              else
               {
                 newNode->data.prm2.reg.des = j;
                 newNode->data.prm2.reg.ARE = A;
               }

               check = strtok(NULL,",");
               if(check)
               {
                   add_err(err_head,TOO_MANY_PARAMETERS,line);
                   return 0;
               }
          break;
         }/*end of is(strcmp)*/
       j++;
      }/*end of loop*/
     }/*end of if(check[0])*/
     else
    {
       if(check_label(check,err_head,line)) 
       {
              newNode->data.dest = LBL_PRM;
              newNode->data.prm1.label.ARE = R;
              sscanf(check,"%s",check);

              newNode->data.prm2.label.name = (char *)malloc(MAX_LABEL_LENGTH);  
              if(newNode->data.prm2.label.name == NULL)
                printf("%sError. No space can be allocate\n",KRED);
                exit(1);       
              strcpy(newNode->data.prm2.label.name,check);
              (*IC)++;
       }
/*by declarition The argument(cmp,prn) is a number*/
       else if(check_param(check,err_head,line))
       {
            if((opCode == CMP) || (opCode == PRN))
            {
                newNode->data.dest = 0;
                newNode->data.prm1.number.ARE = A; /*0*/
                (*IC)++;
            }
            else
            {
               add_err(err_head,ARGUMENT_ILLEGAL,line);
               return 0;
            }
       }
       else
       {
          return 0;
       }
 
       check = strtok(NULL,",");
       if(check)
       {
          add_err(err_head,TOO_MANY_PARAMETERS, line);
          return 0;
       }  
     }
  }
    else
       {
           add_err(err_head,MISSING_COMMA,line);
           return 0;
       }   
    }/*end of else*/
    default:break;
  }/*end of case 2*/
/*check if the intruction linked_list is empty OR not*/ 

    if(*inst_head == NULL)
    {
      *inst_head = newNode;
    }
    else
    {
      temp = (*inst_head)->next;
        while(temp)
        {
          inst_head = &(*inst_head)->next;
           temp = temp->next;
        }
      (*inst_head)->next = newNode;
    }
   }
 return 1;
  }
}/*end of function*/
/* function to free all the memory occupied by the instruction list */
void free_inst_list(inst_p *head)
{
    inst_p curr;
    while(*head){
        curr = (*head)->next;
        free(*head);
        *head = curr;
    }
}

/*----------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------- */


void free_lists(label_p *lbl_head,data_p *data_head,inst_p *inst_head,error_p *err_head)
{
  free_err_list(err_head);
  free_data_list(data_head);
  free_label_list(lbl_head);
  free_inst_list(inst_head);
}
