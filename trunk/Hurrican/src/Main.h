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

#include "windows.h"
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
};

// --------------------------------------------------------------------------------------
// Defines 
// --------------------------------------------------------------------------------------

#define WIN_32_LEAN_AND_MEAN						// MFC abschalten
#define WINDOWCLASSNAME	"HurricanWindowsClass"		// Name der Windows-Klasse
#define SCREENWIDTH		640							// D3D Screen Breite
#define	SCREENHEIGHT	480							// D3D Screen Höhe
#define	SCREENBPP		16							// Bits per Pixel
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

void ShowDebugInfo(void);							// Allen möglichen Kram anzeigen

#endif