/*
./DRIVERSRC/RS232.C
*/

#include "RS232.H"
#include "BIOS.H"
#include <STDIO.H>
#include "../SYSTEM/CPU/IRQ.H"

uint16_t COM1, COM2, COM3, COM4;

enum COM_OFFSETS {
	DATA_REG = 0,
	INT_EN_REG = 1,
	LSB = 0,
	MSB = 1,
	INT_ID_FIFO = 2,
	LINE_CTR_REG = 3,
	MODEM_CTR_REG = 4,
	LINE_STAT_REG = 5,
	MODEM_STAT_REG = 6,
	SCRATCH_REG = 7
};

void write(uint16_t COM, uint8_t OFF, uint8_t DATA)
{
	outb((uint16_t)(COM+(uint16_t)OFF), DATA);
}

uint8_t read(uint16_t COM, uint8_t OFF)
{
	return (uint8_t) inb((uint16_t)(COM+(uint16_t)OFF));
}

void setBaudRate(uint16_t COM, uint32_t RATE)
{
	uint8_t LO_RATE, HI_RATE;
	
	LO_RATE = (uint8_t) ((115200 / RATE) & 0xFF);
	HI_RATE = (uint8_t) ((115200 / RATE) >> 8);
	
	write(COM, INT_EN_REG, 0);		//Disable INT
	write(COM, LINE_CTR_REG, 0x80);	//DLAB
	write(COM, LSB, LO_RATE);
	write(COM, MSB, HI_RATE);
	write(COM, LINE_CTR_REG, 3);
	write(COM, INT_ID_FIFO, 0xC7);	//Enable FIFO
	write(COM, MODEM_CTR_REG, 0x0B);//IRQ EN
	write(COM, INT_EN_REG, 0xF);
}

void tx(uint16_t COM, uint8_t DATA)
{
	while (! (read(COM, LINE_STAT_REG) & 0x20) );
	write(COM, DATA_REG, DATA);
}

uint8_t rx(uint16_t COM)
{
	while (! (read(COM, LINE_STAT_REG) & 1) );
	return read(COM, DATA_REG);
}

int8_t INT_ID(uint16_t COM)
{
	int8_t ret = -(10);
	/*
			Interrupt Identification Register (IIR)
			Bit	Notes
			7 and 6	Bit 7	Bit 6	
			0	0	No FIFO on chip
			0	1	Reserved condition
			1	0	FIFO enabled, but not functioning
			1	1	FIFO enabled
			5	64 Byte FIFO Enabled (16750 only)
			4	Reserved
			3, 2 and 1	Bit 3	Bit 2	Bit 1		Reset Method
			0	0	0	Modem Status Interrupt	Reading Modem Status Register(MSR)
			0	0	1	Transmitter Holding Register Empty Interrupt	Reading Interrupt Identification Register(IIR) or
			Writing to Transmit Holding Buffer(THR)
			0	1	0	Received Data Available Interrupt	Reading Receive Buffer Register(RBR)
			0	1	1	Receiver Line Status Interrupt	Reading Line Status Register(LSR)
			1	0	0	Reserved	N/A
			1	0	1	Reserved	N/A
			1	1	0	Time-out Interrupt Pending (16550 & later)	Reading Receive Buffer Register(RBR)
			1	1	1	Reserved	N/A
			0	Interrupt Pending Flag
	*/
	
	return ret;
}

void _RS232_handler(struct regs *r)
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
	if(COM1)
		setBaudRate(COM1, 19200);
	if(COM2)
		setBaudRate(COM2, 19200);
	if(COM3)
		setBaudRate(COM3, 19200);
	if(COM4)
		setBaudRate(COM4, 19200);
	//Enable Interrupts
	if(COM1 || COM3)
		irq_install_handler(4, _RS232_handler);
	if(COM2 || COM4)
		irq_install_handler(3, _RS232_handler);
}