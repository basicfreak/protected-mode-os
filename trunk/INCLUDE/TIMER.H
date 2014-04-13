/*
./INCLUDE/TIMER.H
*/

#include <FORMATTING.H>

#ifndef				TIMER_H_INCLUDED
	#define				TIMER_H_INCLUDED
	
	unsigned long timer_ticks;
	unsigned int timer_seconds;
	unsigned int timer_minutes;
	unsigned int timer_hours;
	unsigned int timer_days;
	unsigned int timer_years;
	bool UPTIME_ACTIVE;
	
	void display_uptime(void);
	void timer_wait(unsigned int ticks);
	void sleep(unsigned int secs);
	void timer_install(void);
	
#endif
