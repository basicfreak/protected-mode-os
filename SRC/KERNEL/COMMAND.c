/*
./SRC/COMMAND.C
*/

#include <MATH.H>
#include "COMMAND.H"
#include <STRING.H>
#include <STDIO.H>
#include "../HARDWARE/FDC.H"
#include "../HARDWARE/IDE.H"
#include "../HARDWARE/TIMER.H"
#include "../SYSTEM/API/THREADMAN.H"
#include "../SYSTEM/FS/VFS.H"
#include "../SYSTEM/FS/FAT12.H"
#include "../HARDWARE/PCI.H"
#include "../SYSTEM/CPU/INT.H"

extern void enter_usermode(void);

#define DEBUG

void cmd_read (unsigned int sec, unsigned int num);
void cmd_readIDE (uint8_t disk, uint64_t sec, uint8_t num);
void cat(const char* path);
void help(void);
void do_CMD(unsigned int args);
void load(const char* path);
void CopyFloppyTest(void);

char *CurrentPath;
//char cmd_command[1024];
char *cmd_command;
char explode_cmd[50][100];
const char *DataToWrite = "             ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 1234567890                ";
uint8_t *readbuffer;
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
	/*if (streql(explode_cmd[0], "initfat"))
		fsysFatInitialize ();
	if (streql(explode_cmd[0], "initfdc"))
		_FDC_init();*/
	if (streql(explode_cmd[0], "cls"))
		cls();
	if (streql(explode_cmd[0], "reboot"))
		outb(0x64, 0xFE);
	else if (streql(explode_cmd[0], "write"))
		_FDC_IO(true, (int)textTOhex(explode_cmd[1]), 1, (uint8_t*) DataToWrite);
	else if (streql(explode_cmd[0], "cat"))
		cat(explode_cmd[1]);
	else if (streql(explode_cmd[0], "clone"))
		CopyFloppyTest();
	else if (streql(explode_cmd[0], "exit"))
		CMD_ACTIVE = FALSE;
	else if (streql(explode_cmd[0], "help"))
		help();
	else if (streql(explode_cmd[0], "ram")) {
		printf("Total RAM:    %iKB\n", _mmngr_memory_size);
		printf("Free RAM:     %iKB\n", _mmngr_memory_size-(_mmngr_used_blocks*4));
		printf("Used RAM:     %iKB\n", _mmngr_used_blocks*4);
		txf(1, "Total RAM:    %iKB\n\r", _mmngr_memory_size);
		txf(1, "Free RAM:     %iKB\n\r", _mmngr_memory_size-(_mmngr_used_blocks*4));
		txf(1, "Used RAM:     %iKB\n\r", _mmngr_used_blocks*4);
	}
	else if (streql(explode_cmd[0], "echo")) {
		for (i = 1; i <= args; i++) {
			printf ("%s ", explode_cmd[i]);
			txf(1, "%s ", explode_cmd[i]);
		}
		txf(1, "\n\r");
		putch('\n');
	}
	else if (streql(explode_cmd[0], "color"))			{
		if( (i = textTOhex(explode_cmd[1])) != 0)
			setColor(i);								}
	else if (streql(explode_cmd[0], "inb")) {
		uint8_t in = inb((uint16_t) textTOhex(explode_cmd[1]));
		printf("We recived %x = %i = %c\n", in, in, in);
		txf(1, "We recived %x = %i = %c\n\r", in, in, in);
	}
	else if (streql(explode_cmd[0], "outb")) {
		outb((uint16_t) textTOhex(explode_cmd[1]), (uint8_t) textTOhex(explode_cmd[2]));
	}
	else if (streql(explode_cmd[0], "inw")) {
		uint16_t in = inw((uint16_t) textTOhex(explode_cmd[1]));
		printf("We recived %x = %i = %c\n", in, in, in);
		txf(1, "We recived %x = %i = %c\n\r", in, in, in);
	}
	else if (streql(explode_cmd[0], "outw")) {
		outw((uint16_t) textTOhex(explode_cmd[1]), (uint16_t) textTOhex(explode_cmd[2]));
	}
	else if (streql(explode_cmd[0], "inl")) {
		uint32_t in = inl((uint16_t) textTOhex(explode_cmd[1]));
		printf("We recived %x = %i = %c\n", in, in, in);
		txf(1, "We recived %x = %i = %c\n\r", in, in, in);
	}
	else if (streql(explode_cmd[0], "outl")) {
		outl((uint16_t) textTOhex(explode_cmd[1]), (uint32_t) textTOhex(explode_cmd[2]));
	}
	else if (streql(explode_cmd[0], "ramx")) {
		uint64_t start = textTOhex(explode_cmd[1]);
		uint64_t end = textTOhex(explode_cmd[2]);
		for (uint64_t t = start; t <= end; t++) {
			printf("%x ", RAM[t]);
			txf(1, "%x ", RAM[t]);
		}
		txf(1, "\n\r");
		putch('\n');
	}
	else if (streql(explode_cmd[0], "read"))
		cmd_read( textTOhex(explode_cmd[1]), textTOhex(explode_cmd[2]));
	else if (streql(explode_cmd[0], "readide"))
		cmd_readIDE( (uint8_t) textTOhex(explode_cmd[1]), (uint64_t) textTOhex(explode_cmd[2]), (uint8_t) textTOhex(explode_cmd[3]));
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
	/*else if (streql(explode_cmd[0], "threadenable")) {
		_THREAD_MAN_enable();
		for(;;)
			putch('.');
	}*/
	else
		printf("Invalid Command \"%s\"\n", cmd_command);
}

void cmd_handler()
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 115) cmd_handler()\n\r");
#endif
txf(1, "a");
	memset(cmd_command, 0, sizeof cmd_command);
txf(1, "b");
	printf("%s",CurrentPath);
txf(1, "c");
	gets(cmd_command, ">");
txf(1, "k");
	arrayAppend(cmd_command, ' ');
txf(1, "l");
	unsigned int args = (unsigned int) (explode (explode_cmd, cmd_command, ' ') - 1);
txf(1, "m\n\r");
	do_CMD(args);
}

void init_cmd()
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 128) init_cmd()\n\r");
#endif
	readbuffer = calloc(4);
	cmd_command = calloc(1);
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
	int sectornum = (int) sec;
	memset(readbuffer, 0, 0x4000);
	//! read sector from disk
	error errorcode;
	if((errorcode = _FDC_IO(false, sectornum, (uint8_t) num, readbuffer)))
		printf("ERROR CODE = 0x%x\n", errorcode);

	//! display sector
	if (readbuffer && !errorcode) {

		unsigned int i = 0;
		for ( unsigned int c = 0; c < num; c++ ) {
			printf ("\n\rSector %i contents:\n\r", (sectornum + (int) (i/0x200)));
			for (unsigned int j = 0; j < 0x200; j++)
				printf ("%x ", readbuffer[ i + j ]);
			i += 0x200;
		}

		printf ("\n\rDone.\n");
	}
	else
		printf ("\n\r*** Error reading sector from disk\n");
}

void cmd_readIDE (uint8_t disk, uint64_t sec, uint8_t num)
{
#ifdef DEBUG
	txf(1, "(COMMAND.C:Line 219) cmd_readIDE(0x%x, 0x%x)\n\r", sec, num);
#endif
	int sectornum = (int) sec;
	memset(readbuffer, 0, 0x4000);
	//! read sector from disk
	error errorcode;
	if((errorcode = _IDE_IO(disk, false, sec, num, readbuffer)))
		printf("ERROR CODE = 0x%x\n", errorcode);

	//! display sector
	if (readbuffer && !errorcode) {

		unsigned int i = 0;
		for ( unsigned int c = 0; c < num; c++ ) {
			printf ("\n\rSector %i contents:\n\r", (sectornum + (int) (i/0x200)));
			for (unsigned int j = 0; j < 0x200; j++)
				printf ("%x ", readbuffer[ i + j ]);
			i += 0x200;
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
	//! load file
	FILE file = VFS_OpenFile (path);
	//! test for invalid file
	if (file.flags == FS_INVALID) {
		printf ("\n\rUnable to open file\n");
		return;
	}
	//! cant load directories
	if (( file.flags & FS_DIRECTORY ) == FS_DIRECTORY) {
		printf ("\n\rUnable to display contents of directory.\n");
		return;
	}
	_THREAD_MAN_disable();
	uint16_t task = AddThread(0x800000, 0xFF, true);
	uint8_t *location = (uint8_t*) THREAD[task].physaddr;
	unsigned int x = 0;
	while (file.eof != 1) {
		//! read cluster
		unsigned char buf[512];
		VFS_ReadFile ( &file, buf, 512);
			memcpy (&location[x*0x200], buf, 0x200);
		x++;
	}
	THREAD[task].flags = 0x01;
	_THREAD_MAN_enable();
	//! done :)
	//for(uint32_t i = 0; i < 0xAAAAAAAA;i++) printf("%x\n", i);
}

void CopyFloppyTest()
{
	// I only want to test read / write seppeds so lets allocate memory now...
	uint8_t *FloppyTestBuffer = calloc(360);		//1=4KB so... 360=1.44M Be the biggist alloc test yet ;)
	uint32_t startTick, endTick;
	error errorcode = ERROR_NONE;
	//Sector By Sector Read
	startTick = GetTimerTicks();
	/*for(int i=0; i<80; i++)
		if((errorcode=_FDC_IO(false, (i * 36), 36, &FloppyTestBuffer[i*0x4800])))
			if((errorcode=_FDC_IO(false, (i * 36), 36, &FloppyTestBuffer[i*0x4800])))
				printf("FDC IO TRACK: 0x%x\tError: 0x%x\n", i, errorcode);*/


for(int i=0; i<12; i++)
		if((errorcode=_FDC_IO(false, (i * 240), 240, &FloppyTestBuffer[i*0x1E000])))
			if((errorcode=_FDC_IO(false, (i * 240), 240, &FloppyTestBuffer[i*0x1E000])))
				printf("FDC IO TRACK: 0x%x\tError: 0x%x\n", i, errorcode);


	endTick = GetTimerTicks();
	printf("Floppy TRACK Read Time = %i mS\n", (10 * (endTick - startTick)));
	(void) getch("Press Any Key To Write");
	putch('\n');
	//Sector By Sector Write
	startTick = GetTimerTicks();
	for(int i=0; i<80; i++)
		if((errorcode=_FDC_IO(true, (i * 36), 36, &FloppyTestBuffer[i*0x4800])))
			if((errorcode=_FDC_IO(true, (i * 36), 36, &FloppyTestBuffer[i*0x4800])))
				printf("FDC IO TRACK: 0x%x\tError: 0x%x\n", i, errorcode);
	endTick = GetTimerTicks();
	printf("Floppy TRACK Write Time = %i mS\n", (10 * (endTick - startTick)));
}
