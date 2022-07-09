
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// format R
#define add 0x20
#define addu 0x21
#define and 0x24
#define jr 0x08
#define nor 0x27
#define or 0x25
#define slt 0x2a
#define sltu 0x2b
#define sll 0x00
#define srl 0x02
#define sub 0x22
#define subu 0x23
#define move 0x21

// format I
#define addi 0x8
#define addiu 0x9
#define andi 0xc
#define beq 0x4
#define bne 0x5
#define lbu 0x24
#define lhu 0x25
#define ll 0x30
#define lui 0xf
#define lw 0x23
#define ori 0xd
#define slti 0xa
#define sltiu 0xb
#define sb 0x28
#define sc 0x38
#define sh 0x29
#define sw 0x2b


// format J
#define j 0x2
#define jal 0x3
#define jalr 0x9




// what to

typedef struct{
    int ALU_Result;
    int Branch_target;
    int write_data;
    int opcode;
    int rs;
    int rt;
    int rd;
    
    int shamt;
    int address;
    int imm;
    
    int func_;
    int inst;

    int input_1;
    int input_2;
}inst_;


//Controller
typedef struct  {
    bool RegDest;
    bool ALUSrc;
    bool MemtoReg;
    bool RegWrite;
    bool MemRead;
    bool MemWrite;
    bool sign_imm;
    bool Jump_chk;
    bool Jr_chk;
    bool Br_taken;
    bool Jal_chk;
}Control_;

typedef struct{
    int data;
}Cache_lines;

typedef struct {
    Cache_lines inst_cache_line[16];
    int valid;
    int second;
    int tag;
}inst_Cache_;

typedef struct{
    Cache_lines data_cache_line[16];
    int valid;
    int second;
    int tag;
    int dirty;
}data_Cache_;

int Memory[0x400000];
int reg[32];
int pc;
int hit;
int miss;
int data_hit;
int data_miss;
void Control(inst_ *ins, Control_* c) ;
void fetch(inst_ *ins);
void Decode(inst_ *ins, Control_* c);
void Excute(inst_ *ins, Control_* c) ;
void memory_access(inst_ *ins, Control_ * c);
void write_back(inst_ *ins, Control_ * c);
void pc_update(inst_ *ins, Control_* c);
void data_cache_access(inst_ *ins, Control_* c);