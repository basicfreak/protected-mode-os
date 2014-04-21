/*
./LIBSRC/STDIO.C
*/

#include <STDIO.H>
#include <STRING.H>
#include <KEYBOARD.H>

/*
---------------------------------------------------------------------------------------
KEYBOARD INPUT
---------------------------------------------------------------------------------------
*/
char getch(char *msg)
{
	keyboard_buffer = '\0';
	puts(msg);
	while (keyboard_buffer == '\0');
	char out = keyboard_buffer;
	putch(out);
	keyboard_buffer = '\0';
	return out;
}

void gets(char out[1024], char *msg)
{
	keyboard_buffer = '\0';
	puts(msg);
	bool i = true;
	while (i) {
		if (keyboard_buffer != '\0')
		{
			switch (keyboard_buffer)
			{
				case	'\n':
					puts("\n");
					i = false;
					break;
				case	'\b':
					if (arrayRemove(out, 1))
						puts("\b \b");
					break;
				default:
					if (arrayAppend(out, keyboard_buffer)) {
						putch(keyboard_buffer);
						movcur(curX, curY);
					}
					break;
			}
			keyboard_buffer = '\0';
		}
	}
}

/*
---------------------------------------------------------------------------------------
VIDEO
---------------------------------------------------------------------------------------
*/

void initVideo()
{
	color = 0x07;
	curX = 0;
	curY = 0;
	COLS = 80;
	ROWS = 25;
	movcur_disable = 0;
	cls();
	return;
}

void puts(char *message)
{
	while (*message!=0)
	{
		putch (*message);
		*message++;
	}
	movcur (curX, curY);
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
						int c = va_arg (ap, int);
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
	movcur			(curX, curY);
	return;
}

printfc(int cSet, char *message, ...) {
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
						int c = va_arg (ap, int);
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
	movcur			(curX, curY);
	setColor(tempColor);
	return;
}

void putch(char charactor)
{
/*
\b	backspace
\t	horizontal tab
\n	newline
\v	vertical tab
\f	form-feed
\r	carriage return
\\	\
\?	?
\'	'
\"	"
*/
	
	char *VIDMEM = (char *) 0xb8000;
	unsigned int memLoc = ((curY * COLS) + curX) *2;
	switch ( charactor )
	{
		case '\n':
			curX = 0;
			curY ++;
			break;
		case '\r':
			curX = 0;
			break;
		case '\b':
			curX --;
			break;
		case '\t':
			curX += 4;
			break;
		case '\v':
			curY += 4;
			break;
		case '\f':
			curY ++;
			break;
		case '\\':
			VIDMEM[ memLoc ] = (char) 0x5C;
			VIDMEM[ memLoc + 1 ] = color;
			curX ++;
			break;
		case '\?':
			VIDMEM[ memLoc ] = (char) 0x3F;
			VIDMEM[ memLoc + 1 ] = color;
			curX ++;
			break;
		case '\'':
			VIDMEM[ memLoc ] = (char) 0x27;
			VIDMEM[ memLoc + 1 ] = color;
			curX ++;
			break;
		case '\"':
			VIDMEM[ memLoc ] = (char) 0x22;
			VIDMEM[ memLoc + 1 ] = color;
			curX ++;
			break;
		
		default:
			VIDMEM[ memLoc ] = charactor;
			VIDMEM[ memLoc + 1 ] = color;
			curX ++;
			break;
	}
	if (curX >= COLS)
	{
		curX = 0;
		curY ++;
	}
	while (curY >= ROWS)
	{
		scrollScreen ();
	}
	return;
}
void movcur(int x, int y)
{
	if ( movcur_disable == 0 )
	{
		unsigned short position =(y*COLS) + x;
		outb(0x3D4, 0x0F);
		outb(0x3D5, (unsigned char)(position&0xFF));
		outb(0x3D4, 0x0E);
		outb(0x3D5, (unsigned char )((position>>8)&0xFF));
		curX = x;
		curY = y;
	}
	return;
}

void scrollScreen()
{
	unsigned short *VIDMEM = (unsigned short *) 0xb8000;
	int i;
	for (i=0; i < (ROWS-1)*COLS; i++)
		VIDMEM[i] = VIDMEM[i+80];
	for (i=(ROWS-1)*COLS; i < ROWS*COLS; i++)
		//VIDMEM[i] = color | ' ';
		VIDMEM[i] = 0x20 | (color << 8);
	curY--;
	return;
}

void cls()
{
	unsigned blank;
    int i;
	unsigned short *VIDMEM = (unsigned short *) 0xb8000;
	blank = 0x20 | (color << 8);
	for (i = 0; i < ROWS; i++)
        memsetw (VIDMEM + i * COLS, blank, COLS);
	curX = 0;
	curY = 0;
    movcur (0, 0);
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

unsigned char inb (unsigned short _port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return			rv;
}

void outb (unsigned short _port, unsigned char _data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

void *memcpy(void *dest, const void *src, size_t count)
{
	const char *sp = (const char *)src;
	char *dp = (char *)dest;
	for (; count != 0; count--)
		*dp++ = *sp++;
	return dest;
}

void *memset(void *dest, char val, size_t count)
{
	char *temp = (char *)dest;
	for ( ; count != 0; count--)
		*temp++ = val;
	return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count)
{
	unsigned short *temp = (unsigned short *)dest;
	for ( ; count != 0; count--)
		*temp++ = val;
	return dest;
}