/*
./DRIVERSRC/RS232.C
*/

#include "RS232.H"
#include "BIOS.H"
#include <STDIO.H>
#include <STRING.H>
#include "../SYSTEM/CPU/IRQ.H"

uint16_t COMS[5] = {0, 0, 0, 0, 0};
uint16_t COM1, COM2, COM3, COM4;
uint8_t COM1_ID, COM2_ID, COM3_ID, COM4_ID;

enum COM_OFFSETS {
	THR = 0,	// Transmitter Holding Buffer
	RBR = 0,	// Receiver Buffer
	DLL = 0,	// Divisor Latch Low Byte
	IER = 1,	// Interrupt Enable Register
	DLH = 1,	// Divisor Latch High Byte
	IIR = 2,	// Interrupt Identification Register
	FCR = 2,	// FIFO Control Register
	LCR = 3,	// Line Control Register
	MCR = 4,	// Modem Control Register
	LSR = 5,	// Line Status Register
	MSR = 6,	// Modem Status Register
	SR = 7		// Scratch Register
};

enum COM_IDS {
	_ERROR = 0,
	_16750 = 1,
	_16550A = 2,
	_16550 = 3,
	_16450 = 4,
	_8250 = 5
};

enum IER_BREAKDOWN {
	IER_RX = 1,
	IER_TX = 2,
	IER_LineStatus = 4,
	IER_ModemStatus = 8,
	IER_SleepMode = 0x10,
	IER_LowPowerMode = 0x20
};

enum IIR_BREAKDOWN {
	IIR_Pending = 1,
	IIR_ModemStatus = 0,
	IIR_TXEmpty = 2,
	IIR_RXData = 4,
	IIR_LineStatus = 6,
	IIR_TimeOut = 0xC,
	IIR_64BitFIFO = 0x20,
	IIR_NoFIFO = 0,
	IIR_NoFIFOFunctional = 0x80,
	IIR_FIFO = 0xC0
};

enum FCR_BREAKDOWN {	
	FCR_Enable = 1,			//00000001
	FCR_ClearRX = 2,		//00000010
	FCR_ClearTX = 4,		//00000100
	FCR_DMA = 8,			//00001000
	FCR_64 = 0x20,			//00100000
	FCR_16_1 = 0,			//00000000
	FCR_64_1 = 0,
	FCR_16_4 = 0x40,		//01000000
	FCR_64_16 = 0x40,
	FCR_16_8 = 0x80,		//10000000
	FCR_64_32 = 0x80,
	FCR_16_14 = 0xC0,		//11000000
	FCR_64_56 = 0xC0
};

enum LCR_BREAKDOWN {
	LCR_DLAB = 0x80,
	LCR_Break_Enable = 0x40,
	LCR_NONE = 0,
	LCR_ODD = 8,
	LCR_EVEN = 0x18,
	LCR_MARK = 0x28,
	LCR_SPACE = 0x38,
	LCR_1Stop = 0,
	LCR_2Stop = 4,
	LCR_5Bits = 0,
	LCR_6Bits = 1,
	LCR_7Bits = 2,
	LCR_8Bits = 3
};

enum MCR_BREAKDOWN {
	MCR_DataTermRdy = 1,
	MCR_RequestSend = 2,
	MCR_AuxOut1 = 4,
	MCR_AuxOut2 = 8,
	MCR_Loop = 0x10,
	MCR_Autoflow = 0x20
};

enum LSR_BREAKDOWN {
	LSR_DataRdy = 1,
	LSR_Overrun = 2,
	LSR_Parity = 4,
	LSR_Framing = 8,
	LSR_Break = 0x10,
	LSR_EmptyTX = 0x20,
	LSR_EmptyRX = 0x40,
	LSR_FIFO = 0x80
};

enum MSR_BREAKDOWN {
	MSR_DeltaC2S = 1,
	MSR_DeltaDataSetRdy = 2,
	MSR_TrailRingIndicator = 4,
	MSR_DeltaCarrierDetect = 8,
	MSR_C2S = 0x10,
	MSR_DataSetRdy = 0x20,
	MSR_RingIndicator = 0x40,
	MSR_CarrierDetect = 0x80
};

void write(uint16_t COM, uint8_t OFF, uint8_t DATA)
{
	outb((uint16_t)(COM+(uint16_t)OFF), DATA);
}

uint8_t read(uint16_t COM, uint8_t OFF)
{
	return (uint8_t) inb((uint16_t)(COM+(uint16_t)OFF));
}

void IER_handle(uint16_t COM, bool RX, bool TX, bool LS, bool MS)
{
	uint8_t temp = 0;
	if (RX)
		temp |= IER_RX;
	if (TX)
		temp |= IER_TX;
	if (LS)
		temp |= IER_LineStatus;
	if (MS)
		temp |= IER_ModemStatus;
	write(COM, IER, temp);
}

int8_t INT_ID(uint16_t COM)
{
	int8_t ret = -(10);
	uint8_t temp = read(COM, IIR);
	if ((temp & 1)) {
		ret = (int8_t) temp;
	}
	return ret;
}

uint8_t COM_ID(uint16_t COM)
{
	uint8_t temp;
	
	write(COM, FCR, (FCR_16_14 | FCR_ClearTX | FCR_ClearRX | FCR_Enable));
	temp = read(COM, IIR);
	if (temp & 0x40) {
		if (temp & 0x80) {
			if (temp & 0x20)
				return _16750;
			else
				return _16550A;
		} else
			return _16550;
	} else {
		write(COM, SR, 0xA5);
		if (read(COM, SR) == 0xA5)
			return _16450;
		else
			return _8250;
	}
	return _ERROR;
}

void setBaudRate(uint16_t COM, uint32_t RATE)
{
	uint8_t LO_RATE, HI_RATE;
	
	LO_RATE = (uint8_t) ((115200 / RATE) & 0xFF);
	HI_RATE = (uint8_t) ((115200 / RATE) >> 8);
	
	IER_handle(COM, false, false, false, false);							//Disable IRQ
	write(COM, FCR, (FCR_ClearRX | FCR_ClearTX));							//Disable FIFO
	write(COM, LCR, LCR_DLAB);												//DLAB
	write(COM, DLL, LO_RATE);												//Data Rate Low
	write(COM, DLH, HI_RATE);												//Data Rate High
	write(COM, LCR, LCR_8Bits);												//Line Control
	write(COM, FCR, (FCR_Enable | FCR_ClearRX | FCR_ClearTX | FCR_16_14));	//Enable FIFO
	IER_handle(COM, true, true, true, true);								//Enable IRQ
}

void tx(uint16_t COM, uint8_t DATA)
{
	while (! (read(COM, LSR) & 0x20) );
	write(COM, THR, DATA);
}

uint8_t rx(uint16_t COM)
{
	while (! (read(COM, LSR) & 1) );
	return read(COM, RBR);
}

void _RS232_handler(regs *r)
{
	puts("RS232 IRQ:\n");
	
}

void _RS232_init()
{
	COM1 = _BIOS_COM(1);
	COM2 = _BIOS_COM(2);
	COM3 = _BIOS_COM(3);
	COM4 = _BIOS_COM(4);
	//Set Data Rates
	if(COM1) {
		COM1_ID = COM_ID(COM1);
		setBaudRate(COM1, 19200);
	} else COM1_ID = _ERROR;
	if(COM2) {
		COM2_ID = COM_ID(COM2);
		setBaudRate(COM2, 19200);
	} else COM2_ID = _ERROR;
	if(COM3) {
		COM3_ID = COM_ID(COM3);
		setBaudRate(COM3, 19200);
	} else COM3_ID = _ERROR;
	if(COM4) {
		COM4_ID = COM_ID(COM4);
		setBaudRate(COM4, 19200);
	} else COM4_ID = _ERROR;
	//Enable Interrupts
	if(COM1 || COM3)
		install_IR(4, _RS232_handler);
	if(COM2 || COM4)
		install_IR(3, _RS232_handler);
	COMS[1] = COM1;
	COMS[2] = COM2;
	COMS[3] = COM3;
	COMS[4] = COM4;
	
	tx(COM1, 'T');
	tx(COM1, 'e');
	tx(COM1, 's');
	tx(COM1, 't');
}
void txc(uint8_t COM, uint8_t data)
{
	tx(COMS[COM], data);
}
void txs(uint8_t COM, char *message)
{
	while (*message!=0)
	{
		tx (COMS[COM], *message);
		*message++;
	}
}
void txf(uint8_t COM, char *message, ...)
{
	va_list ap;
	va_start(ap, message);
	size_t i;
	for (i=0 ; i<strlen(message)-1;i++)
	{
		switch (message[i]) {
			case '%':
				switch (message[i+1])
				{
					/*** characters ***/
					case 'c': {
						char c = va_arg (ap, char);
						txc(COM, c);
						i++;
						break;
					}
					/*** integers ***/
					case 'd':
					case 'i':
					{
						int c = va_arg (ap, int);
						char s[32]={0};
						itoa_s (c, 10, s);
						txs(COM, s);
						i++;		// go to next character
						break;
					}
					/*** display in hex ***/
					case 'X':
					case 'x':
					{
						int c = va_arg (ap, int);
						char s[32]={0};
						itoa_s (c,16,s);
						txs(COM, s);
						i++;		// go to next character
						break;
					}
					/*** strings ***/
					case 's':
					{
						char *c = va_arg (ap, char*);
						char s[32]={0};
						strcpy (s,(const char*)c);						
						txs(COM, s);
						i++;		// go to next character
						break;
					}
				}
				break;
			default:
				txc(COM, message[i]);
				break;
		}
	}
	return;
}