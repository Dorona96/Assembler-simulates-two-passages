#ifndef FIRST_TRAN_H
#include "parse_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define FIRST_TRAN_H


/*given constants*/
#define MAX_MEMORY 4096
#define REGISTER_DIGIT 8  
#define MEMORY_WORD_LENGTH 14
#define MAX_LABEL_LENGTH 31
#define FIRST_ADDRESS 100 /*memory starting from 100*/
#define OP_CODE 16 /*between 0-15*/
#define AS_FILE ".as"

/*General*/



/*------------------------------------------------------------------------------------------------------------ */
/*------------------------------------------------------------------------------------------------------------ */

typedef struct label_info{
                  char name[MAX_LABEL_LENGTH];
                  int address; /*address of the label*/
                  int external;
                  int entries;
                  int command;
}label_info;

typedef struct label_node *label_p;
typedef struct label_node{
      label_info data;
      label_p next;
}label_node;

/*------------------------------------------------------------------------------------------------------------ */
/*------------------------------------------------------------------------------------------------------------ */
typedef struct op_reg{
unsigned int ARE: 2;/*always 00 A */
unsigned int des :3; 
unsigned int sor :3;
}op_reg;

typedef struct op_arr{
       unsigned int ARE_f: 2; /*0-1 bits option 01/10 R or E*/
       unsigned int addres: 12; 
       unsigned int :0;/*new word */
       unsigned int ARE_s: 2; /*0-1 bits option 00 A*/
       unsigned int index: 12; 
}op_arr;


typedef struct op_num{
     unsigned int ARE: 2;/* */
     unsigned int number:12;    
}op_num;

typedef struct op_lbl{
          char *name; /*small/capital and series of letters or numbers*/
          unsigned int ARE: 2; /*0-1 bits option 01/10 R or E*/
          unsigned int address: 12;
}op_lbl;

typedef union op_type{
        op_arr arr;
        op_num number;
        op_lbl label;
        op_reg reg;
}op_type;

/*------------------------------------------------------------------------------------------------------------ */
/*------------------------------------------------------------------------------------------------------------ */

typedef struct inst_info{
       unsigned int ARE: 2; /* 0-1*/
       unsigned int dest: 2; /*1-2*/
       unsigned int source: 2; /*4-5 bites*/
       unsigned int opCode: 4; /*6-9 bites with 16 actions add(0)*/
       unsigned int unused: 4;
        int IC; /* memory from 100*/
        int line_number;/*from the file*/
        op_type prm1;
        op_type prm2;
}inst_info;

typedef struct inst_node *inst_p;
typedef struct inst_node{
      inst_info data; /*The "pointer" to the inst_info*/
      unsigned int fin_first : 14; /* representation of the instruction word  */
      unsigned int fin_second : 14; /* representation of the first added word */
      unsigned int fin_third : 14; /* representation of the second added word */
      unsigned int fin_four : 14; 
      unsigned int fin_five: 14; 
      inst_p next;
}inst_node;



/*------------------------------------------------------------------------------------------------------------ */
/*------------------------------------------------------------------------------------------------------------ */


typedef struct data_info{
         int type; /*STRING or DATA */
         char c; /* he char varible*/
         unsigned int number;
         int address; /*The data address in the memory, *DC OR *IC */
         int line_number;

}data_info;


typedef struct data_node *data_p;
typedef struct data_node{
         data_info data;
         unsigned int fin: 10;
         data_p next;
}data_node;



/*------------------------------------------------------------------------------------------------------------ */
/*------------------------------------------------------------------------------------------------------------ */

/*------------------------------------------------------------------------------------------------------------ */
/*------------------------------------------------------------------------------------------------------------ */
/*
---------------------------------
function declration 
---------------------------------
*/

int add_label(label_p *head, char *name,int address,int extrnal,int entries);

label_info *find(label_p *lbl_head,char *word);

void free_label_list(label_p *lbl_head);

int add_data(unsigned int *IC,unsigned int *DC,int number,data_p *data_head,error_p *err_head,int line);

int add_char(unsigned int *IC,unsigned int *DC,char c,data_p *data_head,error_p *err_head,int line);

void free_data_list(data_p *data_head);

int add_inst(unsigned int *IC, unsigned int *DC,unsigned int opCode,int parameter,char *str,error_p *err_head,inst_p *inst_head,int line);

void free_inst_list(inst_p *inst_head);

void free_lists(label_p *lbl_head,data_p *data_head,inst_p *inst_head,error_p *err_head);

#endif /* FIRST_TRAN_H */
