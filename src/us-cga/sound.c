#include <string.h>
#include "common.h"
#include "sound.h"
#include "data.h"

byte sound_data[1702];

bool LoadSounds(void) {
	if (!LoadFile("sound.bin", wseg_6_backbuffer1))	/*load to temp buffer*/
		return false;

	memcpy(sound_data + 2, wseg_6_backbuffer1, sizeof(sound_data) - 2);

	return true;
}

void PlaySound(int index) {

}

void StopSound(void) {

}

