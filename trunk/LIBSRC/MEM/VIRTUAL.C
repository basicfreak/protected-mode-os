/*
./LIBSRC/MEM/VIRTUAL.C
*/

#include <STDIO.H>
#include <MEM/VIRTUAL.H>

struct pdirectory* _cur_directory=0;
physical_addr _cur_pdbr=0;

void MmMapPage (void* phys, void* virt)
{
	struct pdirectory* pageDirectory = vmmngr_get_directory ();

   // get page table
   pd_entry* e = &pageDirectory->m_entries [PAGE_DIRECTORY_INDEX ((uint32_t) virt) ];
   if ( (*e & I86_PTE_PRESENT) != I86_PTE_PRESENT) {

      // page table not present, allocate it
      struct ptable* table = (struct ptable*) pmmngr_alloc_block ();
      if (!table)
         return;

      // clear page table
      memset (table, 0, 4096);

      // create a new entry
      pd_entry* entry =
         &pageDirectory->m_entries [PAGE_DIRECTORY_INDEX ( (uint32_t) virt) ];

      // map in the table (Can also just do *entry |= 3) to enable these bits
      pd_entry_add_attrib (entry, I86_PDE_PRESENT);
      pd_entry_add_attrib (entry, I86_PDE_WRITABLE);
      pd_entry_set_frame (entry, (physical_addr)table);
   }

   // get table
   struct ptable* table = (struct ptable*) PAGE_GET_PHYSICAL_ADDRESS ( e );

   // get page
   pt_entry* page = &table->m_entries [ PAGE_TABLE_INDEX ( (uint32_t) virt) ];

   // map it in (Can also do (*page |= 3 to enable..)
   pt_entry_set_frame ( page, (physical_addr) phys);
   pt_entry_add_attrib ( page, I86_PTE_PRESENT);
}

void vmmngr_initialize ()
{
	// allocate default page table
   struct ptable* table = (struct ptable*) pmmngr_alloc_block ();
   if (!table)
      return;

   // allocates 3gb page table
   struct ptable* table2 = (struct ptable*) pmmngr_alloc_block ();
   if (!table2)
      return;

   // clear page table
   memset (table, 0, 4096);

   // 1st 4mb are idenitity mapped
   for (int i=0, frame=0x0, virt=0x00000000; i<1024; i++, frame+=4096, virt+=4096) {

      // create a new page
      pt_entry page=0;
      pt_entry_add_attrib (&page, I86_PTE_PRESENT);
      pt_entry_set_frame (&page, frame);

      // ...and add it to the page table
      table2->m_entries [PAGE_TABLE_INDEX (virt) ] = page;
   }

   
   // create default directory table
   struct pdirectory*   dir = (struct pdirectory*) pmmngr_alloc_blocks (3);
   if (!dir)
      return;

  // clear directory table and set it as current
  memset (dir, 0, sizeof (struct pdirectory));

   pd_entry* entry2 = &dir->m_entries [PAGE_DIRECTORY_INDEX (0x00000000) ];
   pd_entry_add_attrib (entry2, I86_PDE_PRESENT);
   pd_entry_add_attrib (entry2, I86_PDE_WRITABLE);
   pd_entry_set_frame (entry2, (physical_addr)table2);

   // store current PDBR
   _cur_pdbr = (physical_addr) &dir->m_entries;

   // switch to our page directory
   vmmngr_switch_pdirectory (dir);

   // enable paging
   pmmngr_paging_enable (true);
}

bool vmmngr_alloc_page (pt_entry* e)
{
	void* p = pmmngr_alloc_block ();
	if (!p)
		return false;

	//map it to the page
	pt_entry_set_frame (e, (physical_addr)p);
	pt_entry_add_attrib (e, I86_PTE_PRESENT);
	//doesn't set WRITE flag...

	return true;
}

void vmmngr_free_page (pt_entry* e)
{
	void* p = (void*)pt_entry_pfn (*e);
	if (p)
		pmmngr_free_block (p);

	pt_entry_del_attrib (e, I86_PTE_PRESENT);
}

bool vmmngr_switch_pdirectory (struct pdirectory* dir)
{
	if (!dir)
		return false;

	_cur_directory = dir;
	pmmngr_load_PDBR (_cur_pdbr);
	return true;
}

struct pdirectory* vmmngr_get_directory ()
{
	return _cur_directory;
}

void vmmngr_flush_tlb_entry (virtual_addr addr)
{
	/*#ifdef _MSC_VER
	_asm {
		cli
		invlpg	addr
		sti
	}
	#endif*/
}

void vmmngr_ptable_clear (struct ptable* p)
{
	
}

uint32_t vmmngr_ptable_virt_to_index (virtual_addr addr)
{
	
}

pt_entry* vmmngr_ptable_lookup_entry (struct ptable* p,virtual_addr addr)
{
	if (p)
		return &p->m_entries[ PAGE_TABLE_INDEX (addr) ];
	return 0;
}

uint32_t vmmngr_pdirectory_virt_to_index (virtual_addr addr)
{
	
}

void vmmngr_pdirectory_clear (struct pdirectory* dir)
{
	
}

pd_entry* vmmngr_pdirectory_lookup_entry (struct pdirectory* p, virtual_addr addr)
{
	if (p)
		return &p->m_entries[ PAGE_TABLE_INDEX (addr) ];
	return 0;
}