#!/bin/bash
gcc -o theLA.out theLA.c
./theLA.out
gcc -o myGen.out myGen1.c
./myGen.out > Gen_out.txt
gcc -o P-Machine.out P-Machine.c
./P-Machine.out 
exit