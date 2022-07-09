#include "mips.h"
extern int hit;
extern int miss;
extern int pc;
extern inst_Cache_ inst_cache[256];

void fetch(inst_ *ins) {

    //이것도 inst_cache로 나누기. 
    int index=(pc>>6)&0xff;
    int tag=(pc>>14)&0x3ffff;
    int offset=pc&0x3f;
    int start_cache=(pc-offset)/4;
    // 캐시라인이 비워져있다.
    if(inst_cache[index].valid!=1){
        miss++;
        inst_cache[index].valid=1;
        inst_cache[index].tag=tag;
        ins->inst=Memory[pc/4];
        for(int i =0; i<16; i++){
            inst_cache[index].inst_cache_line[i].data = Memory[start_cache+i];
        }
    }else if(inst_cache[index].valid==1){
        if(inst_cache[index].tag==tag){//hit
            hit++;
            ins->inst=inst_cache[index].inst_cache_line[offset/4].data;
        }else{
            miss++;
            ins->inst=Memory[pc/4];
            for(int i =0;i<16;i++){
                inst_cache[index].inst_cache_line[i].data=Memory[start_cache+i];
            }
            inst_cache[index].tag=tag;
            
        }
    }
    //printf("inst 0x%0x\n",ins->inst);
    pc =pc + 4;
}

