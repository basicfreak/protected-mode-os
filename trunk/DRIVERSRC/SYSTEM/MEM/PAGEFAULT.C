/*
./LIBSRC/MEM/PAGEFAULT.C
*/

#include "PAGEFAULT.H"
#include "../CPU/ISR.H"
#include <STDIO.H>

void pageFault_handler(regs *r)
{
	getch("Page Fault");
}

void init_pageFault()
{
	install_ISR(14, pageFault_handler);
}