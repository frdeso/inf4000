#ifndef _UTILS_H_
#define _UTILS_H_
#include <time.h>

uint64_t timevalToUINT64(struct timeval t){
	return (uint64_t)1000000 * t.tv_sec + t.tv_usec;
}

#endif // _UTILS_H_