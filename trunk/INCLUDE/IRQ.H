/*
./INCLUDE/IRQ.H
*/

#ifndef				IRQ_H_INCLUDED
	#define				IRQ_H_INCLUDED
	
	/* These are own ISRs that point to our special IRQ handler
	*  instead of the regular 'fault_handler' function */
	extern void irq0();
	extern void irq1();
	extern void irq2();
	extern void irq3();
	extern void irq4();
	extern void irq5();
	extern void irq6();
	extern void irq7();
	extern void irq8();
	extern void irq9();
	extern void irq10();
	extern void irq11();
	extern void irq12();
	extern void irq13();
	extern void irq14();
	extern void irq15();
	
	struct regs
	{
		unsigned int gs, fs, es, ds;
		unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
		unsigned int int_no, err_code;
		unsigned int eip, cs, eflags, useresp, ss;    
	};
	
	void isrs_install(void);
	void irq_install_handler(int irq, void (*handler)(struct regs *r));
	void irq_uninstall_handler(int irq);
	void irq_install(void);
	void iError(char *Reason);
	
#endif