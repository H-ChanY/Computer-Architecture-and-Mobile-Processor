#include "mips.h"

int Memory[0x400000];           //16MB memory
int reg[32]={0};
int pc=0;
inst_Cache_ inst_cache[256]; 
data_Cache_ data_cache[256];
int miss=0;
int hit=0;
int data_hit=0;
int data_miss=0;