/*
./DRIVERSRC/SYSTEM/CPU/INT.H
*/

#ifndef				INT_H_INCLUDED
	#define				INT_H_INCLUDED
	
	#include <FORMATTING.H>
	#include "IRQ.H"
	
	extern void INT0();
	extern void INT1();
	extern void INT2();
	extern void INT3();
	extern void INT4();
	extern void INT5();
	extern void INT6();
	extern void INT7();
	extern void INT8();
	extern void INT9();
	extern void INT10();
	extern void INT11();
	extern void INT12();
	extern void INT13();
	extern void INT14();
	extern void INT15();
	extern void INT16();
	extern void INT17();
	extern void INT18();
	extern void INT19();
	extern void INT20();
	extern void INT21();
	extern void INT22();
	extern void INT23();
	extern void INT24();
	extern void INT25();
	extern void INT26();
	extern void INT27();
	extern void INT28();
	extern void INT29();
	extern void INT30();
	extern void INT31();
	
	void _INT_init(void);
	void install_INT(uint8_t INTn, void (*handler)(regs *r));
	void uninstall_INT(uint8_t INTn);
	
#endif