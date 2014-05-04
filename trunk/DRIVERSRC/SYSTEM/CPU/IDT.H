/*
./INCLUDE/IDT.H
*/

#ifndef				IDT_H_INCLUDED
	#define				IDT_H_INCLUDED
	
	/* These are function prototypes for all of the exception
	*  handlers: The first 32 entries in the IDT are reserved
	*  by Intel, and are designed to service exceptions! */
	extern void isr0();
	extern void isr1();
	extern void isr2();
	extern void isr3();
	extern void isr4();
	extern void isr5();
	extern void isr6();
	extern void isr7();
	extern void isr8();
	extern void isr9();
	extern void isr10();
	extern void isr11();
	extern void isr12();
	extern void isr13();
	extern void isr14();
	extern void isr15();
	extern void isr16();
	extern void isr17();
	extern void isr18();
	extern void isr19();
	extern void isr20();
	extern void isr21();
	extern void isr22();
	extern void isr23();
	extern void isr24();
	extern void isr25();
	extern void isr26();
	extern void isr27();
	extern void isr28();
	extern void isr29();
	extern void isr30();
	extern void isr31();

	/* Defines an IDT entry */
	struct idt_entry
	{
		unsigned short base_lo;
		unsigned short sel;
		unsigned char always0;
		unsigned char flags;
		unsigned short base_hi;
	} __attribute__((packed));

	struct idt_ptr
	{
		unsigned short limit;
		unsigned int base;
	} __attribute__((packed));

	/* Declare an IDT of 256 entries. Although we will only use the
	*  first 32 entries in this tutorial, the rest exists as a bit
	*  of a trap. If any undefined IDT entry is hit, it normally
	*  will cause an "Unhandled Interrupt" exception. Any descriptor
	*  for which the 'presence' bit is cleared (0) will generate an
	*  "Unhandled Interrupt" exception */
	struct idt_entry idt[256];
	struct idt_ptr idtp;

	/* This exists in 'start.asm', and is used to load our IDT */
	extern void idt_load();	
	
	void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
	void idt_install(void);
	
#endif
