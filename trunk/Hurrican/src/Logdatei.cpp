// Datei : Logdatei.cpp

// -------------------------------------------------------------------------------------- 
//
// Logdatei Klasse
// zum leichten Handhaben einer Protokoll Datei
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "Logdatei.h"									// Header-Datei einbinden
#include <windows.h>									// Windowsheader für Messagebox und Beep
#include <string.h>										// Für String Operationen
#include <stdio.h>										// Für Datei Operationen

// --------------------------------------------------------------------------------------
// Variablen
// --------------------------------------------------------------------------------------

FILE *Logfile;											// Logdatei

extern HWND					g_hwnd;						// Fenster Handle des Hauptfensters
extern bool					GameRunning;				// Läuft das Spiel noch ?

// --------------------------------------------------------------------------------------
// Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// erstellt eine neue Logdatei mit dem Namen "Name"
// --------------------------------------------------------------------------------------

Logdatei::Logdatei(char Name[20])
{
	int len = strlen(Name) + 1;
	strcpy_s(itsFilename, len, Name);							// Namen sichern
	fopen_s(&Logfile, itsFilename, "w");					// Datei neu erstellen
	fclose(Logfile);									// und gleich wieder schliessen

	delLogFile = false;
}

// --------------------------------------------------------------------------------------
// Destruktor
// --------------------------------------------------------------------------------------

Logdatei::~Logdatei()
{
}

// --------------------------------------------------------------------------------------
// WriteText
// schreibt den Text "Text" in die Logdatei
// bei Fehler-Eintrag ggf Messagebox ausgeben
// --------------------------------------------------------------------------------------

void Logdatei::WriteText(char Text[180], bool Abbruch)
{
	fopen_s(&Logfile, itsFilename, "a");					// Datei zum anfügen öffnen
	fprintf_s(Logfile, Text);								// und Text schreiben
	fclose(Logfile);									// Datei wieder schliessen

	if(Abbruch == true)									// Abbruch nach Log-Eintrag ?
	{
		fopen_s(&Logfile, itsFilename, "a");					// Datei zum anfügen öffnen
		fprintf(Logfile, "\n    ^\n");
		fprintf(Logfile, "   /|\\\n");
		fprintf(Logfile, "    |\n");
		fprintf(Logfile, "    |\n");
		fprintf(Logfile, "    |\n");
		fprintf(Logfile, "  Error\n\n");
		fclose(Logfile);									// Datei wieder schliessen

		MessageBox (g_hwnd, Text, "Ein Fehler ist aufgetreten !", MB_OK | MB_ICONEXCLAMATION);
		delLogFile  = false;
		GameRunning = false;
	}
}

// --------------------------------------------------------------------------------------
// WriteText
// schreibt den Wert "Value" in die Logdatei
// --------------------------------------------------------------------------------------

void Logdatei::WriteValue(int Value)
{
	fopen_s(&Logfile, itsFilename, "a");					// Datei zum anfügen öffnen
	fprintf(Logfile, "%i\n", Value);					// und Wert schreiben
	fclose(Logfile);									// Datei wieder schliessen
}