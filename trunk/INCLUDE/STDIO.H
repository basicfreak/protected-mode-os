/*
./INCLUDE/STDIO.H
*/

#include <FORMATTING.H>
#include <stdarg.h>

#ifndef				STDIO_H_INCLUDED
	#define				STDIO_H_INCLUDED

	unsigned int 		curX;
	unsigned int 		curY;
	int					COLS;
	int					ROWS;
	int					color;
	int					movcur_disable;
	
	char getch(char *msg);
	void gets(char out[1024], char *msg);
	
	void				initVideo(void);
	void				puts(char *message);
	void				printf(char *message, ...);
	void				putch(char charactor);
	void				movcur(int x, int y);
	void				scrollScreen();
	void				cls();
	void				setColor(int cSet);
	int					getColor(void);
	
	
	unsigned char 		inb (unsigned short _port);
	void 				outb (unsigned short _port, unsigned char _data);
	void				*memcpy(void *dest, const void *src, size_t count);
	void				*memset(void *dest, char val, size_t count);
	unsigned short		*memsetw(unsigned short *dest, unsigned short val, size_t count);
	
	
#endif
