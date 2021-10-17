#ifndef _PLANET_H_
#define _PLANET_H_

void PlanetPreInit(void);
void PlanetInit(void);

bool PlanetFrame(void);
void PlanetFrameFastCalc(void);
void PlanetFrameFastDraw(byte *buffer);

#endif
