#ifndef _INPUT_H_
#define _INPUT_H_

enum {
	KF_Up = 1,
	KF_Down = 2,
	KF_Left = 4,
	KF_Right = 8,

	KF_Enter = 0x80,
	KF_Esc = 0x40
};

extern byte have_joystick;
extern byte have_mouse;

extern volatile byte key_scan;
extern volatile byte key_bits;

void InitKeyboard(void);
void UninitKeyboard(void);

byte ReadKeys(void);
void ConsumeKeys(void);
void ClearKeys(void);

void InitMouse(void);
byte GetMousePos(uint16 *x, uint16 *y);
void SetMousePos(uint16 x, uint16 y);

bool IsMouseClicked(void);
bool IsJoystickClicked(void);
bool IsKeyboardClicked(void);

#endif
