// Datei : Main.cpp

// --------------------------------------------------------------------------------------
//
// Hurrican
//
// Shoot em up ala Turrican
// benutzt die DirectX8.1 API f�r Grafik, Sound und Input
//
// (c) 2002 J�rg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#ifndef _DEBUG
#define SHOW_CRACKTRO
#endif

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

#include "Main.h"
#include "Texts.h"
#include "Console.h"
#include "CCracktro.h"
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
D3DCAPS8				d3dcaps;						// M�glichkeiten der Hardware
LPDIRECT3D8				lpD3D			= NULL;			// Direct3D Hauptobjekt
LPDIRECT3DDEVICE8		lpD3DDevice		= NULL;			// Direct3D Device-Objekt
LPDIRECT3DSURFACE8		lpBackbuffer	= NULL;			// Der Backbuffer
HWND					DesktopHWND		= NULL;			// Handle des Desktops
HWND					g_hwnd			= NULL;			// Handle des Hauptfensters
HINSTANCE				g_hinst;						// hinstance des Hauptfensters
#endif

bool					FixedFramerate		= false;		// true = Spiel mit 50 Frames laufen lassen
// false = Spiel so fl�ssig wie m�glich laufen lassen
bool					Sprache;						// true == deutsch / false == englisch
bool					GameRunning			= true;		// Spiel l�uft :-)
bool					GamePaused			= false;	// Spiel eingefroren (wenn man zb das Fenster verl�sst)
bool					NochKeinFullScreen	= true;		// Logo noch anzeigen in Paint ?
bool					DebugMode			= false;	// Debug Mode ein/aus
float					SpeedFaktor = 1.0f;				// Faktor, mit dem alle Bewegungen verrechnet werden
DirectGraphicsClass		DirectGraphics;					// Grafik-Objekt
DirectInputClass		DirectInput;					// Input-Objekt
TimerClass				*pTimer;						// Timer Klasse f�r die Framerate
#if defined(__AROS__)
Logdatei				Protokoll("T:Game_Log.txt");		// Protokoll Datei
#else
Logdatei				Protokoll("Game_Log.txt");		// Protokoll Datei
#endif
CSoundManager			*pSoundManager =  NULL;			// Sound Manager
DirectGraphicsFont		*pDefaultFont = new(DirectGraphicsFont);
DirectGraphicsFont		*pMenuFont	  = new(DirectGraphicsFont);
TileEngineClass			*pTileEngine;					// Tile Engine
PartikelsystemClass		*pPartikelSystem;				// Das coole Partikelsystem
ProjectileListClass		*pProjectiles;					// Liste mit Sch�ssen
GegnerListClass			*pGegner;						// Liste mit Gegner
IntroClass				*pIntro;						// Intro-Objekt
OuttroClass				*pOuttro;						// Outtro-Objekt
MenuClass				*pMenu = NULL;					// Hauptmenu-Objekt
ConsoleClass			*pConsole;						// Konsolen-Objekt
CGUISystem				*pGUI;							// GUI System
CCracktro				*Cracktro;
RECT					srcrect, destrect;

DirectGraphicsSprite	LoadingScreen;					// Splash Screen im Fullscreen
DirectGraphicsSprite	LoadingBar;						// Fortschrittsbalken
float					LoadingProgress;				// Fortschritt des Loading Balkens
float					LoadingItemsToLoad;				// Anzahl zu ladender Elemente
int						LoadingItemsLoaded;				// Anzahl geladener Elemente

int						LanguageFileCount;				// Anzahl gefundener Language Files
char					LanguageFiles[MAX_LANGUAGE_FILES][MAX_LANGUAGE_FILENAME_LENGTH]; 
char					ActualLanguage[256];			// Gew�hlte Language

sCommandLineParams		CommandLineParams;

int WINDOWWIDTH;
int WINDOWHEIGHT;


// --------------------------------------------------------------------------------------
// Variablen f�r den Spielablauf
// --------------------------------------------------------------------------------------

#if defined(PLATFORM_DIRECTX)
HBITMAP					SplashScreen = NULL;			// SplashScreen Grafik
#endif
PlayerClass				*pPlayer[2];					// Werte der Spieler
HUDClass				*pHUD;							// Das HUD
unsigned char			SpielZustand = CRACKTRO;		// Aktueller Zustand des Spieles
char					StringBuffer[100];				// F�r die Int / String Umwandlung

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
        return(0);									// Success zur�ckliefern
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
            if (pSoundManager != NULL)
            {
                pSoundManager->PausePlaying();
                pSoundManager->StopAllSounds();
            }

            GamePaused = true;
        }
        else
        {
            if (pSoundManager != NULL)
                pSoundManager->PlayPaused();

            GamePaused = false;
        }

    }
    break;

    default :
        break;
    }

    //unbearbeitete Nachrichten zur�ckliefern
    return(DefWindowProc(hwnd, message, wparam, lparam));
}
#endif

// --------------------------------------------------------------------------------------
// File Exists Funktion
// --------------------------------------------------------------------------------------

bool FileExists(char Filename[256])
{
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

#if defined(ANDROID)
    g_storage_ext = SDL_AndroidGetExternalStoragePath();
#else
    g_storage_ext = ".";
#endif
    Protokoll.WriteText( false, "\n--> Using external storage path '%s' <--\n", g_storage_ext );

    GamePaused = false;

#if defined(PLATFORM_DIRECTX)
    WNDCLASSEX			winclass;							// eigene Windows-Klasse
    MSG					message;							// Message
    RECT				rect;								// Gr�sse des Desktops

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

#if defined(PLATFORM_DIRECTX)
    // Desktop Window holen und Gr�sse auslesen (damit wir unser Fenster in der Mitte des Screens
    // positionnieren k�nnen)
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

    // Werte f�r die Windows-Klasse festlegen
    winclass.cbSize			= sizeof (WNDCLASSEX);					// Gr�sse der Klasse
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
                                  WINDOWHEIGHT,						// Fensterh�he
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
            // und l�schen
            TranslateMessage(&message);							// Nachricht �beretzen
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
        try
        {
            if (GamePaused == false)
            {
                // Main Loop
                Heartbeat();

                // Eingabeger�te updaten
                DirectInput.UpdateTastatur  ();
                DirectInput.UpdateJoysticks ();
                //DirectInput.UpdateMaus(false);

                // Soundchannels updaten
                pSoundManager->Update();

                // Timer updaten
                pTimer->update();

                // Feste Framerate ? (Spiel l�uft in Zeitlupe, wenn zu langsam)
                //
                if (FixedFramerate == true)
                {
                    pTimer->SetMaxFPS (60);
                    SpeedFaktor = 1.0f / 60.0f * pTimer->GetMoveSpeed();
                }
                else
                {
                    //pTimer->SetMaxFPS (0);
                    SpeedFaktor = pTimer->SpeedFaktor;
                }

                pTimer->wait();

                // Bei Demo immer gleichen Speedfaktor setzen
                //
                if (DEMORecording == true ||
                        DEMOPlaying	  == true)
                    SpeedFaktor = 0.28f;
            }
        }

        catch(const char *str)
        {
            Protokoll.WriteText( false, "Failure! Unhandled exception\n %s", str );
            GameRunning = false;
        }
    }

//----- Spiel verlassen

    //pTimer->WriteLogValues();

    if(!GameExit())
        Protokoll.WriteText( true, "-> GameExit Fehler !\n" );

    Protokoll.WriteText( false, "\n-> Hurrican closed !\n" );
    Protokoll.WriteText( false, "\nhttp://www.poke53280.de\n" );
    Protokoll.WriteText( false, "Bugreports, questions etc : information@poke53280.de\n" );
    Protokoll.WriteText( false, "\n-> logfile end" );

    // Kein Fehler im Game? Dann Logfile l�schen
    if (Protokoll.delLogFile == true)
        DeleteFile("Game_Log.txt");

#if defined(PLATFORM_DIRECTX)
    return(message.wParam);										// R�ckkehr zu Windows
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

#if defined(PLATFORM_DIRECTX)
    // Language Files
    HWND	ComboBoxLanguageFiles	= NULL;
    ComboBoxLanguageFiles = CreateWindow("COMBOBOX",
                                         "",
                                         WS_CHILD,
                                         0, 0, 0, 0, g_hwnd, 0, g_hinst, NULL);

    //DKS - Moved this outside the DirectX #ifdef
//    srand(GetTickCount());

    // Loadinfo Text festlegen
//	DisplayHintNr = rand()%30;

    // *.lng Files anf�gen
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

    // Timer initialisieren
    pTimer = new TimerClass();

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

#if defined(ANDROID)
    DirectInput.InitTouchBoxes( DirectGraphics.WindowView.w, DirectGraphics.WindowView.h );
#endif

    // Sound Manager initialisieren
    pSoundManager = new CSoundManager();

    // Splash-Screen nicht mehr anzeigen
    NochKeinFullScreen = false;

#ifdef SHOW_CRACKTRO
    Cracktro = new CCracktro();
    SpielZustand = CRACKTRO;
#endif

    return true;
}

// --------------------------------------------------------------------------------------
// Textur von Spieler 2 anpassen
// --------------------------------------------------------------------------------------

void ConvertPlayerTexture(DirectGraphicsSprite *pTexture)
{
    // PICKLE TODO
#if defined(PLATFORM_DIRECTX)
    HRESULT hr;
    D3DLOCKED_RECT pLockedRect;
    int width, height, r, g, b, a, col, temp;
    BYTE  *pRow;
    DWORD *pPixel;

    // Textur locken
    hr = pTexture->itsTexture->LockRect(0, &pLockedRect, 0, 0);
    if (hr != D3D_OK)
        return;

    // Breite, H�he und Pitch setzen
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
    pTexture->itsTexture->UnlockRect(0);
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

// --------------------------------------------------------------------------------------
// GameInit2, initialisiert den Rest nach dem Cracktro
// --------------------------------------------------------------------------------------

bool GameInit2(void)
{
    // Player initialisieren
    pPlayer[0] = new PlayerClass();
    pPlayer[1] = new PlayerClass();
    pPlayer[0]->SoundOff = 0;
    pPlayer[1]->SoundOff = 1;
    memset(pPlayer[0], 0, sizeof(*pPlayer[0]));
    memset(pPlayer[1], 0, sizeof(*pPlayer[1]));

    // Konfiguration laden
    if (LoadConfig() == false)
    {
        Protokoll.WriteText( false, "\n-> No config found. Creating default\n" );
        CreateDefaultConfig ();
    }

    // Menumusik laden und spielen
    pSoundManager->LoadSong("menu.it",	MUSIC_MENU);
    pSoundManager->SetAllSongVolumes();
    pSoundManager->PlaySong(MUSIC_MENU, false);

    // Menu initialisieren
    pMenuFont->LoadFont("menufont.png", 448, 256, 28, 28, 16, 7);
    pMenu = new MenuClass();


    // Fonts laden
    pDefaultFont->LoadFont  ("smallfont.bmp", 320, 84, 10, 12, 32, 7);

    //DKS - Added support for font scaling
    if (CommandLineParams.LowRes) {
        pDefaultFont->SetScaleFactor(2);    // On lower res, draw smallest font twice as large so it appears 1:1
    }

    LoadingScreen.LoadImage("loading.bmp",    360, 60, 360, 60, 1, 1);
    LoadingBar.LoadImage   ("loadingbar.bmp", 318, 19, 318, 19, 1, 1);
    LoadingProgress = 0.0f;
    LoadingItemsToLoad = 345;
    LoadingItemsLoaded = 0;

    // GUISystem initialiseren
    pGUI = new CGUISystem();
    pGUI->InitGUISystem();

    // Partikelsystem initialisieren
    pPartikelSystem = new(PartikelsystemClass);

    // ProjectileListe initialisieren
    pProjectiles = new(ProjectileListClass);

    // GegnerListe initialisieren
    pGegner = new(GegnerListClass);

    // Tileengine initialisieren
    pTileEngine = new(TileEngineClass);
    pTileEngine->SetScrollSpeed(1.0f, 0.0f);

    // HUD initialisieren
    pHUD = new HUDClass();

    InitReplacers();

    // Sounds laden
    pSoundManager->LoadWave("spreadshot.wav",   SOUND_SPREADSHOT, false);
    pSoundManager->LoadWave("lasershot.wav",    SOUND_LASERSHOT, false);
    pSoundManager->LoadWave("bounceshot.wav",   SOUND_BOUNCESHOT, false);
    pSoundManager->LoadWave("explosion1.wav",   SOUND_EXPLOSION1, false);
    pSoundManager->LoadWave("explosion2.wav",   SOUND_EXPLOSION2, false);
    pSoundManager->LoadWave("explosion3.wav",   SOUND_EXPLOSION3, false);
    pSoundManager->LoadWave("explosion4.wav",   SOUND_EXPLOSION4, false);
    pSoundManager->LoadWave("walkergiggle.wav", SOUND_WALKERGIGGLE, false);
    pSoundManager->LoadWave("collect.wav",		SOUND_COLLECT, false);
    pSoundManager->LoadWave("hit.wav",		    SOUND_SPREADHIT, false);
    pSoundManager->LoadWave("canon.wav",		SOUND_CANON, false);
    pSoundManager->LoadWave("click.wav",		SOUND_CLICK, false);
    pSoundManager->LoadWave("blitzstart.wav",	SOUND_BLITZSTART, false);
    pSoundManager->LoadWave("blitzende.wav",	SOUND_BLITZENDE, false);
    pSoundManager->LoadWave("blitz.wav",		SOUND_BLITZ, true);
    pSoundManager->LoadWave("blitzstart.wav",	SOUND_BLITZSTART_P2, false);
    pSoundManager->LoadWave("blitzende.wav",	SOUND_BLITZENDE_P2, false);
    pSoundManager->LoadWave("blitz.wav",		SOUND_BLITZ_P2, true);
    pSoundManager->LoadWave("powerline.wav",	SOUND_POWERLINE, false);
    pSoundManager->LoadWave("landen.wav",		SOUND_LANDEN, false);
    pSoundManager->LoadWave("waterin.wav",		SOUND_WATERIN, false);
    pSoundManager->LoadWave("waterout.wav",		SOUND_WATEROUT, false);
    pSoundManager->LoadWave("dive.wav",			SOUND_DIVE, false);
    pSoundManager->LoadWave("feuerfalle.wav",	SOUND_FEUERFALLE, false);
    pSoundManager->LoadWave("abzug.wav",		SOUND_ABZUG, false);
    pSoundManager->LoadWave("abzug.wav",		SOUND_ABZUG_P2, false);
    pSoundManager->LoadWave("funke.wav",		SOUND_FUNKE, false);
    pSoundManager->LoadWave("funke2.wav",		SOUND_FUNKE2, false);
    pSoundManager->LoadWave("funke3.wav",		SOUND_FUNKE3, false);
    pSoundManager->LoadWave("funke4.wav",		SOUND_FUNKE4, false);
    pSoundManager->LoadWave("granate.wav",		SOUND_GRANATE, false);
    pSoundManager->LoadWave("stonefall.wav",	SOUND_STONEFALL, false);
    pSoundManager->LoadWave("stoneexplode.wav",	SOUND_STONEEXPLODE, false);
    pSoundManager->LoadWave("rocket.wav",		SOUND_ROCKET, false);
    pSoundManager->LoadWave("presse.wav",		SOUND_PRESSE, false);
    pSoundManager->LoadWave("ammo.wav",			SOUND_AMMO, false);
    pSoundManager->LoadWave("kotzen.wav",		SOUND_KOTZEN, false);
    pSoundManager->LoadWave("made.wav",			SOUND_MADE, false);
    pSoundManager->LoadWave("droneshot.wav",	SOUND_DRONE, false);
    pSoundManager->LoadWave("waterdrop.wav",	SOUND_DROP, false);
    pSoundManager->LoadWave("thunder.wav",		SOUND_THUNDER, false);
    pSoundManager->LoadWave("upgrade.wav",		SOUND_UPGRADE, false);
    pSoundManager->LoadWave("column.wav",		SOUND_COLUMN, false);
    pSoundManager->LoadWave("door.wav",			SOUND_DOOR, true);
    pSoundManager->LoadWave("doorstop.wav",		SOUND_DOORSTOP, false);
    pSoundManager->LoadWave("switch.wav",		SOUND_SWITCH, false);
    pSoundManager->LoadWave("schleim.wav",		SOUND_SCHLEIM, false);
    pSoundManager->LoadWave("message.wav",		SOUND_MESSAGE, false);
    pSoundManager->LoadWave("beamload.wav",		SOUND_BEAMLOAD, true);
    pSoundManager->LoadWave("beamload2.wav",	SOUND_BEAMLOAD2, true);
    pSoundManager->LoadWave("beamload.wav",		SOUND_BEAMLOAD_P2, true);
    pSoundManager->LoadWave("beamload2.wav",	SOUND_BEAMLOAD2_P2, true);
//	pSoundManager->LoadWave("chain.wav",		SOUND_CHAIN, true);
    pSoundManager->LoadWave("mushroomjump.wav",	SOUND_MUSHROOMJUMP, false);
    pSoundManager->LoadWave("golemload.wav",	SOUND_GOLEMLOAD, false);
    pSoundManager->LoadWave("golemshot.wav",	SOUND_GOLEMSHOT, false);
    pSoundManager->LoadWave("dampf.wav",		SOUND_STEAM, false);
    pSoundManager->LoadWave("dampf2.wav",		SOUND_STEAM2, false);
    pSoundManager->LoadWave("hit2.wav",		    SOUND_HIT, false);
    pSoundManager->LoadWave("hit3.wav",		    SOUND_HIT2, false);
    pSoundManager->LoadWave("spiderlila.wav",	SOUND_LILA, false);
    pSoundManager->LoadWave("fireball.wav",		SOUND_FIREBALL, false);
    pSoundManager->LoadWave("takeoff.wav",		SOUND_TAKEOFF, false);
    pSoundManager->LoadWave("laugh.wav",		SOUND_LAUGH, false);
    pSoundManager->LoadWave("standup.wav",		SOUND_STANDUP, false);
    pSoundManager->LoadWave("gatling.wav",		SOUND_GATLING, false);
    pSoundManager->LoadWave("glassbreak.wav",	SOUND_GLASSBREAK, false);
    pSoundManager->LoadWave("mutant.wav",		SOUND_MUTANT, false);
    pSoundManager->LoadWave("heart1.wav",		SOUND_HEART1, false);
    pSoundManager->LoadWave("heart2.wav",		SOUND_HEART2, false);
    pSoundManager->LoadWave("secret.wav",		SOUND_SECRET, false);
    pSoundManager->LoadWave("mario.wav",		SOUND_MARIO, false);
    pSoundManager->LoadWave("flamethrower.wav",	SOUND_FLAMETHROWER, true);
    pSoundManager->LoadWave("flamethrower.wav",	SOUND_FLAMETHROWER2, true);

    // Sound Trigger
    pSoundManager->LoadWave("ambient_wasserfall.wav",	SOUND_WASSERFALL, true);
    pSoundManager->LoadWave("ambient_wind.wav",			SOUND_WIND, true);

    // Voices laden
    pSoundManager->LoadWave("v_spread.wav",		SOUND_VOICE_SPREAD, false);
    pSoundManager->LoadWave("v_laser.wav",		SOUND_VOICE_LASER, false);
    pSoundManager->LoadWave("v_bounce.wav",		SOUND_VOICE_BOUNCE, false);
    pSoundManager->LoadWave("v_lightning.wav",	SOUND_VOICE_LIGHTNING, false);
    pSoundManager->LoadWave("v_shield.wav",		SOUND_VOICE_SHIELD, false);
    pSoundManager->LoadWave("v_powerup.wav",	SOUND_VOICE_POWERUP, false);
    pSoundManager->LoadWave("v_wheel.wav",		SOUND_VOICE_WHEELPOWER, false);
    pSoundManager->LoadWave("v_grenade.wav",	SOUND_VOICE_GRENADE, false);
    pSoundManager->LoadWave("v_powerline.wav",	SOUND_VOICE_POWERLINE, false);
    pSoundManager->LoadWave("v_smartbomb.wav",	SOUND_VOICE_SMARTBOMB, false);
    pSoundManager->LoadWave("v_rapidfire.wav",	SOUND_VOICE_RAPIDFIRE, false);
    pSoundManager->LoadWave("v_supershot.wav",	SOUND_VOICE_SUPERSHOT, false);
    pSoundManager->LoadWave("v_extralife.wav",	SOUND_VOICE_EXTRALIFE, false);

    // Endgegner Sounds
    pSoundManager->LoadWave("pharaoramm.wav",	SOUND_PHARAORAMM, false);
    pSoundManager->LoadWave("pharaodie.wav",	SOUND_PHARAODIE, false);
    pSoundManager->LoadWave("spiderscream.wav",	SOUND_SPIDERSCREAM, false);
    pSoundManager->LoadWave("spiderwalk.wav",	SOUND_SPIDERWALK, false);
    pSoundManager->LoadWave("spiderlaser.wav",	SOUND_SPIDERLASER, false);
    pSoundManager->LoadWave("spidergrenade.wav",SOUND_SPIDERGRENADE, false);
    pSoundManager->LoadWave("blitzhit.wav",     SOUND_BLITZHIT, false);
    pSoundManager->LoadWave("blitzhit2.wav",    SOUND_BLITZHIT2, false);
    pSoundManager->LoadWave("bratlaser.wav",    SOUND_BRATLASER, false);
    pSoundManager->LoadWave("metal.wav",	    SOUND_KLONG, false);

    // restliche musiken laden
    pSoundManager->LoadSong("flugsack.it",	MUSIC_FLUGSACK);
    pSoundManager->LoadSong("credits.it",	MUSIC_CREDITS);
    pSoundManager->LoadSong("stageclear.it",MUSIC_STAGECLEAR);
    pSoundManager->LoadSong("gameover.it",	MUSIC_GAMEOVER);
    pSoundManager->LoadSong("highscore.it",	MUSIC_HIGHSCORE);
    pSoundManager->LoadSong("Punisher.it", MUSIC_PUNISHER);

    // Spieler grafiken laden
    SchussFlamme[0].LoadImage("schussflamme.bmp",  76,  72,  38, 24, 2, 3);
    SchussFlamme[1].LoadImage("schussflamme2.bmp", 80,  108,  40, 36, 2, 3);
    SchussFlamme[2].LoadImage("schussflamme3.bmp", 48,  114,  24, 38, 2, 3);

    SchussFlammeFlare.LoadImage("schussflammeflare.bmp", 140,  140,  140, 140, 1, 1);

    for (int p = 0; p < 2; p++)
    {
        // Idle
        PlayerIdle[p].LoadImage("hurri_idle.png",  350, 320, 70, 80, 5, 4);

        // Kucken
        PlayerKucken[p].LoadImage("hurri_kucken.png",  140, 80, 70, 80, 2, 1);

        // Umkucken
        PlayerIdle2[p].LoadImage("hurri_idleumkuck.png",  980,  320, 70, 80, 14, 4);

        // Laufen
        PlayerRun[p].LoadImage("hurri_laufen.png",  350,  320, 70, 80, 5, 4);

        // Diagonal schauen/schiessen
        PlayerDiagonal[p].LoadImage("hurri_shootdiagonal.png",  140,  80, 70, 80, 2, 1);

        // Hoch schauen/schiessen
        PlayerOben[p].LoadImage("hurri_shootup.png",  140,  80, 70, 80, 2, 1);

        // Ducken
        PlayerCrouch[p].LoadImage("hurri_crouch.png",  140,  80, 70, 80, 2, 1);

        // Springen normal, diagonal und nach oben
        PlayerJump[p].LoadImage("hurri_jump.png",  280,  240, 70, 80, 4, 3);
        PlayerJumpDiagonal[p].LoadImage("hurri_jumpschraeg.png",  280,  240, 70, 80, 4, 3);
        PlayerJumpUp[p].LoadImage("hurri_jumpup.png",  280,  240, 70, 80, 4, 3);

        // Rundumschuss
        PlayerBlitz[p].LoadImage("hurri_rundumschuss.png", 490, 480, 70, 80, 7, 6);

        // Pissen
        PlayerPiss[p].LoadImage("hurri_pissen.png", 490, 240, 70, 80, 7, 3);

        // Flugsack
        PlayerRide[p].LoadImage  ("hurri_ride.png",   450, 480, 90,120, 5, 4);

        // Stachelrad
        PlayerRad[p].LoadImage   ("hurri_rad.bmp",    140,  70, 35, 35, 4, 2);
    }

    Blitzstrahl[0].LoadImage("blitzstrahl1.bmp",32,  32, 32, 32, 1, 1);
    Blitzstrahl[1].LoadImage("blitzstrahl2.bmp",32,  32, 32, 32, 1, 1);
    Blitzstrahl[2].LoadImage("blitzstrahl3.bmp",32,  32, 32, 32, 1, 1);
    Blitzstrahl[3].LoadImage("blitzstrahl4.bmp",32,  32, 32, 32, 1, 1);
    Blitzflash[0].LoadImage("blitzflash1.bmp",	66,  64, 66, 64, 1, 1);
    Blitzflash[1].LoadImage("blitzflash2.bmp",	66,  64, 66, 64, 1, 1);
    Blitzflash[2].LoadImage("blitzflash3.bmp",	66,  64, 66, 64, 1, 1);
    Blitzflash[3].LoadImage("blitzflash4.bmp",	66,  64, 66, 64, 1, 1);
    BlitzTexture.LoadImage("blitztexture.bmp", 64, 64, 64, 64, 1, 1);

    if (!GameRunning)
        return false;

    CreatePlayer2Texture();

    // Konsole initialisieren
    pConsole = new ConsoleClass();

    pSoundManager->SetAllSongVolumes();

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

    // Timer freigeben
    delete(pTimer);

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

    // Tileengine beenden
    delete(pTileEngine);
    Protokoll.WriteText( false, "-> TileEngine released\n" );

    // GegnerListe beenden
    delete(pGegner);
    Protokoll.WriteText( false, "-> Enemy List released\n" );

    // Player freigeben
    delete(pPlayer[0]);
    delete(pPlayer[1]);
    Protokoll.WriteText( false, "-> Player released\n" );

    // Partikelsystem beenden
    delete(pPartikelSystem);
    Protokoll.WriteText( false, "-> Particle System released\n" );

    // ProjectileListe beenden
    delete(pProjectiles);
    Protokoll.WriteText( false, "-> Projectile List released\n" );

    delete(pSoundManager);				// DirectSound beenden

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
        // Bereit f�r Reset des Devices nachdem wir wieder Focus haben?
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
            pSoundManager->StopSong(MUSIC_INTRO, false);
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
            delete(pOuttro);
            pSoundManager->StopSong(MUSIC_OUTTRO, false);
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

void ShowDebugInfo(void)
{
    // Blaues durchsichtiges Rechteck zeichnen
    RenderRect(0, 0, 320, 240, 0xA00000FF);
    pDefaultFont->ShowFPS();								// FPS anzeigen

    // Anzahl der aktuell aktiven Partikel anzeigen
    _itoa_s(pPartikelSystem->GetNumPartikel(), StringBuffer, 10);
    pDefaultFont->DrawText(  0, 60, "Partikel :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 60, StringBuffer, 0xFFFFFFFF);

    // Anzahl der aktuell aktiven Sch�sse anzeigen
    _itoa_s(pProjectiles->GetNumProjectiles(), StringBuffer, 10);
    pDefaultFont->DrawText(200, 60, "Projektile :", 0xFFFFFFFF);
    pDefaultFont->DrawText(300, 60, StringBuffer, 0xFFFFFFFF);

    // Benutzte Sound-Channels angeben
    _itoa_s(pSoundManager->ChannelsUsed, StringBuffer, 10);
    pDefaultFont->DrawText(  0, 75, "Sound Channels :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 75, StringBuffer, 0xFFFFFFFF);

    // Anzahl der Gegner im Level angeben
    _itoa_s(pGegner->GetNumGegner(), StringBuffer, 10);
    pDefaultFont->DrawText(200, 75, "Gegneranzahl :", 0xFFFFFFFF);
    pDefaultFont->DrawText(300, 75, StringBuffer, 0xFFFFFFFF);

    // MoveSpeed anzeigen
    _itoa_s((int)(pTimer->GetMoveSpeed()), StringBuffer, 10);
    pDefaultFont->DrawText(  0, 90, "Move Speed :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 90, StringBuffer, 0xFFFFFFFF);

    // Blitzwinkel angeben
    //_itoa_s((int)(pPlayer->BlitzWinkel), StringBuffer, 10);
    pDefaultFont->DrawText(  0, 135, "Blitzwinkel :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 135, StringBuffer, 0xFFFFFFFF);

    // Blitzwinkel angeben
    sprintf_s (StringBuffer, "%f", pTimer->SpeedFaktor);
    pDefaultFont->DrawText(  0, 150, "Speed :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 150, StringBuffer, 0xFFFFFFFF);

    // Blitzwinkel angeben
    //sprintf_s (StringBuffer, "%f", pPlayer->JumpySave - pPlayer->ypos);
    pDefaultFont->DrawText(  0, 250, "yDiff :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 250, StringBuffer, 0xFFFFFFFF);

    // Blitzwinkel angeben
    //sprintf_s (StringBuffer, "%f", pPlayer->JumpAdd);
    pDefaultFont->DrawText(  0, 270, "yAdd :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 270, StringBuffer, 0xFFFFFFFF);

    /*	for (int i=0; i<128; i++)
    		for (int j=0; j<96; j++)
    			if(pTileEngine->Tiles[i][j].BackArt > 0)
    				pDefaultFont->DrawText(300+i, 100+j, ".", 0xFFFFFF00);*/
}

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
