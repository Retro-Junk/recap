#ifndef _DATA_H_
#define _DATA_H_

extern byte wseg_0[0x1000];
extern byte wseg_1[0x5000];
extern byte bits_pool_seg[0x3000];
extern byte wseg_3[0x2800];
extern byte wseg_4[0x6000];
extern byte wseg_5[0x1000];

extern byte wseg_6_backbuffer1[0x4000];
extern byte wseg_7_backbuffer2[0x4000];
extern byte wseg_8_backbuffer3[0x4000];

unsigned int LoadFile(const char *filename, byte *buffer);

#endif