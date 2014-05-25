/*
./LIBSRC/TSS.C
*/

#include "TSS.H"
#include "GDT.H"
#include <STDIO.H>

static struct tss_entry TSS;

void flush_tss (uint16_t sel)
{
	__asm__ __volatile__ ("ltr %0"::"r" (sel));
}

void tss_set_stack (uint16_t kernelSS, uint16_t kernelESP)
{
	TSS.ss0 = kernelSS;
	TSS.esp0 = kernelESP;
}

void install_tss (uint32_t idx, uint16_t kernelSS, uint16_t kernelESP)
{
	// install TSS descriptor
	uint32_t base = (uint32_t) &TSS;

	// install descriptor
	install_ring(idx, base, base + sizeof (struct tss_entry),
		_ACCESS | _EXEC_CODE | _DPL | _MEMORY,
		0);

	// initialize TSS
	memset ((void*) &TSS, 0, sizeof (struct tss_entry));

	// set stack and segments
	TSS.ss0 = kernelSS;
	TSS.esp0 = kernelESP;
	TSS.cs = 0x0b;
	TSS.ss = 0x13;
	TSS.es = 0x13;
	TSS.ds = 0x13;
	TSS.fs = 0x13;
	TSS.gs = 0x13;

	// flush TSS
	flush_tss (idx * sizeof (struct gdt_entry));
}
