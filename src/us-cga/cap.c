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
#include "hand.h"
#include "game.h"
#include "planet.h"

byte ship_x;
byte ship_y;

void Idle(int ticks);

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

byte oorxx_ready = 0;
uint16 oorxx_pos = 0;

void DrawOorxx(void) {
	CGA_CopyRect(tableau_data, 208, 58, CGAW(32), 64, wseg_8_backbuffer3);
	CGA_CopyRect(tableau_data, 208, 58, CGAW(32), 64, wseg_6_backbuffer1);
	CGA_DrawSprite(89, 208, oorxx_pos, main_data, wseg_6_backbuffer1);
	CGA_DrawSpriteMask(90, 208, 58, main_data, wseg_6_backbuffer1, wseg_8_backbuffer3);
}

void AnimRamp(void) {
	uint16 y;
	if (oorxx_ready)
		return;

	for (y = 0;y <= 58;y += 4) {
		oorxx_pos = y;
		Idle(1);
		DrawOorxx();
		CopyRectWithHand(wseg_8_backbuffer3, 208, 58, CGAW(32), 64);
	}

	Idle(1);
	oorxx_pos -= 1;
	DrawOorxx();
	CopyRectWithHand(wseg_8_backbuffer3, 208, 58, CGAW(32), 64);
	Idle(1);
	oorxx_pos += 1;
	DrawOorxx();
	CopyRectWithHand(wseg_8_backbuffer3, 208, 58, CGAW(32), 64);

	oorxx_ready = 1;	
}

void DrawRamp(void) {
	if (!oorxx_ready)
		return;
	oorxx_pos = 58;
	DrawOorxx();
}

bool CanDisintegrate(void) {
	/*TODO*/
	return 0;
}

void DrawFridge(void) {
	if (CanDisintegrate()) {
		CGA_BlitRect(wseg_0, 44, 40, CGAW(72), 70, wseg_8_backbuffer3);
	}
}

uint16 active_buttons = 0;

#define NUM_BUTTONS 10
#define BF_80 0x80
#define BF_1  1

/*TODO: put it all in a struct?*/
byte dash_butt_x[NUM_BUTTONS] = {  0,  48,  48,  80, 112, 144, 176, 208, 240, 240};
byte dash_butt_y[NUM_BUTTONS] = {152, 121, 154, 166, 174, 153, 175, 166, 154, 121};
byte butt_state[NUM_BUTTONS]  = {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0};
byte butt_delay[NUM_BUTTONS]  = {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0};
byte butt_phase[NUM_BUTTONS]  = {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0};

void DrawDashButton(byte n) {
	byte on;
	if (--butt_delay[n] != 0)
		return;
	butt_delay[n] = 1;

	if (--butt_phase[n] == 0) {
		butt_state[n] &= BF_1;
		on = butt_state[n] & 1;
		if (on == 0)
			return;
	} else
		on = butt_phase[n] & 1;

	DrawImageWithHand(7 + n * 2 + on, dash_butt_x[n], dash_butt_y[n], main_data);
}

void DrawDashButtons(uint16 buttons) {
	byte anim_delay, anim_count, num_draw, num_anim;
	int i;
    ClearKeys();

	/*sync buttons state*/
	for (i = NUM_BUTTONS - 1;i >= 0;i--, buttons >>= 1) {
		if (buttons & 1) {
			if (butt_state[i] != 1) {
				butt_state[i] = BF_80 | BF_1;
				butt_delay[i] = 1;
				butt_phase[i] = 6;
			}
		} else {
			if (butt_state[i] != 0) {
				butt_state[i] = BF_80;
				butt_delay[i] = 1;
				butt_phase[i] = 5;
			}
		}
	}

	/*update buttons with ripple effect*/
	anim_delay = 3;
	anim_count = 1;
	do {
		Idle(1);
		if (--anim_delay == 0) {
			anim_delay = 3;
			if (anim_count != 4)
				anim_count++;
		}
		num_draw = 0;
		num_anim = anim_count;

		for (i = 0;i < NUM_BUTTONS;i++) {
			if (butt_state[i] & BF_80) {
				DrawDashButton(i);
				num_draw++;
				if (--num_anim == 0)
					break;
			}
		}
	} while (num_draw != 0);
}

#define NoButton (-1)
#define NotClicked (-2)

int GetDashButton(void) {
	int i;
	byte x, y;
	if (hand_cycle2 == 0)
		return NotClicked;
	x = (hand_x + 2) /  2;
	y = (hand_y + 2);

	for (i = NUM_BUTTONS - 1;i >= 0;i--) {
		if (butt_state[i] != 1)
			continue;

		if ((x < dash_butt_x[i] / 2) || (x >= dash_butt_x[i] / 2 + 32 / 2))
			continue;

		if ((y < dash_butt_y[i]) || (y >= dash_butt_y[i] + 24))
			continue;

		return i;		
	}

#if 0
	/*this looks redundant*/
	return (hand_cycle2 == 0) ? NotClicked : NoButton;
#endif
	return NoButton;
}

char str_buf[32];

void IntToStr(uint16 value, byte width, char *buffer) {
#if 0
	uint16 i;
	uint16 base = 1;
	for (i = 0;i < width - 1;i++)
		base *= 10;
	for (i = 0;width;width--) {
		byte digit = valey / base;
		if (digit != 0 || width == 1) {
			*buffer++ = digit + '0';
			i++;
		} else if (i != 0) 
			*buffer++ = digit + '0';
		else
			*buffer++ = ' ';
		value %= base;
		base /= 10;
	}	
	*buffer = 0;
#else
	sprintf((char*)buffer, "%*u", width, value);
#endif
}

void DrawShipCoords(byte *target) {
	IntToStr(ship_x, 3, str_buf);
	PrintString(132, 5, str_buf, target);
	IntToStr(ship_y, 3, str_buf);
	PrintString(164, 5, str_buf, target);
}

byte show_time = 1;
uint16 last_time_draw = ~0u;

void DrawTime(byte *target) {
	uint16 sec;
	if (!show_time || ticks_sec == last_time_draw)
		return;

	last_time_draw = ticks_sec;
	sec = ticks_sec;
	if (sec <= 9) {
		char s[] = "00";
		s[1] += sec;
		PrintString(172, 22, s, target);
	} else {
		IntToStr(sec, 2, str_buf);
		PrintString(172, 22, str_buf, target);
	}

	IntToStr(ticks_min, 4, str_buf);
	PrintString(132, 22, str_buf, target);
}

void DrawShipInterior(void) {
	CGA_TableauToBuffer3();
	DrawFridge();
	DrawRamp();
	CGA_Buffer3ToBuffer1();

	DrawShipCoords(wseg_6_backbuffer1);
	show_time = 1;
	last_time_draw = ~0u;
	DrawTime(wseg_6_backbuffer1);

	DrawHand(wseg_6_backbuffer1);

	CGA_Buffer1ToScreen();
}

void DrawShipInteriorPartial(uint16 x, uint16 y, byte w, byte h) {
	CGA_CopyRect(tableau_data, x, y, w, h, wseg_8_backbuffer3);
	DrawFridge();
	DrawRamp();
	DrawShipCoords(wseg_8_backbuffer3);
	show_time = 1;
	last_time_draw = ~0u;
	DrawTime(wseg_8_backbuffer3);
	CopyRectWithHand(wseg_8_backbuffer3, x, y, w, h);
}


void GoInterior(void) {
	DrawShipInteriorPartial(0, 17, CGAW(320), 134);
	DrawDashButtons(active_buttons);
	SetHandClip();
	if (!hand_visible)
		DashHand();
	DrawShipCoords(frontbuffer);
	show_time = 1;
	last_time_draw = ~0u;
	DrawTime(frontbuffer);
	AnimRamp();	
}

void BackToShip(void) {
	active_buttons = (active_buttons & 0x100) | 0xE0;
	if (IsRecentPlanet())
		active_buttons |= 1; 
	GoInterior();
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
	CGA_FillRect(0, 87, 0, CGAW(200), 6, wseg_6_backbuffer1);

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
	oorxx_ready = 0;
	
	DrawShipInterior();

	SetHandClip();
	clip_sy = 160;
	DashHand();

	active_buttons = 0xE0;	/*11100000 - Save, Exterior, Galaxy*/
	DrawDashButtons(active_buttons);

	AnimRamp();
}

unsigned long rand_seed = 0;

uint16 Rand(void) {
	rand_seed *= 314159265ul;
	return rand_seed & 0xFFFF;
}

byte RandByte(void) {
	return Rand() >> 8;
}

planet_t planets[64];
coords_t recent_planets[128];
coords_t destroyed_panets[256];

byte in_exterior = 0;
uint16 num_destroyed;
uint16 total_destroyed;
uint16 current_alien;
uint16 current_alien_img;
byte planet_flag;
int num_recent;
uint16 last_alien_img;

void RandomizeGalaxy(void) {
	int i, j;
	rand_seed = 1234;	/*TODO: this is initialized from random memory*/

	for (i = 0;i < 64;i++) {
		byte x, y;
again:
		x = RandByte();
		y = RandByte() & ~PLF_80;

		planets[i].coords1.x = x;
		planets[i].coords1.y = y;
		planets[i].coords2.x = x;
		planets[i].coords2.y = y;

		for (j = 0;j < i;j++) {
			if (planets[j].coords1.x == x && planets[j].coords1.x == y)
				goto again;
		}
	}

	i = (RandByte() % 4) * 4;
	ship_x = planets[i].coords1.x;
	ship_y = planets[i].coords1.y;

	for (i = 40;i <= 49;i++) {
		SetBit(i);
	}

	ClrBit(999);

	total_destroyed = 0;
	num_destroyed = 0;
	num_recent = 0;
	last_alien_img = 0xFFFF;
}

void MarkRecent(void) {
	recent_planets[num_recent].x = ship_x;
	recent_planets[num_recent].y = ship_y;
	num_recent++;
	if (num_recent >= 128)
		num_recent = 0;
}

bool IsRecentPlanet(void) {
	int i;
	for (i = 0;i < num_recent;i++) {
		if (recent_planets[i].x == ship_x && recent_planets[i].y == ship_y)
			return true;
	}
	return false;
}

bool IsPlanetHere(void) {
	int i;
	current_alien = 0xFFFF;
	current_alien_img = 0xFFFF;
	planet_flag = 0;
	for (i = 0;i < num_destroyed;i++) {
		if (destroyed_panets[i].x == ship_x && destroyed_panets[i].y == ship_y)
			return false;
	}

	for (i = 0;i < 64;i++) {
		if (planets[i].coords2.x == ship_x && planets[i].coords2.y == ship_y) {
			current_alien = i;
			current_alien_img = i / 4;
			ClrBit100(i, 2);
			if (planets[i].coords1.x == ship_x && planets[i].coords1.y == ship_y)
				SetBit100(i, 2);
			if (GetBit100(i, 4))
				planet_flag = 1;
			return true;
		}
	}

	planet_flag = ship_x & 1;
	return true;
}



void Disintegrate(void) {

}

void LoadSave(void) {

}


/*
Draw 3-color band
*/
void DrawThreeLines(uint16 sx, uint16 sy, uint16 ex, uint16 ey, byte *buffer) {
	CGA_HLine(sx, sy,     ex, 3, buffer);
	CGA_HLine(sx, sy - 1, ex, 2, buffer);
	CGA_HLine(sx, sy + 1, ex, 1, buffer);
}

/*
Erase 3-color band
*/
void ClearThreeLines(uint16 sx, uint16 sy, uint16 ex, uint16 ey, byte *buffer) {
	CGA_HLine(sx, sy,     ex, 0, buffer);
	CGA_HLine(sx, sy - 1, ex, 0, buffer);
	CGA_HLine(sx, sy + 1, ex, 0, buffer);
}

/*
Draw 3-pixels high oscilating line
*/
void WavyLine(uint16 x, uint16 y, uint16 w, byte color, byte *buffer) {
	uint16 i;
	byte prev = 1;
	ClearThreeLines(x, y, x + w - 1, y, buffer);

	for (i = 0;i < w;i++) {
		char dy = (RandByte() % 4) - 2;
		if (dy == -2)
			dy = 0;
		/* 0, -1, 0, 1 */

		if (prev + dy > 2) {
			dy = -dy;
		}
		prev += dy;

		CGA_PutPixel(x + i, y + dy, color, buffer);
	}
}

void WavyBorder(uint16 x, uint16 y1, uint16 y2, uint16 w, byte color, byte *buffer) {
	int i;

	for (i = 0;i < 3;i++) {
		WavyLine(x, y1, w, color, buffer);
		WavyLine(x, y2, w, color, buffer);
		Idle(1);
	}

	ClearThreeLines(x, y1, x + w - 1, y1, buffer);
	ClearThreeLines(x, y2, x + w - 1, y2, buffer);
}

void XorTwoPixels(uint16 x, uint16 y, byte color, byte *buffer) {
	if (x >= 320 - 1 || y < 20 || y >= 146)
		return;
	CGA_XorPixel(x, y, color, buffer);
	CGA_XorPixel(x + 1, y, color, buffer);
}

void DrawCirclePixels(uint16 x, uint16 y, uint16 d1, uint16 d2, byte color, byte *buffer) {
	XorTwoPixels(x + d1 * 2, y + d2, color, buffer);
	XorTwoPixels(x - d1 * 2, y + d2, color, buffer);
	XorTwoPixels(x - d1 * 2, y - d2, color, buffer);
	XorTwoPixels(x + d1 * 2, y - d2, color, buffer);

	XorTwoPixels(x + d2 * 2, y + d1, color, buffer);
	XorTwoPixels(x - d2 * 2, y + d1, color, buffer);
	XorTwoPixels(x - d2 * 2, y - d1, color, buffer);
	XorTwoPixels(x + d2 * 2, y - d1, color, buffer);
}

/*
Draw a circle using Bresenham’s algorithm
*/
void DrawCircle(uint16 xc, uint16 yc, uint16 r, byte color, byte *buffer) {
	uint16 rr = xc * r;
	uint16 r1 = rr;
	uint16 r2 = 0;
	uint16 d1 = r;
	uint16 d2 = 0;
	for (;;) {
		DrawCirclePixels(xc, yc, d1, d2, color, buffer);
		if (d1 < d2)
			break;
		r2 += d2 * 2;
		r2 += 1;
		d2 += 1;
		if (r1 + r2 >= rr) {
			r1 -= d1 * 2;
			r1 += 1;
			d1 -= 1;
		}		
	}
}

void ThreeCirclesRipple(byte *buffer) {
	int i;
	for (i = 10;i < 100;i += 20) {
		DrawCircle(160, 82, i + 0, 3, buffer);
		DrawCircle(160, 82, i + 2, 3, buffer);
		DrawCircle(160, 82, i + 4, 3, buffer);
		Idle(1);
		DrawCircle(160, 82, i + 0, 3, buffer);
		DrawCircle(160, 82, i + 2, 3, buffer);
		DrawCircle(160, 82, i + 4, 3, buffer);
	}
}

void DrawRippleAndBorders(void) {
	clip_sy = 149;
	ThreeCirclesRipple(frontbuffer);
	WavyBorder(0, 18, 147, 320, 3, frontbuffer);
	DrawThreeLines(0,  18, 319,  18, frontbuffer);
	DrawThreeLines(0, 147, 319, 147, frontbuffer);
}

void DrawExterior(void) {
	CGA_CopyRect(wseg_8_backbuffer3, 0, 20, CGAW(320), 126, frontbuffer);
}

void DestoryPlanet(void) {
	/*TODO*/
}

void DoPhoto(void) {
	/*TODO*/
}

void DoContact(void) {
	/*TODO*/
}

void DoExterior(void) {
	int butt;
	uint16 old_butt;

	if (!IsPlanetHere()) {
		DrawExterior();
on_destroyed:
		DrawDashButtons(0);
		do {
			Idle(0);
		} while (GetDashButton() == NotClicked);
		active_buttons = (active_buttons & 0x100) | 0xE0;
		GoInterior();
		return;
	}

	DrawExterior();
	DrawDashButtons(0xE);

	PlanetPreInit();
	PlanetInit();
	if (!in_exterior) {
		in_exterior++;
		do {
			Idle(2);
		} while (!PlanetFrame());
		/*TODO: some unreachable code here*/
	}

	/*buttons loop*/
	for (;;) {
		do {
			PlanetFrameFastCalc();
			Idle(0);
			butt = GetDashButton();
			PlanetFrameFastDraw(frontbuffer);
			if (butt != NotClicked)
				break;
			Idle(0);
			butt = GetDashButton();
		} while (butt == NotClicked);

		switch (butt) {
		case 6:	/*contact*/
			old_butt = active_buttons;
			active_buttons = 0;
			GoInterior();
			active_buttons = old_butt;
			DoContact();
			return;
		case 7: /*destroy*/
			DestoryPlanet();
			goto on_destroyed;
		case 8: /*photo*/
			DoPhoto();
			continue;
		case NoButton:
			BackToShip();
			return;
		}
	}
}

void GoExterior(void) {
	DrawDashButtons(0xE0);
	show_time = 0;
	DrawRippleAndBorders();
	CGA_FillRect(0, 0, 20, CGAW(320), 126, wseg_8_backbuffer3);

	/*TODO: reinit rand_seed here again*/
	DrawStars(InitStarfield(320 / 2, 83, 511, 12), 0, wseg_8_backbuffer3);

	DoExterior();
}

void GoGalaxy(void) {

}

void Reactivate(void) {

}

void AnimDisintegrate(void) {

}

uint16 ticks_6_prev = 0;

void Idle(int ticks) {
	if (ticks == 0) {
		if (ticks_6 == ticks_6_prev)
			return;
		UpdateHand();
		DrawTime(frontbuffer);
	} else {
		uint16 now, end = ticks_6_prev + ticks; 
		UpdateHand();
		DrawTime(frontbuffer);
		if (end >= ticks_6) {
			do {
				for (now = ticks_6;now == ticks_6;) ;
			} while (end > ticks_6);
		}
	}
	ticks_6_prev = ticks_6;
}

int main(int argc, char **argv) {
	InitGame();
	RandomizeGalaxy();
	DrawShipCoords(frontbuffer);
	in_exterior++;
	GoExterior();
	for (;;) {
		int butt;
		Idle(1);
		butt = GetDashButton();
#if 1
		if (butt != NotClicked)
			printf("clicked button %d\n", butt);
#endif
		switch (butt) {
		case 1:
			if (CanDisintegrate()) {
				Disintegrate();
				AnimDisintegrate();
			}
			break;
		case 2:
			LoadSave();
			break;
		case 3:
			GoExterior();
			break;
		case 4:
			GoGalaxy();
			break;
		case 9:
			Reactivate();
			break;
		}
	}

	return 0;
}
