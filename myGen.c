//my go at code generator
//will read CodeGen.out and produce correct function calls

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

//function declarations
//for language
void BLOCK(FILE* inFile);
void CONSTDEC(FILE* inFile);
void VARDEC(FILE* inFile);
void PROCDEC(FILE* inFile);
void STATEMENT(FILE* inFile);
void CONDITION();
void REL_OP();
void EXPRESSION();
void TERM();
void FACTOR();

//secodary syntax
void f_ident();
void f_call();
void f_begin();
void f_if();
void f_while();
void f_read();
void f_write();
void f_empty();

//helpers
void get_token(FILE* inFile);
char* get_name(FILE* inFile);
int get_num(FILE* inFile);
void ERROR(int i);

//Global Variables
static int token;
static int errors = 0;

int main(void){
	//declare variables for generator

	FILE* inFile = fopen("quick_debug.txt", "r");
	

	//main statements
	if(inFile == NULL){
		printf("Error opening file\n");
	}
	while(!feof(inFile)){

		get_token(inFile);
		BLOCK(inFile);

		if(token != 19){
			ERROR(9);
		}
	}//end file
	
	fclose(inFile);
}//end main

void BLOCK(FILE* inFile){
	
	if(token > 27 && token < 31){
		if(token == 28){

			get_token(inFile);
			CONSTDEC(inFile);
		}
		if(token == 29){

			get_token(inFile);
			VARDEC(inFile);
		}
		if(token == 30){

			get_token(inFile);
			PROCDEC(inFile);
		}
	}
	else{
		STATEMENT(inFile);
	}

	return;
}//end Block

void CONSTDEC(FILE* inFile){
	printf("Got into CONSTDEC\n");

	return;
}

void VARDEC(FILE* inFile){
	printf("Got into VARDEC\n");

	return;
}

void PROCDEC(FILE* inFile){
	printf("Got into PROCDEC");

	return;
}

void STATEMENT(FILE* inFile){
	printf("Got into STATEMENT");
	if(token == 2){
		f_ident();
	}else if(token == 27){
		f_call();
	}else if(token == 21){
		f_begin();
	}else if(token == 23){
		f_if();
	}else if(token == 25){
		f_while();
	}else if(token == 32){
		f_read();
	}else if(token == 31){
		f_write();
	}else{

	}
}

void f_ident(){}

void f_call(){}

void f_begin(){}

void f_if(){}

void f_while(){}

void f_read(){}

void f_write(){}

void f_empty(){}

void ERROR(int i){
	errors ++;
	switch(i){
		case 1:
			printf("ERROR #%d, use = instead of :=\n", i);
			exit(0);
		break;
		case 2:
			printf("ERROR #%d, = must be followed by a number\n", i);
			exit(0);
		break;
		case 3:
			printf("ERROR #%d, Identifier must be followed by =\n", i);
			exit(0);
		break;
		case 4:
			printf("ERROR #%d, const, int, procedure must be followed by identifier.\n", i);
			exit(0);
		break;
		case 5:
			printf("ERROR #%d, Semicolon or comma missing.\n", i);
			exit(0);
		break;
		case 6:
			printf("ERROR #%d, Incorrect symbol after procedure declaration.\n", i);
			exit(0);
		break;
		case 7:
			printf("ERROR #%d, Statement expected.\n", i);
			exit(0);
		break;
		case 8:
			printf("ERROR #%d, Incorrect symbol after statement part in block\n", i);
			exit(0);
		break;
		case 9:
			printf("ERROR #%d, No period at end of block\n", i);
			exit(0);
		break;
		case 10:
			printf("ERROR #%d, Semicolon between statements missing.\n", i);
			exit(0);
		break;
		case 11:
			printf("ERROR #%d, Undeclared identifier\n", i);
			exit(0);
		break;
		case 12:
			printf("ERROR #%d, Assignment to constant or procedure is not allowed\n", i);
			exit(0);
		break;
		case 13:
			printf("ERROR #%d, Assignment operator expected.\n", i);
			exit(0);
		break;
		case 14:
			printf("ERROR #%d, call must be followed by an identifier.\n", i);
			exit(0);
		break;
		case 15:
			printf("ERROR #%d, Call of a constant or variable is meaningless.\n", i);
			exit(0);
		break;
		case 16:
			printf("ERROR #%d, then	 expected.", i);
			exit(0);
		break;
		case 17:
			printf("ERROR #%d, Semicolon or } expected.\n", i);
			exit(0);
		break;
		case 18:
			printf("ERROR #%d, do expected.\n", i);
			exit(0);
		break;
		case 19:
			printf("ERROR #%d, Incorrect symbol following statement.\n", i);
			exit(0);
		break;
		case 20:
			printf("ERROR #%d, Relational operator expected\n", i);
			exit(0);
		break;
		case 21:
			printf("ERROR #%d, Expression must not contain a procedure identifier.\n", i);
			exit(0);
		break;
		case 22:
			printf("ERROR #%d, Right parenthesis missing.\n", i);
			exit(0);
		break;
		case 23:
			printf("ERROR #%d, The preceding factor cannot begin with this symbol.\n", i);
			exit(0);
		break;
		case 24:
			printf("ERROR #%d, An expression cannot begin with this symbol.\n", i);
			exit(0);
		break;
		case 25:
			printf("ERROR #%d, This number is too large.\n", i);
			exit(0);
		break;
		case 26:
			printf("ERROR #%d, Identifier followed by an expression must be followed by :=\n", i);
			exit(0);
		break;
		case 27:
			printf("ERROR #%d, Unexpected end of file.\n", i);
			exit(0);
		break; 
	}//end switch
}//end error

void get_token(FILE* inFile){

	if(!feof(inFile)){
		fscanf(inFile, "%d", &token);
		printf("Got here with %d\n", token);
	}else{
		ERROR(26);
	}
	return;
}//end get_token