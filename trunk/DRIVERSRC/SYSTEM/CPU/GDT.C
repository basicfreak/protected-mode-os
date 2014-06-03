/*
./LIBSRC/GRT.C
*/

#include "GDT.H"

void install_ring(uint8_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_middle = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;
	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].granularity = ((limit >> 16) & 0x0F);
	gdt[num].granularity |= (gran & 0xF0);
	gdt[num].access = access;
}

void _GDT_init()
{
/* Setup the GDT pointer and limit */
    gp.limit = (sizeof(struct gdt_entry) * 10) - 1;
    gp.base = (uint32_t) &gdt;
// NULL
    install_ring (0, 0, 0, 0, 0);
	
// KERNEL MODE RING 0

    install_ring (1, 0, 0xFFFFFFFF,
			_MEMORY | _CODEDATA | _EXEC_CODE | _READWRITE,
			_4K | _32BIT);

    install_ring (2, 0, 0xFFFFFFFF,
			_MEMORY | _CODEDATA | _READWRITE,
			_4K | _32BIT);
			
// USER MODE RING 3
	install_ring (3, 0, 0xFFFFFFFF,
			_READWRITE | _EXEC_CODE | _CODEDATA | _MEMORY | _DPL,
			_4K | _32BIT);
			
	install_ring (4, 0, 0xFFFFFFFF,
			_READWRITE | _CODEDATA | _MEMORY | _DPL,
			_4K | _32BIT);

/* Flush out the old GDT and install the new changes! */
    gdt_flush ();
	
}

#ifdef DEBUG
	#include <STDIO.H>
	void _GDT_debug()
	{
		for(int gdtent = 0; gdtent < 10; gdtent++)
		{
			uint32_t base, limit;
			uint8_t access, gran;
			//gdt[gdtent].
			/*struct gdt_entry
			{
				uint16_t limit_low;
				uint16_t base_low;
				uint8_t base_middle;
				uint8_t access;
				uint8_t granularity;
				uint8_t base_high;
			} __attribute__((packed));
			
			gdt[num].base_low = (base & 0xFFFF);
			gdt[num].base_middle = (base >> 16) & 0xFF;
			gdt[num].base_high = (base >> 24) & 0xFF;
			gdt[num].limit_low = (limit & 0xFFFF);
			gdt[num].granularity = ((limit >> 16) & 0x0F);
			gdt[num].granularity |= (gran & 0xF0);
			gdt[num].access = access;*/	
			access = gdt[gdtent].access;
			gran = (gdt[gdtent].granularity & 0xF0);
			limit = gdt[gdtent].limit_low;
			limit |= ((gdt[gdtent].granularity & 0xF) << 16);
			base = gdt[gdtent].base_low;
			base |= (gdt[gdtent].base_middle << 16);
			base |= (gdt[gdtent].base_high << 24);
			printf("GDT Entry %i:\n", gdtent);
			//(uint8_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
			printf("BASE = 0x%x\tLIMIT = 0x%x\tACCESS = 0x%x\tGRAN = 0x%x\n", base, limit, access, gran);
		}
	}
#endif