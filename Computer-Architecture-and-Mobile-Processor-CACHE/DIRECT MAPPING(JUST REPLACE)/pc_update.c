#include "mips.h"
extern int pc;
void pc_update(inst_ *ins, Control_* c) {
    // j, jal, jr, branch 인 경우
    
    

    if(c->Jump_chk){
        pc=ins->address;
        //printf("[PC Update] JUMP pc -> %x\n",pc);
    }else if(c->Jal_chk){
        pc=ins->address;
        //printf("[PC Update] JAL jump : 0x%x Linked : 0x%x\n",pc,reg[31]);
    }else if(c->Jr_chk){
        pc=ins->input_1;
        //printf("[PC Update] JR pc-> 0x%x\n",pc);
    }else{
        if(c->Br_taken && ins->ALU_Result){
            pc=ins->Branch_target;
            //printf("[PC Update] branch pc -> 0x%x",pc);
        }else{
            //printf("\n[PC Update] pc -> 0x%x",pc);
        }
    }
}