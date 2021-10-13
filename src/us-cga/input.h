#ifndef _INPUT_H_
#define _INPUT_H_

extern volatile byte key_scan;
extern volatile byte key_bits;

void InitKeyboard(void);
void UninitKeyboard(void);

void ConsumeKeys(void);
void ClearKeys(void);

void InitMouse(void);

bool IsMouseClicked(void);
bool IsJoystickClicked(void);
bool IsKeyboardClicked(void);

#endif
