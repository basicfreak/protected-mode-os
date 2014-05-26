/*
./DRIVERSRC/SYSTEM/CPU/ISR.C
*/

#include "ISR.H"
#include "IDT.H"
#include <STDIO.H>


void *ISRs[32] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

void install_ISR(uint8_t isr, void (*handler)(regs *r))
{
    ISRs[isr] = handler;
}

void uninstall_ISR(uint8_t isr)
{
    ISRs[isr] = 0;
}


void _ISR_init()
{
    install_Gate(0, (unsigned)ISR0, 0x08, 0x8E);
    install_Gate(1, (unsigned)ISR1, 0x08, 0x8E);
    install_Gate(2, (unsigned)ISR2, 0x08, 0x8E);
    install_Gate(3, (unsigned)ISR3, 0x08, 0x8E);
    install_Gate(4, (unsigned)ISR4, 0x08, 0x8E);
    install_Gate(5, (unsigned)ISR5, 0x08, 0x8E);
    install_Gate(6, (unsigned)ISR6, 0x08, 0x8E);
    install_Gate(7, (unsigned)ISR7, 0x08, 0x8E);

    install_Gate(8, (unsigned)ISR8, 0x08, 0x8E);
    install_Gate(9, (unsigned)ISR9, 0x08, 0x8E);
    install_Gate(10, (unsigned)ISR10, 0x08, 0x8E);
    install_Gate(11, (unsigned)ISR11, 0x08, 0x8E);
    install_Gate(12, (unsigned)ISR12, 0x08, 0x8E);
    install_Gate(13, (unsigned)ISR13, 0x08, 0x8E);
    install_Gate(14, (unsigned)ISR14, 0x08, 0x8E);
    install_Gate(15, (unsigned)ISR15, 0x08, 0x8E);

    install_Gate(16, (unsigned)ISR16, 0x08, 0x8E);
    install_Gate(17, (unsigned)ISR17, 0x08, 0x8E);
    install_Gate(18, (unsigned)ISR18, 0x08, 0x8E);
    install_Gate(19, (unsigned)ISR19, 0x08, 0x8E);
    install_Gate(20, (unsigned)ISR20, 0x08, 0x8E);
    install_Gate(21, (unsigned)ISR21, 0x08, 0x8E);
    install_Gate(22, (unsigned)ISR22, 0x08, 0x8E);
    install_Gate(23, (unsigned)ISR23, 0x08, 0x8E);

    install_Gate(24, (unsigned)ISR24, 0x08, 0x8E);
    install_Gate(25, (unsigned)ISR25, 0x08, 0x8E);
    install_Gate(26, (unsigned)ISR26, 0x08, 0x8E);
    install_Gate(27, (unsigned)ISR27, 0x08, 0x8E);
    install_Gate(28, (unsigned)ISR28, 0x08, 0x8E);
    install_Gate(29, (unsigned)ISR29, 0x08, 0x8E);
    install_Gate(30, (unsigned)ISR30, 0x08, 0x8E);
    install_Gate(31, (unsigned)ISR31, 0x08, 0x8E);
}

char *exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

// Common Handler for ISRSs
void ISR_HANDLER(regs *r)
{
	void (*ISR)(regs *r);
	// Get ISRS Resolve Routine
	ISR = ISRs[r->int_no];
	// Do we have a valid Routine?
	if (ISR)
		ISR(r);
	else
		iError(r);	// No Routine BSoD time.
}

void iError(regs *r)						//Reason is Exception Name
{
	setColor (0x1F);
	cls ();
	//print				("    .    1    .    2    .    3    .    4    .    5    .    6    .    7    .    8", (char)0x1F);
	puts ("\t\t\t\t\t\t\t   ");
	setColor (0xCF);
	puts ("Fatal System Error\n");
	setColor (0x1F);
	printf ("\t\t\t\t\t\t   %s Exception\n", exception_messages[r->int_no]);
//HEX OF MEMORY LOCATION
	/*typedef struct registers	{
		uint32_t gs, fs, es, ds;
		uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
		uint32_t int_no, err_code;
		uint32_t eip, cs, eflags, useresp, ss;    
	} regs;*/
	printf("EAX = 0x%x\nEBX = 0x%x\nECX = 0x%x\nEDX = 0x%x\n", r->eax, r->ebx, r->ecx, r->edx);
	printf("EDI = 0x%x\nESI = 0x%x\nEBP = 0x%x\nESP = 0x%x\n", r->edi, r->esi, r->ebp, r->esp);
	printf("GS = 0x%x\nFS = 0x%x\nES = 0x%x\nDS = 0x%x\n", r->gs, r->fs, r->es, r->ds);
	printf("EIP = 0x%x\nCS = 0x%x\nEFLAGS = 0x%x\nSS = 0x%x\n", r->eip, r->cs, r->eflags, r->ss);
	printf("USER-ESP = 0x%x\nERROR-CODE = 0x%x\n", r->useresp, r->err_code);
	
//END HEX
	setColor (0x94);
	puts ("\t\t\t\t\t\t\t\t System  Halted\n");
	__asm__ __volatile__ ("cli");
	__asm__ __volatile__ ("hlt");
	for (;;);
}
