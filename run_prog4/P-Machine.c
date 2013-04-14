//Tabitha Cliver, t2713301
//COP3402
//Homework 1 - P-machine
//Due February 5, 2013

#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

typedef struct{
    int op; //opcode
    int l;  //L
    int m;  //M
}instruction;

void LIT(int *SP, int stack[], int M);
void OPR(int *PC, int *SP, int *BP, int stack[], int M);
void RET(int *PC, int *BP, int *SP, int stack[]);
void NEG(int *SP, int stack[]);
void ADD(int *SP, int stack[]);
void SUB(int *SP, int stack[]);
void MUL(int *SP,int stack[]);
void DIV(int *SP, int stack[]);
void ODD(int SP, int stack[]);
void MOD(int *SP, int stack[]);
void EQL(int *SP, int stack[]);
void NEQ(int *SP, int stack[]);
void LSS(int *SP, int stack[]);
void LEQ(int *SP, int stack[]);
void GTR(int *SP, int stack[]);
void GEQ(int *SP, int stack[]);
void LOD(int L, int M, int *SP, int BP, int stack[]);
void STO(int L, int M, int BP, int *SP, int stack[]);
void CAL(int L, int M, int *BP, int *PC, int SP, int stack[]);
void INC(int M, int *SP);
void JMP(int M, int *PC);
void JPC(int M, int *PC, int *SP, int stack[]);
void SIO(int M, int *SP, int stack[], int *output);
int base(int l, int base, int stack[]);
void print(int output);
int read();

int main()
{
    //declare and initialize integers
    int SP = 0; //stack pointer
    int BP = 1; //base pointer
    int PC = 0; //program counter
    int IR = 0; //instruction register
    int stackTop = 0; //keeps track of the top of the staack even when the SP gets moved or hasn't moved yet

    //declare the stack
    int stack[MAX_STACK_HEIGHT];
    int divBars[MAX_STACK_HEIGHT];

    //open the input file
    FILE *ifp;  //input file pointer
    char *filename = "toVM.txt";
    ifp = fopen(filename, "r");

    if(!ifp){
        printf("Error in opening '%s' for reading!", filename);
        exit(0);
    }

    //dynamically allocate memory for the array of structs 'instruction' the size of MAX_CODE_LENGTH
    instruction *instructArray = (instruction*)malloc(MAX_CODE_LENGTH*sizeof(instruction));

    if(instructArray == NULL){
        printf("Failed to allocate memory for the instructArray!");
        exit(0);
    }

    //read in the input file
    while(!feof(ifp)){

        //read in the opcode, L, and M
        fscanf(ifp, "%d%d%d", &instructArray[PC].op, &instructArray[PC].l, &instructArray[PC].m);

        PC++;

    }//end while loop

    printf("\n");
    printf("***********************************************************************\n\n");
    printf("Virtual Machine Fetch and Execution Record\n\n");
    printf("***********************************************************************\n\n");
    printf("\n");

    //store the size of the array of code
    //subtract one to take away the final time PC is incremented, since this will be after the last fscanf
    //subtract another since feof does not detect eof until after it has read past it
    //i.e. it doesn't return eof on the last read, only the one after that
    int maxLine = PC-2;

    /* ************************************************************************ */
    /*                                                                          */
    /* Print out the program in interpreted assembly language with line numbers */
    /*                                                                          */
    /* ************************************************************************ */
    printf("Line\tOP\tL\tM\n");
    for(PC=0; PC<=maxLine; PC++){

        //print the line number
        printf("%d\t", PC);

        //print the opcode in interpreted assembly language
        switch(instructArray[PC].op){
            case 1:
                printf("LIT\t");
                break;
            case 2:
                printf("OPR\t");
                break;
            case 3:
                printf("LOD\t");
                break;
            case 4:
                printf("STO\t");
                break;
            case 5:
                printf("CAL\t");
                break;
            case 6:
                printf("INC\t");
                break;
            case 7:
                printf("JMP\t");
                break;
            case 8:
                printf("JPC\t");
                break;
            case 9:
                printf("SIO\t");
                break;
            case 10:
                printf("SIO\t");
                break;
            default:
                printf("Invalid instruction - opcode must be in the range 0 <= # <= 10");
                exit(0);
        }//end switch

        //print L and M, and go to the next line
        printf("%d\t%d\t\n", instructArray[PC].l, instructArray[PC].m);

    }//end for loop for printing out the instructions

    /* ************************************************************** */
    /*                                                                */
    /* Print out the execution of the program in the virtual machine, */
    /* showing the stack and registers pc, bp, and sp:                */
    /*                                                                */
    /* ************************************************************** */

    //make sure PC starts at zero
    PC = 0;

    //print out opening statements
    printf("\n***********************************************************************\n\n");
    printf("LINE\tOP\tL\tM\t\tPC\tBP\tSP\tSTACK\n");
    printf("--------------------------------------------------------------------------\n");
    printf("Initial Values:\t\t\t\t0\t1\t0\t0\n");
    printf("--------------------------------------------------------------------------\n");

    int i=0;
    int j, k;
    int output = -1;
    while(BP != 0){

        //print out the PC
        printf("%d\t", PC);

        //print out the opcode in plain english
        switch(instructArray[PC].op){
            case 1:
                printf("LIT\t");
                break;
            case 2:
                printf("OPR\t");
                break;
            case 3:
                printf("LOD\t");
                break;
            case 4:
                printf("STO\t");
                break;
            case 5:
                printf("CAL\t");
                break;
            case 6:
                printf("INC\t");
                break;
            case 7:
                printf("JMP\t");
                break;
            case 8:
                printf("JPC\t");
                break;
            case 9:
                printf("SIO\t");
                break;
            case 10:
                printf("SIO\t");
                break;
            default:
                printf("Invalid instruction - opcode must be in the range 0 <= # <= 10");
                exit(0);
        }//end switch

        //print out L and M
        printf("%d\t%d\t\t", instructArray[PC].l, instructArray[PC].m);

        //execute the instruction according to the opcode
        switch(instructArray[PC].op){
            case 1:
                LIT(&SP, stack, instructArray[PC].m);
                PC++;
                stackTop = SP;
                break;
            case 2:
                i--;
                OPR(&PC, &SP, &BP, stack, instructArray[PC].m);
                stackTop = SP;
                break;
            case 3:
                LOD(instructArray[PC].l, instructArray[PC].m, &SP, BP, stack);
                PC++;
                stackTop = SP;
                break;
            case 4:
                STO(instructArray[PC].l, instructArray[PC].m, BP, &SP, stack);
                PC++;
                stackTop = SP;
                break;
            case 5:
                divBars[i] = SP;
                i++;
                CAL(instructArray[PC].l, instructArray[PC].m, &BP, &PC, SP, stack);
                stackTop = SP+4;
                break;
            case 6:
                INC(instructArray[PC].m, &SP);
                PC++;
                stackTop = SP;
                break;
            case 7:
                JMP(instructArray[PC].m, &PC);
                stackTop = SP;
                break;
            case 8:
                JPC(instructArray[PC].m, &PC, &SP, stack);
                stackTop = SP;
                break;
            case 9:
                SIO(instructArray[PC].m, &SP, stack, &output);
                PC++;
                stackTop = SP;
                break;
            case 10:
                SIO(instructArray[PC].m, &SP, stack, &output);
                PC++;
                stackTop = SP;
                break;
            default:
                printf("Invalid instruction - opcode must be in the range 0 <= # <= 10");
                exit(0);
        }//end switch

        //print out the PC, BP, and SP
        printf("%d\t%d\t%d\t", PC, BP, SP);

        //print out the stack
        for(j=0;j<stackTop;j++){
            for(k=0;k<=i;k++){
                if((j == divBars[k]) && (j != 0))
                printf(" | ");
            }
            printf("%d ", stack[j+1]);
        }

        //print output, if it exists
        if(output != -1){
            printf("Output: %d\n", output);
        }
        printf("\n");

    }//end for loop for printing out the execution of the code


    //free the allocated memory for the array of structs 'instruction'
    free(instructArray);

    return 0;
}//end main

/* ********************************************** */
/* 01 - LIT 0,M                                   */
/* Push constant value (literal) M onto the stack */
/* ********************************************** */
void LIT(int *SP, int stack[], int M){
    *SP = *SP+1;
    stack[*SP] = M;
}

/* ************************************************************* */
/* 02 - OPR 0,#                                                  */
/* Operation to be performed on the data at the top of the stack */
/* ************************************************************* */
void OPR(int *PC, int *SP, int *BP, int stack[], int M) {
    switch(M){
        case 0:
            RET(PC, BP, SP, stack);
            break;
        case 1:
            NEG(SP, stack);
            *PC = *PC+1;
            break;
        case 2:
            ADD(SP, stack);
            *PC = *PC+1;
            break;
        case 3:
            SUB(SP, stack);
            *PC = *PC+1;
            break;
        case 4:
            MUL(SP, stack);
            *PC = *PC+1;
            break;
        case 5:
            DIV(SP, stack);
            *PC = *PC+1;
            break;
        case 6:
            ODD(*SP, stack);
            *PC = *PC+1;
            break;
        case 7:
            MOD(SP, stack);
            *PC = *PC+1;
            break;
        case 8:
            EQL(SP, stack);
            *PC = *PC+1;
            break;
        case 9:
            NEQ(SP, stack);
            *PC = *PC+1;
            break;
        case 10:
            LSS(SP, stack);
            *PC = *PC+1;
            break;
        case 11:
            LEQ(SP, stack);
            *PC = *PC+1;
            break;
        case 12:
            GTR(SP, stack);
            *PC = *PC+1;
            break;
        case 13:
            GEQ(SP, stack);
            *PC = *PC+1;
            break;
        default:
            printf("Invalid instruction - OPR must use a value in the range 0 <= # <= 13");
            exit(0);
    }//end switch
}//end OPR

/* ************* */
/* OPR 0,0 - RET */
/* ************* */
void RET(int *PC, int *BP, int *SP, int stack[]){
    *SP = *BP -1;
    *PC = stack[*SP+4];
    *BP = stack[*SP+3];
}

/* ************* */
/* OPR 0,1 - NEG */
/* ************* */
void NEG(int *SP, int stack[]){
    stack[*SP] = (-stack[*SP]);
}

/* ************* */
/* OPR 0,2 - ADD */
/* ************* */
void ADD(int *SP, int stack[]){
    *SP = *SP - 1;
    stack[*SP] = stack[*SP] + stack[*SP+1];
}

/* ************* */
/* OPR 0,3 - SUB */
/* ************* */
void SUB(int *SP, int stack[]){
    *SP = *SP-1;
    stack[*SP] = stack[*SP] - stack[*SP+1];
}

/* ************* */
/* OPR 0,4 - MUL */
/* ************* */
void MUL(int *SP,int stack[]){
    *SP = *SP-1;
    stack[*SP] = stack[*SP] * stack[*SP+1];
}

/* ************* */
/* OPR 0,5 - DIV */
/* ************* */
void DIV(int *SP, int stack[]){
    *SP = *SP-1;
    stack[*SP] = stack[*SP]/stack[*SP+1];
}

/* ************* */
/* OPR 0,6 - ODD */
/* ************* */
void ODD(int SP, int stack[]){
    stack[SP] = stack[SP] % 2;
}

/* ************* */
/* OPR 0,7 - MOD */
/* ************* */
void MOD(int *SP, int stack[]){
    *SP = *SP-1;
    stack[*SP] = stack[*SP] % stack[*SP+1];
}

/* ************* */
/* OPR 0,8 - EQL */
/* ************* */
void EQL(int *SP, int stack[]){
    *SP = *SP-1;
    stack[*SP] = stack[*SP] == stack[*SP+1];
}

/* ************* */
/* OPR 0,9 - NEQ */
/* ************* */
void NEQ(int *SP, int stack[]){
    *SP = *SP-1;
    stack[*SP] = stack[*SP] != stack[*SP+1];
}

/* ************** */
/* OPR 0,10 - LSS */
/* ************** */
void LSS(int *SP, int stack[]){
    *SP = *SP-1;
    stack[*SP] = stack[*SP] < stack[*SP+1];
}

/* ************** */
/* OPR 0,11 - LEQ */
/* ************** */
void LEQ(int *SP, int stack[]){
    *SP = *SP-1;
    stack[*SP] = stack[*SP] <= stack[*SP+1];
}

/* ************** */
/* OPR 0,12 - GTR */
/* ************** */
void GTR(int *SP, int stack[]){
    *SP = *SP-1;
    stack[*SP] = stack[*SP] > stack[*SP+1];
}

/* ************** */
/* OPR 0,13 - GEQ */
/* ************** */
void GEQ(int *SP, int stack[]){
    *SP = *SP-1;
    stack[*SP] = stack[*SP] >= stack[*SP+1];
}

/* ************************************************************* */
/* 03 - LOD L,M                                                  */
/* Load value to top of stack in the stack location at offset M  */
/* from L lexicographical levels down                            */
/* ************************************************************* */
void LOD(int L, int M, int *SP, int BP, int stack[]){
    *SP = *SP+1;
    stack[*SP] = stack[base(L, BP, stack)+M];
}

/* ************************************************************* */
/* 04 - STO L,M                                                  */
/* Store value at top of stack in the stack location at offset M */
/* from L lexicographical levels down                            */
/* ************************************************************* */
void STO(int L, int M, int BP, int *SP, int stack[]){
    stack[base(L,BP, stack)+M] = stack[*SP];
    *SP = *SP-1;
}

/* *************************************************************** */
/* 05 - CAL L,M                                                    */
/* Call procedure at code index M (generates new Activation Record */
/* and pc <- M)                                                    */
/* *************************************************************** */
void CAL(int L, int M, int *BP, int *PC, int SP, int stack[]){
    *PC = *PC+1;
    stack[SP+1] = 0;                                //space to return value
    stack[SP+2] = base(L, *BP, stack);              //static link (SL)
    stack[SP+3] = *BP;                              //dynamic link (DL)
    stack[SP+4] = *PC;                              //return address (RA)
    *BP = SP+1;
    *PC = M;
}

/* ************************************************************* */
/* 06 - INC 0,M                                                  */
/* Allocate M locals (increment SP by M). First three are Static */
/* Link (SL), Dynamic Link (DL), and Return Address (RA)         */
/* ************************************************************* */
void INC(int M, int *SP){
    *SP = *SP + M;
}

/* ************************************************************* */
/* 07 - JMP 0,M                                                  */
/* Jump to instruction M                                         */
/* ************************************************************* */
void JMP(int M, int *PC){
    *PC = M;
}

/* ************************************************************* */
/* 08 - JPC 0,M                                                  */
/* Jump to instruction M if top stack element is 0               */
/* ************************************************************* */
void JPC(int M, int *PC, int *SP, int stack[]){
    if(stack[*SP]==0){
        *PC = M;
    }
    else{
        *PC = *PC+1;
    }
    *SP = *SP-1;
}

/* **************************************************************** */
/* 09 - SIO 0,1                                                     */
/* Write the top stack element to the screen                        */
/* 10 - SIO 0,2                                                     */
/* Read in input from the user and store it at the top of the stack */
/* **************************************************************** */
void SIO(int M, int *SP, int stack[], int *output){
    if(M==1){
        *output = (stack[*SP]);
        *SP = *SP-1;
    }
    else if(M==2){
        *SP = *SP+1;
        stack[*SP] = read();
    }
}

/* ************************************************************* */
/* Find base L levels down                                       */
/* ************************************************************* */
int base(int l, int base, int stack[]){
    int bl; //base L levels down
    bl = base;
    while(l>0){
        bl = stack[bl+1];
        l--;
    }
    return bl;
}

int read(){
    int input;
    printf("Enter the desired input value: ");
    scanf("%d", &input);
    return input;
}
