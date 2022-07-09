//
//  main.m
//  Project2
//
//  Created by 허찬용 on 2022/04/01.
//
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



// address 에 넣을때  inst는 /4 해서 메모리에 접근해야함 lw 할 때


int Memory[0x400000]={0};           //16MB memory
int reg[32]={0};
int pc=0;







// what to

typedef struct{
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
}inst_;


//Controller
typedef struct  {
    bool RegDest;
    bool ALUSrc;
    bool MemtoReg;
    bool RegWrite;
    bool MemRead;
    bool MemWrite;
    bool Jump_chk;
    bool Jr_chk;
    bool Br_taken;
}Control_;

// stat
typedef struct  {
    int stat_r;
    int stat_i;
    int stat_j;
    int stat_lw;
    int stat_sw;
    int stat_b;
}stat_;
// Memory index를 확인 bin파일에서 주소를 받으면 나누기 4해줘서 index에 접근해야함.



void fetch(inst_ *ins) {
    ins->inst = Memory[pc / 4];
    printf("pc[0x%X]=0x%08X \t", pc, ins->inst);
}

void Decode(inst_ *ins) {
    ins->opcode = ((ins->inst >> 26) & 0x3f);
    ins->rs = (ins->inst >> 21) & 0x1f;
    ins->rt = (ins->inst >> 16) & 0x1f;
    ins->rd = (ins->inst >> 11) & 0x1f;
    if (((ins->inst >> 15) & 0x1) == 1) ins->signimm = 0xffff0000 + (ins->inst & 0xffff);
    else ins->signimm = 0x00000000 + (ins->inst & 0xffff);
    ins->zeroimm = 0x00000000 + (ins->inst & 0xffff);
    ins->func_ = (ins->inst & 0x3f);
    ins->shamt = (ins->inst>>6) & (0x3f);
}

void Control(inst_ *ins, Control_* c) {
    /*// RegDst
    if (ins->opcode == 0) c->RegDest = true;
    else c->RegDest = false;
    //jump
    if (ins->opcode == jr) c->jr_chk = true;
    else c->jr_chk = false;
    // branch
    if (ins->opcode == beq && (ins->rs - ins->rt == 0)) c->Br_taken = true;
    else if (ins->opcode == bne && ((ins->rs - ins->rt) != 0)) c->Br_taken = true;
    else c->Br_taken = false;
    // memread
    if (ins->opcode == lw || ins->opcode == lbu || ins->opcode == lhu || ins->opcode == ll ) c->MemRead = true;
    else c->MemRead = false;
    //MemtoReg*/

    
    if (ins->opcode == 0) c->RegDest = true;
    else c->RegDest = false;
    if (ins->opcode != 0 && ins->opcode != beq && ins->opcode != bne) c->ALUSrc = true;
    else c->ALUSrc = false;
    if (ins->opcode == lw || ins->opcode == ll || ins->opcode == lbu || ins->opcode == lhu) c->MemtoReg = true;
    else c->MemtoReg = false;
    if (ins->opcode != sw && ins->opcode != bne && ins->opcode != beq && ins->opcode != j && ins->opcode != jr) c->RegWrite = true;
    else c->RegWrite = false;
    if (ins->opcode == lw) c->MemRead = true;
    else c->MemRead = false;
    if (ins->opcode == sw || ins->opcode == sh || ins->opcode == sb) c->MemWrite = true;
    else c->MemWrite = false;
    if (ins->opcode == j || ins->opcode == jal ) c->Jump_chk = true;
    else c->Jump_chk = false;
    if (ins->opcode == beq ) c->Br_taken = true;
    else if (ins->opcode == bne ) c->Br_taken = true;
    else c->Br_taken = false;
    if(ins->opcode==0 && ins->func_==jr) c->Jr_chk=true;
    else c->Jr_chk=false;

}


void Excute(inst_ *ins, Control_* c) {
    // Format R
    if (c->ALUSrc == false) {
        if(ins->opcode==0){
            switch (ins->func_) {
                case add:
                    ins->ALU_Result = reg[ins->rs] + reg[ins->rt];
                    printf("ADD ALU_Result= %x + %x   : %x",reg[ins->rs] ,reg[ins->rt],ins->ALU_Result);
                    break;
                case addu:
                    ins->ALU_Result = reg[ins->rs] + reg[ins->rt];
                    printf("ADDU ALU_Result= %x + %x   : %x",reg[ins->rs] ,reg[ins->rt],ins->ALU_Result);
                    break; 
                case and :
                    ins->ALU_Result = (reg[ins->rs] & reg[ins->rt]);
                    printf("AND ALU_Result= %x & %x   : %x",reg[ins->rs],reg[ins->rt],ins->ALU_Result);
                    break;
                case nor:
                    ins->ALU_Result = ~(reg[ins->rs] | reg[ins->rt]);
                    printf("NOR ALU_Result= ~(%x | %x)   : %x", reg[ins->rs],reg[ins->rt] ,ins->ALU_Result);
                    break;
                case or :
                    ins->ALU_Result = (reg[ins->rs] | reg[ins->rt]);
                    printf("OR ALU_Result= ( %x | %x )   : %x", reg[ins->rs] ,reg[ins->rt],ins->ALU_Result);
                    break;
               
                case slt:
                    ins->ALU_Result = (reg[ins->rs] < reg[ins->rt]);
                    printf("SLT ALU_Result= (%x < %x) ",reg[ins->rs],reg[ins->rt]);
                    break;
                case sltu:
                    ins->ALU_Result = (reg[ins->rs] < reg[ins->rt]);
                    printf("SLTU ALU_Result= (%x < %x) ",reg[ins->rs] ,reg[ins->rt]);
                    break;
                case sll:
                    ins->ALU_Result = (reg[ins->rt] << ins->shamt);
                    printf("SLL ALU_Result= ( %x << %x)   : %x",reg[ins->rt], ins->shamt,ins->ALU_Result);
                    break;
                case srl:
                    ins->ALU_Result = (reg[ins->rt] >> ins->shamt);
                    printf("SRL ALU_Result= ( %x >> %x )   : %x",reg[ins->rt], ins->shamt,ins->ALU_Result);
                    break;
                case sub: 
                    ins->ALU_Result = (reg[ins->rs] - reg[ins->rt]);
                    printf("SUB ALU_Result= ( %x - %x )   : %x",reg[ins->rs],reg[ins->rt],ins->ALU_Result);
                    break;
                case subu:
                    ins->ALU_Result = (reg[ins->rs] - reg[ins->rt]);
                    printf("SUBU ALU_Result= ( %x - %x )   : %x",reg[ins->rs], reg[ins->rt],ins->ALU_Result);
                    break;
                case jr:
                    ins->ALU_Result = reg[ins->rs];
                    printf("JR ALU_Result= %x",reg[ins->rs]);
                    break;
                default:
                    break;
                }
        }
            else{
                switch (ins->opcode) {
                    case beq:
                        ins->ALU_Result = ((reg[ins->rs] - reg[ins->rt]) == 0);
                        c->Br_taken=(c->Br_taken && ins->ALU_Result);
                        printf("BEQ ALU_Result %x && %x ",c->Br_taken , ins->ALU_Result);
                        break;
                    case bne:
                        ins->ALU_Result = ((reg[ins->rs] - reg[ins->rt]) != 0);
                        c->Br_taken=(c->Br_taken && ins->ALU_Result);
                        printf("BNE ALU_Result %x != %x ",reg[ins->rs],reg[ins->rt]);
                        break;
                        }
                }
        }
       
    
    // Format I
    else if (c->ALUSrc == true) {
        switch (ins->opcode) {
        case addi:
            ins->ALU_Result = reg[ins->rs] + ins->signimm;
            printf("ADDI ALU_Result= ( %x + %x )   : %x",reg[ins->rs] , ins->signimm, ins->ALU_Result);
            break;
        case addiu:
            ins->ALU_Result = reg[ins->rs] + ins->signimm;
            printf("ADDIU ALU_Result= ( %x + %x )   : %x",reg[ins->rs],ins->signimm,ins->ALU_Result);
            break;
        case andi:
            ins->ALU_Result = reg[ins->rs] & ins->zeroimm;
            printf("ANDI ALU_Result= ( %x & %x )   : %x",reg[ins->rs] , ins->zeroimm,ins->ALU_Result);
            break;
        case lbu:
            ins->ALU_Result = (Memory[reg[ins->rs] + ins->signimm] & 0xff);
            printf("LBU ALU_Result Memory[%x + %x]   : %x",reg[ins->rs],ins->signimm,ins->ALU_Result);
            break;
        case lhu:
            ins->ALU_Result = (Memory[reg[ins->rs] + ins->signimm] & 0xffff);
            printf("LHU ALU_Result Memory[%x + %x]   : %x",reg[ins->rs],ins->signimm,ins->ALU_Result);
            break;
        case ll:
            ins->ALU_Result = (Memory[reg[ins->rs] + ins->signimm] & 0xffff);
            printf("LL ALU_Result Memory[%x + %x]   : %x",reg[ins->rs],ins->signimm,ins->ALU_Result);
            break;
        case lui:
            ins->ALU_Result = (ins->zeroimm << 16);
            printf("LUI ALU_Result %x << 16   : %x",ins->zeroimm,ins->ALU_Result);
            break;
        case lw:
            ins->ALU_Result = (reg[ins->rs] + ins->signimm);
            printf("LW ALU_Result %x + %x   : %x",reg[ins->rs] ,ins->signimm,ins->ALU_Result);
            break;
        case ori:
            ins->ALU_Result = (reg[ins->rs] | ins->zeroimm);
            printf("ORI ALU_Result %x | %x   : %x",reg[ins->rs],ins->zeroimm,ins->ALU_Result);
            break;
        case slti:
            ins->ALU_Result = (reg[ins->rs] < ins->signimm);
            printf("SLTI ALU_Result %x < %x   : %x",reg[ins->rs] , ins->signimm,ins->ALU_Result);
            break;
        case sltiu:
            ins->ALU_Result = (reg[ins->rs] < ins->signimm);
            printf("SLTIU ALU_Result %x < %x   : %x",reg[ins->rs] , ins->signimm,ins->ALU_Result);
            break;
        case sb:
            ins->ALU_Result = (reg[ins->rs] + ins->signimm) & 0xff;
            printf("SB ALU_Result %x + %x   : %x",reg[ins->rs],ins->signimm,ins->ALU_Result);
            break;
        case sh:
            ins->ALU_Result = (reg[ins->rs] + ins->signimm) & 0xffff;
            printf("SH ALU_Result %x + %x   : %x",reg[ins->rs] , ins->signimm,ins->ALU_Result);
            break;
        case sw:
            ins->ALU_Result = (reg[ins->rs] + ins->signimm);
            printf("SW ALU_Result %x + %x   : %x",reg[ins->rs] , ins->signimm,ins->ALU_Result);
            break;
        default:
            break;
        }
    }


}

void pc_update(inst_ *ins, Control_* c) {
    // j, jal, jr, branch 인 경우
    pc =pc + 4;
    ins->jump_addr = ((ins->inst & 0x3ffffff) << 2) + ((pc + 4) & 0xf0000000);
    ins->branch_addr = (ins->signimm << 2 )+ pc;

    if(c->Jump_chk){
        pc=ins->jump_addr;
        printf("jump %x",pc);
    }else if(c->Jr_chk){
        pc=ins->ALU_Result;
        ("jump %x",pc);
    }
    else{
        if(c->Br_taken && ins->ALU_Result){
            pc=ins->branch_addr;
            printf("branch %x",pc);
        }
    }


    /*if (c->Br_taken) {
        if (c->Jump_chk) {
            pc = ins->jump_addr;
        }
        else {
            pc = ins->branch_addr;
        }
    }
    else {
        if (c->Jump_chk) {
            pc = ins->jump_addr;
        }
        else {
            pc = ins->branch_addr;
        }
    }*/
}


void Memmory_and_store(inst_ *ins,Control_ *c) {
    // 여기서 필요한게 memread memtoreg memwrite regdst
    if (c->MemWrite && !(c->MemRead)) {//store word
        Memory[(ins->ALU_Result) / 4] = reg[ins->rt];
    }
    else if (c->MemRead && c->RegWrite) {//lw
        reg[ins->rt] = Memory[(ins->ALU_Result) / 4];
    }else if(c->RegDest && c->RegWrite &&!(c->MemWrite)&& !(c->MemtoReg)){//R type
        reg[ins->rd]=ins->ALU_Result;
    }else if((c->RegWrite)&&(c->ALUSrc)&&!(c->RegDest)&& !(c->MemtoReg) && !(c->Jump_chk)&&!(c->MemRead)){//i type
        reg[ins->rt]=ins->ALU_Result;
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

    inst_* ins;
    ins=malloc(sizeof(*ins));
    Control_ * c;
    c=malloc(sizeof(*c));
    // open the binary input file
    fp = fopen("simple2.bin", "rb");

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

    while (1) {
        if (pc == -1) break;
        memset(ins, 0, sizeof(inst_));
        memset(c, false, sizeof(Control_));
        fetch(ins);
        Decode(ins);
        Control(ins, c);
        Excute(ins, c);
        Memmory_and_store(ins, c);
        pc_update(ins, c);
        printf("\n");
    }
    printf("\n%d\n\n",reg[2]);
    free(ins);
    free(c);
    fclose(fp);
    return 0;
}

