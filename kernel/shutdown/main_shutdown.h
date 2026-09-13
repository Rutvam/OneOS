#include <stdint.h>
#ifndef MAIN_SHUTDOWN_H
#define MAIN_SHUTDOWN_H

int my_memcmp(uint32_t a, const void *b, long unsigned int n);
void find_RSDP(void);
void main_shutdown (void);

#endif
