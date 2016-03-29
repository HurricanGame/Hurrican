// Datei : Mathematics.h

// --------------------------------------------------------------------------------------
//
// Mathe-Hilfsfuktionen
//
// (c) 2004
// Jörg M. Winterstein
// http://www.poke53280.de
//
// --------------------------------------------------------------------------------------

#ifndef __Mathematics_h__
#define __Mathematics_h__
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
#define srand(x) seed_fast_rand(x)
#endif //USE_FAST_RNG

// --------------------------------------------------------------------------------------
// BEGIN GENERAL TRIG SECTION
// --------------------------------------------------------------------------------------
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define PI              3.14159265358979f           // Pi =)

//DKS - RAD/DEG conversions:
// Multiply by a constant ratio (these save a division):
#define DegToRad(x) ((x) * float(double(M_PI) / double(180.0)))
#define RadToDeg(x) ((x) * float(double(180.0) / double(M_PI)))

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
// Remez minimax polynomial approximation of sin(x):
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
static float SineRemezRad(float x)
{
    const float pi_over_two = float(M_PI / 2.0L);
    const float two_over_pi = float(2.0L / M_PI);
    bool negate_result;

    if (x < 0) {
        x = -x;
        negate_result = true;
    } else {
        negate_result = false;
    }

    if (x > pi_over_two) {
        float x_div_half_pi = x * two_over_pi;
        int quotient = int(x_div_half_pi);
        float x_past_quad = x - float(quotient) * pi_over_two;

        switch (quotient % 4) { 
            default: // Default case shouldn't happen; fall through

            case 0: // x is < PI/2
                x = x_past_quad;
                break;
            case 1: // x is < PI
                x = pi_over_two - x_past_quad;
                break;
            case 2: // x is < 3*PI/2
                x = x_past_quad;
                negate_result = !negate_result;
                break;
            case 3: // x is < 2*PI
                x = pi_over_two - x_past_quad;
                negate_result = !negate_result;
                break;
        }
    }

    // Coefficients generated using http://lolengine.net/wiki/oss/lolremez
    // Specifically, these were generated using the method outlined on:
    // http://lolengine.net/wiki/doc/maths/remez/tutorial-fixing-parameters
    // whereby, the first coefficient (a1) is forced to 1.0 and eliminated
    const float a3 = -1.660786242169313753522239789881311891420e-1;
    const float a5 = 7.633773374658546665581958822838108771028e-3;
    float x2 = x*x;
    float x3 = x2*x;
    float result = x + x3*a3 + (x2*x3)*a5;

    //NOTE: results can be slightly over 1.0, i.e. 1.0004, but that is
    //      close enough for our purposes in this game. We won't bother
    //      clamping to +/- 1.0 here..

    return negate_result ? -result : result;
}

// Unnamed namespace to ensure these are isolated to a given translation unit:
namespace
{

inline float sin_deg(const float deg)
{
    return SineRemezRad(DegToRad(deg));
}

inline float cos_deg(const float deg)
{
    return SineRemezRad(DegToRad(deg + 90.0f));
}

inline float sin_rad(const float rad)
{
    return SineRemezRad(rad);
}

inline float cos_rad(const float rad)
{
    return SineRemezRad(rad+float(M_PI/2.0L));
}

} // Unnamed namespace

#else
// Don't use fast trig, use wrappers around libm:

namespace
{

inline float cos_deg(const float deg)
{
    return cosf(DegToRad(deg)); 
}

inline float sin_deg(const float deg)
{
    return sinf(DegToRad(deg)); 
}

inline float cos_rad(const float rad)
{
    return cosf(rad);
}

inline float sin_rad(const float rad)
{
    return sinf(rad);
}

} // Unnamed namespace

#endif //USE_FAST_TRIG
// --------------------------------------------------------------------------------------
// END FAST TRIG SECTION 
// --------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------
// BEGIN MATRIX MATH SECTION
// --------------------------------------------------------------------------------------
// - DKS -  Since the game is purely 2D, and all rotations are around the Z axis,
//         matrix math can be greatly simplified and it's now done entirely on the CPU.
//         Only perspective math is done in the GPU now, which never changed in the
//         game. This means there is a grand total of one matrix uploaded to the GPU,
//         at game startup. This results in not just a reduction in bandwidth demands:
//          Because all vertex transformations are now done by the CPU, huge batches of
//         primitives can now be drawn with one API call, as long as they share
//         the same texture and blend mode, greatly reducing API overhead.

// This struct holds all that we ever need for 2D vertex transforms
// NOTE: The missing row 2 is left out on purpose. If we were doing traditional
//       3D matrix math, it would represent the Z row.
struct ReducedMatrix
{
    float row0_col0;        float row0_col1;     // X row
    float row1_col0;        float row1_col1;     // Y row
    float row3_col0;        float row3_col1;     // Affine translation X, Y 
};

// Unnamed namespace to ensure function definitions are local to a translation unit:
namespace
{
// Transform X coord of a pair of X,Y coordinates by given matrix
inline float RM_X(const float x, const float y, const ReducedMatrix &mat)
{
    return x*mat.row0_col0 + y*mat.row1_col0 + mat.row3_col0;
}

// Transform Y coord of a pair of X,Y coordinates by given matrix
inline float RM_Y(const float x, const float y, const ReducedMatrix &mat)
{
    return x*mat.row0_col1 + y*mat.row1_col1 + mat.row3_col1;
}

// Identity:
inline const ReducedMatrix RM_Ident()
{
    ReducedMatrix mat;
    mat.row0_col0 = 1.0f;    mat.row0_col1 = 0.0f;
    mat.row1_col0 = 0.0f;    mat.row1_col1 = 1.0f;
    mat.row3_col0 = 0.0f;    mat.row3_col1 = 0.0f; //Note: row2_col2 is always assumed to be 1.0 in our reduced matrices
    return mat;
}

//DKS - Not needed, but kept for posterity. All scaling in the
//      game was done in individual functions in DX8Sprite.cpp.
// Scale:
inline const ReducedMatrix RM_Scale(const float scale)
{
    ReducedMatrix mat;
    mat.row0_col0 = scale;   mat.row0_col1 = 0.0f;
    mat.row1_col0 = 0.0f;    mat.row1_col1 = scale;
    mat.row3_col0 = 0.0f;    mat.row3_col1 = 0.0f;
    return mat;
}

// Translation:
inline const ReducedMatrix RM_Trans(const float x, const float y)
{
    ReducedMatrix mat;
    mat.row0_col0 = 1.0f;    mat.row0_col1 = 0.0f;
    mat.row1_col0 = 0.0f;    mat.row1_col1 = 1.0f;
    mat.row3_col0 = x;       mat.row3_col1 = y;
    return mat;
}
// Rotation by radians around Z axis:
inline const ReducedMatrix RM_RotZRad(const float rad)
{
    ReducedMatrix mat;
    float s = sin_rad(rad);
    float c = cos_rad(rad);
    mat.row0_col0 = c;       mat.row0_col1 = s;
    mat.row1_col0 = -s;      mat.row1_col1 = c;
    mat.row3_col0 = 0.0f;    mat.row3_col1 = 0.0f;
    return mat;
}

// Rotation by float degrees around Z axis:
inline const ReducedMatrix RM_RotZDeg(const float deg)
{
    ReducedMatrix mat;
    float s = sin_deg(deg);
    float c = cos_deg(deg);
    mat.row0_col0 = c;       mat.row0_col1 = s;
    mat.row1_col0 = -s;      mat.row1_col1 = c;
    mat.row3_col0 = 0.0f;    mat.row3_col1 = 0.0f;
    return mat;
}

// Version of above that forces use of libm sin/cos, but isn't needed
// now that we use Remez sin/cos polynomial approximation
inline const ReducedMatrix RM_RotZDegAccurate(const float deg)
{
    ReducedMatrix mat;
    float s = sinf(DegToRad(deg));              // Use libm trig
    float c = cosf(DegToRad(deg));              // Use libm trig
    mat.row0_col0 = c;       mat.row0_col1 = s;
    mat.row1_col0 = -s;      mat.row1_col1 = c;
    mat.row3_col0 = 0.0f;    mat.row3_col1 = 0.0f;
    return mat;
}

// ** Note: the following rotation functions are all versions of the above ones, taking
//    x,y parameters that specify the center of rotation. It is more efficient
//    to generate a rotation matrix this way than generating a translation-to-origin
//    matrix, multiplying it by a standard rotation matrix, and then multiplying the
//    result by a translate-back-from-origin matrix. These are the result of working it
//    all out by hand, cancelling terms that end up zero, and extracting common
//    factors in final row's math.

// Rotation by radians around Z axis (with center of rotation at specified point)
inline const ReducedMatrix RM_RotZRadAt(const float rad, const float x, const float y)
{
    ReducedMatrix mat;
    float s = sin_rad(rad);
    float c = cos_rad(rad);
    mat.row0_col0 = c;                   mat.row0_col1 = s;
    mat.row1_col0 = -s;                  mat.row1_col1 = c;
    mat.row3_col0 = y*s - x*c + x;       mat.row3_col1 = x*(-s) - y*c + y;
    return mat;
}

// Rotation by float degrees around Z axis (with center of rotation at specified point)
inline const ReducedMatrix RM_RotZDegAt(const float deg, const float x, const float y)
{
    ReducedMatrix mat;
    float s = sin_deg(deg);
    float c = cos_deg(deg);
    mat.row0_col0 = c;                   mat.row0_col1 = s;
    mat.row1_col0 = -s;                  mat.row1_col1 = c;
    mat.row3_col0 = y*s - x*c + x;       mat.row3_col1 = x*(-s) - y*c + y;
    return mat;
}

// Version of above that forces use of libm sin/cos, but isn't needed
// now that we use Remez sin/cos polynomial approximation
inline const ReducedMatrix RM_RotZDegAccurateAt(const float deg, const float x, const float y)
{
    ReducedMatrix mat;
    float s = sinf(DegToRad(deg));
    float c = cosf(DegToRad(deg));
    mat.row0_col0 = c;                   mat.row0_col1 = s;
    mat.row1_col0 = -s;                  mat.row1_col1 = c;
    mat.row3_col0 = y*s - x*c + x;       mat.row3_col1 = x*(-s) - y*c + y;
    return mat;
}

// Matrix multiply
inline const ReducedMatrix operator*(const ReducedMatrix &lh, const ReducedMatrix &rh)
{
    ReducedMatrix mat;
    mat.row0_col0 = lh.row0_col0 * rh.row0_col0 + lh.row0_col1 * rh.row1_col0;
    mat.row0_col1 = lh.row0_col0 * rh.row0_col1 + lh.row0_col1 * rh.row1_col1;
    mat.row1_col0 = lh.row1_col0 * rh.row0_col0 + lh.row1_col1 * rh.row1_col0;
    mat.row1_col1 = lh.row1_col0 * rh.row0_col1 + lh.row1_col1 * rh.row1_col1;
    mat.row3_col0 = lh.row3_col0 * rh.row0_col0 + lh.row3_col1 * rh.row1_col0 + rh.row3_col0;
    mat.row3_col1 = lh.row3_col0 * rh.row0_col1 + lh.row3_col1 * rh.row1_col1 + rh.row3_col1;
    return mat;
}

} //Unnamed namespace
// --------------------------------------------------------------------------------------
// BEGIN END MATRIX MATH SECTION
// --------------------------------------------------------------------------------------

#endif // __Mathematics_h__
