/*
./INCLUDE/MEM/VIRTUAL.H
*/

#include <FORMATTING.H>

#ifndef				VIRTUAL_H_INCLUDED
	#define				VIRTUAL_H_INCLUDED
	
	#include <MEM/PHYSICAL.H>
	#include <MEM/PAGETABLE.H>
	#include <MEM/PAGEDIR.H>
	
	typedef uint32_t virtual_addr;
	 
	#define PAGES_PER_TABLE 1024
	#define PAGES_PER_DIR	1024
	#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
	#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
	#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)
	#define PTABLE_ADDR_SPACE_SIZE 0x400000
	#define DTABLE_ADDR_SPACE_SIZE 0x100000000
	#define PAGE_SIZE 4096
	 
	struct ptable {
		pt_entry m_entries[PAGES_PER_TABLE];
	};
	struct pdirectory {
		pd_entry m_entries[PAGES_PER_DIR];
	};
	
		
#endif