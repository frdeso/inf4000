#ifndef _UTILS_H_
#define _UTILS_H_
#include <sstream> // for ostringstream
#include <string>
#include <time.h>

inline uint64_t timevalToUINT64(struct timeval t){
	return (uint64_t)1000000 * t.tv_sec + t.tv_usec;
}

inline std::string printInRed(std::string s)
{
	std::ostringstream colorised;
	colorised << "\033[1;31m"<<s<<"\033[0m";
	return colorised.str();
}
inline std::string printInGreen(std::string s)
{
	std::ostringstream colorised;
	colorised << "\033[1;32m"<<s<<"\033[0m";
	return colorised.str();
}

#endif // _UTILS_H_