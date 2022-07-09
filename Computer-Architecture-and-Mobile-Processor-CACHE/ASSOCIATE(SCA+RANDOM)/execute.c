
#include "mips.h"

void Excute(inst_ *ins, Control_* c) {
    // Format R
    if (c->ALUSrc == false){
        ins->input_1=reg[ins->rs];
        ins->input_2=reg[ins->rt];
    }else{
        ins->input_1=reg[ins->rs];
        ins->input_2=ins->imm;
    }
    ins->Branch_target = (ins->imm << 2 )+ pc;
        if(ins->opcode==0){
            switch (ins->func_) {
                case add:
                    ins->ALU_Result = ins->input_1 + ins->input_2;
                    //printf("[Excute] ADD ALU_Result= reg[%d] + reg[%d]   : 0x%x\n",ins->rs,ins->rt,ins->ALU_Result);
                    break;
                case addu:
                    ins->ALU_Result = ins->input_1 + ins->input_2;
                    //printf("[Excute] ADDU ALU_Result= reg[%d] + reg[%d]   : 0x%x\n",ins->rs ,ins->rt,ins->ALU_Result);
                    break; 
                case and :
                    ins->ALU_Result = (ins->input_1 & ins->input_2);
                    //printf("[Excute] AND ALU_Result= reg[%d] & reg[%d]   : 0x%x",ins->rs,ins->rt,ins->ALU_Result);
                    break;
                case nor:
                    ins->ALU_Result = ~(ins->input_1 | ins->input_2);
                    //printf("[Excute] NOR ALU_Result= ~(reg[%d] | reg[%d])   : 0x%x", ins->rs,ins->rt,ins->ALU_Result);
                    break;
                case or :
                    ins->ALU_Result = (ins->input_1 | ins->input_2);
                    //printf("[Excute] OR ALU_Result= ( reg[%d] | reg[%d] )   : 0x%x", ins->rs ,ins->rt,ins->ALU_Result);
                    break;
               
                case slt:
                    ins->ALU_Result = (ins->input_1 < ins->input_2);
                    //printf("[Excute] SLT ALU_Result= (reg[%d] < reg[%d]) %d\n",ins->rs,ins->rt,ins->ALU_Result);
                    break;
                case sltu:
                    ins->ALU_Result = (ins->input_1  < ins->input_2);
                    //printf("[Excute] SLTU ALU_Result= (reg[%d] < reg[%d] %d) ",ins->rs ,ins->rt,ins->ALU_Result);
                    break;
                case sll:
                    ins->ALU_Result = (ins->input_2 << ins->shamt);
                    //printf("[Excute] SLL ALU_Result= ( reg[%d] << 0x%x)   : 0x%x\n",ins->rt, ins->shamt,ins->ALU_Result);
                    break;
                case srl:
                    ins->ALU_Result = (ins->input_2 >> ins->shamt);
                    //printf("[Excute] SRL ALU_Result= ( reg[%d] >> 0x%x )   : 0x%x",ins->rt, ins->shamt,ins->ALU_Result);
                    break;
                case sub: 
                    ins->ALU_Result = (ins->input_1- ins->input_2);
                    //printf("[Excute] SUB ALU_Result= ( reg[%d] - reg[%d] )   : 0x%x",ins->rs,ins->rt,ins->ALU_Result);
                    break;
                case subu:
                    ins->ALU_Result = (ins->input_1-  ins->input_2);
                    //printf("[Excute] SUBU ALU_Result= ( reg[%d] - reg[%d] )   : 0x%x",ins->rs, ins->rt,ins->ALU_Result);
                    break;
                default:
                    break;
                }
        }    
                
            
        
    // Format I
    
    switch (ins->opcode) {
        case addi:
            ins->ALU_Result = ins->input_1+ins->input_2;
            //printf("[Excute] ADDI ALU_Result= ( reg[%d] + imm(%d) )   : 0x%x\n",ins->rs, ins->imm, ins->ALU_Result);
            break;
        case addiu:
            ins->ALU_Result = ins->input_1+ins->input_2;
            //printf("[Excute] ADDIU ALU_Result= ( reg[%d] + imm(%d) )   : 0x%x\n",ins->rs,ins->imm,ins->ALU_Result);
            break;
        case andi:
            ins->ALU_Result = ins->input_1&ins->input_2;
            //printf("[Excute] ANDI ALU_Result= ( reg[%d] & imm(0x%x) )   : 0x%x",ins->rs , ins->imm,ins->ALU_Result);
            break;
        case lui:
            ins->ALU_Result = (ins->input_2 << 16);
            //printf("[Excute] LUI ALU_Result imm(0x%x) << 16   : 0x%x",ins->imm,ins->ALU_Result);
            break;
        case lw:
            ins->ALU_Result = (ins->input_1+ins->input_2);
            //printf("[Excute] LW ALU_Result reg[%d] + imm(0x%x)   : 0x%x\n",ins->rs ,ins->imm,ins->ALU_Result);
            break;
        case ori:
            ins->ALU_Result = (ins->input_1 | ins->input_2);
            //printf("[Excute] ORI ALU_Result reg[%d] | imm(0x%x)   : 0x%x",ins->rs,ins->imm,ins->ALU_Result);
            break;
        case slti:
            ins->ALU_Result = ins->input_1<ins->input_2;
            //printf("[Excute] SLTI ALU_Result reg[%d] < imm(0x%x)   : 0x%x",ins->rs , ins->imm,ins->ALU_Result);
            break;
        case sltiu:
            ins->ALU_Result = ins->input_1<ins->input_2;
            //printf("[Excute] SLTIU ALU_Result reg[%d] < imm(0x%x)   : 0x%x",ins->rs , ins->imm,ins->ALU_Result);
            break;
        case sw:
            ins->ALU_Result = ins->input_1+ins->input_2;
            //printf("[Excute] SW ALU_Result reg[%d] + imm(0x%x)   = 0x%x\n",ins->rs , ins->imm,ins->ALU_Result);
            break;
        case beq:
            ins->ALU_Result = ((ins->input_1-  ins->input_2) == 0);
            c->Br_taken=(c->Br_taken && ins->ALU_Result);
            //printf("[Excute] BEQ ALU_Result reg[%d] == reg[%d] (%d) ",ins->rs,ins->rt,ins->ALU_Result);
            break;
        case bne:
            ins->ALU_Result = ((ins->input_1-  ins->input_2) != 0);
            c->Br_taken=(c->Br_taken && ins->ALU_Result);
            //printf("[Excute] BNE ALU_Result reg[%d] != reg[%d] (%d)",ins->rs,ins->rt,ins->ALU_Result);
            break;
        default:
            break;
        }
        
    


}
