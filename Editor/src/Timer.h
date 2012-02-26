// Datei : Timer.h

// -------------------------------------------------------------------------------------- 
//
// Timer Klasse
// zum Auslesen/Berechnen der Framerate
// und zur Synchronisierung der Bewegungen mit der Framerate
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __Timer_h__
#define __Timer_h__

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include <windows.h>

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Klassen Deklaration
// --------------------------------------------------------------------------------------

class TimerClass
{
	private:
		bool		PerformanceCounter;    	// Performance Counter vorhanden ?
		LONGLONG	aktuelleZeit;			// die aktuelle Zeit
		LONGLONG	Frequenz;				// Performance Timer Frequenz
		LONGLONG	letzterFrame;			// Zeit des vorherigen Frames
		int			maxFPS;					// Maximum Framerate (Framebremse)
		int			MoveSpeed;				// Wird durch die aktuelle Framerate geteilt
		double		vergangeneZeit;			// Zeit seit dem vorherigen Frame
		LONGLONG	vergangeneFrames;		// Vergangene Frames seit Beginn (für Schnitt)
		double		ZeitFaktor;				// Skalierungsfaktor
		double		aktuelleFramerate;		// Aktuelle Framerate
		double		DurchschnittFramerate;	// Durchschnittliche Framerate
		double		FPSMinimum;				// kleinste gemessene Framerate
		double		FPSMaximum;				// grösste  gemessene Framerate

   public:
		float		SpeedFaktor;			// Faktor, mit dem alle Werte verrechnet werden

        TimerClass   (void);				// Konstruktor, prüft auf den PerformanceCounter
       ~TimerClass   (void);				// Desktruktor
        void   update(void);				// Timer updaten und Zeiten berechnen
        void   wait  (void);				// Maximum Framrate abwarten
        void   wait	   (int Wert);			// Wartetet "Wert" Milli-Sekunden
		void   SetMoveSpeed(int Wert);		// neue Movespeed setzen
		int    GetMoveSpeed(void);			// MoveSpeed abfragen
        void   SetMaxFPS(int Wert);			// Maximum Framerate setzen
        double getElapsed();				// vergagnge Zeit in Milli-Sekunden holen
        double getFrameRate	  (void);		// Framerate auslesen
		double getMinFrameRate(void);		// Minimale Framerate auslesen
		double getMaxFrameRate(void);		// Maximale Framerate auslesen
		double getAverageFPS  (void);		// Durchschnittliche FPS auslesen
		double getSpeedFactor (void);		// Wert holen, mit dem die Bewegungen verrechnet werden
		void   resetMaxMinFPS (void);		// Max und Min FPS resetten
		void   WriteLogValues (void);		// Werte in Logdatei sichern
};

#endif