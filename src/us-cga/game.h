#ifndef _GAME_H_
#define _GAME_H_

typedef struct coords_t {
	byte x;
	byte y;	/*top bit is a flag*/
} coords_t;

typedef struct planet_t {
	coords_t coords1;
	coords_t coords2;
} planet_t;

#define PLF_80 0x80

extern planet_t planets[64];
extern coords_t recent_planets[128];
extern coords_t destroyed_panets[256];

extern byte in_exterior;
extern uint16 num_destroyed;
extern uint16 total_destroyed;
extern uint16 current_alien;
extern uint16 current_alien_img;
extern int num_recent;
extern uint16 last_alien_img;


uint16 Rand(void);
byte RandByte(void);

void MarkRecent(void);
bool IsRecentPlanet(void);
bool IsPlanetHere(void);

#endif
