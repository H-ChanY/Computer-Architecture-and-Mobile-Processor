#include "mips.h"

int Memory[0x100000];           //16MB memory
int reg[32]={0};
int pc=0;
inst_Cache_ inst_cache[256]; 
data_Cache_ data_cache[256];
Set_Cache Set_cache[4];
int miss=0;
int hit=0;
int data_hit=0;
int data_miss=0;