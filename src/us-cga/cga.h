#ifndef _CGA_H_
#define _CGA_H_

#include <dos.h>

#define CGA_WIDTH 320
#define CGA_HEIGHT 200
#define CGA_BASE_SEG 0xB800
#define CGA_ODD_LINES_OFS 0x2000
#define CGA_BITS_PER_PIXEL 2
#define CGA_PIXELS_PER_BYTE (8 / CGA_BITS_PER_PIXEL)
#define CGA_BYTES_PER_LINE (CGA_WIDTH / CGA_PIXELS_PER_BYTE)

#define CGAW(x) ((x) / CGA_PIXELS_PER_BYTE)

#ifdef __386__
#define CGA_SCREENBUFFER ((byte*)(CGA_BASE_SEG * 16))
#else
#define CGA_SCREENBUFFER ((byte*)MK_FP(CGA_BASE_SEG, 0))
#endif

#define CGA_SCREEN_SIZE 0x4000

#define CGA_NEXT_LINE(offs) ((CGA_ODD_LINES_OFS ^ (offs)) + (((offs) & CGA_ODD_LINES_OFS) ? 0 : CGA_BYTES_PER_LINE))
#define CGA_PREV_LINE(offs) ((CGA_ODD_LINES_OFS ^ (offs)) - (((offs) & CGA_ODD_LINES_OFS) ? CGA_BYTES_PER_LINE : 0))

#define frontbuffer CGA_SCREENBUFFER

void SwitchToGraphicsMode(void);
void SwitchToTextMode(void);

void CGA_TableauToBuffer3(void);
void CGA_Buffer3ToTableau(void);
void CGA_ScreenToBuffer3(void);
void CGA_Buffer3ToScreen(void);
void CGA_ScreenToBuffer1(void);
void CGA_Buffer1ToScreen(void);
void CGA_Buffer1ToBuffer3(void);
void CGA_Buffer3ToBuffer1(void);

void CGA_Rect3ToScreen(uint16 x, uint16 y, byte w, byte h);

void CGA_CopyRect(byte *source, uint16 x, uint16 y, byte w, byte h, byte *buffer);
void CGA_BlitRect(byte *pixels, uint16 x, uint16 y, byte w, byte h, byte *buffer);
void CGA_GrabRect(byte *pixels, uint16 x, uint16 y, byte w, byte h, byte *buffer);
void CGA_FillRect(byte pixel, uint16 x, uint16 y, byte w, byte h, byte *buffer);
void CGA_ClearScreen(void);

void CGA_BlitCanvasRectRecolor(byte *pixels, uint16 cw, byte color, uint16 x, uint16 y, byte w, byte h, byte *buffer);

void CGA_DotCrossFade(byte *source, uint16 step, byte *target);

void CGA_CopyRectByImage(byte index, uint16 x, uint16 y, byte *bank, byte *source, byte *target);

void CGA_DrawImage(byte index, uint16 x, uint16 y, byte *bank, byte *target);
void CGA_DrawSprite(byte index, uint16 x, uint16 y, byte *bank, byte *target);

uint16 PrintString(uint16 x, uint16 y, char *str, byte *target);

void CGA_DrawHandSprite(byte index, uint16 x, uint16 y, uint16 ey, byte *bank, byte *target);

void CGA_DrawSpriteMask(byte index, uint16 x, uint16 y, byte *bank, byte *source, byte *target);

typedef struct star_t {
	uint16 ofs;
	byte pixel;
	byte mask;
	int16 x;
	int16 y;
	uint16 z;
} star_t;

typedef struct starfield_t {
	uint16 ox;
	uint16 oy;
	uint16 nstars;
	uint16 scale;
	star_t stars[512];
} starfield_t;

starfield_t *InitStarfield(uint16 ox, uint16 oy, uint16 nstars, uint16 scale);
void DrawStars(starfield_t *sf, uint16 zstep, byte *target);


void CGA_Line(uint16 sx, uint16 sy, uint16 ex, uint16 ey, byte color, byte *buffer);
void CGA_HLine(uint16 sx, uint16 sy, uint16 ex, byte color, byte *buffer);

void CGA_PutPixel(uint16 x, uint16 y, byte color, byte *buffer);
void CGA_XorPixel(uint16 x, uint16 y, byte color, byte *buffer);

#endif
