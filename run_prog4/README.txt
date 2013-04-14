Daniel Moore
Tabitha Cliver
HW 4 - 4/15/2013
for COP3402
README File

Folder run_prog4/
Included Files:

myGen1.c - Code generator
P-Machine.c - Virtual Machine
Flex.l - Lexical Analyzer
prog4.sh - Bash script that runs compiler
input[1-8].txt - test files
CG_out.txt - file holds tokens to be read by myGen1.c
output.txt - code echo and object code and stack output
Scanner.exe - scanner created by flex file

To Run Compiler:

Copy desired code into file "input8.txt"

-OR-

Modify line 2 in prog4.sh and set FILEVAR to the desired file name

From Command line type:
chmod +x ./prog4.sh [enter]
./prog4.sh [enter]

File output.txt will hold the appropriate output

If there are any errors, the error will be displayed after the Source Code in output.txt
and no object code will be created.

Otherwise, there will be a message displaying that no error were found, and the object code
and stack will be displayed.




