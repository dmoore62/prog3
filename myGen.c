//my go at code generator
//will read CodeGen.out and produce correct function calls

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

struct cell
	{
		int op;
		int l;
		int m;
	};

struct code_block
	{
		struct cell block_cells[2000];
	};

struct symbol
    {
	int kind; 		// const = 1, var = 2, proc = 3
	char* name;	// name up to 11 chars
	int val; 		// number (ASCII value)
	int level; 		// L level
	int addr; 		// M address
	struct symbols_read_in* next;
    };

//function declarations
//for language
void BLOCK();
void CONSTDEC();
void VARDEC();
void PROCDEC();
void STATEMENT();
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
void get_token();
char* get_name();
int get_num();
void ERROR(int i);
void table_insert(struct symbol sym);
int find_ident(char* id);

//Global Variables
static int token;
static int errors = 0;
static FILE* inFile;
static struct symbol symbol_table[500];
static int table_index = 0;
static int level = 0;
static struct code_block level_blocks[5];

int main(void){
	//declare variables for generator
	
	inFile = fopen("quick_debug.txt", "r");
	

	//main statements
	if(inFile == NULL){
		printf("Error opening file\n");
	}
	while(!feof(inFile)){

		get_token();
		BLOCK();
		level --;

		if(token != 19){
			ERROR(9);
		}
	}//end file
	
	fclose(inFile);
}//end main

void BLOCK(){
	
	if(token > 27 && token < 31){
		if(token == 28){

			CONSTDEC();
		}
		if(token == 29){

			VARDEC();
		}
		if(token == 30){

			PROCDEC();
		}
	}
	
	STATEMENT();

	return;
}//end Block

void CONSTDEC(){
	//printf("Got into CONSTDEC\n");
	struct symbol sym;
	sym.kind = 1;
	sym.level = level;
	do{
		get_token();
		if(token == 2){
			sym.name = get_name();
			if(sym.name != NULL){
				printf("Got here!\n");
				get_token();
				if(token == 9){
					get_token();
					if(token == 3){
						sym.val = get_num();
						printf("%s, %d, %d\n", sym.name, sym.val, sym.level);
						if(!isalpha(sym.val)){
							table_insert(sym);
							sym.name = NULL;
							sym.val = (int)NULL;
							get_token();
						}else{
							ERROR(2);
						}
					}else{
						ERROR(2);
					}
				}else{
					ERROR(3);
				}
			}else{
				ERROR(29);
			}
		}else{
			ERROR(4);
		}
	}while(token == 17);
	if(token != 18){
		ERROR(5);
	}
	get_token();
	return;
}

void VARDEC(){
	struct symbol sym;
	sym.kind = 2;
	sym.level = level;
	do{
		get_token();
		if(token == 2){
			sym.name = get_name();
			if(sym.name != NULL){
				printf("%s, %d\n", sym.name, sym.level);
				table_insert(sym);
				sym.name = NULL;
				get_token();
			}else{
				ERROR(29);
			}
		}else{
			ERROR(4);
		}
	}while(token == 17);
	if(token != 18){
		ERROR(5);
	}
	get_token();
	return;
}

void PROCDEC(){
	struct symbol sym;
	sym.kind = 3;
	sym.level = level;
	do{
		get_token();
		if(token == 2){
			sym.name = get_name();
			printf("Got here!\n");
			if(sym.name != NULL){
				printf("%s, %d\n", sym.name, sym.level);
				table_insert(sym);
				sym.name = NULL;
				level ++;
				get_token();
				if(token == 18){
					get_token();
					BLOCK();
				}else{
					ERROR(17);
				}
			}else{
				ERROR(29);
			}
		}else{
			ERROR(4);
		}
	}while(token == 18);
	get_token();

	return;
}

void STATEMENT(){
	printf("Got into STATEMENT\n");
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
	}else if(token != 19){
		f_empty();
	}

	return;
}

void f_ident(){
	printf("Got into f_ident\n");
	struct symbol sym;
	int sym_index;
	sym.name = get_name();
	if(sym.name != NULL){
		sym_index = find_ident(sym.name);
		if(sym_index >= 0){
			get_token();
			if(token == 20){
				get_token();
				EXPRESSION();

				//>>>>>>>>>>>>>>>>>>>>>>>>>>>>need to do something here!

			}else{ERROR(13);}
		}else{ERROR(11);}
	}else{ERROR(11);}

	return;
}

void f_call(){
	struct symbol sym;
	int sym_index;
	get_token();
	if(token == 2){
		sym.name = get_name();
		if(sym.name != NULL){
			sym_index = find_ident(sym.name);
			if(sym_index >= 0){

				//>>>>>>>>>>>>>>>>>>>>>.need to do something here

			}else{ERROR(11);}
		}else{ERROR(11);}
	}else{ERROR(14);}

	return;
}

void f_begin(){
	do{
		get_token();
		STATEMENT();
	}while(token == 18);
	if(token == 22){
		get_token();
		return;
	}else{ERROR(30);}

	return;
}

void f_if(){
	get_token();
	CONDITION();
	if(token == 24){
		get_token();
		STATEMENT();
	}else{ERROR(16);}
}

void f_while(){
	get_token();
	CONDITION();
	if(token == 26){
		get_token();
		STATEMENT();
	}else{ERROR(18);}

	return;
}

void f_read(){
	struct symbol sym;
	int sym_index;
	get_token();
	if(token == 2){
		sym.name = get_name();
		if(sym.name != NULL){
			sym_index = find_ident(sym.name);
			if(sym_index >= 0){
				//need to do something here
			}else{ERROR(11);}
		}else{ERROR(11);}
	}else{ERROR(29);}

	return;
}

void f_write(){
	struct symbol sym;
	int sym_index;
	get_token();
	if(token == 2){
		sym.name = get_name();
		if(sym.name != NULL){
			sym_index = find_ident(sym.name);
			if(sym_index >= 0){
				//need to do something here
			}else{ERROR(11);}
		}else{ERROR(11);}
	}else{ERROR(29);}

	return;
}

void f_empty(){
	printf("EMPTY_STRING?\n");

	return;
}

void CONDITION(){
	if(token == 8){
		get_token();
		EXPRESSION();

		//>>>>>>>>>>>>>>>>>>>do something

	}else{
		get_token();
		EXPRESSION();
		if(token > 8 && token < 15){
			REL_OP();
		}else{ERROR(20);}
	}

	return;
}

void REL_OP(){
	switch(token){
		case 9:
			get_token();
			EXPRESSION();
		break;
		case 10:
			get_token();
			EXPRESSION();
		break;
		case 11:
			get_token();
			EXPRESSION();
		break;
		case 12:
			get_token();
			EXPRESSION();
		break;
		case 13:
			get_token();
			EXPRESSION();
		break;
		case 14:
			get_token();
			EXPRESSION();
		break;
	}//end switch

	return;
}

void EXPRESSION(){
	if(token == 4 || token == 5){
		get_token();
	}

	do{
		TERM();
		//get_token();
	}while(token == 4 || token == 5);

	return;
}

void TERM(){
	do{
		FACTOR();
		get_token();
	}while(token == 6 || token == 7);

	return;
}

void FACTOR(){
	struct symbol sym;
	int sym_index;
	
	if(token == 2){
		sym.name = get_name();
		if(sym.name != NULL){
			sym_index = find_ident(sym.name);
			if(sym_index >= 0){

				//>>>>>>>>>>>>>>>>>>>need to do something here

			}else{ERROR(11);}
		}else{ERROR(11);}
	}else if(token == 3){
		sym.val = get_num();
		if(!isalpha(sym.val)){
			printf("Got number! %d\n", sym.val);

			//>>>>>>>>>>>>>>>>>>>>>>>need to do something here

		}else{ERROR(2);}
	}else if(token == 15){
		get_token();
		EXPRESSION();
		if(token != 16){ERROR(22);}
	}else{ERROR(29);}

	return;
}

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
		case 28:
			printf("ERROR #%d, variable cannot begin with number\n", i);
			exit(0);
		break;
		case 29:
			printf("ERROR #%d, expecting variable name\n", i);
			exit(0);
		break;
		case 30:
			printf("ERROR #%d, 'End' Expected\n", i);
			exit(0);
		break;
	}//end switch
}//end error

void get_token(){

	if(!feof(inFile)){
		fscanf(inFile, "%d", &token);
		printf("Got here with %d\n", token);
	}else{
		ERROR(26);
	}
	return;
}//end get_token

char* get_name(){

	char* name = (char*)calloc(11, sizeof(char));
	fscanf(inFile, "%s", name);

	return name;
}//end get_name

int get_num(){
	int i;
	fscanf(inFile, "%d", &i);
	return i;
}//end get_num

void table_insert(struct symbol sym){
	if(sym.kind != (int)NULL){
		symbol_table[table_index].kind = sym.kind;
	}
	if(sym.name != NULL){
		symbol_table[table_index].name = sym.name;
	}
	if(sym.val != (int)NULL){
		symbol_table[table_index].val = sym.val;
	}
	if(sym.level != (int)NULL){
		symbol_table[table_index].level = sym.level;
	}
	if(sym.addr != (int)NULL){
		symbol_table[table_index].addr = sym.addr;
	}
	table_index ++;

	return;
}

int find_ident(char* id){
	int i;
	for(i = 0; i < table_index; i ++){
		if(strcmp(id, symbol_table[i].name) == 0){
			return i;
		}
	}

	return -1;
}