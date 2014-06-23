/*
./DRIVERSRC/HARDWARE/PCI.C
*/

#include "PCI.H"
#include <STDIO.H>

#define DEBUG

/**
LOCAL VARIABLES
**/
PCIDEVLIST _PCI_DEVICE_LIST;
enum _PCI_CONFIGURATION_PORTS {
	_PCI_CONFIG_ADDRESS = 0xCF8,
	_PCI_CONFIG_DATA = 0xCFC
};

uint16_t _PCI_DEVICES;

uint16_t _PCI_CONFIG_getWord(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
uint16_t _PCI_getVendorID(uint8_t bus, uint8_t slot);
uint16_t _PCI_getDeviceID(uint8_t bus, uint8_t slot);
uint16_t _PCI_getCommand(uint8_t bus, uint8_t slot);
uint16_t _PCI_getStatus(uint8_t bus, uint8_t slot);
uint8_t _PCI_getRevisionID(uint8_t bus, uint8_t slot);
uint8_t _PCI_getProgIF(uint8_t bus, uint8_t slot);
uint8_t _PCI_getSubclass(uint8_t bus, uint8_t slot);
uint8_t _PCI_getClass(uint8_t bus, uint8_t slot);
uint8_t _PCI_getCacheSize(uint8_t bus, uint8_t slot);
uint8_t _PCI_getLatencyTimer(uint8_t bus, uint8_t slot);
uint8_t _PCI_getHeaderType(uint8_t bus, uint8_t slot);
uint8_t _PCI_getBIST(uint8_t bus, uint8_t slot);
void _PCI_scanDevices(void);


/*
CONFIG
BITS:
31			30-24		23-16		15-11		10-8		7-2			1-0
ENABLE		RESERVED	BUS NUMBER	DEVICE NUM	FUNCTION #	REGISTER #	00
*/

/**
LOCAL FUNTIONS
**/

uint16_t _PCI_CONFIG_getWord(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset)
{
	uint32_t add;
	uint32_t Lbus = (uint32_t) bus;
	uint32_t Lslot = (uint32_t) slot;
	uint32_t Lfunction = (uint32_t) function;
	uint32_t Loffset = (uint32_t) offset;
	
	add = (uint32_t) ((Lbus << 16) | (Lslot << 11) | (Lfunction << 8) | ( Loffset & 0xFC) | ((uint32_t) 0x80000000));
	
	outl(_PCI_CONFIG_ADDRESS, add);
	return (uint16_t) ((inl(_PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
}

uint16_t _PCI_getVendorID(uint8_t bus, uint8_t slot)
{
	return _PCI_CONFIG_getWord(bus, slot, 0, 0);
}

uint16_t _PCI_getDeviceID(uint8_t bus, uint8_t slot)
{
	return _PCI_CONFIG_getWord(bus, slot, 0, 2);
}

uint16_t _PCI_getCommand(uint8_t bus, uint8_t slot)
{
	return _PCI_CONFIG_getWord(bus, slot, 4, 0);
}

uint16_t _PCI_getStatus(uint8_t bus, uint8_t slot)
{
	return _PCI_CONFIG_getWord(bus, slot, 4, 2);
}

uint8_t _PCI_getRevisionID(uint8_t bus, uint8_t slot)
{
	return (uint8_t) (_PCI_CONFIG_getWord(bus, slot, 8, 0) & 0xFF);
}

uint8_t _PCI_getProgIF(uint8_t bus, uint8_t slot)
{
	return (uint8_t) (_PCI_CONFIG_getWord(bus, slot, 8, 0) >> 8) & 0xFF;
}

uint8_t _PCI_getSubclass(uint8_t bus, uint8_t slot)
{
	return (uint8_t) (_PCI_CONFIG_getWord(bus, slot, 8, 2) & 0xFF);
}

uint8_t _PCI_getClass(uint8_t bus, uint8_t slot)
{
	return (uint8_t) (_PCI_CONFIG_getWord(bus, slot, 8, 2) >> 8) & 0xFF; 
}

uint8_t _PCI_getCacheSize(uint8_t bus, uint8_t slot)
{
	return (uint8_t) (_PCI_CONFIG_getWord(bus, slot, 0xC, 0) & 0xFF);
}

uint8_t _PCI_getLatencyTimer(uint8_t bus, uint8_t slot)
{
	return (uint8_t) (_PCI_CONFIG_getWord(bus, slot, 0xC, 0) >> 8) & 0xFF;
}

uint8_t _PCI_getHeaderType(uint8_t bus, uint8_t slot)
{
	return (uint8_t) (_PCI_CONFIG_getWord(bus, slot, 0xC, 2) & 0xFF);
}

uint8_t _PCI_getBIST(uint8_t bus, uint8_t slot)
{
	return (uint8_t) (_PCI_CONFIG_getWord(bus, slot, 0xC, 2) >> 8) & 0xFF; 
}

void _PCI_scanDevices()
{
	uint16_t list = 0;
	for (uint8_t i = 0; i < 0xFF; i++)
		for (uint8_t j = 0; j < 0x20; j++)
			if (_PCI_getVendorID(i, j) != 0xFFFF) {		// Does device exist?
				_PCI_DEVICE_LIST.Device[list].BUS = i;
				_PCI_DEVICE_LIST.Device[list].DEVICE = j;
				_PCI_DEVICE_LIST.Device[list].VendorID = _PCI_getVendorID(i, j);
				_PCI_DEVICE_LIST.Device[list].DeviceID = _PCI_getDeviceID(i, j);
				_PCI_DEVICE_LIST.Device[list].Command =  _PCI_getCommand(i, j);
				_PCI_DEVICE_LIST.Device[list].Status =  _PCI_getStatus(i, j);
				_PCI_DEVICE_LIST.Device[list].RevisionID =  _PCI_getRevisionID(i, j);
				_PCI_DEVICE_LIST.Device[list].ProgIF =  _PCI_getProgIF(i, j);
				_PCI_DEVICE_LIST.Device[list].Subclass =  _PCI_getSubclass(i, j);
				_PCI_DEVICE_LIST.Device[list].Class =  _PCI_getClass(i, j);
				_PCI_DEVICE_LIST.Device[list].CacheSize =  _PCI_getCacheSize(i, j);
				_PCI_DEVICE_LIST.Device[list].LatencyTimer =  _PCI_getLatencyTimer(i, j);
				_PCI_DEVICE_LIST.Device[list].HeaderType =  _PCI_getHeaderType(i, j);
				_PCI_DEVICE_LIST.Device[list].BIST =  _PCI_getBIST(i, j);
				list++;
			}
	_PCI_DEVICES = list;
}

/**
PUBLIC FUNCTIONS
**/

void _PCI_init()
{
	_PCI_scanDevices();
#ifdef DEBUG
	printf("\nPCI\nBUS_DEVICE\tVendor\tDevice\tClass\tSubClass\tHead Type\n");
	for (int i = 0; i < _PCI_DEVICES; i++) {
		printf("PCI_%i_%i\t\t0x%x\t0x%x\t0x%x\t0x%x\t\t0x%x\n", _PCI_DEVICE_LIST.Device[i].BUS, _PCI_DEVICE_LIST.Device[i].DEVICE,
						_PCI_DEVICE_LIST.Device[i].VendorID, _PCI_DEVICE_LIST.Device[i].DeviceID, _PCI_DEVICE_LIST.Device[i].Class,
														_PCI_DEVICE_LIST.Device[i].Subclass, _PCI_DEVICE_LIST.Device[i].HeaderType);
/**		printf("0x%x\n", ((_PCI_CONFIG_getWord(_PCI_DEVICE_LIST.Device[i].BUS, _PCI_DEVICE_LIST.Device[i].DEVICE, 0x10, 2) << 8) + (_PCI_CONFIG_getWord(_PCI_DEVICE_LIST.Device[i].BUS, _PCI_DEVICE_LIST.Device[i].DEVICE, 0x10, 0))));
		printf("0x%x\n", ((_PCI_CONFIG_getWord(_PCI_DEVICE_LIST.Device[i].BUS, _PCI_DEVICE_LIST.Device[i].DEVICE, 0x14, 2) << 8) + (_PCI_CONFIG_getWord(_PCI_DEVICE_LIST.Device[i].BUS, _PCI_DEVICE_LIST.Device[i].DEVICE, 0x14, 0))));
		printf("0x%x\n", ((_PCI_CONFIG_getWord(_PCI_DEVICE_LIST.Device[i].BUS, _PCI_DEVICE_LIST.Device[i].DEVICE, 0x18, 2) << 8) + (_PCI_CONFIG_getWord(_PCI_DEVICE_LIST.Device[i].BUS, _PCI_DEVICE_LIST.Device[i].DEVICE, 0x18, 0))));
		printf("0x%x\n", ((_PCI_CONFIG_getWord(_PCI_DEVICE_LIST.Device[i].BUS, _PCI_DEVICE_LIST.Device[i].DEVICE, 0x1C, 2) << 8) + (_PCI_CONFIG_getWord(_PCI_DEVICE_LIST.Device[i].BUS, _PCI_DEVICE_LIST.Device[i].DEVICE, 0x1C, 0))));
		printf("0x%x\n", ((_PCI_CONFIG_getWord(_PCI_DEVICE_LIST.Device[i].BUS, _PCI_DEVICE_LIST.Device[i].DEVICE, 0x20, 2) << 8) + (_PCI_CONFIG_getWord(_PCI_DEVICE_LIST.Device[i].BUS, _PCI_DEVICE_LIST.Device[i].DEVICE, 0x20, 0))));
**/
	}
#endif
	return;
}