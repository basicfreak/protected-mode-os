/*
./SRC/COMMAND.C
*/

#include <MATH.H>
#include "COMMAND.H"
#include <STRING.H>
#include <STDIO.H>
#include "../DRIVERSRC/HARDWARE/FDC.H"
#include "../DRIVERSRC/SYSTEM/MEM/PHYSICAL.H"
#include "../DRIVERSRC/SYSTEM/FS/VFS.H"
#include "../DRIVERSRC/SYSTEM/FS/FAT12.H"
#include "../DRIVERSRC/HARDWARE/PCI.H"

void cmd_read (int sec, int num);
void cat(char* path);
void help(void);

char *CurrentPath;
char cmd_command[1024];
char explode_cmd[50][100];

void help()
{
	puts ("MyDOS v. 0.1\n");
	puts ("-------------------------------------\n");
	puts ("cat [PATH]      Reads File off disk\n");
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
	if (streql(explode_cmd[0], "initfat"))
		fsysFatInitialize ();
	else if (streql(explode_cmd[0], "cls"))
		cls();
	else if (streql(explode_cmd[0], "pci"))
		_PCI_init();
	else if (streql(explode_cmd[0], "cat"))
		cat(explode_cmd[1]);
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
	else if (streql(explode_cmd[0], "echo")) {
		for (i = 1; i <= args; i++)
			printf ("%s ", explode_cmd[i]);
		putch('\n');
	}
	else if (streql(explode_cmd[0], "color"))			{
		if( (i = textTOhex(explode_cmd[1])) != 0)
			setColor(i);								}
	else if (streql(explode_cmd[0], "inb")) {
		uint8_t in = inb(textTOhex(explode_cmd[1]));
		printf("We recived %x = %i = %c\n", in, in, in);
	}
	else if (streql(explode_cmd[0], "outb")) {
		outb(textTOhex(explode_cmd[1]), textTOhex(explode_cmd[2]));
	}
	else if (streql(explode_cmd[0], "ramx")) {
		uint64_t start = textTOhex(explode_cmd[1]);
		uint64_t end = textTOhex(explode_cmd[2]);
		for (uint64_t t = start; t <= end; t++)
			printf("%x ", RAM[t]);
		putch('\n');
	}
	else if (streql(explode_cmd[0], "read"))
		cmd_read( charTOint(explode_cmd[1]), charTOint(explode_cmd[2]));
	else if (streql(explode_cmd[0], "dir"))
		dirTest();
	else
		printf("Invalid Command \"%s\"\n", cmd_command);
}

void cmd_handler()
{
	memset(cmd_command, 0, sizeof cmd_command);
	printf("%s",CurrentPath);
	gets(cmd_command, ">");
	arrayAppend(cmd_command, ' ');
	int args = explode (explode_cmd, cmd_command, ' ') - 1;
	do_CMD(args);
}

void init_cmd()
{
	cmd_command[0] = '\0';
	CurrentPath = "A:\\";
	setColor (0x07);
	puts ("MyDOS v. 0.1\n");
	CMD_ACTIVE = TRUE;
}

void cmd_read (int sec, int num)
{
	uint32_t sectornum = sec;
	uint8_t* sector = 0;
	printf ("\n\rSector %i contents:\n\n\r", sectornum);

	//! read sector from disk
	sector = floppy_readSector ( sectornum, (uint8_t) num );

	//! display sector
	if (sector!=0) {

		int i = 0;
		for ( int c = 0; c < num; c++ ) {

			for (int j = 0; j < 512; j++)
				printf ("%x ", sector[ i + j ]);
			i += 512;
		}
	}
	else
		printf ("\n\r*** Error reading sector from disk\n");

	printf ("\n\rDone.\n");
}

void cat(char* path)
{
	//! open file
	FILE file = VFS_OpenFile (path);
	//! test for invalid file
	if (file.flags == FS_INVALID) {
		printf ("\n\rUnable to open file\n");
		return;
	}
	//! cant display directories
	if (( file.flags & FS_DIRECTORY ) == FS_DIRECTORY) {
		printf ("\n\rUnable to display contents of directory.\n");
		return;
	}
	//! top line
	printf ("\n\r-------[%s]-------\n\r", file.name);
	//! display file contents
	while (file.eof != 1) {
		//! read cluster
		unsigned char buf[512];
		VFS_ReadFile ( &file, buf, 512);
		//! display file
		for (int i=0; i<512; i++)
			putch (buf[i]);
		//! wait for input to continue if not EOF
		//if (file.eof != 1) {
			//char gethc = getch ("\n\r------[Press a key to continue]------");
			//if (gethc == 'q') return;
			//printf ("\r"); //clear last line
		//}
	}
	//! done :)
	printf ("\n\n\r--------[EOF]--------\n");
}