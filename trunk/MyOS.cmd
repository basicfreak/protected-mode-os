@ECHO OFF
@COLOR 1F
:START
	CLS
	GOTO MENU
	
:MENU
	ECHO -------------------------MY OS MENU-------------------------
	ECHO ------------------------------------------------------------
	ECHO 1.    BUILD LIBRARY
	ECHO 2.    BUILD DRIVERS HARDWARE
	ECHO 3.    BUILD DRIVERS SYSTEM
	ECHO 4.    BUILD KERNEL
	ECHO 5.    SAVE IMAGE
	ECHO 6.    RUN BOCHS
	ECHO 7.    BUILD ALL + SAVE + RUN KERNEL
	ECHO 8.    OPEN IMAGE
	ECHO 9.    CLOSE IMAGE
	ECHO 0.    EXIT
	ECHO ------------------------------------------------------------
	SET INPUT=
	SET /P INPUT=ACTION:
	IF /I '%INPUT%'=='1' (
		@ECHO MATH.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/MATH.O ./LIBSRC/MATH.c
		@ECHO STDIO.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/STDIO.O ./LIBSRC/STDIO.c
		@ECHO STRING.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/STRING.O ./LIBSRC/STRING.c
		@ECHO lib.a
		ar rvs ./LIB/lib.a ./OBJ/STDIO.O ./OBJ/MATH.O ./OBJ/STRING.O
	)
	IF /I '%INPUT%'=='2' (
		@ECHO CMOS.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/CMOS.O ./DRIVERSRC/HARDWARE/CMOS.c
		@ECHO DMA.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/DMA.O ./DRIVERSRC/HARDWARE/DMA.c
		@ECHO FDC.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/FDC.O ./DRIVERSRC/HARDWARE/FDC.c
		@ECHO TIMER.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/TIMER.O ./DRIVERSRC/HARDWARE/TIMER.c
		@ECHO 8042.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/8042.O ./DRIVERSRC/HARDWARE/8042/8042.c
		@ECHO KEYBOARD.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/KEYBOARD.O ./DRIVERSRC/HARDWARE/8042/KEYBOARD.c
		@ECHO MOUSE.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/MOUSE.O ./DRIVERSRC/HARDWARE/8042/MOUSE.c
		@ECHO hw.a
		ar rvs ./DRIVER/hw.a ./OBJ/CMOS.O ./OBJ/DMA.O ./OBJ/FDC.O ./OBJ/TIMER.O ./OBJ/8042.O ./OBJ/KEYBOARD.O
		ar rvs ./DRIVER/hw.a ./OBJ/MOUSE.O
	)
	IF /I '%INPUT%'=='3' (
		@ECHO GDT.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/GDT.O ./DRIVERSRC/SYSTEM/CPU/GDT.c
		@ECHO IDT.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/IDT.O ./DRIVERSRC/SYSTEM/CPU/IDT.c
		@ECHO IRQ.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/IRQ.O ./DRIVERSRC/SYSTEM/CPU/IRQ.c
		@ECHO TSS.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/TSS.O ./DRIVERSRC/SYSTEM/CPU/TSS.c
		@ECHO FAT12.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/FAT12.O ./DRIVERSRC/SYSTEM/FS/FAT12.c
		@ECHO VFS.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/VFS.O ./DRIVERSRC/SYSTEM/FS/VFS.c
		@ECHO PAGEDIR.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/PAGEDIR.O ./DRIVERSRC/SYSTEM/MEM/PAGEDIR.c
		@ECHO PAGEFAULT.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/PAGEFAULT.O ./DRIVERSRC/SYSTEM/MEM/PAGEFAULT.c
		@ECHO PAGETABLE.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/PAGETABLE.O ./DRIVERSRC/SYSTEM/MEM/PAGETABLE.c
		@ECHO PHYSICAL.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/PHYSICAL.O ./DRIVERSRC/SYSTEM/MEM/PHYSICAL.c
		@ECHO VIRTUAL.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/VIRTUAL.O ./DRIVERSRC/SYSTEM/MEM/VIRTUAL.c
		@ECHO sys.a
		ar rvs ./DRIVER/sys.a ./OBJ/GDT.O ./OBJ/IDT.O ./OBJ/IRQ.O ./OBJ/TSS.O ./OBJ/FAT12.O ./OBJ/VFS.O
		ar rvs ./DRIVER/sys.a ./OBJ/PAGEDIR.O ./OBJ/PAGEFAULT.O ./OBJ/PAGETABLE.O ./OBJ/PHYSICAL.O ./OBJ/VIRTUAL.O
	)
	IF /I '%INPUT%'=='4' (
		@ECHO START.ASM
		nasm -f aout ./KERNELSRC/START.ASM -o ./OBJ/START.O
		@ECHO MAIN.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/MAIN.O ./KERNELSRC/MAIN.c
		@ECHO COMMAND.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/COMMAND.O ./KERNELSRC/COMMAND.c
		@ECHO LINKING KERNEL.BIN
		LD -T LINK.LD -o B:\KERNEL.BIN ./OBJ/START.O ./OBJ/MAIN.O ./OBJ/COMMAND.O ./LIB/lib.a ./DRIVER/sys.a ./DRIVER/hw.a
	)
	IF /I '%INPUT%'=='5' (
		VFD SAVE
	)
	IF /I '%INPUT%'=='6' (
		BOCHS
	)
	IF /I '%INPUT%'=='7' (
		@ECHO MATH.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/MATH.O ./LIBSRC/MATH.c
		@ECHO STDIO.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/STDIO.O ./LIBSRC/STDIO.c
		@ECHO STRING.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/STRING.O ./LIBSRC/STRING.c
		@ECHO lib.a
		ar rvs ./LIB/lib.a ./OBJ/STDIO.O ./OBJ/MATH.O ./OBJ/STRING.O
		@ECHO CMOS.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/CMOS.O ./DRIVERSRC/HARDWARE/CMOS.c
		@ECHO DMA.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/DMA.O ./DRIVERSRC/HARDWARE/DMA.c
		@ECHO FDC.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/FDC.O ./DRIVERSRC/HARDWARE/FDC.c
		@ECHO TIMER.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/TIMER.O ./DRIVERSRC/HARDWARE/TIMER.c
		@ECHO 8042.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/8042.O ./DRIVERSRC/HARDWARE/8042/8042.c
		@ECHO KEYBOARD.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/KEYBOARD.O ./DRIVERSRC/HARDWARE/8042/KEYBOARD.c
		@ECHO MOUSE.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/MOUSE.O ./DRIVERSRC/HARDWARE/8042/MOUSE.c
		@ECHO hw.a
		ar rvs ./DRIVER/hw.a ./OBJ/CMOS.O ./OBJ/DMA.O ./OBJ/FDC.O ./OBJ/TIMER.O ./OBJ/8042.O ./OBJ/KEYBOARD.O
		ar rvs ./DRIVER/hw.a ./OBJ/MOUSE.O
		@ECHO GDT.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/GDT.O ./DRIVERSRC/SYSTEM/CPU/GDT.c
		@ECHO IDT.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/IDT.O ./DRIVERSRC/SYSTEM/CPU/IDT.c
		@ECHO IRQ.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/IRQ.O ./DRIVERSRC/SYSTEM/CPU/IRQ.c
		@ECHO TSS.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/TSS.O ./DRIVERSRC/SYSTEM/CPU/TSS.c
		@ECHO FAT12.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/FAT12.O ./DRIVERSRC/SYSTEM/FS/FAT12.c
		@ECHO VFS.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/VFS.O ./DRIVERSRC/SYSTEM/FS/VFS.c
		@ECHO PAGEDIR.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/PAGEDIR.O ./DRIVERSRC/SYSTEM/MEM/PAGEDIR.c
		@ECHO PAGEFAULT.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/PAGEFAULT.O ./DRIVERSRC/SYSTEM/MEM/PAGEFAULT.c
		@ECHO PAGETABLE.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/PAGETABLE.O ./DRIVERSRC/SYSTEM/MEM/PAGETABLE.c
		@ECHO PHYSICAL.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/PHYSICAL.O ./DRIVERSRC/SYSTEM/MEM/PHYSICAL.c
		@ECHO VIRTUAL.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/VIRTUAL.O ./DRIVERSRC/SYSTEM/MEM/VIRTUAL.c
		@ECHO sys.a
		ar rvs ./DRIVER/sys.a ./OBJ/GDT.O ./OBJ/IDT.O ./OBJ/IRQ.O ./OBJ/TSS.O ./OBJ/FAT12.O ./OBJ/VFS.O
		ar rvs ./DRIVER/sys.a ./OBJ/PAGEDIR.O ./OBJ/PAGEFAULT.O ./OBJ/PAGETABLE.O ./OBJ/PHYSICAL.O ./OBJ/VIRTUAL.O
		@ECHO START.ASM
		nasm -f aout ./KERNELSRC/START.ASM -o ./OBJ/START.O
		@ECHO MAIN.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/MAIN.O ./KERNELSRC/MAIN.c
		@ECHO COMMAND.C
		gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/COMMAND.O ./KERNELSRC/COMMAND.c
		@ECHO LINKING KERNEL.BIN
		LD -T LINK.LD -o B:\KERNEL.BIN ./OBJ/START.O ./OBJ/MAIN.O ./OBJ/COMMAND.O ./LIB/lib.a ./DRIVER/sys.a ./DRIVER/hw.a
		VFD SAVE
		BOCHS
	)
	IF /I '%INPUT%'=='8' (
	    VFD INSTALL
		VFD START
		VFD OPEN ./FLOPPY /RAM /W /144
	)
	IF /I '%INPUT%'=='9' (
	    VFD CLOSE
		VFD STOP
		VFD REMOVE
	)
	IF /I '%INPUT%'=='0' EXIT /B
	PAUSE
	GOTO START
