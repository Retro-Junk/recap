#ifndef _HAND_H_
#define _HAND_H_

extern uint16 hand_x, hand_y;
extern uint16 clip_sx, clip_ex, clip_sy, clip_ey;
extern uint16 delta_x, delta_y;
extern byte hand_cycle2;
extern uint16 shake_time;

extern byte hand_visible;

extern byte new_keys;
extern byte press_keys;
extern byte steady_hand;


void SetHandClip(void);
void ResetHand(void);
void SetHandPos(uint16 x, uint16 y);

void DashHand(void);
void FreeHand(void);

void DrawHand(byte *target);
void UndrawHand(byte *target);
void HideHand(void);

void UpdateHand(void);

byte PollInput(uint16 *x, uint16 *y);

void DrawHandInternal(byte *buffer);

void CopyRectWithHand(byte *source, uint16 x, uint16 y, byte w, byte h);
void DrawImageWithHand(byte index, uint16 x, uint16 y, byte *bank);
void PrintStringWithHand(uint16 x, uint16 y, char *str);

#endif
