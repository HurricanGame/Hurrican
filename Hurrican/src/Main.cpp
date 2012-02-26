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

#ifndef _DEBUG
#define SHOW_CRACKTRO
#endif

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include <windows.h>									// Alle Windows Header includen
#include <stdio.h>
#include <Dxerr8.h>
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
#include "Resource.h"
#include "Tileengine.h"
#include "Timer.h"
#include "unrarlib.h"

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

D3DFORMAT				D3DFormat;						// Format der Primary Surface
D3DCAPS8				d3dcaps;						// Möglichkeiten der Hardware
LPDIRECT3D8				lpD3D			= NULL;			// Direct3D Hauptobjekt
LPDIRECT3DDEVICE8		lpD3DDevice		= NULL;			// Direct3D Device-Objekt
LPDIRECT3DSURFACE8		lpBackbuffer	= NULL;			// Der Backbuffer
HWND					DesktopHWND		= NULL;			// Handle des Desktops
HWND					g_hwnd			= NULL;			// Handle des Hauptfensters
HINSTANCE				g_hinst;						// hinstance des Hauptfensters

bool					FixedFramerate		= false;		// true = Spiel mit 50 Frames laufen lassen
														// false = Spiel so flüssig wie möglich laufen lassen
bool					Sprache;						// true == deutsch / false == englisch
bool					GameRunning			= true;		// Spiel läuft :-)
bool					GamePaused			= false;	// Spiel eingefroren (wenn man zb das Fenster verlässt)
bool					NochKeinFullScreen	= true;		// Logo noch anzeigen in Paint ?
bool					DebugMode			= false;	// Debug Mode ein/aus
float					SpeedFaktor = 1.0f;				// Faktor, mit dem alle Bewegungen verrechnet werden
DirectGraphicsClass		DirectGraphics;					// Grafik-Objekt
DirectInputClass		DirectInput;					// Input-Objekt
TimerClass				*pTimer;						// Timer Klasse für die Framerate
Logdatei				Protokoll("Game_Log.txt");		// Protokoll Datei
CSoundManager			*pSoundManager =  NULL;			// Sound Manager
DirectGraphicsFont		*pDefaultFont = new(DirectGraphicsFont);
DirectGraphicsFont		*pMenuFont	  = new(DirectGraphicsFont);
TileEngineClass			*pTileEngine;					// Tile Engine
PartikelsystemClass		*pPartikelSystem;				// Das coole Partikelsystem
ProjectileListClass		*pProjectiles;					// Liste mit Schüssen
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
char					LanguageFiles[20][256];			// Strings, die die Filenamen der LanguageFiles beinhalten						// Endbild der Demo
char					ActualLanguage[256];			// Gewählte Language

sCommandLineParams		CommandLineParams;

int WINDOWWIDTH;
int WINDOWHEIGHT;


// --------------------------------------------------------------------------------------
// Variablen für den Spielablauf
// --------------------------------------------------------------------------------------

HBITMAP					SplashScreen = NULL;			// SplashScreen Grafik
PlayerClass				*pPlayer[2];					// Werte der Spieler
HUDClass				*pHUD;							// Das HUD
unsigned char			SpielZustand = CRACKTRO;		// Aktueller Zustand des Spieles
char					StringBuffer[100];				// Für die Int / String Umwandlung

// --------------------------------------------------------------------------------------
// Callback Funktion
// --------------------------------------------------------------------------------------

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
	{
		case WM_CREATE:								// Splash Screen beim erstellen des
		{											// Fensters laden
			SplashScreen = LoadBitmap (g_hinst, MAKEINTRESOURCE(IDB_SPLASHSCREEN));
			return (0);
		} break;

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
		} break;

		case WM_DESTROY:								// Fenster wird geschlossen
		{
			GameRunning = false;						// Spiel beenden
			PostQuitMessage(0);							// Quit-Message posten
			return(0);									// Success zurückliefern
		} break;

		case WM_SYSCOMMAND:								// "Alt aktiviert Menu" ausschalten
		{
			if (wparam == SC_TASKLIST ||
				wparam == SC_KEYMENU)
				return (0);
		} break;

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

		} break;

		default : break;
	}

	//unbearbeitete Nachrichten zurückliefern
	return(DefWindowProc(hwnd, message, wparam, lparam));
}

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

}

// --------------------------------------------------------------------------------------
// Win-Main Funktion
// --------------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstace, 
				   LPSTR lpcmdline,		int nshowcmd)
{
	GamePaused = false;

	WNDCLASSEX			winclass;							// eigene Windows-Klasse
	MSG					message;							// Message	
	RECT				rect;								// Grösse des Desktops

	// evtle Parameter holen und Typ des Parameters rausfinden
	strcpy_s (CommandLineParams.Params, 1, "");

	if (strlen(lpcmdline) != 0)
		strcpy_s (CommandLineParams.Params, strlen(lpcmdline) + 1, lpcmdline);

	FillCommandLineParams();	

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

	// Desktop Window holen und Grösse auslesen (damit wir unser Fenster in der Mitte des Screens
	// positionnieren können)
	DesktopHWND = GetDesktopWindow();
	GetWindowRect(DesktopHWND, &rect);

	// Anfang der Logdatei mit Datum und Uhrzeit
	Protokoll.WriteText(">-------------------------<\n", false);
	Protokoll.WriteText("|        Hurrican         |\n", false);
	Protokoll.WriteText("|   (c) 2007 poke53280    |\n", false);
	Protokoll.WriteText("|                         |\n", false);
	Protokoll.WriteText("|    www.poke53280.de     |\n", false);
	Protokoll.WriteText("|  www.hurrican-game.de   |\n", false);
	Protokoll.WriteText(">-------------------------<\n", false);
	Protokoll.WriteText("Logfile date: ", false);
	strcpy_s(StringBuffer, __DATE__);	Protokoll.WriteText(StringBuffer, false); Protokoll.WriteText(" - ", false);
	strcpy_s(StringBuffer, __TIME__);	Protokoll.WriteText(StringBuffer, false); 

	Protokoll.WriteText("\n\n>-------------<\n", false);
	Protokoll.WriteText(    "| Init Window |\n", false);
	Protokoll.WriteText(    ">-------------<\n\n", false);

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
		Protokoll.WriteText("RegisterClassEx error!\n", true);
		return(0);
	}

	Protokoll.WriteText("RegisterClassEx successfull!\n", false);

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
			Protokoll.WriteText("CreateWindowEx error!\n", true);
			return(0);
		}

	Protokoll.WriteText("CreateWindowEx	successfull!\n", false);
	Protokoll.WriteText("WindowSizeX : ", false); Protokoll.WriteValue(WINDOWWIDTH);
	Protokoll.WriteText("WindowSizeY : ", false); Protokoll.WriteValue(WINDOWHEIGHT);

	if (CommandLineParams.RunWindowMode == false)
		ShowCursor(false);

	Protokoll.WriteText("\n-> Init Window successfull!\n", false);

	ShowWindow(g_hwnd, nshowcmd);						// Fenster anzeigen (sicher ist sicher)
	UpdateWindow(g_hwnd);								// Fenster-infos updaten

//----- Spiel-Initialisierung
	
	if(!GameInit(g_hwnd, hinstance))
	{
		Protokoll.WriteText("\n-> GameInit error!\n\n", true);
		GameRunning = false;
	}
	else
		Protokoll.WriteText("\n-> GameInit successfull!\n\n", false);

//----- Main-Loop 

	while(GameRunning == true)
	{
		while (PeekMessage (&message, NULL, 0, 0, PM_REMOVE))	// Nachricht vom Stapel holen
		{														// und löschen
			TranslateMessage(&message);							// Nachricht überetzen
			DispatchMessage(&message);							// Nachricht an WinProc weiterleiten
			UpdateWindow(g_hwnd);
		}		

		try
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
				pSoundManager->Update();

				// Timer updaten
				pTimer->update();

				// Feste Framerate ? (Spiel läuft in Zeitlupe, wenn zu langsam)
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
			char Buffer[200];

			sprintf_s (Buffer, "Fehler! Unbehandelte Ausname\n %s", str);
			Protokoll.WriteText (Buffer, true);
			GameRunning = false;
		}		
	}

//----- Spiel verlassen

	//pTimer->WriteLogValues();

	if(!GameExit())
		Protokoll.WriteText("-> GameExit Fehler !\n", true);

	Protokoll.WriteText("\n-> Hurrican closed !\n", false);
	Protokoll.WriteText("\nhttp://www.poke53280.de\n", false);
	Protokoll.WriteText("Bugreports, questions etc : information@poke53280.de\n", false);
	Protokoll.WriteText("\n-> logfile end", false);

	// Kein Fehler im Game? Dann Logfile löschen
	if (Protokoll.delLogFile == true)
		DeleteFile("Game_Log.txt");

	return(message.wParam);										// Rückkehr zu Windows
}

// -------------------------------------------------------------------------------------- 
// GameInit, initialisiert die DX Objekte
// -------------------------------------------------------------------------------------- 

bool GameInit(HWND hwnd, HINSTANCE hinstance)
{
	options_Detail = DETAIL_LOW;

	// Language Files
	HWND	ComboBoxLanguageFiles	= NULL;
	ComboBoxLanguageFiles = CreateWindow("COMBOBOX",
			 				"",
							WS_CHILD,
							0, 0, 0, 0, g_hwnd, 0, g_hinst, NULL);

	// Loadinfo Text festlegen
	srand(GetTickCount());
//	DisplayHintNr = rand()%30;

	// *.lng Files anfügen
	if (SendMessage(ComboBoxLanguageFiles, CB_DIR, DDL_READWRITE, (LPARAM) "*.lng") == CB_ERR)
	{
		Protokoll.WriteText ("No language Files found!", true);
	}

	LanguageFileCount = SendMessage (ComboBoxLanguageFiles, CB_GETCOUNT, 0, 0);
	for (int i = 0; i < LanguageFileCount; i++)
	{
		SendMessage (ComboBoxLanguageFiles, CB_GETLBTEXT, i, (LPARAM) LanguageFiles [i]);
	}

	Protokoll.WriteText("\n>--------------------<\n", false);
	Protokoll.WriteText(  "| GameInit gestartet |\n", false);
	Protokoll.WriteText(  ">--------------------<\n", false);

	// Timer initialisieren
	pTimer = new TimerClass();

	// DirectInput initialisieren
	if(!DirectInput.Init(hwnd, hinstance))
	{
		Protokoll.WriteText("\n-> DirectInput8 Initialisierung Fehler ...!\n", true);
		return false;
	}	

	// Sound Manager initialisieren
	pSoundManager = new CSoundManager();

	// Direct3D initialisieren
	if(!DirectGraphics.Init(hwnd, SCREENWIDTH, SCREENHEIGHT, SCREENBPP, true))
	{
		Protokoll.WriteText("\n-> Direct3D Initialisierung Fehler ...!\n", true);
		return false;
	}	

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
	HRESULT hr;
	D3DLOCKED_RECT pLockedRect; 
	int width, height, r, g, b, a, col, temp;
	BYTE  *pRow;
	DWORD *pPixel;

	// Textur locken
	hr = pTexture->itsTexture->LockRect(0, &pLockedRect, 0, 0); 
	if (hr != D3D_OK)
		return;

	// Breite, Höhe und Pitch setzen
	width  = (int)pTexture->itsXSize;
	height = (int)pTexture->itsYSize;
	pRow = (BYTE*)pLockedRect.pBits;	

	for (int y = 0;y < height;y++)
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
	pPlayer[0]  = new PlayerClass();	
	pPlayer[1] = new PlayerClass();
	pPlayer[0]->SoundOff = 0;
	pPlayer[1]->SoundOff = 1;
	memset(pPlayer[0], 0, sizeof(pPlayer[0]));
	memset(pPlayer[1], 0, sizeof(pPlayer[1]));
	
	// Konfiguration laden
	if (LoadConfig() == false)
	{
		Protokoll.WriteText("\n-> No config found. Creating default\n", false);
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

	LoadingScreen.LoadImage("Loading.bmp",    360, 60, 360, 60, 1, 1);
	LoadingBar.LoadImage   ("Loadingbar.bmp", 318, 19, 318, 19, 1, 1);
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
	SchussFlamme[0].LoadImage("SchussFlamme.bmp",  76,  72,  38, 24, 2, 3);
	SchussFlamme[1].LoadImage("SchussFlamme2.bmp", 80,  108,  40, 36, 2, 3);
	SchussFlamme[2].LoadImage("SchussFlamme3.bmp", 48,  114,  24, 38, 2, 3);

	SchussFlammeFlare.LoadImage("SchussFlammeFlare.bmp", 140,  140,  140, 140, 1, 1);

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
		PlayerRide[p].LoadImage  ("Hurri_ride.png",   450, 480, 90,120, 5, 4);

		// Stachelrad
		PlayerRad[p].LoadImage   ("Hurri_rad.bmp",    140,  70, 35, 35, 4, 2);
	}

	Blitzstrahl[0].LoadImage("Blitzstrahl1.bmp",32,  32, 32, 32, 1, 1);
	Blitzstrahl[1].LoadImage("Blitzstrahl2.bmp",32,  32, 32, 32, 1, 1);
	Blitzstrahl[2].LoadImage("Blitzstrahl3.bmp",32,  32, 32, 32, 1, 1);
	Blitzstrahl[3].LoadImage("Blitzstrahl4.bmp",32,  32, 32, 32, 1, 1);
	Blitzflash[0].LoadImage("Blitzflash1.bmp",	66,  64, 66, 64, 1, 1);
	Blitzflash[1].LoadImage("Blitzflash2.bmp",	66,  64, 66, 64, 1, 1);
	Blitzflash[2].LoadImage("Blitzflash3.bmp",	66,  64, 66, 64, 1, 1);
	Blitzflash[3].LoadImage("Blitzflash4.bmp",	66,  64, 66, 64, 1, 1);
	BlitzTexture.LoadImage("Blitztexture.bmp", 64, 64, 64, 64, 1, 1);

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
	Protokoll.WriteText("\n>--------------------<\n", false);
	Protokoll.WriteText(  "| GameExit gestartet |\n", false);
	Protokoll.WriteText(  ">--------------------<\n\n", false);

	// GUI freigeben
	delete(pGUI);

	// Timer freigeben
	delete(pTimer);

	// Sprites freigeben
	delete(pDefaultFont);
	delete(pMenuFont);
	Protokoll.WriteText("-> Fonts freigegeben\n", false);

	// Console beenden
	delete (pConsole);

	// Menu beenden
	delete(pMenu);
	Protokoll.WriteText("-> Hauptmenu freigegeben\n", false);	

	// HUD freigeben
	delete(pHUD);
	Protokoll.WriteText("-> HUD freigegeben\n", false);

	// Tileengine beenden
	delete(pTileEngine);
	Protokoll.WriteText("-> TileEngine freigegeben\n", false);

	// GegnerListe beenden
	delete(pGegner);
	Protokoll.WriteText("-> Gegner-Liste freigegeben\n", false);

	// Player freigeben
	delete(pPlayer[0]);
	delete(pPlayer[1]);
	Protokoll.WriteText("-> Player freigegeben\n", false);

	// Partikelsystem beenden
	delete(pPartikelSystem);
	Protokoll.WriteText("-> Partikelsystem freigegeben\n", false);

	// ProjectileListe beenden
	delete(pProjectiles);
	Protokoll.WriteText("-> Projektil-Liste freigegeben\n", false);

	DirectGraphics.Exit();				// Direct3D    beenden

	delete(pSoundManager);				// DirectSound beenden

	DirectInput.Exit();					// DirectInput beenden

	//PrintStatus();

	return true;
}

// -------------------------------------------------------------------------------------- 
// Heartbeat, der Mainloop. der bei jedem Frame durchlaufen wird
// -------------------------------------------------------------------------------------- 

bool Heartbeat(void)
{
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

		} break;

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
		} break;

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
		} break;

		//----- Hauptmenu
		case MAINMENU :
		{
			pMenu->DoMenu();
		} break;

		//---- Haupt-Gameloop
		case GAMELOOP :
		{
			GameLoop();
		} break;

		default : break;
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

	// Anzahl der aktuell aktiven Schüsse anzeigen
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