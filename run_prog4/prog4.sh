#!/bin/bash
FILEVAR="input8.txt"
flex Flex.l
gcc -o Scanner lex.yy.c
Scanner $FILEVAR
gcc -o myGen.out myGen1.c
./myGen.out < $FILEVAR > output.txt
gcc -o P-Machine.out P-Machine.c
./P-Machine.out >> output.txt
exit