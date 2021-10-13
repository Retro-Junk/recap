#ifndef _DATA_H_
#define _DATA_H_

extern byte wseg_0[0x1000];
extern byte wseg_1[0x5000];
extern byte bits_pool_seg[0x3000];
extern byte galax_data[0x2800];
extern byte main_data[0x6000];
extern byte wseg_5[0x1000];

extern byte wseg_6_backbuffer1[0x4000];
extern byte tableau_data[0x4000];
extern byte wseg_8_backbuffer3[0x4000];

unsigned int LoadFile(const char *filename, byte *buffer);

#endif
