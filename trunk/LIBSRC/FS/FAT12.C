/*
./LIBSRC/FS/FAT12.C
*/

#include <FS/FAT12.H>
#include <FS/BPB.H>
#include <STRING.H>
#include <STDIO.H>
#include <FDC.H>

#define SECTOR_SIZE 512
FILESYSTEM _FSysFat;
MOUNT_INFO _MountInfo;
uint8_t FAT [SECTOR_SIZE*2];

void ToDosFileName (const char* filename, char* fname, unsigned int FNameLength)
{
	unsigned int  i=0;
	if (FNameLength > 11)
		return;
	if (!fname || !filename)
		return;
	// set all characters in output name to spaces
	memset (fname, ' ', FNameLength);
	// 8.3 filename
	for (i=0; i < strlen(filename)-1 && i < FNameLength; i++) {
		if (filename[i] == '.' || i==8 )
			break;
		// capitalize character and copy it over (we dont handle LFN format)
		fname[i] = ChartoUpper (filename[i] );
	}
	// add extension if needed
	if (filename[i]=='.') {
		// note: cant just copy over-extension might not be 3 chars
		for (int k=0; k<3; k++) {
			++i;
			if ( filename[i] )
				fname[8+k] = filename[i];
		}
	}
	// extension must be uppercase (we dont handle LFNs)
	for (i = 0; i < 3; i++)
		fname[8+i] = ChartoUpper (fname[8+i]);
}

FILE fsysFatDirectory (const char* DirectoryName)
{
	FILE file;
	unsigned char* buf;
	PDIRECTORY directory;
	// get 8.3 directory name
	char DosFileName[11];
	ToDosFileName (DirectoryName, DosFileName, 11);
	DosFileName[11]=0;
	// 14 sectors per directory
	for (int sector=0; sector<14; sector++) {
		// read in sector of root directory
		buf = (unsigned char*) floppy_readSector (_MountInfo.rootOffset + sector, 1 );
		// get directory info
		directory = (PDIRECTORY) buf;
		// 16 entries per sector
		for (int i=0; i<16; i++) {
			// get current filename
			char name[11];
			memcpy (name, directory->Filename, 11);
			name[11]=0;
			// find a match?
			if (strcmp (DosFileName, name) == 0) {
				// found it, set up file info
				strcpy (file.name, DirectoryName);
				file.id             = 0;
				file.currentCluster = directory->FirstCluster;
				file.fileLength     = directory->FileSize;
				file.eof            = 0;
				file.fileLength     = directory->FileSize;
				// set file type
				if (directory->Attrib == 0x10)
					file.flags = FS_DIRECTORY;
				else
					file.flags = FS_FILE;
				// return file
				return file;
			}
			// go to next directory
			directory++;
		}
	}
	// unable to find file
	file.flags = FS_INVALID;
	return file;
}

void fsysFatRead(PFILE file, unsigned char* Buffer, unsigned int Length)
{
	if (file) {
		// starting physical sector
		unsigned int physSector = 32 + (file->currentCluster - 1);
		// read in sector
		unsigned char* sector = (unsigned char*) floppy_readSector ( physSector, 1 );
		// copy block of memory
		memcpy (Buffer, sector, 512);
		// locate FAT sector
		unsigned int FAT_Offset = file->currentCluster + (file->currentCluster / 2); //multiply by 1.5
		unsigned int FAT_Sector = 1 + (FAT_Offset / SECTOR_SIZE);
		unsigned int entryOffset = FAT_Offset % SECTOR_SIZE;
		// read 1st FAT sector
		sector = (unsigned char*) floppy_readSector ( FAT_Sector, 1 );
		memcpy (FAT, sector, 512);
		// read 2nd FAT sector
		sector = (unsigned char*) floppy_readSector ( FAT_Sector + 1, 1 );
		memcpy (FAT + SECTOR_SIZE, sector, 512);
		// read entry for next cluster
		uint16_t nextCluster = *( uint16_t*) &FAT [entryOffset];
		// test if entry is odd or even
		if( file->currentCluster & 0x0001 )
			nextCluster >>= 4;      //grab high 12 bits
		else
			nextCluster &= 0x0FFF;   //grab low 12 bits
		// test for end of file
		if ( nextCluster >= 0xff8) {
			file->eof = 1;
			return;
		}
		// test for file corruption
		if ( nextCluster == 0 ) {
			file->eof = 1;
			return;
		}
		// set next cluster
		file->currentCluster = nextCluster;
	}
}

void fsysFatClose (PFILE file)
{
	if (file)
		file->flags = FS_INVALID;
}

FILE fsysFatOpenSubDir (FILE kFile, const char* filename)
{
	FILE file;
	// get 8.3 directory name
	char DosFileName[11];
	ToDosFileName (filename, DosFileName, 11);
	DosFileName[11]=0;
	if (kFile.flags != FS_INVALID) {
		// read directory
		while (! kFile.eof ) {
			// read directory
			unsigned char buf[512];
			fsysFatRead (&file, buf, 512);
			// set directort
			PDIRECTORY pkDir = (PDIRECTORY) buf;
			// 16 entries in buffer
			for (unsigned int i = 0; i < 16; i++) {
				// get current filename
				char name[11];
				memcpy (name, pkDir->Filename, 11);
				name[11]=0;
				// match?
				if (strcmp (name, DosFileName) == 0) {
					// found it, set up file info
					strcpy (file.name, filename);
					file.id             = 0;
					file.currentCluster = pkDir->FirstCluster;
					file.fileLength     = pkDir->FileSize;
					file.eof            = 0;
					file.fileLength     = pkDir->FileSize;
					// set file type
					if (pkDir->Attrib == 0x10)
						file.flags = FS_DIRECTORY;
					else
						file.flags = FS_FILE;
					// return file
					return file;
				}
				// go to next entry
				pkDir++;
			}
		}
	}
	// unable to find file
	file.flags = FS_INVALID;
	return file;
}

FILE fsysFatOpen (const char* FileName)
{
	FILE curDirectory;
	char* p = 0;
	bool rootDir=true;
	char* path = (char*) FileName;
	// any '\'s in path?
	p = strchr (path, '\\');
	if (!p) {
		// nope, must be in root directory, search it
		curDirectory = fsysFatDirectory (path);
		// found file?
		if (curDirectory.flags == FS_FILE)
			return curDirectory;
		// unable to find
		FILE ret;
		ret.flags = FS_INVALID;
		return ret;
	}
	// go to next character after first '\'
	p++;
	while ( p ) {
		// get pathname
		char pathname[16];
		int i=0;
		for (i=0; i<16; i++) {
			// if another '\' or end of line is reached, we are done
			if (p[i]=='\\' || p[i]=='\0')
				break;
			// copy character
			pathname[i]=p[i];
		}
		pathname[i]=0; //null terminate
		// open subdirectory or file
		if (rootDir) {
			// search root directory - open pathname
			curDirectory = fsysFatDirectory (pathname);
			rootDir=false;
		}
		else {
			// search a subdirectory instead for pathname
			curDirectory = fsysFatOpenSubDir (curDirectory, pathname);
		}
		// found directory or file?
		if (curDirectory.flags == FS_INVALID)
			break;
		// found file?
		if (curDirectory.flags == FS_FILE)
			return curDirectory;
		// find next '\'
		p=strchr (p+1, '\\');
		if (p)
			p++;
	}
	// unable to find
	FILE ret;
	ret.flags = FS_INVALID;
	return ret;
}

void fsysFatInitialize ()
{
	// initialize filesystem struct
	strcpy (_FSysFat.Name, "FAT12");
	_FSysFat.Directory = fsysFatDirectory;
	_FSysFat.Mount     = fsysFatMount;
	_FSysFat.Open      = fsysFatOpen;
	_FSysFat.Read      = fsysFatRead;
	_FSysFat.Close     = fsysFatClose;
	// register ourself to volume manager
	VFS_RegisterFileSystem ( &_FSysFat, 0 );
	// mounr filesystem
	fsysFatMount ();
}

void fsysFatMount ()
{
	// Boot sector info
	PBOOTSECTOR bootsector;
	// read boot sector
	bootsector = (PBOOTSECTOR) floppy_readSector ( 0, 1 );
	// store mount info
	_MountInfo.numSectors     = bootsector->Bpb.NumSectors;
	_MountInfo.fatOffset      = 1;
	_MountInfo.fatSize        = bootsector->Bpb.SectorsPerFat;
	_MountInfo.fatEntrySize   = 8;
	_MountInfo.numRootEntries = bootsector->Bpb.NumDirEntries;
	_MountInfo.rootOffset     = (bootsector->Bpb.NumberOfFats * bootsector->Bpb.SectorsPerFat) + 1;
	_MountInfo.rootSize       = ( bootsector->Bpb.NumDirEntries * 32 ) / bootsector->Bpb.BytesPerSector;
}
