//
//  main.m
//  Project2
//
//  Created by 허찬용 on 2022/04/01.
//

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
extern int num_success;
extern int num_fail;


int main() {
    FILE* fp = NULL;

    // 읽는 거 확인
    int ret = 0;

    // read instruction 
    unsigned char _in[4] = {0};
    int* p = NULL; // _in을 가르킬 예정

    int fin = 0;
    
    reg[29] = 0x1000000;
    reg[31] = -1;
    num_br=0;
    // 넘겨줄 때 IF_ID[0]->ins 이런식이면 될 듯
    memset(&btb_,0,sizeof(BTB)*100);

    // open the binary input file
    fp = fopen("sample/input4.bin", "rb");

    int index;
    // 메모리에 전부 저장.
    for (index = 0;; index++) {
        for (int i = 0; i < 4; i++) {
            // swap bytes
            ret = fread(&_in[3 - i], 1, 1, fp);
            if (ret != 1) {
                fin = 1;
                break;
            }
        }
        if (fin == 1) break;
        p = &_in[0]; // p는 instruction을 가르킴
        // print value
        Memory[index] = *p; // 메모리에 모든 instruction을 저장한다. 
    }


// latch 구현

    IF_ID if_id[2];
    ID_EX id_exe[2];
    EX_MEM exe_mem[2];
    MEM_WB mem_wb[2];
    memset(&if_id, false, sizeof(IF_ID)*2);
    memset(&id_exe, false, sizeof(ID_EX)*2);
    memset(&exe_mem, false, sizeof(EX_MEM)*2);
    memset(&mem_wb, false, sizeof(MEM_WB)*2);
    int cycle = 1;

    num_inst=0;
    num_jump=0;
    num_memory=0;
    num_branch=0;
    num_not_branch=0;
    num_success=0;
    num_fail=0;

    while (1) {
        if (pc == -1){
                cycle++;
                Excute(&if_id[0],&id_exe[1],&id_exe[0],&exe_mem[0],&exe_mem[1],&mem_wb[1]);
                memory_access(&exe_mem[1],&mem_wb[0]);
                write_back(&mem_wb[1]);
                cycle++;
                exe_mem[1]=exe_mem[0];
                mem_wb[1]=mem_wb[0];
                memory_access(&exe_mem[1],&mem_wb[0]);
                write_back(&mem_wb[1]);
                mem_wb[1]=mem_wb[0];
                cycle++;
                write_back(&mem_wb[1]);
                
            break; 
        }    

        
        //printf("%d\n\n",cycle);
        fetch(&if_id[0]);
        Decode(&if_id[1],&id_exe[0],&mem_wb[1]);
        Excute(&if_id[0],&id_exe[1],&id_exe[0],&exe_mem[0],&exe_mem[1],&mem_wb[1]);
        memory_access(&exe_mem[1],&mem_wb[0]);
        write_back(&mem_wb[1]);
        cycle++;
        if_id[1]=if_id[0];
        id_exe[1]=id_exe[0];
        exe_mem[1]=exe_mem[0];
        mem_wb[1]=mem_wb[0];
        
        memset(&if_id, false, sizeof(IF_ID));
        memset(&id_exe, false, sizeof(ID_EX));
        memset(&exe_mem, false, sizeof(EX_MEM));
        memset(&mem_wb, false, sizeof(MEM_WB));
    }
    printf("Final result: %d\n",reg[2]);
    printf("Total number of excution cycles: %d\n",cycle);
    printf("Total number of instructions: %d\n",num_inst);
    printf("Total number of memory ops %d\n",num_memory);
    printf("Total number of branches ops %d\n",num_branch);
    printf("Total number of not-taken branches ops %d\n",num_not_branch);
    printf("Total number of jump %d\n",num_jump);
    printf("Total number of predict success %d\n",num_success);
    printf("Total number of predict fail %d\n",num_fail);
    fclose(fp);
    return 0;
}

