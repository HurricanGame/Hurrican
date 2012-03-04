/**
 *
 *  EGLPORT.H
 *  Copyright (C) 2011 Scott R. Smith
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 *
 */

#ifndef EGLPORT_H
#define EGLPORT_H

#include <stdlib.h>
#include <stdint.h>
#include "EGL/egl.h"

#ifdef __cplusplus
extern "C" {
#endif

// Defines (in every case choose only one)
// EGL window system
#if defined(USE_EGL_RAW) && !defined(USE_EGL_SDL)
    //#warning Using RAW EGL mode USE_EGL_RAW
#elif !defined(USE_EGL_RAW) && defined(USE_EGL_SDL)
    //#warning Using SDL EGL mode USE_EGL_SDL
#else	// default configuration
    #define USE_EGL_SDL 1
    #define USE_GLES1 1
#endif
// GLES Version
//  USE_GLES1
//  USE_GLES2
// Platform
//  PANDORA
//  WIZ
//  CAANOO

// External API
void    EGL_Close                   ( void );
int8_t  EGL_Open                    ( void );
int8_t  EGL_Init                    ( void );
void    EGL_SwapBuffers             ( void );

// Internal API
int8_t  ConfigureEGL                ( EGLConfig config );
int8_t  FindAppropriateEGLConfigs   ( void );
int8_t  CheckEGLErrors              ( const char* file, uint16_t line );

void    Platform_Open               ( void );
void    Platform_Close              ( void );
void    Platform_VSync              ( void );

#if defined(_DEBUG)
#define GET_EGLERROR(X)                                     \
    X;                                                      \
    {                                                       \
        CheckEGLErrors(__FILE__, __LINE__);  \
    }
#else
#define GET_EGLERROR(X) X;
#endif

#define peglQueryString(A,B)                    GET_EGLERROR(eglQueryString(A,B))
#define peglDestroyContext(A,B)                 GET_EGLERROR(eglDestroyContext(A,B))
#define peglDestroySurface(A,B)                 GET_EGLERROR(eglDestroySurface(A,B))
#define peglTerminate(A)                        GET_EGLERROR(eglTerminate(A))
#define peglSwapBuffers(A,B)                    GET_EGLERROR(eglSwapBuffers(A,B))
#define peglGetDisplay(A)                       GET_EGLERROR(eglGetDisplay(A))
#define peglBindAPI(A)                          GET_EGLERROR(eglBindAPI(A))
#define peglCreateContext(A,B,C,D)              GET_EGLERROR(eglCreateContext(A,B,C,D))
#define peglCreateWindowSurface(A,B,C,D)        GET_EGLERROR(eglCreateWindowSurface(A,B,C,D))
#define peglInitialize(A,B,C)                   GET_EGLERROR(eglInitialize(A,B,C))
#define peglMakeCurrent(A,B,C,D)                GET_EGLERROR(eglMakeCurrent(A,B,C,D))
#define peglChooseConfig(A,B,C,D,E)             GET_EGLERROR(eglChooseConfig(A,B,C,D,E))

#ifdef __cplusplus
}
#endif

#endif // EGLPORT_H
