#include <string.h>
#include "common.h"
#include "cga.h"
#include "data.h"

uint16 cga_screen_size = CGA_SCREEN_SIZE;

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

void CGA_Buffer2ToBuffer3(void) {
	CGA_CopyScreen(wseg_7_backbuffer2, wseg_8_backbuffer3);
}

void CGA_Buffer3ToBuffer2(void) {
	CGA_CopyScreen(wseg_8_backbuffer3, wseg_7_backbuffer2);
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
