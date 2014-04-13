@ECHO OFF
@COLOR 1F
@CLS
REM ./BUILDMYOS.CMD
@ECHO START.ASM
nasm -f aout ./SRC/START.ASM -o ./OBJ/START.O
@ECHO MAIN.C
gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/MAIN.O ./SRC/MAIN.c
@ECHO LINKING KERNEL.BIN
LD -T LINK.LD -o B:\KERNEL.BIN ./OBJ/START.O ./OBJ/MAIN.O ./LIB/libmyos.a
