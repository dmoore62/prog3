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
		int size;
	};

struct symbol
    {
	int kind; 		// const = 1, var = 2, proc = 3
	char* name;	// name up to 11 chars
	int val; 		// number (ASCII value)
	int level; 		// L level
	int addr; 		// M address
	//struct symbols_read_in* next;
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
int get_table_addr();
void print_code_arrays();

//Global Variables
static int token;
static int errors = 0;
static FILE* inFile;
static FILE* outFile;
static struct symbol symbol_table[500];
static int table_index = 0;
static int level = 0;
static struct code_block level_blocks[3];
static int jpc_holder[10];
static int jpc_holder_index = 0;
static struct code_block master_block;
static int level_addr_index[3];
static int level_offset = 0;
static int jmp_holder[10];

int main(void){
	//declare variables for generator
	int i;
	for (i = 0; i < 3; i ++){
		level_blocks[i].size = 0;
	}


	master_block.size = 0;
	level_addr_index[0] = 4;
	level_addr_index[1] = 4;
	level_addr_index[2] = 4;
	inFile = fopen("CG_out.txt", "r");
	outFile = fopen("toVM.txt", "w");
	int stitch_index;

	//main statements
	if(inFile == NULL){
		printf("Error opening file\n");
	}
	while(!feof(inFile)){

		get_token();
		BLOCK();
		//update jump address in master table
		master_block.block_cells[level].m = master_block.size;

		//stitch code block from this level into master
		level_offset = master_block.size;
		for(stitch_index = 0; stitch_index < level_blocks[level].size; stitch_index ++){
			if(level_blocks[level].block_cells[stitch_index].op == 8 || level_blocks[level].block_cells[stitch_index].op == 7){
				level_blocks[level].block_cells[stitch_index].m += level_offset;
			}
			master_block.block_cells[master_block.size] = level_blocks[level].block_cells[stitch_index];
			master_block.size ++;
		}
		level --;

		if(token != 19){
			ERROR(9);
		}
	}//end file

	print_code_arrays();
	
	fclose(inFile);
	fclose(outFile);

}//end main

void BLOCK(){
	struct cell jmp_cell;
	jmp_cell.op = 7;
	jmp_cell.l = 0;
	jmp_cell.m = 0;
	master_block.block_cells[master_block.size] = jmp_cell;
	master_block.size ++;

	printf("Called Block\n");
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
	struct cell inc_cell;
	struct cell opr_cell;
	printf("in f_begin\n");
	//create machine code for increment
	inc_cell.op = 6;
	inc_cell.l = 0;
	inc_cell.m = level_addr_index[level];
	printf("set increament level to %d at level %d\n", inc_cell.m, level);
	//put cell in appropriate position
	level_blocks[level].block_cells[level_blocks[level].size] = inc_cell;
	level_blocks[level].size ++;
	
	STATEMENT();

	opr_cell.op = 2;
	opr_cell.l = 0;
	opr_cell.m = 0;
	//put cell in appropriate position
	level_blocks[level].block_cells[level_blocks[level].size] = opr_cell;
	level_blocks[level].size ++;

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
						//if(!isalpha(sym.val)){
							table_insert(sym);
							sym.name = NULL;
							sym.val = (int)NULL;
							get_token();
						//}else{
						//	ERROR(2);
						//}
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
			sym.addr = get_table_addr();
			if(sym.name != NULL){
				printf("%s, %d, %d\n", sym.name, sym.level, sym.addr);
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
	int sym_index;
	int stitch_index;
	struct symbol sym;
	sym.kind = 3;
	sym.level = level;
	do{
		get_token();
		if(token == 21){
			break;
		}
		if(token == 2){
			sym.name = get_name();
			printf("Got here!\n");
			if(sym.name != NULL){
				printf("%s, %d\n", sym.name, sym.level);
				table_insert(sym);
				sym_index = find_ident(sym.name);
				sym.name = NULL;
				level ++;
				get_token();
				if(token == 18){
					get_token();
					BLOCK();
					//update the address of the previous proc declaration
					symbol_table[sym_index].addr = master_block.size;

					//update jump address in master table
					master_block.block_cells[level].m = master_block.size;

					//stitch code block from this level into master
					level_offset = master_block.size;
					for(stitch_index = 0; stitch_index < level_blocks[level].size; stitch_index ++){
						if(level_blocks[level].block_cells[stitch_index].op == 8){
							level_blocks[level].block_cells[stitch_index].m += level_offset;
						}
						master_block.block_cells[master_block.size] = level_blocks[level].block_cells[stitch_index];
						master_block.size ++;
					}

					level --;
					printf("back From Block\n");
				}else{
					ERROR(17);
				}
			}else{
				ERROR(29);
			}
		}else{
			printf("This one!\n");
			ERROR(4);
		}
	}while(token == 18);
	if(token != 21){
		get_token();
	}
	return;
}

void STATEMENT(){
	printf("Got into STATEMENT\n");
	if(token == 2){
		f_ident();
	}else if(token == 27){
		printf("calling call\n");
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
	struct cell sto_cell;
	int sym_index;
	sym.name = get_name();
	if(sym.name != NULL){
		sym_index = find_ident(sym.name);
		if(sym_index >= 0){
			get_token();
			if(token == 20){
				get_token();
				EXPRESSION();
				//create code to store cell
				sto_cell.op = 4;
				sto_cell.l = (level - symbol_table[sym_index].level);
				sto_cell.m = symbol_table[sym_index].addr;
				//put cell in appropriate cell_block
				level_blocks[level].block_cells[level_blocks[level].size] = sto_cell;
				level_blocks[level].size ++;

			}else{ERROR(13);}
		}else{ERROR(11);}
	}else{ERROR(11);}

	return;
}

void f_call(){
	struct symbol sym;
	struct cell cal_cell;
	int sym_index;
	get_token();
	if(token == 2){
		sym.name = get_name();
		if(sym.name != NULL){
			sym_index = find_ident(sym.name);
			if(sym_index >= 0){
				cal_cell.op = 5;
				cal_cell.l = 0;
				cal_cell.m = symbol_table[sym_index].addr;
				//put cell in appropriate position
				level_blocks[level].block_cells[level_blocks[level].size] = cal_cell;
				level_blocks[level].size ++;
				get_token();
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
	printf("leaving f_begin\n");
	if(token == 22){
		get_token();
		return;
	}else{ERROR(30);}

	return;
}

void f_if(){
	struct cell if_cell;
	if_cell.op = 8;
	if_cell.l = 0;
	get_token();
	CONDITION();
	if(token == 24){
		get_token();
		//put empty jpc cell at this level and remeber index
		jpc_holder[jpc_holder_index] = level_blocks[level].size;
		jpc_holder_index ++;
		//place unfinished jump conditional cell in place
		level_blocks[level].block_cells[level_blocks[level].size] = if_cell;
		level_blocks[level].size ++;
		STATEMENT();
		//back from statement block, must update jump condition m value
		jpc_holder_index --;
		level_blocks[level].block_cells[jpc_holder[jpc_holder_index]].m = level_blocks[level].size;
	}else{ERROR(16);}

	return;
}

void f_while(){
	struct cell while_cell;
	while_cell.op = 8;
	while_cell.l = 0;
	//mark the point to jump back to at the end of every while termination
	jmp_holder[jpc_holder_index] = level_blocks[level].size;
	get_token();
	CONDITION();
	if(token == 26){
		get_token();
		//put empty jpc cell at this level and remeber index
		jpc_holder[jpc_holder_index] = level_blocks[level].size;
		jpc_holder_index ++;
		//place unfinished jump conditional cell in place
		level_blocks[level].block_cells[level_blocks[level].size] = while_cell;
		level_blocks[level].size ++;
		STATEMENT();
		//back from statement block, jump back to while condition
		jpc_holder_index --;
		struct cell jmp_cell;
		jmp_cell.op = 7;
		jmp_cell.l = 0;
		jmp_cell.m = jmp_holder[jpc_holder_index];
		level_blocks[level].block_cells[level_blocks[level].size] = jmp_cell;
		level_blocks[level].size ++;
		//back from statement block, must update jump condition m value
		level_blocks[level].block_cells[jpc_holder[jpc_holder_index]].m = level_blocks[level].size;
	}else{ERROR(18);}

	return;
}

void f_read(){
	struct cell read_cell;
	struct cell lod_cell;
	struct symbol sym;
	int sym_index;
	get_token();
	if(token == 2){
		sym.name = get_name();
		if(sym.name != NULL){
			sym_index = find_ident(sym.name);
			if(sym_index >= 0){
				//send read command
				read_cell.op = 10;
				read_cell.l = 0;
				read_cell.m = 2;
				//put cell in appropriate position
				level_blocks[level].block_cells[level_blocks[level].size] = read_cell;
				level_blocks[level].size ++;
				//load address to stack
				lod_cell.op = 4;
				lod_cell.l = level - symbol_table[sym_index].level;
				lod_cell.m = symbol_table[sym_index].addr;
				//put cell in appropriate position
				level_blocks[level].block_cells[level_blocks[level].size] = lod_cell;
				level_blocks[level].size ++;
				get_token();
			}else{ERROR(11);}
		}else{ERROR(11);}
	}else{ERROR(29);}

	return;
}

void f_write(){
	struct cell wrt_cell;
	struct cell lod_cell;
	struct symbol sym;
	int sym_index;
	get_token();
	if(token == 2){
		sym.name = get_name();
		if(sym.name != NULL){
			sym_index = find_ident(sym.name);
			if(sym_index >= 0){
				//load address to stack
				lod_cell.op = 3;
				lod_cell.l = level - symbol_table[sym_index].level;
				lod_cell.m = symbol_table[sym_index].addr;
				//put cell in appropriate position
				level_blocks[level].block_cells[level_blocks[level].size] = lod_cell;
				level_blocks[level].size ++;
				//send write command
				wrt_cell.op = 9;
				wrt_cell.l = 0;
				wrt_cell.m = 1;
				//put cell in appropriate position
				level_blocks[level].block_cells[level_blocks[level].size] = wrt_cell;
				level_blocks[level].size ++;
				get_token();
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
	struct cell rel_cell;
	if(token == 8){
		get_token();
		EXPRESSION();
		rel_cell.op = 2;
		rel_cell.l = 0;
		rel_cell.m = 6;
		level_blocks[level].block_cells[level_blocks[level].size] = rel_cell;
		level_blocks[level].size ++;		

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
	struct cell rel_cell;
	rel_cell.op = 2;
	rel_cell.l = 0;

	switch(token){
		case 9:
			get_token();
			EXPRESSION();
			rel_cell.m = 8;
			//put cell in appropriate cell_block
			level_blocks[level].block_cells[level_blocks[level].size] = rel_cell;
			level_blocks[level].size ++;

		break;
		case 10:
			get_token();
			EXPRESSION();
			rel_cell.m = 9;
			//put cell in appropriate cell_block
			level_blocks[level].block_cells[level_blocks[level].size] = rel_cell;
			level_blocks[level].size ++;
		break;
		case 11:
			get_token();
			EXPRESSION();
			rel_cell.m = 10;
			//put cell in appropriate cell_block
			level_blocks[level].block_cells[level_blocks[level].size] = rel_cell;
			level_blocks[level].size ++;
		break;
		case 12:
			get_token();
			EXPRESSION();
			rel_cell.m = 11;
			//put cell in appropriate cell_block
			level_blocks[level].block_cells[level_blocks[level].size] = rel_cell;
			level_blocks[level].size ++;
		break;
		case 13:
			get_token();
			EXPRESSION();
			rel_cell.m = 12;
			//put cell in appropriate cell_block
			level_blocks[level].block_cells[level_blocks[level].size] = rel_cell;
			level_blocks[level].size ++;
		break;
		case 14:
			get_token();
			EXPRESSION();
			rel_cell.m = 13;
			//put cell in appropriate cell_block
			level_blocks[level].block_cells[level_blocks[level].size] = rel_cell;
			level_blocks[level].size ++;
		break;
	}//end switch

	return;
}

void EXPRESSION(){
	struct cell exp_cell;
	exp_cell.op = 2;
	exp_cell.l = 0;

	if ((token == 4)||(token == 5)){
	    //if the number is negative
	    if(token == 5){
	        exp_cell.m = 1;
	        //put cell in appropriate cell_block
	        level_blocks[level].block_cells[level_blocks[level].size] = exp_cell;
			level_blocks[level].size ++;
	    }
        get_token();
        TERM();
	}
	else{
        TERM();
	}

	while(token == 4 || token == 5){
		if(token == 4){
			get_token();
			TERM();
			exp_cell.m = 2;
			//put cell in appropriate cell_block
			level_blocks[level].block_cells[level_blocks[level].size] = exp_cell;
			level_blocks[level].size ++;
		}else if(token == 5){
			get_token();
			TERM();
			exp_cell.m = 3;
			//put cell in appropriate cell_block
			level_blocks[level].block_cells[level_blocks[level].size] = exp_cell;
			level_blocks[level].size ++;
		}
	}//end while

	return;
}

void TERM(){
	struct cell term_cell;
	term_cell.op = 0;
	term_cell.l = 0;

	do{
		FACTOR();
		//get_token();
		if(token == 6){
			term_cell.m = 4;
			//put cell in appropriate cell_block
			level_blocks[level].block_cells[level_blocks[level].size] = term_cell;
			level_blocks[level].size ++;
		}else if(token == 7){
			term_cell.m = 5;
			//put cell in appropriate cell_block
			level_blocks[level].block_cells[level_blocks[level].size] = term_cell;
			level_blocks[level].size ++;
		}
		

	}while(token == 6 || token == 7);

	return;
}

void FACTOR(){
	struct symbol sym;
	struct cell lit_cell;
	int sym_index;
	
	if(token == 2){
		sym.name = get_name();
		if(sym.name != NULL){
			sym_index = find_ident(sym.name);
			if(sym_index >= 0){
				//get symbol info
				sym = symbol_table[sym_index];
				printf("Found symbol, %s, %d, %d, %d\n", sym.name, sym.level, sym.val, sym.addr);
				if(sym.addr > 0 && sym.addr < 500){
					//has addr load from VM
					//create new cell with proper info
					lit_cell.op = 3;
					lit_cell.l = level - sym.level;
					lit_cell.m = sym.addr;
				}else{
					//no addr push lit to stack
					lit_cell.op = 1;
					lit_cell.l = 0;
					lit_cell.m = sym.val;
				}
				//put cell in appropriate cell_block
				level_blocks[level].block_cells[level_blocks[level].size] = lit_cell;
				level_blocks[level].size ++;
				

			}else{ERROR(11);}
		}else{ERROR(11);}
	}else if(token == 3){
		sym.val = get_num();
		//if(!isalpha(sym.val)){
			printf("Got number! %d\n", sym.val);
			//input number into code list
			lit_cell.op = 1;
			lit_cell.l = 0;
			lit_cell.m = sym.val;
			//put cell in appropriate cell_block
			level_blocks[level].block_cells[level_blocks[level].size] = lit_cell;
			level_blocks[level].size ++;

		//}else{ERROR(2);}
	}else if(token == 15){
		get_token();
		EXPRESSION();
		if(token != 16){ERROR(22);}
	}else{ERROR(29);}
	get_token();
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
	int maybe = -1;
	for(i = 0; i < table_index; i ++){
		if(strcmp(id, symbol_table[i].name) == 0){
			if(symbol_table[i].level == level){
				return i;
			}else{
				maybe = i;
			}
		}
	}

	return maybe;
}

int get_table_addr(){
	int i = -1;
	
	i = level_addr_index[level] ++;
	printf("Increased increment to %d\n", i);
	return i;
}

void print_code_arrays(){
	int i, j;
	for(i = 2; i > -1; i --){
		if(level_blocks[i].size > 0){
			printf("Code Block %d:\n\n", i);
			for(j = 0; j < level_blocks[i].size; j++){
				printf("%d %d %d\n", level_blocks[i].block_cells[j].op,
										level_blocks[i].block_cells[j].l,
										level_blocks[i].block_cells[j].m);
			}
		}
	}

	printf("\nMaster Block: \n\n");
	for(j = 0; j < master_block.size; j ++){
		printf("%d %d %d\n", master_block.block_cells[j].op, master_block.block_cells[j].l, master_block.block_cells[j].m);
		fprintf(outFile, "%d %d %d\n", master_block.block_cells[j].op, master_block.block_cells[j].l, master_block.block_cells[j].m);
	}
}