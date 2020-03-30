#!/bin/bash

gcc -o loadFile ./src/loadFile.c

# nasm -f as86 ./ass/lib.asm -o ./obj/lib.o     We want to keep the modified object code...

bcc -ansi -c -o ./obj/shell.o ./src/shell.c

ld86 -o shell -d ./obj/shell.o ./obj/lib.o

./loadFile shell

./loadFile hello

# ./loadFile milestone1