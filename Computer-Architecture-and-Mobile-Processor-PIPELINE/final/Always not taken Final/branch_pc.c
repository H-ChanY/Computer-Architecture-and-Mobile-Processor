#include "mips.h"

extern int Memory[0x400000];       //16MB memory
extern int reg[32];
extern int pc;
extern BTB btb_[100];
extern int num_br;

void branch_pc(IF_ID *if_id,ID_EX* id_ex,ID_EX* id_ex1 ,EX_MEM * ex_mem){
    // 첫 시작 
    int index=0;
    int chk=0;    
        if(id_ex->opcode==bne||id_ex->opcode==beq){
            for(int k=0; k<=num_br;k++){
                // 현재 pc가 branch_pc에 있는가 확인해야함. 
                if(btb_[k].branch_pc==ex_mem->pc_4-4){
                    chk=1;
                    index=k;
                    break;
                }
            }
            if(chk==0){
                btb_[num_br].branch_pc=ex_mem->pc_4-4;
                btb_[num_br].Target=ex_mem->branch_addr;
                if(ex_mem->zero_){
                    //branch taken이다. 
                    pc=ex_mem->branch_addr;
                    memset(if_id,false,sizeof(IF_ID));
                    memset(id_ex1,false,sizeof(ID_EX));
                }   
                num_br+=1;
            }
        }
        if(chk!=0 && ex_mem->zero_&& id_ex->Br_taken){
                pc=btb_[index].Target;
                memset(if_id,false,sizeof(IF_ID));
                memset(id_ex1,false,sizeof(ID_EX));
                num_fail+=1;
        }else if(chk!=0&&!ex_mem->zero_&& id_ex->Br_taken ){
            num_success+=1;
            num_not_branch+=1;
        }

}