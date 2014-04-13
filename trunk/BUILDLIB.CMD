@ECHO OFF
@COLOR 1F
@CLS
REM ./BUILDLIB.CMD
@ECHO CMOS.C
gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/CMOS.O ./LIBSRC/CMOS.c
@ECHO GDT.C
gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/GDT.O ./LIBSRC/GDT.c
@ECHO IDT.C
gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/IDT.O ./LIBSRC/IDT.c
@ECHO IRQ.C
gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/IRQ.O ./LIBSRC/IRQ.c
@ECHO MATH.C
gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/MATH.O ./LIBSRC/MATH.c
@ECHO STDIO.C
gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/STDIO.O ./LIBSRC/STDIO.c
@ECHO STRING.C
gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/STRING.O ./LIBSRC/STRING.c
@ECHO TIMER.C
gcc -Wall -std=gnu99 -nostdinc -I./include -c -o ./OBJ/TIMER.O ./LIBSRC/TIMER.c
@ECHO libmyos.a
ar rvs ./LIB/libmyos.a ./OBJ/CMOS.O ./OBJ/GDT.O ./OBJ/IDT.O ./OBJ/IRQ.O
ar rvs ./LIB/libmyos.a ./OBJ/TIMER.O ./OBJ/MATH.O ./OBJ/STDIO.O ./OBJ/STRING.O