#include "common.h"
#include "cga.h"


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
