@ECHO OFF
@COLOR 1F

SET FLOPPY_IMAGE=./floppy
SET OUTPUT_FILE=B:\CLOCK.EXE
SET LD_SCRIPT=LINK.LD
SET GCC_OPTIONS=-Wall -std=gnu99 -nostdinc -c -o

:START
	CLS
	GOTO MENU
	
:MENU
	ECHO -------------------------MY OS MENU-------------------------
	ECHO ------------------------------------------------------------
	ECHO 1.    BUILD Executable
	ECHO 2.    SAVE Image
	ECHO 3.    Run Bochs
	ECHO 4.    ALL
	ECHO 0.    EXIT
	ECHO ------------------------------------------------------------
	SET INPUT=
	SET /P INPUT=ACTION:
	IF /I '%INPUT%'=='1' (
		@ECHO ENTRY.ASM
		nasm -f aout ./ENTRY.ASM -o ./ENTRY.O
		@ECHO MAIN.C
		gcc %GCC_OPTIONS% ./MAIN.O ./MAIN.c
		@ECHO LINKING
		LD -T %LD_SCRIPT% -o %OUTPUT_FILE% ./ENTRY.O ./MAIN.O
	)
	IF /I '%INPUT%'=='2' (
		vfd save
	)
	IF /I '%INPUT%'=='3' (
		cd ..
		bochs
		cd "Test Exe"
	)
	IF /I '%INPUT%'=='4' (
		@ECHO ENTRY.ASM
		nasm -f aout ./ENTRY.ASM -o ./ENTRY.O
		@ECHO MAIN.C
		gcc %GCC_OPTIONS% ./MAIN.O ./MAIN.c
		@ECHO LINKING
		LD -T %LD_SCRIPT% -o %OUTPUT_FILE% ./ENTRY.O ./MAIN.O
		vfd save
		cd ..
		bochs
		cd "Test Exe"
	)
	IF /I '%INPUT%'=='0' EXIT /B
	
	PAUSE
	GOTO START
