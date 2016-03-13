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

//DKS - Added optional trig-lookup table with 1/4-deg resolution for use on
//      platforms whose CPUs lack trigonometric functions:
// Benchmark comparison running on 1GHz 32-bit MIPS GCW Zero w/ Linux+ucLibc:
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
#ifdef USE_TRIG_LOOKUP_TABLE
extern float cos_deg(int deg);
extern float cos_deg(float deg);
extern float sin_deg(int deg);
extern float sin_deg(float deg);
#endif //USE_TRIG_LOOKUP_TABLE

// Unnamed namespace to ensure function definitions are local to a translation unit:
namespace
{

#ifdef USE_TRIG_LOOKUP_TABLE
class TrigTableClass
{
public:
    TrigTableClass()
    {
        double deg_ctr = 0;
        for (size_t i=0; i < SIN_TABLE_SIZE; ++i, deg_ctr += double(0.25))
            sin_table[i] = sinf(float(deg_ctr * double(M_PI) / double(180.0)));
    }

    float sin_int(int deg)
    {
        int frac_deg = deg*4;                 // Expand to quarter-degree increments
        frac_deg = restrict_domain(frac_deg);
        return sin_table[frac_deg]; 
    }

    float sin_float(float deg)
    {
        int frac_deg = int(deg*4.0f + 0.5f);  // Expand to quarter-deg increments and round to nearest int
        frac_deg = restrict_domain(frac_deg);
        return sin_table[frac_deg]; 
    }

    float cos_int(int deg)
    { 
        deg += 90;                            // Read from sin table starting at 90 deg to get cos
        int frac_deg = deg*4;                 // Expand to quarter-degree increments
        frac_deg = restrict_domain(frac_deg);
        return sin_table[frac_deg]; 
    }

    float cos_float(float deg)
    {
        deg += 90.0f;                         // Read from sine table starting at 90 deg to get cosine
        int frac_deg = int(deg*4.0f + 0.5f);  // Expand to quarter-deg increments and round to nearest int
        frac_deg = restrict_domain(frac_deg);
        return sin_table[frac_deg]; 
    }

private:
    int mod(int a, int b)
    {
        return (a < 0 ? (a % b + b) : (a % b));
    }

    //DKS - TODO remove the domain check if we can (will require careful
    //      examination of all calls to sin_rad(),sin_deg(),cos_rad(),cos_deg())
    int restrict_domain(int frac_deg)
    {
        if (frac_deg < 0 || frac_deg >= SIN_TABLE_SIZE)
            frac_deg = mod(frac_deg, 360*4);                // Restrict domain to 0-359.75 degrees

        return frac_deg;
    }

    static const int SIN_TABLE_SIZE = (360+90+1)*4;    // Quater-degree increments and large enough
                                                       //  to handle both sine and cosine. Can handle
                                                       //  inputs 0-360.75 degrees (not just 0-359).
    float sin_table[SIN_TABLE_SIZE];
};

inline float cos_rad(float rad)
{
    return cos_deg(RadToDeg(rad));
}

inline float sin_rad(float rad)
{
    return sin_deg(RadToDeg(rad));
}

#else
// Use libm/libc for trig:
inline float cos_deg(float deg)
{
    return cosf(DegToRad(deg)); 
}

inline float sin_deg(float deg)
{
    return sinf(DegToRad(deg)); 
}

inline float cos_rad(float rad)
{
    return cosf(rad);
}

inline float sin_rad(float rad)
{
    return sinf(rad);
}

#endif //USE_TRIG_LOOKUP_TABLE
} // Unnamed namespace


//DKS - BEGIN NEW CUSTOM MATRIX MATH:
//      Since the game is purely 2D, and all rotations are around the Z axis,
//      matrix math can be greatly simplified and it's now done on the CPU.
//      Only perspective math is done in the GPU now. This way, you don't 
//      need to send a whole new 4x4 matrix every time you draw something.
//      Because all transformations are now on the CPU, huge batches of
//      primitives can now be drawn with one GL call, as long as they share
//      the same texture and blend mode, greatly reducing graphics API overhead.

// This struct holds all that we ever need for 2D vertex transforms
struct ReducedMatrix
{
    float row0_col0;        float row0_col1;     // X row
    float row1_col0;        float row1_col1;     // Y row
    float row3_col0;        float row3_col1;     // Affine translation X, Y 
};

// Macros to transform X,Y coordinates using a 2x3 'reduced matrix'
#define RM_X(X,Y,MAT) (((X) * (MAT).row0_col0) + ((Y) * (MAT).row1_col0) + (MAT).row3_col0)
#define RM_Y(X,Y,MAT) (((X) * (MAT).row0_col1) + ((Y) * (MAT).row1_col1) + (MAT).row3_col1)

// Unnamed namespace to ensure function definitions are local to a translation unit:
namespace
{

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

// Rotation by integer degrees around Z axis:
inline const ReducedMatrix RM_RotZDeg(const int deg)
{
    ReducedMatrix mat;
    float s = sin_deg(deg);
    float c = cos_deg(deg);
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

// Version of above that forces bypassing of trig lookup table, for edge cases like the
//  slow-moving menu background that would reveal its limited resolution.
inline const ReducedMatrix RM_RotZDegAccurate(const float deg)
{
    ReducedMatrix mat;
    float s = sinf(DegToRad(deg));
    float c = cosf(DegToRad(deg));
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
    mat.row3_col0 = y*s - x*(c-1.0f);    mat.row3_col1 = x*(-s) - y*(c-1.0f);
    return mat;
}

// Rotation by integer degrees around Z axis (with center of rotation at specified point)
inline const ReducedMatrix RM_RotZDegAt(const int deg, const float x, const float y)
{
    ReducedMatrix mat;
    float s = sin_deg(deg);
    float c = cos_deg(deg);
    mat.row0_col0 = c;                   mat.row0_col1 = s;
    mat.row1_col0 = -s;                  mat.row1_col1 = c;
    mat.row3_col0 = y*s - x*(c-1.0f);    mat.row3_col1 = x*(-s) - y*(c-1.0f);
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
    mat.row3_col0 = y*s - x*(c-1.0f);    mat.row3_col1 = x*(-s) - y*(c-1.0f);
    return mat;
}

// Version of above that forces bypassing of trig lookup table, for edge cases like the
//  slow-moving menu background that would reveal its limited resolution.
inline const ReducedMatrix RM_RotZDegAccurateAt(const float deg, const float x, const float y)
{
    ReducedMatrix mat;
    float s = sinf(DegToRad(deg));
    float c = cosf(DegToRad(deg));
    mat.row0_col0 = c;                   mat.row0_col1 = s;
    mat.row1_col0 = -s;                  mat.row1_col1 = c;
    mat.row3_col0 = y*s - x*(c-1.0f);    mat.row3_col1 = x*(-s) - y*(c-1.0f);
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
//DKS - END CUSTOM MATRIX MATH

#endif // __Mathematics_h__
