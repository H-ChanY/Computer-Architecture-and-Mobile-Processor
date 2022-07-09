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




typedef struct IF_ID{
    int pc_4; // branch,jal,j 때 필요. 
    int inst;
}IF_ID;

typedef struct ID_EX{
    int pc_4;
    int opcode;
    int rs;
    int rt;
    int rd;
    int func; 
    int imm;
    int shamt;
    int address;
    int read_data1;
    int read_data2;
    
    //WB
    bool RegWrite;
    bool MemtoReg;
    //M
    bool Br_taken;
    bool MemWrite;
    bool MemRead;
    //EX
    bool RegDest;
    bool ALUSrc;
    bool sign_imm;
    
    //Decode
    bool Jump_chk;
    bool Jr_chk;
    bool Jal_chk;
}ID_EX;

typedef struct EX_MEM{
    int branch_addr;
    int alu_result;
    int write_data;
    // rs 나 rt랑 비교해줘야함. 
    int write_reg;
    int pc_4;
    //branch    
    bool zero_;

    //WB
    bool Jal_chk;
    bool RegWrite;
    bool MemtoReg;
    //M
    bool Br_taken;
    bool MemWrite;
    bool MemRead;
}EX_MEM;

typedef struct MEM_WB{
    int mem_read_data;
    int alu_result;
    int write_reg_data;
    int pc_4;
    // rs 나 rt랑 비교해줘야함. 
    int write_reg;
    //WB
    bool Jal_chk;
    bool RegWrite;
    bool MemtoReg;
}MEM_WB;





int Memory[0x400000];           //16MB memory
int reg[32];
int pc;

int num_inst;
int num_jump;
int num_memory;
int num_branch;
int num_not_branch;
int num_success;
int num_fail;

void fetch(IF_ID *if_id);
void Control(ID_EX * id_ex) ;
void Decode(IF_ID *if_id,ID_EX * id_ex,MEM_WB* mem_wb);
void Excute(IF_ID *if_id,ID_EX * id_ex,EX_MEM * ex_mem,EX_MEM * ex_mem2, MEM_WB * mem_wb);
void branch_pc(IF_ID *if_id,EX_MEM * ex_mem); 
void memory_access(EX_MEM * ex_mem,MEM_WB* mem_wb);
void write_back(MEM_WB* mem_wb);
