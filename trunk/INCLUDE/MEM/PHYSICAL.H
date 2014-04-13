/*
./INCLUDE/MEM/PHYSICAL.H
*/

#include <FORMATTING.H>

#ifndef				PHYSICAL_H_INCLUDED
	#define				PHYSICAL_H_INCLUDED
	
	#define PMMNGR_BLOCKS_PER_BYTE	8
	#define PMMNGR_BLOCK_SIZE		4096
	#define PMMNGR_BLOCK_ALIGN		PMMNGR_BLOCK_SIZE
	#define MMAP_LOCATION			0x200000
	
	typedef uint32_t physical_addr;
	
	uint32_t	_mmngr_memory_size;
	uint32_t	_mmngr_used_blocks;
	uint32_t	_mmngr_max_blocks;
	uint32_t*	_mmngr_memory_map;
	uint32_t	_mmngr_free_blocks;
	
	unsigned char *RAM;
	uint32_t TotalRAM;
	
	void mmap_set (int bit);
	void mmap_unset (int bit);
	bool mmap_test (int bit);
	int mmap_first_free (void);
	int mmap_first_free_s (size_t size);
	void initPHYSMEM(void);
	void	pmmngr_init (size_t, physical_addr);
	void	pmmngr_init_region (physical_addr, size_t);
	void	pmmngr_deinit_region (physical_addr base, size_t);
	void*	pmmngr_alloc_block (void);
	void	pmmngr_free_block (void*);
	void*	pmmngr_alloc_blocks (size_t);
	void	pmmngr_free_blocks (void*, size_t);
	size_t pmmngr_get_memory_size (void);
	uint32_t pmmngr_get_use_block_count (void);
	uint32_t pmmngr_get_free_block_count (void);
	uint32_t pmmngr_get_block_count (void);
	uint32_t pmmngr_get_block_size (void);
	
#endif