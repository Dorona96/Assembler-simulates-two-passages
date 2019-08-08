/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
 maman 14 made by
 Doron Sharaby - 204862197
 Or Soffer -205619513
 for: Tzachi Broder
 data: 12.08.2019
 final version of "project2019B"
 project decrption:




------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
#include "check_file.h"
#include "parse_file.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM 2


extern int IC,DC;
extern const char *data_arr[];
extern const char *reg_arr[];
extern command op_code[OP_CODE];
extern enum{STRING,DATA}data_type;
extern enum{LBL_LINE = 0,NUM_LINE = 1,INST_LINE = 2,ERR_LINE = 3}line_opt;
extern enum {A, E, R} ARE;
extern enum{MOV,CMP,ADD,SUB,NOT,CLR,LEA,INC,DEC,JMP,BNE,RED,PRN,JSR,RTS,STOP}op_code_dec;
extern enum {NONE_PRM = 0, NUM_PRM = 0, LBL_PRM = 1,ARR_PRM=2, REG_PRM = 3}prm_type;
/* Main method. sends each file name in argv to the parsing functions */
int main(int argc, char *argv[]) {
    int i;

    if (argc < NUM)
    {
        printf("%sERROR! MISSING FILES\n",KRED);
        return 1;
    }
    for (i = 1; i < argc; i++) 
    {
        
        check_file(argv[i]);   /* parse the file */
    }
    return 0;
}
