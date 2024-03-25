#ifndef LCG_H
#define LCG_H

// Define parameters for the LCG algorithm
#define LCG_MULTIPLIER 1664525
#define LCG_INCREMENT 1013904223
#define LCG_MODULUS 4294967296  // 2^32

void lcg_seed(unsigned int seed);
unsigned int lcg_rand();

#endif
