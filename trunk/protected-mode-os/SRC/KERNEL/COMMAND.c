/*
./SRC/COMMAND.C
*/

#include <MATH.H>
#include "COMMAND.H"
#include <STRING.H>
#include <STDIO.H>
#include "../HARDWARE/FDC.H"
#include "../SYSTEM/MEM/PHYSICAL.H"
#include "../SYSTEM/API/THREADMAN.H"
#include "../SYSTEM/FS/VFS.H"
#include "../SYSTEM/FS/FAT12.H"
#include "../HARDWARE/PCI.H"
#include "../SYSTEM/CPU/INT.H"

extern void enter_usermode(void);

#define DEBUG

void cmd_read (unsigned int sec, unsigned int num);
void cat(const char* path);
void help(void);
void do_CMD(unsigned int args);
void load(const char* path);

char *CurrentPath;
char cmd_command[1024];
char explode_cmd[50][100];

void help()
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 31) help()\n\r");
#endif
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
	puts ("inb [PORT]      Byte input PORT (HEX)\n");
	puts ("outb [PORT] [=] Byte output PORT = (HEX)\n");
	puts ("read # #        Read Floppy Sector X*X\n");
	puts ("ramx X X        Read Ram X-X\n");
	
}

void do_CMD(unsigned int args)
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 53) do_CMD(0x%x)\n\r", args);
#endif
	unsigned int i = 0;
	if (streql(explode_cmd[0], "initfat"))
		fsysFatInitialize ();
	if (streql(explode_cmd[0], "initfdc"))
		floppy_install ();
	else if (streql(explode_cmd[0], "cls"))
		cls();
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
	else if (streql(explode_cmd[0], "echo")) {
		for (i = 1; i <= args; i++)
			printf ("%s ", explode_cmd[i]);
		putch('\n');
	}
	else if (streql(explode_cmd[0], "color"))			{
		if( (i = textTOhex(explode_cmd[1])) != 0)
			setColor(i);								}
	else if (streql(explode_cmd[0], "inb")) {
		uint8_t in = inb((uint16_t) textTOhex(explode_cmd[1]));
		printf("We recived %x = %i = %c\n", in, in, in);
	}
	else if (streql(explode_cmd[0], "outb")) {
		outb((uint16_t) textTOhex(explode_cmd[1]), (uint8_t) textTOhex(explode_cmd[2]));
	}
	else if (streql(explode_cmd[0], "ramx")) {
		uint64_t start = textTOhex(explode_cmd[1]);
		uint64_t end = textTOhex(explode_cmd[2]);
		for (uint64_t t = start; t <= end; t++)
			printf("%x ", RAM[t]);
		putch('\n');
	}
	else if (streql(explode_cmd[0], "read"))
		cmd_read( textTOhex(explode_cmd[1]), textTOhex(explode_cmd[2]));
	else if (streql(explode_cmd[0], "dir"))
		dirTest();
	else if (streql(explode_cmd[0], "user")) {
		enter_usermode();
		puts("Welcome to User Land!\n");
	}
	else if (streql(explode_cmd[0], "load")) {
		load(explode_cmd[1]);
	}
	else if (streql(explode_cmd[0], "kill"))
		KillThread((uint16_t) textTOhex(explode_cmd[1]));
	else
		printf("Invalid Command \"%s\"\n", cmd_command);
}

void cmd_handler()
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 115) cmd_handler()\n\r");
#endif
	memset(cmd_command, 0, sizeof cmd_command);
	printf("%s",CurrentPath);
	gets(cmd_command, ">");
	arrayAppend(cmd_command, ' ');
	unsigned int args = (unsigned int) (explode (explode_cmd, cmd_command, ' ') - 1);
	do_CMD(args);
}

void init_cmd()
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 128) init_cmd()\n\r");
#endif
	cmd_command[0] = '\0';
	CurrentPath = (char *)"A:\\";
	setColor (0x07);
	puts ("MyDOS v. 0.1\n");
	CMD_ACTIVE = TRUE;
}

void cmd_read (unsigned int sec, unsigned int num)
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 140) cmd_read(0x%x, 0x%x)\n\r", sec, num);
#endif
	uint32_t sectornum = sec;
	uint8_t* sector = 0;
	

	//! read sector from disk
	sector = floppy_readSector ( (int) sectornum, (uint8_t) num );

	//! display sector
	if (sector!=0) {

		unsigned int i = 0;
		for ( unsigned int c = 0; c < num; c++ ) {
			printf ("\n\rSector %i contents:\n\r", (sectornum + (i/0x200)));
			for (unsigned int j = 0; j < 512; j++)
				printf ("%x ", sector[ i + j ]);
			i += 512;
		}

		printf ("\n\rDone.\n");
	}
	else
		printf ("\n\r*** Error reading sector from disk\n");
}

void cat(const char* path)
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 169) cat(%s)\n\r", path);
#endif
	//! open file
	FILE file = VFS_OpenFile (path);
	//! cant display directories
	if (( file.flags & FS_DIRECTORY )){// == FS_DIRECTORY) {
		printf ("\n\rUnable to display contents of directory.\n");
		return;
	}
	//! test for invalid file
	if (file.flags & FS_INVALID) {
		printf ("\n\rUnable to open file\n");
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
			putch ((char)buf[i]);
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

void load(const char* path)
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 207) load(%s)\n\r", path);
#endif
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
	_THREAD_MAN_disable();
	uint16_t task = AddThread(0x800000, 0xFF, true);
	uint8_t *location = (uint8_t*) THREAD[task].physaddr;
	//! top line
	//! display file contents
	unsigned int x = 0;
	while (file.eof != 1) {
		//! read cluster
		unsigned char buf[512];
		VFS_ReadFile ( &file, buf, 512);
		//! display file
		///for (int i=0; i<512; i++)
			memcpy (&location[x*0x200], buf, 0x200);
		x++;
		//! wait for input to continue if not EOF
	}
	THREAD[task].flags = 0x01;
	_THREAD_MAN_enable();
	//! done :)
}
