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

#endif // __Math_h__
