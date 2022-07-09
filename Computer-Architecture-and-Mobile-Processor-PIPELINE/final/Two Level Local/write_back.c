#include "mips.h"

extern int Memory[0x400000];       //16MB memory
extern int reg[32];
extern int pc;
extern BTB btb_[100];

void write_back(MEM_WB* mem_wb){
    
    if(mem_wb->RegWrite && mem_wb->MemtoReg){
        //lw 인 경우 
        mem_wb->write_reg_data=mem_wb->mem_read_data;
        reg[mem_wb->write_reg]=mem_wb->write_reg_data;
        //printf("lw reg[%d] =%d\n",mem_wb->write_reg,mem_wb->mem_read_data);
    }else if(mem_wb->Jal_chk){
        mem_wb->write_reg_data=mem_wb->pc_4+4;
        reg[mem_wb->write_reg]=mem_wb->write_reg_data;
        //printf("JAL: reg[%d] =%d \n",mem_wb->write_reg,mem_wb->write_reg_data);
    }
    else if(mem_wb->RegWrite){
        mem_wb->write_reg_data=mem_wb->alu_result;
        reg[mem_wb->write_reg]=mem_wb->write_reg_data;
        //printf("reg[%d] =%d \n",mem_wb->write_reg,mem_wb->alu_result);
    }else{
        //printf("THIS IS WRITE_BACK\n");
    }
}
