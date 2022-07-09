#include "mips.h"
extern int pc;

void Control(inst_ *ins, Control_* c) {
    if (ins->opcode == 0 ) c->RegDest = true;
    if(ins->opcode!=ori && ins->opcode!=andi) c->sign_imm=true;
    if (ins->opcode != 0 && ins->opcode != beq && ins->opcode != bne) c->ALUSrc = true;
    if (ins->opcode == lw || ins->opcode == ll || ins->opcode == lbu || ins->opcode == lhu) c->MemtoReg = true;
    if (ins->opcode != sw && ins->opcode != bne && ins->opcode != beq && ins->opcode != j ) {
        if(ins->opcode==0 && ins->func_ == jr){
            c->RegWrite = false;
        }else{
            c->RegWrite = true;
        }
    }
    if (ins->opcode == lw) c->MemRead = true;
    if (ins->opcode == sw || ins->opcode == sh || ins->opcode == sb) c->MemWrite = true;
    if (ins->opcode == j) c->Jump_chk = true;
    if (ins->opcode == beq ) c->Br_taken = true;
    else if (ins->opcode == bne ) c->Br_taken = true;
    if(ins->opcode==0 && ins->func_==jr) c->Jr_chk=true;
    if (ins->opcode==jal) {
        c->Jal_chk=true;
        c->RegWrite=true;    
    }
}

void Decode(inst_ *ins, Control_* c) {
    ins->opcode = ((ins->inst >> 26) & 0x3f);
    ins->func_ = (ins->inst & 0x3f);
    Control(ins,c);
    if(c->Jal_chk|| c->Jump_chk){
        pc =pc + 4;
    }
    if(c->sign_imm){
        if (((ins->inst >> 15) & 0x1) == 1) ins->imm =  0xffff0000 + (ins->inst & 0xffff);
        else ins->imm = 0x00000000 + (ins->inst & 0xffff);
    }else{
        ins->imm = 0x00000000 + (ins->inst & 0xffff);
    }
    ins->func_=0;
    if(c->Jal_chk){
        ins->rd=31;
    }else if(ins->opcode==0){
        ins->rd = (ins->inst >> 11) & 0x1f;
    }else{
        ins->rd=(ins->inst >> 16) & 0x1f; // I type rt에 저장하니까 rd번호를 rt로 해줌.
    }
    // opcode 에 맞게 사용하는 레지스터 변경. 
    if(ins->opcode==0){
        ins->rs = (ins->inst >> 21) & 0x1f;
        ins->rt = (ins->inst >> 16) & 0x1f;
        ins->func_ = (ins->inst & 0x3f);
        ins->shamt = (ins->inst>>6) & (0x3f);
        //printf("[Decode] opcode(%02x) rs: %x rt: %x rd: %x shamt: %x funct(%x)\n",ins->opcode,ins->rs,ins->rt,ins->rd,ins->shamt,ins->func_);
    }else if(ins->opcode==2 || ins->opcode==3){
        ins->address=(ins->inst&0x3ffffff);
        ins->address=(ins->address << 2) + ((pc) & 0xf0000000);
        //printf("[Decode] opcode(%02x) address: %x\n",ins->opcode,ins->address);
        
    }else{
        ins->rs = (ins->inst >> 21) & 0x1f;
        ins->rt = (ins->inst >> 16) & 0x1f;
        //printf("[Decode] opcode(%02x) rs: %x rt: %x imm: %x\n",ins->opcode,ins->rs,ins->rt,ins->imm);
    }
    
   


}
