/*
./LIBSRC/MEM/PHYSICAL.C
*/

#include <MEM/PHYSICAL.H>
#include <IRQ.H>			//for iError();
#include <STDIO.H>

//IN MEM.ASM:
extern void enable_paging(void);
extern void disable_paging(void);
extern physical_addr pmmngr_get_PDBR(void);

//Private FUNCTIONS
uint32_t probeRAM(void);
void mmap_update(void);
void initMMAP(void);

uint32_t probeRAM() //in KB
{
	uint32_t out = 0x7FF; 									//we will start at 2MB and assume you have 2MB of RAM Minimal 1024 bytes = 1kb 1024 kb = 1MB
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
	return out;
}

void mmap_set (int bit)
{
	_mmngr_memory_map[bit / 32] |= (1 << (bit % 32));
}

void mmap_unset (int bit)
{
	_mmngr_memory_map[bit / 32] &= ~ (1 << (bit % 32));
}

bool mmap_test (int bit)
{
	return _mmngr_memory_map[bit / 32] &  (1 << (bit % 32));
}

int mmap_first_free ()
{
	for (uint32_t i=0; i< _mmngr_max_blocks /32; i++)
		if (_mmngr_memory_map[i] != 0xffffffff)
			for (int j=0; j<32; j++) {				// test each bit in the dword
				int bit = 1 << j;
				if (! (_mmngr_memory_map[i] & bit) )
					return i*4*8+j;
			}
	return -1;
}

int mmap_first_free_s (size_t size)
{
	if (size==0)
		return -1;
	if (size==1)
		return mmap_first_free ();
	for (uint32_t i=0; i<_mmngr_max_blocks /32; i++)
		if (_mmngr_memory_map[i] != 0xffffffff)
			for (int j=0; j<32; j++) {	// test each bit in the dword
				int bit = 1<<j;
				if (! (_mmngr_memory_map[i] & bit) ) {
					int startingBit = i*32;
					startingBit+=bit;		//get the free bit in the dword at index i
					uint32_t free=0; //loop through each bit to see if its enough space
					for (uint32_t count=0; count<=size;count++) {
						if (! mmap_test (startingBit+count) )
							free++;	// this bit is clear (free frame)
						if (free==size)
							return i*4*8+j; //free count==size needed; return index
					}
				}
			}
	return -1;
}

void mmap_update()
{
	_mmngr_free_blocks = 0;
	_mmngr_used_blocks = 0;
	for (uint32_t i=0; i < _mmngr_max_blocks / PMMNGR_BLOCKS_PER_BYTE; i++) {
		if (_mmngr_memory_map[i] != 0xffffffff)
			for (int j=0; j<PMMNGR_BLOCKS_PER_BYTE; j++) {				// test each bit in the dword
				int bit = 1 << j;
				if (! (_mmngr_memory_map[i] & bit) )
					_mmngr_free_blocks++;
				else
					_mmngr_used_blocks++;
			}
		else
			_mmngr_used_blocks += 32;
	}
}

void initMMAP()
{
	memset (_mmngr_memory_map, 0x0, _mmngr_max_blocks / PMMNGR_BLOCKS_PER_BYTE );
	//if (i == 0 || i == 1 || (i >=512 && i < 1024) || (i >= 15360 && i <= 16384) || i >= 3145728) [KBs]
	mmap_set(0);			//BIOS and IVT plus I want 0 as Error
	mmap_set(7);			//Bootsector
	for(uint32_t i = 128; (i < 256) && (i <= _mmngr_max_blocks); i++)
		mmap_set(i);
	for(uint32_t i = 3840; (i < 4096) && (i <= _mmngr_max_blocks); i++)
		mmap_set(i);
	for(uint32_t i = 786432; (i < _mmngr_max_blocks); i++)
		mmap_set(i);
	for(uint32_t i = 256; (i < 512) && (i <= _mmngr_max_blocks); i++)												//KERNEL
		mmap_set(i);
	for(uint32_t i = 512; (i < _mmngr_max_blocks / PMMNGR_BLOCKS_PER_BYTE) && (i <= _mmngr_max_blocks); i++)		//MMAP
		mmap_set(i);
	mmap_update();
}
/*if (i == 0 || i == 1 || (i >=512 && i < 1024) || (i >= 15360 && i <= 16384) || i >= 3145728)
			mapInfo = 0xFC;	//Memory Holes
		else if (i >= 1024 && i < 2048)
			mapInfo = 0x1;	//KERNEL (1MB LIMIT)
		else if (i >= 2048 && i < mapLoc )
			mapInfo = 0x2;	//Memory Map
		else
			mapInfo = 0x0;*/
void initPHYSMEM()
{
	*RAM = 0x0;
	TotalRAM = probeRAM();
	if (TotalRAM < 0x1000)	//4MB ([4KB]KB)
		iError("NOT ENOUGH RAM");
	_mmngr_memory_size	=	TotalRAM;
	_mmngr_memory_map	=	(uint32_t*) MMAP_LOCATION;
	_mmngr_max_blocks	=	(TotalRAM*1024) / PMMNGR_BLOCK_SIZE;
	_mmngr_used_blocks	=	_mmngr_max_blocks;
	_mmngr_free_blocks	=	0;
	initMMAP();
}

void	pmmngr_init (size_t memSize, physical_addr bitmap) {

	_mmngr_memory_size	=	memSize;
	_mmngr_memory_map	=	(uint32_t*) bitmap;
	_mmngr_max_blocks	=	(pmmngr_get_memory_size()*1024) / PMMNGR_BLOCK_SIZE;
	_mmngr_used_blocks	=	pmmngr_get_block_count();

	//! By default, all of memory is in use
	memset (_mmngr_memory_map, 0xf, pmmngr_get_block_count() / PMMNGR_BLOCKS_PER_BYTE );

}

void	pmmngr_init_region (physical_addr base, size_t size) {

	int align = base / PMMNGR_BLOCK_SIZE;
	int blocks = size / PMMNGR_BLOCK_SIZE;

	for (; blocks>0; blocks--) {
		mmap_unset (align++);
		_mmngr_used_blocks--;
	}

	mmap_set (0);	//first block is always set. This insures allocs cant be 0
}

void	pmmngr_deinit_region (physical_addr base, size_t size) {

	int align = base / PMMNGR_BLOCK_SIZE;
	int blocks = size / PMMNGR_BLOCK_SIZE;

	for (; blocks>0; blocks--) {
		mmap_set (align++);
		_mmngr_used_blocks++;
	}

	mmap_set (0);	//first block is always set. This insures allocs cant be 0
}

void*	pmmngr_alloc_block () {

	if (pmmngr_get_free_block_count() <= 0)
		return 0;	//out of memory

	int frame = mmap_first_free ();

	if (frame == -1)
		return 0;	//out of memory

	mmap_set (frame);

	physical_addr addr = frame * PMMNGR_BLOCK_SIZE;
	_mmngr_used_blocks++;

	return (void*)addr;
}

void	pmmngr_free_block (void* p) {

	physical_addr addr = (physical_addr)p;
	int frame = addr / PMMNGR_BLOCK_SIZE;

	mmap_unset (frame);

	_mmngr_used_blocks--;
}

void*	pmmngr_alloc_blocks (size_t size) {

	if (pmmngr_get_free_block_count() <= size)
		return 0;	//not enough space

	int frame = mmap_first_free_s (size);

	if (frame == -1)
		return 0;	//not enough space

	for (uint32_t i=0; i<size; i++)
		mmap_set (frame+i);

	physical_addr addr = frame * PMMNGR_BLOCK_SIZE;
	_mmngr_used_blocks+=size;

	return (void*)addr;
}

void	pmmngr_free_blocks (void* p, size_t size) {

	physical_addr addr = (physical_addr)p;
	int frame = addr / PMMNGR_BLOCK_SIZE;

	for (uint32_t i=0; i<size; i++)
		mmap_unset (frame+i);

	_mmngr_used_blocks-=size;
}

size_t	pmmngr_get_memory_size () {

	return _mmngr_memory_size;
}

uint32_t pmmngr_get_block_count () {

	return _mmngr_max_blocks;
}

uint32_t pmmngr_get_use_block_count () {

	return _mmngr_used_blocks;
}

uint32_t pmmngr_get_free_block_count () {

	return _mmngr_max_blocks - _mmngr_used_blocks;
}

uint32_t pmmngr_get_block_size () {

	return PMMNGR_BLOCK_SIZE;
}

void	pmmngr_paging_enable (bool b)
{
	if(b)
		enable_paging();
	else
		disable_paging();
}

bool pmmngr_is_paging () {
/*unsigned int cr0;
asm volatile("mov %%cr0, %0": "=b"(cr0));
cr0 |= 0x80000000;
asm volatile("mov %0, %%cr0":: "b"(cr0));*/
	uint32_t res=0;

	
	return (res & 0x80000000) ? false : true;
}

void pmmngr_load_PDBR (physical_addr addr)
{
	
}

