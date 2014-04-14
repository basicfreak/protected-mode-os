/*
./LIBSRC/MEM/PAGEFAULT.C

IRQ 14
*/

#include <MEM/PAGEFAULT.H>
#include <IRQ.H>

void pageFault_handler(struct regs *r)
{
	for(;;);
}

void init_pageFault()
{
	irq_install_handler(14, pageFault_handler);
}