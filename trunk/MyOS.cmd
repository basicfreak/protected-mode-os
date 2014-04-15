@ECHO OFF
@COLOR 1F
:START
	CLS
	GOTO MENU
	
:MENU
	ECHO -------------------------MY OS MENU-------------------------
	ECHO ------------------------------------------------------------
	ECHO 1.    BUILD LIBRARY
	ECHO 2.    BUILD KERNEL
	ECHO 3.    SAVE IMAGE
	ECHO 4.    RUN BOCHS
	ECHO 5.    BUILD ALL + SAVE + RUN KERNEL
	ECHO 6.    OPEN IMAGE
	ECHO 7.    CLOSE IMAGE
	ECHO 0.    EXIT
	ECHO ------------------------------------------------------------
	SET INPUT=
	SET /P INPUT=ACTION:
	IF /I '%INPUT%'=='1' (
		@ECHO CMOS.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/CMOS.O ./LIBSRC/CMOS.c
		@ECHO DMA.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/DMA.O ./LIBSRC/DMA.c
		@ECHO FDC.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/FDC.O ./LIBSRC/FDC.c
		@ECHO GDT.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/GDT.O ./LIBSRC/GDT.c
		@ECHO IDT.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/IDT.O ./LIBSRC/IDT.c
		@ECHO IRQ.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/IRQ.O ./LIBSRC/IRQ.c
		@ECHO KEYBOARD.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/KEYBOARD.O ./LIBSRC/KEYBOARD.c
		@ECHO MATH.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/MATH.O ./LIBSRC/MATH.c
		@ECHO STDIO.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/STDIO.O ./LIBSRC/STDIO.c
		@ECHO STRING.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/STRING.O ./LIBSRC/STRING.c
		@ECHO TIMER.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/TIMER.O ./LIBSRC/TIMER.c
		@ECHO TSS.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/TSS.O ./LIBSRC/TSS.c
		@ECHO lib.a
		ar rvs ./LIB/lib.a ./OBJ/CMOS.O ./OBJ/GDT.O ./OBJ/IDT.O ./OBJ/IRQ.O ./OBJ/KEYBOARD.O
		ar rvs ./LIB/lib.a ./OBJ/TIMER.O ./OBJ/MATH.O ./OBJ/STDIO.O ./OBJ/STRING.O ./OBJ/DMA.O
		ar rvs ./LIB/lib.a ./OBJ/FDC.O ./OBJ/TSS.O
		@ECHO MEM/PHYSICAL.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/PHYSICAL.O ./LIBSRC/MEM/PHYSICAL.c
		@ECHO MEM/VIRTUAL.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/VIRTUAL.O ./LIBSRC/MEM/VIRTUAL.c
		@ECHO MEM/PAGETABLE.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/PAGETABLE.O ./LIBSRC/MEM/PAGETABLE.c
		@ECHO MEM/PAGEDIR.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/PAGEDIR.O ./LIBSRC/MEM/PAGEDIR.c
		@ECHO MEM/PAGEFAULT.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/PAGEFAULT.O ./LIBSRC/MEM/PAGEFAULT.c
		@ECHO libMEM.a
		ar rvs ./LIB/libMEM.a ./OBJ/PHYSICAL.O ./OBJ/VIRTUAL.O ./OBJ/PAGETABLE.O ./OBJ/PAGEDIR.O ./OBJ/PAGEFAULT.O
	)
	IF /I '%INPUT%'=='2' (
		@ECHO START.ASM
		nasm -f aout ./SRC/START.ASM -o ./OBJ/START.O
		@ECHO MAIN.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/MAIN.O ./SRC/MAIN.c
		@ECHO COMMAND.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/COMMAND.O ./SRC/COMMAND.c
		@ECHO LINKING KERNEL.BIN
		LD -T LINK.LD -o B:\KERNEL.BIN ./OBJ/START.O ./OBJ/MAIN.O ./LIB/lib.a ./OBJ/COMMAND.O ./LIB/libMEM.a
	)
	IF /I '%INPUT%'=='3' (
		VFD SAVE
	)
	IF /I '%INPUT%'=='4' (
		BOCHS
	)
	IF /I '%INPUT%'=='5' (
		@ECHO CMOS.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/CMOS.O ./LIBSRC/CMOS.c
		@ECHO DMA.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/DMA.O ./LIBSRC/DMA.c
		@ECHO FDC.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/FDC.O ./LIBSRC/FDC.c
		@ECHO GDT.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/GDT.O ./LIBSRC/GDT.c
		@ECHO IDT.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/IDT.O ./LIBSRC/IDT.c
		@ECHO IRQ.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/IRQ.O ./LIBSRC/IRQ.c
		@ECHO KEYBOARD.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/KEYBOARD.O ./LIBSRC/KEYBOARD.c
		@ECHO MATH.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/MATH.O ./LIBSRC/MATH.c
		@ECHO STDIO.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/STDIO.O ./LIBSRC/STDIO.c
		@ECHO STRING.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/STRING.O ./LIBSRC/STRING.c
		@ECHO TIMER.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/TIMER.O ./LIBSRC/TIMER.c
		@ECHO TSS.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/TSS.O ./LIBSRC/TSS.c
		@ECHO lib.a
		ar rvs ./LIB/lib.a ./OBJ/CMOS.O ./OBJ/GDT.O ./OBJ/IDT.O ./OBJ/IRQ.O ./OBJ/KEYBOARD.O
		ar rvs ./LIB/lib.a ./OBJ/TIMER.O ./OBJ/MATH.O ./OBJ/STDIO.O ./OBJ/STRING.O ./OBJ/DMA.O
		ar rvs ./LIB/lib.a ./OBJ/FDC.O ./OBJ/TSS.O
		@ECHO MEM/PHYSICAL.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/PHYSICAL.O ./LIBSRC/MEM/PHYSICAL.c
		@ECHO MEM/VIRTUAL.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/VIRTUAL.O ./LIBSRC/MEM/VIRTUAL.c
		@ECHO MEM/PAGETABLE.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/PAGETABLE.O ./LIBSRC/MEM/PAGETABLE.c
		@ECHO MEM/PAGEDIR.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/PAGEDIR.O ./LIBSRC/MEM/PAGEDIR.c
		@ECHO MEM/PAGEFAULT.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/PAGEFAULT.O ./LIBSRC/MEM/PAGEFAULT.c
		@ECHO libMEM.a
		ar rvs ./LIB/libMEM.a ./OBJ/PHYSICAL.O ./OBJ/VIRTUAL.O ./OBJ/PAGETABLE.O ./OBJ/PAGEDIR.O ./OBJ/PAGEFAULT.O
		@ECHO START.ASM
		nasm -f aout ./SRC/START.ASM -o ./OBJ/START.O
		@ECHO MAIN.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/MAIN.O ./SRC/MAIN.c
		@ECHO COMMAND.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/COMMAND.O ./SRC/COMMAND.c
		@ECHO LINKING KERNEL.BIN
		LD -T LINK.LD -o B:\KERNEL.BIN ./OBJ/START.O ./OBJ/MAIN.O ./LIB/lib.a ./OBJ/COMMAND.O ./LIB/libMEM.a
		VFD SAVE
		BOCHS
	)
	IF /I '%INPUT%'=='6' (
	    VFD INSTALL
		VFD START
		VFD OPEN ./FLOPPY /RAM /W /144
	)
	IF /I '%INPUT%'=='7' (
	    VFD CLOSE
		VFD STOP
		VFD REMOVE
	)
	IF /I '%INPUT%'=='0' EXIT /B
	PAUSE
	GOTO START
