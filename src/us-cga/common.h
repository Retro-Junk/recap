#ifndef _COMMON_H_
#define _COMMON_H_

#include "platform.h"

#define BE(x) (((x) >> 8) | ((x) << 8))
#define LE16(x) (x)

#define TODO(s) \
	{   \
		printf(s);  \
		for(;;) ;   \
	}

#endif
