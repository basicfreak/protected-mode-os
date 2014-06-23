/*
./LIBSRC/FDC.C
*/

#include "FDC.H"
#include <STDIO.H>
#include "CMOS.H"
#include "DMA.H"
#include "../SYSTEM/CPU/IRQ.H"
#include "TIMER.H"

#define DEBUG

int tries;
uint16_t _currentCylinder;

void floppy_dma_read (void);
void floppy_dma_write (void);
uint8_t FIFO_read(void);
bool FIFO_write(uint8_t cmd);
void DOR_write(uint8_t cmd);
bool floppy_waitIRQ(void);		
bool FIFO_ready(void);
void floppy_handler(regs *r);
void floppy_IRQ_handle(uint32_t* st0, uint32_t* cyl);
bool floppy_changed(void);
void floppy_speed(uint8_t speed); //0=500KB\s 1=300KB\s 2=250KB\s 3=1MB\s
void floppy_current(uint8_t driveNum);
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

uint8_t *DMA_BUFFER = (uint8_t *) 0x1000;
uint8_t DMA_CHANNEL = 2;

int FLOPPY_MOTOR_LIST[4] = {16, 32, 64, 128};

void floppy_set_dma (uint8_t *addr)
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 45) floppy_set_dma(0x%x)\n\r", addr);
#endif
	DMA_BUFFER = addr;
}

//initialize DMA to use phys addr 1k-64k
bool floppy_init_dma(uint8_t* buffer, unsigned length)
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 53) floppy_init_dma(%x, %x)\n\r", buffer, length);
#endif
	union {
      uint8_t byte[4];//Lo[0], Mid[1], Hi[2]
      unsigned long l;
	}a, c;
   a.l=(unsigned)buffer;
   c.l=(unsigned)length-1;
   //Check for buffer issues
   if ((a.l >> 24) || (c.l >> 16) || (((a.l & 0xffff)+c.l) >> 16)) {
#ifdef DEBUG
	txf(1, "\t(FDC.C:Line 66) BUFFER ERROR\n\r");
#endif
		return false;
   }
   dma_reset ();
   dma_mask_channel( DMA_CHANNEL );//Mask channel 2
   dma_reset_flipflop ( 1 );//Flipflop reset on DMA 1
   dma_set_address( DMA_CHANNEL, a.byte[0],a.byte[1]);//Buffer address
   dma_reset_flipflop( 1 );//Flipflop reset on DMA 1
   dma_set_count( DMA_CHANNEL, c.byte[0],c.byte[1]);//Set count
   dma_set_read ( DMA_CHANNEL );
   dma_unmask_all();//Unmask channel 2
   return true;
}
 
//prepare the DMA for read transfer
void floppy_dma_read ()
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 84) floppy_dma_read()\n\r");
#endif
	dma_set_read ( DMA_CHANNEL );
}
 
//prepare the DMA for write transfer
void floppy_dma_write ()
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 93) floppy_dma_write()\n\r");
#endif
	dma_set_write ( DMA_CHANNEL );
}

uint8_t FIFO_read()
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 101) FIFO_read()\n\r");
#endif
	for (int i = 0; i < 750; i++ ) {
		if ( FIFO_ready () ) {
///			if (debug) puts("FIFO Read\n");
			return inb (FIFO);
		}// else timer_wait(1);
	}
#ifdef DEBUG
	txf(1, "\t(FDC.C:Line 107) FIFO FAILED TO READ Continue...\n\r");
#endif
	return 0;
}

bool FIFO_write(uint8_t cmd)
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 118) FIFO_write(0x%x)\n\r", cmd);
#endif
	for (int i = 0; i < 750; i++ ) {
		if ( FIFO_ready () ) {
///			if (debug) puts("FIFO Wrote\n");
			outb (FIFO, cmd);
			return 1;
		}// else timer_wait(1);
	}
#ifdef DEBUG
	txf(1, "\t(FDC.C:Line 124) FIFO FAILED TO WRITE Contine...\n\r");
#endif
	return 0;
}

void DOR_write(uint8_t cmd)
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 136) DOR_write(0x%x)\n\r", cmd);
#endif
	outb(DOR, cmd);
}

bool floppy_waitIRQ()
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 144) floppy_waitIRQ()\n\r");
#endif
	//while (!_FloppyDiskIRQ);
	int timeout = 10;
	while ( !_FloppyDiskIRQ && timeout) {
		timer_wait(6);
		timeout--;
	}
	_FloppyDiskIRQ = FALSE;
	if (!timeout) {
#ifdef DEBUG
	txf(1, "\t(FDC.C:Line 156) NO IRQ\n\r");
#endif
		return FALSE;
	}
	else return TRUE;
}

void floppy_reset()		
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 165) floppy_reset()\n\r");
#endif
	uint32_t st0, cyl;
	//reset the controller
	fdc_disable ();
	fdc_enable ();
	
	if(floppy_waitIRQ ()) {
	// send CHECK_INT/SENSE INTERRUPT command to all drives
		for (int i=0; i<4; i++) {
			floppy_IRQ_handle (&st0,&cyl);
		}
	}
	else
	{
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
#ifdef DEBUG
	txf(1, "(FDC.C:Line 203) FIFO_ready()\n\r");
#endif
	bool ret = FALSE;
	//int temp = floppy_status();
	//if ( (temp & 0xc0) == 0x80 || (temp & 0xc0) == 0x81) ret = TRUE;
	if (floppy_status() & 0x80) ret = TRUE;
///	if (debug && ret) puts("FIFO Ready\n");
	//if (debug && !ret) puts("FIFO NOT Ready\n");
#ifdef DEBUG
	if(!ret)
		txf(1, "(FDC.C:Line 211) Not Ready\n\r");
	else
		txf(1, "(FDC.C:Line 211) Ready\n\r");
#endif
	return ret;
}

uint8_t floppy_status()
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 223) floppy_status()\n\r");
#endif
	return inb(MSR);
}

void floppy_handler(regs *r)
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 231) floppy_handler(0x%x)\n\r\tFDC IRQ RECIVED\n\r", r);
#endif
	if(r->eax){}
	//! irq fired
	_FloppyDiskIRQ = TRUE;
}

void floppy_IRQ_handle(uint32_t* st0, uint32_t* cyl)
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 241) floppy_IRQ_handle(0x%x, 0x%x)\n\r", st0, cyl);
#endif
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
#ifdef DEBUG
	txf(1, "\t(FDC.C:Line 260) IRQ HANDLED\n\r");
#endif
			return;
		} else timer_wait(6);
	}
}

bool floppy_changed()
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 265) floppy_changed()\n\r");
#endif
	bool ret = FALSE;
	if (inb(DIR) >= 0x80) ret = TRUE;
	return ret;
}

void floppy_motor(bool on)
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 275) floppy_motor(%s)\n\r", ((on) ? "TRUE" : "FALSE"));
#endif
	if(on)
		DOR_write ((uint8_t) (0x0C | floppy_drive_number  | FLOPPY_MOTOR_LIST[floppy_drive_number]));
	else
		DOR_write ((uint8_t) (0x0C | floppy_drive_number));
	timer_wait(20);
}

void floppy_speed(uint8_t speed) //0=500KB\s 1=300KB\s 2=250KB\s 3=1MB\s
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 287) floppy_speed(0x%x)\n\r", speed);
#endif
	if ( speed > 3)
		return;
	outb(CCR, speed);
	//outb(DSR, speed);
}

void floppy_current(uint8_t driveNum)
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 298) floppy_current(0x%x)\n\r", driveNum);
#endif
	int temp = 0;
	if ( driveNum > 1 )
		return;
	floppy_drive_number = driveNum;
	if(driveNum == 0) temp = CMOS_Floppy_Master;
	else temp = CMOS_Floppy_Slave;
	floppy_setVars(temp);
	
}

void fdc_quickReset()
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 313) fdc_quickReset()\n\r");
#endif
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
#ifdef DEBUG
	txf(1, "(FDC.C:Line 328) fdc_enable()\n\r");
#endif
	DOR_write (0x0C | floppy_drive_number);
}

void fdc_disable()
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 336) fdc_disable()\n\r");
#endif
	DOR_write (0);
}

void floppy_setVars(int i)
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 344) floppy_setVars(0x%x)\n\r", i);
#endif
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
void floppy_mechanical_info (uint8_t stepr, uint8_t loadt, uint8_t unloadt, bool dma ) //(3,16,240,true)
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 397) floppy_mechanical_info(0x%x, 0x%x, 0x%x, %s)\n\r", stepr, loadt, unloadt, ((dma) ? "TRUE" : "FALSE"));
#endif
	uint8_t data = 0;
	FIFO_write (SPECIFY);
	data = (uint8_t) (( (stepr & 0xf) << 4) | (unloadt & 0xf)); //110000 | 0000 = 110000 = 30
	FIFO_write (data);
	data = (uint8_t) (( loadt << 1 ) | ( (dma) ? 0 : 1 ) ); //100000 | 0 = 20
	FIFO_write (data);
}

int floppy_calibrate()
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 410) floppy_calibrate()\n\r");
#endif
	uint32_t st0, cyl;
	// turn on the motor
	floppy_motor (true);
	for (int i = 0; i < 4; i++)
	{
		// send command
		FIFO_write ( RECALIBRATE );
		FIFO_write ( floppy_drive_number );
		if (floppy_waitIRQ ()) {
			floppy_IRQ_handle ( &st0, &cyl);
		// did we fine cylinder 0? if so, we are done
			if (!cyl) {
				_currentCylinder = (uint16_t) cyl;
#ifdef DEBUG
	txf(1, "\t(FDC.C:Line 426) CALIBRATED!\n\r");
#endif
				floppy_motor (false);
				return 0;
			}  
#ifdef DEBUG
	txf(1, "\t(FDC.C:Line 426) NOT CALIBRATED()\n\r");
#endif
		} else {
			fdc_quickReset();
		}
	}
	floppy_motor (false);
	return -1;
}


void floppy_readSector_imp (uint8_t head, uint8_t track, uint8_t sector, uint8_t sectors)
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 446) floppy_readSector_imp(0x%x, 0x%x, 0x%x, 0x%x)\n\r", head, track, sector, sectors);
#endif
	uint32_t st0, cyl;
	// set the DMA for read transfer
	floppy_init_dma( (uint8_t*) DMA_BUFFER, (unsigned int) (512*sectors) );
	floppy_dma_read (); //  turned out to be in above function.
	// read in a sector
	FIFO_write ( (uint8_t) (READ_DATA | MULTI_TRACK | SKIP_MODE | MAGNETIC_ENCODING) );
//	FIFO_write ( READ_DATA );
	FIFO_write ( (uint8_t) (head << 2 | floppy_drive_number) );
	FIFO_write ( (uint8_t) track);
	FIFO_write ( (uint8_t) head);
	FIFO_write ( (uint8_t) sector);
	FIFO_write ( (uint8_t) 2 );	// ALWAYS 2 (512 bytes per sector)
	FIFO_write ( (uint8_t) (( ( sector + sectors ) >= curFloppy_sectors ) ? curFloppy_sectors : sector + sectors) );
//	FIFO_write ( ( ( sector + 1 ) >= curFloppy_sectors ) ? curFloppy_sectors : sector + 1 );
//	FIFO_write ( sectors );	//	REPLACED ABOVE WITH THIS TESTING MULTI SECTOR READ
	FIFO_write ( (uint8_t) 0x1B );	// GAP1
	FIFO_write ( (uint8_t) 0xff );	// ALWAYS FF (512 bytes per sector)
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
#ifdef DEBUG
	txf(1, "(FDC.C:Line 483) floppy_seek(0x%x, 0x%x)\n\r", cyl, head);
#endif
	tries = 4;
	if (cyl < (uint8_t) curFloppy_cylinders) {
		uint32_t st0, cyl0;
		if (floppy_drive_number >= 2)
			return 0;
		for (int i = 0; i < 10; i++ ) {
			// send the command
			FIFO_write (SEEK);
			FIFO_write ( (uint8_t) ((head) << 2 | floppy_drive_number));
			FIFO_write (cyl);
			// wait for the results phase IRQ
			if (floppy_waitIRQ()) {
				floppy_IRQ_handle (&st0,&cyl0);
				// found the cylinder?
				if ( cyl0 == cyl)
					_currentCylinder = (uint16_t) cyl0;
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
#ifdef DEBUG
	txf(1, "(FDC.C:Line 518) floppy_readSector(%x, %x)\n\r", sectorLBA, sectors);
#endif
	floppy_set_dma((uint8_t *) 0x3500);
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
#ifdef DEBUG
	txf(1, "\t(FDC.C:Line 533) Did not seek properly!\n\r");
#endif
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
#ifdef DEBUG
	txf(1, "(FDC.C:Line 550) floppy_install()\n\r");
#endif
	floppy_drive_number = 0;
	floppy_setVars(CMOS_Floppy_Master);
	install_IR(6, floppy_handler);
	_FloppyDiskIRQ = FALSE;
	fdc_disable();
	floppy_reset();	
}

void lbaCHS (int lba,int *head,int *track,int *sector)
{
#ifdef DEBUG
	txf(1, "(FDC.C:Line 563) lbaCHS(");
#endif
	*head = ( lba % ( curFloppy_sectors * 2 ) ) / ( curFloppy_sectors );
	*track = lba / ( curFloppy_sectors * 2 );
	*sector = lba % curFloppy_sectors + 1;
#ifdef DEBUG
   txf(1, "0x%x, 0x%x, 0x%x, 0x%x)\n\r", lba, *head, *track, *sector);
#endif
}