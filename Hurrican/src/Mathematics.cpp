#include <stdint.h>
#include <cmath>
#include "Mathematics.h"

#ifdef USE_FAST_RNG
//DKS - added a very fast RNG, here is an example speed comparison on the GCW Zero:
//      (The GCW Zero is a 1GHz 32-bit MIPS device running uclibc Dingux/Linux)
//+------------------------------------------------------------------------------+
//|                               FPBENCH SUMMARY                                |
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


//DKS - Added optional trig-lookup table with 1/4-deg resolution for use on
//      platforms whose CPUs lack trigonometric functions:
#ifdef USE_TRIG_LOOKUP_TABLE

static TrigTableClass TrigTable;

float cos_deg(int deg)
{
    return TrigTable.cos_int(deg);
}

float cos_deg(float deg)
{
    return TrigTable.cos_float(deg);
}

float sin_deg(int deg)
{
    return TrigTable.sin_int(deg);
}

float sin_deg(float deg)
{
    return TrigTable.sin_float(deg);
}
#endif //USE_TRIG_LOOKUP_TABLE
