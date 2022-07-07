//Behnoosh Behyani
//993613012
//project 2
//spim
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <minmax.h>
//#include <conio.h>
#include <windows.h>
HANDLE  h;
struct inst
{
    char name[8];
    int opcode;
    int type;
    int dec;
    char hex[8];
    int rs;
    int rt;
    int rd;
    int offset;

};
struct memory{
    int address;
    int value;
};
struct registers
{
    int value;
    bool use;
};
void dec_to_hex(char *dec_str , char *hex_str , char *t)
{
    int dec_int = atoi(dec_str);
    long unsigned int n = 4294967295;
    if(dec_int<0)
    {
        dec_int+=1;
        n+=dec_int;
    }
    else
    {
        n=dec_int;
    }
    int r , i=0;
    while(n!=0)
    {
        r = n % 16;
            if(r<10)
            {
                hex_str[i] = r + 48;
            }
            else
            {
                hex_str[i] = r + 55;
            }
            i++;
            n/=16;
    }
    int size = strlen(hex_str);
    int k=0;
    for(int j=size-1; j>=0; j--) {
        t[k] = hex_str[j];
        k++;
    }
    t[k]='\0';
}
int hex_to_dec(char *hex , int index_start , int index_end)
{
    int dec=0;
    int r;
    int counter=0;
    for(int i=index_end; i>=index_start; i--)
    {
        if(hex[i]>='0' && hex[i]<='9')
            r = hex[i] - '0';
        else
            r = hex[i] - 'A' + 10;

        dec+=r*pow(16 , counter);
        counter++;
    }
    if(dec>32768)
        dec = dec - 65536;
    return dec;
}
int main(int argc,char **argv) {
    h = GetStdHandle(STD_OUTPUT_HANDLE);
    FILE *input , *output;
    char *line;
    line = (char *)malloc(72);
    size_t lineSize=70;
    int counter=0;
    char instruction[100][100];
    char *instruct[]={"add" , "sub" , "slt" , "or" , "nand" , "addi" ,
                          "slti" , "ori" , "lui" , "lw" , "sw" , "beq" , "jalr" , "j" , "halt"};

    if((input=fopen(argv[1],"r")) == NULL)
    {
        printf("cannot be openned\n");
        exit(1);
    }
    if((output=fopen(argv[2],"w+")) == NULL)
    {
        printf("cannot be openned\n");
        exit(1);
    }
    //input=fopen(argv[1],"r");
    //output= fopen(argv[2] , "w+");
    while(fgets(line , lineSize , input) != NULL)
    {
        strcpy(instruction[counter], line );
        counter++;
    }

    //instruction part
    int i=0;
    int instNo=0;
    int flag;
    struct inst inst[100];
    while(i<counter)
    {
        //check .fill part
        if(strcmp(inst[i-1].name , "halt")==0)
        {
            flag=i;
        }
        if(atoi(instruction[i])>0) {
            char hex_temp[9];
            int size = sizeof(hex_temp) / sizeof(hex_temp[0]);
            char hex_str[size];
            strcpy(hex_temp, "00000000");

            dec_to_hex(instruction[i], hex_temp, hex_str);

            //decimal
            inst[i].dec = atoi(instruction[i]);

            //heximal
            strcpy(inst[i].hex, hex_str);

            //opcode
            inst[i].opcode = hex_to_dec(hex_str, 1, 1);

            //type
            if (inst[i].opcode >= 0 && inst[i].opcode <= 4)
                inst[i].type = 1;
            else if (inst[i].opcode >= 5 && inst[i].opcode <= 12)
                inst[i].type = 2;
            else
                inst[i].type = 3;

            //name of instruction
            strcpy(inst[i].name, instruct[inst[i].opcode]);

            //split rs rt rd offset base on type
            switch (inst[i].type) {
                case 1:
                    inst[i].rs = hex_to_dec(hex_str, 2, 2);
                    inst[i].rt = hex_to_dec(hex_str, 3, 3);
                    inst[i].rd = hex_to_dec(hex_str, 4, 4);
                    break;
                case 2:
                    inst[i].rs = hex_to_dec(hex_str, 2, 2);
                    inst[i].rt = hex_to_dec(hex_str, 3, 3);
                    inst[i].offset = hex_to_dec(hex_str, 4, 7);
                    break;
                case 3:
                    inst[i].offset = hex_to_dec(hex_str, 4, 7);
                    break;

            }

        }
            instNo++;
            i++;
    }

    //memory part
    struct memory memory[100];
    int memoryNo=0;

    i=0;
    while(i<counter)
    {
        int dec = atoi(instruction[i]);
        memory[memoryNo].value=dec;
        memory[memoryNo].address=i;
        memoryNo++;
        i++;
    }

    //Let's go
    int pc=0;
    int totalInst=0;
    int RegUse=0;
    int MemUse=0;
    int rs , rt , rd , offset;

    struct registers reg[16];
    reg[0].value=0;
    reg[0].use=false;

    for(int k=1; k<=15; k++)
    {
        reg[k].value=-1;
        reg[k].use=false;
    }

    int last_pc=0;
    int halt_address;
    while( inst[last_pc].opcode!=14)
    {
        halt_address=last_pc;
        last_pc=pc;
        totalInst++;
        rs=inst[pc].rs;
        rt=inst[pc].rt;
        rd=inst[pc].rd;
        offset=inst[pc].offset;

        SetConsoleTextAttribute(h , 11);
        printf("%s(%d) : " , "Inst" , totalInst);
        switch (inst[pc].type) {
            case 1:
                printf("%s\t%d, %d, %d\n" , inst[pc].name , rd , rs , rt);
                if(!reg[rd].use)
                {
                    reg[rd].use=true;
                    RegUse++;
                }

                if(rd!=0) {
                    if (inst[pc].opcode == 0) {//add
                        reg[rd].value = reg[rs].value + reg[rt].value;

                    } else if (inst[pc].opcode == 1) {//sub
                        reg[rd].value = reg[rs].value - reg[rt].value;

                    } else if (inst[pc].opcode == 2) {//slt
                        if(reg[rs].value < reg[rt].value)
                            reg[rd].value = 1;
                        else
                            reg[rd].value = 0;

                    } else if (inst[pc].opcode == 3) {//or
                        reg[rd].value = reg[rs].value | reg[rt].value;

                    } else {//nand
                        reg[rd].value = ~(reg[rs].value & reg[rt].value);

                    }
                }
                pc = pc + 1;
                break;
            case 2:
                if(inst[pc].opcode==8)
                    printf("%s\t %d, %d\n" , inst[pc].name , rt , offset);
                else if(inst[pc].opcode==12)
                    printf("%s\t%d, %d\n" , inst[pc].name , rs , rt);
                else
                    printf("%s\t%d, %d, %d\n" , inst[pc].name , rt , rs , offset);
                if(!reg[rt].use && inst[pc].opcode!=11 )
                {
                    reg[rt].use=true;
                    RegUse++;
                }
                if(rt!=0 || inst[pc].opcode==11)
                {
                    if(inst[pc].opcode==5){//addi
                        reg[rt].value = reg[rs].value + offset;

                    }else if(inst[pc].opcode==6){//slti
                        if(reg[rs].value < offset)
                            reg[rt].value = 1;
                        else
                            reg[rt].value = 0;

                    }else if(inst[pc].opcode==7){//ori
                        reg[rt].value = reg[rs].value | offset;

                    }else if(inst[pc].opcode==8){//lui
                        reg[rt].value =offset<<16;

                    }else if(inst[pc].opcode==9){//lw
                        int address = reg[rs].value + offset;
                        for(int j=0; j<memoryNo; j++)
                        {
                            if(memory[j].address==address)
                            {
                                reg[rt].value = memory[j].value;
                                break;
                            }
                        }
                        MemUse++;

                    }else if(inst[pc].opcode==10){//sw
                        int address = reg[rs].value + offset;
                        int j;
                        for(j=0; j<memoryNo; j++)
                        {
                            if(memory[j].address==address)
                            {
                                memory[j].value = reg[rt].value ;
                                break;
                            }
                        }
                        if(j==memoryNo)
                        {
                            memory[memoryNo].value = reg[rt].value;
                            memory[memoryNo].address = address;
                            memoryNo++;
                        }
                        MemUse++;

                    }else if(inst[pc].opcode==11){//beq
                        if(reg[rt].value == reg[rs].value)
                        {
                                pc += offset;
                        }

                    }else{//jalr
                        reg[rt].value = pc + 1;
                        pc = reg[rs].value - 1;
                    }
                }

                pc = pc + 1;
                break;
            case 3:
                if(inst[pc].opcode==13){
                    printf("%s\t %d\n" , inst[pc].name , offset);
                    pc = offset;
                }
                else
                {
                    printf("%s\n" , inst[pc].name );
                    pc = pc + 1;
                }
                break;

        }

        SetConsoleTextAttribute(h , 9);
        printf("-------Register-------\n");
        SetConsoleTextAttribute(h , 7);
        printf("+-------+------------+\n");
        printf("| %5s | %10s |\n" , "NoReg" , "Value");
        printf("+-------+------------+\n");
        for(int l=0; l<16; l++)
        {
            fprintf(output , "%s%d=%d\t" , "R" , l , reg[l].value );
            printf("| %s%d\t| %10d |\n" ,"R" , l , reg[l].value );
            printf("+-------+------------+\n");
        }

        SetConsoleTextAttribute(h , 9);
        printf("\n----------Memory---------\n");
        SetConsoleTextAttribute(h , 7);
        printf("+-----------+-----------+\n");
        printf("| %9s | %9s |\n" , "Address" , "Value");
        printf("+-----------+-----------+\n");
        for(int g=0; g<memoryNo; g++)
        {
            printf("| %9d | %9d |\n" ,memory[g].address , memory[g].value);
            printf("+-----------+-----------+\n");
        }

        SetConsoleTextAttribute(h , 11);
        float f =  (float)RegUse/16*100;
        fprintf(output , "%s : %d\t","PC" , last_pc);
        printf("%s : %d\n" , "Program Counter" , last_pc);

        fprintf(output ,"%s : %d\t" , "NumInst" , totalInst );
        printf("%s : %d\n" , "Number of inst" , totalInst );

        fprintf(output ,"%s : %.2f%\t" , "RegUsed" , f );
        printf("%s : %.2f%\n" , "Register used" , f );

        fprintf(output , "%s : %d\n" ,"MemUsed (lw & sw)", MemUse );
        printf("%s : %d\n" ,"Memory used (lw & sw)", MemUse);

        SetConsoleTextAttribute(h , 3);
        printf("\n************************\n\n");
        SetConsoleTextAttribute(h , 7);
        //system("pause");
    }

    int max_add = max(halt_address , last_pc);
    printf("%s : %d\n" ,"Memory used (lw & sw)", MemUse);
    printf("%s : %d\n" , "Memory used (.fill)" , counter-max_add-1 );
    fprintf(output , "%s : %d\n" , "Memory Used" , MemUse + counter-max_add-1);
    fclose(input);
    fclose(output);
}
