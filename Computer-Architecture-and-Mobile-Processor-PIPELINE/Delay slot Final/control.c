#include "mips.h"

extern int Memory[0x400000];       //16MB memory
extern int reg[32];
extern int pc;

void Control(ID_EX * id_ex) {

    if (id_ex->opcode == 0 ) id_ex->RegDest = true;
    if(id_ex->opcode!=ori && id_ex->opcode!=andi) id_ex->sign_imm=true;
    if (id_ex->opcode != 0 && id_ex->opcode != beq && id_ex->opcode != bne) id_ex->ALUSrc = true;
    if (id_ex->opcode == lw || id_ex->opcode == ll || id_ex->opcode == lbu || id_ex->opcode == lhu) id_ex->MemtoReg = true;
    if (id_ex->opcode != sw && id_ex->opcode != bne && id_ex->opcode != beq && id_ex->opcode != j ) {
        if(id_ex->opcode==0 && id_ex->func == jr ){
            id_ex->RegWrite = false;
        }else{
            id_ex->RegWrite = true;
        }
    }
    if (id_ex->opcode == lw) id_ex->MemRead = true;
    if (id_ex->opcode == sw || id_ex->opcode == sh || id_ex->opcode == sb) id_ex->MemWrite = true;
    if (id_ex->opcode == j) id_ex->Jump_chk = true;
    if (id_ex->opcode == beq ) id_ex->Br_taken = true;
    else if (id_ex->opcode == bne ) id_ex->Br_taken = true;
    if(id_ex->opcode==0 && id_ex->func==jr) id_ex->Jr_chk=true;
    if (id_ex->opcode==jal) {
        id_ex->Jal_chk=true;
        id_ex->RegWrite=true;    
    }
}
