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
#include <cmath>
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

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define PI				3.14159265358979f			// Pi =)

//DKS - RAD/DEG conversions:
// Multiply by a constant ratio (these save a division):
#define DegToRad(x) ((x) * float(double(M_PI) / double(180.0)))
#define RadToDeg(x) ((x) * float(double(180.0) / double(M_PI)))

#ifdef USE_TRIG_LOOKUP_TABLE
//DKS - New sin-lookup table has enough elements to easily handle cos + sin together:
//      It has 1/4-degree increments.
// Benchmark comparison running on 1GHz 32-bit MIPS GCW Zero w/ Linux+ucLibc:
// NOTE: On Intel, results are less impressive and the lookup table is therefore
//       not helpful nor recommended.
//+------------------------------------------------------------------------------+
//|                               FPBENCH SUMMARY                                |
//+-------------------------------------+-------------------+--------------------+
//| Benchmark description               | ns per operation  | millions of op / s |
//|                                     | (lower = better)  |  (higher = better) |
//+-------------------------------------+-------------------+--------------------+
//| SIN LIBM (float sinf())             |           468.5492|            2.134247|
//| SIN LIBM (double sin())             |           445.6858|            2.243733|
//| SIN LOOKUP (float 1/4-degree acc.)  |            83.7235|           11.944071|
//+-------------------------------------+-------------------+--------------------+

// Override libm's sin/cos (but not sinf/cosf, keep those for when we need accuracy)
#define sin(X) sin_rad((X))
#define cos(X) cos_rad((X))
#define SIN_TABLE_ELEMS ((360+90+1)*4)
extern float sin_table[SIN_TABLE_ELEMS];
extern void  populate_sin_table(void);
extern float cos_deg(int deg);
extern float sin_deg(int deg);
extern float cos_deg(float deg);
extern float sin_deg(float deg);

static inline float cos_rad(float rad)
{
    float deg = RadToDeg(rad);
    return cos_deg(deg);
}

static inline float sin_rad(float rad)
{
    float deg = RadToDeg(rad);
    return sin_deg(deg);
}

#else
// Use libm/libc for trig:
static inline float cos_deg(float deg)
{
    float rad = DegToRad(deg);
    return cosf(rad); 
}

static inline float sin_deg(float deg)
{
    float rad = DegToRad(deg);
    return sinf(rad); 
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
