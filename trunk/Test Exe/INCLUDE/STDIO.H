/*
./INCLUDE/STDIO.H
*/

#include <FORMATTING.H>
#include <stdarg.h>

#ifndef				STDIO_H_INCLUDED
	#define				STDIO_H_INCLUDED

	void disable_scroll(void);
	void disable_curmov(void);
	void enable_scroll(void);
	void enable_curmov(void);
	
	void puts(char *message);
	void printf(char *message, ...);
	void printfc(int cSet, char *message, ...);
	void putch(char charactor);
	void setPos(uint8_t x, uint8_t y);
	void getPos(uint8_t *x, uint8_t *y);
	void cls();
	void setColor(int cSet);
	int getColor(void);	
	
	uint8_t inb (uint16_t _port);
	void outb (uint16_t _port, uint8_t _data);
	uint16_t inw (uint16_t _port);
	void outw (uint16_t _port, uint16_t _data);
	uint32_t inl (uint16_t _port);
	void outl (uint16_t _port, uint32_t _data);
	void INT(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);
	
#endif
