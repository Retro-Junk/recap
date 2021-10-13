#include <string.h>
#include "common.h"
#include "cga.h"
#include "data.h"

uint16 cga_screen_size = CGA_SCREEN_SIZE;

uint16 cga_lines_ofs[206];

const byte cga_pixel_masks[4][4] = {
	{~0xC0, ~0x30, ~0x0C, ~0x03},
	{~0xC0, ~0x30, ~0x0C, ~0x03},
	{~0xC0, ~0x30, ~0x0C, ~0x03},
	{~0xC0, ~0x30, ~0x0C, ~0x03}
};

const byte cga_pixel_colors[4][4] = {
	{0x00, 0x00, 0x00, 0x00},
	{0x40, 0x10, 0x04, 0x01},
	{0x80, 0x20, 0x08, 0x02},
	{0xC0, 0x30, 0x0C, 0x03}
};

/*
  Switch to CGA 320x200x2bpp mode
*/
void SwitchToGraphicsMode(void) {
#ifdef __386__
	union REGS reg;
	reg.w.ax = 4;
	int386(0x10, &reg, &reg);
#else
	union REGS reg;
	reg.x.ax = 4;
	int86(0x10, &reg, &reg);
#endif

	/*initialize line offsets table*/
	{
		int i, ofs = 0;
		for(i = 0;ofs < CGA_ODD_LINES_OFS;ofs += CGA_BYTES_PER_LINE) {
			cga_lines_ofs[i++] = ofs;
			cga_lines_ofs[i++] = ofs + CGA_ODD_LINES_OFS;
		}
	}
}

/*
  Switch to text mode
*/
void SwitchToTextMode(void) {
#ifdef __386__
	union REGS reg;
	reg.w.ax = 3;
	int386(0x10, &reg, &reg);
#else
	union REGS reg;
	reg.x.ax = 3;
	int86(0x10, &reg, &reg);
#endif
}

void CGA_CopyScreen(byte *source, byte *target) {
	memcpy(target, source, cga_screen_size);
}

void CGA_TableauToBuffer3(void) {
	CGA_CopyScreen(tableau_data, wseg_8_backbuffer3);
}

void CGA_Buffer3ToTableau(void) {
	CGA_CopyScreen(wseg_8_backbuffer3, tableau_data);
}

void CGA_ScreenToBuffer3(void) {
	CGA_CopyScreen(frontbuffer, wseg_8_backbuffer3);
}

void CGA_Buffer3ToScreen(void) {
	CGA_CopyScreen(wseg_8_backbuffer3, frontbuffer);
}

void CGA_ScreenToBuffer1(void) {
	CGA_CopyScreen(frontbuffer, wseg_6_backbuffer1);
}

void CGA_Buffer1ToScreen(void) {
	CGA_CopyScreen(wseg_6_backbuffer1, frontbuffer);
}

void CGA_Buffer1ToBuffer3(void) {
	CGA_CopyScreen(wseg_6_backbuffer1, wseg_8_backbuffer3);
}

void CGA_Buffer3ToBuffer1(void) {
	CGA_CopyScreen(wseg_8_backbuffer3, wseg_6_backbuffer1);
}


/*
Copy area of one CGA frame buffer to another frame buffer
NB! Width is in bytes, not pixels
*/
void CGA_CopyRect(byte *source, uint16 x, uint16 y, byte w, byte h, byte *buffer) {
	uint16 i, ofs;
	/*even lines*/
	ofs = cga_lines_ofs[y] + x / CGA_PIXELS_PER_BYTE;
	for (i = 0;i < h / 2;i++) {
		memcpy(buffer + ofs, source + ofs, w);
		ofs += CGA_BYTES_PER_LINE - w;
	}

	/*odd lines*/
	ofs = cga_lines_ofs[y + 1] + x / CGA_PIXELS_PER_BYTE;
	h -= h / 2;
	for (i = 0;i < h;i++) {
		memcpy(buffer + ofs, source + ofs, w);
		ofs += CGA_BYTES_PER_LINE - w;
	}
}

void CGA_Rect3ToScreen(uint16 x, uint16 y, byte w, byte h) {
	CGA_CopyRect(wseg_8_backbuffer3, x, y, w, h, frontbuffer);
}

/*
Blit interlaced pixels to CGA frame buffer
NB! Width is in bytes, not pixels
*/
void CGA_BlitRect(byte *pixels, uint16 x, uint16 y, byte w, byte h, byte *buffer) {
	uint16 i, ofs;
	/*even lines*/
	ofs = cga_lines_ofs[y] + x / CGA_PIXELS_PER_BYTE;
	for (i = 0;i < h / 2;i++) {
		memcpy(buffer + ofs, pixels, w);
		pixels += w;
		ofs += CGA_BYTES_PER_LINE;
	}

	/*odd lines*/
	ofs = cga_lines_ofs[y + 1] + x / CGA_PIXELS_PER_BYTE;
	for (i = 0;i < h / 2;i++) {
		memcpy(buffer + ofs, pixels, w);
		pixels += w;
		ofs += CGA_BYTES_PER_LINE;
	}
}

/*
Grab interlaced pixels from CGA frame buffer
NB! Width is in bytes, not pixels
*/
void CGA_GrabRect(byte *pixels, uint16 x, uint16 y, byte w, byte h, byte *buffer) {
	uint16 i, ofs;
	/*even lines*/
	ofs = cga_lines_ofs[y] + x / CGA_PIXELS_PER_BYTE;
	for (i = 0;i < h / 2;i++) {
		memcpy(pixels, buffer + ofs, w);
		pixels += w;
		ofs += CGA_BYTES_PER_LINE;
	}

	/*odd lines*/
	ofs = cga_lines_ofs[y + 1] + x / CGA_PIXELS_PER_BYTE;
	for (i = 0;i < h / 2;i++) {
		memcpy(pixels, buffer + ofs, w);
		pixels += w;
		ofs += CGA_BYTES_PER_LINE;
	}
}

/*
Fill CGA frame buffer with pixel (four-pixel)
NB! Width is in bytes, not pixels
*/
void CGA_FillRect(byte pixel, uint16 x, uint16 y, byte w, byte h, byte *buffer) {
	uint16 i, ofs;
	/*even lines*/
	ofs = cga_lines_ofs[y] + x / CGA_PIXELS_PER_BYTE;
	for (i = 0;i < h / 2;i++) {
		memset(buffer + ofs, pixel, w);
		ofs += CGA_BYTES_PER_LINE;
	}

	/*odd lines*/
	ofs = cga_lines_ofs[y + 1] + x / CGA_PIXELS_PER_BYTE;
	for (i = 0;i < h / 2;i++) {
		memset(buffer + ofs, pixel, w);
		ofs += CGA_BYTES_PER_LINE;
	}
}

void CGA_ClearScreen(void) {
	memset(frontbuffer, 0, CGA_SCREEN_SIZE);
}

/*
Replace target screen with source, by drawing each step'th dot
*/
void CGA_DotCrossFade(byte *source, uint16 step, byte *target) {
	uint16 ofs = 0, x, y, o;
	do {
		x = ofs % CGA_WIDTH;
		y = ofs / CGA_WIDTH;
		o = cga_lines_ofs[y] + x / CGA_PIXELS_PER_BYTE;

		target[o] &= cga_pixel_masks[0][x % CGA_PIXELS_PER_BYTE];
		target[o] |= source[o] & cga_pixel_colors[3][x % CGA_PIXELS_PER_BYTE];

		ofs += step;
		if (ofs >= 64000u)
			ofs -= 64000u;
	} while(ofs != 0);
}
