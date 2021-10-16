#include <string.h>
#include "common.h"
#include "cga.h"
#include "data.h"
#include "game.h"

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
		ofs += CGA_BYTES_PER_LINE;
	}

	/*odd lines*/
	ofs = cga_lines_ofs[y + 1] + x / CGA_PIXELS_PER_BYTE;
	h -= h / 2;
	for (i = 0;i < h;i++) {
		memcpy(buffer + ofs, source + ofs, w);
		ofs += CGA_BYTES_PER_LINE;
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

/*
Copy rect from source to target, using image's dimensions
*/
void CGA_CopyRectByImage(byte index, uint16 x, uint16 y, byte *bank, byte *source, byte *target) {
	byte sprw, sprh;
	uint16 sprofs;

	if (x >= 320 || y >= 200)
		return;

	sprofs = bank[index * 2];
	sprofs |= bank[index * 2 + 1] << 8;
	if (sprofs == 0)
		return;

	sprw = bank[sprofs];
	sprh = bank[sprofs + 1];

	CGA_CopyRect(source, x, y, sprw, sprh, target);
}

/*
Draw solid image from bank
*/
void CGA_DrawImage(byte index, uint16 x, uint16 y, byte *bank, byte *target) {
	byte sprw, sprh;
	uint16 sprofs, oddsofs, ofs, i;
	byte *sprdata;

	if (x >= 320 || y >= 200)
		return;

	sprofs = bank[index * 2];
	sprofs |= bank[index * 2 + 1] << 8;
	if (sprofs == 0)
		return;

	sprw = bank[sprofs];
	sprh = bank[sprofs + 1];
	oddsofs = bank[sprofs + 4] | (bank[sprofs + 5] << 8);

#if 1
	/*even lines*/
	sprdata = bank + sprofs + 6;
	ofs = cga_lines_ofs[y] + x / CGA_PIXELS_PER_BYTE;
	for (i = 0;i < sprh / 2;i++) {
		memcpy(target + ofs, sprdata, sprw);
		sprdata += sprw;
		ofs += CGA_BYTES_PER_LINE;
	}
#endif

#if 1
	/*odd lines*/
	sprdata = bank + sprofs + 6 + oddsofs;
	ofs = cga_lines_ofs[y + 1] + x / CGA_PIXELS_PER_BYTE;
	for (i = 0;i < sprh / 2;i++) {
		memcpy(target + ofs, sprdata, sprw);
		sprdata += sprw;
		ofs += CGA_BYTES_PER_LINE;
	}
#endif
}

/*
Draw transparent image from bank
*/
void CGA_DrawSprite(byte index, uint16 x, uint16 y, byte *bank, byte *target) {
	byte sprw, sprh;
	uint16 sprofs, maskofs, oddsofs, ofs, i, j;
	byte *sprdata, *sprmask;
	byte *source = wseg_8_backbuffer3;

	if (x >= 320 || y >= 200)
		return;

	sprofs = bank[index * 2];
	sprofs |= bank[index * 2 + 1] << 8;
	if (sprofs == 0)
		return;

	sprw = bank[sprofs];
	sprh = bank[sprofs + 1];
	maskofs = bank[sprofs + 2] | (bank[sprofs + 3] << 8);
	oddsofs = bank[sprofs + 4] | (bank[sprofs + 5] << 8);

#if 1
	/*even lines*/
	sprdata = bank + sprofs + 6;
	sprmask = bank + sprofs + maskofs;
	ofs = cga_lines_ofs[y] + x / CGA_PIXELS_PER_BYTE;
	for (i = 0;i < sprh / 2;i++) {
		for (j = 0;j < sprw;j++) {
			target[ofs + j] = (source[ofs + j] & *sprmask++) | *sprdata++;
		}
		ofs += CGA_BYTES_PER_LINE;
	}
#endif

#if 1
	/*odd lines*/
	sprdata = bank + sprofs + 6 + oddsofs;
	sprmask = bank + sprofs + maskofs + oddsofs;
	ofs = cga_lines_ofs[y + 1] + x / CGA_PIXELS_PER_BYTE;
	for (i = 0;i < sprh / 2;i++) {
		for (j = 0;j < sprw;j++) {
			target[ofs + j] = (source[ofs + j] & *sprmask++) | *sprdata++;
		}
		ofs += CGA_BYTES_PER_LINE;
	}
#endif
}

void PrintChar(byte c, uint16 x, uint16 y, byte *target) {
	if (c < ' ' || c > 'Z')
		return;
	CGA_DrawSprite(c - ' ' + 27, x, y, main_data, target);
}

void PrintString(uint16 x, uint16 y, char *str, byte *target) {
	char c;
	while ((c = *str++) != 0) {
		if (c != 1)
			PrintChar(c, x, y, target);
		x += 8;
	}
}

void CGA_DrawHandSpriteLines(byte shift, byte h, uint16 ofs, byte *sprdata, byte *sprmask, byte *source, byte *target) {
	byte i, j;
	if (shift == 0) {
		/*aligned*/
		for (i = 0;i < h;i++) {
			for (j = 0;j < 32 / 4;j++) {
				target[ofs + j] = (source[ofs + j] & *sprmask++) | *sprdata++;
			}
			ofs += CGA_BYTES_PER_LINE;
		}
	} else {
		/*unaligned*/
		for (i = 0;i < h;i++) {
			uint16 mx = 0xFF00;
			uint16 px = 0x0000;
			for (j = 0;j < 32 / 4;j++) {
				mx |= *sprmask++;
				px |= *sprdata++;
				target[ofs + j] = (source[ofs + j] & (mx >> shift)) | (px >> shift);
				mx <<= 8;
				px <<= 8;
			}
			mx |= 0xFF;
			px |= 0;
			target[ofs + j] = (source[ofs + j] & (mx >> shift)) | (px >> shift);
			ofs += CGA_BYTES_PER_LINE;
		}
	}
}

void CGA_DrawHandSprite(byte index, uint16 x, uint16 y, uint16 ey, byte *bank, byte *target) {
	byte shift;
	byte maxy;

	byte sprw, sprh;
	uint16 sprofs, maskofs, oddsofs, ofs, i, j;
	byte *sprdata, *sprmask;

	if (x >= 320 || y >= 200)
		return;

	shift = (x % CGA_PIXELS_PER_BYTE) * CGA_BITS_PER_PIXEL;
	maxy = (ey > CGA_HEIGHT) ? CGA_HEIGHT : ey;

	sprofs = bank[index * 2];
	sprofs |= bank[index * 2 + 1] << 8;
	if (sprofs == 0)
		return;

	sprw = bank[sprofs];
	sprh = bank[sprofs + 1];
	maskofs = bank[sprofs + 2] | (bank[sprofs + 3] << 8);
	oddsofs = bank[sprofs + 4] | (bank[sprofs + 5] << 8);

	if (maxy - y < sprh)
		sprh = maxy - y;

	/*even lines*/
	sprdata = bank + sprofs + 6;
	sprmask = bank + sprofs + maskofs;
	ofs = cga_lines_ofs[y] + x / CGA_PIXELS_PER_BYTE;
	CGA_DrawHandSpriteLines(shift, (sprh + 1) / 2, ofs, sprdata, sprmask, wseg_8_backbuffer3, target);

	/*odd lines*/
	sprh -= (sprh + 1) / 2;
	if (sprh != 0) {
		sprdata = bank + sprofs + 6 + oddsofs;
		sprmask = bank + sprofs + maskofs + oddsofs;
		ofs = cga_lines_ofs[y + 1] + x / CGA_PIXELS_PER_BYTE;
		CGA_DrawHandSpriteLines(shift, sprh, ofs, sprdata, sprmask, wseg_8_backbuffer3, target);
	}
}

/*
Draw source's pixels to target, if they are not obscured by sprite
*/
void CGA_DrawSpriteMask(byte index, uint16 x, uint16 y, byte *bank, byte *source, byte *target) {
	byte sprw, sprh;
	uint16 sprofs, maskofs, oddsofs, ofs, i, j;
	byte *sprmask;

	if (x >= 320 || y >= 200)
		return;

	sprofs = bank[index * 2];
	sprofs |= bank[index * 2 + 1] << 8;
	if (sprofs == 0)
		return;

	sprw = bank[sprofs];
	sprh = bank[sprofs + 1];
	maskofs = bank[sprofs + 2] | (bank[sprofs + 3] << 8);
	oddsofs = bank[sprofs + 4] | (bank[sprofs + 5] << 8);

	/*TODO: sprw is always CGAW(32) ?*/
	sprw = CGAW(32);

#if 1
	/*even lines*/
	sprmask = bank + sprofs + maskofs;
	ofs = cga_lines_ofs[y] + x / CGA_PIXELS_PER_BYTE;
	for (i = 0;i < (sprh + 1) / 2;i++) {
		for (j = 0;j < sprw;j++) {
			byte m = *sprmask++;
			target[ofs + j] = (target[ofs + j] & ~m) | (source[ofs + j] & m);
		}
		ofs += CGA_BYTES_PER_LINE;
	}
#endif

#if 1
	/*odd lines*/
	sprmask = bank + sprofs + maskofs + oddsofs;
	ofs = cga_lines_ofs[y + 1] + x / CGA_PIXELS_PER_BYTE;
	for (i = 0;i < sprh / 2;i++) {
		for (j = 0;j < sprw;j++) {
			byte m = *sprmask++;
			target[ofs + j] = (target[ofs + j] & ~m) | (source[ofs + j] & m);
		}
		ofs += CGA_BYTES_PER_LINE;
	}
#endif
}


/*
Generate random star
*/
void RandomStar(star_t *star) {
	star->x = Rand();
	star->y = Rand();
	star->z = Rand() & 0xFFF;
}

/*
Generate a bunch of random stars
*/
starfield_t *InitStarfield(uint16 ox, uint16 oy, uint16 nstars, uint16 scale) {
	int i;
#if 0
	starfield_t *sf = (starfield_t *)wseg_5;	/*scratch mem, TODO: is it enough to fit all?*/
#else
	starfield_t *sf = (starfield_t *)wseg_6_backbuffer1;	/*FIXED: use bigger buffer*/
#endif
	sf->ox = ox;
	sf->oy = oy;
	sf->nstars = nstars;
	sf->scale = scale;
	for (i = 0; i < 512; i++) {
		sf->stars[i].ofs = 0;
		sf->stars[i].pixel = 0xFF;
		sf->stars[i].mask = 0xFF;
		RandomStar(&sf->stars[i]);
	}
	return sf;
}

/*
Draw a frame of starfield animation and update stars
*/
void DrawStars(starfield_t *sf, uint16 zstep, byte *target) {
	int i;
	star_t *stars = sf->stars;
	unsigned long zscale = (((unsigned long)sf->scale) << 16) | 0xFFFFu;
	for (i = 0; i < sf->nstars; i++, stars++) {
		short z, x, y;
		byte pixel;

		target[stars->ofs] &= stars->mask;
		if (stars->z < zstep) {
			RandomStar(stars);
			stars->z |= 0x1800;
			continue;
		}

		stars->z -= zstep;
		z = zscale / (stars->z + 16);
		x = ((long)z * stars->x) >> 16;
		y = ((long)z * stars->y) >> 16;
		y /= 2;

		x += sf->ox;
		y += sf->oy;
		if (x < 0 || x >= 320 || y <= 20 || y >= 146) {
			stars->z = 0;
			continue;
		}

		stars->ofs = cga_lines_ofs[y] + x / CGA_PIXELS_PER_BYTE;

		if (stars->z < 1024)
			pixel = 3;
		else if (stars->z < 2560)
			pixel = 1;
		else
			pixel = 2;

		stars->pixel = cga_pixel_colors[pixel][x % CGA_PIXELS_PER_BYTE];
		stars->mask = cga_pixel_masks[pixel][x % CGA_PIXELS_PER_BYTE];

		target[stars->ofs] &= stars->mask;
		target[stars->ofs] |= stars->pixel;
	}
}
