/*
./DRIVERSRC/HARDWARE/PCI.H
*/

#ifndef				_PCI_H_INCLUDED
	#define				_PCI_H_INCLUDED
	
	#include <FORMATTING.H>
	
	typedef struct _PCI_DEVICE_HEADER {
		uint8_t		BUS;
		uint8_t		DEVICE;
		uint16_t	VendorID;
		uint16_t	DeviceID;
		uint16_t	Command;
		uint16_t	Status;
		uint8_t		RevisionID;
		uint8_t		ProgIF;
		uint8_t		Subclass;
		uint8_t		Class;
		uint8_t		CacheSize;
		uint8_t		LatencyTimer;
		uint8_t		HeaderType;
		uint8_t		BIST;				// Built In Self Test
	}PCIDEV;		// But really would someone have this many?!?
	
	typedef struct _PCI_DEVICE_HEADERS {
		PCIDEV	Device[224];
	}PCIDEVLIST;
	
	void _PCI_init(void);
	
	
#endif