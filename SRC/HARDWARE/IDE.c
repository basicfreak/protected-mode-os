/*
./DRIVERSRC/HARDWARE/IDE.C
*/

#include "IDE.H"
#include <STDIO.H>

/**
LOCAL VARIABLES
**/

uint16_t _IDE_PRIMARY_PORT_BASE = 0x1F0;
uint16_t _IDE_SECONDARY_PORT_BASE = 0x170;
uint16_t _IDE_PRIMARY_PORT_NOINT = 0x3F6;
uint16_t _IDE_SECONDARY_PORT_NOINT = 0x376;

struct _IDE_device {
	uint8_t Reserved;
	uint8_t Channel;
	uint8_t Drive;
	uint16_t Type;
	uint16_t Signiture;
	uint16_t Capabilities;
	uint32_t CommandSets;
	uint32_t Size;
	uint8_t Model[41];
} _IDE_devices[4];

struct _IDE_Channels {
	uint16_t base;
	uint16_t control;
	uint16_t busmasteride;
	uint16_t noINT;
} channels[2];
	
enum _IDE_PORT_OFFSET {
	_IDE_PORT_DATA = 0x0,
	_IDE_PORT_FEATURES = 0x1,
	_IDE_PORT_ERROR = 0x1,
	_IDE_PORT_SECTCOUNT0 = 0x2,
	_IDE_PORT_LBA0 = 0x3,
	_IDE_PORT_LBA1 = 0x4,
	_IDE_PORT_LBA2 = 0x5,
	_IDE_PORT_HEAD = 0x6,
	_IDE_PORT_DRIVE = 0x6,
	_IDE_PORT_COMMAND = 0x7,
	_IDE_PORT_STATUS = 0x7,
	_IDE_PORT_SECTCOUNT1 = 0x8,
	_IDE_PORT_LBA3 = 0x9,
	_IDE_PORT_LBA4 = 0xA,
	_IDE_PORT_LBA5 = 0xB,
	_IDE_PORT_CONTROL = 0xC,
	_IDE_PORT_ALTSTATUS = 0xC,
	_IDE_PORT_DEVADDRESS = 0xD
};

enum _IDE_STATUS_BYTE {
	_IDE_STATUS_BSY = 0x80,
	_IDE_STATUS_RDY = 0x40,
	_IDE_STATUS_DF = 0x20,
	_IDE_STATUS_SRV = 0x10,
	_IDE_STATUS_DRQ = 0x08,
	_IDE_STATUS_COR = 0x04,
	_IDE_STATUS_IDX = 0x02,
	_IDE_STATUS_ERR = 0x01
};

enum _IDE_ERROR_BYTE {
	_IDE_ERROR_BBK = 0x80,
	_IDE_ERROR_UNC = 0x40,
	_IDE_ERROR_MC = 0x20,
	_IDE_ERROR_IDNF = 0x10,
	_IDE_ERROR_MCR = 0x08,
	_IDE_ERROR_ABRT = 0x04,
	_IDE_ERROR_TK0NF = 0x02,
	_IDE_ERROR_AMNF = 0X01
};

enum _IDE_COMMAND_LIST {
	_IDE_COMMAND_TEST = 0x00,
	_IDE_COMMAND_SERVICEACTIONIN = 0x01,
	_IDE_COMMAND_SENSE = 0x03,
	_IDE_COMMAND_FORMAT = 0x04,
	_IDE_COMMAND_INQUIRY = 0x12,
	_IDE_COMMAND_EJECT = 0x1B,	//STOP UNIT
	_IDE_COMMAND_REMOVAL = 0x1E,
	_IDE_COMMAND_FORMATCAPACITIES = 0x23,
	_IDE_COMMAND_CAPACITY = 0x25,
	_IDE_COMMAND_READ10 = 0x28,
	_IDE_COMMAND_WRITE10 = 0x2A,
	_IDE_COMMAND_SEEK10 = 0x2B,
	_IDE_COMMAND_WRITEANDVARIFY10 = 0x2E,
	_IDE_COMMAND_VERIFY10 = 0x2F,
	_IDE_COMMAND_SYNCHRONIZE = 0x35,
	_IDE_COMMAND_WRITEBUFFER = 0x3B,
	_IDE_COMMAND_READBUFFER = 0x3C,
	_IDE_COMMAND_READTOC = 0x43, // PMA / ATIP
	_IDE_COMMAND_GETCONFIG = 0x46,
	_IDE_COMMAND_GETSTATUS = 0x4A,
	_IDE_COMMAND_DISKINFO = 0x51,
	_IDE_COMMAND_TRACKINFO = 0x52,
	_IDE_COMMAND_RESERVETRACK = 0x53,
	_IDE_COMMAND_SENDOPCINFO = 0x54,
	_IDE_COMMAND_MODESET10 = 0x55,
	_IDE_COMMAND_REPAIRTRACK = 0x58,
	_IDE_COMMAND_MODEGET10 = 0x5A,
	_IDE_COMMAND_CLOSETRACK = 0x5B,
	_IDE_COMMAND_GETBUFFERCAPACITY = 0x5C,
	_IDE_COMMAND_SENDCUESHEET = 0x5D,
	_IDE_COMMAND_REPORTLUNS = 0xA0,
	_IDE_COMMAND_BLANK = 0xA1,
	_IDE_COMMAND_SECURITYIN = 0xA2,
	_IDE_COMMAND_SENDKEY = 0xA3,
	_IDE_COMMAND_GETKEY = 0xA4,
	_IDE_COMMAND_UN_LOADMEDIUM = 0xA6,
	_IDE_COMMAND_SETREADAHEAD = 0xA7,
	_IDE_COMMAND_READ = 0xA8,
	_IDE_COMMAND_WRITE = 0xAA,
	_IDE_COMMAND_GETSN = 0xAB,
	_IDE_COMMAND_GETPERFORMANCE = 0xAC,
	_IDE_COMMAND_READDISKSTRUCTURE = 0xAD,
	_IDE_COMMAND_SECURITYOUT = 0xB5,
	_IDE_COMMAND_SETSTREAMING = 0xB6,
	_IDE_COMMAND_READCDMSF = 0xB9,
	_IDE_COMMAND_SETCDSPEED = 0xBE,
	_IDE_COMMAND_SENDDISKSTRUCTURE = 0xBF
};

enum _IDE_FEATURE_LIST {
	_IDE_FEATURE_READPIO = 0x20,
	_IDE_FEATURE_READPIOEXT = 0x24,
	_IDE_FEATURE_READDMA = 0xC8,
	_IDE_FEATURE_READDMAEXT = 0x25,
	_IDE_FEATURE_WRITEPIO = 0x30,
	_IDE_FEATURE_WRITEPIOEXT = 0x34,
	_IDE_FEATURE_WRITEDMA = 0xCA,
	_IDE_FEATURE_WRITEDMAEXT = 0x35,
	_IDE_FEATURE_CACHEFLUSH = 0xE7,
	_IDE_FEATURE_CACHEFLUSHEXT = 0xEA,
	_IDE_FEATURE_PACKET = 0xA0,
	_IDE_FEATURE_IDENTIFYPACKET = 0xA1,
	_IDE_FEATURE_IDENTIFY = 0xEC
};

enum _IDE_IDENTIFY_INFO {
	_IDE_IDENTIFY_DEVICETYPE = 0x00,
	_IDE_IDENTIFY_CYLINDERS = 0x02,
	_IDE_IDENTIFY_HEADS = 0x06,
	_IDE_IDENTIFY_SECTORS = 0x0C,
	_IDE_IDENTIFY_SERIAL = 0x14,
	_IDE_IDENTIFY_MODEL = 0x36,
	_IDE_IDENTIFY_CAPABILITIES = 0x62,
	_IDE_IDENTIFY_FIELDVALID = 0x6A,
	_IDE_IDENTIFY_MAXLBA = 0x78,
	_IDE_IDENTIFY_COMMANDSETS = 0xA4,
	_IDE_IDENTIFY_MAXLBAEXT = 0xC8
};

/**
LOCAL FUNTIONS
**/
void _IDE_writePort(bool secondary, uint8_t reg, uint8_t data);
uint8_t _IDE_readPort(bool secondary, uint8_t reg);
bool _IDE_waitBusy(bool secondary, bool errchk);

uint8_t _IDE_readPort(bool secondary, uint8_t reg)
{
	unsigned char result;
	if (reg > 0x07 && reg < 0x0C)
		_IDE_writePort(secondary, _IDE_PORT_CONTROL, (uint8_t) (0x80 | channels[secondary].noINT));
	if (reg < 0x08)
		result = inb((uint16_t) (channels[secondary].base + reg - 0x00));
	else if (reg < 0x0C)
		result = inb((uint16_t) (channels[secondary].base  + reg - 0x06));
	else if (reg < 0x0E)
		result = inb((uint16_t) (channels[secondary].control  + reg - 0x0A));
	else if (reg < 0x16)
		result = inb((uint16_t) (channels[secondary].busmasteride + reg - 0x0E));
	if (reg > 0x07 && reg < 0x0C)
		_IDE_writePort(secondary, _IDE_PORT_CONTROL, (uint8_t) channels[secondary].noINT);
	return result;
	/*uint16_t base = _IDE_PRIMARY_PORT_BASE;
	if (secondary)
		base = _IDE_SECONDARY_PORT_BASE;
	return inb (base+port);*/
}

void _IDE_writePort(bool secondary, uint8_t reg, uint8_t data)
{
	if (reg > 0x07 && reg < 0x0C)
		_IDE_writePort(secondary, _IDE_PORT_CONTROL, (uint8_t) (0x80 | channels[secondary].noINT));
	if (reg < 0x08)
		outb((uint16_t) (channels[secondary].base  + reg - 0x00), data);
	else if (reg < 0x0C)
		outb((uint16_t) (channels[secondary].base  + reg - 0x06), data);
	else if (reg < 0x0E)
		outb((uint16_t) (channels[secondary].control  + reg - 0x0A), data);
	else if (reg < 0x16)
		outb((uint16_t) (channels[secondary].busmasteride + reg - 0x0E), data);
	if (reg > 0x07 && reg < 0x0C)
		_IDE_writePort(secondary, _IDE_PORT_CONTROL, (uint8_t) channels[secondary].noINT);
	/*uint16_t base = _IDE_PRIMARY_PORT_BASE;
	if (secondary)
		base = _IDE_SECONDARY_PORT_BASE;
	outb (base+port, data);*/
}

bool _IDE_waitBusy(bool secondary, bool errchk) ///int; 0 = all good; 1 = device fault; 2 = error; 3 = drq; 4 = timeout
{
	for (int i = 0; i < 100; i++); // delay just a moment in case busy has not been set (400ns required)
	uint32_t timeout = 10000;
	while ((_IDE_readPort(secondary, _IDE_PORT_STATUS) & _IDE_STATUS_BSY) || timeout == 0)
		timeout--;
	if (timeout) {
		if(!errchk)
			return 0;
		uint8_t state = _IDE_readPort(secondary, _IDE_PORT_STATUS);
		if (state & _IDE_STATUS_ERR)
			return 2;
		if (state & _IDE_STATUS_DF)
			return 1;
		if (state & _IDE_STATUS_DRQ)
			return 3;
	}
	return 4;
}

/**
PUBLIC FUNCTIONS
**/

void _IDE_init()
{
	// Setup Channel Ports For PATA
	channels[0].base = (uint16_t) ((uint16_t) ((_IDE_PRIMARY_PORT_BASE & 0xFFFFFFFC) + _IDE_PRIMARY_PORT_BASE) * (uint16_t) ((!_IDE_PRIMARY_PORT_BASE)));
	channels[0].control = (uint16_t) ((uint16_t) ((_IDE_PRIMARY_PORT_NOINT & 0xFFFFFFFC) + _IDE_PRIMARY_PORT_NOINT) * (uint16_t) ((!_IDE_PRIMARY_PORT_NOINT)));
	channels[0].busmasteride = (uint16_t) ((0 & 0xFFFFFFFC) + 0);
	channels[1].base = (uint16_t) ((uint16_t) ((_IDE_SECONDARY_PORT_BASE & 0xFFFFFFFC) + _IDE_SECONDARY_PORT_BASE) * (uint16_t) ((!_IDE_SECONDARY_PORT_BASE)));
	channels[1].control = (uint16_t) ((uint16_t) ((_IDE_SECONDARY_PORT_NOINT & 0xFFFFFFFC) + _IDE_SECONDARY_PORT_NOINT) * (uint16_t) ((!_IDE_SECONDARY_PORT_NOINT)));
	channels[1].busmasteride = (uint16_t) ((0 & 0xFFFFFFFC) + 8);
	
	// Search For Installed Drives
	
}