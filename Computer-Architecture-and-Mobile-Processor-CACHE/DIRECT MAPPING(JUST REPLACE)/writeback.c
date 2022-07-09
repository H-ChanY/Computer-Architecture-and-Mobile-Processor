
#include "mips.h"

void write_back(inst_ *ins, Control_ * c){
    if(c->RegWrite && c->Jal_chk){
        reg[ins->rd]=pc;
    }else if(c->RegWrite &&c->MemtoReg){
        reg[ins->rd]=ins->write_data;
        //printf("lw reg[%d] =%d\n",ins->rd,ins->write_data);
    }else if(c->RegWrite){
        reg[ins->rd]=ins->ALU_Result;
        //printf("reg[%d] =%d \n",ins->rd,ins->ALU_Result);
    }
}