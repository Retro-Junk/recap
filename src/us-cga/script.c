#include "common.h"
#include "script.h"
#include "data.h"

byte bitmasks[] = {1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80};

void ClrBit(uint16 index) {
	bits_pool_seg[index / 8] &= ~bitmasks[index % 8];
}

void SetBit(uint16 index) {
	bits_pool_seg[index / 8] |= bitmasks[index % 8];
}

bool GetBit(uint16 index) {
	return (bits_pool_seg[index / 8] & bitmasks[index % 8]) != 0;
}

void ClrBit100(uint16 index1, uint16 index2) {
	ClrBit(100 + index1 * 5 + index2);
}

void SetBit100(uint16 index1, uint16 index2) {
	SetBit(100 + index1 * 5 + index2);
}

bool GetBit100(uint16 index1, uint16 index2) {
	return GetBit(100 + index1 * 5 + index2);
}
