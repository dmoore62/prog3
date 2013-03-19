//Tabitha Cliver and Daniel Moore
//COP3402 - System Software
//Assignment 3 - Parser-Code Generator
//Due March 31, 2013


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

// symbol_table[MAX_SYMBOL_TABLE_SIZE];

typedef struct symbols_read_in
    {
	int kind; 		// const = 1, var = 2, proc = 3
	char name[10];	// name up to 11 chars
	int val; 		// number (ASCII value)
	int level; 		// L level
	int addr; 		// M address
	struct symbols_read_in* next;
    } symbol;

//void read_input_file(FILE *ifp);
void BLOCK(char c, FILE *ifp);
void STATEMENT(char c, FILE *ifp);

void ERROR_1();  //Use = instead of :=.
void ERROR_2();  //= must be followed by a number.
void ERROR_3();  //Identifier must be followed by =.
void ERROR_4();  //const, int, procedure must be followed by identifier.
void ERROR_5();  //Semicolon or comma missing.
void ERROR_6();  //Incorrect symbol after procedure declaration.
void ERROR_7();  //Statement expected.
void ERROR_8();  //Incorrect symbol after statement part in block.
void ERROR_9();  //Period expected.
void ERROR_10(); //Semicolon between statements missing.
void ERROR_11(); //Undeclared identifier.
void ERROR_12(); //Assignment to constant or procedure is not allowed.
void ERROR_13(); //Assignment operator expected.
void ERROR_14(); //call must be followed by an identifier.
void ERROR_15(); //Call of a constant or variable is meaningless.
void ERROR_16(); //then	 expected.
void ERROR_17(); //Semicolon or } expected.
void ERROR_18(); //do expected.
void ERROR_19(); //Incorrect symbol following statement.
void ERROR_20(); //Relational operator expected.
void ERROR_21(); //Expression must not contain a procedure identifier.
void ERROR_22(); //Right parenthesis missing.
void ERROR_23(); //The preceding factor cannot begin with this symbol.
void ERROR_24(); //An expression cannot begin with this symbol.
void ERROR_25(); //This number is too large.
void ERROR_26(); //Identifier followed by an expression must be followed by :=


int main()
{
    typedef enum {
    nulsym = 1, identsym, numbersym, plussym, minussym,
    multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, intsym, procsym, writesym,
    readsym , elsesym } token_type;


    //open the input file
    FILE *ifp;  //input file pointer
    char *filename = "CG_input.txt";
    ifp = fopen(filename, "r");

    if(!ifp){
        printf("Error in opening '%s' for reading!", filename);
        exit(0);
    }

    //declare variables
    char c;

    while(!feof(ifp)){

        //read in file one character at a time
        c = getc(ifp);

        //call function BLOCK
        BLOCK(c, ifp);

        //the last symbol in the file should be a period
        if(c != periodsym){
            ERROR_9();
        }

    }//end while

    //close input file
    fclose(ifp);

    return 0;
}

////read the input file and store the values into a linked list
//void read_input_file(){
//
//    //open the input file
//    FILE *ifp;  //input file pointer
//    char *filename = "input.txt";
//    ifp = fopen(filename, "r");
//
//    if(!ifp){
//        printf("Error in opening '%s' for reading!", filename);
//        exit(0);
//    }
//
//    //declare variables
//    char c;
//
//    //create a struct for the array that will hold the tokens
//    symbol* symbol_array;
//    symbol_array = malloc(sizeof(symbol));
//    symbol_array->next = NULL;
//
//    //create a struct for the current token that is read in
//    //let the current_token point to the beginning of token_array
//    symbol* current_symbol = symbol_array;
//    current_token->next = NULL;
//
//    while(!feof(ifp)){
//
//        //read in file one character at a time
//        c = getc(ifp);
//
//        //store values in current_symbol
//
//
//
//    //close input file
//    fclose(ifp);
//
//}


void BLOCK(char c, FILE *ifp){

    //check for a constant
    if(char c == constsym){
        while (char c != commasym){
            c = getc(ifp);
            if(c != identsym){
                //const, int, procedure must be followed by identifier.
                ERROR_4();
            }//end if
            c = getc(ifp);
            if (c != eqsym){
                //Identifier must be followed by =.
                ERROR_3();
            }//end if
            c = getc(ifp);
            if(!isdigit(c)){
                //= must be followed by a number.
                ERROR_2();
            }//end if
            c = getc(ifp);
        }//end while
        if(c != semicolonsym){
            //Incorrect symbol following statement.
            ERROR_19();
        }//end if
        c = getc(ifp);
    }//end if constsym

    //check for intsym
    if(c == intsym){
        while(c != commasym){
            c = getc(ifp);
            if(c != identsym){
                //const, int, procedure must be followed by identifier.
                ERROR_4();
            }//end if
            c = getc(ifp);
        }//end while
        if(c != semicolonsym){
            //Incorrect symbol following statement.
            ERROR_19();
        }//end if
        c = getc(ifp);
    }//end if intsym

    //check for procsym
    while(c == procsym){
        c = getc(ifp);
        if(c != identsym){
            //const, int, procedure must be followed by identifier.
            ERROR_4();
        }//end if
        c = getc(ifp);
        if(c != semicolonsym){
            //Incorrect symbol following statement.
            ERROR_19();
        }//end if
        c = getc(ifp);
        //call BLOCK again
        BLOCK(c, ifp);
        if(c != semicolonsym){
            //Incorrect symbol following statement.
            ERROR_19();
        }//end if
        c = getc(ifp);
    }//end while procsym

    //call STATEMENT
    STATEMENT(c, ifp)

}//end BLOCK



STATEMENT(char c, FILE *ifp){

    //check for identifier
    if(c == identsym){
        c = getc(ifp);
        if(c != becomessym){
            //Identifier followed by an expression must be followed by :=
            ERROR_26();
        }
    }//end if

}//end STATEMENT

/* ************************************************************* */
/* Print ERROR messages                                          */
/* ************************************************************* */

//Use = instead of :=.
void ERROR_1(){
    printf("*****Error number 1, use = instead of :=");
    exit(0);
}

//= must be followed by a number.
void ERROR_2(){
    printf("*****Error number 2, = must be followed by a number");
    exit(0);
}

//Identifier must be followed by =.
void ERROR_3(){
    printf("*****Error number 3, Identifier must be followed by =");
    exit(0);
}

//const, int, procedure must be followed by identifier.
void ERROR_4(){
    printf("*****Error number 4, const, int, procedure must be followed by identifier.");
    exit(0);
}

//Semicolon or comma missing.
void ERROR_5(){
    printf("*****Error number 5, Semicolon or comma missing.");
    exit(0);
}

//Incorrect symbol after procedure declaration.
void ERROR_6(){
    printf("*****Error number 6, Incorrect symbol after procedure declaration.");
    exit(0);
}

//Statement expected.
void ERROR_7(){
    printf("*****Error number 7, Statement expected.");
    exit(0);
}

//Incorrect symbol after statement part in block.
void ERROR_8(){
    printf("*****Error number 8, Incorrect symbol after statement part in block.");
    exit(0);
}

//Period expected.
void ERROR_9(){
    printf("*****Error number 9, Period expected.");
    exit(0);
}

//Semicolon between statements missing.
void ERROR_10(){
    printf("*****Error number 10, Semicolon between statements missing.");
    exit(0);
}

//Undeclared identifier.
void ERROR_11(){
    printf("*****Error number 11, Undeclared identifier.");
    exit(0);
}

//Assignment to constant or procedure is not allowed.
void ERROR_12(){
    printf("*****Error number 12, Assignment to constant or procedure is not allowed.");
    exit(0);
}

//Assignment operator expected.
void ERROR_13(){
    printf("*****Error number 13, Assignment operator expected.");
    exit(0);
}

//call must be followed by an identifier.
void ERROR_14(){
    printf("*****Error number 14, call must be followed by an identifier.");
    exit(0);
}

//Call of a constant or variable is meaningless.
void ERROR_15(){
    printf("*****Error number 15, Call of a constant or variable is meaningless.");
    exit(0);
}

//then	 expected.
void ERROR_16(){
    printf("*****Error number 16, then	 expected.");
    exit(0);
}

//Semicolon or } expected.
void ERROR_17(){
    printf("*****Error number 17, Semicolon or } expected.");
    exit(0);
}

//do expected.
void ERROR_18(){
    printf("*****Error number 18, do expected.");
    exit(0);
}

//Incorrect symbol following statement.
void ERROR_19(){
    printf("*****Error number 19, Incorrect symbol following statement.");
    exit(0);
}

//Relational operator expected.
void ERROR_20(){
    printf("*****Error number 20, Relational operator expected.");
    exit(0);
}

//Expression must not contain a procedure identifier.
void ERROR_21(){
    printf("*****Error number 21, Expression must not contain a procedure identifier.");
    exit(0);
}

//Right parenthesis missing.
void ERROR_22(){
    printf("*****Error number 22, Right parenthesis missing.");
    exit(0);
}

//The preceding factor cannot begin with this symbol.
void ERROR_23(){
    printf("*****Error number 23, The preceding factor cannot begin with this symbol.");
    exit(0);
}

//An expression cannot begin with this symbol.
void ERROR_24(){
    printf("*****Error number 24, An expression cannot begin with this symbol.");
    exit(0);
}

 //This number is too large.
void ERROR_25(){
    printf("*****Error number 25, This number is too large.");
    exit(0);
}

//Identifier followed by an expression must be followed by :=
void ERROR_26(){
    printf("*****Identifier followed by an expression must be followed by :=");
    exit(0);
}
