#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <bitset>

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
    FILE *fp;
    fp = fopen("./r_type", "r");
    if (fp == NULL)
    {
        perror("Failed: ");
        return;
    }
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            fscanf(fp, "%s", r_type[i][j]);
        }
    }
    fclose(fp);
}

void i_type_build()
{
    FILE *fp;
    fp = fopen("./i_type", "r");
    if (fp == NULL)
    {
        perror("Failed: ");
        return;
    }
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            fscanf(fp, "%s", i_type[i][j]);
        }
    }
    fclose(fp);
}

void registers_build()
{
    FILE *fp;
    fp = fopen("./registers", "r");
    if (fp == NULL)
    {
        perror("Failed: ");
        return;
    }
    for (int i = 0; i < 32; i++)
    {
        fscanf(fp, "%s", registers[i]);
    }
    fclose(fp);
}

void read_op(int ins, FILE *out)
{
    int op = (ins >> 26) & 63;
    int rs, rt, rd, shamt, funct, imm, jump_address;

    if (op == 0)
    {

        cout << "opcode: " << bitset<6>(op) << endl;

        // op == 0, then instruction is r-type
        funct = ins & 63;
        int first_3_bit = (funct >> 3) & 7;
        int last_3_bit = funct & 7;

        shamt = (ins >> 6) & 31;
        rs = (ins >> 21) & 31;
        rt = (ins >> 16) & 31;
        rd = (ins >> 11) & 31;

        cout << "f3: " << bitset<3>(first_3_bit) << endl;
        cout << "l3: " << bitset<3>(last_3_bit) << endl;

        if (first_3_bit == 0)
        {
            if (shamt == 0)
            {

                printf("nop\n");

                fprintf(out, "nop\n");
            }
            else
            {
                // shift instruction

                printf("%s %s, %s, %d\n", r_type[first_3_bit][last_3_bit], registers[rd], registers[rt], shamt);

                fprintf(out, "%s ", r_type[first_3_bit][last_3_bit]);
                fprintf(out, "%s, ", registers[rd]);
                fprintf(out, "%s, ", registers[rt]);
                fprintf(out, "%d\n", shamt);
            }
        }
        else if (first_3_bit == 1)
        {
            // jump instruction

            printf("%s %s\n", r_type[first_3_bit][last_3_bit], registers[rs]);

            fprintf(out, "%s ", r_type[first_3_bit][last_3_bit]);
            fprintf(out, "%s\n", registers[rs]);
        }
        else
        {
            // other

            printf("%s %s, %s, %s\n", r_type[first_3_bit][last_3_bit], registers[rd], registers[rt], registers[rt]);

            fprintf(out, "%s ", r_type[first_3_bit][last_3_bit]);
            fprintf(out, "%s, ", registers[rd]);
            fprintf(out, "%s, ", registers[rs]);
            fprintf(out, "%s\n", registers[rt]);
        }
    }
    else
    {
        // op != 0, then instruction is not r-type

        printf("i_type  ");
        cout << "opcode: " << bitset<6>(op) << endl;

        int first_3_bit = (op >> 3) & 7;
        int last_3_bit = op & 7;

        rs = (ins >> 21) & 31;
        rt = (ins >> 16) & 31;
        imm = ins & 32767;
        if ((ins & 32768) > 0)
        {
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

        cout << "f3: " << bitset<3>(first_3_bit) << endl;
        cout << "l3: " << bitset<3>(last_3_bit) << endl;
        printf("%s ", i_type[first_3_bit][last_3_bit]);

        fprintf(out, "%s ", i_type[first_3_bit][last_3_bit]);

        if (first_3_bit == 0)
        {
            if (last_3_bit == 2 || last_3_bit == 3)
            {
                // jump instruction

                printf("0x%08x\n", jump_address);

                fprintf(out, "0x%08x\n", jump_address);
            }
            else if (last_3_bit == 4 || last_3_bit == 5)
            {
                // branch instruction

                printf("%s, %s, %d\n", registers[rs], registers[rt], imm);

                fprintf(out, "%s, ", registers[rs]);
                fprintf(out, "%s, ", registers[rt]);
                fprintf(out, "%d\n", imm);
            }
            else
            {
                // branch zero

                printf("%s, %d\n", registers[rs], imm);

                fprintf(out, "%s, ", registers[rs]);
                fprintf(out, "%d\n", imm);
            }
        }
        else if (first_3_bit == 1)
        {
            if (last_3_bit == 7)
            {

                printf("%s, %d\n", registers[rt], imm);

                fprintf(out, "%s, ", registers[rt]);
                fprintf(out, "%d\n", imm);
            }
            else
            {

                printf("%s, %s, %d\n", registers[rt], registers[rs], imm);

                fprintf(out, "%s, ", registers[rt]);
                fprintf(out, "%s, ", registers[rs]);
                fprintf(out, "%d\n", imm);
            }
        }
        else
        {

            printf("%s, %d(%s)\n", registers[rt], imm, registers[rs]);

            fprintf(out, "%s, %d(%s)\n", registers[rt], imm, registers[rs]);
        }
    }
}

int main(int argc, char *argv[])
{
    registers_build();
    r_type_build();
    i_type_build();

    FILE *ifile;
    // ifile = fopen("input.txt", "r+");
    ifile = fopen("input.dat", "rb");
    // if (get_filename_ext(argv[1]) == "dat")
    if (1)
    {
        // FILE *ifile = fopen(argv[1], "rb");
        unsigned char in[5] = {0};
        while (fgets(in, 5, ifile))
        {
            int ins = 0;
            for (int i = 0; i < 3; i++)
            {
                int temp = in[i];
                ins += temp;
                ins <<= 4;
            }
            int temp = in[3];
            ins += temp;
            read_op(ins);
        }
    }
    else
    {
        char in[4] = {0};
        char string[32] = {0};

        while (fread(in, 4, 1, ifile))
        {
            for (int i = 0; i < 4; i++)
            {

                strcat(string, in);
                if (strstr(in, "(") != NULL)
                {
                    i++;
                }
                if (strstr(in, "j") != NULL)
                {
                    i += 2;
                }
            }
            assembly2bin(string);
        }
    }

    fclose(ifile);
    return 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("%s ", i_type[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("%s ", r_type[i][j]);
        }
        printf("\n");
    }

    FILE *in = fopen(argv[1], "r+");
    if (in == NULL)
    {
        perror("Failed to open input file: ");
        return 0;
    }
    FILE *out = fopen(argv[2], "w+");

    int ins = 0;
    while (1)
    {
        ins = 0;
        char p1[3], p2[3], p3[3], p4[3];
        if (fscanf(in, "%s %s %s %s", p1, p2, p3, p4) == EOF)
        {
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

        printf("0x%s%s%s%s : ", p1, p2, p3, p4);
        cout << bitset<32>(ins) << endl;

        read_op(ins, out);

        printf("\n");
    }
    fclose(in);
    fclose(out);
    return 0;
}
