#include "first_tran.h"

#ifndef CHECK_FILE_H
#define CHECK_FILE_H

#define MAX_DATA 10
#define LINE_LENGTH 31
#define FIRST_ADDRESS 100

/*extra*/
#define KYEL "\x1B[33m"


/*------------------------------------------------------------------------
 functions for pasre the lines in the given file
--------------------------------------------------------------------------*/


void check_file(char *name);

void parse_line(char *line, int num,label_p *lbl_head,error_p *err_head,data_p *head_data,inst_p *inst_head,unsigned int *IC,unsigned int *DC, int *extern_flag, int *entry_flag);
  
int parse_data(char *line, int type ,error_p *err_head,data_p *data_head,int line_A,unsigned int *IC,unsigned int *DC);

int parse_entry(label_p *lbl_head, error_p *err_head, int line_A,char *line, unsigned int *DC);

int parse_extern(label_p *lbl_head,error_p *err_head,int line_A,char *line);

/* ---------------------------------------------------------------------------*/

#endif /* CHECK_FILE_H */
