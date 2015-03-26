// Datei : Globals.h

// --------------------------------------------------------------------------------------
//
// Header Datei mit global g�ltigen Makros, Defines usw
//
// (c) 2002 J�rg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __Globals_h__
#define __Globals_h__

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "Texts.h"
#include "SDL_port.h"
#include "Math.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

//----- Spielzust�nde

#define CRACKTRO		0
#define VORSPANN		1
#define INTRO			2
#define MAINMENU		3
#define GAMELOOP		4
#define OUTTRO			5

// Tasten Abfragen (simple Methode)
#define SimpleKeydown(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

// Schnittstellen erst pr�fen, ob sie existieren, und dann freigeben
#define SafeRelease(x) if (x != NULL) { x->Release(); x=NULL; }

// --------------------------------------------------------------------------------------
// Definitionen f�r den Spielablauf
// --------------------------------------------------------------------------------------

#define MAX_ENERGY				100					// Maximalwert f�r die Spielerenergie
#define MAX_ARMOUR				200					// Maximalwert f�r das Rad
#define DIAMONDS_FOR_EXTRALIFE	 50					// Diamanten die man f�r 200 Zeiteinheiten

#define RARFILENAME "Hurrican.dat"
#define RARFILEPASSWORD "+/#(%,,/"

#if defined(__AROS__)
#define TEMP_FILE_PREFIX "T:"
#else
#define TEMP_FILE_PREFIX
#endif

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern bool	Sprache;								// true == deutsch / false == englisch (nur f�r Menugrafiken)
extern bool	GameRunning;							// Spiel l�uft :-)
#ifdef _DEBUG
extern bool	DebugMode;								// Debug Mode ein/aus
#endif
extern HWND	g_hwnd;									// Handle des Hauptfensters
extern bool FlameThrower;
extern bool HasCheated;
extern RECT	srcrect, destrect;

#endif

