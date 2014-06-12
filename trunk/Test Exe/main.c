/*
./TEST\ EXE/main.c
*/

#include "stdarg.h"

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned uint32_t;
typedef	int bool;
typedef int size_t;

#define CMOS 0x70
#define CMOSdata 0x71
#define HOURS 0x04
#define MINS 0x02
#define SECS 0x00
#define BCD(x) ( (x & 0xF0) >> 1) + ( (x & 0xF0) >> 3) + (x & 0xf)

void INT(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
	__asm__ __volatile__ ( "int $0x31" : : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi) );
}

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

void io_wait()
{
	__asm__ __volatile__ ( "jmp 1f\n\t"
						   "1:jmp 2f\n\t"
						   "2:" );
}

uint8_t readCMOS(uint16_t reg)
{
	io_wait();
	outb(CMOS, reg);
	io_wait();
	return inb(CMOSdata);
}

uint8_t BCDtoDEC(uint8_t BCD)
{
	return (uint8_t) ( (BCD & 0xF0) >> 1) + ( (BCD & 0xF0) >> 3) + (BCD & 0xf) ;
}

int strlen(const char *str)
{
	size_t ret = 0;
	while (str[ret++]);
	return ret;
}

char *strcpy(char *s1, const char *s2)
{
	char *s1_p = s1;
	while ( (*s1++ = *s2++) );
	return s1_p;
}

char tbuf[32];
char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void itoa(unsigned i,unsigned base,char* buf)
{
   int pos = 0;
   int opos = 0;
   int top = 0;

   if (i == 0 || base > 16)
   {
      buf[0] = '0';
      buf[1] = '\0';
      return;
   }

   while (i != 0)
   {
      tbuf[pos] = bchars[i % base];
      pos++;
      i /= base;
   }
   top=pos--;
   for (opos=0; opos<top; pos--,opos++)
   {
      buf[opos] = tbuf[pos];
   }
   buf[opos] = 0;
   return;
}

void itoa_s(int i,unsigned base,char* buf)
{
   if (base > 16) return;
   if (i < 0)
   {
      *buf++ = '-';
      i *= -1;
   }
   itoa(i,base,buf);
   return;
}

int stringf(char *str, const char *format, ...)
{
	va_list ap;
	va_start(ap,format);
	size_t loc=0;
	size_t i;
	for (i=0 ; i<=strlen(format);i++, loc++)
	{
		switch (format[i]) {
			case '%':
				switch (format[i+1])
				{
					/*** characters ***/
					case 'c': {
						char c = va_arg (ap, char);
						str[loc] = c;
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
						strcpy (&str[loc], s);
						loc+= strlen(s) - 2;
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
						strcpy (&str[loc], s);
						i++;		// go to next character
						loc+=strlen(s) - 2;
						break;
					}
					/*** strings ***/
					case 's':
					{
						char *c = va_arg (ap, char*);
						char s[32]={0};
						strcpy (s,(const char*)c);						
						strcpy (&str[loc], s);
						i++;		// go to next character
						loc+=strlen(s) - 2;
						break;
					}
				}
				break;
			default:
				str[loc] = format[i];
				break;
		}
	}
	return i;
}
/* INT 0x31: Standard Video Outputs
REGS:
AL = Command (X | X | X | PUTS | SCROLLDIS | POS | MOVECURDIS | CLS)
AH = Color (0xBF)
BL = X pos
BH = Y pos
esi = string pointer
*/
int main()
{ 
	for(;;) {
		char *TIME = "          ";
		stringf(TIME, "%i:%i:%i", BCD(readCMOS(HOURS)), BCD(readCMOS(MINS)), BCD(readCMOS(SECS)));
		INT(0x1F1E, 0x0046, 0, 0, (uint32_t) TIME, 0);
	}
	return 0;
}