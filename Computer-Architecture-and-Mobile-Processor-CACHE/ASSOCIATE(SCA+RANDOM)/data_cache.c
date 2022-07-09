#include "mips.h"
extern data_Cache_ data_cache[256];
extern Set_Cache Set_cache[4];
extern int data_hit;
extern int data_miss;
//direct mapping이면 replacement가 필요. 
// set associate 할 때 필요함.

void hit_cache(int set_hit,int offset,int index, inst_* ins, Control_*c){
    data_hit++;  
    if(c->MemRead){
        Set_cache[set_hit].cache_line[index].second=1;
        ins->write_data= Set_cache[set_hit].cache_line[index].data_cache_line[offset/4].data;
    }else if(c->MemWrite){
        Set_cache[set_hit].cache_line[index].second=1;
        Set_cache[set_hit].cache_line[index].data_cache_line[offset/4].data=reg[ins->rt];
        Set_cache[set_hit].cache_line[index].dirty=1;
    }
}


void miss_cache(int set_index,int index, int tag, inst_* ins, Control_* c){
    data_miss++;
    if(c->MemRead){
        ins->write_data=Memory[ins->ALU_Result/4];
        Set_cache[set_index].cache_line[index].tag=tag;
        Set_cache[set_index].cache_line[index].second=0;
        Set_cache[set_index].cache_line[index].valid=1;
        Set_cache[set_index].cache_line[index].dirty=0;
        int a=((ins->ALU_Result)/64)*16;
        for(int i=0;i<16;i++){
            Set_cache[set_index].cache_line[index].data_cache_line[i].data=Memory[(a)+i];
        }
    }else if(c->MemWrite){
        Memory[(ins->ALU_Result)/4]=reg[ins->rt];
        Set_cache[set_index].cache_line[index].tag=tag;
        Set_cache[set_index].cache_line[index].valid=1;
        Set_cache[set_index].cache_line[index].second=0;
        Set_cache[set_index].cache_line[index].dirty=0;
        int a=((ins->ALU_Result)/64)*16;
        for(int i=0;i<16;i++){
            Set_cache[set_index].cache_line[index].data_cache_line[i].data=Memory[(a)+i];
        }
    }
}

void replace(int set_replace,int index, int tag, inst_* ins, Control_* c){
    data_miss++;
    if(c->MemRead){
        ins->write_data=Memory[ins->ALU_Result/4];
        if(Set_cache[set_replace].cache_line[index].second==1){
            Set_cache[set_replace].cache_line[index].second=0;
        }else{
            int return_address=(((Set_cache[set_replace].cache_line[index].tag<<14)+(index<<6)))/4;
            Set_cache[set_replace].cache_line[index].tag=tag;
            int a=(ins->ALU_Result/64)*16;
            if(Set_cache[set_replace].cache_line[index].dirty==1){    
                for(int i=0;i<16;i++){
                    Memory[return_address+i]=Set_cache[set_replace].cache_line[index].data_cache_line[i].data;
                    Set_cache[set_replace].cache_line[index].data_cache_line[i].data=Memory[a+i];
                }
                Set_cache[set_replace].cache_line[index].dirty=0;                
            }else{
                for(int i=0;i<16;i++){
                    Set_cache[set_replace].cache_line[index].data_cache_line[i].data=Memory[a+i];
                }
            }   
        }
    }
    else if(c->MemWrite){
        Memory[ins->ALU_Result/4]=reg[ins->rt];
        if(Set_cache[set_replace].cache_line[index].second==1){
            Set_cache[set_replace].cache_line[index].second=0;
        }else{
            int a=(ins->ALU_Result/64)*16;
            int return_address=(((Set_cache[set_replace].cache_line[index].tag<<14)+(index<<6)))/4;
            Set_cache[set_replace].cache_line[index].tag=tag;
            if(Set_cache[set_replace].cache_line[index].dirty==1){
                for(int i=0;i<16;i++){
                    Memory[return_address+i]=Set_cache[set_replace].cache_line[index].data_cache_line[i].data;
                    Set_cache[set_replace].cache_line[index].data_cache_line[i].data=Memory[a+i];
                }
                Set_cache[set_replace].cache_line[index].dirty=0;
            }else{
                for(int i=0;i<16;i++){
                    Set_cache[set_replace].cache_line[index].data_cache_line[i].data=Memory[a+i];
                }
            }
            }
    }
}




void data_cache_access(inst_ *ins, Control_ * c){
    int index=(ins->ALU_Result>>6)&0xff;
    int tag=(ins->ALU_Result>>14)&0x3ffff;
    int offset=ins->ALU_Result&0x3f;
    int set_index=-1;
    int set_hit=-1;
    int set_replace=-1;
    int no_chance[4]={0,0,0,0};
    int count=0;
    for(int i=0;i<4;i++){
        if(Set_cache[i].cache_line[index].tag==tag && Set_cache[i].cache_line[index].valid==1) break;
        if(Set_cache[i].cache_line[index].valid!=1){
            set_index=i;
            break;
        }
    }
    if(set_index<0){
        for(int i=0;i<4;i++){
            if(Set_cache[i].cache_line[index].valid==1 && Set_cache[i].cache_line[index].tag==tag){
                set_hit=i;
                break;
            }
    }
    }
    if(set_index<0 && set_hit<0){
        for(int i=0;i<4;i++){
            if(Set_cache[i].cache_line[index].second!=1){
                no_chance[i]=1;
                count++;
            }
        }
        if(count!=0){
            while(no_chance[set_replace]==0 && set_replace!=-1){
                set_replace=rand()%4;
            }
        }else{
            set_replace=rand()%4;
        }
        
    }

    if(set_index>=0){
        miss_cache(set_index,index,tag,ins,c);
    }else if(set_hit>=0){
        hit_cache(set_hit,offset,index,ins,c);
    }else{
        replace(set_replace,index,tag,ins,c);   
    }
   
}
