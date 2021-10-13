#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include "common.h"
#include "data.h"
#include "cga.h"
#include "script.h"
#include "timer.h"
#include "input.h"
#include "sound.h"

byte rand_0;
byte rand_1;

void InitSystem(void) {
	InitKeyboard();
	InitTimerAll();
}

void UninitSystem(void) {
	UninitTimerAll();
	UninitKeyboard();
}

void ExitGame(char *msg) {
	UninitSystem();
	SwitchToTextMode();
	printf("%s\n", msg);
	exit(0);	
}

int nocbrk(void) { return 1; }

bool CanDispatch(void) {
	/*TODO*/
	return 0;
}

void DrawSomething1(void) {
	if (CanDispatch()) {
		CGA_BlitRect(wseg_0, 44, 40, CGAW(72), 70, wseg_8_backbuffer3);
	}
}

void DrawSomething2(void) {
	/*TODO*/
}

void DrawCountes1(byte *target) {
	/*TODO*/
}

byte show_counters2 = 1;
uint16 last_counters2_update = ~0u;

void DrawCounters2(byte *target) {
	/*TODO*/
}

void DrawHand(byte *target) {
	/*TODO*/
}

void DrawShipInterior(void) {
	CGA_TableauToBuffer3();
	DrawSomething1();
	DrawSomething2();
	CGA_Buffer3ToBuffer1();

	DrawCountes1(wseg_6_backbuffer1);
	show_counters2 = 1;
	last_counters2_update = ~0u;
	DrawCounters2(wseg_6_backbuffer1);

	DrawHand(wseg_6_backbuffer1);

	CGA_Buffer1ToScreen();
}

void InitGame(void) {
	SwitchToGraphicsMode();
#ifdef __TURBOC__
    ctrlbrk(nocbrk);
#endif

	/*Mindscape logo*/
	if (!LoadFile("egalogo.cga", wseg_8_backbuffer3))
		ExitGame("egalogo");
	CGA_Buffer3ToScreen();

	if (!LoadSounds())
		ExitGame("sound");

	if (!LoadFile("gene.cga", wseg_6_backbuffer1))
		ExitGame("gene");

	CGA_GrabRect(wseg_1, 87, 0, CGAW(200), 6, wseg_6_backbuffer1);	
	CGA_FillRect(0, 87, 0, CGAW(200), 6, wseg_6_backbuffer1);	/*TODO: this does not fully remove "insert disk" header?*/

	/*Title screen*/
	CGA_DotCrossFade(wseg_6_backbuffer1, 149, frontbuffer);

	if (!LoadFile("titre.cga", wseg_8_backbuffer3))
		ExitGame("titre");

	CGA_BlitRect(wseg_1, 87, 0, CGAW(200), 6, wseg_6_backbuffer1);	
	CGA_BlitRect(wseg_1, 87, 0, CGAW(200), 6, wseg_8_backbuffer3);	

	/*TODO: init joystick*/

	InitMouse();

	InitSystem();

	PlaySound(0);

	{
		int screen, delay, good = 0;
		for(screen = 0;!good;screen ^= 1) {
			if (screen == 0) {
				CGA_DotCrossFade(wseg_8_backbuffer3, 99, frontbuffer);
				delay = 5;
			} else {
				CGA_DotCrossFade(wseg_6_backbuffer1, 21, frontbuffer);
				delay = 6;
			}
			ticks_sec = 0;

			while (ticks_sec <= delay) {
				if (IsMouseClicked() || IsJoystickClicked() || IsKeyboardClicked()) {
					StopSound();
					UninitTimerAll();	/*TODO: bug? ticks will never update after that*/

					if (LoadFile("iko.cgc", bits_pool_seg + 0xA00))
					{
						good = 1;
						break;
					}
				}
			}
		}
	}

	if (!LoadFile("dicogb.bin", bits_pool_seg))
		ExitGame("dicogb");

	ClearKeys();

	if (!LoadFile("galax.cgc", galax_data))
		ExitGame("galax");

	ClearKeys();

	if (!LoadFile("main.cgs", main_data))
		ExitGame("main");

	ClearKeys();

	if (!LoadFile("tableau.cga", tableau_data))
		ExitGame("tableau");

	InitTimerAll();
	ClearKeys();

	sound_data[0] = 0;

	/*
	TODO: tweak some sounds speed according to machine performance
	ticks_6 = 0;
	// burn cpu cycles
	if (ticks_6 > 9) {
		sound_data[0] = 1;
		sounds_table[7].speed = 2;
		sounds_table[9].speed = 2;
	}
	*/

	ConsumeKeys();

	ticks_min = 0;
	ticks_sec = 0;
	
	DrawShipInterior();

for (;;) ;

}

unsigned long rand_seed = 0;

uint16 Rand(void) {
	rand_seed *= 314159265ul;
	return rand_seed & 0xFFFF;
}

byte RandByte(void) {
	return Rand() >> 8;
}

uint16 rand_seq[64][2];

void RandomizeThings(void) {
	int i, j;
	uint16 r;
	rand_seed = 1234;	/*TODO: this is initialized from random memory*/

	for (i = 0;i < 64;i++) {
again:
		r = RandByte();
		r |= (RandByte() & 0x7F) << 8;
		rand_seq[i][0] = r;
		rand_seq[i][1] = r;
		for (j = 0;j < i;j++) {
			if (rand_seq[j][0] == r)
				goto again;
		}
	}

	r = rand_seq[(RandByte() % 4) * 2][0];
	rand_0 = r & 255;
	rand_1 = r >> 8;

	for (i = 40;i <= 49;i++) {
		SetBit(i);
	}

	ClrBit(999);

	

}

void Dispatch1(void) {

}

void Dispatch2(void) {

}

void Dispatch3(void) {

}

void Dispatch4(void) {

}

void Dispatch9(void) {

}

void Redraw1(void) {

}

void RunEvents(int ticks) {

}

byte GetNextEvent(void) {
	return 0;
}

int main(int argc, char **argv) {
	InitGame();
	RandomizeThings();
	Dispatch3();
	for (;;) {
		byte event;
		RunEvents(1);
		event = GetNextEvent();
		switch (event) {
		case 1:
			if (CanDispatch()) {
				Dispatch1();
				Redraw1();
			}
			break;
		case 2:
			Dispatch2();
			break;
		case 3:
			Dispatch3();
			break;
		case 4:
			Dispatch4();
			break;
		case 9:
			Dispatch9();
			break;
		}
	}

	return 0;
}
