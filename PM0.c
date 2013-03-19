#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

//structs and function dec;arations
struct instruction{
       int line; 
       int op;
       char *op_code;
       int l;
       int m;       
};

int base(int l, int base, int* s);

//functions
int main(void){
//main declarations
       FILE* fpin = fopen("file.in", "r");
       FILE* fpout = fopen("output.txt", "w");
       struct instruction instr_ary[MAX_CODE_LENGTH];
//main statements    
       //fill array of structs
       int struct_index = 0;
       int i, j;
       while(!feof(fpin)){
          fscanf(fpin, "%d", &i);
          instr_ary[struct_index].op = i;
          fscanf(fpin, "%d", &i);
          instr_ary[struct_index].l = i;
          fscanf(fpin, "%d", &i);
          instr_ary[struct_index].m = i;
          instr_ary[struct_index].line = struct_index;
          
          switch(instr_ary[struct_index].op){

              case 1:
                instr_ary[struct_index].op_code = "LIT";
                break;
              case 2:
                instr_ary[struct_index].op_code = "OPR";
                break;
              case 3:
                instr_ary[struct_index].op_code = "LOD";
                break;
              case 4:
                instr_ary[struct_index].op_code = "STO";
                break;
              case 5:
                instr_ary[struct_index].op_code = "CAL";
                break;
              case 6:
                instr_ary[struct_index].op_code = "INC";
                break;
              case 7:
                instr_ary[struct_index].op_code = "JMP";
                break;
              case 8:
                instr_ary[struct_index].op_code = "JPC";
                break;
              case 9:
                instr_ary[struct_index].op_code = "SIO";
                break;
              case 10:
                instr_ary[struct_index].op_code = "SIO";
                break;

          }//end switch
          struct_index ++;                
       }
       
       //print instructions
       fprintf(fpout, "Line\tOP\t\tL\tM\n");
       int print_index;
       for(print_index = 0; print_index < struct_index - 1; print_index ++){
               fprintf(fpout, "%d\t\t%s\t\t%d\t%d\n", print_index, instr_ary[print_index].op_code, instr_ary[print_index].l, instr_ary[print_index].m);
       }//end print for
       
       //open file for VM output
       FILE* fpVMout = fopen("VMoutput.txt", "w");
          if(fpVMout == NULL){
            printf("NO FILE!!\n");
          }

       //declare and init stack and program pointers
       int pc = 0;
       int bp = 1;
       int sp = 0;
       int break_index = 0;
       int stack[MAX_STACK_HEIGHT];
       int AR_breaks[MAX_LEXI_LEVELS];
       
       for (i = 1; i <= MAX_STACK_HEIGHT - 1; i ++){
          stack[i] = 0;
       }
       
       for(i = 0; i < MAX_LEXI_LEVELS; i ++){
          AR_breaks[i] = -1;
       }

       //write initial 2 lines to file
       fprintf(fpVMout, "\t\t\t\t\t\t\tpc\tbp\tsp\tstack\n");
       fprintf(fpVMout, "Initial values\t\t\t\t\t\t%d\t%d\t%d\n", pc, bp, sp);
       
       //begin fetch - execution process
       while(bp != 0){

        //fetch
        int fetch_instr = 0;
        while(instr_ary[fetch_instr].line != pc) fetch_instr++;
        
        //write instruction to VM file
        fprintf(fpVMout, "%d\t%s\t\t%d\t%d\t\t\t", instr_ary[fetch_instr].line, instr_ary[fetch_instr].op_code, instr_ary[fetch_instr].l, instr_ary[fetch_instr].m);


        //bump program counter
        pc ++;
        
        //locate execution instructions and execute
        switch(instr_ary[fetch_instr].op){

          //LIT
          case 1: 
            sp ++;
            stack[sp] = instr_ary[fetch_instr].m;
            
            break;

          //OPR
          case 2: 
            switch(instr_ary[fetch_instr].m){
              //RET
              case 0:
                sp = bp - 1;
                pc = stack[sp + 4];
                bp = stack[sp + 3];
                AR_breaks[break_index - 1] = -1;
                break_index --;
                break;

              //NEG
              case 1:
                if (stack[sp] <= 0){
                  stack[sp] = abs(stack[sp]);
                }else{
                  stack[sp] = -(stack[sp]);
                }
                
                break;

              //ADD
              case 2:
                sp = sp - 1;
                stack[sp] = stack[sp] + stack[sp + 1];
                
                break;

              //SUB
              case 3:
                sp = sp - 1;
                stack[sp] = stack[sp] - stack[sp + 1];
                
                break;

              //MUL
              case 4:
                sp = sp - 1;
                stack[sp] = stack[sp] * stack[sp + 1];
                
                break;

              //DIV
              case 5:
                sp = sp - 1;
                stack[sp] = stack[sp] / stack[sp + 1];
                
                break;

              //ODD
              case 6:
                stack[sp] = stack[sp] % 2;
                pc++;
                break;

              //MOD
              case 7:
                sp = sp - 1;
                stack[sp] = stack[sp] % stack[sp + 1];
                
                break;

              //EQL
              case 8:
                sp = sp - 1;
                if(stack[sp] == stack[sp + 1]){
                  stack[sp] = 1;
                }else{
                  stack[sp] = 0;
                }
                
                break;

              //NEQ
              case 9:
                sp = sp - 1;
                if(stack[sp] != stack[sp + 1]){
                  stack[sp] = 1;
                }else{
                  stack[sp] = 0;
                }
                
                break;

              //LSS
              case 10:
                sp = sp - 1;
                if(stack[sp] < stack[sp + 1]){
                  stack[sp] = 1;
                }else{
                  stack[sp] = 0;
                }
                
                break;

              //LEQ
              case 11:
                sp = sp - 1;
                if(stack[sp] <= stack[sp + 1]){
                  stack[sp] = 1;
                }else{
                  stack[sp] = 0;
                }
                
                break;

              //GTR
              case 12:
                sp = sp - 1;
                if(stack[sp] > stack[sp + 1]){
                  stack[sp] = 1;
                }else{
                  stack[sp] = 0;
                }
                
                break;

              //GEQ
              case 13:
                sp = sp - 1;
                if(stack[sp] >= stack[sp + 1]){
                  stack[sp] = 1;
                }else{
                  stack[sp] = 0;
                }
                
                break;
            }//end switch
            break;

          //LOD  
          case 3:
            sp = sp + 1;
            stack[sp] = stack[ base(instr_ary[fetch_instr].l, bp, stack) + instr_ary[fetch_instr].m];
            
            break;

          //STO
          case 4:
            stack[ base(instr_ary[fetch_instr].l, bp, stack) + instr_ary[fetch_instr].m] = stack[sp];
            sp = sp - 1;
            
            break;

          //CAL
          case 5:
            stack[sp + 1] = 0;
            stack[sp + 2] = base(instr_ary[fetch_instr].l, bp, stack);
            stack[sp + 3] = bp;
            stack[sp + 4] = pc;
            bp = sp + 1;
            AR_breaks[break_index] = bp;
            break_index ++;
            pc = instr_ary[fetch_instr].m;
            break;

          //INC
          case 6:
            sp = sp + instr_ary[fetch_instr].m;
            
            break;

          //JMP
          case 7:
            pc = instr_ary[fetch_instr].m;
            
            break;

          //JPC
          case 8:
            if(stack[sp] == 0){
              pc = instr_ary[fetch_instr].m;
            }else{
              
            }
            sp = sp - 1;
            break;

          //SIO
          case 9:
            printf("POPPED OFF STACK: %d\n", stack[sp]);
            sp = sp - 1;
            
            break;

          //SIO
          case 10:
            sp = sp + 1;
            printf("INPUT INT TO BE PLACED ON STACK: ");
            scanf("%d", &stack[sp]);
            
            break;

        }//end switch

        //print stack
         fprintf(fpVMout, "%d\t%d\t%d\t", pc, bp, sp);

         if(bp != 0){

          for (i = 1; i <= sp; i ++){
            for(j = 0; j < MAX_LEXI_LEVELS; j ++){
              if(i == AR_breaks[j]){
                fprintf(fpVMout, "|");
              }  
            }
            fprintf(fpVMout, "%d ", stack[i]);
          }
          fprintf(fpVMout, "\n");
          

         }else{
          fprintf(fpVMout, "\n");
         }//end bp if

      }//end while


       fclose(fpin);
       fclose(fpout);
       fclose(fpVMout);
}//end main

int base(int l, int base, int* s){
//base declarations
       int b1 = base;
//base statements
       while(l > 0)
       {
               b1 = s[b1 + 1];
               l --;        
       }
       return b1;    
}//end base
