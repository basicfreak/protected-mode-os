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
PCIDEVLIST00h _PCI_DEVICE_LIST00h;
PCIDEVLIST01h _PCI_DEVICE_LIST01h;

enum _PCI_CONFIGURATION_PORTS {
	_PCI_CONFIG_ADDRESS = 0xCF8,
	_PCI_CONFIG_DATA = 0xCFC
};

uint16_t _PCI_CONFIG_getWord(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
uint32_t _PCI_CONFIG_getLong(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
uint16_t _PCI_getVendorID(uint8_t bus, uint8_t slot, uint8_t function);
uint16_t _PCI_getDeviceID(uint8_t bus, uint8_t slot, uint8_t function);
uint16_t _PCI_getCommand(uint8_t bus, uint8_t slot, uint8_t function);
uint16_t _PCI_getStatus(uint8_t bus, uint8_t slot, uint8_t function);
uint8_t _PCI_getRevisionID(uint8_t bus, uint8_t slot, uint8_t function);
uint8_t _PCI_getProgIF(uint8_t bus, uint8_t slot, uint8_t function);
uint8_t _PCI_getSubclass(uint8_t bus, uint8_t slot, uint8_t function);
uint8_t _PCI_getClass(uint8_t bus, uint8_t slot, uint8_t function);
uint8_t _PCI_getCacheSize(uint8_t bus, uint8_t slot, uint8_t function);
uint8_t _PCI_getLatencyTimer(uint8_t bus, uint8_t slot, uint8_t function);
uint8_t _PCI_getHeaderType(uint8_t bus, uint8_t slot, uint8_t function);
uint8_t _PCI_getBIST(uint8_t bus, uint8_t slot, uint8_t function);
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

uint32_t _PCI_CONFIG_getLong(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset)
{
	uint32_t add;
	uint32_t Lbus = (uint32_t) bus;
	uint32_t Lslot = (uint32_t) slot;
	uint32_t Lfunction = (uint32_t) function;
	uint32_t Loffset = (uint32_t) offset;
	
	add = (uint32_t) ((Lbus << 16) | (Lslot << 11) | (Lfunction << 8) | ( Loffset & 0xFC) | ((uint32_t) 0x80000000));
	
	outl(_PCI_CONFIG_ADDRESS, add);
	return inl(_PCI_CONFIG_DATA);
}

uint16_t _PCI_getVendorID(uint8_t bus, uint8_t slot, uint8_t function)
{
	return _PCI_CONFIG_getWord(bus, slot, function, 0);
}

uint16_t _PCI_getDeviceID(uint8_t bus, uint8_t slot, uint8_t function)
{
	return _PCI_CONFIG_getWord(bus, slot, function, 2);
}

uint16_t _PCI_getCommand(uint8_t bus, uint8_t slot, uint8_t function)
{
	return _PCI_CONFIG_getWord(bus, slot, function, 4);
}

uint16_t _PCI_getStatus(uint8_t bus, uint8_t slot, uint8_t function)
{
	return _PCI_CONFIG_getWord(bus, slot, function, 6);
}

uint8_t _PCI_getRevisionID(uint8_t bus, uint8_t slot, uint8_t function)
{
	return (uint8_t) (_PCI_CONFIG_getWord(bus, slot, function, 8) & 0xFF);
}

uint8_t _PCI_getProgIF(uint8_t bus, uint8_t slot, uint8_t function)
{
	return (uint8_t) (_PCI_CONFIG_getWord(bus, slot, function, 8) >> 8) & 0xFF;
}

uint8_t _PCI_getSubclass(uint8_t bus, uint8_t slot, uint8_t function)
{
	return (uint8_t) (_PCI_CONFIG_getWord(bus, slot, function, 0xA) & 0xFF);
}

uint8_t _PCI_getClass(uint8_t bus, uint8_t slot, uint8_t function)
{
	return (uint8_t) (_PCI_CONFIG_getWord(bus, slot, function, 0xA) >> 8) & 0xFF; 
}

uint8_t _PCI_getCacheSize(uint8_t bus, uint8_t slot, uint8_t function)
{
	return (uint8_t) (_PCI_CONFIG_getWord(bus, slot, function, 0xC) & 0xFF);
}

uint8_t _PCI_getLatencyTimer(uint8_t bus, uint8_t slot, uint8_t function)
{
	return (uint8_t) (_PCI_CONFIG_getWord(bus, slot, function, 0xC) >> 8) & 0xFF;
}

uint8_t _PCI_getHeaderType(uint8_t bus, uint8_t slot, uint8_t function)
{
	return (uint8_t) (_PCI_CONFIG_getWord(bus, slot, function, 0xE) & 0xFF);
}

uint8_t _PCI_getBIST(uint8_t bus, uint8_t slot, uint8_t function)
{
	return (uint8_t) (_PCI_CONFIG_getWord(bus, slot, function, 0xE) >> 8) & 0xFF; 
}

void _PCI_scanDevices()
{
	uint16_t list = 0;
	for (uint8_t bus = 0; bus < 0xFF; bus++)
		for (uint8_t slot = 0; slot < 0x20; slot++) {
			for (uint8_t function = 0; function < 8; function ++) {
				if (_PCI_getVendorID(bus, slot, function) != 0xFFFF) {		// Does device exist?
					_PCI_DEVICE_LIST.Device[list].BUS = bus;
					_PCI_DEVICE_LIST.Device[list].DEVICE = slot;
					_PCI_DEVICE_LIST.Device[list].FUNCTION = function;
					_PCI_DEVICE_LIST.Device[list].VendorID = _PCI_getVendorID(bus, slot, function);
					_PCI_DEVICE_LIST.Device[list].DeviceID = _PCI_getDeviceID(bus, slot, function);
					_PCI_DEVICE_LIST.Device[list].Command = _PCI_getCommand(bus, slot, function);
					_PCI_DEVICE_LIST.Device[list].Status = _PCI_getStatus(bus, slot, function);
					_PCI_DEVICE_LIST.Device[list].RevisionID = _PCI_getRevisionID(bus, slot, function);
					_PCI_DEVICE_LIST.Device[list].ProgIF = _PCI_getProgIF(bus, slot, function);
					_PCI_DEVICE_LIST.Device[list].Subclass = _PCI_getSubclass(bus, slot, function);
					_PCI_DEVICE_LIST.Device[list].Class = _PCI_getClass(bus, slot, function);
					_PCI_DEVICE_LIST.Device[list].CacheSize = _PCI_getCacheSize(bus, slot, function);
					_PCI_DEVICE_LIST.Device[list].LatencyTimer = _PCI_getLatencyTimer(bus, slot, function);
					_PCI_DEVICE_LIST.Device[list].HeaderType = _PCI_getHeaderType(bus, slot, function);
					_PCI_DEVICE_LIST.Device[list].BIST = _PCI_getBIST(bus, slot, function);
					if((_PCI_DEVICE_LIST.Device[list].HeaderType & 0x7F) == 0x00) {
						_PCI_DEVICE_LIST.Device[list].HeadID = _PCI_DEVICES00h;
						_PCI_DEVICE_LIST00h.Device[_PCI_DEVICES00h].BAR[0] = _PCI_CONFIG_getLong(bus, slot, function, 0x10);
						_PCI_DEVICE_LIST00h.Device[_PCI_DEVICES00h].BAR[1] = _PCI_CONFIG_getLong(bus, slot, function, 0x14);
						_PCI_DEVICE_LIST00h.Device[_PCI_DEVICES00h].BAR[2] = _PCI_CONFIG_getLong(bus, slot, function, 0x18);
						_PCI_DEVICE_LIST00h.Device[_PCI_DEVICES00h].BAR[3] = _PCI_CONFIG_getLong(bus, slot, function, 0x1C);
						_PCI_DEVICE_LIST00h.Device[_PCI_DEVICES00h].BAR[4] = _PCI_CONFIG_getLong(bus, slot, function, 0x20);
						_PCI_DEVICE_LIST00h.Device[_PCI_DEVICES00h].BAR[5] = _PCI_CONFIG_getLong(bus, slot, function, 0x24);
						_PCI_DEVICE_LIST00h.Device[_PCI_DEVICES00h].CIS = _PCI_CONFIG_getLong(bus, slot, function, 0x28);
						_PCI_DEVICE_LIST00h.Device[_PCI_DEVICES00h].SubSysVendorID = _PCI_CONFIG_getWord(bus, slot, function, 0x2C);
						_PCI_DEVICE_LIST00h.Device[_PCI_DEVICES00h].SybSysID = _PCI_CONFIG_getWord(bus, slot, function, 0x2E);
						_PCI_DEVICE_LIST00h.Device[_PCI_DEVICES00h].ExROM = _PCI_CONFIG_getLong(bus, slot, function, 0x30);
						_PCI_DEVICE_LIST00h.Device[_PCI_DEVICES00h].CapabilitiesPointer = (uint8_t) (_PCI_CONFIG_getWord(bus, slot, function, 0x34) & 0xFF);
						_PCI_DEVICE_LIST00h.Device[_PCI_DEVICES00h].IntLine = (uint8_t) (_PCI_CONFIG_getWord(bus, slot, function, 0x3C) & 0xFF);
						_PCI_DEVICE_LIST00h.Device[_PCI_DEVICES00h].IntPIN = (uint8_t) ((_PCI_CONFIG_getWord(bus, slot, function, 0x3C) >> 8) & 0xFF);
						_PCI_DEVICE_LIST00h.Device[_PCI_DEVICES00h].MinGrant = (uint8_t) (_PCI_CONFIG_getWord(bus, slot, function, 0x3E) & 0xFF);
						_PCI_DEVICE_LIST00h.Device[_PCI_DEVICES00h].MaxLatency = (uint8_t) ((_PCI_CONFIG_getWord(bus, slot, function, 0x3E) >> 8) & 0xFF);
						_PCI_DEVICES00h++;
					} else if ((_PCI_DEVICE_LIST.Device[list].HeaderType & 0x7F) == 0x01) {
						_PCI_DEVICE_LIST.Device[list].HeadID = _PCI_DEVICES01h;

						_PCI_DEVICES01h++;
					}
					if (function == 0 && !(_PCI_DEVICE_LIST.Device[list].HeaderType & 0x80)) function=0xF0;
					list++;
				}
			}
		}
	_PCI_DEVICES = list;
}

/**
PUBLIC FUNCTIONS
**/

void _PCI_init()
{
	_PCI_DEVICES = 0;
	_PCI_DEVICES00h = 0;
	_PCI_DEVICES01h = 0;
	_PCI_scanDevices();
#ifdef DEBUG
	printf("\nPCI\nBUS_DEV_FUNC\tVendor\tDevice\tClass\tSubClass\tHead Type\n");
	for (int i = 0; i < _PCI_DEVICES; i++) {
		printf("PCI_%i_%i_%i\t\t0x%x\t0x%x\t0x%x\t0x%x\t\t0x%x\n", _PCI_DEVICE_LIST.Device[i].BUS, _PCI_DEVICE_LIST.Device[i].DEVICE, _PCI_DEVICE_LIST.Device[i].FUNCTION,
						_PCI_DEVICE_LIST.Device[i].VendorID, _PCI_DEVICE_LIST.Device[i].DeviceID, _PCI_DEVICE_LIST.Device[i].Class,
														_PCI_DEVICE_LIST.Device[i].Subclass, _PCI_DEVICE_LIST.Device[i].HeaderType);
		if(_PCI_DEVICE_LIST.Device[i].Class == 0x01)
			printf("\tBAR0 = 0x%x\tBAR1 = 0x%x\tBAR2 = 0x%x\t BAR3 = 0x%x\n\tBAR4 = 0x%x\tBAR5 = 0x%x\tINTPIN = %i, INTLine = %i\n",
				_PCI_DEVICE_LIST00h.Device[_PCI_DEVICE_LIST.Device[i].HeadID].BAR[0], _PCI_DEVICE_LIST00h.Device[_PCI_DEVICE_LIST.Device[i].HeadID].BAR[1], _PCI_DEVICE_LIST00h.Device[_PCI_DEVICE_LIST.Device[i].HeadID].BAR[2],
				_PCI_DEVICE_LIST00h.Device[_PCI_DEVICE_LIST.Device[i].HeadID].BAR[3], _PCI_DEVICE_LIST00h.Device[_PCI_DEVICE_LIST.Device[i].HeadID].BAR[4], _PCI_DEVICE_LIST00h.Device[_PCI_DEVICE_LIST.Device[i].HeadID].BAR[5],
				_PCI_DEVICE_LIST00h.Device[_PCI_DEVICE_LIST.Device[i].HeadID].IntPIN, _PCI_DEVICE_LIST00h.Device[_PCI_DEVICE_LIST.Device[i].HeadID].IntLine);
	}
	for(;;);
#endif
}