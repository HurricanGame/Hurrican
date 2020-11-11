// Datei : Main.hpp

// --------------------------------------------------------------------------------------
//
// Logdatei.cpp Header
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _MAIN_HPP_
#define _MAIN_HPP_

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "Intro.hpp"
#include "Outtro.hpp"

// TODO an enum would be nicer
#define SCREEN_FULLSCREEN           0
#define SCREEN_FULLSCREEN_STRETCHED 1
#define SCREEN_WINDOW               2

struct sCommandLineParams {
    uint8_t RunWindowMode; // 0 = fullscreen, 1 = fullscreen stretched, 2 = Windowed
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
    bool AllowNPotTextureSizes;
#endif
    bool ShowFPS;
    bool VSync;
    uint8_t ScreenDepth;
    bool LowRes;
};

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define WIN_32_LEAN_AND_MEAN                    // MFC abschalten
#define WINDOWCLASSNAME "HurricanWindowsClass"  // Name der Windows-Klasse

#define SCREENWIDTH 640
#define SCREENHEIGHT 480

#define LOWRES_SCREENWIDTH 320
#define LOWRES_SCREENHEIGHT 240

#define RENDERWIDTH 640
#define RENDERHEIGHT 480

// DKS - Screen depth is now configurable from the command line; if your
//      platform does not support the default value of 32, you may define it
//      by setting DEFAULT_SCREENBPP to correct value in Makefile.
#ifndef DEFAULT_SCREENBPP
#define DEFAULT_SCREENBPP 32
#endif

#define PARAM_NONE 0
#define PARAM_LEVEL 1
#define PARAM_LIST 2

// --------------------------------------------------------------------------------------
// Prototypen von Funktionen
// --------------------------------------------------------------------------------------

bool GameInit();  // Spiel vor  dem Cracktro initialisieren
bool GameInit2();                           // Spiel nach dem Cracktro initialisieren
bool GameExit();                            // Spiel de-initialisieren
bool Heartbeat();                           // Haupt Game Loop

// DKS - I made a separate set of spritesheets with blue coloring for Player 2, so these
//      are no longer necessary and have been disabled:
#if 0
void ConvertPlayerTexture(DirectGraphicsSprite *pTexture);
void CreatePlayer2Texture();
#endif  // 0

void StartOuttro();
void StartIntro();

extern int WINDOWWIDTH;
extern int WINDOWHEIGHT;
extern sCommandLineParams CommandLineParams;  // Externer Paramter?

extern char *g_storage_ext;  // Where data files (levels, graphics, music, etc) for the game are stored (read)
extern char *g_save_ext;     // Where configuration files, logs, and save games are written (-DKS) (write)

#ifdef _DEBUG
void ShowDebugInfo();  // Allen möglichen Kram anzeigen
#endif

// DKS - Added FPS reporting:
void ShowFPS();

// --------------------------------------------------------------------------------------
// Endianess handling
// We will Swap values only for Big_Endian, Little_Endian should be unchanged
// --------------------------------------------------------------------------------------
static inline uint32_t FixEndian(uint32_t x) {
#if defined(PLATFORM_SDL) && (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    return SDL_Swap32(x);
#else
    return x;
#endif
}

static inline int32_t FixEndian(int32_t x) {
#if defined(PLATFORM_SDL) && (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    uint32_t val = SDL_Swap32(*reinterpret_cast<uint32_t *>(&x));
    return *reinterpret_cast<int32_t *>(&val);
#else
    return x;
#endif
}

#endif
