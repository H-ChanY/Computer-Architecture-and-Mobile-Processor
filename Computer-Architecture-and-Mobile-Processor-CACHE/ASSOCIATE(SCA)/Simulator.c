//
//  main.m
//  Project2
//
//  Created by 허찬용 on 2022/04/01.
//

// address 에 넣을때  inst는 /4 해서 메모리에 접근해야함 lw 할 때

#include "mips.h"

extern int Memory[0x100000];           //16MB memory
extern int reg[32];
extern int pc;

extern data_Cache_ data_cache[256];
extern Set_Cache Set_cache[4];
extern int miss;
extern int hit;

int main() {
    FILE* fp = NULL;

    // 읽는 거 확인
    int ret = 0;

    // read instruction 
    unsigned char _in[4] = {0};
    int* p = NULL; // _in을 가르킬 예정

    int fin = 0;
    
    reg[29] = 0x400000;
    reg[31] = -1;

    inst_* ins;
    Control_ * c; 
    ins=malloc(sizeof(*ins));
    c=malloc(sizeof(*c));
    memset(&data_cache,-1,sizeof(data_Cache_)*256);
    memset(&Set_cache,-1,sizeof(Set_Cache)*4);
    // open the binary input file
    fp = fopen("sample/fib.bin", "rb");

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

    int k=0;
    while (1) {
        if (pc == -1) break;
        k++;
        memset(ins, 0, sizeof(inst_));
        memset(c, false, sizeof(Control_));
        //printf("\ncycle %d \n",k);
        fetch(ins);
        Decode(ins,c);
        Excute(ins, c);
        memory_access(ins,c);
        write_back(ins,c);
        pc_update(ins, c);

    }
    printf("cycle %d",k);
    printf("\n%d\n\n",reg[2]);
    printf("\nhit %d",hit);
    printf("\nmiss %d",miss);
    printf("\ndata hit %d",data_hit);
    printf("\ndata miss %d",data_miss);
    free(ins);
    free(c);
    fclose(fp);
    return 0;
}

