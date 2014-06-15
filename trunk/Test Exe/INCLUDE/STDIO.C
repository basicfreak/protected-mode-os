/*
./LIBSRC/STDIO.C
*/

#include <STDIO.H>
#include <STRING.H>

/*
---------------------------------------------------------------------------------------
VIDEO
---------------------------------------------------------------------------------------
*/
uint8_t curX = 0;
uint8_t curY = 0;
uint8_t COLS = 80;
uint8_t ROWS = 25;
uint8_t color = 0x07;
bool movcur_disable = false;
bool scroll_disable = false;

void disable_scroll()
{
	scroll_disable = true;
}

void disable_curmov()
{
	movcur_disable = true;
}

void enable_scroll()
{
	scroll_disable = false;
}

void enable_curmov()
{
	movcur_disable = false;
}

void puts(char *message)
{
	while (*message!=0)
	{
		putch (*message);
		*message++;
	}
	return;
}

void printf(char *message, ...)
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
						putch(c);
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
						puts(s);
						i++;		// go to next character
						break;
					}
					/*** display in hex ***/
					case 'X':
					case 'x':
					{
						uint32_t c = va_arg (ap, uint32_t);
						char s[32]={0};
						itoa_s (c,16,s);
						puts(s);
						i++;		// go to next character
						break;
					}
					/*** strings ***/
					case 's':
					{
						char *c = va_arg (ap, char*);
						char s[32]={0};
						strcpy (s,(const char*)c);						
						puts(s);
						i++;		// go to next character
						break;
					}
				}
				break;
			default:
				putch(message[i]);
				break;
		}
	}
	return;
}
void printfc(int cSet, char *message, ...) {
	int tempColor = getColor();
	setColor(cSet);
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
						putch(c);
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
						puts(s);
						i++;		// go to next character
						break;
					}
					/*** display in hex ***/
					case 'X':
					case 'x':
					{
						uint32_t c = va_arg (ap, uint32_t);
						char s[32]={0};
						itoa_s (c,16,s);
						puts(s);
						i++;		// go to next character
						break;
					}
					/*** strings ***/
					case 's':
					{
						char *c = va_arg (ap, char*);
						char s[32]={0};
						strcpy (s,(const char*)c);						
						puts(s);
						i++;		// go to next character
						break;
					}
				}
				break;
			default:
				putch(message[i]);
				break;
		}
	}
	setColor(tempColor);
	return;
}

void putch(char charactor)
{
	uint8_t command = 0x24;
	if (movcur_disable)
		command |= 0x02;
	if (scroll_disable)
		command |= 0x08;
	INT((uint32_t)((color << 8) | command), (uint32_t) ((curY << 8) | curX), 0, 0, charactor, 0);
	curX++;
	if (curX >= COLS) {
		curY++;
		curX = curX - COLS;
	}
}

void setPos(uint8_t x, uint8_t y)
{
	curX = x;
	curY = y;
}

void getPos(uint8_t *x, uint8_t *y)
{
	*x = curX;
	*y = curY;
}

void cls()
{
	INT( (uint32_t)((color << 8) | 0x1), 0, 0, 0, 0, 0);
	return;
}

void setColor(int cSet)
{
	color = cSet;
	return;
}

int getColor()
{
	return color;
}



/*
---------------------------------------------------------------------------------------
STANDARD IO
---------------------------------------------------------------------------------------
*/

uint8_t inb (uint16_t _port)
{
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outb (uint16_t _port, uint8_t _data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

uint16_t inw (uint16_t _port)
{
    uint16_t rv;
    __asm__ __volatile__ ("inw %1, %0" : "=aN" (rv) : "dN" (_port));
    return rv;
}

void outw (uint16_t _port, uint16_t _data)
{
    __asm__ __volatile__ ("outw %1, %0" : : "dN" (_port), "aN" (_data));
}

uint32_t inl (uint16_t _port)
{
    uint32_t rv;
    __asm__ __volatile__ ("inl %1, %0" : "=aL" (rv) : "dN" (_port));
    return rv;
}

void outl (uint16_t _port, uint32_t _data)
{
    __asm__ __volatile__ ("outl %1, %0" : : "dN" (_port), "aL" (_data));
}
/*
INT 0x31: Standard Video Outputs
REGS:
AL = Command (X | X | PUTCH | PUTS | SCROLLDIS | POS | MOVECURDIS | CLS)
AH = Color (0xBF)
BL = X pos
BH = Y pos
esi = string pointer or char if putch
INT(0x1F1E, 0x0046, 0, 0, (uint32_t) TIME, 0);
*/
void INT(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
	__asm__ __volatile__ ( "int $0x31" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
}
