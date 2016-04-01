// Datei : DX8Graphics.cpp

// --------------------------------------------------------------------------------------
//
// Direct Graphics Klasse
// zum initialisieren von DirectX8
// beinhaltet zudem verschiedene Grafik-Funktionen zum Speichern von Screenshots usw
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include <time.h>
#include <stdexcept>
#include <string>
#include "Globals.h"
#include "Gameplay.h"
#include "Logdatei.h"
#include "Main.h"
#include "DX8Font.h"
#include "DX8Texture.h"
#include "DX8Graphics.h"
#if defined(USE_EGL_SDL) || defined(USE_EGL_RAW) || defined(USE_EGL_RPI)
#include "eglport.h"
#endif

const double BackBufferX = 1024.0f;
const double BackBufferY = 1024.0f;

// --------------------------------------------------------------------------------------
// sonstige Variablen
// --------------------------------------------------------------------------------------

D3DXMATRIX           matProj;                // Projektionsmatrix
D3DXMATRIX           matWorld;               // Weltmatrix
D3DXMATRIX           matProjWindow;
D3DXMATRIX           matProjRender;
float						DegreetoRad[360];			// Tabelle mit Rotationswerten

// --------------------------------------------------------------------------------------
// Klassenfunktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// DirectGraphics Klasse
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

DirectGraphicsClass::DirectGraphicsClass(void)
{
#if defined(PLATFORM_DIRECTX)
    lpD3D = NULL;
#elif defined(PLATFORM_SDL)
    SupportedETC1 = false;
    SupportedPVRTC = false;
#endif
    use_texture = false;

#if defined(USE_GL2)
    ProgramCurrent = PROGRAM_NONE;
#endif
}

// --------------------------------------------------------------------------------------
// Desktruktor
// --------------------------------------------------------------------------------------

DirectGraphicsClass::~DirectGraphicsClass(void)
{
}

// --------------------------------------------------------------------------------------
// D3D Initialisieren
// --------------------------------------------------------------------------------------

#if defined(PLATFORM_DIRECTX)
bool DirectGraphicsClass::Init(HWND hwnd, DWORD dwBreite, DWORD dwHoehe,
                               DWORD dwZ_Bits, bool VSync)
{
    Protokoll.WriteText( false, "\n--> Direct3D init <--\n" );
    Protokoll.WriteText( false, "---------------------\n\n" );

    if(VSync == true)
        Protokoll.WriteText( false, "using VSync...\n" );
    else
        Protokoll.WriteText( false, "NOT using VSync...\n" );

    //D3DDISPLAYMODE			DisplayMode;
    //HRESULT					Res;

    lpD3DDevice = NULL;

    ZeroMemory(&d3dpp,sizeof(d3dpp));

    lpD3D = Direct3DCreate8(D3D_SDK_VERSION);

    //Res = lpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &DisplayMode);

    d3dpp.Windowed							= CommandLineParams.RunWindowMode;	// Fenster Modus ?
    d3dpp.BackBufferCount					= 1;							// 1 Backbuffer setzen
    //DKS - Since I removed all use of the Z-coordinate, this should be changed too. Note: DirectX is entirely untested.
    //d3dpp.EnableAutoDepthStencil			= true;
    d3dpp.EnableAutoDepthStencil			= false;
    d3dpp.AutoDepthStencilFormat			= D3DFMT_D16;
    d3dpp.hDeviceWindow						= hwnd;							// Fenster Handle
    d3dpp.BackBufferWidth					= dwBreite;					    // ScreenBreite
    d3dpp.BackBufferHeight					= dwHoehe;					    // Screenhöhe
    d3dpp.BackBufferFormat					= D3DFMT_X8R8G8B8;

    d3dpp.SwapEffect	= D3DSWAPEFFECT_COPY_VSYNC;		// VSync an
    //if(VSync == true)	d3dpp.SwapEffect	= D3DSWAPEFFECT_COPY_VSYNC;		// VSync an
//				else
    d3dpp.SwapEffect	= D3DSWAPEFFECT_DISCARD;		// VSync aus

    if(CommandLineParams.RunWindowMode == false)
    {
        d3dpp.FullScreen_RefreshRateInHz		= D3DPRESENT_RATE_DEFAULT;		// Refresh Rate
        if(VSync == true)														// VSYNC anlassen
            d3dpp.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_ONE;
        else																	// VSYNC auslassen
            d3dpp.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
    }

    d3dpp.Flags								= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;	// Backbuffer

    // ok, now try some video modes

    // 32 Bit / Hardware  X8R8G8B8
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    if (SUCCEEDED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
                                      D3DDEVTYPE_HAL,
                                      g_hwnd,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp,
                                      &lpD3DDevice)))
        goto _ModeFound;

    // 16 Bit / Hardware R5B6G5
    d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
    if (SUCCEEDED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
                                      D3DDEVTYPE_HAL,
                                      g_hwnd,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp,
                                      &lpD3DDevice)))
        goto _ModeFound;

    // 32 Bit / Software X8R8G8B8
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    if (SUCCEEDED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
                                      D3DDEVTYPE_HAL,
                                      g_hwnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp,
                                      &lpD3DDevice)))
        goto _ModeFound;

    // 16 Bit / Software R5G6B5
    d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
    if (SUCCEEDED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
                                      D3DDEVTYPE_HAL,
                                      g_hwnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp,
                                      &lpD3DDevice)))
        goto _ModeFound;

    // nothing so far? OK... go for 32 bit reference rasterizer = ultraslow!
    // note: this does not work on my machine, so i have no idea if that helps
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    if (SUCCEEDED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
                                      D3DDEVTYPE_REF,
                                      g_hwnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp,
                                      &lpD3DDevice)))
        goto _ModeFound;

    // no mode found!
    Protokoll.WriteText( true, "No compatible Graphics Mode found!\n" );
    return false;

_ModeFound:

    Protokoll.WriteText( false, "DX8 Device initialised!\n" );

    /* Jetzt haben wir die Informationen zusammen und machen weiter */

    // Rausfinden, wie ich die Texturen laden muss (nur rechteckig, 2er Potenz)
    // Device kann nur quadratische Texturen
    D3DCAPS8 d3dCaps;

    lpD3DDevice->GetDeviceCaps(&d3dCaps);

    Protokoll.WriteText( false, "Texture restrictions:\n" );

    if (d3dCaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
    {
        Protokoll.WriteText( false, "Square Only: TRUE\n" );
        SquareOnly = true;
    }
    else
    {
        Protokoll.WriteText( false, "Square Only: FALSE\n" );
        SquareOnly = false;
    }

    // Device kann nur Texturen mit 2er-Potenz-Grösse
    if (d3dCaps.TextureCaps & D3DPTEXTURECAPS_POW2)
    {
        Protokoll.WriteText( false, "Power of Two: TRUE\n" );
        PowerOfTwo = true;
    }
    else
    {
        Protokoll.WriteText( false, "Power of Two: FALSE\n" );
        PowerOfTwo = false;
    }

    SetDeviceInfo();

    Protokoll.WriteText( false, "\n-> Direct3D init successful!\n\n" );

    // DegreetoRad-Tabelle füllen
    for(int i=0; i<360; i++)
        DegreetoRad[i] = float(PI * i / 180);

    SetColorKeyMode();

    return true;
}
#elif defined(PLATFORM_SDL)
bool DirectGraphicsClass::Init(HWND hwnd, DWORD dwBreite, DWORD dwHoehe,
                               DWORD dwZ_Bits, bool VSync)
{
    bool isFullscreen       = !CommandLineParams.RunWindowMode;
    uint16_t ScreenWidth    = SCREENWIDTH;
    uint16_t ScreenHeight   = SCREENHEIGHT;

    if (CommandLineParams.LowRes) {
        ScreenWidth    = LOWRES_SCREENWIDTH;
        ScreenHeight   = LOWRES_SCREENHEIGHT;
    }

    uint16_t ScreenDepth    = CommandLineParams.ScreenDepth;
#if SDL_VERSION_ATLEAST(2,0,0)
    uint32_t flags          = SDL_WINDOW_OPENGL;
#else /* SDL 1.2 */
#if defined(USE_EGL_SDL) || defined(USE_EGL_RAW) || defined(USE_EGL_RPI)
    uint32_t flags          = SDL_SWSURFACE;
#else
    uint32_t flags          = SDL_SWSURFACE|SDL_OPENGL;
#endif
#endif

#if defined(RPI)
    ScreenWidth    = 0;
    ScreenHeight   = 0;
#endif

    Protokoll.WriteText( false, "\n--> SDL/OpenGL init <--\n" );
    Protokoll.WriteText( false, "---------------------\n\n" );

    // Initialize defaults, Video and Audio subsystems
    Protokoll.WriteText( false, "Initializing SDL.\n" );
    if (SDL_Init( SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER|SDL_INIT_JOYSTICK )==-1)
    {
        Protokoll.WriteText( false, "Failed to initialize SDL: %s.\n", SDL_GetError() );
        return false;
    }
    Protokoll.WriteText( false, "SDL initialized.\n" );

#if !defined(USE_EGL_SDL) && !defined(USE_EGL_RAW) && !defined(USE_EGL_RPI)
    if (CommandLineParams.ScreenDepth > 16) {               //DKS - Screen depth is now default 16 under GLES, 32 others
        SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );          //      (Can now be changed via command line switch)
        SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );

        if (CommandLineParams.ScreenDepth >= 32) {
            SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
            SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 32 );
        } else {
            SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 0 );
            SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 24 );
        }
    } else {
        SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
        SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 6 );
        SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
        SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 16 );
    }
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 0 );         // DKS - No need for a depth buffer in this game
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, VSync);    // DKS - VSync on still the default, but controlled w/ cmd switch
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 0 ); // DKS - Changed this to 0 (Game would not load w/ GL1.2 laptop)
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 0 );

#if SDL_VERSION_ATLEAST(2,0,0)
#if defined(USE_GLES1)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#elif defined(USE_GLES2)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif /* defined(USE_GLES1) */
#endif /* SDL_VERSION_ATLEAST(2,0,0) */
#endif /* PLATFORM_SDL */

    // Setup SDL Screen
    if (isFullscreen == true)
    {
#if SDL_VERSION_ATLEAST(2,0,0)
        flags |= SDL_WINDOW_FULLSCREEN;
#else /* SDL 1.2 */
        flags |= SDL_FULLSCREEN;
#endif
    }

#if SDL_VERSION_ATLEAST(2,0,0)
    // Create a window. Window mode MUST include SDL_WINDOW_OPENGL for use with OpenGL.
    Window = SDL_CreateWindow("Hurrican", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              ScreenWidth, ScreenHeight, flags);

    // Create an OpenGL context associated with the window.
    GLcontext = SDL_GL_CreateContext(Window);
#else /* SDL 1.2 */
    //SDL_WM_SetCaption("Hurrican", "Hurrican");

    Screen = SDL_SetVideoMode( ScreenWidth, ScreenHeight, ScreenDepth, flags );
    if (Screen == NULL)
    {
        Protokoll.WriteText( false, "Failed to %dx%dx%d video mode: %s\n", ScreenWidth, ScreenHeight, ScreenDepth, SDL_GetError() );
        return false;
    }
#endif

    SDL_ShowCursor(SDL_DISABLE);

#if defined(USE_EGL_SDL) || defined(USE_EGL_RAW) || defined(USE_EGL_RPI)
    uint16_t actual_w, actual_h;
#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_GetWindowSize( Window, &actual_w, &actual_h );
#else
    actual_w = Screen->w;
    actual_h = Screen->h;
#endif

    //DKS - EGL_Open now takes additional parameters to specify desired screen
    //      depth and vsync. If eglport.cfg file is present, it will override
    //      whatever is passed here, however:
    if (EGL_Open( actual_w, actual_h, CommandLineParams.ScreenDepth == 16 ? 16 : 32, CommandLineParams.VSync ) != 0 )
    {
        return 1;
    }
#endif

    SetDeviceInfo();

    Protokoll.WriteText( false, "\n-> OpenGL init successful!\n\n" );

    // DegreetoRad-Tabelle füllen
    for(int i=0; i<360; i++)
        DegreetoRad[i] = float(PI * i / 180);

    SetColorKeyMode();

    return true;
}
#endif

// --------------------------------------------------------------------------------------
// Direct3D beenden
// --------------------------------------------------------------------------------------

bool DirectGraphicsClass::Exit(void)
{
#if defined(PLATFORM_DIRECTX)
    SafeRelease (lpD3DDevice);
    SafeRelease (lpD3D);
    Protokoll.WriteText( false, "-> Direct3D shutdown successfully completed !\n" );
#elif defined(PLATFORM_SDL)
#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_GL_DeleteContext(GLcontext);
    SDL_DestroyWindow(Window);
#endif
#if defined(USE_GL2)
    Shaders[PROGRAM_COLOR].Close();
    Shaders[PROGRAM_TEXTURE].Close();
#if defined(USE_FBO)
    RenderBuffer.Close();
#endif /* USE_FBO */
#endif /* USE_GL2 */
#if defined(USE_EGL_SDL) || defined(USE_EGL_RAW) || defined(USE_EGL_RPI)
    EGL_Close();
#endif
    SDL_Quit();
    Protokoll.WriteText( false, "-> SDL/OpenGL shutdown successfully completed !\n" );
#endif
    return true;
}

// --------------------------------------------------------------------------------------
// Infos für Device Objekt setzen
// Für Init und nach Task Wechsel
// --------------------------------------------------------------------------------------

bool DirectGraphicsClass::SetDeviceInfo(void)
{
#if defined(PLATFORM_DIRECTX)
    HRESULT hr;

    // Globale Variable mit dem tatsächlichen BackBuffer füllen
    lpD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &lpBackbuffer);

    // Licht, Cullmodus und Z-Buffer aktivieren
    lpD3DDevice->SetRenderState(D3DRS_AMBIENT,D3DCOLOR_XRGB(255,255,255));
    lpD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
    lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    lpD3DDevice->SetRenderState(D3DRS_ZENABLE,  D3DZB_FALSE);

    // Projektionsmatrix einstellen
    //
    D3DXMatrixOrthoOffCenterLH(&matProj, 0, 640, 480, 0, 0.0f, 1.0f);

    hr = lpD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

    if(hr != D3D_OK)
    {
        Protokoll.WriteText( true, "\n-> SetTransform error!\n" );
        return false;
    }

    // Flexible Vertex Format setzen
    hr = lpD3DDevice->SetVertexShader (D3DFVF_TLVERTEX);
    if(hr != D3D_OK)
    {
        Protokoll.WriteText( true, "\n-> SetVertexShader error!\n" );
        return false;
    }

    // Alphablending aktivieren
    lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
#elif defined(PLATFORM_SDL)
#if defined(USE_GL2)
    char vert[256];
    char frag[256];
#endif
    char* output;

#if defined(__WIN32__)
    if (LoadGLFunctions() != 0)
    {
        return false;
    }
#endif /* __WIN32__ */

    SetupFramebuffers();

    /* OpenGL Information */
    output = (char*)glGetString( GL_VENDOR );
    Protokoll.WriteText( false, "GL_VENDOR: %s\n", output );
    output = (char*)glGetString( GL_RENDERER );
    Protokoll.WriteText( false, "GL_RENDERER: %s\n", output );
    output = (char*)glGetString( GL_VERSION );
    Protokoll.WriteText( false, "GL_VERSION: %s\n", output );
#if defined(USE_GL2)
    output = (char*)glGetString( GL_SHADING_LANGUAGE_VERSION );
    Protokoll.WriteText( false, "GL_SHADING_LANGUAGE_VERSION: %s\n", output );
#endif
    glextentsions = (char*)glGetString( GL_EXTENSIONS );
    Protokoll.WriteText( false, "GL_EXTENSIONS: %s\n", glextentsions );

#if defined(USE_ETC1)
    SupportedETC1 = ExtensionSupported( "GL_OES_compressed_ETC1_RGB8_texture" );
#endif
#if defined(USE_PVRTC)
    SupportedPVRTC = ExtensionSupported( "GL_IMG_texture_compression_pvrtc" );
#endif

    /* Init OpenGL */
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );                 /* Set the background black */
    glClearDepth( 1.0f );                                   /* Depth buffer setup */

    glDisable( GL_DEPTH_TEST );                             /* No Depth Testing */
    glEnable( GL_BLEND );

#if defined(USE_GL1)
    glDisable( GL_LIGHTING );                               /* No light */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );    /* Really Nice Perspective Calculations */
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#endif

#if defined(USE_GL2)
    // Compile the shader code and link into a program
    sprintf_s(vert, "%s/data/shaders/shader_color.vert", g_storage_ext);
    sprintf_s(frag, "%s/data/shaders/shader_color.frag", g_storage_ext);

    if (Shaders[PROGRAM_COLOR].Load( vert, frag ) != 0)
    {
        return false;
    }

    sprintf_s(vert, "%s/data/shaders/shader_texture.vert", g_storage_ext);
#if defined(USE_ETC1)
    if (SupportedETC1 == true) {
        sprintf_s(frag, "%s/data/shaders/shader_etc1_texture.frag", g_storage_ext);
    } else {
#endif
        sprintf_s(frag, "%s/data/shaders/shader_texture.frag", g_storage_ext);
#if defined(USE_ETC1)
    }
#endif


    if (Shaders[PROGRAM_TEXTURE].Load( vert, frag ) != 0)
    {
        return false;
    }

    // Get names for attributes and uniforms
    Shaders[PROGRAM_COLOR].NamePos = Shaders[PROGRAM_COLOR].GetAttribute( "a_Position" );
    Shaders[PROGRAM_COLOR].NameClr = Shaders[PROGRAM_COLOR].GetAttribute( "a_Color" );
    Shaders[PROGRAM_COLOR].NameMvp = Shaders[PROGRAM_COLOR].GetUniform( "u_MVPMatrix" );

    Shaders[PROGRAM_TEXTURE].NamePos = Shaders[PROGRAM_TEXTURE].GetAttribute( "a_Position" );
    Shaders[PROGRAM_TEXTURE].NameClr = Shaders[PROGRAM_TEXTURE].GetAttribute( "a_Color" );
    Shaders[PROGRAM_TEXTURE].NameTex = Shaders[PROGRAM_TEXTURE].GetAttribute( "a_Texcoord0" );
    Shaders[PROGRAM_TEXTURE].NameMvp = Shaders[PROGRAM_TEXTURE].GetUniform( "u_MVPMatrix" );

#if defined(USE_ETC1)
    if (SupportedETC1 == true)
    {
        Shaders[PROGRAM_TEXTURE].texUnit0 = Shaders[PROGRAM_TEXTURE].GetUniform( "u_Texture0" );
        Shaders[PROGRAM_TEXTURE].texUnit1 = Shaders[PROGRAM_TEXTURE].GetUniform( "u_Texture1" );
    }
#endif
#endif /* USE_GL2 */

    /* Matrices setup */
    g_matView.identity();
    g_matModelView.identity();

    cml::matrix_orthographic_RH( matProjWindow, 0.0f, (float)WindowView.w, (float)WindowView.h, 0.0f, 0.0f, 1.0f, cml::z_clip_neg_one );
    cml::matrix_orthographic_RH( matProjRender, 0.0f, (float)RenderView.w, (float)RenderView.h, 0.0f, 0.0f, 1.0f, cml::z_clip_neg_one );
    matProj = matProjWindow;

#if defined(USE_GL1)
    /* change to the projection matrix and set our viewing volume. */
    load_matrix( GL_PROJECTION, matProj.data() );
    load_matrix( GL_MODELVIEW, g_matModelView.data() );
#endif /* USE_GL1 */

#if defined(USE_FBO)
    SelectBuffer( true );
#endif

#endif /* PLATFORM_SDL */

    return true;
}

// --------------------------------------------------------------------------------------
// Screenshot machen
// bei Filename000.bmp anfangen und dann weiter bis 999, falls diese schon existieren
// --------------------------------------------------------------------------------------

bool DirectGraphicsClass::TakeScreenshot(const char Filename[100], int screenx, int screeny)
{
#if defined(PLATFORM_DIRECTX)
    FILE*				f = NULL;					// Datei
    HRESULT				hr;							// Für Fehler-Prüfung
    IDirect3DSurface8*	FrontBuffer;				// Zeiger auf Frontbuffer

    // Surface erzeugen, in die das Bild kopiert wird
    lpD3DDevice->CreateImageSurface(screenx, screeny, D3DFMT_A8R8G8B8, &FrontBuffer);

    // Front Buffer hineinkopieren
    hr = lpD3DDevice->GetFrontBuffer(FrontBuffer);

    // Fehler ?
    if(hr != D3D_OK)
    {
        Protokoll.WriteText( false, "\n-> TakeScreenshot error!\n" );
        FrontBuffer->Release();
        return false;
    }

    // BMP Datei erzeugen, wobei bereits existierende Dateien nicht überschrieben werden
    // so entstehen dann Screenshot000 - Screenshot999

    char	TempName[100];
    char	TempDate[128];
    char	Buffer[10];
    int		i=0;

    _tzset();
    _strdate_s(TempDate);

    // Zeichen im Datum ersetzen
    int len = strlen(TempDate);
    for (int i=0; i<len; i++)
        if (TempDate[i] == '/')
            TempDate[i] = '-';

    i = 0;

    do
    {
        _itoa_s(i, Buffer, 10);

        // Datei Namen zusammensetzen
        strcpy_s(TempName, strlen(Filename) + 1, Filename);					// Name
        strcat_s(TempName, 2, "_");
        strcat_s(TempName, strlen(TempDate) + 1, TempDate);
        strcat_s(TempName, 2, "_");
        if(i<10)  strcat_s(TempName, 2, "0"); 			// evtl noch 0
        if(i<100) strcat_s(TempName, 2, "0");			// oder gar  00 davor schreiben
        strcat_s(TempName, strlen(Buffer) + 1, Buffer);					// Aktuelle Zahl
        strcat_s(TempName, 5, ".BMP");					// und BMP Endung
        fopen_s(&f, TempName, "rb");
        i++;
    }
    while (f != NULL);

    // Screenshot Datei erzeugen
    D3DXSaveSurfaceToFile(TempName, D3DXIFF_BMP, FrontBuffer, NULL, NULL);

    // Surface wieder freigeben
    FrontBuffer->Release();
#elif defined(PLATFORM_SDL)
#endif
    return true;
}

// --------------------------------------------------------------------------------------
// Renderstates für Sprites mit ColorKey setzen
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::SetColorKeyMode(void)
{
    if (BlendMode == COLORKEY_MODE)
        return;

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);	// Colorkey
    lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// einschalten
#elif defined(PLATFORM_SDL)
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
#endif

    BlendMode = COLORKEY_MODE;
}

// --------------------------------------------------------------------------------------
// Renderstates für Sprites setzen, die komplett weiss gerendert werden
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::SetWhiteMode(void)
{
    if (BlendMode == WHITE_MODE)
        return;

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_INVSRCALPHA  );	// White
    lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA );	// einschalten
#elif defined(PLATFORM_SDL)
    glBlendFunc( GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA );
#endif

    BlendMode = WHITE_MODE;
}

// --------------------------------------------------------------------------------------
// Renderstates für Sprites mit Additivem Alphablending setzen
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::SetAdditiveMode(void)
{
    if (BlendMode == ADDITIV_MODE)
        return;

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);	// Additive Blending
    lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);	// einschalten
#elif defined(PLATFORM_SDL)
    glBlendFunc( GL_SRC_ALPHA, GL_ONE );
#endif

    BlendMode = ADDITIV_MODE;
}

// --------------------------------------------------------------------------------------
// Renderstates für linearen Texturfilter ein/ausschalten
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::SetFilterMode (bool filteron)
{
    // Filter schon an? Dann nichts ändern
    //

    if (filteron == FilterMode)
        return;

    // Filter einschalten
    //
    if (filteron == true)
    {
#if defined(PLATFORM_DIRECTX)
        lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
#endif
    }

    // andernfalls Filter ausschalten
    //
    else
    {
#if defined(PLATFORM_DIRECTX)
        lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
        lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
#endif
    }

    FilterMode = filteron;
}

// --------------------------------------------------------------------------------------
// Rendert in den Buffer, der am Ende eines jeden Frames komplett
// in den Backbuffer gerendert wird
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::RendertoBuffer (D3DPRIMITIVETYPE PrimitiveType,
        UINT PrimitiveCount,
        CONST void* pVertexStreamZeroData)
{
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, sizeof(VERTEX2D));
#elif defined(PLATFORM_SDL)
    int stride      = sizeof(VERTEX2D);
    //DKS - Changed multiplier from 3 to 2, since I removed the unnecessary Z coordinate:
    int clr_offset  = sizeof(float)*2;
    int tex_offset  = clr_offset + sizeof(D3DCOLOR);

#if defined(USE_GL2)
    uint8_t program_next = PROGRAM_COLOR;

    // Determine the shader program to use
    if (use_texture == true)
    {
        program_next = PROGRAM_TEXTURE;
    }
    else
    {
        program_next = PROGRAM_COLOR;
    }

    // Check if the program is alreadt in use
    if (ProgramCurrent != program_next)
    {
        Shaders[program_next].Use();
    }
    ProgramCurrent = program_next;
#endif

    if (PrimitiveType == GL_LINES)
    {
        PrimitiveCount *= 2;
    }
    else if (PrimitiveType == GL_LINE_STRIP )
    {
        PrimitiveCount += 2;
    }
    else if (PrimitiveType == GL_TRIANGLES )
    {
        PrimitiveCount *= 3;
    }
    else if (PrimitiveType == GL_TRIANGLE_STRIP )
    {
        PrimitiveCount += 2;
    }
    else
    {
        Protokoll.WriteText( false, "Add type to count indinces\n" );
        return;
    }

#if defined(USE_GL1)
    // Swizzle the color from bgra to rgba
    uint32_t color;
    uint8_t* data = (uint8_t*)pVertexStreamZeroData;
    for (uint32_t count=0; count<PrimitiveCount; count++)
    {
        color = *(uint32_t*)(data+clr_offset);

        if (color != 0xFFFFFFFF)
        {
            *(uint32_t*)(data+clr_offset) =  (color & 0xFF00FF00) +
                                             ((color & 0x00FF0000) >> 16) +
                                             ((color & 0x000000FF) << 16);
            data += stride;
        }
    }

    // Enable the client states for transfer
    if (use_texture == true)
    {
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer( 2, GL_FLOAT, stride, (uint8_t*)pVertexStreamZeroData+tex_offset );
    }

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 2, GL_FLOAT, stride, pVertexStreamZeroData );

    glEnableClientState( GL_COLOR_ARRAY );
    glColorPointer( 4, GL_UNSIGNED_BYTE, stride, (uint8_t*)pVertexStreamZeroData+clr_offset );
#elif defined(USE_GL2)
    // Enable attributes and uniforms for transfer
    if (ProgramCurrent == PROGRAM_TEXTURE)
    {
#if defined(USE_ETC1)
        if (SupportedETC1 == true)
        {
            glUniform1i( Shaders[ProgramCurrent].texUnit0, 0);
            glUniform1i( Shaders[ProgramCurrent].texUnit1, 1);
        }
#endif
        glEnableVertexAttribArray( Shaders[ProgramCurrent].NameTex );
        glVertexAttribPointer( Shaders[ProgramCurrent].NameTex, 2, GL_FLOAT, GL_FALSE, stride, (uint8_t*)pVertexStreamZeroData+tex_offset );
    }

    glEnableVertexAttribArray( Shaders[ProgramCurrent].NamePos );
    glVertexAttribPointer( Shaders[ProgramCurrent].NamePos, 2, GL_FLOAT, GL_FALSE, stride, pVertexStreamZeroData );

    glEnableVertexAttribArray( Shaders[ProgramCurrent].NameClr );
    glVertexAttribPointer( Shaders[ProgramCurrent].NameClr, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (uint8_t*)pVertexStreamZeroData+clr_offset );

    D3DXMATRIXA16 matMVP = g_matModelView*matProj;
    glUniformMatrix4fv( Shaders[ProgramCurrent].NameMvp, 1, GL_FALSE, matMVP.data() );
#endif

    glDrawArrays( PrimitiveType, 0, PrimitiveCount );

#if defined(USE_GL1)
    // Disbale the client states
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );

    if (use_texture == true)
    {
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    }
#elif defined(USE_GL2)
    // Disbale attributes and uniforms
    glDisableVertexAttribArray( Shaders[ProgramCurrent].NamePos );
    glDisableVertexAttribArray( Shaders[ProgramCurrent].NameClr );

    if (ProgramCurrent == PROGRAM_TEXTURE)
    {
        glDisableVertexAttribArray( Shaders[ProgramCurrent].NameTex );
    }
#endif

#endif
}

// --------------------------------------------------------------------------------------
// Render den Buffer auf den Backbuffer
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::DisplayBuffer  (void)
{
#if defined(PLATFORM_DIRECTX)
    // Darstellung beenden
    lpD3DDevice->EndScene();
#endif
    // Backbuffer mit Frontbuffer tauschen
    ShowBackBuffer();
}

#if defined(PLATFORM_SDL)
bool DirectGraphicsClass::ExtensionSupported( const char* ext )
{
    if( strstr( glextentsions, ext ) != NULL)
    {
        Protokoll.WriteText( false, "%s is supported\n", ext );
        return true;
    }

    Protokoll.WriteText( false, "%s is not supported\n", ext );
    return false;
}
#endif

//DKS - Supports new TexturesystemClass and is now used for both GL and DirectX
#if 0
#if defined(PLATFORM_SDL)
void DirectGraphicsClass::SetTexture( int32_t index )
{
    if (index >= 0)
    {
        use_texture = true;
        glBindTexture( GL_TEXTURE_2D, textures.at(index) );
#if defined(USE_GL1)
        glEnable( GL_TEXTURE_2D );
#endif
#if defined(USE_ETC1)
        if (SupportedETC1 == true)
        {
            glActiveTexture( GL_TEXTURE1 );
            glBindTexture( GL_TEXTURE_2D, alphatexs.at(index) );
            glActiveTexture( GL_TEXTURE0 );
        }
#endif
    }
    else
    {
        use_texture = false;
        glBindTexture( GL_TEXTURE_2D, 0 );
#if defined(USE_GL1)
        glDisable( GL_TEXTURE_2D );
#endif

#if defined(USE_ETC1)
        if (SupportedETC1 == true)
        {
            glActiveTexture( GL_TEXTURE1 );
            glBindTexture( GL_TEXTURE_2D, 0 );
            glActiveTexture( GL_TEXTURE0 );
        }
#endif
    }
}
#endif
#endif //0
void DirectGraphicsClass::SetTexture( int idx )
{
    if (idx >= 0)
    {
        use_texture = true;
        TextureHandle &th = Textures[idx];

#if defined(PLATFORM_DIRECTX)
#else   // BEGIN GL CODE
        glBindTexture( GL_TEXTURE_2D, th.tex);
#if defined(USE_ETC1)
        if (SupportedETC1 == true)
        {
            // Bind the alpha-channel texture when using ETC1-compressed textures:
            glActiveTexture( GL_TEXTURE1 );
            glBindTexture( GL_TEXTURE_2D, th.alphatex );
            glActiveTexture( GL_TEXTURE0 );
        }
#endif

#if defined(USE_GL1)
        glEnable( GL_TEXTURE_2D );
#endif
#endif  // END GL CODE
    }
    else
    {
        use_texture = false;
#if defined(PLATFORM_DIRECTX)
        lpD3DDevice->SetTexture (0, NULL);							// Textur setzen
#else // BEGIN GL CODE
        //DKS - There is no need to call glBindTexture():
        //glBindTexture( GL_TEXTURE_2D, 0 );
#if defined(USE_GL1)
        glDisable( GL_TEXTURE_2D );
#endif

        //DKS - There is no need to call glBindTexture():
#if 0
#if defined(USE_ETC1)
        if (SupportedETC1 == true)
        {
            glActiveTexture( GL_TEXTURE1 );
            glBindTexture( GL_TEXTURE_2D, 0 );
            glActiveTexture( GL_TEXTURE0 );
        }
#endif
#endif //0
#endif // END GL CODE
    }
}

// --------------------------------------------------------------------------------------
// Present aufrufen
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::ShowBackBuffer(void)
{
#if defined(PLATFORM_DIRECTX)
    HRESULT hresult;

    hresult = lpD3DDevice->Present(NULL, NULL, 0, NULL);		// Frontbuffer anzeigen
#elif defined(PLATFORM_SDL)

#if defined(USE_GL2) && defined(USE_FBO)
    if (RenderBuffer.Enabled == true)
    {
        VERTEX2D vertices[4];

        //Protokoll.WriteText( false, "%dx%d at %dx%d\n", RenderRect.w, RenderRect.h, RenderRect.x, RenderRect.y );

        vertices[0].x = RenderRect.x;
        vertices[0].y = RenderRect.y+RenderRect.h; /* lower left */
        vertices[1].x = RenderRect.x;
        vertices[1].y = RenderRect.y;   /* upper left */
        vertices[2].x = RenderRect.x+RenderRect.w;
        vertices[2].y = RenderRect.y+RenderRect.h; /* lower right */
        vertices[3].x = RenderRect.x+RenderRect.w;
        vertices[3].y = RenderRect.y;   /* upper right */
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = 0xFFFFFFFF;

        vertices[0].tu = 0;
        vertices[0].tv = 0; /* lower left */
        vertices[1].tu = 0;
        vertices[1].tv = 1; /* upper left */
        vertices[2].tu = 1;
        vertices[2].tv = 0; /* lower right */
        vertices[3].tu = 1;
        vertices[3].tv = 1; /* upper right */

        SelectBuffer( false );

        use_texture = true;
        RenderBuffer.BindTexture( use_texture );

        RendertoBuffer( D3DPT_TRIANGLESTRIP, 2, &vertices[0] );

        use_texture = false;
        RenderBuffer.BindTexture( use_texture );

#if defined(ANDROID)
        DrawTouchOverlay();
#endif
    }
#endif

#if defined(USE_EGL_SDL) || defined(USE_EGL_RAW) || defined(USE_EGL_RPI)
    EGL_SwapBuffers();
#else
#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_GL_SwapWindow(Window);
#else
    SDL_GL_SwapBuffers();
#endif
#endif

#if defined(DEBUG)
    int error = glGetError();

    if (error != 0)
    {
        Protokoll.WriteText( false, "GL Error %X file %s: line %d\n", error, __FILE__, __LINE__ );
    }
#endif

#if defined(USE_GL2) && defined(USE_FBO)
    SelectBuffer( true );
#endif

#endif
}

#if defined(PLATFORM_SDL)
void DirectGraphicsClass::SetupFramebuffers( void )
{
    /* Read the current window size */
#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_GetWindowSize( Window, &WindowView.w, &WindowView.h );
#else
    WindowView.w = Screen->w;
    WindowView.h = Screen->h;
#endif
    Protokoll.WriteText( false, "Window resolution: %dx%d\n", WindowView.w, WindowView.h );

    RenderView.x = 0;
    RenderView.y = 0;
    RenderView.w = RENDERWIDTH;
    RenderView.h = RENDERHEIGHT;
#if defined(USE_GL2) && defined(USE_FBO)
    /* Create an FBO for rendering */
    RenderBuffer.Open( RenderView.w, RenderView.h );

    if (RenderBuffer.Enabled == true)
    {
        /* Set the render viewport */
        SelectBuffer( true );
        glViewport( RenderView.x, RenderView.y, RenderView.w, RenderView.h );
        SelectBuffer( false );
        Protokoll.WriteText( false, "Render viewport resolution: %dx%d at %dx%d\n", RenderView.w, RenderView.h, RenderView.x, RenderView.y );

        /* Select the best 4:3 resolution */
        if (WindowView.w<WindowView.h)
        {
            RenderRect.w = WindowView.w;
            RenderRect.h = (WindowView.w/4)*3;
        }
        else
        {
            RenderRect.w = (WindowView.h/3)*4;
            RenderRect.h = WindowView.h;
        }
        RenderRect.x = MAX(0, WindowView.w - RenderRect.w)/2;
        RenderRect.y = MAX(0, WindowView.h - RenderRect.h)/2;

        Protokoll.WriteText( false, "Render area: %dx%d at %dx%d\n", RenderRect.w, RenderRect.h, RenderRect.x, RenderRect.y );
    }
    else
#endif
    {
        /* No scaling just center the rendering in the window */
        WindowView.x = MAX( 0,(WindowView.w-RenderView.w)/2 );
        WindowView.y = MAX( 0,(WindowView.h-RenderView.h)/2 );
        WindowView.w = RenderView.w;
        WindowView.h = RenderView.h;

#if defined(PANDORA)
        WindowView.x = 80;
        WindowView.y = 0;
#endif
    }

    if (CommandLineParams.LowRes) {
        glViewport( 0, 0, LOWRES_SCREENWIDTH, LOWRES_SCREENHEIGHT);
    } else {
        glViewport( WindowView.x, WindowView.y, WindowView.w, WindowView.h );    /* Setup our viewport. */
    }
    Protokoll.WriteText( false, "Window viewport: %dx%d at %dx%d\n", WindowView.w, WindowView.h, WindowView.x, WindowView.y );
}

void DirectGraphicsClass::ClearBackBuffer( void )
{
#if defined(USE_GL2) && defined(USE_FBO)
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
#endif
    glClear( GL_COLOR_BUFFER_BIT );

#if defined(USE_GL2) && defined(USE_FBO)
    glBindFramebuffer( GL_FRAMEBUFFER, RenderBuffer.framebuffer );
    glClear( GL_COLOR_BUFFER_BIT );
#endif
}

#if defined(USE_GL2) && defined(USE_FBO)
void DirectGraphicsClass::SelectBuffer( bool active )
{
    if (RenderBuffer.Enabled == true)
    {
        if (active == true)
        {
            glBindFramebuffer( GL_FRAMEBUFFER, RenderBuffer.framebuffer );
            glViewport( RenderView.x, RenderView.y, RenderView.w, RenderView.h );
            matProj = matProjRender;
        }
        else
        {
            glBindFramebuffer( GL_FRAMEBUFFER, 0 );
            glViewport( WindowView.x, WindowView.y, WindowView.w, WindowView.h );
            D3DXMatrixIdentity( &g_matModelView );
            matProj = matProjWindow;
        }
    }
}

#if defined(ANDROID)
void DirectGraphicsClass::DrawTouchOverlay( void )
{
    int i;
    int x, y, w, h;
    VERTEX2D vertices[4];

    for (i=0; i<DirectInput.TouchBoxes.size(); i++)
    {
        SDL_Rect *box = &DirectInput.TouchBoxes.at(i);

        vertices[0].x = box->x;
        vertices[0].y = box->y+box->h; /* lower left */
        vertices[1].x = box->x;
        vertices[1].y = box->y;   /* upper left */
        vertices[2].x = box->x+box->w;
        vertices[2].y = box->y+box->h; /* lower right */
        vertices[3].x = box->x+box->w;
        vertices[3].y = box->y;   /* upper right */
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = 0x4000FF00;

#if 0
        vertices[0].tu = 0;
        vertices[0].tv = 0; /* lower left */
        vertices[1].tu = 0;
        vertices[1].tv = 1; /* upper left */
        vertices[2].tu = 1;
        vertices[2].tv = 0; /* lower right */
        vertices[3].tu = 1;
        vertices[3].tv = 1; /* upper right */
#endif
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        RendertoBuffer( D3DPT_TRIANGLESTRIP, 2, &vertices[0] );
    }

    DrawCircle( DirectInput.TouchdpadX, DirectInput.TouchdpadY, DirectInput.TouchdpadRadius );
}

void DirectGraphicsClass::DrawCircle( uint16_t x, uint16_t y, uint16_t radius )
{
#define SECTORS 40
    VERTEX2D vtx[SECTORS+2];

    float radians = 0;
    for (int i=0; i<SECTORS+1; i++)
    {
        vtx[i].color = 0x4000FF00;

        vtx[i].x = x + (radius * cosf(cml::rad(radians)));
        vtx[i].y = y + (radius * sinf(cml::rad(radians)));
        radians += (360/SECTORS);
    }
    vtx[SECTORS+1].x = vtx[0].x;
    vtx[SECTORS+1].y = vtx[0].y;
    vtx[SECTORS+1].color = 0x8000FF00;

    glLineWidth( 3 );
    RendertoBuffer( D3DPT_LINESTRIP, SECTORS, &vtx[0] );
}
#endif /* ANDROID */
#endif /* USE_GL2 && USE_FBO */

#endif /* PLATFORM_SDL */
