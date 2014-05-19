/*
./DRIVERSRC/LPT.H
*/

#ifndef				_LPT_H_INCLUDED
	#define				_LPT_H_INCLUDED
	
	#include <FORMATTING.H>
	
	void LPT1_OUT(uint8_t data);
	uint8_t LPT1_IN();
	void LPT2_OUT(uint8_t data);
	uint8_t LPT2_IN();
	void LPT3_OUT(uint8_t data);
	uint8_t LPT3_IN();
	
#endif