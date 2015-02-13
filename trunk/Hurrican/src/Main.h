// Datei : Main.h

// --------------------------------------------------------------------------------------
//
// Logdatei.cpp Header
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __Main_h__
#define __Main_h__

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#if defined(PLATFORM_DIRECTX)
#include "windows.h"
#endif
#include "Intro.h"
#include "Outtro.h"

struct sCommandLineParams
{
    bool RunWindowMode;
    bool RunOwnLevelList;
    bool RunUserLevel;
    char UserLevelName[256];
    char OwnLevelList[256];
    char Params[256];
#if defined(PLATFORM_SDL)
    uint16_t TexFactor;
    uint16_t TexSizeMin;
    bool    AllowNPotTextureSizes;
#endif
    bool    ShowFPS;
    bool    VSync;
    uint8_t ScreenDepth;
    bool    LowRes;
};

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define WIN_32_LEAN_AND_MEAN						   // MFC abschalten
#define WINDOWCLASSNAME	"HurricanWindowsClass"	// Name der Windows-Klasse
#if defined(USE_GL2) && defined(USE_FBO)
#define SCREENWIDTH		1024
#define	SCREENHEIGHT	768
#else
#define SCREENWIDTH     640
#define	SCREENHEIGHT    480
#endif

#define LOWRES_SCREENWIDTH    320
#define LOWRES_SCREENHEIGHT   240

#define RENDERWIDTH		640
#define RENDERHEIGHT	480

// DKS - Screen depth is now configurable from the command line; Defaults to 16bpp under GLES, 32 all others
#if defined(USE_GLES1) || defined(USE_GLES2)
#define DEFAULT_SCREENBPP  16
#else
#define DEFAULT_SCREENBPP  32
#endif

#define PARAM_NONE  0
#define PARAM_LEVEL 1
#define PARAM_LIST  2

// --------------------------------------------------------------------------------------
// Prototypen von Funktionen
// --------------------------------------------------------------------------------------

bool GameInit (HWND hwnd, HINSTANCE hinstance);		// Spiel vor  dem Cracktro initialisieren
bool GameInit2(void);								// Spiel nach dem Cracktro initialisieren
bool GameExit   (void);								// Spiel de-initialisieren
bool Heartbeat  (void);								// Haupt Game Loop
bool FileExists(char Filename[256]);
void ConvertPlayerTexture(DirectGraphicsSprite *pTexture);
void CreatePlayer2Texture(void);
void StartOuttro(void);
void StartIntro(void);

extern int WINDOWWIDTH;
extern int WINDOWHEIGHT;
extern sCommandLineParams CommandLineParams;			// Externer Paramter?
#if defined(PLATFORM_SDL)
extern const char* g_storage_ext;
#endif

void ShowDebugInfo(void);							// Allen möglichen Kram anzeigen

//DKS - Added FPS reporting:
void ShowFPS(void);

#endif

