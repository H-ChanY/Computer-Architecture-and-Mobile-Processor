#include "mips.h"


extern int Memory[0x400000];       //16MB memory
extern int reg[32];
extern int pc;
extern BTB btb_[100];
extern int num_br;
extern int num_inst;
extern int num_jump;
extern int num_memory;
extern int num_branch;
extern int num_not_branch;

void Decode(IF_ID *if_id,ID_EX * id_ex,MEM_WB* mem_wb) {
    memset(id_ex,0,sizeof(ID_EX));
    id_ex->opcode = ((if_id->inst >> 26) & 0x3f);
    id_ex->func = (if_id->inst & 0x3f);
    
    Control(id_ex);
    
    id_ex->func = 0;
    
    if(id_ex->sign_imm){
        if (((if_id->inst >> 15) & 0x1) == 1) id_ex->imm =  0xffff0000 + (if_id->inst & 0xffff);
        else id_ex->imm = 0x00000000 + (if_id->inst & 0xffff);
    }else{
        id_ex->imm = 0x00000000 + (if_id->inst & 0xffff);
    }
    // opcode 에 맞게 사용하는 레지스터 변경. 
    if(id_ex->opcode==0){
        id_ex->rs = (if_id->inst >> 21) & 0x1f;
        id_ex->rt = (if_id->inst >> 16) & 0x1f;
        id_ex->rd = (if_id->inst >> 11) & 0x1f;
        id_ex->shamt = (if_id->inst>>6) & (0x3f);
        id_ex->func = (if_id->inst & 0x3f);

        // 거리가 3인 경우 
        
        if(id_ex->rs!=0 && id_ex->rs == mem_wb->write_reg&&mem_wb->RegWrite && mem_wb->MemtoReg){
            id_ex->read_data1=mem_wb->mem_read_data;
            reg[id_ex->rs]=mem_wb->mem_read_data;
        }else if(id_ex->rs == mem_wb->write_reg&&mem_wb->RegWrite){
            id_ex->read_data1=mem_wb->alu_result;
            reg[id_ex->rs]=mem_wb->alu_result;
        }else{
            id_ex->read_data1=reg[id_ex->rs];
        }
        

        if(id_ex->rt == mem_wb->write_reg&&mem_wb->RegWrite && mem_wb->MemtoReg){
            id_ex->read_data2=mem_wb->mem_read_data;
            reg[id_ex->rt]=mem_wb->mem_read_data;
        }else if(id_ex->rt ==mem_wb->write_reg&& mem_wb->RegWrite){
            id_ex->read_data2=mem_wb->alu_result;
            reg[id_ex->rt]=mem_wb->alu_result;
        }else{
            id_ex->read_data2=reg[id_ex->rt];
        }

        if(id_ex->Jr_chk){
            pc=reg[id_ex->rs];
            num_jump+=1;
        }
        
    }else if (id_ex->opcode==2 || id_ex->opcode==3){
        id_ex->address=if_id->inst&0x3ffffff;
        id_ex->address=(id_ex->address<<2) + (if_id->pc_4& 0xf0000000);
    }else{
        
        id_ex->rs = (if_id->inst >> 21) & 0x1f;
        id_ex->rt = (if_id->inst >> 16) & 0x1f;
        
        if(id_ex->rt == mem_wb->RegWrite && mem_wb->MemtoReg){
            id_ex->read_data2=mem_wb->mem_read_data;
        }else if(id_ex->rt == mem_wb->RegWrite){
            id_ex->read_data2=mem_wb->write_reg_data;
        }else{
            id_ex->read_data2=reg[id_ex->rt];
        }
    }

    
    id_ex->pc_4=if_id->pc_4;

    if(id_ex->Jal_chk || id_ex->Jump_chk){
        // jal 이거나 jump 같은 경우는 decode에서 다 결정남. 
        pc=id_ex->address;
        num_jump+=1;
    }
    

    if(id_ex->sign_imm){
        if (((if_id->inst >> 15) & 0x1) == 1) id_ex->imm =  0xffff0000 + (if_id->inst & 0xffff);
        else id_ex->imm = 0x00000000 + (if_id->inst & 0xffff);
    }else{
        id_ex->imm = 0x00000000 + (if_id->inst & 0xffff);
    }
    //printf("[Decode] opcode(%02x) rs: %x rt: %x rd: %x shamt: %x imm %x funct(%x)\n",id_ex->opcode,id_ex->rs,id_ex->rt,id_ex->rd,id_ex->shamt,id_ex->imm,id_ex->func);

}


