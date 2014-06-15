/*
./TEST\ EXE/main.c
*/
#include <STRING.H>
#include <STDIO.H>
#include <FORMATTING.H>

extern void GetTime(void);

extern volatile uint8_t secs;
extern volatile uint8_t mins;
extern volatile uint8_t hours;

uint8_t X;
uint8_t Y;
uint8_t Color;

#define BCD(x) ( (x & 0xF0) >> 1) + ( (x & 0xF0) >> 3) + (x & 0xf)

int main()
{
	disable_scroll();
	disable_curmov();
	setColor(0x1F);
	while(1) {
		X = 0x8;
		Y = 0;
		setPos(X, Y);
		/*GetTime();		
		putch(hours);
		puts(" : ");
		putch(mins);
		puts(" : ");
		putch(secs);
		puts("puts test");*/
		putch('T');
		putch('e');
		putch('s');
		putch('t');
		putch('.');
		putch('.');
		putch('.');/*
		getPos(&X, &Y);
		Y++;
		setPos(X, Y);
		puts("...");*/
	}
	return 0;
}