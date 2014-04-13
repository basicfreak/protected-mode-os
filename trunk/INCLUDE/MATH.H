/*
./INCLUDE/MATH.H
*/

#include <FORMATTING.H>

#ifndef				MATH_H_INCLUDED
	#define				MATH_H_INCLUDED
	
	int exponent(int base, unsigned int exp);
	unsigned int textTOhex(char* s);
	char* hexTOtext(int in);
	char* intTOchar(int in);
	unsigned int charTOint(char* in);
	uint64_t BIN2INT(unsigned char* in);
	bool isEven(uint64_t in);
	
#endif