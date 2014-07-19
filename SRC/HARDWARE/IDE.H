/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *					HARDWARE/IDE.H					 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef IDE_H_INCLUDED
	#define IDE_H_INCLUDED

	#include <FORMATTING.H>

	void _IDE_init(void);
	error _IDE_IO(uint8_t drive, bool write, uint64_t start, uint8_t count, void* Buffer);

#endif