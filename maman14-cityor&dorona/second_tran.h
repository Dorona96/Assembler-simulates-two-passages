#include "first_tran.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BITS_LINE 14
#define SP_LINE 7
#define REG_ZERO '0'
#define REG_ONE '1'

/*output files */
#define ENT_FILE ".ent"
#define EXT_FILE ".ext"
#define OB_FILE ".ob"


/*special base */
#define SP_ZERO '*'
#define SP_ONE '#'
#define SP_TWO '%'
#define SP_THREE '!'

/* extra*/
#define KRED "\x1B[31m"

/*----------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------- */

void create_ent(label_p *head, char *name);
void create_ext(inst_p *head, char *name);
void create_ob(inst_p *inst_head, data_p *data_head, char *name, unsigned int *DC, unsigned int *IC);

/*----------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------- */

char *dec_2_spBase(int num, char *result_p);

/*-----------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------- */


void fin_inst(inst_p p);
void fin_data(data_p p);

/*------------------------------------------------------------------------------------------------------------ */
/*------------------------------------------------------------------------------------------------------------ */

void update_data_ads(unsigned int *IC, data_p *data_head);

void update_label(label_p *lbl_head, unsigned int *IC);

void update_inst_list(label_p *lbl_head,error_p *err_head, inst_p *inst_head);



