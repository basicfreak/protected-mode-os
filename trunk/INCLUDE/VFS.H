//VFS.H

#ifndef				VFS_H_INCLUDED
	#define				VFS_H_INCLUDED


/*
typedef signed char int8_t;
typedef unsigned char   uint8_t;
typedef short  int16_t;
typedef unsigned short  uint16_t;
typedef int  int32_t;
typedef unsigned   uint32_t;
typedef long long  int64_t;
typedef unsigned long long   uint64_t;
*/	

struct disk_entry											//32 bytes
{
	uint8_t		volumeLabel[11];
	uint64_t	sectors;
	uint32_t	heads;
	uint32_t	tracks;
	uint8_t		FS;
	uint8_t		DiskType;
	uint16_t	DiskNumber;
	uint8_t		DriveLetter;								//"A" - "Z" = 26 disks
} __attribute__((packed));

#endif