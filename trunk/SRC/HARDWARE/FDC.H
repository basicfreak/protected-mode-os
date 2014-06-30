/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *					HARDWARE/FDC.H					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FDC_H_INCLUDED
	#define FDC_H_INCLUDED

	#include <FORMATTING.H>

	void _FDC_init(void);
	error _FDC_IO(bool write, int start, uint8_t count, void *buffer);

#endif