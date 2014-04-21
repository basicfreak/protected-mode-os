/*
./LIBSRC/FS/FAT12.C
*/

#include <FS/FAT12.H>
#include <FS/BPB.H>
#include <STRING.H>
#include <STDIO.H>
#include <FDC.H>
#include <MATH.H>

#define debug 1

#define SECTOR_SIZE 512
FILESYSTEM _FSysFat;
MOUNT_INFO _MountInfo;
uint16_t FATBlock [(SECTOR_SIZE * 9 * 8)/12];
RDIRS FATRDIR;
PRDIR _ROOTdir;
RDIRS TempDir;
PRDIR _CurDir;

void FATBlock_init()
{
	uint8_t* data = floppy_readSector(1, 9);
	for (int i = 0; i < (SECTOR_SIZE * 9 * 8)/12; i++) {
		uint16_t clust = i * 1.5;
		uint16_t value = 0;
		value = (uint16_t) data[clust+1];
		value = (value * 256) + (uint16_t) data[clust];
		if(isEven(i))
			FATBlock[i] = value & 0x0FFF;
		else
			FATBlock[i] = (value >> 4) & 0x0FFF;
		if (debug) printf("%x ",FATBlock[i]);
	}
	if (debug) puts("\n");
}

uint16_t Cluster(uint16_t cluster)
{
	return FATBlock[cluster];
}

void dirTest()
{
	for (int i = 0, j = 0; i < 224; i++) {
		//printf("%i\t", i);
		PDIRECTORY temp = _ROOTdir->entry[i];
		//for(int lol = 0; lol < 11; lol++)
		//	name[lol] = temp.Filename[lol];
		///char name[11];
		///memcpy(name, temp->Filename, 11);
		///name[11]='\0';
		if(temp->Filename[0] != 0xE5 && temp->Filename[0] != 0 && strlen((const char*) temp->Filename) >= 7) {
			char name[13];
			name[0] = temp->Filename[0];
			name[1] = temp->Filename[1];
			name[2] = temp->Filename[2];
			name[3] = temp->Filename[3];
			name[4] = temp->Filename[4];
			name[5] = temp->Filename[5];
			name[6] = temp->Filename[6];
			name[7] = temp->Filename[7];
			name[8] = '.';
			name[9] = temp->Ext[0];
			name[10] = temp->Ext[1];
			name[11] = temp->Ext[2];
			name[12] = 0;
			if(name[0] == 0x5E)
				name[0] = 0xE5;
			if(temp->Attrib&0x10) printfc(0x1, "%s", name);
			else printf("%s", name);///printf("%i.\t%s\t0x%x\n",j,name,temp->FirstCluster);
			j++;
			if (j%5==0) putch('\n');
			else putch('\t');
		}
		if(!temp->Filename[0]) break;
	}
	putch('\n');
}

void ROOTdir_init()
{
	PRDIR data = (PRDIR) floppy_readSector(19, 14);
	memcpy(&FATRDIR, data, SECTOR_SIZE*14);
	for (int i = 0; i < 224; i++) {
		_ROOTdir->entry[i] = &(FATRDIR.entry[i]);
	}
	_CurDir = _ROOTdir;
	if(debug)dirTest();
}

PRDIR getSubDirectory(FILE subDir)
{
	if (debug) puts("getSubDirectory(FILE)\n");
	unsigned char data[512];
	unsigned char ALLdata[512*14];
	PRDIR ret;
	int i = 0;
	while (subDir.eof != 1) {
		fsysFatRead ( &subDir, &ALLdata[0x200*i], 512);
		i++;
	}
	memcpy(&TempDir, ALLdata, i*0x200);
	for (int z = 0; z < i*(512/32); z++) {
		ret->entry[z] = &(TempDir.entry[z]);
	}
	return ret;
}

/***void ROOTdir_init()
{
	int j = 0;
	PRDIRS data = (PRDIRS) floppy_readSector(19, 14);
	memcpy(&FATRDIR, data, SECTOR_SIZE*14);
	for(int i = 0; i < 224; i++) {
		PDIRECTORY temp = &FATRDIR.entry[i];
		if(temp->Filename[0] != 0xE5 && temp->Filename[0] != 0) {
			_ROOTdir->entry[j] = &FATRDIR.entry[i];
			j++;
		}
	}
	DIRECTORY temp;
	temp.Filename[0] = 0;
	for(;j<224;j++)
		_ROOTdir->entry[j] = &temp;
	if(debug) dirTest();
}**/

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
	if (debug) printf("fsysFatDirectory(%s)\n", DirectoryName);
	FILE file;
	// get 8.3 directory name
	char DosFileName[12];
	ToDosFileName (DirectoryName, DosFileName, 11);
	DosFileName[11]=0;
	// get current filename
	for (int i = 0; i < 224; i++) {
		PDIRECTORY directory = _CurDir->entry[i];
		char name[12];
		memcpy (name, directory->Filename, 11);
		name[11]=0;
		// find a match?
		if (streql (DosFileName, name)) {
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
	}
	// unable to find file
	file.flags = FS_INVALID;
	return file;
}

void fsysFatRead(PFILE file, unsigned char* Buffer, unsigned int Length)
{
	if (debug) printf("fsysFatRead(FILE, %x, %x)\n", Buffer, Length);
	if (file) {
		// starting physical sector
		///unsigned int physSector = 32 + (file->currentCluster);
		// read in sector
		unsigned char* sector = (unsigned char*) floppy_readSector ( 31 + file->currentCluster, 1 );
		// copy block of memory
		memcpy (Buffer, sector, 512);
		// read entry for next cluster
		uint16_t nextCluster = Cluster(file->currentCluster);
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
	if (debug) printf("fsysFatOpenSubDir(FILE, %s)\n", filename);
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
				if (streql (name, DosFileName)) {
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
	if (debug) printf("fsysFatOpen(%s)\n", FileName);
	FILE curDirectory;
	char* p = 0;
	bool rootDir=true;
	char* path = (char*) FileName;
	// any '\'s in path?
	p = strchr (path, '\\');
	if (!p) {
		// nope, must be in root directory, search it
		_CurDir = _ROOTdir;
		curDirectory = fsysFatDirectory (path);
		// found file?
		if (curDirectory.flags == FS_FILE)
			return curDirectory;
	}
	else {
		char out[50][100];
		int temp = explode( out, (const char *)path, '\\' );
		for (int z = 0; z < temp; z ++) {
			printf("%i = %s\n", z, out[z]);
			if (!z) { // z = 0 looking for first sub dir
				_CurDir = _ROOTdir;
				curDirectory = fsysFatDirectory (out[z]);
				// found file?
				if (curDirectory.flags != FS_DIRECTORY) { //subdirectory NOT found
					FILE ret;
					ret.flags = FS_INVALID;
					return ret;
				if (curDirectory.flags == FS_FILE)
					return curDirectory;
				}
			}
			else {
				_CurDir = getSubDirectory(curDirectory);
				curDirectory = fsysFatDirectory (out[z]);
				if (curDirectory.flags != FS_DIRECTORY) { //subdirectory NOT found
					FILE ret;
					ret.flags = FS_INVALID;
					return ret;
				if (curDirectory.flags == FS_FILE)
					return curDirectory;
				}
			}
		}
		_CurDir = getSubDirectory(curDirectory);
		curDirectory = fsysFatDirectory (out[temp]);
		_CurDir = _ROOTdir;
		if (curDirectory.flags == FS_FILE)
			return curDirectory;
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
	FATBlock_init();
	ROOTdir_init();
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
