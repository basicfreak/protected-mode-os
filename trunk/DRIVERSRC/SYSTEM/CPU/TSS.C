/*
./LIBSRC/TSS.C
*/

#include "TSS.H"
#include "GDT.H"
#include <STDIO.H>

static struct tss_entry TSS;

void flush_tss (uint16_t sel)
{
	//__asm__ __volatile__ ("ltr %0"::"r" (sel));
	__asm__ __volatile__ (".intel_syntax;\
		mov ax, %0;\
		ltr ax;\
	.att_syntax;"::"r"(sel):"ax");
}

void tss_set_stack (uint32_t kernelSS, uint32_t kernelESP)
{
	TSS.ss0 = kernelSS;
	TSS.esp0 = kernelESP;
}

uint32_t Kernel_Stack()
{
	return TSS.esp0;
}

void install_tss (uint8_t idx, uint32_t kernelSS, uint32_t kernelESP)
{
	// install TSS descriptor
	uint32_t base = (uint32_t) &TSS;

	// install descriptor
	install_ring(idx, base, base + sizeof (struct tss_entry),
		///_ACCESS | _EXEC_CODE | _DPL | _MEMORY,
		(_MEMORY | _EXEC_CODE | _ACCESS),
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
	TSS.iomap = sizeof(TSS);

	// flush TSS
	flush_tss (idx * sizeof (struct gdt_entry));
}

#ifdef DEBUG
	void _TSS_debug()
	{
		printf("ESP0 = 0x%x\tSS0 = 0x%x\tCS = 0x%x\tSS = 0x%x\tPOINTER = 0x%x\n", TSS.esp0, TSS.ss0, TSS.cs, TSS.ss, (uint32_t)&TSS);
		printf("ES = 0x%x\tDS = 0x%x\tFS = 0x%x\tGS = 0x%x\tSIZE = 0x%x\n", TSS.es, TSS.ds, TSS.fs, TSS.gs, sizeof(struct tss_entry));
		/*
	uint32_t prevTss;
		uint32_t esp0;
		uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
		uint32_t es;
		uint32_t cs;
		uint32_t ss;
		uint32_t ds;
		uint32_t fs;
		uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap;
		*/
	}
#endif

