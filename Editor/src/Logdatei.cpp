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

// --------------------------------------------------------------------------------------
// Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// erstellt eine neue Logdatei mit dem Namen "Name"
// --------------------------------------------------------------------------------------

Logdatei::Logdatei(char Name[20])
{
	strcpy(itsFilename, Name);							// Namen sichern
	Logfile = fopen(itsFilename, "w");					// Datei neu erstellen
	fclose(Logfile);									// und gleich wieder schliessen
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
	Logfile = fopen(itsFilename, "a");					// Datei zum anfügen öffnen
	fprintf(Logfile, Text);								// und Text schreiben
	fclose(Logfile);									// Datei wieder schliessen

	if(Abbruch == true)									// Abbruch nach Log-Eintrag ?
	{
		MessageBox (NULL, Text, "Ein Fehler ist aufgetreten !", MB_OK | MB_ICONEXCLAMATION);
		MessageBeep(MB_ICONEXCLAMATION);
	}
}

// --------------------------------------------------------------------------------------
// WriteText
// schreibt den Wert "Value" in die Logdatei
// --------------------------------------------------------------------------------------

void Logdatei::WriteValue(int Value)
{
	Logfile = fopen(itsFilename, "a");					// Datei zum anfügen öffnen
	fprintf(Logfile, "%i\n", Value);					// und Wert schreiben
	fclose(Logfile);									// Datei wieder schliessen
}