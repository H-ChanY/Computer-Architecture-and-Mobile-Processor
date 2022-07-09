#include "mips.h"

extern int Memory[0x400000];       //16MB memory
extern int reg[32];
extern int pc;
extern BTB btb_[100];

void memory_access(EX_MEM * ex_mem,MEM_WB * mem_wb){
    if(ex_mem->MemRead){
        mem_wb->mem_read_data=Memory[(ex_mem->alu_result)/4];
        printf("lw address 0x%0x get %d\n",ex_mem->alu_result,mem_wb->mem_read_data);
    }
    else if(ex_mem->MemWrite){
        Memory[(ex_mem->alu_result)/4]=ex_mem->write_data;
        printf("sw address mem[0x%0x] = %d\n",ex_mem->alu_result/4,ex_mem->write_data);
    }else{
        printf("This is MEMORY_ACCESS\n");
    }  
    mem_wb->Jal_chk=ex_mem->Jal_chk;
    mem_wb->pc_4=ex_mem->pc_4;
    mem_wb->write_reg=ex_mem->write_reg;
    mem_wb->alu_result=ex_mem->alu_result;
    mem_wb->MemtoReg=ex_mem->MemtoReg;
    mem_wb->RegWrite=ex_mem->RegWrite;
}