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


//DKS - BEGIN NEW CUSTOM MATRIX MATH:
//      Since the game is purely 2D, and all rotations are around the Z axis,
//      matrix math can be greatly simplified and it's now done on the CPU.
//      Only perspective math is done in the GPU now. This way, you don't 
//      need to send a whole new 4x4 matrix every time you draw something.
//      Because all transformations are now on the CPU, huge batches of
//      primitives can now be drawn with one GL call, as long as they share
//      the same texture and blend mode, greatly reducing graphics API overhead.

// This struct holds all that we ever need to do vertex transforms using matrices. No humongous 4x4 matrix required.
struct ReducedMatrix
{
    float row0_col0;        float row0_col1;     // X row
    float row1_col0;        float row1_col1;     // Y row
    float row3_col0;        float row3_col1;     // Affine translation X, Y 
};

// Translation:
static inline const ReducedMatrix RM_Trans(const float x, const float y)
{
    ReducedMatrix mat;
    mat.row0_col0 = 1.0f;    mat.row0_col1 = 0.0f;
    mat.row1_col0 = 0.0f;    mat.row1_col1 = 1.0f;
    mat.row3_col0 = x;       mat.row3_col1 = y;
    return mat;
}

// Identity:
static inline const ReducedMatrix RM_Ident()
{
    ReducedMatrix mat;
    mat.row0_col0 = 1.0f;    mat.row0_col1 = 0.0f;
    mat.row1_col0 = 0.0f;    mat.row1_col1 = 1.0f;
    mat.row3_col0 = 0.0f;    mat.row3_col1 = 0.0f; //Note: row2_col2 is always assumed to be 1.0 in our reduced matrices
    return mat;
}

// Rotation by radians around Z axis:
static inline const ReducedMatrix RM_RotZRad(const float rad)
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
static inline const ReducedMatrix RM_RotZDeg(const int deg)
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
static inline const ReducedMatrix RM_RotZDeg(const float deg)
{
    ReducedMatrix mat;
    float s = sin_deg(deg);
    float c = cos_deg(deg);
    mat.row0_col0 = c;       mat.row0_col1 = s;
    mat.row1_col0 = -s;      mat.row1_col1 = c;
    mat.row3_col0 = 0.0f;    mat.row3_col1 = 0.0f;
    return mat;
}

// Version of above that forces bypassing of trig lookup table (for edge cases like slow-moving menu background)
static inline const ReducedMatrix RM_RotZDegAccurate(const float deg)
{
    ReducedMatrix mat;
    float s = sinf(DegToRad(deg));
    float c = cosf(DegToRad(deg));
    mat.row0_col0 = c;       mat.row0_col1 = s;
    mat.row1_col0 = -s;      mat.row1_col1 = c;
    mat.row3_col0 = 0.0f;    mat.row3_col1 = 0.0f;
    return mat;
}

// Matrix multiply
static inline const ReducedMatrix operator*(const ReducedMatrix &lh, const ReducedMatrix &rh)
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

// Macros to transform X, Y coordinates when multiplying by a 2x3 'reduced matrix'
// These are for convenience when dealing with code where a full matrix-multiply
// is not practical.
#define RM_X(X,Y,MV_MAT) (((X) * (MV_MAT).row0_col0) + ((Y) * (MV_MAT).row1_col0) + (MV_MAT).row3_col0)
#define RM_Y(X,Y,MV_MAT) (((X) * (MV_MAT).row0_col1) + ((Y) * (MV_MAT).row1_col1) + (MV_MAT).row3_col1)

// Scaling in this game is done manually, no need for this, kept for posterity:
static inline const ReducedMatrix RM_Scale(const float scale)
{
    ReducedMatrix mat;
    mat.row0_col0 = scale;   mat.row0_col1 = 0.0f;
    mat.row1_col0 = 0.0f;    mat.row1_col1 = scale;
    mat.row3_col0 = 0.0f;    mat.row3_col1 = 0.0f;
    return mat;
}
//DKS - END CUSTOM MATRIX MATH

#endif // __Mathematics_h__
