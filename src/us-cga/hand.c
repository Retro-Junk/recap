#include "common.h"
#include "hand.h"
#include "data.h"
#include "input.h"
#include "cga.h"

uint16 prev_hand_x, prev_hand_y;
uint16 hand_x, hand_y;
uint16 clip_sx, clip_ex, clip_sy, clip_ey;
uint16 delta_x, delta_y;
uint16 hand_height;
byte hand_visible;
byte hand_extend;
byte hand_y_clip;
byte hand_cycle;
byte hand_cycle2;
byte hand_cycle3;
byte hand_anim;

byte PollInput(uint16 *x, uint16 *y) {
	prev_hand_x = hand_x;
	prev_hand_y = hand_y;

	*x = hand_x;
	*y = hand_y;

	/*TODO*/
	return 0;
}

void UpdateHand(void) {
	uint16 x, y;
	byte keys;
	byte update = 0;
	if (!hand_visible)
		return;
	keys = PollInput(&x, &y);
	hand_cycle2 = 0;
	if (hand_cycle != 0)
		goto resume;

	while (keys & 1) {
		hand_cycle3 = 1;
		hand_anim = 1;
		hand_cycle = 2;
resume:
		if (--hand_cycle3)
			break;
		hand_cycle3 = 1;

		update++;

		if (--hand_anim)
			break;
		hand_anim = 4;

		if (--hand_cycle)
			break;
		hand_cycle2++;
	}

	if (x != hand_x || y != hand_y || update) {
		hand_x = x;
		hand_y = y;
		UndrawHand(frontbuffer);
		DrawHandInternal(frontbuffer);
	}
}

void DrawHandInternal(byte *buffer) {
	hand_y_clip = CGA_HEIGHT;
	CGA_DrawHandSprite((hand_cycle == 0) ? 0 : hand_anim, hand_x, hand_y, hand_y_clip, main_data, buffer);

	if (hand_extend == 0)
		hand_y_clip = hand_y + 48;

	CGA_DrawHandSprite((hand_cycle == 0) ? 5 : 6, hand_x, hand_y + 16, hand_y_clip, main_data, buffer);
	hand_y_clip = CGA_HEIGHT;
}

/*
Hide hand
*/
void HideHand(void) {
	CGA_Rect3ToScreen(hand_x, hand_y, CGAW(36), (hand_y + hand_height <= CGA_HEIGHT) ? hand_height : (CGA_HEIGHT - hand_y));
	hand_visible = 0;
}

/*
Set hand clip rect
*/
void SetHandClip(void) {
	clip_sx = 8;
	clip_ex = 288;
	clip_sy = 108;
	clip_ey = 196;
	delta_x = 8;
	delta_y = 4;
}

/*
Switch hand to dash mode and draw it
*/
void DashHand(void) {
	hand_extend = 1;
	hand_height = 116;
	SetHandPos(160, 160);
	delta_x = 8;
	delta_y = 4;
	DrawHandInternal(frontbuffer);
	hand_visible = 1;
}

/*
Switch hand to free mode and draw it
*/
void FreeHand(void) {
	hand_extend = 0;
	hand_height = 48;
	delta_x = 1;
	delta_y = 1;
	DrawHandInternal(frontbuffer);
	hand_visible = 1;
}

/*
Set hand coords (but don't redraw it)
*/
void SetHandPos(uint16 x, uint16 y) {
	hand_x = x;
	hand_y = y;
	if (have_mouse) {
		SetMousePos(x, y);
	}
}

/*
Draw hand, if enabled
*/
void DrawHand(byte *target) {
	if (hand_visible)
		DrawHandInternal(target);
}

/*
Undraw hand's pixels, but only those that's now outside of current hand rect
*/
void UndrawHand(byte *target) {
	uint16 w, h;
	uint16 nx, px, ey;
	byte moved_up = 0;
	uint16 x = prev_hand_x;
	uint16 y = prev_hand_y;

	/*erase upper/lower parts*/
	if (hand_y > prev_hand_y) {
		/*moved down*/
		h = hand_y - prev_hand_y;
		if (h >= hand_height) {
			/*fully leave old location*/
			CGA_Rect3ToScreen(x, y, CGAW(36), hand_height);
			return;
		} else {
			/*clear some top lines*/
			CGA_Rect3ToScreen(x, y, CGAW(36), h);
		}
	} else if (hand_y < prev_hand_y) {
		/*moved up*/
		moved_up = 1;
		ey = hand_y + hand_height;
		if (ey > CGA_HEIGHT)
			ey = CGA_HEIGHT;
		y += hand_height;
		if (y > CGA_HEIGHT)
			y = CGA_HEIGHT;
		h = y - ey;
		if (h != 0) {
			if (h > hand_height) {
				/*fully leave old location*/
				CGA_Rect3ToScreen(x, prev_hand_y, CGAW(36), hand_height);
				return;
			} else {
				/*clear some bottom lines*/
				CGA_Rect3ToScreen(x, y - h, CGAW(36), h);
			}
		}
	}

	/*erase sides*/
	y = moved_up ? prev_hand_y : (prev_hand_y + h);
	h = hand_height - h;
	if (y + h > CGA_HEIGHT)
		h = CGA_HEIGHT - y;

	nx = hand_x / 4;
	px = prev_hand_x / 4;

	if (nx > px) {
		/*moved right*/
		w = nx - px;
		if (w > CGAW(36))
			w = CGAW(36);
		CGA_Rect3ToScreen(x, y, w, h);
	} else {
		/*moved left*/
		nx += CGAW(32);
		px += CGAW(32);
		x += 32;
		if (hand_x % 4 != 0)
			nx += CGAW(4);
		if (prev_hand_x % 4 != 0) {
			px += CGAW(4);
			x += 4;
		}

		if (nx < px) {
			w = px - nx;
			if (w > CGAW(36))
				w = CGAW(36);
			CGA_Rect3ToScreen(x - w * 4, y, w, h);
		}
	}
}
