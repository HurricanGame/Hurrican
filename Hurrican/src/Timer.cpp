// Datei : Timer.cpp

// -------------------------------------------------------------------------------------- 
//
// Timer Klasse
// zum Auslesen/Berechnen der Framerate
// und zur Synchronisierung der Bewegungen mit der Framerate
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "Globals.h"
#include "Timer.h"
#include "Logdatei.h"
#include "DX8Font.h"

// --------------------------------------------------------------------------------------
// Klassen Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor, prüft auf den PerformanceCounter und setzt diverse Werte auf den Startwert
// --------------------------------------------------------------------------------------

TimerClass::TimerClass(void)
{
   aktuelleZeit				= 0;							// Aktuelle Zeit auf 0 setzen
   letzterFrame				= 0;							// die des letzten Frames auch
   maxFPS					= 0;							// best mögliche Peformance fordern
   vergangeneZeit			= 1.0f;
   vergangeneFrames			= 0;							// Gesamtzahl der Frames
   FPSMinimum				= 10000.0f;						// kleinste Framerate setzen
   FPSMaximum				= 0.0f;							// grösste Framerate setzen
   DurchschnittFramerate	= 0;							// durchscnittliche Framerate
   MoveSpeed				= 10.0f;						// so moven wie bei 60 fps
   SpeedFaktor				= 1.0f;

   // testen, ob ein PerformanceCounter exisitert
   if(QueryPerformanceFrequency((LARGE_INTEGER *) &Frequenz))
   { 
	   PerformanceCounter=true;
	   QueryPerformanceCounter((LARGE_INTEGER *) &letzterFrame); 
	   ZeitFaktor=1.0f/Frequenz;
	   //Protokoll.WriteValue(Frequenz);
   }
   // wenn nicht, dann timeGetTime verwenden
   else
   { 
     PerformanceCounter=false;
	 letzterFrame=timeGetTime();
	 ZeitFaktor=0.001f;
   } 
}

// --------------------------------------------------------------------------------------
// Desktruktor (leer)
// --------------------------------------------------------------------------------------

TimerClass::~TimerClass(void)
{
}

// --------------------------------------------------------------------------------------
// Timer updaten und Zeiten berechnen
// --------------------------------------------------------------------------------------

void  TimerClass::update(void)
{
	vergangeneFrames++;												// für die Schnittberechnung
    if(PerformanceCounter)											// Counter vorhanden ?
        QueryPerformanceCounter((LARGE_INTEGER *) &aktuelleZeit);   // dann beutzen
    else															// wenn nicht, dann benutzen
	aktuelleZeit=timeGetTime();										// wir timeGetTime

    vergangeneZeit=(aktuelleZeit-letzterFrame)*ZeitFaktor;			// vergangene Zeit neu setzen
    letzterFrame=aktuelleZeit;										// letzten Frame aktualisieren

	aktuelleFramerate=1/vergangeneZeit;								// Framerate berechnen
	if(aktuelleFramerate>FPSMaximum)								// neue Maximale Framerate ?
		FPSMaximum = aktuelleFramerate;
	if(aktuelleFramerate<FPSMinimum)								// neue Minimale Framerate ?
		FPSMinimum = aktuelleFramerate;

	// Durschnitt der Framerates berechnen
	DurchschnittFramerate = (vergangeneFrames*DurchschnittFramerate+aktuelleFramerate)
				  		   /(vergangeneFrames+1);

	// Speedfaktor errechnen
	SpeedFaktor = (float)(MoveSpeed * vergangeneZeit);

	// begrenzen
	if (SpeedFaktor > 1.0f)
		SpeedFaktor = 1.0f;
}

// --------------------------------------------------------------------------------------
// Warten bis die maximal gesetzte Framerate erreicht ist
// --------------------------------------------------------------------------------------

void  TimerClass::wait(void)
{
	if(maxFPS==0)							// bei Framerate = 0 gleich wieder zurück
		return;								// da wir da nichts abwarten müssen :-)

	// Diese Schleife wird solange durchlaufen, bis die gewünschte Framerate erreicht ist
	do
	{	// Zeit holen
		if(PerformanceCounter)				// mit PerformanceCounter
			QueryPerformanceCounter((LARGE_INTEGER *) &aktuelleZeit); 
		else								// oder timeGetTime, je nach dem
			aktuelleZeit=timeGetTime(); 
	}
	while(maxFPS<(int)1/((aktuelleZeit-letzterFrame)*ZeitFaktor));
}

// --------------------------------------------------------------------------------------
// Wartetet "Wert" Milli-Sekunden
// --------------------------------------------------------------------------------------

void  TimerClass::wait(int Wert)
{
	do
	{	// Zeit holen
		if(PerformanceCounter)						// mit PerformanceCounter
			QueryPerformanceCounter((LARGE_INTEGER *) &aktuelleZeit); 
		else										// oder timeGetTime, je nach dem
			aktuelleZeit=timeGetTime(); 
	}
	while(Wert>(aktuelleZeit-letzterFrame)*ZeitFaktor*1000);
}

// --------------------------------------------------------------------------------------
// Neue Movespeed setzen, die durch die FPS geteilt wird
// --------------------------------------------------------------------------------------

void TimerClass::SetMoveSpeed(float Wert)
{
	MoveSpeed = Wert;
}

// --------------------------------------------------------------------------------------
// Movespeed zurückliefern
// --------------------------------------------------------------------------------------

float TimerClass::GetMoveSpeed(void)
{
	return MoveSpeed;
}

// --------------------------------------------------------------------------------------
// Maximum Framerate setzen
// --------------------------------------------------------------------------------------

void  TimerClass::SetMaxFPS(int Wert)
{
	maxFPS=Wert;									// Wert setzen
}

// --------------------------------------------------------------------------------------
// vergangene Zeit in Milli-Sekunden holen
// --------------------------------------------------------------------------------------

double TimerClass::getElapsed()
{
	return vergangeneZeit;							// Den Bahnhof in die Luft jagen, was sonst ?
}

// --------------------------------------------------------------------------------------
// Framerate auslesen
// --------------------------------------------------------------------------------------

double TimerClass::getFrameRate(void)
{
	return(aktuelleFramerate);						// Framerate berechnen und zurückgeben
}

// --------------------------------------------------------------------------------------
// Minimale Framerate auslesen
// --------------------------------------------------------------------------------------

double TimerClass::getMinFrameRate(void)
{
	return(FPSMinimum);
}

// --------------------------------------------------------------------------------------
// Maximale Framerate auslesen
// --------------------------------------------------------------------------------------

double TimerClass::getMaxFrameRate(void)
{
	return(FPSMaximum);
}

// --------------------------------------------------------------------------------------
// Durchschnittliche FPS auslesen
// --------------------------------------------------------------------------------------

double TimerClass::getAverageFPS(void)
{
	return DurchschnittFramerate;
}

// --------------------------------------------------------------------------------------
// Wert holen, mit dem die Bewegungen verrechnet werden
// --------------------------------------------------------------------------------------
double TimerClass::getSpeedFactor (void)
{
	return vergangeneZeit;
}

// --------------------------------------------------------------------------------------
// Max und Min FPS resetten
// --------------------------------------------------------------------------------------

void TimerClass::resetMaxMinFPS(void)
{
	FPSMaximum			  = 0.0f;
	FPSMinimum			  = 10000.0f;
	DurchschnittFramerate = aktuelleFramerate;
}

// --------------------------------------------------------------------------------------
// Max und Min und Durchschnitts FPS in Logdatei sichern
// --------------------------------------------------------------------------------------

void TimerClass::WriteLogValues(void)
{
	Protokoll.WriteText("\nMaximale      FPS : ", false); Protokoll.WriteValue((int)(FPSMaximum));
	Protokoll.WriteText(  "Minimale      FPS : ", false); Protokoll.WriteValue((int)(FPSMinimum));
	Protokoll.WriteText(  "Durchschnitts FPS : ", false); Protokoll.WriteValue((int)(DurchschnittFramerate));
}