/*
./SRC/MAIN.C
*/

#include "../DRIVERSRC/SYSTEM/CPU/GDT.H"
#include "../DRIVERSRC/SYSTEM/CPU/IDT.H"
#include "../DRIVERSRC/SYSTEM/CPU/IRQ.H"
#include "../DRIVERSRC/SYSTEM/MEM/VIRTUAL.H"
#include <STDIO.H>
#include "../DRIVERSRC/HARDWARE/TIMER.H"
#include "../DRIVERSRC/HARDWARE/CMOS.H"
#include "../DRIVERSRC/HARDWARE/8042/KEYBOARD.H"
#include "../DRIVERSRC/HARDWARE/8042/MOUSE.H"
#include "../DRIVERSRC/HARDWARE/FDC.H"
#include "../DRIVERSRC/SYSTEM/MEM/PHYSICAL.H"
#include "../DRIVERSRC/SYSTEM/MEM/PAGEFAULT.H"
#include "COMMAND.H"
#include "../DRIVERSRC/SYSTEM/FS/FAT12.H"

int main()
{
	gdt_install		();
	idt_install		();
    isrs_install	();
    irq_install		();
	initVideo		();
	readCMOS		();
	install_keyboard();
	init_PS2		();
    timer_install	();
	initPHYSMEM();
	init_pageFault();
	vmmngr_initialize ();
	__asm__ __volatile__ ("sti");					//DON'T FROGET TO RE-ENABLE INTS OR NOTHING WILL WORK RIGHT!!!!
	floppy_install	();
	//floppy_readSector(0,1);
	//fsysFatInitialize ();
	
	//printf ("textTOhex() test: 07 = %i && CF = %i", textTOhex("07"), textTOhex("CF"));
	
	init_cmd ();
	while(CMD_ACTIVE)
		cmd_handler();
	movcur(ROWS - 1, COLS - 1);
	movcur_disable ++;
	setColor		(0x0F);
	cls();
	__asm__ __volatile__ ("cli");
	puts("So long, farewell\nAuf Wiedersehen, goodnight\n\n");
	puts("I hate to go and leave this pretty sight\n\nSo long, farewell\nAuf Wiedersehen, adieu\n\n");
	puts("Adieu, adieu\nTo you and you and you\n\n...\n\n");
	puts("The sun has gone\nTo bed and so must I\n\n");
	puts("So long, farewell\nAuf Wiedersehen, goodbye\n\n");
	puts("Goodbye\nGoodbye\nGoodbye\n\nGoodbye");
	setColor		(0x0E);
	curX = (COLS / 2) - 17;
	curY = ((ROWS - 1) / 2) - 1;
	puts("        So sad to see you go");
	curX = (COLS / 2) - 17;
	curY = ((ROWS + 1) / 2) - 1;
	puts("It is safe to turn off your computer");
	curX = (COLS / 2) - 17;
	curY = ROWS - 1;
	puts("             GOOD BYE!");
	//__asm__ __volatile__ ("sti");
	__asm__ __volatile__ ("hlt");
/*	sleep(60);				//wait 60 seconds and display EASTER EGG
	__asm__ __volatile__ ("cli");
	setColor (0x1F);
	cls();
	puts("So long and thanks for all the fish\n\
So sad that it should come to this\nWe tried to warn you all but oh dear?\n\n\
You may not share our intellect\nWhich might explain your disrespect\nFor all the natural wonders that grow around you\n\n\
So long, so long and thanks\nfor all the fish\n\n\
The world's about to be destroyed\nThere's no point getting all annoyed\nLie back and let the planet dissolve\n\n\
Despite those nets of tuna fleets\nWe thought that most of you were sweet\nEspecially tiny tots and your pregnant women\n\n\
So long, so long, so long, so long, so long\nSo long, so long, so long, so long, so long\n\n\
So long, so long and thanks\nfor all the fish\n");
	__asm__ __volatile__ ("hlt");
*/
	for(;;);
	return 0;
}