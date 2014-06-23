/*
./INCLUDE/FDC.H
*/

#include <FORMATTING.H>

#ifndef				FDC_H_INCLUDED
	#define				FDC_H_INCLUDED
	
//1
	#define				Heads_Floppy_360		2
	#define				Sectors_Floppy_360		9
	#define				Cylinders_Floppy_360	40
	#define				Datarate_Floppy_360		2
//2
	#define				Heads_Floppy_12			2
	#define				Sectors_Floppy_12		15
	#define				Cylinders_Floppy_12		80
	#define				Datarate_Floppy_12		0
//3
	#define				Heads_Floppy_720		2
	#define				Sectors_Floppy_720		9
	#define				Cylinders_Floppy_720	80
	#define				Datarate_Floppy_720		1
//4
	#define				Heads_Floppy_144		2
	#define				Sectors_Floppy_144		18
	#define				Cylinders_Floppy_144	80
	#define				Datarate_Floppy_144		0
//5
	#define				Heads_Floppy_288		2
	#define				Sectors_Floppy_288		36
	#define				Cylinders_Floppy_288	80
	#define				Datarate_Floppy_288		3
	
	enum FloppyRegisters
	{
	   STAT_A						= 0x3F0, // read-only
	   STAT_B						= 0x3F1, // read-only
	   DOR							= 0x3F2,
	   TDR							= 0x3F3,
	   MSR							= 0x3F4, // read-only
	   DSR							= 0x3F4, // write-only
	   FIFO                        	= 0x3F5,
	   DIR         	  				= 0x3F7, // read-only
	   CCR   						= 0x3F7  // write-only
	};

	enum FIFO_Commands
	{
	   READ_TRACK =                 0x02,	// generates IRQ6
	   SPECIFY =                    0x03,   // * set drive parameters
	   SENSE_DRIVE_STATUS =         0x04,
	   WRITE_DATA =                 0x05,   // * write to the disk
	   READ_DATA =                  0x06,   // * read from the disk
	   RECALIBRATE =                0x07,   // * seek to cylinder 0
	   SENSE_INTERRUPT =            0x08,	// * ack IRQ6, get status of last command
	   WRITE_DELETED_DATA =         0x09,
	   READ_ID =                    0x0A,	// generates IRQ6
	   READ_DELETED_DATA =          0x0C,
	   FORMAT_TRACK =               0x0D,   // *
	   SEEK =                       0x0F,   // * seek both heads to cylinder X
	   VERSION =                    0x10,	// * used during initialization, once
	   SCAN_EQUAL =                 0x11,
	   PERPENDICULAR_MODE =         0x12,	// * used during initialization, once, maybe
	   CONFIGURE =                  0x13,   // * set controller parameters
	   LOCK =                       0x14,   // * protect controller params from a reset
	   VERIFY =                     0x16,
	   SCAN_LOW_OR_EQUAL =          0x19,
	   SCAN_HIGH_OR_EQUAL =         0x1D
	};

	enum FIFO_CommandExt
	{
		SKIP_MODE =					0x20,
		MAGNETIC_ENCODING =			0x40,
		MULTI_TRACK =				0x80
	};
	
	uint8_t floppy_drive_number, curFloppy_sectors, curFloppy_tracks, curFloppy_cylinders, curFloppy_speed, curFloppy_type;
	
	void floppy_set_dma (uint8_t *addr);
	bool floppy_init_dma(uint8_t* buffer, unsigned length);
	uint8_t* floppy_readSector (int sectorLBA, uint8_t secotrs);
	void floppy_install(void);
	void lbaCHS (int lba,int *head,int *track,int *sector);
	void floppy_reset(void);
	void floppy_motor(bool on);
	
#endif