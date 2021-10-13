#include <dos.h>
#include <bios.h>
#include "common.h"
#include "input.h"

byte have_joystick = 0;
byte have_key = 0;

void (INTERRUPT *old_keyboard_isr)(void);
bool key_passdown = true;

volatile byte key_scan = 0;
volatile byte key_bits = 0;

static struct {
	byte scan;
	byte action;
} keyboard_map[] = {
	{0x48, 1},		/*Up*/
	{0x50, 2},		/*Down*/
	{0x4B, 4},		/*Left*/
	{0x4D, 8},		/*Right*/

	{0x47, 1 | 4},	/*Up + Left*/
	{0x49, 1 | 8},	/*Up + Right*/
	{0x4F, 2 | 4},	/*Down + Left*/
	{0x51, 2 | 8},	/*Down + Right*/

	{0x39, 0x80},	/*Space*/
	{0x01, 0x40},	/*Esc*/
	{0x1C, 0x80},	/*Enter*/

	{0x74, 0x80},
	{0x7C, 1},
	{0x7B, 2},
	{0x7A, 4},
	{0x79, 8},
	{0x77, 0x80},

	{0x29, 1},		/*`*/
	{0x4A, 2},		/*-*/
	{0x2B, 4},		/*\*/
	{0x4E, 8},		/*+*/

	{0x00, 0}
};

void TranslateScancode(byte scan) {
	int i;
	byte ks = scan & 0x7F;

	for (i = 0;keyboard_map[i].scan != 0;i++) {
		if (keyboard_map[i].scan == ks) {
			byte action = keyboard_map[i].action;
			if (scan & 0x80) {
				/*release*/
				key_bits &= ~action;
			} else {
				/*mark*/
				key_bits |= action;
			}
			return;
		}
	}

	key_scan = ks;
}

void INTERRUPT KeyboardIsr() {
	byte scan, strobe;
	scan = inportb(0x60);
	if (!key_passdown) {
		/*consume scan from kbd. controller*/
		strobe = inportb(0x61);
		outportb(0x61, strobe | 0x80);
		outportb(0x61, strobe);
	}

	TranslateScancode(scan);

	if (!key_passdown) {
		outportb(0x20, 0x20);
	} else {
	   (*old_keyboard_isr)();
	}
}


void InitKeyboard(void) {
	disable();
	key_passdown = true;
	old_keyboard_isr = getvect(9);
	setvect(9, KeyboardIsr);
	enable();
}

void UninitKeyboard(void) {
	disable();
	setvect(9, old_keyboard_isr);
	enable();
}

bool KeyPressed(void) {
	return bioskey(1) != 0;
}

uint16 GetKey(void) {
	return bioskey(0);
}

void ConsumeKeys(void) {
	while (KeyPressed()) {
		GetKey();
	}
}

void ClearKeys(void) {
	ConsumeKeys();
}

void InitMouse(void) {

}

bool IsMouseClicked(void) {
	return false;
}

bool IsJoystickClicked(void) {
	return false;
}

byte GetKeys(void) {
	byte joykeys = 0;
	if (have_joystick) {
		/*TODO*/
	}
	return key_bits | joykeys;
}

byte ReadKeys(void) {
	byte key = GetKeys();
	ConsumeKeys();
	have_key = key != 0;
	return key;
}

bool IsKeyboardClicked(void) {
	return (ReadKeys() & 0x80) != 0;
}
