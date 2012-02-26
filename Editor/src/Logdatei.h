// Datei : Logdatei.h

// -------------------------------------------------------------------------------------- 
//
// Logdatei Klasse
// zum leichten Handhaben einer Protokoll Datei
//
// (c) 2002 J�rg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __Logdatei_h__
#define __Logdatei_h__

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

class Logdatei
{
	private:
		char itsFilename[20];								// Dateiname der Log-Datei

	public:
		     Logdatei    (char Name[20]);					// Konstruktor
			~Logdatei    (void);							// Desktruktor;
		void WriteText   (char Text[180], bool Abbruch);	// Text in Datei schreiben
		void WriteValue  (int value);						// Wert in Datei schreiben
};

#endif