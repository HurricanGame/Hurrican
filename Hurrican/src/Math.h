// Datei : Math.h

// --------------------------------------------------------------------------------------
//
// Mathe-Hilfsfuktionen
//
// (c) 2004
// Jörg M. Winterstein
// http://www.poke53280.de
//
// --------------------------------------------------------------------------------------

#ifndef __Math_h__
#define __Math_h__
#include <stdint.h>
#ifdef PLATFORM_SDL
#include "SDLPort/SDL_port.h"
#endif

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

//DKS - Disabled:
// Schnelle Float to Int Konvertierung
#if 0
#if defined(USE_X86)

#define MYMATH_FTOL(fx,x)\
		_asm fld fx\
		_asm fistp x

#else

#define MYMATH_FTOL(fx,x)  (x) = (long)(fx)

#endif /* defined(USE_X86) */
#endif //0

#define MYMATH_FTOL(fx,x)  (x) = (long)(fx)

// When the following is defined, all calls to rand() end up calling fast_rand()
#ifdef USE_FAST_RNG
extern void seed_fast_rand(uint32_t seed);
extern int fast_rand(void);
#define rand() fast_rand()
#endif //USE_FAST_RNG


#ifdef USE_TRIG_LOOKUP_TABLE
//DKS - New sin-lookup table has enough elements to easily handle cos + sin together:
//      It has 1/4-degree increments.
// Benchmark comparison running on 1GHz GCW Zero w/ Linux+ucLibc:
//+------------------------------------------------------------------------------+
//|                               FPBENCH SUMMARY                                |
//+-------------------------------------+-------------------+--------------------+
//| Benchmark description               | ns per operation  | millions of op / s |
//|                                     | (lower = better)  |  (higher = better) |
//+-------------------------------------+-------------------+--------------------+
//| SIN LIBM (float sinf())             |           462.6650|            2.161391|
//| SIN LIBM (double sin())             |           421.9306|            2.370058|
//| SIN LOOKUP (float)                  |            29.1242|           34.335693|
//+-------------------------------------+-------------------+--------------------+

// Override libm's sin/cos (but not sinf/cosf, keep those for when we need accuracy)
#define sin(X) sin_rad((X))
#define cos(X) cos_rad((X))
#define SIN_TABLE_ELEMS ((360+90+1)*4)
extern float sin_table[SIN_TABLE_ELEMS];
extern void populate_sin_table(void);

static inline float cos_deg(int deg)
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

static inline float sin_deg(int deg)
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

static inline float cos_deg(float deg)
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

static inline float sin_deg(float deg)
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

static inline float cos_rad(float rad)
{
   float deg = rad * (180.0f / (float)D3DX_PI);
   return cos_deg(deg);
}

static inline float sin_rad(float rad)
{
   float deg = rad * (180.0f / (float)D3DX_PI);
   return sin_deg(deg);
}

#else
// Use libm/libc for trig:
static inline float cos_deg(float deg)
{
   deg *= ((float)D3DX_PI / 180.0f);   
   return cosf(deg); 
}

static inline float sin_deg(float deg)
{
   deg *= ((float)D3DX_PI / 180.0f);
   return sinf(deg); 
}

static inline float cos_rad(float rad)
{
   return cosf(rad);
}

static inline float sin_rad(float rad)
{
   return sinf(rad);
}
#endif //USE_TRIG_LOOKUP_TABLE

#endif // __Math_h__
