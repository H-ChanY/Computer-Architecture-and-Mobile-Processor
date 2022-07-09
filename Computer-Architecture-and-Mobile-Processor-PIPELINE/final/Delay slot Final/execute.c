#include "mips.h"

extern int Memory[0x400000];       //16MB memory
extern int reg[32];
extern int pc;
extern int num_inst;
extern int num_jump;
extern int num_memory;
extern int num_branch;

void Excute(IF_ID* if_id,ID_EX * id_ex, EX_MEM * ex_mem,EX_MEM * ex_mem2, MEM_WB * mem_wb) {
    // Format R
    memset(ex_mem,false,sizeof(EX_MEM));
    int forward_A=0;
    int forward_B=0;

    
    
    //forward a 에 대한 것
    if((id_ex->rs!=0)&&(mem_wb->write_reg !=ex_mem2->write_reg)&&mem_wb->write_reg ==id_ex->rs&& mem_wb->RegWrite){
        // 거리가 2일 때 rs와 rd가 같은 경우
        forward_A=2;
    }else if((id_ex->rs!=0)&&ex_mem2->write_reg==id_ex->rs  && ex_mem2->RegWrite) {
        // 거리가 1일 때 rs와 rd가 같은 경우
        forward_A=1;
    }

    //forward b 에 대한 것
    if ((id_ex->rt!=0)&&(mem_wb->write_reg !=ex_mem2->write_reg)&&mem_wb->write_reg ==id_ex->rt&& mem_wb->RegWrite){
        // 거리가 2일 때 rt와 rd가 같은 경우
        forward_B=2;
    }else if((id_ex->rt!=0)&&ex_mem2->write_reg==id_ex->rt&&ex_mem2->RegWrite){
            // 거리가 1일 때 rt와 rd가 같은 경우
        forward_B=1;
    }
        


    // rs값 구해주기
    if(forward_A==2){
        // 거리 2 rs와 rd가 같은 경우 
        if(mem_wb->MemtoReg){
            id_ex->read_data1=mem_wb->mem_read_data;
        }else{
            id_ex->read_data1=mem_wb->alu_result;
        }
    }else if(forward_A==1){
        // 거리 1 rs와 rd가 같은 경우 
        id_ex->read_data1=ex_mem2->alu_result; 
    }else if(forward_A==0){
        id_ex->read_data1=reg[id_ex->rs];
    }
    


    //rt 값 구하기
    if(forward_B==2){
        // 거리 2 rt와 rd가 같은 경우 
        if(mem_wb->MemtoReg){
            id_ex->read_data2=mem_wb->mem_read_data;
        }else{
            id_ex->read_data2=mem_wb->alu_result;
        } 
    }else if(forward_B==1){
        // 거리 1 rt와 rd가 같은 경우 
        id_ex->read_data2=ex_mem2->alu_result;
    }else if(forward_B==0){
        id_ex->read_data2=reg[id_ex->rt];
    }
    
    

    if(id_ex->Jal_chk){
        ex_mem->write_data=id_ex->pc_4+4;
    }else{
        ex_mem->write_data=id_ex->read_data2;
    }
    ex_mem->branch_addr = (id_ex->imm << 2 )+ id_ex->pc_4;

        if(id_ex->opcode==0){
            switch (id_ex->func) {
                case add:
                    ex_mem->alu_result = id_ex->read_data1 + id_ex->read_data2;
                    printf("[Excute] add alu_result= reg[%d] + reg[%d]   : 0x%x\n",id_ex->rs,id_ex->rt,ex_mem->alu_result);
                    num_inst+=1;
                    break;
                case addu:
                    ex_mem->alu_result = id_ex->read_data1 + id_ex->read_data2;
                    num_inst+=1;
                    printf("[Excute] addu alu_result= reg[%d] + reg[%d]   : 0x%x\n",id_ex->rs ,id_ex->rt,ex_mem->alu_result);
                    break; 
                case and :
                    ex_mem->alu_result = (id_ex->read_data1 & id_ex->read_data2);
                    num_inst+=1;
                    printf("[Excute] and alu_result= reg[%d] & reg[%d]   : 0x%x\n",id_ex->rs,id_ex->rt,ex_mem->alu_result);
                    break;
                case nor:
                    ex_mem->alu_result = ~(id_ex->read_data1 | id_ex->read_data2);
                    num_inst+=1;
                    printf("[Excute] nor alu_result= ~(reg[%d] | reg[%d])   : 0x%x\n", id_ex->rs,id_ex->rt,ex_mem->alu_result);
                    break;
                case or :
                    ex_mem->alu_result = (id_ex->read_data1 | id_ex->read_data2);
                    num_inst+=1;
                    printf("[Excute] or alu_result= ( reg[%d] | reg[%d] )   : 0x%x\n", id_ex->rs ,id_ex->rt,ex_mem->alu_result);
                    break;
                case slt:
                    ex_mem->alu_result = (id_ex->read_data1 < id_ex->read_data2);
                    num_inst+=1;
                    printf("[Excute] slt alu_result= (reg[%d] < reg[%d]) %d\n",id_ex->rs,id_ex->rt,ex_mem->alu_result);
                    break;
                case sltu:
                    ex_mem->alu_result = (id_ex->read_data1  < id_ex->read_data2);
                    num_inst+=1;
                    printf("[Excute] sltu alu_result= (reg[%d] < reg[%d] %d) \n",id_ex->rs ,id_ex->rt,ex_mem->alu_result);
                    break;
                case sll:
                    ex_mem->alu_result = (id_ex->read_data2 << id_ex->shamt);
                    if(id_ex->shamt!=0){
                        num_inst+=1;
                    }
                    printf("[Excute] sll alu_result= ( reg[%d] << 0x%x)   : 0x%x\n",id_ex->rt, id_ex->shamt,ex_mem->alu_result);
                    break;
                case srl:
                    ex_mem->alu_result = (id_ex->read_data2 >> id_ex->shamt);
                    num_inst+=1;
                    printf("[Excute] srl alu_result= ( reg[%d] >> 0x%x )   : 0x%x\n",id_ex->rt, id_ex->shamt,ex_mem->alu_result);
                    break;
                case sub: 
                    ex_mem->alu_result = (id_ex->read_data1- id_ex->read_data2);
                    num_inst+=1;
                    printf("[Excute] sub alu_result= ( reg[%d] - reg[%d] )   : 0x%x\n",id_ex->rs,id_ex->rt,ex_mem->alu_result);
                    break;
                case subu:
                    ex_mem->alu_result = (id_ex->read_data1-  id_ex->read_data2);
                    num_inst+=1;
                    printf("[Excute] subu alu_result= ( reg[%d] - reg[%d] )   : 0x%x\n",id_ex->rs, id_ex->rt,ex_mem->alu_result);
                    break;
                default:
                    printf("THIS IS EXCUTE R Type\n");
                    break;
                }
        }else{
            switch (id_ex->opcode) {
                case addi:
                    ex_mem->alu_result = id_ex->read_data1+id_ex->imm;
                    num_inst+=1;
                    printf("[Excute] addi alu_result= ( reg[%d] + imm(%d) )   : 0x%x\n",id_ex->rs, id_ex->imm, ex_mem->alu_result);
                    break;
                case addiu:
                    ex_mem->alu_result = id_ex->read_data1+id_ex->imm;
                    num_inst+=1;
                    printf("[Excute] addiu alu_result= ( reg[%d] + imm(%d) )   : 0x%x\n",id_ex->rs,id_ex->imm,ex_mem->alu_result);
                    break;
                case andi:
                    ex_mem->alu_result = id_ex->read_data1&id_ex->imm;
                    num_inst+=1;
                    printf("[Excute] andi alu_result= ( reg[%d] & imm(0x%x) )   : 0x%x\n",id_ex->rs , id_ex->imm,ex_mem->alu_result);
                    break;
                case lui:
                    ex_mem->alu_result = (id_ex->imm << 16);
                    num_inst+=1;
                    printf("[Excute] lui alu_result imm(0x%x) << 16   : 0x%x\n",id_ex->imm,ex_mem->alu_result);
                    break;
                case lw:
                    ex_mem->alu_result = (id_ex->read_data1+id_ex->imm);
                    num_memory+=1;
                    num_inst+=1;
                    printf("[Excute] lw alu_result reg[%d] + imm(0x%x)   : 0x%x\n",id_ex->rs ,id_ex->imm,ex_mem->alu_result);
                    break;
                case ori:
                    ex_mem->alu_result = (id_ex->read_data1 | id_ex->imm);
                    num_inst+=1;
                    printf("[Excute] ori alu_result reg[%d] | imm(0x%x)   : 0x%x\n",id_ex->rs,id_ex->imm,ex_mem->alu_result);
                    break;
                case slti:
                    ex_mem->alu_result = id_ex->read_data1<id_ex->imm;
                    num_inst+=1;
                    printf("[Excute] slti alu_result reg[%d] < imm(0x%x)   : 0x%x\n",id_ex->rs , id_ex->imm,ex_mem->alu_result);
                    break;
                case sltiu:
                    ex_mem->alu_result = id_ex->read_data1<id_ex->imm;
                    num_inst+=1;
                    printf("[Excute] sltiu alu_result reg[%d] < imm(0x%x)   : 0x%x\n",id_ex->rs , id_ex->imm,ex_mem->alu_result);
                    break;
                case sw:
                    ex_mem->alu_result = id_ex->read_data1+id_ex->imm;
                    num_memory+=1;
                    num_inst+=1;
                    printf("[Excuete] sw alu_result reg[%d] + imm(0x%x)   = 0x%x\n",id_ex->rs , id_ex->imm,ex_mem->alu_result);
                    break;
                case beq:
                    ex_mem->zero_ = ((id_ex->read_data1-  id_ex->read_data2) == 0);
                    num_inst+=1;
                    num_branch+=1;
                    printf("[Excute] beq alu_result reg[%d] == reg[%d] (%d) \n",id_ex->rs,id_ex->rt,ex_mem->zero_);
                    break;
                case bne:
                    ex_mem->zero_ = ((id_ex->read_data1-  id_ex->read_data2) != 0);
                    num_inst+=1;
                    num_branch+=1;
                    printf("[Excute] bne alu_result reg[%d] != reg[%d] (%d)\n",id_ex->rs,id_ex->rt,ex_mem->zero_);
                    break;
                default:
                    printf("THIS IS EXCUTE I Type\n");
                    break;
                }
        }    
                
            
        
    
    
    
        
        if(id_ex->RegDest){
            ex_mem->write_reg=id_ex->rd;
        }else if (id_ex->Jal_chk){
            ex_mem->write_reg=31;
        }else{
            ex_mem->write_reg=id_ex->rt;
        }


        
        ex_mem->Jal_chk=id_ex->Jal_chk;
        ex_mem->RegWrite=id_ex->RegWrite;
        ex_mem->MemtoReg=id_ex->MemtoReg;
        ex_mem->pc_4=id_ex->pc_4;
        ex_mem->Br_taken=id_ex->Br_taken;
        ex_mem->MemWrite=id_ex->MemWrite;
        ex_mem->MemRead=id_ex->MemRead;
        branch_pc(if_id,ex_mem);
        


}