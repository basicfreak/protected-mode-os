/*
./DRIVERSRC/SYSTEM/CPU/ISR.H
*/

#ifndef				ISR_H_INCLUDED
	#define				ISR_H_INCLUDED
	
	#include <FORMATTING.H>
	#include "IRQ.H"
	
	extern void ISR0();
	extern void ISR1();
	extern void ISR2();
	extern void ISR3();
	extern void ISR4();
	extern void ISR5();
	extern void ISR6();
	extern void ISR7();
	extern void ISR8();
	extern void ISR9();
	extern void ISR10();
	extern void ISR11();
	extern void ISR12();
	extern void ISR13();
	extern void ISR14();
	extern void ISR15();
	extern void ISR16();
	extern void ISR17();
	extern void ISR18();
	extern void ISR19();
	extern void ISR20();
	extern void ISR21();
	extern void ISR22();
	extern void ISR23();
	extern void ISR24();
	extern void ISR25();
	extern void ISR26();
	extern void ISR27();
	extern void ISR28();
	extern void ISR29();
	extern void ISR30();
	extern void ISR31();
	
	void _ISR_init(void);
	void install_ISR(uint8_t isrs, void (*handler)(regs *r));
	void uninstall_ISR(uint8_t isrs);
	void iError(regs *r);
	
#endif