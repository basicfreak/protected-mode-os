/*
./LIBSRC/PS2.C

IRQ12
*/

#include "MOUSE.H"
#include "../../SYSTEM/CPU/IRQ.H"
#include <STDIO.H>

//Mouse.inc by SANiK
//License: Use as you wish, except to cause damage
uint8_t mouse_cycle=0;     //unsigned char
int8_t mouse_byte[3];    //signed char
uint16_t SCREENSIZE_X = 80;
uint16_t SCREENSIZE_Y = 25;
uint32_t _PS2_PREX = 0;
uint32_t _PS2_PREY = 0;
uint8_t PREVCHAR;
uint8_t PREVCOLOR;

void PS2_updateCursur()
{
	/**
	curX
	curY 
	**/
	unsigned int tempX = curX;
	unsigned int tempY = curY;
	movcur_disable++;
	scroll_disable++;
	
	curX = _PS2_PREX;
	curY = _PS2_PREY;
	printfc(PREVCOLOR, "%c", PREVCHAR);
	
	PREVCHAR = getCharOf(_PS2_X, _PS2_Y);
	PREVCOLOR = getColorOf(_PS2_X, _PS2_Y);
	
	curX = _PS2_X;
	curY = _PS2_Y;
	int tcolor = 0xF0;
	if (_PS2_0)
		tcolor -= 0x10;
	if (_PS2_1)
		tcolor -= 0x20;
	if (_PS2_2)
		tcolor -= 0x30;
	
	printfc(tcolor, "%c", PREVCHAR);
	
	curX = tempX;
	curY = tempY;
	movcur_disable--;
	scroll_disable--;
}

void MouseEvent()
{
	/**
	uint32_t		_PS2_X;
	uint32_t		_PS2_Y;
	bool		_PS2_0;
	bool		_PS2_1;
	bool		_PS2_2;
	
				0x	80 | 40 | 20 | 10 | 8 | 04 | 02 | 01
	mouse_byte[0]	YO | XO | YS | XS | 1 | B2 | B1 | B0
	mouse_byte[1]	X AXIS MOVEMENT VALUE
	mouse_byte[2]	Y AXIS MOVEMENT VALUE
	**/
	uint8_t tempX = (uint8_t) mouse_byte[1];
	uint8_t tempY = (uint8_t) mouse_byte[2];
	_PS2_PREX = _PS2_X;
	_PS2_PREY = _PS2_Y;
	
	if (mouse_byte[0] & 0x1)
		_PS2_0 = TRUE;
	else
		_PS2_0 = FALSE;
	if (mouse_byte[0] & 0x2)
		_PS2_1 = TRUE;
	else
		_PS2_1 = FALSE;
	if (mouse_byte[0] & 0x4)
		_PS2_2 = TRUE;
	else
		_PS2_2 = FALSE;
	
	_PS2_X += (tempX - ((mouse_byte[0] << 4) & 0x100)) / 6;
	_PS2_Y -= (tempY - ((mouse_byte[0] << 3) & 0x100)) / 6;
		
	if (_PS2_X >= SCREENSIZE_X)
		_PS2_X = SCREENSIZE_X - 1;
	if (_PS2_Y >= SCREENSIZE_Y)
		_PS2_Y = SCREENSIZE_Y - 1;
	if (_PS2_X < 0)
		_PS2_X = 0;
	if (_PS2_Y < 0)
		_PS2_Y = 0;
		
	PS2_updateCursur();
}

void PS2_setScreenSize(uint16_t X, uint16_t Y)
{
	SCREENSIZE_X = X;
	SCREENSIZE_Y = Y;
}

//Mouse functions
void mouse_handler(struct regs *a_r) //struct regs *a_r (not used but just there)
{
	switch(mouse_cycle)
	{
		case 0:
			mouse_byte[0]=inb(0x60);
			mouse_cycle++;
			break;
		case 1:
			mouse_byte[1]=inb(0x60);
			mouse_cycle++;
			break;
		case 2:
			mouse_byte[2]=inb(0x60);
			mouse_cycle=0;
			MouseEvent();
			break;
	}
}

void mouse_wait(uint8_t a_type)
{
	uint32_t _time_out=100000;
	if(a_type==0) {
		while(_time_out--)
			if((inb(0x64) & 1)==1)
				return;
		return;
	} else {
		while(_time_out--)
			if((inb(0x64) & 2)==0)
				return;
		return;
	}
}

void mouse_write(uint8_t a_write)
{
	//Wait to be able to send a command
	mouse_wait(1);
	//Tell the mouse we are sending a command
	outb(0x64, 0xD4);
	//Wait for the final part
	mouse_wait(1);
	//Finally write
	outb(0x60, a_write);
}

uint8_t mouse_read()
{
	//Gets response from mouse
	mouse_wait(0); 
	return inb(0x60);
}

void init_PS2()
{
	_PS2_X = 0;
	_PS2_Y = 0;
	_PS2_0 = FALSE;
	_PS2_1 = FALSE;
	_PS2_2 = FALSE;
	PREVCHAR = getCharOf(0, 0);
	PREVCOLOR = getColorOf(0, 0);
	uint8_t _status;  //unsigned char

	//Enable the auxiliary mouse device
	mouse_wait(1);
	outb(0x64, 0xA8);

	//Enable the interrupts
	mouse_wait(1);
	outb(0x64, 0x20);
	mouse_wait(0);
	_status=(inb(0x60) | 2);
	mouse_wait(1);
	outb(0x64, 0x60);
	mouse_wait(1);
	outb(0x60, _status);

	//Tell the mouse to use default settings
	mouse_write(0xF6);
	mouse_read();  //Acknowledge

	//Enable the mouse
	mouse_write(0xF4);
	mouse_read();  //Acknowledge

	//Setup the mouse handler
	irq_install_handler(12, mouse_handler);
}