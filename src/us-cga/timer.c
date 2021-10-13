#include <dos.h>
#include "common.h"
#include "timer.h"
#include "sound.h"

byte timer_reload1 = 4;

uint16 ticks_6_prev = 0;
volatile uint16 ticks_6 = 0;

volatile byte timer_countdown1 = 1;
volatile byte timer_countdown2 = 1;
volatile byte timer_countdown3 = 1;
volatile uint16 ticks_min = 0;
volatile uint16 ticks_sec = 0;

void (INTERRUPT *old_timer_isr)(void);
               
void INTERRUPT TimerIsr() {
	(*old_timer_isr)();

	if (--timer_countdown1 == 0) {
		timer_countdown1 = timer_reload1;
		/*SpeakerUpdate();*/
	}

	if (--timer_countdown2 == 0) {
		timer_countdown2 = 6;
		ticks_6++;
	}

	if (--timer_countdown3 == 0) {
		timer_countdown3 = 100;
		ticks_sec++;
		if (ticks_sec >= 60) {
			ticks_sec = 0;
			ticks_min++;
		}
	}

    outportb(0x20, 0x20);	/*eoi*/
}

byte timer_isr_installed = 0;
byte old_pic;

void InitTimer(void) {
	uint16 freq = 11941;

	StopSound();

	if (timer_isr_installed != 0)
		return;

	timer_isr_installed = ~0;	

	disable();

	old_timer_isr = getvect(8);
	setvect(8, TimerIsr);

	old_pic = inportb(0x21);

	outportb(0x43, 0x34);
	outportb(0x40, freq & 255);
	outportb(0x40, freq >> 8);

	enable();
}

void UninitTimer(void) {
	if (!timer_isr_installed)
		return;

	disable();

	setvect(8, old_timer_isr);

	outportb(0x21, old_pic);

	outportb(0x43, 0x34);
	outportb(0x40, 0);
	outportb(0x40, 0);

	enable();

	timer_isr_installed = 0;
}

void InitTimerAll(void) {
	InitTimer();
}

void UninitTimerAll(void) {
	UninitTimer();
}
