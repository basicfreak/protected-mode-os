/*
./LIBSRC/FDC.C
*/

#include "FDC.H"
#include <STDIO.H>
#include "CMOS.H"
#include "DMA.H"
#include "../SYSTEM/CPU/IRQ.H"
#include "TIMER.H"

#define debug 0

int tries;
uint16_t _currentCylinder;

void floppy_dma_read ( int addr );
void floppy_dma_write (void);
uint8_t FIFO_read(void);
bool FIFO_write(uint8_t cmd);
void DOR_write(uint8_t cmd);
bool floppy_waitIRQ(void);		
bool FIFO_ready(void);
void floppy_handler(regs *r);
void floppy_IRQ_handle(uint32_t* st0, uint32_t* cyl);
bool floppy_changed(void);
void floppy_speed(int speed); //0=500KB\s 1=300KB\s 2=250KB\s 3=1MB\s
void floppy_current(int driveNum);
void floppy_setVars(int i);
void floppy_mechanical_info (uint8_t stepr, uint8_t loadt, uint8_t unloadt, bool dma );
void floppy_readSector_imp (uint8_t head, uint8_t track, uint8_t sector, uint8_t secotrs);
bool floppy_seek ( uint8_t cyl, uint8_t head );
volatile bool _FloppyDiskIRQ;
int floppy_calibrate(void);
void fdc_quickReset(void);
void fdc_enable(void);
void fdc_disable(void);
uint8_t floppy_status(void);

int DMA_BUFFER = 0x1000;
const int DMA_CHANNEL = 2;

int FLOPPY_MOTOR_LIST[4] = {16, 32, 64, 128};

void floppy_set_dma (int addr)
{

	DMA_BUFFER = addr;
}

//initialize DMA to use phys addr 1k-64k
bool floppy_init_dma(uint8_t* buffer, unsigned length)
{
if (debug) printf ("floppy_init_dma(%x, %x)\n", buffer, length);
	union {
      uint8_t byte[4];//Lo[0], Mid[1], Hi[2]
      unsigned long l;
	}a, c;
   a.l=(unsigned)buffer;
   c.l=(unsigned)length-1;
   //Check for buffer issues
   if ((a.l >> 24) || (c.l >> 16) || (((a.l & 0xffff)+c.l) >> 16)) {
		if (debug) puts("BUFFER ERROR\n");
		return false;
   }
   dma_reset (1);
   dma_mask_channel( DMA_CHANNEL );//Mask channel 2
   dma_reset_flipflop ( 1 );//Flipflop reset on DMA 1
   dma_set_address( DMA_CHANNEL, a.byte[0],a.byte[1]);//Buffer address
   dma_reset_flipflop( 1 );//Flipflop reset on DMA 1
   dma_set_count( DMA_CHANNEL, c.byte[0],c.byte[1]);//Set count
   dma_set_read ( DMA_CHANNEL );
   dma_unmask_all( 1 );//Unmask channel 2
   return true;
}
 
//prepare the DMA for read transfer
void floppy_dma_read ( int addr )
{
	dma_set_read ( DMA_CHANNEL );
}
 
//prepare the DMA for write transfer
void floppy_dma_write ()
{
	dma_set_write ( DMA_CHANNEL );
}

uint8_t FIFO_read()
{
	for (int i = 0; i < 750; i++ ) {
		if ( FIFO_ready () ) {
///			if (debug) puts("FIFO Read\n");
			return inb (FIFO);
		}// else timer_wait(1);
	}
	if (debug) puts ("FIFO FAILED TO READ Continue...\n");
	return 0;
}
bool FIFO_write(uint8_t cmd)
{
	for (int i = 0; i < 750; i++ ) {
		if ( FIFO_ready () ) {
///			if (debug) puts("FIFO Wrote\n");
			outb (FIFO, cmd);
			return 1;
		}// else timer_wait(1);
	}
	if (debug) puts ("FIFO FAILED TO WRITE Continue...\n");
	return 0;
}
void DOR_write(uint8_t cmd)
{
	outb(DOR, cmd);
}

bool floppy_waitIRQ()
{
	if (debug) puts("Waiting for IRQ to fire...");
	//while (!_FloppyDiskIRQ);
	int timeout = 10;
	while ( !_FloppyDiskIRQ && timeout) {
		timer_wait(6);
		timeout--;
	}
	_FloppyDiskIRQ = FALSE;
	if (!timeout) {
		return FALSE;
		if (debug) puts ("\tNO INT\n");
	}
	else return TRUE;
}

void floppy_reset()		
{
	uint32_t st0, cyl;
	//reset the controller
	fdc_disable ();
	fdc_enable ();
	
	if(floppy_waitIRQ ()) {
	// send CHECK_INT/SENSE INTERRUPT command to all drives
		for (int i=0; i<4; i++) {
			if (debug) puts("IRQ Handle\t");
			floppy_IRQ_handle (&st0,&cyl);
		}
	}
	else
	{
		if (debug) puts("IRQ not revived\n");
		floppy_reset();
	}
///	if (debug) puts("fdc send CONFIGURE to FIFO\n");
	FIFO_write (CONFIGURE);
	//outb (FIFO, CONFIGURE);
	// transfer speed 500kb/s
///	if (debug) puts("floppy_speed()\n");
	floppy_speed (curFloppy_speed);
	// pass mechanical drive info. steprate=3ms, unload time=240ms, load time=16ms
///	if (debug) puts("fdc send Mechanical info\n");
	floppy_mechanical_info (3,16,240,true);
	// calibrate the disk
///	if (debug) puts("waitIRQ\n");
	
///	if (debug) puts("calibrate:");
	floppy_calibrate ();
	FIFO_write (LOCK);
	return;
}
bool FIFO_ready()
{
	bool ret = FALSE;
	//int temp = floppy_status();
	//if ( (temp & 0xc0) == 0x80 || (temp & 0xc0) == 0x81) ret = TRUE;
	if (floppy_status () & 0x80) ret = TRUE;
///	if (debug && ret) puts("FIFO Ready\n");
	//if (debug && !ret) puts("FIFO NOT Ready\n");
	return ret;
}
uint8_t floppy_status()
{
	return inb(MSR);
}
void floppy_handler(regs *r)
{ 
	
	//! irq fired
	_FloppyDiskIRQ = TRUE;
	if(debug) puts("\tIRQ Revived\t");
}
void floppy_IRQ_handle(uint32_t* st0, uint32_t* cyl)
{
	if (debug) puts ("floppy_IRQ_handle(st0, cyl)\t");
	for (int i = 0; i < 75; i++) {
		if (FIFO_ready()) {
			FIFO_write(SENSE_INTERRUPT);
			//outb(FIFO, SENSE_INTERRUPT);
			timer_wait(6);
			*st0 = FIFO_read();
			//*st0 = inb (FIFO);
			timer_wait(6);
			*cyl = FIFO_read();
			//*cyl = inb (FIFO);
			//while (!FIFO_ready()) FIFO_read();
			if (debug) puts("IRQ HANDLED\n");
			return;
		} else timer_wait(6);
	}
}
bool floppy_changed()
{
	bool ret = FALSE;
	if (inb(DIR) >= 0x80) ret = TRUE;
	return ret;
}
void floppy_motor(bool on)
{
	if (debug) printf ("Motor = %i\n", on);
	if(on)
		DOR_write (0x0C | floppy_drive_number  | FLOPPY_MOTOR_LIST[floppy_drive_number]);
	else
		DOR_write (0x0C | floppy_drive_number);
	timer_wait(20);
}
void floppy_speed(int speed) //0=500KB\s 1=300KB\s 2=250KB\s 3=1MB\s
{
	if ( 3 <= speed && speed >= 0 ) {
		outb(CCR, speed);
		//outb(DSR, speed);
	}
}
void floppy_current(int driveNum)
{
	int temp = 0;
	if ( 1 <= driveNum && driveNum >= 0 )
		floppy_drive_number = driveNum;
	if(driveNum == 0) temp = CMOS_Floppy_Master;
	else temp = CMOS_Floppy_Slave;
	floppy_setVars(temp);
	
}
void fdc_quickReset()
{
	uint32_t st0, cyl0;
	fdc_disable();
	fdc_enable();
	if(floppy_waitIRQ ()) {
		for (int i=0; i<4; i++)
			floppy_IRQ_handle (&st0,&cyl0);
	}
	floppy_calibrate();
}
void fdc_enable()
{
	if (debug) puts("fdc_enable()\n");
	DOR_write (0x0C | floppy_drive_number);
}
void fdc_disable()
{
	if (debug) puts("fdc_disable()\n");
	DOR_write (0);
}
void floppy_setVars(int i)
{
	switch (i)
	{
		case	1:
			curFloppy_sectors = Sectors_Floppy_360;
			curFloppy_tracks = Heads_Floppy_360;
			curFloppy_cylinders = Cylinders_Floppy_360;
			curFloppy_speed = Datarate_Floppy_360;
			curFloppy_type = 1;
			break;
		case	2:
			curFloppy_sectors = Sectors_Floppy_12;
			curFloppy_tracks = Heads_Floppy_12;
			curFloppy_cylinders = Cylinders_Floppy_12;
			curFloppy_speed = Datarate_Floppy_12;
			curFloppy_type = 2;
			break;
		case	3:
			curFloppy_sectors = Sectors_Floppy_720;
			curFloppy_tracks = Heads_Floppy_720;
			curFloppy_cylinders = Cylinders_Floppy_720;
			curFloppy_speed = Datarate_Floppy_720;
			curFloppy_type = 3;
			break;
		case	4:
			curFloppy_sectors = Sectors_Floppy_144;
			curFloppy_tracks = Heads_Floppy_144;
			curFloppy_cylinders = Cylinders_Floppy_144;
			curFloppy_speed = Datarate_Floppy_144;
			curFloppy_type = 4;
			break;
		case	5:
			curFloppy_sectors = Sectors_Floppy_288;
			curFloppy_tracks = Heads_Floppy_288;
			curFloppy_cylinders = Cylinders_Floppy_288;
			curFloppy_speed = Datarate_Floppy_288;
			curFloppy_type = 5;
			break;
		default:
			curFloppy_sectors = 0;
			curFloppy_tracks = 0;
			curFloppy_cylinders = 0;
			curFloppy_speed = 0;
			curFloppy_type = 0;
			break;
	}
}
//pass mechanical drive info. step rate=3ms, unload time=240ms, load time=16ms
void floppy_mechanical_info (uint8_t stepr, uint8_t loadt, uint8_t unloadt, bool dma )
{
	if (debug) puts ("Send Mechanical Info\n");
	uint8_t data = 0;
///	if (debug) puts("fdc send SPECIFY to FIFO\n");
	FIFO_write (SPECIFY);
	data = ( (stepr & 0xf) << 4) | (unloadt & 0xf);
///	if (debug) printf("fdc send 0x%x to FIFO\n", data);
		FIFO_write (data);
	data = (( loadt << 1 ) | ( (dma) ? 0 : 1 ) );
///	if (debug) printf("fdc send 0x%x to FIFO\n", data);
		FIFO_write (data);
}

int floppy_calibrate()
{
	uint32_t st0, cyl;
	// turn on the motor
	floppy_motor (true);
	for (int i = 0; i < 4; i++)
	{
		// send command
		if (debug) puts ("Send Recalibrate command\t");
		FIFO_write ( RECALIBRATE );
		//outb (FIFO, RECALIBRATE);
		if (debug) puts ("Send Drive Number\n");
		FIFO_write ( floppy_drive_number );
		//outb (FIFO, floppy_drive_number);
		if (debug) puts ("Wait for IRQ\n");
		if (floppy_waitIRQ ()) {
			if (debug) puts("Handle IRQ\n");
			floppy_IRQ_handle ( &st0, &cyl);
		// did we fine cylinder 0? if so, we are done
			if (!cyl)
			{
				_currentCylinder = cyl;
				if (debug) puts("Calibrated\n");
				floppy_motor (false);
				return 0;
			}
			else { if (debug) puts ("Not at 0\n"); }
		}
		else
		{
			if (debug) puts ("No IRQ\n");
			fdc_quickReset();
		}
	}
	if (debug) puts ("NOT Calibrated!\n");
	floppy_motor (false);
	return -1;
}


void floppy_readSector_imp (uint8_t head, uint8_t track, uint8_t sector, uint8_t sectors)
{
if (debug) printf ("floppy_readSector_imp(%x, %x, %x, %x)\n", head, track, sector, sectors);
	uint32_t st0, cyl;
	// set the DMA for read transfer
	floppy_init_dma( (uint8_t*) DMA_BUFFER, 512*sectors );
	floppy_dma_read ( DMA_CHANNEL ); //  turned out to be in above function.
	// read in a sector
	FIFO_write ( READ_DATA | MULTI_TRACK | SKIP_MODE | MAGNETIC_ENCODING );
//	FIFO_write ( READ_DATA );
	FIFO_write ( head << 2 | floppy_drive_number );
	FIFO_write ( track);
	FIFO_write ( head);
	FIFO_write ( sector);
	FIFO_write ( 2 );	// ALWAYS 2 (512 bytes per sector)
	FIFO_write ( ( ( sector + sectors ) >= curFloppy_sectors ) ? curFloppy_sectors : sector + sectors );
//	FIFO_write ( ( ( sector + 1 ) >= curFloppy_sectors ) ? curFloppy_sectors : sector + 1 );
//	FIFO_write ( sectors );	//	REPLACED ABOVE WITH THIS TESTING MULTI SECTOR READ
	FIFO_write ( 0x1B );	// GAP1
	FIFO_write ( 0xff );	// ALWAYS FF (512 bytes per sector)
	// wait for irq
	if (floppy_waitIRQ()) {
		// read status info
		for (int j=0; j<7; j++)
			FIFO_read ();
		// let FDC know we handled interrupt
		floppy_IRQ_handle (&st0,&cyl);
	} else {
		fdc_quickReset();
		tries--;
		floppy_motor(1);
		if(tries) floppy_readSector_imp (head, track, sector, sectors);
	}
}

bool floppy_seek ( uint8_t cyl, uint8_t head )
{
	tries = 4;
	if (debug)
		printf ("floppy_seek(%x, %x)\n", cyl, head);
	if (cyl < (uint8_t) curFloppy_cylinders) {
		uint32_t st0, cyl0;
		if (floppy_drive_number >= 2)
			return 0;
		for (int i = 0; i < 10; i++ ) {
			// send the command
			FIFO_write (SEEK);
			FIFO_write ( (head) << 2 | floppy_drive_number);
			FIFO_write (cyl);
			// wait for the results phase IRQ
			if (floppy_waitIRQ()) {
				floppy_IRQ_handle (&st0,&cyl0);
				// found the cylinder?
				if ( cyl0 == cyl)
					_currentCylinder = cyl0;
					return true;
			} else {
				fdc_quickReset();
				tries--;
				floppy_motor(1);
				if(!tries) return false;
			}
		}
	}
	else
		getch("floppy_seek(): Invalid Cylinder\n");
	return false;
}

uint8_t* floppy_readSector (int sectorLBA, uint8_t sectors)
{
if (debug) printf ("floppy_readSector(%x, %x)\n", sectorLBA, sectors);
	floppy_set_dma(0x3500);
//	floppy_reset();
	floppy_motor (true);
	if (floppy_drive_number >= 2)
		return 0;
	// convert LBA sector to CHS
	int head=0, track=0, sector=1;
	lbaCHS (sectorLBA, &head, &track, &sector);
	// turn motor on and seek to track
	if (_currentCylinder != (uint16_t) track)
		if(!floppy_seek ((uint8_t)track, (uint8_t)head)) {
			puts("Did not seek properly!\n");
			return 0;
		}
	// read sector and turn motor off
	tries = 4;
	floppy_readSector_imp ((uint8_t)head, (uint8_t)track, (uint8_t)sector, sectors);
	floppy_motor (false);
	// warning: this is a bit hackish
	return (uint8_t*) DMA_BUFFER;
}


/* Sets up the Floppy Controller into IRQ6 */
void floppy_install()
{
	floppy_drive_number = 0;
	floppy_setVars(CMOS_Floppy_Master);
	install_IR(6, floppy_handler);
	_FloppyDiskIRQ = FALSE;
	fdc_disable();
	floppy_reset();	
}

void lbaCHS (int lba,int *head,int *track,int *sector)
{
   *head = ( lba % ( curFloppy_sectors * 2 ) ) / ( curFloppy_sectors );
   *track = lba / ( curFloppy_sectors * 2 );
   *sector = lba % curFloppy_sectors + 1;
}