/*
./DRIVERSRC/SYSTEM/CPU/IRQ.H
*/

#ifndef				IRQ_H_INCLUDED
	#define				IRQ_H_INCLUDED
	
	#include <FORMATTING.H>

	extern void IRQ0();
	extern void IRQ1();
	extern void IRQ2();
	extern void IRQ3();
	extern void IRQ4();
	extern void IRQ5();
	extern void IRQ6();
	extern void IRQ7();
	extern void IRQ8();
	extern void IRQ9();
	extern void IRQ10();
	extern void IRQ11();
	extern void IRQ12();
	extern void IRQ13();
	extern void IRQ14();
	extern void IRQ15();
	
	typedef struct registers	{
		uint32_t gs, fs, es, ds;
		uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
		uint32_t int_no, err_code;
		uint32_t eip, cs, eflags, useresp, ss;    
	} regs;
	
	void install_IR(uint8_t irq, void (*handler)(regs *r));
	void uninstall_IR(uint8_t irq);
	void _IRQ_init(void);
	
#endif