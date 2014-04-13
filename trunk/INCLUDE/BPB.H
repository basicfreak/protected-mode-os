//BPB.H

#ifndef				BPB_H_INCLUDED
	#define				BPB_H_INCLUDED
/*
BPB (BOOTSECTOR):
0x03 - 0x0A		DESCRIPTION
0x0B - 0x0C		BYTES PER SECTOR
0x0D			SECTORS PER CLUSTER
0x0E - 0x0F		RESERVED SECTORS (1)
0x10			NUMBER OF FATS
0x11 - 0x12		ROOT ENTRIES
0x13 - 0x14		TOTAL SECTORS
				IF > 65535 SET TO 0
				AND STORE AT 0x20
0x15			MEDIA DESCRIPTOR
0x16 - 0x17		SECTORS PER FAT
0x18 - 0x19		SECTORS PER TRACK (HEAD)
0x1A - 0x1B		NUMBER OF HEADS
0x1C - 0x1F		HIDDEN SECTORS (0)
0x20 - 0x23		TOTAL SECTORS
0x24 - 0x25		DRIVE NUMBER
0x26			EXT BOOT RECORD SIGNATURE
0x27 - 0x2A		VOLUME SERIAL NUMBER
0x2B - 0x35		VOLUME LABEL
0x36 - 0x3D		FS LABEL
*/

struct bpb_entry											//59 bytes
{
	uint8_t		Description[8];
	uint16_t	bytesPerSector;
	uint8_t		sectorsPerCluster;
	uint16_t	reservedSectors;
	uint8_t		FATs;
	uint16_t	rootEntries;
	uint16_t	sectors;
	uint8_t		media;
	uint16_t	sectorsPerFAT;
	uint16_t	sectorsPerHead;
	uint16_t	heads;
	uint32_t	hiddenSectors;
	uint32_t	sectorsExt;
	uint16_t	driveNumber;
	uint8_t		bootSignature;
	uint32_t	serialNumber;
	uint8_t		volumeLabel[11];
	uint8_t		FS[8];
} __attribute__((packed));

struct bpb_entry bpb[26];



#endif
