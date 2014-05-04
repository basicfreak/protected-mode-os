/*
./INCLUDE/GDT.H
*/

#ifndef				GDT_H_INCLUDED
	#define				GDT_H_INCLUDED
	
	enum I86_GDT_DESC {
		_ACCESS=0x0001,			//00000001
		_READWRITE=0x0002,		//00000010
		_EXPANSION=0x0004,		//00000100
		_EXEC_CODE=0x0008,		//00001000
		_CODEDATA=0x0010,		//00010000
		_DPL=0x0060,			//01100000
		_MEMORY=0x0080			//10000000
	};
	enum I86_GDT_GRAND {
		_LIMITHI_MASK=0x0f,		//00001111
		_OS=0x10,				//00010000
		_32BIT=0x40,			//01000000
		_4K=0x80				//10000000
	};

	
	/* Defines a GDT entry. We say packed, because it prevents the
	*  compiler from doing things that it thinks is best: Prevent
	*  compiler "optimization" by packing */
	struct gdt_entry
	{
		unsigned short limit_low;
		unsigned short base_low;
		unsigned char base_middle;
		unsigned char access;
		unsigned char granularity;
		unsigned char base_high;
	} __attribute__((packed));

	/* Special pointer which includes the limit: The max bytes
	*  taken up by the GDT, minus 1. Again, this NEEDS to be packed */
	struct gdt_ptr
	{
		unsigned short limit;
		unsigned int base;
	} __attribute__((packed));

	/* Our GDT, with 3 entries, and finally our special GDT pointer */
	struct gdt_entry gdt[3];
	struct gdt_ptr gp;

	/* This will be a function in start.asm. We use this to properly
	*  reload the new segment registers */
	extern void gdt_flush();
	
	void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
	void gdt_install(void);
	
#endif
