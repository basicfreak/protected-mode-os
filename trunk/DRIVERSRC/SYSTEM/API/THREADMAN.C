/*
./DRIVERSRC/SYSTEM/API/THREADMAN.C
*/

#include "THREADMAN.H"
#include <STDIO.H>

uint16_t Current_Thread = 1; // 0 always refers to idle thread - 1 will be kernel all info will be saved on first task switch (hopefully...)

extern void _SYSTEM_IDLE_THREAD(void); // thread 0
bool forceidleonecycle = false;

void _THREAD_MAN_PIT_ENTRY(tregs *r)
{
	// Save current Threads Register Information
	memcpy(&THREAD[Current_Thread].registers, r, sizeof(tregs));
	// Find next thread
	uint16_t Next_Thread;
	/*bool found = FALSE;
	for (Next_Thread = Current_Thread; ((Next_Thread < MAX_THREADS) && (!found) && (THREAD[Next_Thread].flags)); Next_Thread++)
		if (THREAD[Next_Thread].flags < 0x80)
			found = true;
	if (!found)
		for (Next_Thread = 0; ((Next_Thread <= Current_Thread) && (!found) && (THREAD[Next_Thread].flags)); Next_Thread++)
			if (THREAD[Next_Thread].flags < 0x80)
				found = true;
	if (!found)*/
		if (Current_Thread) Next_Thread = 0;
		else Next_Thread = 1;
		/*if(forceidleonecycle) {
			Next_Thread = 0;
			forceidleonecycle = false;
			memcpy(r, &THREAD[Next_Thread].registers, sizeof(tregs));
			printf("EIP = 0x%x EFLAGS = 0x%x\n", THREAD[Next_Thread].registers.eip, THREAD[Next_Thread].registers.eflags);
		} else Next_Thread = 1;*/
	// Load next Thread
	Current_Thread = Next_Thread;
	memcpy(r, &THREAD[Current_Thread].registers, sizeof(tregs));
}

void _THREAD_MAN_init()
{
	//printf("\n\tsizeof(tregs) * MAX_THREADS = 0x%x\n", sizeof(tregs) * MAX_THREADS);
	memset (&THREAD, 0, sizeof(tregs) * MAX_THREADS); // Clear Thread Table
	puts("Done.\n\t\tADDING IDLE THREAD... #");
	AddThread((uint32_t) &_SYSTEM_IDLE_THREAD, 0xFF, 0);
	puts("Done.\n\t\tADDING KERNEL THREAD... #");
	AddThread(0, 0x01, 0);
	puts("Done.\n\tEnable Tasking...");
	_THREAD_MAN_enable();
}
void _THREAD_MAN_enable()
{
	_THREADMAN_ACTIVE_ = true;
}

void _THREAD_MAN_disable()
{
	_THREADMAN_ACTIVE_ = false;
}

extern uint32_t Kernel_Stack(void);
void test()
{
	forceidleonecycle = true;
}

uint16_t AddThread(uint32_t location, uint8_t flags, bool user)
{
	printf("\n\tAddThread(0x%x, 0x%x, 0x%x) = ", location, flags, user);
	bool found = false;
	int task = 0;
	for (int i = 0; i < MAX_THREADS ; i++)
		if (!THREAD[i].cr3 && !found) {
			found = true;
			task = i;
		}
printf ("%i ", task);
	tregs tempreg;
	memset(&tempreg, 0, sizeof(tregs));
	if (!user) {		// Kernel Thread
		tempreg.gs = 0x10;
		tempreg.fs = 0x10;
		tempreg.es = 0x10;
		tempreg.ds = 0x10;
		tempreg.ss = 0x10;
		tempreg.cs = 0x08;
		tempreg.useresp = Kernel_Stack();
		THREAD[task].cr3 = vmmngr_get_directory();
	} else {			// User Thread
		tempreg.gs = 0x23;
		tempreg.fs = 0x23;
		tempreg.es = 0x23;
		tempreg.ds = 0x23;
		tempreg.ss = 0x23;
		tempreg.cs = 0x1B;
		tempreg.useresp = 0xC0000000;
// NEED TO SETUP NEW PDIR FOR USER TASKS... LATER
	}					// Common
	tempreg.eip = (uint32_t)location;
	tempreg.esp = Kernel_Stack();
	tempreg.eflags = 0x0202;
	memcpy(&THREAD[task].registers, &tempreg, sizeof(tregs));
	THREAD[task].flags = flags;
	printf("& EIP = 0x%x\n", THREAD[task].registers.eip);
	return task;
}
