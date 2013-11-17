#ifndef _UTILS_H_
#define _UTILS_H_
#include <inttypes.h>
#include <sstream> // for ostringstream
#include <string>
#include <time.h>

#define RED 		"\033[1;31m"
#define GREEN 		"\033[1;32m"
#define YELLOW		"\033[1;33m"
#define NO_COLOR 	"\033[0m"

inline uint64_t timevalToUINT64(struct timeval t){
	return (uint64_t)1000000 * t.tv_sec + t.tv_usec;
}

#endif // _UTILS_H_