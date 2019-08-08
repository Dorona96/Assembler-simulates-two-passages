#include "second_tran.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define FIRST_ADDRESS 100

extern int IC,DC;
extern const char *data_arr[];
extern const char *reg_arr[];
extern command op_code[OP_CODE];
extern enum{STRING,DATA}data_type;
extern enum{LBL_LINE = 0,NUM_LINE = 1,INST_LINE = 2,ERR_LINE = 3}line_opt;
extern enum {A, E, R} ARE;
extern enum{MOV,CMP,ADD,SUB,NOT,CLR,LEA,INC,DEC,JMP,BNE,RED,PRN,JSR,RTS,STOP}op_code_dec;
extern enum {NONE_PRM = 0, NUM_PRM = 0, LBL_PRM = 1,ARR_PRM=2, REG_PRM = 3}prm_type;
/*extra*/

/*------------------------------------------------------------------------------------------------------------ */
/*------------------------------------------------------------------------------------------------------------ */
/*create the ".ent" file */
void create_ent(label_p *lbl_head, char *name){

    size_t length = strlen(name)+strlen(ENT_FILE);
    label_p curr= (label_p)malloc(sizeof(label_node));
    FILE *ent_file;
    char *str = (char*)malloc(sizeof(char)*length);
    if(!str) {
        fprintf(stderr, "%sError Failed to allocate",KRED);
        return ;
    }
    
    strcpy(str, name);
    strcat(str, ENT_FILE);

    ent_file = fopen(str, "w+");

    if(!ent_file)
        return;
    curr = *lbl_head;
   
    while(curr){
        if(curr->data.entries)
            fprintf(ent_file,"%s\t%d\n", curr->data.name, curr->data.address);
        curr = curr->next;
    }

    fclose(ent_file);
}


/*------------------------------------------------------------------------------------------------------------ */
/*------------------------------------------------------------------------------------------------------------ */

/*creat the ".ext" file*/

void create_ext(inst_p *head, char *name)
{
    size_t length = strlen(name)+strlen(EXT_FILE);
    FILE *ext_file;
    inst_p curr;
     char*str = (char*)malloc(sizeof(char)*length);
 
    if(!str)
    {
        fprintf(stderr, "%sError Failed to allocate\n",KRED);
        return;
    }
    strcpy(str, name);
    strcat(str, EXT_FILE);

    ext_file = fopen(str, "w+");

    if(!ext_file)
        return;
    curr = *head;
    /* print external valued labels to the file */
    while(curr){
        if(curr->data.source == LBL_PRM){
            if(curr->data.prm1.label.ARE == E){
                fprintf(ext_file, "%s\t%d\n", curr->data.prm1.label.name, curr->data.IC);
            }
        }
        if(curr->data.dest == LBL_PRM){
            if(curr->data.prm2.label.ARE == E) {
                if (curr->fin_first == 0)
                    fprintf(ext_file, "%s\t%d\n", curr->data.prm2.label.name, curr->data.IC);
                else
                    fprintf(ext_file, "%s\t%d\n", curr->data.prm2.label.name, curr->data.IC+1);
            }
        }
        curr = curr->next;
    }

    fclose(ext_file);
}

/*------------------------------------------------------------------------------------------------------------ */
/*------------------------------------------------------------------------------------------------------------ */
/* create the ".ob" file */

void create_ob(inst_p *inst_head, data_p *data_head, char *name, unsigned int *DC, unsigned int *IC)
{
    inst_p curr_inst=(inst_p)malloc(sizeof(inst_node));
    data_p curr_data;
    size_t length = strlen(name)+strlen(OB_FILE);
    FILE *ob_file;
    char *res = (char *)malloc(BITS_LINE);
    char *str = (char *)malloc(sizeof(char)*length);

    if(!res){
        fprintf(stderr, "%sError! Failed to allocate\n",KRED);
        return;
    }
    if(!str)
    {
        fprintf(stderr, "%sError! Failed to allocate\n",KRED);
        return;
    }
    
    strcpy(str, name);
    strcat(str, OB_FILE);

    ob_file = fopen(str, "w+");

    
    fprintf(ob_file, "%d %d\n", *IC-FIRST_ADDRESS, *DC);
    curr_inst = *inst_head;

    
    while(curr_inst)
    {
        if(curr_inst->fin_first != 0)
        {
           
            dec_2_spBase(curr_inst->fin_first, res);
            fprintf(ob_file, "%s\n", res);

            if(curr_inst->fin_second != 0)
            {
               dec_2_spBase(curr_inst->fin_second, res);
                fprintf(ob_file, "%s\n", res);
            }
            if(curr_inst->fin_third != 0)

            {    
                dec_2_spBase(curr_inst->fin_third, res);
                fprintf(ob_file, "%s\n", res);
            }
        }
      curr_inst = curr_inst->next;
    }/*end of loop*/
    curr_data = *data_head;
    
    while(curr_data)
    {
       
        dec_2_spBase(curr_data->fin, res);
        fprintf(ob_file, "%s\n", res);
        curr_data = curr_data->next;
    }
}

 /*------------------------------------------------------------------------------------------------------ */
 /*------------------------------------------------------------------------------------------------------ */

void fin_inst(inst_p p)
{
    p=(inst_p)malloc(sizeof(inst_node));
    unsigned int temp=0;
    int flag=0;
    /* set the binary representation for the command */
    temp |= p->data.opCode; /* 4 bits */
    temp <<= 2;
    temp |= p->data.source; /* 2 bits */
    temp <<= 2;
    temp |= p->data.dest;  /* 2 bits */
    temp <<= 2;
    temp |= p->data.ARE; /* 2 bits */
    p->fin_first = temp;
    /* set the binary representation for the first added word */

    temp = 0;
    switch (p->data.source)
    {
        case LBL_PRM: 
        {
            temp |= p->data.prm1.label.address; /* takes up 12 bits */
            temp <<= 2;
            temp |= p->data.prm1.label.ARE; /* takes up 2 bits */
            
            break;
        }
        case REG_PRM: 
        {
            if(p->data.dest == REG_PRM)
            {
                
                 temp |= p->data.prm1.reg.sor; /* takes up 3 bits */
                 temp <<= 3;    
                 temp |= p->data.prm1.reg.des; /* takes up 3 bits */
                 temp <<=2;
                 temp |= p->data.prm1.reg.ARE; /* takes up 2 bits */
                 
               return;
            }
           
           temp |= p->data.prm1.reg.ARE; 
           break;
        }
        case NUM_PRM: 
        {
            temp |= p->data.prm1.number.number; /* takes up 10 bits */
            temp <<= 2;
            /* The value for the last two bits is 00 */
            break;
        }

        case ARR_PRM:
        {
            flag++;
            temp |= p->data.prm1.arr.addres;
            temp<<=2;
            temp |= p->data.prm1.arr.ARE_f;
            p->fin_second = temp;
            temp = 0;
            temp |= p->data.prm1.arr.index;
            temp<<=2;
            temp |= p->data.prm1.arr.ARE_s;
        }
        default:break;
    }/*end of switch*/
    if(!flag)
    {
        p->fin_second = temp;
        temp = 0;  
    }
    else
    {
         p->fin_third = temp;
         temp = 0; 
    }
    switch (p->data.dest)
    {
        case LBL_PRM:
        {
            temp |= p->data.prm2.label.address; /* takes up 12 bits */
            temp <<= 2;
            temp |= p->data.prm2.label.ARE; /* takes up 2 bits */
            break;
        }
        case REG_PRM:
        {
            temp |= p->data.prm2.reg.des; /* takes up 3 bits */
            temp <<= 2;
            temp |= p->data.prm2.reg.ARE; /* takes up 2 bits */
            break;
        }
        case NUM_PRM:
        {
            temp |= p->data.prm2.number.number; /* takes up 12 bits */
            temp <<= 2;
            /* The value for the last two bits is 00 */
            break;
        }
        case ARR_PRM:
        {
            flag++;
            temp |= p->data.prm2.arr.addres;
            temp<<=2;
            temp |= p->data.prm2.arr.ARE_f;
            if(flag==2)
            { 
            p->fin_four = temp;
            }
            else
            {
                p->fin_third=temp;  
            }
            temp=0;
            temp |= p->data.prm2.arr.index;
            temp<<=2;
            temp |= p->data.prm2.arr.ARE_s;
        }
        default:break;
        }
        if(flag==2)
{
    p->fin_five = temp;
}
else
{
    if(flag==1)
    {
        p->fin_four=temp;
    }
    else
    {
    p->fin_third=temp;
    }

}


}

/* binary representations of the data */
void fin_data(data_p p)
{
    p=(data_p)malloc(sizeof(data_node));
    unsigned int temp = 0;
    switch (p->data.type)
    {
       case STRING:
        {
            temp |= (int) p->data.c; /* cast the character to an int */
            break;
        }
       case DATA:
        {
            temp |= p->data.number; /* save the number in the data node */
            break;
        }

       default:break;
    }

    p->fin = temp;
}
 /*------------------------------------------------------------------------------------------------------ */
 /*------------------------------------------------------------------------------------------------------ */
/* The function gets number and returns result as char * */
char *dec_2_spBase(int num, char *result_p) {
    result_p=(char*)malloc(sizeof(7));
    int i=13,flag=0;
    char before[14], result[7];
   
    /*convert dec to bin */

    if(num<0)
    flag=1;

  while(num)
  {
      if(!(num%2))
      {
          before[i]=REG_ZERO;
      }
      else
      {
         before[i]=REG_ONE;/* code */
      }
      
    i--;
  }

  /*covert bin to special base */
  if(flag==1)
  for(i=12;i>-1;i--)/*i=12 cause the first 1 stay the same */
              {
                  if (before[i]==REG_ZERO)
                  { 
                       before[i]=REG_ONE;
                  }
                  else
                  {
                      before[i]=REG_ZERO;
                  }
              }
   i=0;
   while(i!=8)
    {
        if (before[i]==REG_ZERO &&before[i+1]==REG_ZERO)
        result[i]=SP_ZERO;

        if(before[i]==REG_ZERO &&before[i+1]==REG_ONE)
        result[i]=SP_ONE;

        if(before[i]==REG_ONE&&before[i+1]==REG_ZERO)
        result[i]=SP_TWO;

        if(before[i]==REG_ONE&&before[i+1]==REG_ONE)
        result[i]=SP_THREE;

        i+=2;

    }
    result_p= &result[7];
	return result_p;
}

void update_data_add(unsigned int *IC, data_p *head){
    data_p curr= (data_p)malloc(sizeof(data_node));
    curr = *head;
    while (curr){
        curr->data.address += *IC;
        curr = curr->next;
    }
}
/* this function updates all the nodes in the label list with the current information */
void update_labels_info(label_p *head_label, unsigned int *IC){
    label_p curr= (label_p)malloc(sizeof(label_node));
    curr = *head_label;
    while(curr){
        if(curr->data.external)
            /* if the label is marked external, the address should be set to zero */
            curr->data.address = NO;
        else if(!curr->data.command)
            curr->data.address += *IC;
        curr = curr->next;
    }
}
/* function to update the information in the instructions list , only label references need to be updated*/
void update_inst_list(label_p *head_label, error_p *head_error, inst_p *head_instruction){
    inst_p curr= (inst_p)malloc(sizeof(inst_node));
    label_p lbl= (label_p)malloc(sizeof(label_node));
    curr = *head_instruction;
    /* iterate through the instruction list to update all the nodes */
    while(curr){
        /* the origin argument is a label */
        if(curr->data.dest == LBL_PRM){
            
            lbl = find(head_label, curr->data.prm1.label.name);
            if(!lbl){
                add_err(head_error, LABEL_NOT_DECLARED, curr->data.line_number);
            }
            else{
                curr->data.prm1.label.address = lbl->data.address;
                if(lbl->data.external) {
                    curr->data.prm1.label.ARE = E;
                    curr->data.prm1.label.address = NONE_PRM;
                }
                else
                    curr->data.prm1.label.ARE = R;
            }
        }
        /* the destination argument is a label */
        if(curr->data.dest == LBL_PRM){
            lbl = find(head_label, curr->data.prm2.label.name);
            if(!lbl){
                add_err(head_error, LABEL_NOT_DECLARED, curr->data.line_number);
            }
            else{
                curr->data.prm2.label.address = lbl->data.address;
                if(lbl->data.external)
                    curr->data.prm2.label.ARE = E;
                else
                    curr->data.prm2.label.ARE = R;
            }
        }
        /* after the instruction node has been updated, the binary representation should be set */
        fin_inst(curr);
        curr = curr->next;
    }
}


