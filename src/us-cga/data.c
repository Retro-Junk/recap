#include <io.h>
#include <fcntl.h>
#include "common.h"
#include "data.h"

byte wseg_0[0x1000];
byte wseg_1[0x5000];
byte bits_pool_seg[0x3000];
byte wseg_3[0x2800];

typedef struct fileheader_t {
byte	mode;
uint16	loadseg;
uint16	loadofs;
uint16	loadlen;
} fileheader_t;

unsigned int LoadFile(const char *filename, byte *buffer) {
	int f;
	int rlen;
	f = open(filename, O_RDONLY | O_BINARY);
	if (f == -1)
		return 0;
	lseek(f, 7, SEEK_SET);	/*skip fileheader*/
	rlen = read(f, buffer, 0xFFF0);
	close(f);
	if (rlen == -1)
		return 0;
	return (unsigned int)rlen;
}
