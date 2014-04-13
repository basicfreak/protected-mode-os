/*
./LIBSRC/TIMER.C
*/

#include <TIMER.H>
#include <IRQ.H>
#include <STDIO.H>

/* Handles the timer. In this case, it's very simple: We
*  increment the 'timer_ticks' variable every time the
*  timer fires. By default, the timer fires 18.222 times
*  per second. Why 18.222Hz? Some engineer at IBM must've
*  been smoking something funky */
void timer_handler(struct regs *r)
{
    /* Increment our 'tick count' */
    timer_ticks++;

    /* Every 18 clocks (approximately 1 second), we will
    *  display a message on the screen */
    if (timer_ticks % 18 == 0)
    {
        timer_seconds ++;
		if (timer_seconds >= 60)
		{
			timer_minutes ++;
			timer_seconds = timer_seconds - 60;
		}
		if (timer_minutes >= 60)
		{
			timer_hours ++;
			timer_minutes = timer_minutes - 60;
		}
		if (timer_hours >= 24)
		{
			timer_days ++;
			timer_hours = timer_hours - 24;
		}
		if (timer_days >= 365)
		{
			timer_years ++;
			timer_days = timer_days - 365;
		}
		if(UPTIME_ACTIVE) display_uptime();
    }
}
void display_uptime()
{
	movcur_disable ++;
	int tempX = curX;
	int tempY = curY;
	curX = 40;
	curY = 24;
	printf	("%i Years, %i Days, %i:%i:%i Uptime!   ", timer_years, timer_days, timer_hours, timer_minutes, timer_seconds);
	curX = tempX;
	curY = tempY;
	movcur_disable --;
	return;
}

void timer_wait(unsigned int ticks)
{
    unsigned long eticks;
    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks);
	return;
}

void sleep(unsigned int secs)
{
	while(secs--) timer_wait(18);
	return;
}

/* Sets up the system clock by installing the timer handler into IRQ0 */
void timer_install()
{
	UPTIME_ACTIVE = TRUE;
	timer_seconds = 0;
	timer_minutes = 0;
	timer_hours = 0;
	timer_days = 0;
	timer_years = 0;
	timer_ticks = 0;
    /* Installs 'timer_handler' to IRQ0 */
    irq_install_handler(0, timer_handler);
}