#ifndef _TIMER_H_
#define _TIMER_H_

extern volatile uint16 ticks_min;
extern volatile uint16 ticks_sec;

void InitTimerAll(void);
void UninitTimerAll(void);

#endif