/*
./DRIVERSRC/8042/8042.C
*/

#include "8042.H"
#include <STDIO.H>

/*
LOCAL DATA
*/
enum _8042_Commands
{
	_8042_READBYTE = 0x20, //+0-1F
	_8042_WRITEBYTE = 0x60, //+0-1F
	_8042_DISABLE_PORT2 = 0xA7,
	_8042_ENABLE_PORT2 = 0xA8,
	_8042_TEST_PORT2 = 0xA9, //0 = PASS
	_8042_TEST = 0xAA, //0x55 = PASS - 0xFC = FAIL
	_8042_TEST_PORT1 = 0xAB, //0 = PASS
	_8042_DUMP = 0xAC,
	_8042_DISABLE_PORT1 = 0xAD,
	_8042_ENABLE_PORT1 = 0xAE,
	_8042_READ_INPUT_PORT = 0xC0,
	_8042_COPY_INPUT_LOW = 0xC1,
	_8042_COPY_INPUT_HIGH = 0xC2,
	_8042_READ_OUTPUT_PORT = 0xD0,
	_8042_WRITE_OUTPUT_PORT = 0xD1,
	_8042_WRITE_DEVICE_OUT_1 = 0xD2,
	_8042_WRITE_DEVICE_OUT_2 = 0xD3,
	_8042_WRITE_PORT2 = 0xD4		
};

enum _8042_Config
{
	_8042_PORT1_INT = 0x1, // 1 = ENABLE
	_8042_PORT2_INT = 0x2, // 1 = ENABLE
	_8042_SYS_FLAG = 0x4, // 1 IF PASSED BIOS POST
	_8042_PORT1_CLOCK = 0x10, // 0 = ENABLE
	_8042_PORT2_CLOCK = 0x20, // 0 = ENABLE
	_8042_PORT1_TRANSLATION = 0x40 // 1 = ENABLE
};

enum _8042_Ports
{
	_8042_DEVICE = 0x60,
	_8042_STATUS = 0x64,
	_8042_COMMAND = 0x64
};

enum _8042_Status
{
	_8042_OUTPUT_STATUS = 0x1,
	_8042_INPUT_STATUS = 0x2,
	_8042_FLAG = 0x4,
	_8042_CMD_DATA = 0x8,
	_8042_TIMEOUT = 0x40,
	_8042_PARITY =  0x80
};

bool dualPort = false;
bool initilized = false;
uint8_t CONFIG_8042;

/*
PRIVATE FUNCTIONS
*/
void sendCommand_8042(uint8_t data)
{
	outb(_8042_COMMAND, data);
}
uint8_t getStatus_8042()
{
	return (uint8_t) inb(_8042_STATUS);
}
void sendDevice_8042(uint8_t data)
{
	outb(_8042_DEVICE, data);
}
uint8_t getDevice_8042()
{
	return (uint8_t) inb(_8042_DEVICE);
}


/*
PUBLIC FUNCTIONS
*/
bool init_8042()
{
	// DISABLE 8042'S PORTS
	sendCommand_8042(_8042_DISABLE_PORT1);
	sendCommand_8042(_8042_DISABLE_PORT2);
	// FLUSH BUFFER
	while (getStatus_8042() & _8042_OUTPUT_STATUS)
		getDevice_8042();
	// GET CONFIG
	sendCommand_8042(_8042_READBYTE);
/// DOES DATA COME IN ON STATUS OR DEVICE PORT??? LETS ASSUME DEVICE
	CONFIG_8042 = getDevice_8042();
	// WE HAVE 2 PORTS?
	if (!(CONFIG_8042 & _8042_PORT2_CLOCK))
		dualPort = true;
	// CLEAR BIT 0, 1, 6
	CONFIG_8042 &= (_8042_SYS_FLAG | _8042_PORT1_CLOCK | _8042_PORT2_CLOCK);
	// SEND CONFIG
	sendCommand_8042(_8042_WRITEBYTE);
	sendDevice_8042(CONFIG_8042);
	// SELF TEST
	sendCommand_8042(_8042_TEST);
/// DOES DATA COME IN ON STATUS OR DEVICE PORT??? LETS ASSUME DEVICE
	if (!getDevice_8042() == 0x55)	// ERROR
		return false;
	sendCommand_8042(_8042_TEST_PORT1);
	if (getDevice_8042())
		return false;
	sendCommand_8042(_8042_TEST_PORT2);
	if (getDevice_8042())
		dualPort = false;
	return true;
}