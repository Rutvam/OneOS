#include "function.h"
static unsigned long int next_graine = 1;

void seed_random(unsigned int seed)
{
	next_graine = seed;
}

int randint(void)
{
	next_graine *= 1103515245; 
	next_graine += 12345;
	return (unsigned int)(next_graine / 65536) % 32768;
}
