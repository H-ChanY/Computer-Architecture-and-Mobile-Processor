//
//  main.m
//  Project2
//
//  Created by 허찬용 on 2022/04/01.
//
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
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



// address 에 넣을때  inst는 /4 해서 메모리에 접근해야함 lw 할 때


int Memory[0x400000]={0};           //16MB memory
int reg[32]={0};
int pc=0;







// what to

struct inst_ {
    int ALU_Result;
    int rs;
    int rt;
    int rd;
    int opcode;
    int inst;
    int shamt;
    int funct;
    int signimm;
    int zeroimm;
    int imm;
    int func_;
    int branch_addr;
    int jump_addr;
} inst;


//Controller
struct Control_ {
    bool RegDest;
    bool ALUSrc;
    bool MemtoReg;
    bool RegWrite;
    bool MemRead;
    bool MemWrite;
    bool Jump_chk;
    bool Br_taken;
    bool jr_chk;
} Cont;

// stat
struct stat_ {
    int stat_r;
    int stat_i;
    int stat_j;
    int stat_lw;
    int stat_sw;
    int stat_b;
} stat;
// Memory index를 확인 bin파일에서 주소를 받으면 나누기 4해줘서 index에 접근해야함.



void fetch( struct inst_ ins) {
    ins.inst = Memory[pc / 4];
    printf("fetch : pc[0x%08X]=0x%08X \n", pc, ins.inst);
}

void Decode(struct inst_ ins) {
    ins.opcode = ((ins.inst >> 26) & 0x3f);
    ins.rs = (ins.inst >> 21) & 0x1f;
    ins.rt = (ins.inst >> 16) & 0x1f;
    ins.rd = (ins.inst >> 11) & 0x1f;
    if (((ins.inst >> 15) & 0x1) == 1) ins.signimm = 0xffff0000 + (ins.inst & 0xffff);
    else ins.signimm = 0x00000000 + (ins.inst & 0xffff);
    ins.zeroimm = 0x00000000 + (ins.inst & 0xffff);
    ins.func_ = (ins.inst & 0x3f);
    ins.shamt = (ins.inst >> 6) & (0x3f);
}

void Control(struct inst_ ins, struct Control_ c) {
    if (ins.opcode == 0) c.RegDest = true;
    else c.RegDest = false;
    if (ins.opcode != 0 && ins.opcode != beq && ins.opcode != bne) c.ALUSrc = true;
    else c.ALUSrc = false;
    if (ins.opcode == lw || ins.opcode == ll || ins.opcode == lbu || ins.opcode == lhu) c.MemtoReg = true;
    else c.MemtoReg = false;
    if (ins.opcode != sw && ins.opcode != bne && ins.opcode != beq && ins.opcode != j && ins.opcode != jr) c.RegWrite = true;
    else c.RegWrite = false;
    if (ins.opcode == lw) c.MemRead = true;
    else c.MemRead = false;
    if (ins.opcode == sw || ins.opcode == sh || ins.opcode == sb) c.MemWrite = true;
    else c.MemWrite = false;
    if (ins.opcode == j || ins.opcode == jal) c.Jump_chk = true;
    else c.Jump_chk = false;
    if (ins.opcode == beq && (ins.rs - ins.rt == 0)) c.Br_taken = true;
    else if (ins.opcode == bne && (ins.rs - ins.rt != 0)) c.Br_taken = true;
    else c.Br_taken = false;
    if (ins.opcode == jr) c.jr_chk = true;
    else c.jr_chk = false;
}


void Excute(struct inst_ ins, struct Control_ c) {
    // Format R
    if (c.ALUSrc == true) {
        switch (ins.func_) {
        case add:
            ins.ALU_Result = reg[ins.rs] + reg[ins.rt];
            break;
        case addu:
            ins.ALU_Result = reg[ins.rs] + reg[ins.rt];
            break;
        case and :
            ins.ALU_Result = (reg[ins.rs] & reg[ins.rt]);
            break;
        case nor:
            ins.ALU_Result = ~(reg[ins.rs] | reg[ins.rt]);
            break;
        case or :
            ins.ALU_Result = (reg[ins.rs] | reg[ins.rt]);
            break;
        case slt:
            ins.ALU_Result = (reg[ins.rs] < reg[ins.rt]);
            break;
        case sltu:
            ins.ALU_Result = (reg[ins.rs] < reg[ins.rt]);
            break;
        case sll:
            ins.ALU_Result = (reg[ins.rt] << ins.shamt);
            break;
        case srl:
            ins.ALU_Result = (reg[ins.rt] >> ins.shamt);
            break;
        case sub:
            ins.ALU_Result = (reg[ins.rs] - reg[ins.rt]);
            break;
        case subu:
            ins.ALU_Result = (reg[ins.rs] - reg[ins.rt]);
            break;
        case jr:
            ins.ALU_Result = reg[ins.rs];
        default:
            break;
        }
    }
    // Format I
    else if (c.ALUSrc == false) {
        switch (ins.opcode) {
        case addi:
            ins.ALU_Result = reg[ins.rs] + ins.signimm;
            break;
        case addiu:
            ins.ALU_Result = reg[ins.rs] + ins.signimm;
            break;
        case andi:
            ins.ALU_Result = reg[ins.rs] + ins.zeroimm;
            break;
        case beq:
            ins.ALU_Result = ((reg[ins.rs] - reg[ins.rt]) == 0);
            break;
        case bne:
            ins.ALU_Result = ((reg[ins.rs] - reg[ins.rt]) != 0);
            break;
        case lbu:
            ins.ALU_Result = (Memory[m.reg[ins.rs] + ins.signimm] & 0xff);
            break;
        case lhu:
            ins.ALU_Result = (Memory[m.reg[ins.rs] + ins.signimm] & 0xffff);
            break;
        case ll:
            ins.ALU_Result = (Memory[m.reg[ins.rs] + ins.signimm] & 0xffff);
            break;
        case lui:
            ins.ALU_Result = (ins.zeroimm << 16);
            break;
        case lw:
            ins.ALU_Result = (reg[ins.rs] + ins.signimm);
            break;
        case ori:
            ins.ALU_Result = (reg[ins.rs] | ins.zeroimm);
            break;
        case slti:
            ins.ALU_Result = (reg[ins.rs] < ins.signimm);
            break;
        case sltiu:
            ins.ALU_Result = (reg[ins.rs] < ins.signimm);
            break;
        case sb:
            ins.ALU_Result = (reg[ins.rs] + ins.signimm) & 0xff;
            break;
        case sh:
            ins.ALU_Result = (reg[ins.rs] + ins.signimm) & 0xffff;
            break;
        case sw:
            ins.ALU_Result = (reg[ins.rs] + ins.signimm);
            break;
        default:
            break;
        }
    }


}

void pc_update(struct inst_ ins, struct Control_ c) {
    // j, jal, jr, branch 인 경우
    ins.pc = ins.pc + 4;
    ins.jump_addr = ((ins.inst & 0x3ffffff) << 2) + ((ins.pc + 4) & 0xf0000000);
    ins.branch_addr = (ins.signimm << 2 )+ m.pc;
    if (c.Br_taken) {
        if (c.Jump_chk) {
            m.pc = ins.jump_addr;
        }
        else {
            m.pc = ins.branch_addr;
        }
    }
    else {
        if (c.Jump_chk) {
            m.pc = ins.jump_addr;
        }
        else {
            m.pc = ins.branch_addr;
        }
    }
}


void Memmory_and_store(struct inst_ ins, struct Control_ c, struct Memory_ m) {
    if (c.MemWrite) {
        m.Memory[ins.ALU_Result / 4] = m.reg[ins.rt];
    }
    else if (c.MemRead && c.MemtoReg) {
        m.reg[ins.rt] = m.Memory[ins.ALU_Result / 4];
    }


}




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

    // open the binary input file
    fp = fopen("simple.bin", "rb");

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
        printf("read from file: 0x%08X\n", *p);
        Memory[index] = *p; // 메모리에 모든 instruction을 저장한다. 
        

    }

    while (1) {
        if (pc == -1) break;
        memset(&inst, 0, sizeof(int));
        memset(&Cont, false, sizeof(bool));
        fetch(inst);
        Decode(inst);
        Control(inst, Cont);
        /*
        Excute(inst, Cont, Mem);
        Memmory_and_store(inst, Cont, Mem);
        pc_update(inst, Cont, Mem);
        */
    }

    fclose(fp);
    return 0;
}

