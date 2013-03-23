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
#define MAX_SYMBOL_TABLE_SIZE 2000


typedef struct symbols_read_in
    {
    int kind;       // const = 1, var = 2, proc = 3
    char name[10];  // name up to 11 chars
    int val;        // number (ASCII value)
    int level;      // L level
    int addr;       // M address
//  struct symbols_read_in* next;
    } symbol;


symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];


void BLOCK(char c[], FILE *ifp, int Lev, int tx);
void STATEMENT(int c, FILE *ifp);
void CONDITION(int c, FILE *ifp);
void EXPRESSION(int c, FILE *ifp);
void TERM(int c, FILE *ifp);
void FACTOR(int c, FILE *ifp);
void ENTER(int type, char name[], int val, int tx, int dx, int Lev);

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
void ERROR_16(); //then  expected.
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
void ERROR_27(); //end expected

typedef enum {
    nulsym = 1, identsym, numbersym, plussym, minussym,
    multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, intsym, procsym, writesym,
    readsym , elsesym } token_type;

int main()
{

    //open the input file
    FILE *ifp;  //input file pointer
    char *filename = "input.txt";
    ifp = fopen(filename, "r");

    if(!ifp){
        printf("Error in opening '%s' for reading!", filename);
        exit(0);
    }

    //declare variables
    int c;          //token read from file
    char s[11];     //identifier read from file
    int Lev = 0;    //lexicological level
    int tx = 1;     //table index
    int dx = 4;     //data index = distance from bottom data is located

    while(!feof(ifp)){

        //read in file one character at a time
        fscanf(ifp, "%d", &c);

        //call function BLOCK
        BLOCK(c, ifp, Lev, tx);

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
//        fscanf(ifp, "%d", &c);
//
//        //store values in current_symbol
//
//
//
//    //close input file
//    fclose(ifp);
//
//}


void BLOCK(int c, FILE *ifp, int Lev, int tx){

    //declare variables
    int type;    //keep track of type stored in symbol table - const = 1, var = 2, proc = 3
    char s[11];  //stores token if token is a string (identifier)
    int dx = 4;  //data index = distance from bottom data is located
    int addr;    //M address for symbol table

    //store the table index
    int tx0 = tx;

    //check that the maximum lexicological level is not exceeded
    if(Lev > MAX_LEXI_LEVELS){
        printf("*****Error - maximum lexicological levels exceeded.");
        exit(0);
    }

    //check for a constant (const-declaration)
    if(c == constsym){
        while (c == commasym){
            fscanf(ifp, "%d", &c);
            if(c != identsym){
                //const, int, procedure must be followed by identifier.
                ERROR_4();
            }//end if
            //the identsym should be followed by the identifier
            fscanf(ifp, "%s", s);
            fscanf(ifp, "%d", &c);
            if (c != eqsym){
                //Identifier must be followed by =.
                ERROR_3();
            }//end if
            fscanf(ifp, "%d", &c);
            if(!isdigit(c)){
                //= must be followed by a number.
                ERROR_2();
            }//end if
            //type for symbole table is const = 1
            type = 1;
            ENTER(type, s, c, tx, dx, Lev);
            fscanf(ifp, "%d", &c); //if this is a comma, the loop is repeated
        }//end while
        if(c != semicolonsym){
            //Incorrect symbol following statement.
            ERROR_19();
        }//end if
        fscanf(ifp, "%d", &c);
    }//end if constsym

    //check for intsym (var-declaration)
    if(c == intsym){
        while(c == commasym){
            fscanf(ifp, "%d", &c);
            if(c != identsym){
                //const, int, procedure must be followed by identifier.
                ERROR_4();
            }//end if
            //the next token should be an identifier, i.e. it will not be an int
            fscanf(ifp, "%s", s);
            //type for symbole table is var = 2
            type = 2;
            ENTER(type, s, c, tx, dx, Lev);
            fscanf(ifp, "%d", c); //if this is a comma, the loop is repeated
        }//end while
        if(c != semicolonsym){
            //Incorrect symbol following statement.
            ERROR_19();
        }//end if
        fscanf(ifp, "%d", &c);
    }//end if intsym

    //check for procsym (procedure-declaration)
    while(c == procsym){
        fscanf(ifp, "%d", &c);
        if(c != identsym){
            //const, int, procedure must be followed by identifier.
            ERROR_4();
        }//end if
        //the next token should be an identifier, i.e. it will not be an int
        fscanf(ifp, "%s", s);
        //type for symbole table is proc = 3
        type = 3;
        ENTER(type, s, c, tx, dx, Lev);
        fscanf(ifp, "%d", &c);
        if(c != semicolonsym){
            //Incorrect symbol following statement.
            ERROR_19();
        }//end if
        fscanf(ifp, "%d", &c);
        //call BLOCK again
        BLOCK(c, ifp, Lev, tx);
        if(c != semicolonsym){
            //Incorrect symbol following statement.
            ERROR_19();
        }//end if
        fscanf(ifp, "%d", &c);
    }//end while procsym

    //call STATEMENT
    STATEMENT(c, ifp);

}//end BLOCK



void STATEMENT(int c, FILE *ifp){

    //check for identifier
    if(c == identsym){
        fscanf(ifp, "%d", &c);
        if(c != becomessym){
            //Identifier followed by an expression must be followed by :=
            ERROR_26();
        }//end if
        fscanf(ifp, "%d", &c);
        EXPRESSION(c, ifp);
    }//end if
    else if(c == callsym){
        fscanf(ifp, "%d", &c);
        if(c != identsym){
            //call must be followed by an identifier.
            ERROR_14();
        }//end if
        fscanf(ifp, "%d", &c);
    }//end if callsym
    else if(c == beginsym){
        fscanf(ifp, "%d", &c);
        STATEMENT(c, ifp);
        while(c == semicolonsym){
            fscanf(ifp, "%d", &c);
            STATEMENT(c, ifp);
        }//end while
        if(c != endsym){
            //end expected
            ERROR_27();
        }
        fscanf(ifp, "%d", &c);
    }//end if beginsym
    else if(c == ifsym){
        fscanf(ifp, "%d", &c);
        CONDITION(c, ifp);
        if(c != thensym){
            //then   expected.
            ERROR_16();
        }
        fscanf(ifp, "%d", &c);
        STATEMENT(c, ifp);
    }//end if ifsym
    else if(c == whilesym){
        fscanf(ifp, "%d", &c);
        CONDITION(c, ifp);
        if(c != dosym){
            //do expected.
            ERROR_18();
        }
        fscanf(ifp, "%d", &c);
        STATEMENT(c, ifp);
    }//end if whilesym

}//end STATEMENT

void CONDITION(int c, FILE *ifp){
    if(c == oddsym){
        fscanf(ifp, "%d", &c);
        EXPRESSION(c, ifp);
    }
    else{
        EXPRESSION(c, ifp);
        if((c != eqsym)||(c != neqsym)||(c != lessym)||(c != leqsym)||(c != gtrsym)||(c != geqsym)){
            //Relational operator expected.
            ERROR_20();
        }
        fscanf(ifp, "%d", &c);
        EXPRESSION(c, ifp);
    }//end else
}//end CONDITION

void EXPRESSION(int c, FILE *ifp){
    if((c == plussym)||(c == minussym)){
        fscanf(ifp, "%d", &c);
    }//end if
    TERM(c, ifp);
    while((c == plussym)||(c == minussym)){
        fscanf(ifp, "%d", &c);
        TERM(c,ifp);
    }//end while
}//end EXPRESSION

void TERM(int c, FILE *ifp){
    FACTOR(c, ifp);
    while((c == multsym)||(c == slashsym)){
        fscanf(ifp, "%d", &c);
        FACTOR(c, ifp);
    }//end while
}//end TERM

void FACTOR(int c, FILE *ifp){
    if(c == identsym){
        fscanf(ifp, "%d", &c);
    }
    else if(isdigit(c)){
        fscanf(ifp, "%d", &c);
    }
    else if(c == lparentsym){
        fscanf(ifp, "%d", &c);
        EXPRESSION(c, ifp);
        if(c != rparentsym){
            //Right parenthesis missing.
            ERROR_22();
        }
        fscanf(ifp, "%d", &c);
    }
    else{
        //The preceding factor cannot begin with this symbol.
        ERROR_23();
    }
}//end FACTOR



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

//then   expected.
void ERROR_16(){
    printf("*****Error number 16, then   expected.");
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
    printf("*****Identifier must be followed by := when followed by an expression");
    exit(0);
}

//end expected
void ERROR_27(){
    printf("*****end expected");
    exit(0);
}
