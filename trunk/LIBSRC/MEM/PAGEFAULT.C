/*
./LIBSRC/MEM/PAGEFAULT.C

IRQ 14
*/

#include <MEM/PAGEFAULT.H>
#include <IRQ.H>
#include <STDIO.H>

void pageFault_handler(struct regs *r)
{
	getch("Page Fault");
}

void init_pageFault()
{
	irq_install_handler(14, pageFault_handler);
}