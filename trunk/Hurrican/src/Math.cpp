#include <stdint.h>
#include <cmath>
#include "Math.h"

#ifdef USE_FAST_RNG
//DKS - added a very fast RNG, here is an example speed comparison on the GCW Zero:
//      (The GCW Zero is a 1GHz 32-bit MIPS device running uclibc Dingux/Linux)
//+------------------------------------------------------------------------------+
//|                               FPBENCH SUMMARY                                |
//+-------------------------------------+-------------------+--------------------+
//| Iterations: 10000                   | Math ops per iteration: 1024           |
//+-------------------------------------+-------------------+--------------------+
//| Benchmark description               | ns per operation  | millions of op / s |
//|                                     | (lower = better)  |  (higher = better) |
//+-------------------------------------+-------------------+--------------------+
//| rand() (32-bit int)                 |           649.9810|            1.538507|
//| fast_rand() (32-bit int)            |             5.1835|          192.921248|
//+-------------------------------------+-------------------+--------------------+
static uint32_t fast_rand_seed_val;

//Used to seed the generator.
void seed_fast_rand(uint32_t seed)
{
   fast_rand_seed_val = seed;
}

// fast_rand routine returns an integer in range 0..32767
int fast_rand(void)
{
   fast_rand_seed_val = (214013 * fast_rand_seed_val + 2531011);
   return (int)((fast_rand_seed_val>>16)&0x7FFF);
}
#endif // USE_FAST_RNG
