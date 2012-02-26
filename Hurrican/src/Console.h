// Datei : Console.h

// -------------------------------------------------------------------------------------- 
//
// Konsolen-Klasse für Hurrican
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __Console_h__
#define __Console_h__

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "DX8Input.h"
#include "DX8Font.h"
#include "DX8Sprite.h"
#include "Globals.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define MAX_LINES	20				// Anzahl der Zeilen in der Konsole
#define MAX_CHARS	90				// Anzahl der Zeichen pro Zeile

#define CONSOLE_COMMAND(x) (false)//strcmp (_strlwr_s(Buffer), x) == 0)
#define CONSOLE_PRINT(x)   {strcpy_s(Buffer, strlen(x) + 1, x); ScrollUp();}

// --------------------------------------------------------------------------------------
// Consolen-Klasse
// --------------------------------------------------------------------------------------

class ConsoleClass
{
	private:
		float					its_Alpha;					// Aktueller Alphawert der Konsole
		float					Fade;						// Ein / Aus faden ?
		DirectGraphicsSprite	ConsoleGFX;					// Grafik der Konsole
		bool					Activate;					// Aktivierungstaste gedrückt oder nicht
		char					Text[MAX_LINES][MAX_CHARS]; // Text in der Konsole
		char					Buffer[MAX_CHARS];
		bool					Pressed[256];				// Für Tasten-lock		
		bool					CONSOLE_CHEAT(char *cheat);

	public:
		bool					Active;						// Gerade aktiv ?
		bool					Showing;					// Gerade angezeigt ?
		bool					RenderBinary;				// Alles als 0 und 1 rendern (nur fun)

		     ConsoleClass	 (void);						// Menu initialisieren und Grafiken laden
			~ConsoleClass	 (void);						// Menu freigeben

		void ShowConsole	 (void);						// Menu anzeigen
		void Open			 (void);						// öffnen
		void Hide			 (void);						// verstecken
		void CheckInput		 (void);						// Wurde was eingegeben ?
		void CheckCommands	 (void);						// Text nach enter drücken checken
		void ScrollUp		 (void);						// Eine Zeile hochscrollen
		bool DoConsole	     (void);						// Alles machen, was die Console betrifft =)
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern ConsoleClass			*pConsole;

#endif