#ifndef _SOUND_H_
#define _SOUND_H_

extern byte sound_data[1702];

bool LoadSounds(void);

void PlaySound(int index);
void StopSound(void);

#endif