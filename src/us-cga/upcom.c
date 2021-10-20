#include "common.h"
#include "upcom.h"
#include "game.h"
#include "hand.h"


void DoMyPhrase(void) {

}


void GetFingerPos(uint16 *x, uint16 *y) {
	*x = hand_x + 3;
	*y = hand_y + 1;
}

void Upcom(void) {
	InitUpcom();
	DrawDashButtons(0);
	show_time = 1;
	clip_sy = 108;
	CGA_CopyRect(tableau_data, 0, 108, CGAW(320), 92, wseg_8_backbuffer3);
	DrawThreeLines(16, 108, 144, wseg_8_backbuffer3);
	DrawThreeLines(16, 133, 144, wseg_8_backbuffer3);

	DrawThreeLines(176, 108, 304, wseg_8_backbuffer3);
	DrawThreeLines(176, 133, 304, wseg_8_backbuffer3);

	DrawThreeLines(51, 176, 269, wseg_8_backbuffer3);
	DrawThreeLines(51, 177, 269, wseg_8_backbuffer3);

	CGA_FillRect(0, 48, 135, CGAW(224), 41, wseg_8_backbuffer3);
	CGA_FillRect(0, 176, 110, CGAW(128), 22, wseg_8_backbuffer3);
	CGA_FillRect(0, 16, 110, CGAW(128), 22, wseg_8_backbuffer3);

	CGA_GrabRect(wseg_5_1000, 176, 130, CGAW(144), 6, wseg_8_backbuffer3);
	CopyRectWithHand(wseg_8_backbuffer3, 0, 100, CGAW(320), 99);
	last_time_draw = ~0;
	DrawTime(frontbuffer);
	/*TODO*/
	DrawIconsLine();
	inp_cursor_pos = ~0;
	ClearMyPhrase();
	DrawMyPhrase();
	ClearAlienPhrase();
	HideIconText();
	/*TODO*/
	goto alien_response;
	for (;;) {
		uint16 x, y;

		Idle(1);

		/*scroll buttons*/
		GetFingerPos(&cx, &cy);
		if (y >= 180 && y <= 200) {
			if (x >= 24 && x <= 56)
				IconsScrollLeft();
			else if (x >= 264 && x <= 296)
				IconsScrollRight();
		}

		/*scroller*/
		GetFingerPos(&x, &y);
		if (hand_cycle2 && y >= 184 && y <= 200 && x >= 56 && x < 264) {
			uint16 sp = 0;
			if (x >= 72 && x <= 72 + 20)
				sp = (x - 72) / 3 * 2;
			if (sp < 2)
				sp = 2;
			if (sp >= 92)
				sp = 94;
			scroll_pos = sp;
			DrawIconsLine();	
		}

		/*TODO*/

		/*icons*/		
		GetFingerPos(&x, &y);
		if (y >= 137 && y <= 175 && x >= 48 && x < 272) {
			/*icons palette*/		
			uint16 nx = x / 4 - 48 / 4;
			uint16 ny = scroll_pos;
			if (y >= 156)
				ny++;
			if (nx < icons_base)
				icon_hover_index = ny - 2;
			else
				icon_hover_index = ny + (nx - icons_base) / 4 * 2;
			icon_hover_type = 0;
		} else if (y >= 112 && y < 130 && x >= 176 && x < 304) {
			/*my phrase*/
			icon_hover_type = 1;
			icon_hover_index = my_phrase[(x - 176) / 16];
		} else if (y >= 112 && y < 130 && x >= 16 && x < 144) {
			/*alien phrase*/
			icon_hover_type = 2;
			icon_hover_index = alien_phrase[(x - 16) / 16];
		}

		icon_valid = icon_hover_index;
		if (icon_hover_index == 111)
			desc = 123
		else {
			desc = logic->dic.icon_flags[icon_hover_index];
			if (desc & 0x80) {
				icon_valid = 0xFF;
				desc &= ~0x80;
			}
		}

		if (desc != icon_desc) {
			icon_desc = desc;
			ClearIconText();
			if (icon_desc != 0xFF) {
				byte i;
				byte *text = logic->dic.words[icon_desc] + 1;
				for (i = 0;text[i];i++) {
					icon_text[i] = text[i];
				}
			}
			DrawIconText();
		}

		/*back to ship button*/
		if (hand_cycle2 != 0) {
			GetFingerPos(&x, &y);
			if (y >= 156 && y < 168 && x >= 280 && x < 300)
				break;
		}

		/*click my phrase - move cursor*/
		if (hand_cycle2 != 0) {
			GetFingerPos(&x, &y);
			if (y >= 112 && y < 135 && x >= 176 && x < 304) {
				uint16 pos = (x - 176) / 16;
				if (pos != inp_cursor_pos && pos <= my_phrase_len) {
					DrawInputCursor(pos);
					DrawMyPhrase();
				}
			}
		}

		/*left/right click - enter icon*/
		if (new_keys & (0x80 | 0x40)) {
			if (debounce == 0) {
				debounce = 1;
				if (icon_valid != 0xFF && icon_hover_type != 1 && icon_hover_type != 2 && my_phrase_len < 8) {
					/*shift tail and insert word at current cursor pos*/
					byte i = 15;
					do {
						my_phrase[i] = my_phrase[i - 1];
					} while (--i != inp_cursor_pos);
					my_phrase[i] = icon_valid;
					if (inp_cursor_pos != 7)
						DrawInputCursor(inp_cursor_pos + 1);
					my_phrase_len++;
					DrawMyPhrase();
				}
			}
		} else
			debounce = 0;

		/*backspace button*/
		if (hand_cycle2 != 0) {
			GetFingerPos(&x, &y);
			if (y >= 136 && y < 148 && x >= 280 && x < 300 && inp_cursor_pos != 0) {
				if (inp_cursor_pos != 7 || my_phrase_len != 8)
					DrawInputCursor(inp_cursor_pos - 1);
				/*shift tail*/
				byte i = inp_cursor_pos;
				do {
					my_phrase[i] = my_phrase[i + 1];
				} while (++i != 14);
				my_phrase_len--;
				DrawMyPhrase();
			}
		}

		DrawMouth();

		/*backspace button*/
		if (hand_cycle2 != 0) {
			GetFingerPos(&x, &y);
			if (y >= 107 && y <= 127 && x >= 144 && x <= 176 && inp_cursor_pos != 0) {
				if (end_talk)
					break;
				ClearAlienPhrase();
				DoMyPhrase();
alien_response:
				DrawUpcomAlienPhrase();
				if (end_talk) {
					DrawShipInteriorPartial(0, 17, CGAW(320), 90);
					DrawIconText();
				}
			}
		}

		/*what's next*/
		if (GetBit(6) == 0) {
			if (dash_mode != 0) {
				dash_mode = 0;
				DrawDashButtons(0);
			}  
		} else {
			if (dash_mode == 0) {
				dash_mode = 1;
				if (CanDisintegrate())
					DrawDashButtons(0x200);
			}
		}

		butt = GetDashButton();
		if (butt != NotClicked && butt == 0 && !end_talk) {
			active_buttons |= 0x100;
			DrawShipInteriorNoButtons();
			PutToFridge();
			/*TODO*/
			if (GetBit(999) == 0)
				return;

			AnimDisintegrate();
			CGA_DrawSprite(91, 40, 65, main_data, wseg_8_backbuffer3);
			CopyRectWithHand(wseg_8_backbuffer3, 40, 65, CGAW(160), 100);
			steady_hand = 1;
			/*TODO: hang?*/
			for (;;) {
				DrawDashButtons(0);
				DrawDashButtons(0x3FF);
			}
		}

		DrawUpcomBackground();		
	}

	DrawShipInteriorNoButtons();
}

void DoUpcom(void) {
	int butt;

	if (current_alien != -1) {
		if (GetBit100(current_alien, 0) && GetBit100(current_alien, 3) && (!CanDisintegrate() || current_alien != fridge_alien)) {
			Upcom();
			return;
		}
	}

	if (CanDisintegrate()) {
		DrawDashButtons(0x200);
		do {
			Idle(1);
			butt = GetDashButton();
		} while (butt == NotClicked);
		if (butt == 0) {	/*teleport*/
			Unfreeze();
			active_buttons &= 0xFF;
			Upcom();
			return;
		}
	}

	do {
		Idle(1);
	} while (hand_cycle2 == 0);
	DrawShipInteriorNoButtons();
	return;
}
