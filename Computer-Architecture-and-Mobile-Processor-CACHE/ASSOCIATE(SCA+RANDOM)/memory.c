#include "mips.h"
extern int data_miss;
extern int data_hit;

void memory_access(inst_ *ins, Control_ * c){
    if(c->MemRead){
        
        //ins->write_data=Memory[(ins->ALU_Result)/4];
        data_cache_access(ins, c);
        //printf("lw address 0x%0x get %d\n",(ins->ALU_Result)/4,ins->write_data);
    }
    if(c->MemWrite){
        data_cache_access(ins,c);
        //Memory[(ins->ALU_Result)/4]=reg[ins->rt];
        //printf("sw address mem[0x%0x] = %d\n",ins->ALU_Result/4,reg[ins->rt]);
    }
}