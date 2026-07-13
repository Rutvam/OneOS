#ifndef FUNCTION_H
#define FUNCTION_H

void seed_random(unsigned int seed);
int randint(void);
void itoa(int num, char* str);
// void print(const char* text, int (*cursor), ...);
void print(const char* text, int (*cursor));

#endif
