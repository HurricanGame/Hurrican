// Datei : Globals.h

// -------------------------------------------------------------------------------------- 
//
// Header Datei mit global gültigen Makros, Defines usw
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __Globals_h__
#define __Globals_h__

// --------------------------------------------------------------------------------------
// Defines 
// --------------------------------------------------------------------------------------

#define PI				3.14159265358979f			// Pi =)

//----- Spielzustände

#define CRACKTRO		0
#define VORSPANN		1
#define INTRO			2
#define MAINMENU		3
#define GAMELOOP		4
#define OUTTRO			5

// Tasten Abfragen (simple Methode)
#define SimpleKeydown(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

// Schnittstellen erst prüfen, ob sie existieren, und dann freigeben
#define SafeRelease(x) if (x != NULL) { x->Release(); x=NULL; }

// --------------------------------------------------------------------------------------
// Definitionen für den Spielablauf
// --------------------------------------------------------------------------------------

#define MAX_ENERGY				100					// Maximalwert für die Spielerenergie
#define MAX_ARMOUR				200					// Maximalwert für das Rad
#define DIAMONDS_FOR_EXTRALIFE	 50					// Diamanten die man für 200 Zeiteinheiten

#define RARFILENAME "Hurrican.dat"
#define RARFILEPASSWORD "+/#(%,,/"

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "Texts.h"
#include "mymath.h"

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern bool	Sprache;								// true == deutsch / false == englisch (nur für Menugrafiken)
extern bool	GameRunning;							// Spiel läuft :-)
extern bool	DebugMode;								// Debug Mode ein/aus
extern HWND	g_hwnd;									// Handle des Hauptfensters
extern bool FlameThrower;
extern bool HasCheated;
extern RECT	srcrect, destrect;

#endif