/*
./DRIVERSRC/SYSTEM/MEM/PHYSICAL.C
*/

#include "PHYSICAL.H"
#include <STDIO.H>

#define DEBUG

uint32_t *Mem_Map;
uint32_t Total_Ram;

uint32_t probeRAM() //in KB
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 13) probeRAM() = ");
#endif
	uint32_t out = 0xFFF; 									//we will start at 4MB and assume you have 4MB of RAM Minimal 1024 bytes = 1kb 1024 kb = 1MB
	bool issue = false;
	while (!issue) {
		if (out < 0x4000)
			out += 0x1;									// add 1kb to memory location if at less than 16MB
		else if (out < 0x20000)
			out += 0x400;								// else add 1mb to memory location if less than 128MB
		else if (out < 0x100000)
			out += 0x4000;								// else add 16MB to location if less than 1024MB\1GB
		else
			out += 0x20000;								// else add 128MB to location
			
		char temp = RAM[out*0x400];							//Save the byte that was here to temp
		/*	Do a bit-shift to find usable RAM	*
		*		  01010101 -> 10101010			*/
		RAM[out*0x400] = 0x55; //01010101
		if (RAM[out*0x400] != 0x55) issue = true;
		RAM[out*0x400] = 0xAA; //10101010
		if (RAM[out*0x400] != 0xAA) issue = true;
		
		RAM[out*0x400] = temp;								//Restore the byte that was here from temp
	}
#ifdef DEBUG
	txf(1, "0x%x\n\r", out);
#endif
	return out;
}

bool mmap_test (int bit)
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 46) mmap_test(0x%x) = 0x%x\n\r", bit, (Mem_Map[bit / 32] &  (1 << (bit % 32))) );
#endif
	return Mem_Map[bit / 32] &  (1 << (bit % 32));
}

void set_used(uint32_t page)
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 54) set_used(0x%x)\n\r", page);
#endif
	Mem_Map[page / 32] |= (1 << (page % 32));
	_mmngr_free_blocks--;
	_mmngr_used_blocks++;
}

int mmap_first_free ()
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 64) mmap_first_free () = ");
#endif
	for (uint32_t i=0; i<(_mmngr_max_blocks/32); i++)
		if (Mem_Map[i] != 0xffffffff)
			for (int j=0; j<32; j++) {				// test each bit in the dword
				int bit = 1 << j;
				if (! (Mem_Map[i] & bit) ) {
#ifdef DEBUG
	txf(1, "0x%x\n\r", (i*32)+j);
#endif
					return (i*32)+j;
				}
			}
#ifdef DEBUG
	txf(1, "ERROR\n\r");
#endif
	return -1;
}

int mmap_first_free_s (size_t size)
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 86) mmap_first_free_s (0x%x) = ", (uint32_t) size);
#endif
	if (size==0) {
#ifdef DEBUG
	txf(1, "ERROR\n\r");
#endif
		return -1;
	}
	if (size==1)
		return mmap_first_free ();
	for (uint32_t i=0; i<_mmngr_max_blocks/32; i++)
		if (Mem_Map[i] != 0xffffffff)
			for (int j=0; j<32; j++) {	// test each bit in the dword
				int bit = 1<<j;
				if (! (Mem_Map[i] & bit) ) {
					int startingBit = (i*32)+bit;		//get the free bit in the dword at index i
					uint32_t free=0; //loop through each bit to see if its enough space
					for (uint32_t count=0; (count<=size) && (startingBit+count < _mmngr_max_blocks);count++) {	// Make sure we do not overflow?
						if (! mmap_test (startingBit+count) )
							free++;	// this bit is clear (free frame)
						else {							// Lets break out of the loop if they are not in order and test next MemMap Section
							count = size+1;
							free = 0;
						}
						if (free==size) {
#ifdef DEBUG
	txf(1, "0x%x\n\r", (i*32)+j);
#endif
							return i*4*8+j; //free count==size needed; return index
						}
					}
				}
			}
#ifdef DEBUG
	txf(1, "ERROR\n\r");
#endif
	return -1;
}

void *malloc(uint32_t pages)	// Just Allocate
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 125) malloc(0x%x) = ", pages);
#endif
	if (_mmngr_free_blocks <= pages) {
#ifdef DEBUG
	txf(1, "ERROR\n\r");
#endif
		return 0;
	}	//not enough space

	int frame = mmap_first_free_s (pages);

	if (frame == -1) {
#ifdef DEBUG
	txf(1, "ERROR\n\r");
#endif
		return 0;
	}	//not enough space

	for (uint32_t i=0; i<pages; i++)
		set_used (frame+i);

	uint32_t addr = frame * PMMNGR_BLOCK_SIZE;
#ifdef DEBUG
	txf(1, "0x%x\n\r", addr);
#endif
	return (void*)addr;
}

void *calloc(uint32_t pages)	// Clear and Allocate
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 156) calloc(0x%x)\n\r", pages);
#endif
	uint32_t* location = (uint32_t*) malloc(pages);
	memset (location, 0, (pages * 0x1000));
	return (void *) location;
}

void free(uint32_t page)		// Deallocate Page
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 166) free(0x%x)\n\r", page);
#endif
	Mem_Map[page / 32] &= ~ (1 << (page % 32));
	_mmngr_free_blocks++;
	_mmngr_used_blocks--;
}

void _PMem_init()
{
#ifdef DEBUG
	txf(1, "\n\r(PHYSICAL.C:Line 176) _PMem_init()\n\r");
#endif
	RAM = 0;
	Mem_Map = (uint32_t *)MMAP_LOCATION;
	memset(&Mem_Map[0], 0xFF, MMAP_SIZE);	// Allocate all RAM as Used
	Total_Ram = probeRAM();
	memset(&Mem_Map[0x20], 0, (Total_Ram - 0x1000));  // Free available RAM after 4MB..?
	_mmngr_memory_size	=	Total_Ram;
	_mmngr_memory_map	=	(uint32_t*) MMAP_LOCATION;
	_mmngr_max_blocks	=	(Total_Ram*0x400) / PMMNGR_BLOCK_SIZE;
	_mmngr_used_blocks	=	0x400;
	_mmngr_free_blocks	=	(Total_Ram / 4);
#ifdef DEBUG
	txf(1, "\tTotal RAM:\t%iKB\n\r", _mmngr_memory_size);
	txf(1, "\tFree RAM:\t%iKB\n\r", _mmngr_memory_size-(_mmngr_used_blocks*4));
	txf(1, "\tUsed RAM:\t%iKB\n\r", _mmngr_used_blocks*4);
#endif
}