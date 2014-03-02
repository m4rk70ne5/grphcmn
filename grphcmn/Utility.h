#ifndef UTILITY_H

#include <random>
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

template<typename T>
T RandomNum(T min, T max)
{
	// finds a position for the cube, between min and max
	using namespace std;

	time_t seed_val = time(NULL);
	// set up random number generator
	default_random_engine randGen;
	uniform_real_distribution<T> distribution(min, max);
	
	randGen.seed(seed_val);
	// generate numbers
	return distribution(randGen);
}

inline double to_rad(float degrees) {
    return degrees*(M_PI / 180.0);
}

#endif