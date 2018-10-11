#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <bitset>
#define DEBUG 1

using namespace std;

char r_type[8][8][8] = {0};
char i_type[8][8][8] = {0};
char registers[8][8] = {0};

int hex2int(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}

void r_type_build()
{
    FILE * fp;
    fp = fopen("./r_type", "r");
    if(fp == NULL) {
        perror("Failed: ");
        return;
    }
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            fscanf(fp, "%s", r_type[i][j]);
        }
    }
    fclose(fp);
}

void i_type_build()
{
    FILE * fp;
    fp = fopen("./i_type", "r");
    if(fp == NULL) {
        perror("Failed: ");
        return;
    }
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            fscanf(fp, "%s", i_type[i][j]);
        }
    }
    fclose(fp);
}

void registers_build()
{
    FILE * fp;
    fp = fopen("./registers", "r");
    if(fp == NULL) {
        perror("Failed: ");
        return;
    }
    for(int i = 0; i < 32; i++) {
        fscanf(fp, "%s", registers[i]);
    }
    fclose(fp);
}


void read_op(int ins, FILE *out) 
{
    int op = (ins >> 26) & 63;
    int rs, rt, rd, shamt, funct, imm, jump_address;

    if(op == 0) {
#ifdef DEBUG
        cout << "opcode: " << bitset<6>(op) << endl;
#endif
        // op == 0, then instruction is r-type
        funct = ins & 63;
        int first_3_bit = (funct >> 3) & 7;
        int last_3_bit = funct & 7;

        shamt = (ins >> 6) & 31;
        rs = (ins >> 21) & 31;
        rt = (ins >> 16) & 31;
        rd = (ins >> 11) & 31;

#ifdef DEBUG
        cout << "f3: " << bitset<3>(first_3_bit) << endl;
        cout << "l3: " << bitset<3>(last_3_bit) << endl;
#endif
        if(first_3_bit == 0) {    
            if(shamt == 0) {
#ifdef DEBUG
                printf("nop\n");
#endif
                fprintf(out, "nop\n");
            } else {
                // shift instruction
#ifdef DEBUG
                printf("%s %s, %s, %d\n", r_type[first_3_bit][last_3_bit], registers[rd], registers[rt], shamt);
#endif
                fprintf(out, "%s ", r_type[first_3_bit][last_3_bit]);
                fprintf(out, "%s, ", registers[rd]);
                fprintf(out, "%s, ", registers[rt]);
                fprintf(out, "%d\n", shamt);
            }
        } else if(first_3_bit == 1) {
            // jump instruction
#ifdef DEBUG
            printf("%s %s\n", r_type[first_3_bit][last_3_bit], registers[rs]);
#endif
            fprintf(out, "%s ", r_type[first_3_bit][last_3_bit]);
            fprintf(out, "%s\n", registers[rs]);
        } else {
            // other
#ifdef DEBUG
            printf("%s %s, %s, %s\n", r_type[first_3_bit][last_3_bit], registers[rd], registers[rt], registers[rt]);
#endif
            fprintf(out, "%s ", r_type[first_3_bit][last_3_bit]);
            fprintf(out, "%s, ", registers[rd]);
            fprintf(out, "%s, ", registers[rs]);
            fprintf(out, "%s\n", registers[rt]);
        }
    } else {
        // op != 0, then instruction is not r-type
        
#ifdef DEBUG
        printf("i_type  ");
        cout << "opcode: " << bitset<6>(op) << endl;
#endif
        int first_3_bit = (op >> 3) & 7;
        int last_3_bit = op & 7;

        rs = (ins >> 21) & 31;
        rt = (ins >> 16) & 31;
        imm = ins & 32767;
        if((ins & 32768) > 0) {
            imm = imm ^ 32767;
            imm++;
            imm *= -1;
        }

        jump_address = ins & 67108863;

        // f3 (first_3_bit) l3 (last_3_bit)
        // f3 == 0 l3 == 2,3 jump(adr) 
        //         l3 == 4,5 branch(s t adr) 
        //          otherwise branch_zero(s adr)
        // f3 = 1 l3 == 0-6 t s ival 
        //        l3 == 7 lui t ival
        // other t = adr(s)
#ifdef DEBUG
        cout << "f3: " << bitset<3>(first_3_bit) << endl;
        cout << "l3: " << bitset<3>(last_3_bit) << endl;
        printf("%s ", i_type[first_3_bit][last_3_bit]);
#endif
        fprintf(out, "%s ", i_type[first_3_bit][last_3_bit]);

        if(first_3_bit == 0) {
            if(last_3_bit == 2 || last_3_bit == 3) {
                // jump instruction
#ifdef DEBUG
                printf("0x%08x\n", jump_address);
#endif

                fprintf(out, "0x%08x\n", jump_address);
            } else if(last_3_bit == 4 || last_3_bit == 5) {
                // branch instruction
#ifdef DEBUG
                printf("%s, %s, %d\n", registers[rs], registers[rt], imm);
#endif
                fprintf(out, "%s, ", registers[rs]);
                fprintf(out, "%s, ", registers[rt]);
                fprintf(out, "%d\n", imm);
            } else {
                // branch zero 
#ifdef DEBUG
                printf("%s, %d\n", registers[rs], imm);
#endif
                fprintf(out, "%s, ", registers[rs]);
                fprintf(out, "%d\n", imm);
            }
        } else if(first_3_bit == 1) {
            if(last_3_bit == 7) {
#ifdef DEBUG
                printf("%s, %d\n", registers[rt], imm);
#endif
                fprintf(out, "%s, ", registers[rt]);
                fprintf(out, "%d\n", imm);
            } else {
#ifdef DEBUG
                printf("%s, %s, %d\n", registers[rt], registers[rs], imm);
#endif
                fprintf(out, "%s, ", registers[rt]);
                fprintf(out, "%s, ", registers[rs]);
                fprintf(out, "%d\n", imm);
            }
        } else {
#ifdef DEBUG
            printf("%s, %d(%s)\n", registers[rt], imm, registers[rs]);
#endif
            fprintf(out, "%s, %d(%s)\n", registers[rt], imm, registers[rs]);
        }
    }
}

int main(int argc, char *argv[])
{
    registers_build();
    r_type_build();
    i_type_build();

#ifdef DEBUG
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            printf("%s ", i_type[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            printf("%s ", r_type[i][j]);
        }
        printf("\n");
    }
#endif

    FILE *in = fopen(argv[1], "r+");
    if(in == NULL) {
        perror("Failed to open input file: ");
        return 0;
    }
    FILE *out = fopen(argv[2], "w+");

    int ins = 0;
    while(1) {
        ins = 0;
        char p1[3], p2[3], p3[3], p4[3];
        if(fscanf(in, "%s %s %s %s", p1, p2, p3, p4) == EOF) {
            break;
        }

        ins += hex2int(p1[0]);
        ins = ins << 4;
        ins += hex2int(p1[1]);
        ins = ins << 4;
        ins += hex2int(p2[0]);
        ins = ins << 4;
        ins += hex2int(p2[1]);
        ins = ins << 4;
        ins += hex2int(p3[0]);
        ins = ins << 4;
        ins += hex2int(p3[1]);
        ins = ins << 4;
        ins += hex2int(p4[0]);
        ins = ins << 4;
        ins += hex2int(p4[1]);

#ifdef DEBUG
        printf("0x%s%s%s%s : ", p1, p2, p3, p4);
        cout << bitset<32>(ins) << endl;
#endif
        read_op(ins, out);
        
#ifdef DEBUG
        printf("\n");
#endif
    }
    fclose(in);
    fclose(out);
    return 0; 
} 
