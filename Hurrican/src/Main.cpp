// Datei : Main.cpp

// --------------------------------------------------------------------------------------
//
// Hurrican
//
// Shoot em up ala Turrican
// benutzt die DirectX8.1 API für Grafik, Sound und Input
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

//DKS - Always show cracktro when debugging
//#ifndef _DEBUG
#define SHOW_CRACKTRO
//#endif

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#if defined(PLATFORM_DIRECTX)
#include <windows.h>									// Alle Windows Header includen
#include <Dxerr8.h>
#endif
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

#include "Main.h"
#include "Texts.h"
#include "Console.h"
#include "CCracktro.h"
#include "DX8Texture.h"
#include "DX8Graphics.h"
#include "DX8Sprite.h"
#include "DX8Input.h"
#include "DX8Sound.h"
#include "DX8Font.h"
#include "Gameplay.h"
#include "GUISystem.h"
#include "GegnerClass.h"
#include "Globals.h"
#include "HUD.h"
#include "Intro.h"
#include "Outtro.h"
#include "Logdatei.h"
#include "Menu.h"
#include "Partikelsystem.h"
#include "Player.h"
#include "Projectiles.h"
#include "resource.h"
#include "Tileengine.h"
#include "Timer.h"
#include "Math.h"
#include "unrarlib.h"

#if defined(ANDROID)
#include <android/log.h>
#endif

using namespace std;

// Memory Leaks

//#include <stdlib.h>
//#include <crtdbg.h>

// --------------------------------------------------------------------------------------
// externe Variablen
// --------------------------------------------------------------------------------------

extern bool						DEMORecording;
extern bool						DEMOPlaying;
extern DirectGraphicsSprite		*PartikelGrafix[MAX_PARTIKELGFX];	// Grafiken der Partikel

// --------------------------------------------------------------------------------------
// globale Variablen
// --------------------------------------------------------------------------------------

#if defined(PLATFORM_DIRECTX)
D3DFORMAT				D3DFormat;						// Format der Primary Surface
D3DCAPS8				d3dcaps;						// Möglichkeiten der Hardware
LPDIRECT3D8				lpD3D			= NULL;			// Direct3D Hauptobjekt
LPDIRECT3DDEVICE8		lpD3DDevice		= NULL;			// Direct3D Device-Objekt
LPDIRECT3DSURFACE8		lpBackbuffer	= NULL;			// Der Backbuffer
HWND					DesktopHWND		= NULL;			// Handle des Desktops
HWND					g_hwnd			= NULL;			// Handle des Hauptfensters
HINSTANCE				g_hinst;						// hinstance des Hauptfensters
#endif

bool					FixedFramerate		= false;		// true = Spiel mit 50 Frames laufen lassen
// false = Spiel so flüssig wie möglich laufen lassen
bool					Sprache;						// true == deutsch / false == englisch
bool					GameRunning			= true;		// Spiel läuft :-)
bool					GamePaused			= false;	// Spiel eingefroren (wenn man zb das Fenster verlässt)
bool					NochKeinFullScreen	= true;		// Logo noch anzeigen in Paint ?
#ifdef _DEBUG
bool					DebugMode			= false;	// Debug Mode ein/aus
#endif //_DEBUG
float					SpeedFaktor = 1.0f;				// Faktor, mit dem alle Bewegungen verrechnet werden
TexturesystemClass      Textures;                       // DKS - Added Texturesystem class (see DX8Sprite.cpp)
DirectGraphicsClass		DirectGraphics;					// Grafik-Objekt
DirectInputClass		DirectInput;					// Input-Objekt
TimerClass				Timer;						    // Timer Klasse für die Framerate
#if defined(__AROS__)
Logdatei				Protokoll("T:Game_Log.txt");		// Protokoll Datei
#else
Logdatei				Protokoll("Game_Log.txt");		// Protokoll Datei
#endif
SoundManagerClass       SoundManager;                   // Sound Manager
DirectGraphicsFont		*pDefaultFont = new(DirectGraphicsFont);
DirectGraphicsFont		*pMenuFont	  = new(DirectGraphicsFont);
TileEngineClass			TileEngine;                     // Tile Engine
PartikelsystemClass		*pPartikelSystem;				// Das coole Partikelsystem
ProjectileListClass		Projectiles;					// Liste mit Schüssen
GegnerListClass			*pGegner;						// Liste mit Gegner
IntroClass				*pIntro;						// Intro-Objekt
OuttroClass				*pOuttro;						// Outtro-Objekt
MenuClass				*pMenu = NULL;					// Hauptmenu-Objekt
ConsoleClass			*pConsole;						// Konsolen-Objekt
CGUISystem				*pGUI;							// GUI System
CCracktro				*Cracktro;
RECT					srcrect, destrect;

int						LanguageFileCount;				// Anzahl gefundener Language Files
char					LanguageFiles[MAX_LANGUAGE_FILES][MAX_LANGUAGE_FILENAME_LENGTH]; 
char					ActualLanguage[256];			// Gewählte Language
char                    *g_storage_ext = NULL;          // Where data files (levels, graphics, music, etc) 
                                                        //      for the game are stored (read)
char                    *g_save_ext = NULL;             // Where configuration files, logs, and save games 
                                                        //      are written (-DKS) (write)

sCommandLineParams		CommandLineParams;

int WINDOWWIDTH;
int WINDOWHEIGHT;


// --------------------------------------------------------------------------------------
// Variablen für den Spielablauf
// --------------------------------------------------------------------------------------

#if defined(PLATFORM_DIRECTX)
HBITMAP					SplashScreen = NULL;			// SplashScreen Grafik
#endif

//DKS - PlayerClass array is now static, not dynamically-allocated:
//PlayerClass				*pPlayer[2];					// Werte der Spieler
PlayerClass				Player[2];					// Werte der Spieler

HUDClass				*pHUD;							// Das HUD
unsigned char			SpielZustand = CRACKTRO;		// Aktueller Zustand des Spieles
char					StringBuffer[100];				// Für die Int / String Umwandlung

// --------------------------------------------------------------------------------------
// Callback Funktion
// --------------------------------------------------------------------------------------

#if defined(PLATFORM_DIRECTX)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch(message)
    {
    case WM_CREATE:								// Splash Screen beim erstellen des
    {
        // Fensters laden
        SplashScreen = LoadBitmap (g_hinst, MAKEINTRESOURCE(IDB_SPLASHSCREEN));
        return (0);
    }
    break;

    case WM_PAINT:								// Beim starten den Splash Screen anzeigen
    {
        if (NochKeinFullScreen == true &&
                CommandLineParams.RunWindowMode == false)
        {
            PAINTSTRUCT		ps;
            HDC				hdc, hdcMem;

            hdc = BeginPaint (g_hwnd, &ps);

            hdcMem = CreateCompatibleDC (hdc);
            SelectObject (hdcMem, SplashScreen);

            BitBlt (hdc, -1, -1, 450, 110, hdcMem, 0, 0, SRCCOPY);

            DeleteDC (hdcMem);
            EndPaint (g_hwnd, &ps);

            InvalidateRect (g_hwnd, NULL, false);
        }
    }
    break;

    case WM_DESTROY:								// Fenster wird geschlossen
    {
        GameRunning = false;						// Spiel beenden
        PostQuitMessage(0);							// Quit-Message posten
        return(0);									// Success zurückliefern
    }
    break;

    case WM_SYSCOMMAND:								// "Alt aktiviert Menu" ausschalten
    {
        if (wparam == SC_TASKLIST ||
                wparam == SC_KEYMENU)
            return (0);
    }
    break;

    case WM_ACTIVATE:
    {
        int Active = LOWORD(wparam);           // activation flag

        if (Active == WA_INACTIVE)
        {
            SoundManager.PauseSongs();
            SoundManager.PauseSounds();

            GamePaused = true;
        }
        else
        {
            SoundManager.UnpauseSongs();
            SoundManager.UnpauseSounds();

            GamePaused = false;
        }

    }
    break;

    default :
        break;
    }

    //unbearbeitete Nachrichten zurückliefern
    return(DefWindowProc(hwnd, message, wparam, lparam));
}
#endif

// --------------------------------------------------------------------------------------
// File Exists Funktion
// --------------------------------------------------------------------------------------

bool FileExists(const char* Filename)
{
    if (!Filename)
        return false;

    std::fstream fin;

    fin.open(Filename,std::ios::in);
    if (fin.is_open())
    {
        fin.close();
        return true;
    }

    fin.close();
    return false;
}

//DKS - Added function:
// Create the directory dir if it doesn't already exist. Return true if created or already exists.
bool CreateDir(const char *dir)
{
    if (!dir) return false;

#if defined (PLATFORM_DIRECTX)
	struct _stat	st;
	return (_stat(dir, &st) != 0) ? (mkdir(dir) == 0) : (_S_ISDIR(st.st_mode) != 0);
#else
	struct stat	st;
	return (stat(dir, &st) != 0) ? (mkdir(dir, 0755) == 0) : (S_ISDIR(st.st_mode) != 0);
#endif
}

//DKS - Added function:
// If directory exists (and is indeed a directory), return true
bool FindDir(const char *dir)
{
#if defined (PLATFORM_DIRECTX)
    struct _stat st;
    if (!dir || _stat(dir, &st) != 0)
        return false;
    else 
        return _S_ISDIR(st.st_mode) != 0;
#else
    struct stat st;
    if (!dir || stat(dir, &st) != 0)
        return false;
    else 
        return S_ISDIR(st.st_mode) != 0;
#endif
}

int GetStringPos(const char
                 *string, const char *substr)
{
    int len = strlen(string);
    for (int i = 0; i < len; i++)
    {
        int index = 0;

        while(string[i] == substr[index])
        {
            i++;
            index++;

            int len = strlen(substr);
            if (index >= len)
                return i + 1;
        }
    }

    return -1;
}

#if defined(PLATFORM_DIRECTX)
void FillCommandLineParams(void)
{
    int windowpos;
    int listpos;
    int levelpos;
    char buffer[256];
    CommandLineParams.DataPath = NULL;
    CommandLinesParams.SavePath = NULL;
//	char *temppos;

    // windowmode?
    windowpos = GetStringPos(CommandLineParams.Params, "windowmode");
    CommandLineParams.RunWindowMode = windowpos > -1;

    // own levellist?
    listpos = GetStringPos(CommandLineParams.Params, "custom");
    CommandLineParams.RunOwnLevelList = listpos > -1;
    if (CommandLineParams.OwnLevelList)
    {
        int i = 0;
        int len = strlen(CommandLineParams.Params);
        for (i = 0; i < len; i++)
        {
            if (CommandLineParams.Params[listpos + i] == 0  ||
                    CommandLineParams.Params[listpos + i] == 32 ||
                    CommandLineParams.Params[listpos + i] == 10)
                break;

            buffer[i] = CommandLineParams.Params[listpos + i];
        }

        buffer[i] = 0;
        strcpy_s(CommandLineParams.OwnLevelList, strlen(buffer) + 1, buffer);
    }

    // own single level?
    levelpos = GetStringPos(CommandLineParams.Params, "level");
    CommandLineParams.RunUserLevel = levelpos > -1;
    if (CommandLineParams.RunUserLevel)
    {
        int i = 0;
        int len = strlen(CommandLineParams.Params);
        for (i = 0; i < len; i++)
        {
            if (CommandLineParams.Params[levelpos + i] == 0  ||
                    CommandLineParams.Params[levelpos + i] == 32 ||
                    CommandLineParams.Params[levelpos + i] == 10)
                break;

            buffer[i] = CommandLineParams.Params[levelpos + i];
        }

        buffer[i] = 0;
        strcpy_s(CommandLineParams.UserLevelName, strlen(buffer) + 1, buffer);
    }

    //DKS - Forced this for now for new option on Windows: (TODO: fix this to be flexible on windows too)
    CommandLineParams.ScreenDepth = 32;
}

#elif defined(PLATFORM_SDL)
void FillCommandLineParams( int argc, char* args[] )
{
    uint16_t i;

    // Set some sensible defaults
    CommandLineParams.RunWindowMode = false;
    CommandLineParams.TexFactor = 1;
    CommandLineParams.TexSizeMin = 1024;
    CommandLineParams.ScreenDepth = DEFAULT_SCREENBPP;
    CommandLineParams.VSync = true;
    CommandLineParams.ShowFPS = false;
    CommandLineParams.AllowNPotTextureSizes = false;
    CommandLineParams.LowRes = false;
    CommandLineParams.DataPath = NULL;
    CommandLineParams.SavePath = NULL;

    for (i=1; i<argc; i++)
    {
        if ((strstr( args[i], "--help" ) != NULL) || (strstr( args[i], "-?") != NULL ) || 
                (strstr( args[i], "-H") !=NULL) || (strstr( args[i], "-h") != NULL))
        {
            Protokoll.WriteText( false, "Hurrican\n"  );
            Protokoll.WriteText( false, "  Usage      : hurrican <arguments>\n" );
            Protokoll.WriteText( false, "  Arguments\n" );
            Protokoll.WriteText( false, "  -H,-?, --help           : Show this information\n" );
            Protokoll.WriteText( false, "  -W,    --windowmode     : Run in a window, not fullsreen\n" );
            Protokoll.WriteText( false, "  -F,    --showfps        : Show the current frames per second\n" );
            Protokoll.WriteText( false, "  -D x,  --depth x        : Set screen pixel depth to x (16, 24, 32)\n" );
            Protokoll.WriteText( false, "                            ( Default is %d )\n", DEFAULT_SCREENBPP );
            Protokoll.WriteText( false, "  -L,    --lowres         : Use %dx%d low-resolution screen dimensions\n",
                                                                        LOWRES_SCREENWIDTH, LOWRES_SCREENHEIGHT );
            Protokoll.WriteText( false, "  -NV,   --novsync        : Disable VSync / double-buffering\n" );
            Protokoll.WriteText( false, "  -NP,   --nonpot         : Allow non-power-of-two texture sizes\n" );
            Protokoll.WriteText( false, "                            Normally, GPUs require texture dimensions that are\n" );
            Protokoll.WriteText( false, "                            powers of two. If your GPU does not require that,\n" );
            Protokoll.WriteText( false, "                            you can reduce VRAM usage with this switch.\n" );
            Protokoll.WriteText( false, "  -TF x, --texfactor x    : Division factor for textures\n" );
            Protokoll.WriteText( false, "                            Valid values: 1, 2, 4\n" );
            Protokoll.WriteText( false, "                            If set to 2, textures dimensions will be halved.\n" );
            Protokoll.WriteText( false, "                            If set to 4, textures dimensions will be quartered.\n" );
            Protokoll.WriteText( false, "                            ( Default is 1 (no resizing) )\n" );
            Protokoll.WriteText( false, "  -TS x, --texsizemin x   : Size limitation for texture division factor\n" );
            Protokoll.WriteText( false, "                            Only textures with widths or heights above this\n" );
            Protokoll.WriteText( false, "                            value will be resized. MIN: 16  MAX: 1024\n" );
            Protokoll.WriteText( false, "                            ( Default is 1024 )\n" );
            Protokoll.WriteText( false, "  -PD x, --pathdata x     : Look in this path for the game's read-only data\n" );
            Protokoll.WriteText( false, "                            i.e. music, sound, graphics, levels, etc.\n" );
            Protokoll.WriteText( false, "  -PS x, --pathsave x     : Use this path for the game's save data\n ");
            Protokoll.WriteText( false, "                            i.e. save-games, settings, high-scores, etc.\n" );
            exit(1);
        }
        else if ((strstr( args[i], "--windowmode" ) != NULL) || (strstr( args[i], "-W") != NULL))
        {
            CommandLineParams.RunWindowMode = true;
            fprintf( stdout, "Window mode is enabled\n" );
        }
        else if ((strstr( args[i], "--showfps" ) != NULL) || (strstr( args[i], "-F") != NULL))
        {
            CommandLineParams.ShowFPS = true;
            fprintf( stdout, "FPS will be displayed\n" );
        }
        else if ((strstr( args[i], "--depth" ) != NULL) || (strstr( args[i], "-D") != NULL))
        {
            i++;
            if (i<argc)
            {
                CommandLineParams.ScreenDepth = LIM(atoi(args[i]), 16, 32);
                if (CommandLineParams.ScreenDepth >= 32)
                    CommandLineParams.ScreenDepth = 32;
                else if (CommandLineParams.ScreenDepth > 24 && CommandLineParams.ScreenDepth < 32)
                    CommandLineParams.ScreenDepth = 24;
                else if (CommandLineParams.ScreenDepth > 16 && CommandLineParams.ScreenDepth < 24)
                    CommandLineParams.ScreenDepth = 16;
                fprintf( stdout, "Screen depth (bpp) requested is %d\n", CommandLineParams.ScreenDepth);
            }
        }
        else if ((strstr( args[i], "--lowres" ) != NULL) || (strstr( args[i], "-L") != NULL))
        {
            fprintf( stdout, "Low-resolution 320x240 screen dimensions are requested\n" );
            CommandLineParams.LowRes = true;
        }
        else if ((strstr( args[i], "--novsync" ) != NULL) || (strstr( args[i], "-NV") != NULL))
        {
            fprintf( stdout, "VSync / double-buffering will disabled, if supported\n" );
            CommandLineParams.VSync = false;
        }
        else if ((strstr( args[i], "--nonpot" ) != NULL) || (strstr( args[i], "-NP") != NULL))
        {
            fprintf( stdout, "Non-power-of-two textures are allowed\n" );
            CommandLineParams.AllowNPotTextureSizes = true;
        }
        else if ((strstr( args[i], "--texfactor" ) != NULL) || (strstr( args[i], "-TF") != NULL))
        {
            i++;
            if (i<argc)
            {
                CommandLineParams.TexFactor = LIM(atoi(args[i]), 1, 4);
                if (CommandLineParams.TexFactor == 3)
                    CommandLineParams.TexFactor = 4;
                fprintf( stdout, "Texfactor set to %d\n", CommandLineParams.TexFactor );
            }
        }
        else if ((strstr( args[i], "--texsizemin" ) != NULL) || (strstr( args[i], "-TS") != NULL))
        {
            i++;
            if (i<argc)
            {
                CommandLineParams.TexSizeMin = LIM(atoi(args[i]), 16, 1024);
                fprintf( stdout, "Texsizemin set to %d\n", CommandLineParams.TexSizeMin );
            }
        }
        else if ((strstr( args[i], "--pathdata" ) != NULL) || (strstr( args[i], "-PD") != NULL))
        {
            i++;
            if (i<argc) {
                if (args[i] && strlen(args[i]) > 0 && !CommandLineParams.DataPath) {
                    CommandLineParams.DataPath = (char*)malloc(strlen(args[i] + 1));
                    strcpy_s(CommandLineParams.DataPath, args[i]);
                    if (FindDir(CommandLineParams.DataPath)) {
                        fprintf( stdout, "Data path set to %s\n", CommandLineParams.DataPath );
                    } else {
                        fprintf( stdout, "ERROR: could not find data path %s\n", CommandLineParams.DataPath );
                        free(CommandLineParams.DataPath);
                        CommandLineParams.DataPath = NULL;
                    }
                }
            }
        }
        else if ((strstr( args[i], "--pathsave" ) != NULL) || (strstr( args[i], "-PS") != NULL))
        {
            i++;
            if (i<argc) {
                if (args[i] && strlen(args[i]) > 0 && !CommandLineParams.SavePath) {
                    CommandLineParams.SavePath = (char*)malloc(strlen(args[i] + 1));
                    strcpy_s(CommandLineParams.SavePath, args[i]);
                    if (CreateDir(CommandLineParams.SavePath)) {
                        fprintf( stdout, "Save path set to %s\n", CommandLineParams.SavePath );
                    } else {
                        fprintf( stdout, "ERROR: could not find save path %s\n", CommandLineParams.SavePath );
                        free(CommandLineParams.SavePath);
                        CommandLineParams.SavePath = NULL;
                    }
                }
            }
        }
        else if ((strstr( args[i], "--npot" ) != NULL) || (strstr( args[i], "-NP") != NULL))
        {
            fprintf( stdout, "Non-power-of-two textures are allowed\n" );
            CommandLineParams.AllowNPotTextureSizes = true;
        }
        else if (strstr( args[i], "--custom" ) != NULL)
        {
            i++;
            if (i < argc && strlen(args[i]) < 256) {
                strcpy(CommandLineParams.OwnLevelList, args[i]);
                CommandLineParams.RunOwnLevelList = true;
            }
        }
        else if (strstr( args[i], "--level" ) != NULL)
        {
            // own single level?
            i++;
            if (i < argc && strlen(args[i]) < 256) {
                strcpy(CommandLineParams.UserLevelName, args[i]);
                CommandLineParams.RunUserLevel = true;
            }
        }
    }
}

#endif

// --------------------------------------------------------------------------------------
// Win-Main Funktion
// --------------------------------------------------------------------------------------

#if defined(PLATFORM_DIRECTX)
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstace,
                   LPSTR lpcmdline,		int nshowcmd)
{
#elif defined(PLATFORM_SDL)
int main(int argc, char *argv[])
{
#endif

    //DKS - Added optional Sin/Cos lookup table
#ifdef USE_TRIG_LOOKUP_TABLE
   populate_sin_table();
#endif //USE_TRIG_LOOKUP_TABLE

    GamePaused = false;

#if defined(PLATFORM_DIRECTX)
    WNDCLASSEX			winclass;							// eigene Windows-Klasse
    MSG					message;							// Message
    RECT				rect;								// Grösse des Desktops

    // evtle Parameter holen und Typ des Parameters rausfinden
    strcpy_s (CommandLineParams.Params, 1, "");

    if (strlen(lpcmdline) != 0)
        strcpy_s (CommandLineParams.Params, strlen(lpcmdline) + 1, lpcmdline);

    FillCommandLineParams();

#elif defined(PLATFORM_SDL)
    HWND g_hwnd = 0;
    HINSTANCE hinstance = 0;

    FillCommandLineParams( argc, argv );

#endif

    if (CommandLineParams.RunWindowMode)
    {
        WINDOWWIDTH	 = 1024;
        WINDOWHEIGHT = 768;
    }
    else
    {
        WINDOWWIDTH	 = 449;
        WINDOWHEIGHT = 109;
    }

    // Set game's data path:
    g_storage_ext = NULL;
    // First, see if a command line parameter was passed:
    if (CommandLineParams.DataPath) {
        g_storage_ext = (char*)malloc(strlen(CommandLineParams.DataPath+1));
        strcpy_s(g_storage_ext, CommandLineParams.DataPath);
        free(CommandLineParams.DataPath);
        CommandLineParams.DataPath = NULL;
    } else
    {
#if defined(ANDROID)
        g_storage_ext = (char*)malloc(strlen(SDL_AndroidGetExternalStoragePath() + 1));
        strcpy_s(g_storage_ext, SDL_AndroidGetExternalStoragePath());
#else // NON-ANDROID:
#ifdef USE_STORAGE_PATH
        // A data-files storage path has been specified in the Makefile:
        g_storage_ext = (char*)malloc(strlen(USE_STORAGE_PATH) + 1);
        strcpy_s(g_storage_ext, USE_STORAGE_PATH);
        // Attempt to locate the dir
        if (!FindDir(g_storage_ext)) {
            // Failed, print message and use "." folder as fall-back
            Protokoll.WriteText( false, "ERROR: Failed to locate data directory %s\n", g_storage_ext );
            Protokoll.WriteText( false, "\tUsing '.' folder as fallback.\n" );
            g_storage_ext = (char*)malloc(strlen(".") + 1);
            strcpy_s(g_storage_ext, ".");
        }
#else
        g_storage_ext = (char*)malloc(strlen(".") + 1);
        strcpy_s(g_storage_ext, ".");
#endif
#endif //ANDROID
    }

    // Set game's save path (save games, settings, logs, high-scores, etc)
    g_save_ext = NULL;
    if (CommandLineParams.SavePath) {
        g_save_ext = (char*)malloc(strlen(CommandLineParams.SavePath+1));
        strcpy_s(g_save_ext, CommandLineParams.SavePath);
        free(CommandLineParams.SavePath);
        CommandLineParams.SavePath = NULL;
    } else
    {
#if defined(ANDROID)
        g_save_ext = (char*)malloc(strlen(SDL_AndroidGetExternalStoragePath() + 1));
        strcpy_s(g_save_ext, SDL_AndroidGetExternalStoragePath());
#else // NON-ANDROID:
#ifdef USE_HOME_DIR
    // Makefile is specifying this is a UNIX machine and we should write saves,settings,etc to $HOME/.hurrican/ dir
        char *homedir = getenv("HOME");
        bool success = false;
        if (homedir) {
            const char *subdir = "/.hurrican";
            g_save_ext = (char*)malloc(strlen(homedir) + strlen(subdir) + 1);
            strcpy_s(g_save_ext, homedir);
            strcat_s(g_save_ext, subdir);
            success = CreateDir(g_save_ext);
            if (!success) {
                // We weren't able to create the $HOME/.turrican directory, or if it exists, it is
                // not a directory or is not accessible somehow.. 
                Protokoll.WriteText( false, "ERROR: unable to create or access $HOME/.hurrican/ directory.\n" );
                Protokoll.WriteText( false, "\tFull path that was tried: %s\n", g_save_ext );
                free(g_save_ext);
            }
        } else {
            // We weren't able to find the $HOME env var
            Protokoll.WriteText( false, "ERROR: unable to find $HOME environment variable\n" );
            success = false;
        }

        if (!success) {
            Protokoll.WriteText( false, "\tUsing '.' folder as fallback.\n" );
            g_save_ext = (char*)malloc(strlen(".") + 1);
            strcpy_s(g_save_ext, ".");
        }
#else
        g_save_ext = (char*)malloc(strlen(".") + 1);
        strcpy_s(g_save_ext, ".");
#endif //USE_HOME_DIR
#endif //ANDROID
    }

    Protokoll.WriteText( false, "--> Using external storage path '%s' <--\n", g_storage_ext );
    Protokoll.WriteText( false, "--> Using save path '%s' <--\n\n", g_save_ext );

#if defined(PLATFORM_DIRECTX)
    // Desktop Window holen und Grösse auslesen (damit wir unser Fenster in der Mitte des Screens
    // positionnieren können)
    DesktopHWND = GetDesktopWindow();
    GetWindowRect(DesktopHWND, &rect);

    // Anfang der Logdatei mit Datum und Uhrzeit
    Protokoll.WriteText( false, ">-------------------------<\n" );
    Protokoll.WriteText( false, "|        Hurrican         |\n" );
    Protokoll.WriteText( false, "|   (c) 2007 poke53280    |\n" );
    Protokoll.WriteText( false, "|                         |\n" );
    Protokoll.WriteText( false, "|    www.poke53280.de     |\n" );
    Protokoll.WriteText( false, "|  www.hurrican-game.de   |\n" );
    Protokoll.WriteText( false, ">-------------------------<\n" );
    Protokoll.WriteText( false, "Logfile date: ", false);
    strcpy_s(StringBuffer, __DATE__);
    Protokoll.WriteText(StringBuffer, false);
    Protokoll.WriteText(" - ", false);
    strcpy_s(StringBuffer, __TIME__);
    Protokoll.WriteText(StringBuffer, false);

    Protokoll.WriteText( false, "\n\n>-------------<\n" );
    Protokoll.WriteText( false,     "| Init Window |\n" );
    Protokoll.WriteText( false,     ">-------------<\n\n" );

    g_hinst = hinstance;

    // Werte für die Windows-Klasse festlegen
    winclass.cbSize			= sizeof (WNDCLASSEX);					// Grösse der Klasse
    winclass.style			= CS_HREDRAW | CS_VREDRAW;				// Fenster-Einstellungen
    winclass.lpfnWndProc	= WindowProc;							// Callback Funktion
    winclass.cbClsExtra		= 0;									// extra Klassen-Info Space
    winclass.cbWndExtra		= 0;									// extra Fenster-Info Space
    winclass.hInstance		= hinstance;							// Fenster-Handle
    winclass.hIcon			= LoadIcon(hinstance, MAKEINTRESOURCE(IDI_ICON1));		// Fenster-Icon
    winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);			// Mauscursor setzen
    winclass.hbrBackground  = CreateSolidBrush(RGB(0, 0, 0));		// Hintergrundfarbe setzen
    winclass.lpszMenuName	= NULL;									// Menu-Name
    winclass.lpszClassName	= WINDOWCLASSNAME;						// Name der neuen Klasse
    winclass.hIconSm		= LoadIcon(hinstance, MAKEINTRESOURCE(IDI_ICON1));	// Task-Leiste Icon laden

    // Fensterklasse bei Windows registrieren
    if (!RegisterClassEx(&winclass))
    {
        Protokoll.WriteText( true, "RegisterClassEx error!\n" );
        return(0);
    }

    Protokoll.WriteText( false, "RegisterClassEx successful!\n" );

    DWORD style;

    if (CommandLineParams.RunWindowMode)
        style = WS_OVERLAPPED	|						// Fenster Style
                WS_CAPTION		|
                WS_SYSMENU		|
                WS_BORDER		|
                WS_MINIMIZEBOX	|
                WS_MAXIMIZEBOX	|
                WS_SIZEBOX	    |
                WS_VISIBLE;
    else
        style = WS_POPUPWINDOW;

    // Neues Fenster mit der registrierten Klasse erstellen
    if (!(g_hwnd = CreateWindowEx(NULL,								// Eigenschaften
                                  WINDOWCLASSNAME,					// Name der neuen Klasse
                                  "Hurrican",							// Titel des Fenster
                                  style,								// ohne Alles =)
                                  (rect.right - WINDOWWIDTH)/2,		// x und y des Fensters
                                  (rect.bottom- WINDOWHEIGHT)/2,		// in der Mitte zentriert
                                  WINDOWWIDTH,						// Fensterbreite
                                  WINDOWHEIGHT,						// Fensterhöhe
                                  NULL,								// Handle des Parentfensters
                                  NULL,								// Handle des Menus
                                  hinstance,							// Instance von Main
                                  NULL)))								// extra creation parms
    {
        Protokoll.WriteText( true, "CreateWindowEx error!\n" );
        return(0);
    }

    Protokoll.WriteText( false, "CreateWindowEx	successful!\n" );
    Protokoll.WriteText( false, "WindowSizeX : " );
    Protokoll.WriteValue(WINDOWWIDTH);
    Protokoll.WriteText( false, "WindowSizeY : " );
    Protokoll.WriteValue(WINDOWHEIGHT);

    if (CommandLineParams.RunWindowMode == false)
        ShowCursor(false);

    Protokoll.WriteText( false, "\n-> Init Window successful!\n" );

    ShowWindow(g_hwnd, nshowcmd);						// Fenster anzeigen (sicher ist sicher)
    UpdateWindow(g_hwnd);								// Fenster-infos updaten
#endif

//----- Spiel-Initialisierung

    if(!GameInit(g_hwnd, hinstance))
    {
        Protokoll.WriteText( true, "\n-> GameInit error!\n\n" );
        GameRunning = false;
    }
    else
        Protokoll.WriteText( false, "\n-> GameInit successful!\n\n" );

//----- Main-Loop

    while(GameRunning == true)
    {
#if defined(PLATFORM_DIRECTX)
        while (PeekMessage (&message, NULL, 0, 0, PM_REMOVE))	// Nachricht vom Stapel holen
        {
            // und löschen
            TranslateMessage(&message);							// Nachricht überetzen
            DispatchMessage(&message);							// Nachricht an WinProc weiterleiten
            UpdateWindow(g_hwnd);
        }
#endif
#if defined(PLATFORM_SDL)
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                GameRunning = false;
        }
#endif

        //DKS - Exceptions can now be disabled, reducing unnecessary code-bloat:
#ifndef USE_NO_EXCEPTIONS
		try
#endif // USE_NO_EXCEPTIONS
        {
            if (GamePaused == false)
            {
                // Main Loop
                Heartbeat();

                // Eingabegeräte updaten
                DirectInput.UpdateTastatur  ();
                DirectInput.UpdateJoysticks ();
                //DirectInput.UpdateMaus(false);

                // Soundchannels updaten
                SoundManager.Update();

                // Timer updaten
                Timer.update();

                // Feste Framerate ? (Spiel läuft in Zeitlupe, wenn zu langsam)
                //
                if (FixedFramerate == true)
                {
                    Timer.SetMaxFPS (60);
                    SpeedFaktor = 1.0f / 60.0f * Timer.GetMoveSpeed();
                }
                else
                {
                    //Timer.SetMaxFPS (0);
                    SpeedFaktor = Timer.SpeedFaktor;
                }

                Timer.wait();

                // Bei Demo immer gleichen Speedfaktor setzen
                //
                if (DEMORecording == true ||
                        DEMOPlaying	  == true)
                    SpeedFaktor = 0.28f;
            }
        }
        //DKS - Exceptions can now be disabled, reducing unnecessary code-bloat:
#ifndef USE_NO_EXCEPTIONS
        catch(const char *str)
        {
            Protokoll.WriteText( false, "Failure! Unhandled exception\n %s", str );
            GameRunning = false;
        }
#endif // USE_NO_EXCEPTIONS
    }

//----- Spiel verlassen

    //Timer.WriteLogValues();

    if(!GameExit())
        Protokoll.WriteText( true, "-> GameExit Fehler !\n" );

    Protokoll.WriteText( false, "\n-> Hurrican closed !\n" );
    Protokoll.WriteText( false, "\nhttp://www.poke53280.de\n" );
    Protokoll.WriteText( false, "Bugreports, questions etc : information@poke53280.de\n" );
    Protokoll.WriteText( false, "\n-> logfile end\n" );

    // Kein Fehler im Game? Dann Logfile löschen
    if (Protokoll.delLogFile == true)
        DeleteFile("Game_Log.txt");

    free(g_storage_ext);
    free(g_save_ext);

#if defined(PLATFORM_DIRECTX)
    return(message.wParam);										// Rückkehr zu Windows
#elif defined(PLATFORM_SDL)
    return 0;
#endif
}

// --------------------------------------------------------------------------------------
// GameInit, initialisiert die DX Objekte
// --------------------------------------------------------------------------------------

bool GameInit(HWND hwnd, HINSTANCE hinstance)
{
    options_Detail = DETAIL_LOW;

    srand(timeGetTime());

    //DKS - added fast RNG, this is to ensure it always gets seeded, though the above should already do so:
#ifdef USE_FAST_RNG
    seed_fast_rand(timeGetTime());
#endif //USE_FAST_RNG

#if defined(PLATFORM_DIRECTX)
    // Language Files
    HWND	ComboBoxLanguageFiles	= NULL;
    ComboBoxLanguageFiles = CreateWindow("COMBOBOX",
                                         "",
                                         WS_CHILD,
                                         0, 0, 0, 0, g_hwnd, 0, g_hinst, NULL);

    // Loadinfo Text festlegen
//	DisplayHintNr = rand()%30;

    // *.lng Files anfügen
    if (SendMessage(ComboBoxLanguageFiles, CB_DIR, DDL_READWRITE, (LPARAM) "*.lng") == CB_ERR)
    {
        Protokoll.WriteText( false, "No language Files found!", true);
    }

    LanguageFileCount = SendMessage (ComboBoxLanguageFiles, CB_GETCOUNT, 0, 0);
    for (int i = 0; i < LanguageFileCount; i++)
    {
        SendMessage (ComboBoxLanguageFiles, CB_GETLBTEXT, i, (LPARAM) LanguageFiles [i]);
    }
#elif defined(PLATFORM_SDL)
    //DKS - Added language-translation files support to SDL port:
    char langfilepath[256];
    if (g_storage_ext) {
        strcpy(langfilepath, g_storage_ext);
        strcat(langfilepath, "/lang");
    } else {
        strcpy(langfilepath, "./lang");
    }

    LanguageFileCount = FindLanguageFiles(langfilepath);

    // Try again if needed
    if (LanguageFileCount == 0) {
        strcpy(langfilepath, "./");
        LanguageFileCount = FindLanguageFiles(langfilepath);
    }

    // One more time if needed
    if (LanguageFileCount == 0) {
        strcpy(langfilepath, "./lang");
        LanguageFileCount = FindLanguageFiles(langfilepath);
    }

    if (LanguageFileCount == 0) {
        Protokoll.WriteText( false, "ERROR: Failed to find any language files, aborting.\n" );
        return false;
    }
#endif

    Protokoll.WriteText( false, "\n>--------------------<\n" );
    Protokoll.WriteText( false,   "| GameInit started   |\n" );
    Protokoll.WriteText( false,   ">--------------------<\n" );

    // Direct3D initialisieren
    if(!DirectGraphics.Init(hwnd, RENDERWIDTH, RENDERHEIGHT, CommandLineParams.ScreenDepth, CommandLineParams.VSync))
    {
        Protokoll.WriteText( true, "\n-> Direct3D Initialisierung Fehler ...!\n" );
        return false;
    }

    // DirectInput initialisieren
    if(!DirectInput.Init(hwnd, hinstance))
    {
        Protokoll.WriteText( true, "\n-> DirectInput8 Initialisierung Fehler ...!\n" );
        return false;
    }

    // DKS - Read texture scale factor files
    Textures.ReadScaleFactorsFiles();

#if defined(ANDROID)
    DirectInput.InitTouchBoxes( DirectGraphics.WindowView.w, DirectGraphics.WindowView.h );
#endif

    //DKS - Sound manager is now a static global, and initialized with Init()
    // Sound Manager initialisieren
    //pSoundManager = new CSoundManager();
    SoundManager.Init();

    // Splash-Screen nicht mehr anzeigen
    NochKeinFullScreen = false;

#ifdef SHOW_CRACKTRO
    Cracktro = new CCracktro();
    SpielZustand = CRACKTRO;
#endif

    return true;
}

//DKS - I made a separate set of spritesheets with blue coloring for Player 2, so these
//      are no longer necessary and have been disabled:
#if 0
// --------------------------------------------------------------------------------------
// Textur von Spieler 2 anpassen
// --------------------------------------------------------------------------------------

void ConvertPlayerTexture(DirectGraphicsSprite *pTexture)
{
#if defined(PLATFORM_DIRECTX)
    HRESULT hr;
    D3DLOCKED_RECT pLockedRect;
    int width, height, r, g, b, a, col, temp;
    BYTE  *pRow;
    DWORD *pPixel;

    // Textur locken
    //DKS - Adapted to new TexturesystemClass
    //hr = pTexture->itsTexture->LockRect(0, &pLockedRect, 0, 0);
    TextureHandle &th = Textures[pTexture->itsTexIdx];
    hr = th.tex->LockRect(0, &pLockedRect, 0, 0);
    if (hr != D3D_OK)
        return;

    // Breite, Höhe und Pitch setzen
    width  = (int)pTexture->itsXSize;
    height = (int)pTexture->itsYSize;
    pRow = (BYTE*)pLockedRect.pBits;

    for (int y = 0; y < height; y++)
    {
        pPixel = (DWORD*)pRow; //pPixel auf Zeilenstart setzen
        pRow += pLockedRect.Pitch; //Zeilenpointer eine Zeile weiter..

        for (int x = 0; x < width; x++)
        {
            // farbe holen
            col = (int)*pPixel;
            a = (col >> 24) & 255;
            r = (col >> 16) & 255;
            g = (col >>  8) & 255;
            b = (col) & 255;

            // Kein Colorkey?
            if (a > 0)
            {
                // Rot und Blau vertauschen
                if (b > r)
                {
                    temp = b;
                    b = r;
                    r = temp;
                }

                r = r * 3 / 2;

                if (r > 255)
                    r = 255;
            }

            temp = 0;

            *pPixel = D3DCOLOR_RGBA(b, g, r, a);

            *pPixel++;
        }
    }

    // Textur wieder freigeben
    //DKS - Adapted to new TexturesystemClass
    //pTexture->itsTexture->UnlockRect(0);
    th.tex->UnlockRect(0);
#endif
}

void CreatePlayer2Texture(void)
{
    ConvertPlayerTexture(&PlayerKucken[1]);
    ConvertPlayerTexture(&PlayerIdle[1]);
    ConvertPlayerTexture(&PlayerRun[1]);
    ConvertPlayerTexture(&PlayerRide[1]);
    ConvertPlayerTexture(&PlayerRad[1]);
    ConvertPlayerTexture(&PlayerBlitz[1]);
    ConvertPlayerTexture(&PlayerJump[1]);
    ConvertPlayerTexture(&PlayerCrouch[1]);
    ConvertPlayerTexture(&PlayerIdle2[1]);
    ConvertPlayerTexture(&PlayerJumpUp[1]);
    ConvertPlayerTexture(&PlayerJumpDiagonal[1]);
    ConvertPlayerTexture(&PlayerOben[1]);
    ConvertPlayerTexture(&PlayerPiss[1]);
    ConvertPlayerTexture(&PlayerPiss[1]);
    ConvertPlayerTexture(&PlayerDiagonal[1]);
}
#endif //0

// --------------------------------------------------------------------------------------
// GameInit2, initialisiert den Rest nach dem Cracktro
// --------------------------------------------------------------------------------------

bool GameInit2(void)
{
    //DKS-Player[] is a static global now:
    // Player initialisieren
    //Player[0] = new PlayerClass(0);
    //Player[1] = new PlayerClass(1);

    //DKS - Now that the player sprites are stored in the class, I've disabled this
    //      in favor of actual constructors:
    //Player[0]->SoundOff = 0;
    //Player[1]->SoundOff = 1;
    //memset(Player[0], 0, sizeof(*Player[0]));
    //memset(Player[1], 0, sizeof(*Player[1]));

    // Konfiguration laden
    if (LoadConfig() == false)
    {
        Protokoll.WriteText( false, "\n-> No config found. Creating default\n" );
        CreateDefaultConfig ();
    }

    // Menumusik laden und spielen
    SoundManager.LoadSong("menu.it",	MUSIC_MENU);

    //DKS - Renamed:
    //SoundManager.ResetAllSongVolumes();
    SoundManager.ResetAllSoundVolumes();

    SoundManager.PlaySong(MUSIC_MENU, false);

    // Menu initialisieren
    //DKS - Resized menufont.png and added missing glyphs to make Swedish translation work:
    //pMenuFont->LoadFont("menufont.png", 448, 256, 28, 28, 16, 7);
    pMenuFont->LoadFont("menufont.png", 448, 336, 28, 28, 16, 12, menufont_charwidths);
    pMenu = new MenuClass();

    // Fonts laden
    pDefaultFont->LoadFont  ("smallfont.png", 320, 84, 10, 12, 32, 7, smallfont_charwidths);

    //DKS - Added support for font scaling
    if (CommandLineParams.LowRes) {
        pDefaultFont->SetScaleFactor(2);    // On lower res, draw smallest font twice as large so it appears 1:1
    }

    pMenu->LoadingProgress = 0.0f;
    pMenu->LoadingItemsToLoad = 345;
    pMenu->LoadingItemsLoaded = 0;

    // GUISystem initialiseren
    pGUI = new CGUISystem();
    pGUI->InitGUISystem();

    // Partikelsystem initialisieren
    pPartikelSystem = new(PartikelsystemClass);

    // GegnerListe initialisieren
    pGegner = new(GegnerListClass);

    // Tileengine initialisieren
    TileEngine.LoadSprites();   //DKS - Added this function to TileEngineClass
    TileEngine.SetScrollSpeed(1.0f, 0.0f);

    // DKS Load projectile sprites:
    Projectiles.LoadSprites();

    // HUD initialisieren
    pHUD = new HUDClass();

    InitReplacers();

    // Sounds laden
    SoundManager.LoadWave("spreadshot.wav",   SOUND_SPREADSHOT, false);
    SoundManager.LoadWave("lasershot.wav",    SOUND_LASERSHOT, false);
    SoundManager.LoadWave("bounceshot.wav",   SOUND_BOUNCESHOT, false);
    SoundManager.LoadWave("explosion1.wav",   SOUND_EXPLOSION1, false);
    SoundManager.LoadWave("explosion2.wav",   SOUND_EXPLOSION2, false);
    SoundManager.LoadWave("explosion3.wav",   SOUND_EXPLOSION3, false);
    SoundManager.LoadWave("explosion4.wav",   SOUND_EXPLOSION4, false);
    SoundManager.LoadWave("walkergiggle.wav", SOUND_WALKERGIGGLE, false);
    SoundManager.LoadWave("collect.wav",		SOUND_COLLECT, false);
    SoundManager.LoadWave("hit.wav",		    SOUND_SPREADHIT, false);
    SoundManager.LoadWave("canon.wav",		SOUND_CANON, false);
    SoundManager.LoadWave("click.wav",		SOUND_CLICK, false);
    SoundManager.LoadWave("blitzstart.wav",	SOUND_BLITZSTART, false);
    SoundManager.LoadWave("blitzende.wav",	SOUND_BLITZENDE, false);
    SoundManager.LoadWave("blitz.wav",		SOUND_BLITZ, true);
    SoundManager.LoadWave("blitzstart.wav",	SOUND_BLITZSTART_P2, false);
    SoundManager.LoadWave("blitzende.wav",	SOUND_BLITZENDE_P2, false);
    SoundManager.LoadWave("blitz.wav",		SOUND_BLITZ_P2, true);
    SoundManager.LoadWave("powerline.wav",	SOUND_POWERLINE, false);
    SoundManager.LoadWave("landen.wav",		SOUND_LANDEN, false);
    SoundManager.LoadWave("waterin.wav",		SOUND_WATERIN, false);
    SoundManager.LoadWave("waterout.wav",		SOUND_WATEROUT, false);
    SoundManager.LoadWave("dive.wav",			SOUND_DIVE, false);
    SoundManager.LoadWave("feuerfalle.wav",	SOUND_FEUERFALLE, false);
    SoundManager.LoadWave("abzug.wav",		SOUND_ABZUG, false);
    SoundManager.LoadWave("abzug.wav",		SOUND_ABZUG_P2, false);
    SoundManager.LoadWave("funke.wav",		SOUND_FUNKE, false);
    SoundManager.LoadWave("funke2.wav",		SOUND_FUNKE2, false);
    SoundManager.LoadWave("funke3.wav",		SOUND_FUNKE3, false);
    SoundManager.LoadWave("funke4.wav",		SOUND_FUNKE4, false);
    SoundManager.LoadWave("granate.wav",		SOUND_GRANATE, false);
    SoundManager.LoadWave("stonefall.wav",	SOUND_STONEFALL, false);
    SoundManager.LoadWave("stoneexplode.wav",	SOUND_STONEEXPLODE, false);
    SoundManager.LoadWave("rocket.wav",		SOUND_ROCKET, false);
    SoundManager.LoadWave("presse.wav",		SOUND_PRESSE, false);
    SoundManager.LoadWave("ammo.wav",			SOUND_AMMO, false);
    SoundManager.LoadWave("kotzen.wav",		SOUND_KOTZEN, false);
    SoundManager.LoadWave("made.wav",			SOUND_MADE, false);
    SoundManager.LoadWave("droneshot.wav",	SOUND_DRONE, false);
    SoundManager.LoadWave("waterdrop.wav",	SOUND_DROP, false);
    SoundManager.LoadWave("thunder.wav",		SOUND_THUNDER, false);
    SoundManager.LoadWave("upgrade.wav",		SOUND_UPGRADE, false);
    SoundManager.LoadWave("column.wav",		SOUND_COLUMN, false);
    SoundManager.LoadWave("door.wav",			SOUND_DOOR, true);
    SoundManager.LoadWave("doorstop.wav",		SOUND_DOORSTOP, false);
    SoundManager.LoadWave("switch.wav",		SOUND_SWITCH, false);
    SoundManager.LoadWave("schleim.wav",		SOUND_SCHLEIM, false);
    SoundManager.LoadWave("message.wav",		SOUND_MESSAGE, false);
    SoundManager.LoadWave("beamload.wav",		SOUND_BEAMLOAD, true);
    SoundManager.LoadWave("beamload2.wav",	SOUND_BEAMLOAD2, true);
    SoundManager.LoadWave("beamload.wav",		SOUND_BEAMLOAD_P2, true);
    SoundManager.LoadWave("beamload2.wav",	SOUND_BEAMLOAD2_P2, true);

    //DKS - This was commented out in original code, but I've added support for
    //      Trigger_Stampfstein.cpp's chain retraction sound back in:
	SoundManager.LoadWave("chain.wav",		SOUND_CHAIN, true);

    SoundManager.LoadWave("mushroomjump.wav",	SOUND_MUSHROOMJUMP, false);
    SoundManager.LoadWave("golemload.wav",	SOUND_GOLEMLOAD, false);
    SoundManager.LoadWave("golemshot.wav",	SOUND_GOLEMSHOT, false);
    SoundManager.LoadWave("dampf.wav",		SOUND_STEAM, false);
    SoundManager.LoadWave("dampf2.wav",		SOUND_STEAM2, false);
    SoundManager.LoadWave("hit2.wav",		    SOUND_HIT, false);
    SoundManager.LoadWave("hit3.wav",		    SOUND_HIT2, false);
    SoundManager.LoadWave("spiderlila.wav",	SOUND_LILA, false);
    SoundManager.LoadWave("fireball.wav",		SOUND_FIREBALL, false);
    SoundManager.LoadWave("takeoff.wav",		SOUND_TAKEOFF, false);
    SoundManager.LoadWave("laugh.wav",		SOUND_LAUGH, false);
    SoundManager.LoadWave("standup.wav",		SOUND_STANDUP, false);
    SoundManager.LoadWave("gatling.wav",		SOUND_GATLING, false);
    SoundManager.LoadWave("glassbreak.wav",	SOUND_GLASSBREAK, false);
    SoundManager.LoadWave("mutant.wav",		SOUND_MUTANT, false);
    SoundManager.LoadWave("heart1.wav",		SOUND_HEART1, false);
    SoundManager.LoadWave("heart2.wav",		SOUND_HEART2, false);
    SoundManager.LoadWave("secret.wav",		SOUND_SECRET, false);
    SoundManager.LoadWave("mario.wav",		SOUND_MARIO, false);
    SoundManager.LoadWave("flamethrower.wav",	SOUND_FLAMETHROWER, true);
    SoundManager.LoadWave("flamethrower.wav",	SOUND_FLAMETHROWER2, true);

    // Sound Trigger
    SoundManager.LoadWave("ambient_wasserfall.wav",	SOUND_WASSERFALL, true);
    SoundManager.LoadWave("ambient_wind.wav",			SOUND_WIND, true);

    // Voices laden
    SoundManager.LoadWave("v_spread.wav",		SOUND_VOICE_SPREAD, false);
    SoundManager.LoadWave("v_laser.wav",		SOUND_VOICE_LASER, false);
    SoundManager.LoadWave("v_bounce.wav",		SOUND_VOICE_BOUNCE, false);
    SoundManager.LoadWave("v_lightning.wav",	SOUND_VOICE_LIGHTNING, false);
    SoundManager.LoadWave("v_shield.wav",		SOUND_VOICE_SHIELD, false);
    SoundManager.LoadWave("v_powerup.wav",	SOUND_VOICE_POWERUP, false);
    SoundManager.LoadWave("v_wheel.wav",		SOUND_VOICE_WHEELPOWER, false);
    SoundManager.LoadWave("v_grenade.wav",	SOUND_VOICE_GRENADE, false);
    SoundManager.LoadWave("v_powerline.wav",	SOUND_VOICE_POWERLINE, false);
    SoundManager.LoadWave("v_smartbomb.wav",	SOUND_VOICE_SMARTBOMB, false);
    SoundManager.LoadWave("v_rapidfire.wav",	SOUND_VOICE_RAPIDFIRE, false);
    SoundManager.LoadWave("v_supershot.wav",	SOUND_VOICE_SUPERSHOT, false);
    SoundManager.LoadWave("v_extralife.wav",	SOUND_VOICE_EXTRALIFE, false);

    // Endgegner Sounds
    SoundManager.LoadWave("pharaoramm.wav",	SOUND_PHARAORAMM, false);
    SoundManager.LoadWave("pharaodie.wav",	SOUND_PHARAODIE, false);
    SoundManager.LoadWave("spiderscream.wav",	SOUND_SPIDERSCREAM, false);
    SoundManager.LoadWave("spiderwalk.wav",	SOUND_SPIDERWALK, false);
    SoundManager.LoadWave("spiderlaser.wav",	SOUND_SPIDERLASER, false);
    SoundManager.LoadWave("spidergrenade.wav",SOUND_SPIDERGRENADE, false);
    SoundManager.LoadWave("blitzhit.wav",     SOUND_BLITZHIT, false);
    SoundManager.LoadWave("blitzhit2.wav",    SOUND_BLITZHIT2, false);
    SoundManager.LoadWave("bratlaser.wav",    SOUND_BRATLASER, false);
    SoundManager.LoadWave("metal.wav",	    SOUND_KLONG, false);

    // restliche musiken laden
    //DKS - Flugsack song is now loaded on-demand in Gegner_Helper.cpp:
    //SoundManager.LoadSong("flugsack.it",	MUSIC_FLUGSACK);

    SoundManager.LoadSong("credits.it",	MUSIC_CREDITS);

    //DKS - New parameter specifies whether to loop, and stage-clear music shouldn't:
    SoundManager.LoadSong("stageclear.it",MUSIC_STAGECLEAR, false);

    //DKS - New parameter specifies whether to loop, and game-over music shouldn't:
    SoundManager.LoadSong("gameover.it",	MUSIC_GAMEOVER, false);

    SoundManager.LoadSong("highscore.it",	MUSIC_HIGHSCORE);

    //DKS - Punisher music is now loaded on-demand in Gegner_Punisher.cpp
    //SoundManager.LoadSong("Punisher.it", MUSIC_PUNISHER);

    if (!GameRunning)
        return false;

    //DKS - I made a separate set of spritesheets with blue coloring for Player 2, so this
    //      is no longer necessary and has been disabled:
    //CreatePlayer2Texture();

    // Konsole initialisieren
    pConsole = new ConsoleClass();

    //DKS - renamed:
    //SoundManager.ResetAllSongVolumes();
    SoundManager.ResetAllSoundVolumes();

    return true;
}

// --------------------------------------------------------------------------------------
// GameExit, de-initialisieren der DX Objekte, Sprites etc.
// --------------------------------------------------------------------------------------

bool GameExit(void)
{
    Protokoll.WriteText( false, "\n>--------------------<\n" );
    Protokoll.WriteText( false,   "| GameExit started   |\n" );
    Protokoll.WriteText( false,   ">--------------------<\n\n" );

    // GUI freigeben
    delete(pGUI);

    // Sprites freigeben
    delete(pDefaultFont);
    delete(pMenuFont);
    Protokoll.WriteText( false, "-> Fonts released\n" );

    // Console beenden
    delete (pConsole);

    // Menu beenden
    delete(pMenu);
    Protokoll.WriteText( false, "-> Head menu released\n" );

    // HUD freigeben
    delete(pHUD);
    Protokoll.WriteText( false, "-> HUD released\n" );

    // GegnerListe beenden
    delete(pGegner);
    Protokoll.WriteText( false, "-> Enemy List released\n" );

    // Partikelsystem beenden
    delete(pPartikelSystem);
    Protokoll.WriteText( false, "-> Particle System released\n" );

    //DKS - Sound manager is now a static global, and we use new Exit() method:
    SoundManager.Exit();
    Protokoll.WriteText( false, "-> Sound system released\n" );

    //DKS - Free any straggling textures in VRAM before closing down graphics:
    //      NOTE: this is important! Global systems that contain their own
    //      sprites might get destructed after graphics has been shut down.
    //      In the original code, many systems like TileEngineClass were accessed
    //      through global pointers to dynamically allocated classes, in the
    //      interest of speed and code-compactness, if not clarity.
    //      I changed them to be globally allocated static vars. Since some contain
    //      sprite variables, their destructors would then possible end up freeing
    //      textures after the graphics system already closed. Textures::Exit()
    //      below will prevent that.
    Textures.Exit();
    Protokoll.WriteText( false, "-> Texture system released\n" );

    DirectInput.Exit();					// DirectInput beenden

    DirectGraphics.Exit();				// Direct3D    beenden

    //PrintStatus();

    return true;
}

// --------------------------------------------------------------------------------------
// Heartbeat, der Mainloop. der bei jedem Frame durchlaufen wird
// --------------------------------------------------------------------------------------

bool Heartbeat(void)
{
#if defined(PLATFORM_DIRECTX)
    // Test cooperative level
    HRESULT hr;

    // testen, ob das Device noch da ist
    hr = lpD3DDevice->TestCooperativeLevel();

    // device lost?
    if (hr == D3DERR_DEVICELOST)
    {
        // 500 milliseconds
        Sleep (500);
        return false;
    }
    else
        // Bereit für Reset des Devices nachdem wir wieder Focus haben?
        if (hr == D3DERR_DEVICENOTRESET)
        {
            SafeRelease (lpBackbuffer);

            // device neu initialisieren
            hr = lpD3DDevice->Reset(&DirectGraphics.d3dpp);

            if (hr == D3D_OK)
            {
                DirectGraphics.SetDeviceInfo();
            }
            else
            {
                DXTRACE_ERR("lpD3DDevice->Reset", hr);
                return false;
            }
        }

    lpD3DDevice->BeginScene();					// Mit dem Darstellen beginnen
#endif

    switch (SpielZustand)
    {

    // Cracktro
    case CRACKTRO :
    {
#ifdef SHOW_CRACKTRO

        Cracktro->Run();

        if (Cracktro->b_running == false)
        {
            delete (Cracktro);
            SpielZustand = MAINMENU;

            if (!GameInit2())
                return false;
        }
#else
        if (!GameInit2())
            return false;

        SpielZustand = MAINMENU;

#endif
//		pOuttro = new OuttroClass();
//		SpielZustand = OUTTRO;


        goto jump;

    }
    break;

    //----- Intro anzeigen ?
    case INTRO :
    {
        // Laufen lassen, bis beendet
        if (pIntro->Zustand != INTRO_DONE)
        {
            pIntro->DoIntro();

            if (DirectInput.AnyKeyDown() ||
                    DirectInput.AnyButtonDown())
                pIntro->EndIntro();
        }
        else
        {
            SoundManager.StopSong(MUSIC_INTRO, false);
            delete(pIntro);
            InitNewGame();
            InitNewGameLevel(1);
            SpielZustand = GAMELOOP;
        }
    }
    break;

    //----- Outtro anzeigen ?
    case OUTTRO :
    {
        pOuttro->DoOuttro();

        if(KeyDown(DIK_ESCAPE))			// Intro beenden ?
        {
            SoundManager.StopSong(MUSIC_OUTTRO, false);
            delete(pOuttro);
            Stage = MAX_LEVELS;
            pMenu->CheckForNewHighscore();
        }
    }
    break;

    //----- Hauptmenu
    case MAINMENU :
    {
        pMenu->DoMenu();
    }
    break;

    //---- Haupt-Gameloop
    case GAMELOOP :
    {
        GameLoop();
    }
    break;

    default :
        break;
    }

#ifdef _DEBUG

    // Debugmode ?
    if (DebugMode == true)
        ShowDebugInfo();

    // Debug-Mode ein/ausschalten
    if (KeyDown(DIK_F10))
    {
        if (DebugMode == true)
            DebugMode = false;
        else
            DebugMode = true;
        while(KeyDown(DIK_F10))
            DirectInput.UpdateTastatur();
    }

#endif

    if (CommandLineParams.ShowFPS)
        ShowFPS();

    // GUI abhandeln
    pGUI->Run();

    // Konsole abhandeln
    pConsole->DoConsole();

jump:

    DirectGraphics.DisplayBuffer ();

    // Screenshot machen
#ifdef _DEBUG
    if(KeyDown(DIK_F12))
        DirectGraphics.TakeScreenshot("HurricanShot", 640, 480);

    // Screenshot machen
    if(KeyDown(DIK_F9))
        pGUI->HideBox();
#endif
    return true;
}

// --------------------------------------------------------------------------------------
// So Firlefanz wie FPS usw anzeigen
// --------------------------------------------------------------------------------------

#ifdef _DEBUG
void ShowDebugInfo(void)
{
    // Blaues durchsichtiges Rechteck zeichnen
    RenderRect(0, 0, 320, 240, 0xA00000FF);
    pDefaultFont->ShowFPS();								// FPS anzeigen

    // Anzahl der aktuell aktiven Partikel anzeigen
    _itoa_s(pPartikelSystem->GetNumPartikel(), StringBuffer, 10);
    pDefaultFont->DrawText(  0, 60, "Partikel :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 60, StringBuffer, 0xFFFFFFFF);

    // Anzahl der aktuell aktiven Schüsse anzeigen
    _itoa_s(Projectiles.GetNumProjectiles(), StringBuffer, 10);
    pDefaultFont->DrawText(200, 60, "Projektile :", 0xFFFFFFFF);
    pDefaultFont->DrawText(300, 60, StringBuffer, 0xFFFFFFFF);

    // Benutzte Sound-Channels angeben
    _itoa_s(SoundManager.most_channels_used, StringBuffer, 10);
    pDefaultFont->DrawText(  0, 75, "MaxChannels :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 75, StringBuffer, 0xFFFFFFFF);

    // Anzahl der Gegner im Level angeben
    _itoa_s(pGegner->GetNumGegner(), StringBuffer, 10);
    pDefaultFont->DrawText(200, 75, "Gegneranzahl :", 0xFFFFFFFF);
    pDefaultFont->DrawText(300, 75, StringBuffer, 0xFFFFFFFF);

    // MoveSpeed anzeigen
    _itoa_s((int)(Timer.GetMoveSpeed()), StringBuffer, 10);
    pDefaultFont->DrawText(  0, 90, "Move Speed :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 90, StringBuffer, 0xFFFFFFFF);

    // Blitzwinkel angeben
    //_itoa_s((int)(Player->BlitzWinkel), StringBuffer, 10);
    pDefaultFont->DrawText(  0, 135, "Blitzwinkel :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 135, StringBuffer, 0xFFFFFFFF);

    // Blitzwinkel angeben
    sprintf_s (StringBuffer, "%f", Timer.SpeedFaktor);
    pDefaultFont->DrawText(  0, 150, "Speed :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 150, StringBuffer, 0xFFFFFFFF);

    // Blitzwinkel angeben
    //sprintf_s (StringBuffer, "%f", Player->JumpySave - Player->ypos);
    pDefaultFont->DrawText(  0, 250, "yDiff :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 250, StringBuffer, 0xFFFFFFFF);

    // Blitzwinkel angeben
    //sprintf_s (StringBuffer, "%f", Player->JumpAdd);
    pDefaultFont->DrawText(  0, 270, "yAdd :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 270, StringBuffer, 0xFFFFFFFF);

    /*	for (int i=0; i<128; i++)
    		for (int j=0; j<96; j++)
    			if(TileEngineTiles[i][j].BackArt > 0)
    				pDefaultFont->DrawText(300+i, 100+j, ".", 0xFFFFFF00);*/
}
#endif //_DEBUG

//DKS - added FPS reporting via command switch
void ShowFPS()
{
    const unsigned int fps_update_freq_in_ticks = 500;
    static unsigned int ticks_fps_last_updated = 0;
    static int frame_ctr = 0;
    static char char_buf[24] = "";

    frame_ctr++;
    unsigned int cur_ticks = timeGetTime();
    unsigned int ticks_elapsed = cur_ticks - ticks_fps_last_updated;
    if (ticks_elapsed > fps_update_freq_in_ticks && frame_ctr > 0) {
        float avg_fps = (float)frame_ctr * 
            (1000.0f / (float)fps_update_freq_in_ticks) *
            ((float)fps_update_freq_in_ticks / (float)ticks_elapsed);
        sprintf_s(char_buf, "FPS: %.1f", avg_fps );
        fprintf_s(stdout, char_buf);
        fprintf_s(stdout, "\n");
        frame_ctr = 0;
        ticks_fps_last_updated = cur_ticks;
    }
    pMenuFont->DrawText(  0, 0, char_buf, 0xFFFFFFFF);
}

//----------------------------------------------------------------------------
// Outtro starten
//----------------------------------------------------------------------------

void StartOuttro(void)
{
    Stage = -1;
    pOuttro = new OuttroClass();
    SpielZustand = OUTTRO;
}

//----------------------------------------------------------------------------
// Intro starten
//----------------------------------------------------------------------------

void StartIntro(void)
{
    pIntro = new IntroClass();
    SpielZustand = INTRO;
}
