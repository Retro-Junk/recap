#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include "common.h"
#include "data.h"
#include "cga.h"
#include "script.h"
#include "timer.h"
#include "input.h"

byte rand_0;
byte rand_1;

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

void InitGame(void) {
	SwitchToGraphicsMode();
#ifdef __TURBOC__
    ctrlbrk(nocbrk);
#endif

	if (!LoadFile("egalogo.cga", wseg_8_backbuffer3))
		ExitGame("egalogo");
	CGA_Buffer3ToScreen();

for(;;) ;


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


bool CanDispatch(void) {
	return 0;
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
