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
                    btb_[num_br].PHT[btb_[num_br].GHR]=2;
                    pc=ex_mem->branch_addr;
                    memset(if_id,false,sizeof(IF_ID));
                    memset(id_ex1,false,sizeof(ID_EX));
                }else{
                    //branch not taken이다 .
                    btb_[num_br].PHT[btb_[num_br].GHR]=1;
                }    
                
                num_br+=1;
            }
        }
    
    

    // branch prediction 의 성공 여부에 대해서 확인을 해야함. 
    // 어디로 갔는지 그러면 bit를 확인해야함. 
    if(chk!=0&&ex_mem->zero_&& ex_mem->Br_taken && btb_[index].PHT[btb_[index].GHR]>=2){
            //branch 성공한 경우 그냥 놔두면 됨 어차피 예측 성공이니까.
            //pc=ex_mem->branch_addr;
            if(btb_[index].PHT[btb_[index].GHR]<3){
                btb_[index].PHT[btb_[index].GHR]+=1;
            }
            num_success+=1;
    }
    else if(chk!=0&&ex_mem->zero_&& ex_mem->Br_taken && btb_[index].PHT[btb_[index].GHR]<2){
        // branch prediction 실패
        // 실제로는 branch 하지만 branch 되지 않은 경우 
        // prediction 실패하면 fetch decode 부분 실행하면 안된다. flush 해줘야한다. 
        // prediction_bit 한개 줄여줘야 한다. 
        if(btb_[index].PHT[btb_[index].GHR]<3){
                btb_[index].PHT[btb_[index].GHR]+=1;
            }
        num_fail+=1;
        pc=btb_[index].Target;
        memset(if_id,false,sizeof(IF_ID));
        memset(id_ex1,false,sizeof(ID_EX));           
    }
    else if(chk!=0&&!ex_mem->zero_&& ex_mem->Br_taken && btb_[index].PHT[btb_[index].GHR]<2){
        // br not taken 정답
        if(btb_[index].PHT[btb_[index].GHR]>0){
                btb_[index].PHT[btb_[index].GHR]-=1;
            } 
        num_not_branch+=1;
        num_success+=1;
    }
    else if(chk!=0&&!ex_mem->zero_&& ex_mem->Br_taken && btb_[index].PHT[btb_[index].GHR]>=2){
        if(btb_[index].PHT[btb_[index].GHR]>0){
                btb_[index].PHT[btb_[index].GHR]-=1;
            }
        pc=btb_[index].branch_pc+4;
        num_not_branch+=1;
        memset(if_id,false,sizeof(IF_ID));
        memset(id_ex1,false,sizeof(ID_EX));    
        num_fail+=1; 
    }
    if(chk!=0&&ex_mem->Br_taken){
        btb_[index].GHR<<=1;
        btb_[index].GHR|=ex_mem->zero_;
    }else if(chk=0&&ex_mem->Br_taken){
        btb_[num_br].GHR<<=1;
        btb_[num_br].GHR|=ex_mem->zero_;
    }
}