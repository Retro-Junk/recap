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

#ifdef __386__
#define CGA_SCREENBUFFER ((byte*)(CGA_BASE_SEG * 16))
#else
#define CGA_SCREENBUFFER ((byte*)MK_FP(CGA_BASE_SEG, 0))
#endif

#define CGA_NEXT_LINE(offs) ((CGA_ODD_LINES_OFS ^ (offs)) + (((offs) & CGA_ODD_LINES_OFS) ? 0 : CGA_BYTES_PER_LINE))
#define CGA_PREV_LINE(offs) ((CGA_ODD_LINES_OFS ^ (offs)) - (((offs) & CGA_ODD_LINES_OFS) ? CGA_BYTES_PER_LINE : 0))

#define frontbuffer CGA_SCREENBUFFER

void SwitchToGraphicsMode(void);
void SwitchToTextMode(void);


#endif
