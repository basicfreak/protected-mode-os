/*
./DRIVERSRC/SYSTEM/MEM/VIRTUAL.C
*/

#include "VIRTUAL.H"
#include "PAGEFAULT.H"
#include "../../HARDWARE/RS232.H"
#include <STDIO.H>
//IN START.ASM:
extern void enable_paging(void);
extern void disable_paging(void);

pdir_p KERNEL_PAGE_DIR;

Page_Entry Create_Page(paddr physical, bool user, bool write)
{
	Page_Entry e = I86_PTE_PRESENT;
	if (user)
		e |= I86_PTE_USER;
	if (write)
		e |= I86_PTE_WRITABLE;
	e = (e & ~I86_PTE_FRAME) | physical;
	return e;
}

void Add_Table_To_Dir(uint32_t index, ptbl_p table, pdir_p dir, bool user, bool write)
{
	uint32_t *entry = (uint32_t*) &dir->table [index];
	*entry = I86_PDE_PRESENT;
	if (user)
		*entry |= I86_PDE_USER;
	if (write)
		*entry |= I86_PDE_WRITABLE;
	*entry = (*entry & ~I86_PDE_FRAME) | (uint32_t)table;
}

void setPaging(bool enable)
{
	if (enable)
		enable_paging();
	else
		disable_paging();
}

void setPageDirectroy(pdir_p dir)
{
	__asm__ __volatile__ ("mov %0, %%cr3":: "b"(dir));
}

pdir_t *Kernel_Page_Dir_Address()
{
	return (pdir_t *) KERNEL_PAGE_DIR;
}

void _VMem_init()
{
	// Allocate Kernel page directory
	KERNEL_PAGE_DIR = (pdir_p) calloc(3);
	if (!KERNEL_PAGE_DIR)
		return;

	// Map Table With All RAM in system identically mapped
	uint32_t frame = 0, virt = 0;//, x = 0;
	for(int x = 0; x < (_mmngr_memory_size / 0x1000); x++) {	
		// Allocate a page table
		ptbl_p table = (ptbl_p) calloc(1);
		if (!table)
			return;
		for (int i=0; i<PAGES_PER_TABLE; i++, frame+=PAGE_SIZE, virt+=PAGE_SIZE) {
			// Create Page
			Page_Entry page = Create_Page(frame, true, true);
			// Add Page To Table
			table->page[i] = page;
		}
		// Add Table To Directory
		Add_Table_To_Dir(x, table, KERNEL_PAGE_DIR, true, true);
	}
	
	// Setup Page Fault Handler
	_pageFault_init();
	
	// switch to our page directory
	setPageDirectroy (KERNEL_PAGE_DIR);

	// enable paging
	setPaging (true);
}


/*typedef struct Process_Return {
		pdir_p dir;
		uint32_t location;
		uint32_t size;
	} p_ret;*/
pdir_p Create_Process_Directory(uint32_t process_size_bytes, uint32_t *physical_location)
{
	pdir_p ret = (pdir_p) calloc(3);
	*physical_location = (uint32_t) calloc(process_size_bytes / PAGE_SIZE);
	// Setup directory...
	// Stack
	paddr *stack = calloc(2);		//8KB stack for process ends at 0xC0000000 Virtual
	ptbl_p tablestack = (ptbl_p) calloc(1);								//C0000000
	Page_Entry stackpage0 = Create_Page((uint32_t)stack, true, true);	//BFFFE000
	Page_Entry stackpage1 = Create_Page(((uint32_t)stack)+0x1000, true, true);
	tablestack->page[1022] = stackpage0;
	tablestack->page[1023] = stackpage1;
	Add_Table_To_Dir(0x2FF, tablestack, ret, true, true);
	// Process Code/Data/Bss (its the file from disk hopefully)
	uint32_t frame = *physical_location;
	uint32_t virt = 0x800000;
	for(int x = 2; x <= (process_size_bytes / PAGE_SIZE)+1; x++)	// process offset = 0x800000 / 6MB
	{
		// Allocate a page table
		ptbl_p table = (ptbl_p) calloc(1);
		if (!table)
			return 0;
		for (int i = 0; i<PAGES_PER_TABLE; i++, frame+=PAGE_SIZE, virt+=PAGE_SIZE) {
			// Create Page
			Page_Entry page = Create_Page(frame, true, true);
			// Add Page To Table
			table->page[i] = page;
		}
		// Add Table To Directory
		Add_Table_To_Dir(x, table, ret, true, true);
	}
	Add_Table_To_Dir(0, KERNEL_PAGE_DIR->table[0], ret, false, false);
	Add_Table_To_Dir(1, KERNEL_PAGE_DIR->table[1], ret, false, false);
	return ret;
}