#include "mips.h"
extern data_Cache_ data_cache[256];
extern int data_hit;
extern int data_miss;
//direct mapping이면 replacement가 필요. 
// set associate 할 때 필요함. 
void data_cache_access(inst_ *ins, Control_ * c){
    int index=(ins->ALU_Result>>6)&0xff;
    int tag=(ins->ALU_Result>>14)&0x3ffff;
    int offset=ins->ALU_Result&0x3f;
    if(c->MemRead){
        if(data_cache[index].valid!=1){
            ins->write_data=Memory[ins->ALU_Result/4];
            data_miss++;
            data_cache[index].tag=tag;
            data_cache[index].second=0;
            data_cache[index].valid=1;
            int a=((ins->ALU_Result)/64)*16;
            for(int i=0;i<16;i++){
                data_cache[index].data_cache_line[i].data=Memory[(a)+i];
            }
        }
        else if(data_cache[index].tag==tag){
            ins->write_data=data_cache[index].data_cache_line[offset/4].data;
            data_cache[index].second=1;
            //printf("hit\n");
            data_hit++;  
        }else{
            data_miss++;
            ins->write_data=Memory[ins->ALU_Result/4];
            
            if(data_cache[index].second==1){
                data_cache[index].second=0;
            }else{
                int return_address=(((data_cache[index].tag<<14)+(index<<6)))/4;
                data_cache[index].tag=tag;
                int a=(ins->ALU_Result/64)*16;
                if(data_cache[index].dirty==1){    
                    for(int i=0;i<16;i++){
                        Memory[return_address+i]=data_cache[index].data_cache_line[i].data;
                        data_cache[index].data_cache_line[i].data=Memory[a+i];
                    }
                    data_cache[index].dirty=0;                
                }else{
                    for(int i=0;i<16;i++){
                        data_cache[index].data_cache_line[i].data=Memory[a+i];
                    }
                }   
            }
        }
   }else if(c->MemWrite){
        if (data_cache[index].valid!=1){
            data_miss++;
            Memory[(ins->ALU_Result)/4]=reg[ins->rt];
            data_cache[index].tag=tag;
            data_cache[index].valid=1;
            data_cache[index].dirty=0;
            int a=((ins->ALU_Result)/64)*16;
            for(int i=0;i<16;i++){
                data_cache[index].data_cache_line[i].data=Memory[(a)+i];
            }
        }else if(data_cache[index].tag==tag){
            data_cache[index].second=1;
            data_cache[index].data_cache_line[offset/4].data=reg[ins->rt];
            data_cache[index].second=1;
            data_cache[index].dirty=1;
            data_hit++;
        }else{
            //여기에도 넣어줘야함. 만약에 접근을 하면 캐시에 담아줘야함. 
            data_miss++;
            Memory[ins->ALU_Result/4]=reg[ins->rt];
            if(data_cache[index].second==1){
                data_cache[index].second=0;
            }else{
                int a=(ins->ALU_Result/64)*16;
                int return_address=(((data_cache[index].tag<<14)+(index<<6)))/4;
                data_cache[index].tag=tag;
                if(data_cache[index].dirty==1){
                    for(int i=0;i<16;i++){
                        Memory[return_address+i]=data_cache[index].data_cache_line[i].data;
                        data_cache[index].data_cache_line[i].data=Memory[a+i];
                }
                data_cache[index].dirty=0;
                }else{
                    for(int i=0;i<16;i++){
                        data_cache[index].data_cache_line[i].data=Memory[a+i];
            }
            }
            }
            
        }
   }
}
