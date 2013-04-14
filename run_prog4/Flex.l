/* Tabitha Cliver and Daniel Moore
System Software
Assignment 4 - Flex
Due 4/15/13 */

/*Tell the scanner to stop scanning at eof - no more files to scan*/
%option noyywrap

/*Put includes here to include them in yylex() (not in main)*/
%{
#include <stdio.h>
#include <stdlib.h>
%}

/*definitions*/
DIGIT      [0-9]
LETTER     [a-zA-Z]
IDENTIFIER [a-zA-Z][a-zA-Z0-9]*
WHITESPACE [ \t\n\r]


%x C_COMMENT


/*rules*/
%%

"/*"            { BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>.    { }

null {
	fprintf(yyout, " 1");
}

"+" {
	fprintf(yyout, " 4");
}

"-" {
	fprintf(yyout, " 5");
}

"*" {
	fprintf(yyout, " 6");
}

"/" {
	fprintf(yyout, " 7");
}

odd {
	fprintf(yyout, " 8");
}

"=" {
	fprintf(yyout, " 9");
}

"<>" {
	fprintf(yyout, " 10");
}

"<" {
	fprintf(yyout, " 11");
}

"<=" {
	fprintf(yyout, " 12");
}

">" {
	fprintf(yyout, " 13");
}

">=" {
	fprintf(yyout, " 14");
}

"(" {
	fprintf(yyout, " 15");
}

")" {
	fprintf(yyout, " 16");
}

"," {
	fprintf(yyout, " 17");
}

";" {
	fprintf(yyout, " 18");
}

"." {
	fprintf(yyout, " 19");
}

":=" {
	fprintf(yyout, " 20");
}

":" {
	printf("\n':' must be followed by '='\n\n");
	exit(0);
}

begin {
	fprintf(yyout, " 21");
}

end {
	fprintf(yyout, " 22");
}

if {
	fprintf(yyout, " 23");
}

then {
	fprintf(yyout, " 24");
}

while {
	fprintf(yyout, " 25");
}

do {
	fprintf(yyout, " 26");
}

call {
	fprintf(yyout, " 27");
}

const {
	fprintf(yyout, " 28");
}

int {
	fprintf(yyout, " 29");
}

procedure {
	fprintf(yyout, " 30");
}

write {
	fprintf(yyout, " 31");
}

read {
	fprintf(yyout, " 32");
}

else {
	fprintf(yyout, " 33");
}

{IDENTIFIER} {
	fprintf(yyout, " 2");
	fprintf(yyout, " %s", yytext);
	if(yyleng > 11){
		 printf("\nExceeds limit - identifier can be up to 11 characters long\n\n");
		 exit(0);
	}
}

{DIGIT}{LETTER}+ {
	printf("\nIdentifier cannot begin with a number\n\n");
	exit(0);
}

{DIGIT}+ {
	fprintf(yyout, " 3");
	fprintf(yyout, " %s", yytext);
	if(yyleng > 5) {
		printf("\nExceeds limit - numbers can be up to 5 digits long\n\n");
		exit(0);
	}
}

{WHITESPACE} {

}

[^{DIGIT}{LETTER}{WHITESPACE}"+""-""*""/""=""<>""<""<="">"">=""("")"","";""."":="":""/*""*/"] {
	printf("\nInvalid Symbol\n\n");
}

%%
/*end rules*/



main(int argc, char *argv[]){

/* 
token = text corresponding to match
char *yytext holds the text of the  current  token.
yytext = global character pointer to token
int yyleng = global integer of length of token 
default output file (of the generated scanner) is lex.yy.c
*/
	
	//Tell yyin to point to the input file. Default is stdin.
	//FILE  *yyin is the file which by default flex reads from.
	++argv, --argc; /*skip over program name*/
	if(argc > 0){
		yyin = fopen(argv[0], "r");
	}
	else {
		char *infile = "input1.txt";
		yyin = fopen(infile, "r");
	}
	if(!yyin){
		printf("Error in opening input text file for scanning!");
		exit(0);
	}

	//open the output file
	//Output from the ECHO macro is done to the  file-pointer  yyout (default stdout).
	//FILE *yyout;  //output file pointer
	char *outfile = "CG_out.txt";
	yyout = fopen(outfile, "w");

	if(!yyout){
		printf("Error in opening '%s' for writing!", outfile);
		exit(0);
	}
	
	//Tell flex to take over and write the scanner according to the rules given
	yylex();
	
	//close the files
	fclose(yyin);
	fclose(yyout);
	
	return(0);
}