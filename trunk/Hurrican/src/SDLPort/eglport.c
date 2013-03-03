/**
 *
 *  Copyright (C) 2011-2012 Scott R. Smith
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

#include "eglport.h"

#include <stdio.h>
#include <stdlib.h>

#if defined(USE_EGL_SDL)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "SDL_syswm.h"
#include "SDL.h"
#endif /* USE_EGL_SDL */

#if defined(PANDORA) /* Pandora VSync Support */
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

#ifndef FBIO_WAITFORVSYNC
#define FBIO_WAITFORVSYNC _IOW('F', 0x20, __u32)
#endif
int fbdev = -1;

#elif defined(RPI)
#include "bcm_host.h"
#endif /* PANDORA */

EGLint useVsync    = 0; /** Controls on the system vsync if available. */
EGLint useFSAA     = 0; /** Number of samples for full screen AA. 0 is off, 2/4 samples */

NativeDisplayType   g_nativeDisplay = 0;    /** Reference to the systems native display */
NativeWindowType    g_nativeWindow  = 0;    /** Reference to the systems native window */
EGLDisplay          g_eglDisplay    = NULL; /** Reference to the EGL display */
EGLConfig           g_eglConfig     = NULL; /** Reference to the EGL config */
EGLContext          g_eglContext    = NULL; /** Reference to the EGL context */
EGLSurface          g_eglSurface    = NULL; /** Reference to the EGL surface */

#define     g_totalConfigsIn 20             /** Total number of configurations to request */
EGLint      g_totalConfigsFound = 0;        /** Total number of configurations matching attributes */
EGLConfig   g_allConfigs[g_totalConfigsIn]; /** Structure containing references to matching configurations */

/** Private API */
int8_t  ConfigureEGL                ( EGLConfig config );
int8_t  FindAppropriateEGLConfigs   ( void );
int8_t  CheckEGLErrors              ( const char* file, uint16_t line );

int8_t  GetNativeDisplay            ( void );
int8_t  GetNativeWindow             ( void );
void    FreeNativeDisplay           ( void );
void    FreeNativeWindow            ( void );

void    Platform_Open               ( void );
void    Platform_Close              ( void );
void    Platform_VSync              ( void );

/** @brief Release all EGL and system resources
 */
void EGL_Close( void )
{
    if (g_eglDisplay != NULL)
    {
        peglMakeCurrent( g_eglDisplay, NULL, NULL, EGL_NO_CONTEXT );
        if (g_eglContext != NULL) {
            peglDestroyContext( g_eglDisplay, g_eglContext );
        }
        if (g_eglSurface != NULL) {
            peglDestroySurface( g_eglDisplay, g_eglSurface );
        }
        peglTerminate( g_eglDisplay );
    }

    g_eglSurface = NULL;
    g_eglContext = NULL;
    g_eglDisplay = NULL;

    FreeNativeWindow();
    FreeNativeDisplay();
    Platform_Close();

    CheckEGLErrors( __FILE__, __LINE__ );

    printf( "EGL Closed\n" );
}

/** @brief Configure EGL for use on the system
 * @return : 0 if the function passed, else 1
 */
int8_t EGL_Init( void )
{
    int configIndex = 0;

    if (FindAppropriateEGLConfigs() != 0)
    {
        printf( "EGL ERROR: Unable to configure EGL. See previous error.\n" );
        return 1;
    }

    printf( "EGL Using Config %d\n", configIndex );
    if ( ConfigureEGL(g_allConfigs[configIndex]) != 0)
    {
        CheckEGLErrors( __FILE__, __LINE__ );
        printf( "EGL ERROR: Unable to configure EGL. See previous error.\n" );
        return 1;
    }

    return 0;
}

/** @brief Swap the surface buffer onto the display
 */
void EGL_SwapBuffers( void )
{
    if (useVsync != 0) {
        Platform_VSync();
    }
    peglSwapBuffers( g_eglDisplay, g_eglSurface );
}


/** @brief Obtain the system display and initialize EGL
 * @return : 0 if the function passed, else 1
 */
int8_t EGL_Open( void )
{
    EGLint eglMajorVer, eglMinorVer;
    EGLBoolean result;
    const char* output;

    // Setup any platform specific bits
    Platform_Open();

    printf( "EGL Opening EGL display\n" );
    if (GetNativeDisplay() != 0)
    {
        printf( "EGL ERROR: Unable to obtain native display!\n" );
        return 1;
    }

    g_eglDisplay = peglGetDisplay( g_nativeDisplay );
    if (g_eglDisplay == EGL_NO_DISPLAY)
    {
        CheckEGLErrors( __FILE__, __LINE__ );
        printf( "EGL ERROR: Unable to create EGL display.\n" );
        return 1;
    }

    printf( "EGL Initializing\n" );
    result = peglInitialize( g_eglDisplay, &eglMajorVer, &eglMinorVer );
    if (result != EGL_TRUE )
    {
        CheckEGLErrors( __FILE__, __LINE__ );
        printf( "EGL ERROR: Unable to initialize EGL display.\n" );
        return 1;
    }

    // Get EGL Library Information
    printf( "EGL Implementation Version: Major %d Minor %d\n", eglMajorVer, eglMinorVer );
    output = peglQueryString( g_eglDisplay, EGL_VENDOR );
    printf( "EGL_VENDOR: %s\n", output );
    output = peglQueryString( g_eglDisplay, EGL_VERSION );
    printf( "EGL_VERSION: %s\n", output );
    output = peglQueryString( g_eglDisplay, EGL_EXTENSIONS );
    printf( "EGL_EXTENSIONS: %s\n", output );

    CheckEGLErrors( __FILE__, __LINE__ );

    return 0;
}

/** @brief Use the EGL configuration to creat the context and surface and make them current
 * @return : 0 if the function passed, else 1
 */
int8_t ConfigureEGL( EGLConfig config )
{
    EGLBoolean result;

    static const EGLint s_contextAttribs[] =
    {
#if defined(USE_GLES2)
          EGL_CONTEXT_CLIENT_VERSION,     2,
#endif
          EGL_NONE
    };

    // Cleanup in case of a reinit
    if (g_eglDisplay != NULL)
    {
        peglMakeCurrent( g_eglDisplay, NULL, NULL, EGL_NO_CONTEXT );
        if (g_eglContext != NULL) {
            peglDestroyContext( g_eglDisplay, g_eglContext );
        }
        if (g_eglSurface != NULL) {
            peglDestroySurface( g_eglDisplay, g_eglSurface );
        }
    }

#if defined(EGL_VERSION_1_2)
    // Bind GLES and create the context
    printf( "EGL Binding API\n" );
    result = peglBindAPI( EGL_OPENGL_ES_API );
    if ( result == EGL_FALSE )
    {
        CheckEGLErrors( __FILE__, __LINE__ );
        printf( "EGL ERROR: Could not bind EGL API.\n" );
        return 1;
    }
#endif /* EGL_VERSION_1_2 */

    printf( "EGL Creating Context\n" );
    g_eglContext = peglCreateContext( g_eglDisplay, config, NULL, s_contextAttribs );
    if (g_eglContext == EGL_NO_CONTEXT)
    {
        CheckEGLErrors( __FILE__, __LINE__ );
        printf( "EGL ERROR: Unable to create GLES context!\n");
        return 1;
    }

    printf( "EGL Creating window surface\n" );
    if (GetNativeWindow() != 0)
    {
        printf( "EGL ERROR: Unable to obtain native window!\n" );
        return 1;
    }

    g_eglSurface = peglCreateWindowSurface( g_eglDisplay, config, g_nativeWindow, 0 );
    if (g_eglSurface == EGL_NO_SURFACE)
    {
        CheckEGLErrors( __FILE__, __LINE__ );
        printf( "EGL ERROR: Unable to create EGL surface!\n" );
        return 1;
    }

    printf( "EGL Making Current\n" );
    result = peglMakeCurrent( g_eglDisplay,  g_eglSurface,  g_eglSurface, g_eglContext );
    if (result != EGL_TRUE)
    {
        CheckEGLErrors( __FILE__, __LINE__ );
        printf( "EGL ERROR: Unable to make GLES context current\n" );
        return 1;
    }

    CheckEGLErrors( __FILE__, __LINE__ );
    printf( "EGL Complete\n" );
    return 0;
}

/** @brief Find a EGL configuration tht matches the defined attributes
 * @return : 0 if the function passed, else 1
 */
int8_t FindAppropriateEGLConfigs( void )
{
    EGLBoolean result;
    int attrib = 0;
    EGLint ConfigAttribs[19];

    ConfigAttribs[attrib++] = EGL_RED_SIZE;
    ConfigAttribs[attrib++] = 5;
    ConfigAttribs[attrib++] = EGL_GREEN_SIZE;
    ConfigAttribs[attrib++] = 6;
    ConfigAttribs[attrib++] = EGL_BLUE_SIZE;
    ConfigAttribs[attrib++] = 5;
    ConfigAttribs[attrib++] = EGL_DEPTH_SIZE;
    ConfigAttribs[attrib++] = 16;
    ConfigAttribs[attrib++] = EGL_STENCIL_SIZE;
    ConfigAttribs[attrib++] = 0;
    ConfigAttribs[attrib++] = EGL_SURFACE_TYPE;
    ConfigAttribs[attrib++] = EGL_WINDOW_BIT;
#if defined(EGL_VERSION_1_2)
    ConfigAttribs[attrib++] = EGL_RENDERABLE_TYPE;
#if defined(USE_GLES1)
    ConfigAttribs[attrib++] = EGL_OPENGL_ES_BIT;
#elif defined(USE_GLES2)
    ConfigAttribs[attrib++] = EGL_OPENGL_ES2_BIT;
#endif /* USE_GLES1 */
#endif /* EGL_VERSION_1_2 */
    ConfigAttribs[attrib++] = EGL_SAMPLE_BUFFERS;
    ConfigAttribs[attrib++] = (useFSAA > 0) ? 1 : 0;
    ConfigAttribs[attrib++] = EGL_SAMPLES;
    ConfigAttribs[attrib++] = useFSAA;
    ConfigAttribs[attrib++] = EGL_NONE;

    result = peglChooseConfig( g_eglDisplay, ConfigAttribs, g_allConfigs, g_totalConfigsIn, &g_totalConfigsFound );
    if (result != EGL_TRUE || g_totalConfigsFound == 0)
    {
        CheckEGLErrors( __FILE__, __LINE__ );
        printf( "EGL ERROR: Unable to query for available configs, found %d.\n", g_totalConfigsFound );
        return 1;
    }
    printf( "EGL Found %d available configs\n", g_totalConfigsFound );

    return 0;
}

/** @brief Error checking function
 * @param file : string reference that contains the source file that the check is occuring in
 * @param line : numeric reference that contains the line number that the check is occuring in
 * @return : 0 if the function passed, else 1
 */
int8_t CheckEGLErrors( const char* file, uint16_t line )
{
    EGLenum error;
    const char* errortext;

    error = eglGetError();

    if (error != EGL_SUCCESS && error != 0)
    {
        switch (error)
        {
            case EGL_NOT_INITIALIZED:           errortext = "EGL_NOT_INITIALIZED"; break;
            case EGL_BAD_ACCESS:                errortext = "EGL_BAD_ACCESS"; break;
            case EGL_BAD_ALLOC:                 errortext = "EGL_BAD_ALLOC"; break;
            case EGL_BAD_ATTRIBUTE:             errortext = "EGL_BAD_ATTRIBUTE"; break;
            case EGL_BAD_CONTEXT:               errortext = "EGL_BAD_CONTEXT"; break;
            case EGL_BAD_CONFIG:                errortext = "EGL_BAD_CONFIG"; break;
            case EGL_BAD_CURRENT_SURFACE:       errortext = "EGL_BAD_CURRENT_SURFACE"; break;
            case EGL_BAD_DISPLAY:               errortext = "EGL_BAD_DISPLAY"; break;
            case EGL_BAD_SURFACE:               errortext = "EGL_BAD_SURFACE"; break;
            case EGL_BAD_MATCH:                 errortext = "EGL_BAD_MATCH"; break;
            case EGL_BAD_PARAMETER:             errortext = "EGL_BAD_PARAMETER"; break;
            case EGL_BAD_NATIVE_PIXMAP:         errortext = "EGL_BAD_NATIVE_PIXMAP"; break;
            case EGL_BAD_NATIVE_WINDOW:         errortext = "EGL_BAD_NATIVE_WINDOW"; break;
            case EGL_CONTEXT_LOST:                errortext = "EGL_CONTEXT_LOST"; break;
            default:                            errortext = "Unknown EGL Error"; break;
        }

        printf( "ERROR: EGL Error detected in file %s at line %d: %s (0x%X)\n", file, line, errortext, error );
        return 1;
    }

    return 0;
}

/** @brief Obtain a reference to the system's native display
 * @param window : pointer to save the display reference
 * @return : 0 if the function passed, else 1
 */
int8_t GetNativeDisplay( void )
{
#if defined(USE_EGL_SDL)
    printf( "EGL Opening X11 display\n" );

    g_nativeDisplay = XOpenDisplay( NULL );
    if (g_nativeDisplay == NULL)
    {
        printf( "EGL ERROR: unable to get display!\n" );
        return 1;
    }
#else
    g_nativeDisplay = EGL_DEFAULT_DISPLAY;
#endif /* USE_EGL_SDL */

    return 0;
}

/** @brief Obtain a reference to the system's native window
 * @param window : pointer to save the window reference
 * @return : 0 if the function passed, else 1
 */
int8_t GetNativeWindow( void )
{
    g_nativeWindow = 0;

#if defined(WIZ) || defined(CAANOO)
    g_nativeWindow = (NativeWindowType)malloc(16*1024);

    if(g_nativeWindow == NULL) {
        printf( "EGL ERROR: Memory for window Failed\n" );
        return 1;
    }
#elif defined(RPI)
    int32_t result;
    uint32_t screen_width, screen_height;
    static EGL_DISPMANX_WINDOW_T nativewindow;
    DISPMANX_ELEMENT_HANDLE_T dispman_element;
    DISPMANX_DISPLAY_HANDLE_T dispman_display;
    DISPMANX_UPDATE_HANDLE_T dispman_update;
    VC_RECT_T dst_rect;
    VC_RECT_T src_rect;

    // create an EGL window surface
    result = graphics_get_display_size(0 /* LCD */, &screen_width, &screen_height);
    if(result < 0) {
        printf( "EGL ERROR: RPi graphics_get_display_size failed\n" );
        return 1;
    }

    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.width = screen_width;
    dst_rect.height = screen_height;

    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.width = 640 << 16;
    src_rect.height = 480 << 16;

    dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
    dispman_update  = vc_dispmanx_update_start( 0 );
    dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
      0 /*layer*/, &dst_rect, 0 /*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE,  (VC_DISPMANX_ALPHA_T*)0 /*alpha*/,  (DISPMANX_CLAMP_T*)0 /*clamp*/,  (DISPMANX_TRANSFORM_T)0 /*transform*/);

    nativewindow.element = dispman_element;
    nativewindow.width = screen_width;
    nativewindow.height = screen_height;
    vc_dispmanx_update_submit_sync( dispman_update );

    g_nativeWindow = (NativeWindowType)&nativewindow;
#elif defined(USE_EGL_SDL)
    // Get the SDL window handle
    SDL_SysWMinfo sysInfo; //Will hold our Window information
    SDL_VERSION(&sysInfo.version); //Set SDL version

    if(SDL_GetWMInfo(&sysInfo) <= 0)
    {
        printf( "EGL ERROR: Unable to get SDL window handle: %s\n", SDL_GetError() );
        return 1;
    }
    g_nativeWindow = sysInfo.info.x11.window;
#endif /* WIZ / CAANOO */

    return 0;
}

/** @brief Release the system's native display
 */
void FreeNativeDisplay( void )
{
#if defined(USE_EGL_SDL)
    if (g_nativeDisplay != NULL) {
        XCloseDisplay( g_nativeDisplay );
    }
    g_nativeDisplay = NULL;
#endif /* USE_EGL_SDL */
}

/** @brief Release the system's native window
 */
void FreeNativeWindow( void )
{
#if defined(WIZ) || defined(CAANOO)
    if (g_nativeWindow != NULL) {
        free( g_nativeWindow );
    }
    g_nativeWindow = NULL;
#endif /* WIZ / CAANOO */
}

/** @brief Open any system specific resources
 */
void Platform_Open( void )
{
#if defined(PANDORA)
    /* Pandora VSync */
    fbdev = open ("/dev/fb0", O_RDONLY /* O_RDWR */ );
    if ( fbdev < 0 ) {
      printf( "EGL Couldn't open /dev/fb0 for vsync\n" );
    }
#elif defined(RPI)
    bcm_host_init();
#endif /* PANDORA */
}

/** @brief Release any system specific resources
 */
void Platform_Close( void )
{
#if defined(PANDORA)
    /* Pandora VSync */
    close(fbdev);
    fbdev = -1;
#endif /* PANDORA */
}

/** @brief Check the systems vsync state
 */
void Platform_VSync( void )
{
#if defined(PANDORA)
    /* Pandora VSync */
    if ( fbdev >= 0 ) {
        int arg = 0;
        ioctl( fbdev, FBIO_WAITFORVSYNC, &arg );
    }
#endif /* PANDORA */
}
