#ifndef _HAND_H_
#define _HAND_H_

extern uint16 hand_x, hand_y;
extern uint16 clip_sx, clip_ex, clip_sy, clip_ey;

void SetHandClip(void);
void ResetHand(void);
void SetHandPos(uint16 x, uint16 y);

void DashHand(void);
void FreeHand(void);

void DrawHand(byte *target);
void UndrawHand(byte *target);
void HideHand(void);

void UpdateHand(void);

void DrawHandInternal(byte *buffer);

void CopyRectWithHand(byte *source, uint16 x, uint16 y, byte w, byte h);
void DrawImageWithHand(byte index, uint16 x, uint16 y, byte *bank);

#endif
