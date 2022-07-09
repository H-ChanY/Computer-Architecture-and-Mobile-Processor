#include "mips.h"

extern int Memory[0x400000];       //16MB memory
extern int reg[32];
extern int pc;
extern BTB btb_[100];
extern int num_br;

void fetch(IF_ID* if_id) {
    int valid=1;
    
    //fetch를 해준다. 
    if_id->inst = Memory[pc / 4];
    if_id->pc_4=pc+4;
    printf("[Fetch] pc[0x%X] instruction 0x%08X \n", pc, if_id->inst);
    //pc를 업데이트 한다. 
    //branch인지 아닌지 확인을 한다. 
    for(int i=0;i<=num_br;i++){
        if(pc==btb_[i].branch_pc && pc!=0){
            pc=btb_[i].branch_pc+4;
            valid=0;
            break;
        }       
    }

    if(valid){
        pc =pc + 4;
    }
      


}
    