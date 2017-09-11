// Datei : Logdatei.hpp

// --------------------------------------------------------------------------------------
//
// Logdatei Klasse
// zum leichten Handhaben einer Protokoll Datei
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _LOGDATEI_HPP_
#define _LOGDATEI_HPP_

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

class Logdatei
{
private:
    char itsFilename[20];								// Dateiname der Log-Datei

public:
    bool	delLogFile;									// Logfile am Ende löschen, wenn kein Fehler auftrat

    Logdatei    (const char Name[20]);					// Konstruktor
    ~Logdatei    (void);							// Desktruktor;
    void WriteText   (bool Abbruch, const char* text, ...);	// Text in Datei schreiben
    void WriteValue  (int value);						// Wert in Datei schreiben
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern Logdatei	Protokoll;

#endif

