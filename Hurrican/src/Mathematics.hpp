// Datei : Mathematics.hpp

// --------------------------------------------------------------------------------------
//
// Mathe-Hilfsfuktionen
//
// (c) 2004
// Jörg M. Winterstein
// http://www.poke53280.de
//
// --------------------------------------------------------------------------------------

#ifndef _MATHEMATICS_HPP_
#define _MATHEMATICS_HPP_
#include <stdint.h>
#include <cmath>
#ifdef PLATFORM_SDL
#include "SDLPort/SDL_port.hpp"
#endif

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define MYMATH_FTOL(fx, x) (x) = static_cast<long>(fx)

// DKS - When the following is defined, all calls to rand() end up calling fast_rand()
#ifdef USE_FAST_RNG
extern void seed_fast_rand(uint32_t seed);
extern int fast_rand();
#define rand() fast_rand()
#define srand(x) seed_fast_rand(x)
#endif  // USE_FAST_RNG

inline int random(int max) { return rand() % max; }

// --------------------------------------------------------------------------------------
// BEGIN GENERAL TRIG SECTION
// --------------------------------------------------------------------------------------
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define PI 3.14159265358979f  // Pi =)

// DKS - RAD/DEG conversions:
// Multiply by a constant ratio (these save a division):
inline float DegToRad(float x) { return x * static_cast<float>(M_PI / 180.0); }
inline float RadToDeg(float x) { return x * static_cast<float>(180.0 / M_PI); }

// Override libm's double sin/cos (but not sinf/cosf, keep those for when we need accuracy)
#define sin(X) sin_rad(X)
#define cos(X) cos_rad(X)
// --------------------------------------------------------------------------------------
// END GENERAL TRIG SECTION
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// BEGIN FAST TRIG SECTION
// --------------------------------------------------------------------------------------
#ifdef USE_FAST_TRIG
// DKS - Added Remez minimax polynomial approximation of sin(x):
// https://en.wikipedia.org/wiki/Remez_algorithm
// Coefficients attained via:
// http://lolengine.net/wiki/doc/maths/remez
// Error compared to sinf():
// MAX ERROR: 1.140833e-04   AVG ERROR: 5.777799e-05
// Benchmark results from a 32-bit MIPS GCW Zero 1Ghz w/ hardware FPU + uclibc:
// (Last line of results is this function)
// +-------------------------------------+-------------------+--------------------+
// | Benchmark description               | ns per operation  | millions of op / s |
// |                                     | (lower = better)  |  (higher = better) |
// +-------------------------------------+-------------------+--------------------+
// | SIN LIBM (float sinf())             |           488.4712|            2.047203|
// | SIN LIBM (double sin())             |           460.6874|            2.170669|
// | SIN LOOKUP (float 1/4-degree acc.)  |           112.4062|            8.896305|
// | SIN TINY LOOKUP (float a+b ident)   |           177.5208|            5.633143|
// | SIN APPROX (6th-deg Taylor)         |           145.9791|            6.850294|
// | SIN APPROX (3rd-deg Taylor)         |           124.1250|            8.056397|
// | SIN APPROX (3rd-deg Taylor acc.)    |           126.2031|            7.923736|
// | SIN APPROX (5th-deg Remez minimax)  |           118.1302|            8.465237|
// +-------------------------------------+-------------------+--------------------+
static float SineRemezRad(float x) {
    const float pi_over_two = static_cast<float>(M_PI / 2.0L);
    const float two_over_pi = static_cast<float>(2.0L / M_PI);
    bool negate_result;

    if (x < 0) {
        x = -x;
        negate_result = true;
    } else {
        negate_result = false;
    }

    if (x > pi_over_two) {
        const float x_div_half_pi = x * two_over_pi;
        const int quotient = static_cast<int>(x_div_half_pi);
        const float x_past_quad = x - static_cast<float>(quotient) * pi_over_two;

        switch (quotient % 4) {
            case 0:  // x is < PI/2
                x = x_past_quad;
                break;
            case 1:  // x is < PI
                x = pi_over_two - x_past_quad;
                break;
            case 2:  // x is < 3*PI/2
                x = x_past_quad;
                negate_result = !negate_result;
                break;
            case 3:  // x is < 2*PI
                x = pi_over_two - x_past_quad;
                negate_result = !negate_result;
                break;
        }
    }

    // Coefficients generated using http://lolengine.net/wiki/oss/lolremez
    // Specifically, these were generated using the method outlined on:
    // http://lolengine.net/wiki/doc/maths/remez/tutorial-fixing-parameters
    // whereby, the first coefficient (a1) is forced to 1.0 and eliminated
    constexpr float a3 = -1.660786242169313753522239789881311891420e-1;
    constexpr float a5 = 7.633773374658546665581958822838108771028e-3;
    const float x2 = x * x;
    const float x3 = x2 * x;
    const float result = x + x3 * a3 + (x2 * x3) * a5;

    // NOTE: results can be slightly over 1.0, i.e. 1.0004, but that is
    //      close enough for our purposes in this game. We won't bother
    //      clamping to +/- 1.0 here..

    return negate_result ? -result : result;
}

// Unnamed namespace to ensure these are isolated to a given translation unit:
namespace {

inline float sin_deg(const float deg) {
    return SineRemezRad(DegToRad(deg));
}

inline float cos_deg(const float deg) {
    return SineRemezRad(DegToRad(deg + 90.0f));
}

inline float sin_rad(const float rad) {
    return SineRemezRad(rad);
}

inline float cos_rad(const float rad) {
    return SineRemezRad(rad + static_cast<float>(M_PI / 2.0L));
}

}  // Unnamed namespace

#else  // Normal trig (wrappers around libm sinf/cosf):

namespace {

inline float cos_deg(const float deg) {
    return cosf(DegToRad(deg));
}

inline float sin_deg(const float deg) {
    return sinf(DegToRad(deg));
}

inline float cos_rad(const float rad) {
    return cosf(rad);
}

inline float sin_rad(const float rad) {
    return sinf(rad);
}

}  // Unnamed namespace

#endif  // USE_FAST_TRIG
// --------------------------------------------------------------------------------------
// END FAST TRIG SECTION
// --------------------------------------------------------------------------------------

// - DKS -  Since the game is purely 2D, and all rotations are around the Z axis,
//         matrix math can be greatly simplified and it's now done entirely on the CPU.
//         Only perspective math is done in the GPU now, which never changed in the
//         game. This means there is a grand total of one matrix uploaded to the GPU,
//         at game startup. This results in not just a reduction in bandwidth demands:
//          Because all vertex transformations are now done by the CPU, huge batches of
//         primitives can now be drawn with one API call, as long as they share
//         the same texture and blend mode, greatly reducing API overhead.

#endif  // __Mathematics_h__
