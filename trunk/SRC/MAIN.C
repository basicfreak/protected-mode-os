/*
./SRC/MAIN.C
*/

#include <GDT.H>
#include <IDT.H>
#include <IRQ.H>
#include <STRING.H>
#include <STDIO.H>
#include <MATH.H>
#include <TIMER.H>
#include <CMOS.H>
#include <KEYBOARD.H>
#include <FDC.H>
#include <MEM/PHYSICAL.H>
#include "COMMAND.H"

int main()
{
	gdt_install		();
	idt_install		();
    isrs_install	();
    irq_install		();
	initVideo		();
	readCMOS		();
	install_keyboard();
    timer_install	();
	__asm__ __volatile__ ("sti");					//DON'T FROGET TO RE-ENABLE INTS OR NOTHING WILL WORK RIGHT!!!!
	floppy_install	();
	
	setColor		(0x5F);
	cls				();
	setColor		(0x4F);
	puts			("Hello World\nKernel Test\nWill the first letter on this line be a Z\?\rZ");
	setColor		(0x1F);
	puts			("\f\'\\\"\nThe last letter on this line should be an X, right\bX\?\ntab test\t...\v...\n");
	unsigned int divtest = 85 / 1024;
	printf ("\n\t\t85/1024=%i", divtest);
	divtest = 2000 / 1024;
	printf ("\t\t2000/1024=%i\n", divtest);
	initPHYSMEM();
/*initMEM();
BPB_init();
FAT_init();	
ROOT_init();*/
	printf ("textTOhex() test: 07 = %i && CF = %i", textTOhex("07"), textTOhex("CF"));
	printf ("\nAnd for S&G 1c5A8d9 = %i & 29731033 = %s", textTOhex("1c5A8d9"), hexTOtext(textTOhex("1c5A8d9")));
	
	printf ("\nCMOS Read Test:\nMaster Floppy = %i = %s\nSlave Floppy = %i = %s\n", CMOS_Floppy_Master, CMOS_Floppy_Decode(CMOS_Floppy_Master), CMOS_Floppy_Slave, CMOS_Floppy_Decode(CMOS_Floppy_Slave));
	printf ("\'a\' => \'%c\' => \'%c\'\n", ChartoUpper( 'a' ), ChartoLower( ChartoUpper('a') ) );
	printf ("\'A\' => \'%c\' => \'%c\'\n", ChartoUpper( 'A' ), ChartoLower( ChartoUpper('A') ) );
	printf ("\"p\" => \"%s\" => \"%s\"\n", StringtoUpper( "p" ), StringtoLower( StringtoUpper("p") ) );
	printf ("\"pUmPkIn\" => \"%s\" => \"%s\"\n", StringtoUpper( "pUmPkIn" ), StringtoLower( StringtoUpper("pUmPkIn") ) );
	printf ("BIN to INT test: \"P@ \" = %i (2113616)\n", BIN2INT((unsigned char*)"P@ "));
	printf ("1 << 1 = 0x%x", (1<<1));
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
	sleep(60);				//wait 60 seconds and display EASTER EGG
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
	for(;;);
	return 0;
}