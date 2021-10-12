#ifndef _INPUT_H_
#define _INPUT_H_

void InitKeyboard(void);
void UninitKeyboard(void);

void InitMouse(void);

bool IsMouseClicked(void);
bool IsJoystickClicked(void);
bool IsKeyboardClicked(void);

#endif
