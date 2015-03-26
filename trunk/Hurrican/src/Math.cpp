#include <stdint.h>
#include <cmath>
#include "Math.h"

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


//DKS - Added trig-lookup table with 1/4-deg resolution for speedup:
#ifdef USE_TRIG_LOOKUP_TABLE
float sin_table[SIN_TABLE_ELEMS];

void populate_sin_table(void)
{
   double x = 0.0;
   int i;
   for (i=0; i < SIN_TABLE_ELEMS; i++, x += (double)0.25) {
      sin_table[i] = sinf(float(x * double(M_PI) / double(180.0)));
   }
}

float cos_deg(int deg)
{
    //DKS - TODO remove the range check if we can
    while (deg > 360) {
        deg -= 360;
    }
    while (deg < 0) {
        deg += 360;
    }

    deg += 90; // Read from sin table starting at 90 deg to get cos
    deg *= 4;  // Expand to quarter-deg increments

    return sin_table[deg];
}

float sin_deg(int deg)
{
    //DKS - TODO remove the range check if we can
    while (deg > 360) {
        deg -= 360;
    }
    while (deg < 0) {
        deg += 360;
    }

    deg *= 4;  // Expand to quarter-deg increments
    return sin_table[deg];
}

float cos_deg(float deg)
{
    //DKS - TODO remove the range check if we can
    while (deg > 360.0f) {
        deg -= 360.0f;
    }
    while (deg < 0.0f) {
        deg += 360.0f;
    }

    deg += 90.0f; // Read from sin table starting at 90 deg to get cos
    deg *= 4.0f;  // Expand to quarter-deg increments
    int idx = (int)(deg + 0.5f); // Round to nearest int
    return sin_table[idx];
}

float sin_deg(float deg)
{
    //DKS - TODO remove the range check if we can
    while (deg > 360.0f) {
        deg -= 360.0f;
    }
    while (deg < 0.0f) {
        deg += 360.0f;
    }

    deg *= 4.0f;  // Expand to quarter-deg increments
    int idx = (int)(deg + 0.5f); // Round to nearest int
    return sin_table[idx];
}
#endif //USE_TRIG_LOOKUP_TABLE
