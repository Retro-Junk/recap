#ifndef _INPUT_H_
#define _INPUT_H_

extern byte have_joystick;
extern byte have_mouse;

extern volatile byte key_scan;
extern volatile byte key_bits;

void InitKeyboard(void);
void UninitKeyboard(void);

void ConsumeKeys(void);
void ClearKeys(void);

void InitMouse(void);
void SetMousePos(uint16 x, uint16 y);

bool IsMouseClicked(void);
bool IsJoystickClicked(void);
bool IsKeyboardClicked(void);

#endif
