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
#define jalr 0x9


// address 에 넣을때  inst는 /4 해서 메모리에 접근해야함 lw 할 때


int Memory[0x400000]={0};           //16MB memory
int reg[32]={0};
int pc=0;







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



// Memory index를 확인 bin파일에서 주소를 받으면 나누기 4해줘서 index에 접근해야함.


void fetch(inst_ *ins) {
    ins->inst = Memory[pc / 4];
    //printf("[Fetch] pc[0x%X] instruction 0x%08X \n", pc, ins->inst);
    pc =pc + 4;
}

void Control(inst_ *ins, Control_* c) {
    if (ins->opcode == 0 ) c->RegDest = true;
    if(ins->opcode!=ori && ins->opcode!=andi) c->sign_imm=true;
    if (ins->opcode != 0 && ins->opcode != beq && ins->opcode != bne) c->ALUSrc = true;
    if (ins->opcode == lw || ins->opcode == ll || ins->opcode == lbu || ins->opcode == lhu) c->MemtoReg = true;
    if (ins->opcode != sw && ins->opcode != bne && ins->opcode != beq && ins->opcode != j ) {
        if(ins->opcode==0 && ins->func_ == jr){
            c->RegWrite = false;
        }else{
            c->RegWrite = true;
        }
    }
    if (ins->opcode == lw) c->MemRead = true;
    if (ins->opcode == sw || ins->opcode == sh || ins->opcode == sb) c->MemWrite = true;
    if (ins->opcode == j) c->Jump_chk = true;
    if (ins->opcode == beq ) c->Br_taken = true;
    else if (ins->opcode == bne ) c->Br_taken = true;
    if(ins->opcode==0 && ins->func_==jr) c->Jr_chk=true;
    if (ins->opcode==jal) {
        c->Jal_chk=true;
        c->RegWrite=true;    
    }
}

void Decode(inst_ *ins, Control_* c) {
    ins->opcode = ((ins->inst >> 26) & 0x3f);
    ins->func_ = (ins->inst & 0x3f);
    Control(ins,c);
    if(c->Jal_chk|| c->Jump_chk){
        pc =pc + 4;
    }
    if(c->sign_imm){
        if (((ins->inst >> 15) & 0x1) == 1) ins->imm =  0xffff0000 + (ins->inst & 0xffff);
        else ins->imm = 0x00000000 + (ins->inst & 0xffff);
    }else{
        ins->imm = 0x00000000 + (ins->inst & 0xffff);
    }
    ins->func_=0;
    if(c->Jal_chk){
        ins->rd=31;
    }else if(ins->opcode==0){
        ins->rd = (ins->inst >> 11) & 0x1f;
    }else{
        ins->rd=(ins->inst >> 16) & 0x1f; // I type rt에 저장하니까 rd번호를 rt로 해줌.
    }
    // opcode 에 맞게 사용하는 레지스터 변경. 
    if(ins->opcode==0){
        ins->rs = (ins->inst >> 21) & 0x1f;
        ins->rt = (ins->inst >> 16) & 0x1f;
        ins->func_ = (ins->inst & 0x3f);
        ins->shamt = (ins->inst>>6) & (0x3f);
        //printf("[Decode] opcode(%02x) rs: %x rt: %x rd: %x shamt: %x funct(%x)\n",ins->opcode,ins->rs,ins->rt,ins->rd,ins->shamt,ins->func_);
    }else if(ins->opcode==2 || ins->opcode==3){
        ins->address=(ins->inst&0x3ffffff);
        ins->address=(ins->address << 2) + ((pc) & 0xf0000000);
        //printf("[Decode] opcode(%02x) address: %x\n",ins->opcode,ins->address);
        
    }else{
        ins->rs = (ins->inst >> 21) & 0x1f;
        ins->rt = (ins->inst >> 16) & 0x1f;
        //printf("[Decode] opcode(%02x) rs: %x rt: %x imm: %x\n",ins->opcode,ins->rs,ins->rt,ins->imm);
    }
    
   


}




void Excute(inst_ *ins, Control_* c) {
    // Format R
    if (c->ALUSrc == false){
        ins->input_1=reg[ins->rs];
        ins->input_2=reg[ins->rt];
    }else{
        ins->input_1=reg[ins->rs];
        ins->input_2=ins->imm;
    }
    ins->Branch_target = (ins->imm << 2 )+ pc;
        if(ins->opcode==0){
            switch (ins->func_) {
                case add:
                    ins->ALU_Result = ins->input_1 + ins->input_2;
                    //printf("[Excute] ADD ALU_Result= reg[%d] + reg[%d]   : 0x%x\n",ins->rs,ins->rt,ins->ALU_Result);
                    break;
                case addu:
                    ins->ALU_Result = ins->input_1 + ins->input_2;
                    //printf("[Excute] ADDU ALU_Result= reg[%d] + reg[%d]   : 0x%x\n",ins->rs ,ins->rt,ins->ALU_Result);
                    break; 
                case and :
                    ins->ALU_Result = (ins->input_1 & ins->input_2);
                    //printf("[Excute] AND ALU_Result= reg[%d] & reg[%d]   : 0x%x",ins->rs,ins->rt,ins->ALU_Result);
                    break;
                case nor:
                    ins->ALU_Result = ~(ins->input_1 | ins->input_2);
                    //printf("[Excute] NOR ALU_Result= ~(reg[%d] | reg[%d])   : 0x%x", ins->rs,ins->rt,ins->ALU_Result);
                    break;
                case or :
                    ins->ALU_Result = (ins->input_1 | ins->input_2);
                    //printf("[Excute] OR ALU_Result= ( reg[%d] | reg[%d] )   : 0x%x", ins->rs ,ins->rt,ins->ALU_Result);
                    break;
               
                case slt:
                    ins->ALU_Result = (ins->input_1 < ins->input_2);
                    //printf("[Excute] SLT ALU_Result= (reg[%d] < reg[%d]) %d\n",ins->rs,ins->rt,ins->ALU_Result);
                    break;
                case sltu:
                    ins->ALU_Result = (ins->input_1  < ins->input_2);
                    //printf("[Excute] SLTU ALU_Result= (reg[%d] < reg[%d] %d) ",ins->rs ,ins->rt,ins->ALU_Result);
                    break;
                case sll:
                    ins->ALU_Result = (ins->input_2 << ins->shamt);
                    //printf("[Excute] SLL ALU_Result= ( reg[%d] << 0x%x)   : 0x%x\n",ins->rt, ins->shamt,ins->ALU_Result);
                    break;
                case srl:
                    ins->ALU_Result = (ins->input_2 >> ins->shamt);
                    //printf("[Excute] SRL ALU_Result= ( reg[%d] >> 0x%x )   : 0x%x",ins->rt, ins->shamt,ins->ALU_Result);
                    break;
                case sub: 
                    ins->ALU_Result = (ins->input_1- ins->input_2);
                    //printf("[Excute] SUB ALU_Result= ( reg[%d] - reg[%d] )   : 0x%x",ins->rs,ins->rt,ins->ALU_Result);
                    break;
                case subu:
                    ins->ALU_Result = (ins->input_1-  ins->input_2);
                    //printf("[Excute] SUBU ALU_Result= ( reg[%d] - reg[%d] )   : 0x%x",ins->rs, ins->rt,ins->ALU_Result);
                    break;
                default:
                    break;
                }
        }    
                
            
        
    // Format I
    
    switch (ins->opcode) {
        case addi:
            ins->ALU_Result = ins->input_1+ins->input_2;
            //printf("[Excute] ADDI ALU_Result= ( reg[%d] + imm(%d) )   : 0x%x\n",ins->rs, ins->imm, ins->ALU_Result);
            break;
        case addiu:
            ins->ALU_Result = ins->input_1+ins->input_2;
            //printf("[Excute] ADDIU ALU_Result= ( reg[%d] + imm(%d) )   : 0x%x\n",ins->rs,ins->imm,ins->ALU_Result);
            break;
        case andi:
            ins->ALU_Result = ins->input_1&ins->input_2;
            //printf("[Excute] ANDI ALU_Result= ( reg[%d] & imm(0x%x) )   : 0x%x",ins->rs , ins->imm,ins->ALU_Result);
            break;
        case lui:
            ins->ALU_Result = (ins->input_2 << 16);
            //printf("[Excute] LUI ALU_Result imm(0x%x) << 16   : 0x%x",ins->imm,ins->ALU_Result);
            break;
        case lw:
            ins->ALU_Result = (ins->input_1+ins->input_2);
            //printf("[Excute] LW ALU_Result reg[%d] + imm(0x%x)   : 0x%x\n",ins->rs ,ins->imm,ins->ALU_Result);
            break;
        case ori:
            ins->ALU_Result = (ins->input_1 | ins->input_2);
            //printf("[Excute] ORI ALU_Result reg[%d] | imm(0x%x)   : 0x%x",ins->rs,ins->imm,ins->ALU_Result);
            break;
        case slti:
            ins->ALU_Result = ins->input_1<ins->input_2;
            //printf("[Excute] SLTI ALU_Result reg[%d] < imm(0x%x)   : 0x%x",ins->rs , ins->imm,ins->ALU_Result);
            break;
        case sltiu:
            ins->ALU_Result = ins->input_1<ins->input_2;
            //printf("[Excute] SLTIU ALU_Result reg[%d] < imm(0x%x)   : 0x%x",ins->rs , ins->imm,ins->ALU_Result);
            break;
        case sw:
            ins->ALU_Result = ins->input_1+ins->input_2;
            //printf("[Excute] SW ALU_Result reg[%d] + imm(0x%x)   = 0x%x\n",ins->rs , ins->imm,ins->ALU_Result);
            break;
        case beq:
            ins->ALU_Result = ((ins->input_1-  ins->input_2) == 0);
            c->Br_taken=(c->Br_taken && ins->ALU_Result);
            //printf("[Excute] BEQ ALU_Result reg[%d] == reg[%d] (%d) ",ins->rs,ins->rt,ins->ALU_Result);
            break;
        case bne:
            ins->ALU_Result = ((ins->input_1-  ins->input_2) != 0);
            c->Br_taken=(c->Br_taken && ins->ALU_Result);
            //printf("[Excute] BNE ALU_Result reg[%d] != reg[%d] (%d)",ins->rs,ins->rt,ins->ALU_Result);
            break;
        default:
            break;
        }
        
    


}



void memory_access(inst_ *ins, Control_ * c){
    if(c->MemRead){
        ins->write_data=Memory[(ins->ALU_Result)/4];
        //printf("lw address 0x%0x get %d\n",(ins->ALU_Result)/4,ins->write_data);
    }
    if(c->MemWrite){
        Memory[(ins->ALU_Result)/4]=reg[ins->rt];
        //printf("sw address mem[0x%0x] = %d\n",ins->ALU_Result/4,reg[ins->rt]);
    }
}
void write_back(inst_ *ins, Control_ * c){
    if(c->RegWrite && c->Jal_chk){
        reg[ins->rd]=pc;
    }else if(c->RegWrite &&c->MemtoReg){
        reg[ins->rd]=ins->write_data;
        //printf("lw reg[%d] =%d\n",ins->rd,ins->write_data);
    }else if(c->RegWrite){
        reg[ins->rd]=ins->ALU_Result;
        //printf("reg[%d] =%d \n",ins->rd,ins->ALU_Result);
    }
}

void pc_update(inst_ *ins, Control_* c) {
    // j, jal, jr, branch 인 경우
    
    

    if(c->Jump_chk){
        pc=ins->address;
        //printf("[PC Update] JUMP pc -> %x\n",pc);
    }else if(c->Jal_chk){
        pc=ins->address;
        //printf("[PC Update] JAL jump : 0x%x Linked : 0x%x\n",pc,reg[31]);
    }else if(c->Jr_chk){
        pc=ins->input_1;
        //printf("[PC Update] JR pc-> 0x%x\n",pc);
    }else{
        if(c->Br_taken && ins->ALU_Result){
            pc=ins->Branch_target;
            //printf("[PC Update] branch pc -> 0x%x",pc);
        }else{
            //printf("\n[PC Update] pc -> 0x%x",pc);
        }
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
    Control_ * c; 
    ins=malloc(sizeof(*ins));
    c=malloc(sizeof(*c));


    // open the binary input file
    fp = fopen("sample/input4.bin", "rb");

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


// latch 구현

    int k=1;
    while (1) {
        if (pc == -1) break;
        if(reg[2]==84&&k > 23300000){
            printf("\n\n\nhello\n\n\n");
        }
        memset(ins, 0, sizeof(inst_));
        memset(c, false, sizeof(Control_));
        //printf("\ncycle %d \n",k);
        k++;
        fetch(ins);
        Decode(ins,c);
        Excute(ins, c);
        memory_access(ins,c);
        write_back(ins,c);
        pc_update(ins, c);

    }
    printf("cycle %d",k);
    printf("\n%d\n\n",reg[2]);
    free(ins);
    free(c);
    fclose(fp);
    return 0;
}

