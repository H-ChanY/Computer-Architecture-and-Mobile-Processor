#include "mips.h"

extern int Memory[0x400000];       //16MB memory
extern int reg[32];
extern int pc;

void branch_pc(IF_ID *if_id,EX_MEM * ex_mem){
    if(ex_mem->zero_ && ex_mem->Br_taken){
            pc=ex_mem->branch_addr;
            if_id->inst=Memory[pc/4];
            if_id->pc_4=pc+4;
            pc=pc+4;
        }
}