#include "mips.h"

extern int Memory[0x400000];       //16MB memory
extern int reg[32];
extern int pc;

void fetch(IF_ID* if_id) {
    if_id->inst = Memory[pc / 4];
    printf("[Fetch] pc[0x%X] instruction 0x%08X \n", pc, if_id->inst);
    pc =pc + 4;
    if_id->pc_4=pc;
}