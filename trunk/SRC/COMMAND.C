/*
./SRC/COMMAND.C
*/

#include "COMMAND.H"
#include <STDIO.H>
#include <STRING.H>
#include <MATH.H>
#include <FDC.H>
#include <MEM/PHYSICAL.H>

void cmd_read (int sec, int num);
void help(void);

char cmd_command[1024];
char explode_cmd[50][100];

void help()
{
	puts ("MyDOS v. 0.1\n");
	puts ("-------------------------------------\n");
	puts ("cls             Clears Screen\n");
	puts ("color [XX]      Sets color [XX] = hex\n");
	puts ("echo [text]     Echos TEXT\n");
	puts ("help            Displays Help\n");
	puts ("ram             Outputs RAM info\n");
	puts ("exit            Exits CLI\n");
	puts ("------------DEBUG LIST---------------\n");
	puts ("div0            Divide by 0\n");
	puts ("inb [PORT]      Byte input PORT (HEX)\n");
	puts ("outb [PORT] [=] Byte output PORT = (HEX)\n");
	puts ("read # #        Read Floppy Sector #-#\n");
	puts ("ramx X X        Read Ram X-X\n");
	
}

void do_CMD(int args)
{
	int i = 0;
	if (streql(explode_cmd[0], "cls"))
		cls();
	else if (streql(explode_cmd[0], "exit"))
		CMD_ACTIVE = FALSE;
	else if (streql(explode_cmd[0], "help"))
		help();
	else if (streql(explode_cmd[0], "ram")) {
		printf("Total RAM:    %iKB\n", _mmngr_memory_size);
		printf("Free RAM:     %iKB\n", _mmngr_memory_size-(_mmngr_used_blocks*4));
		printf("Used RAM:     %iKB\n", _mmngr_used_blocks*4);
	}
	else if (streql(explode_cmd[0], "div0"))
		{ int divz = 1/0; }
	else if (streql(explode_cmd[0], "echo"))
		for (i = 1; i <= args; i++)
			printf ("%s ", explode_cmd[i]);
	else if (streql(explode_cmd[0], "color"))			{
		if( (i = textTOhex(explode_cmd[1])) != 0)
			setColor(i);								}
	else if (streql(explode_cmd[0], "inb")) {
		uint8_t in = inb(textTOhex(explode_cmd[1]));
		printf("We recived %x = %i = %c", in, in, in);
	}
	else if (streql(explode_cmd[0], "outb")) {
		outb(textTOhex(explode_cmd[1]), textTOhex(explode_cmd[2]));
	}
	else if (streql(explode_cmd[0], "ramx")) {
		uint64_t start = textTOhex(explode_cmd[1]);
		uint64_t end = textTOhex(explode_cmd[2]);
		for (uint64_t t = start; t <= end; t++)
			printf("%x ", RAM[t]);
	}
	else if (streql(explode_cmd[0], "read"))
		cmd_read( charTOint(explode_cmd[1]), charTOint(explode_cmd[2]));
	else
		printf("Invalid Command \"%s\"", cmd_command);
}

void cmd_handler()
{
	memset(cmd_command, 0, sizeof cmd_command);
	gets(cmd_command, "\n>");
	arrayAppend(cmd_command, ' ');
	int args = explode (explode_cmd, cmd_command, ' ') - 1;
	do_CMD(args);
}

void init_cmd()
{
	cmd_command[0] = '\0';
	setColor (0x07);
	CMD_ACTIVE = TRUE;
}

void cmd_read (int sec, int num)
{
	uint32_t sectornum = sec;
	uint8_t* sector = 0;
	printf ("\n\rSector %i contents:\n\n\r", sectornum);

	//! read sector from disk
	sector = floppy_readSector ( sectornum, 0x1000, (uint8_t) num );

	//! display sector
	if (sector!=0) {

		int i = 0;
		for ( int c = 0; c < 4*num; c++ ) {

			for (int j = 0; j < 128; j++)
				printf ("%x ", sector[ i + j ]);
			i += 128;
		}
	}
	else
		printf ("\n\r*** Error reading sector from disk");

	printf ("\n\rDone.");
}