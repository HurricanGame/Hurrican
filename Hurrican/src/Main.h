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
    char *DataPath;
    char *SavePath;
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
#define SCREENWIDTH		0
#define	SCREENHEIGHT	0
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
bool CreateDir(const char *dir); //DKS - Added function to create a directory if it doesn't already exist (1 on success)
bool FindDir(const char *dir);   //DKS - Added function to check if a directory exists (1 on success)
bool FileExists(const char *Filename);
void ConvertPlayerTexture(DirectGraphicsSprite *pTexture);
void CreatePlayer2Texture(void);
void StartOuttro(void);
void StartIntro(void);

extern int WINDOWWIDTH;
extern int WINDOWHEIGHT;
extern sCommandLineParams CommandLineParams;			// Externer Paramter?

extern char* g_storage_ext;   // Where data files (levels, graphics, music, etc) for the game are stored (read)
extern char* g_save_ext;      // Where configuration files, logs, and save games are written (-DKS) (write)

void ShowDebugInfo(void);							// Allen möglichen Kram anzeigen

//DKS - Added FPS reporting:
void ShowFPS(void);

// --------------------------------------------------------------------------------------
// Endianess handling
// We will Swap values only for Big_Endian, Little_Endian should be unchanged
// --------------------------------------------------------------------------------------
static inline uint32_t FixEndian(uint32_t x)
{
#if defined(PLATFORM_SDL) && (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    return SDL_Swap32(x);
#else
    return x;
#endif
}

static inline int32_t FixEndian(int32_t x)
{
#if defined(PLATFORM_SDL) && (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    uint32_t val = SDL_Swap32(*reinterpret_cast<uint32_t*>(&x));
    return *reinterpret_cast<int32_t*>(&val);
#else
    return x;
#endif
}


#endif

