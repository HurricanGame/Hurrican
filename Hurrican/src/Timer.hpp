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

#ifndef _TIMER_HPP_
#define _TIMER_HPP_

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

// Variable mit der aktuellen FrameRate synchronisieren
#define SYNC *SpeedFaktor

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------
#include "SDL_port.hpp"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Klassen Deklaration
// --------------------------------------------------------------------------------------

class TimerClass {
  private:
    std::int64_t vergangeneFrames;  // Vergangene Frames seit Beginn (für Schnitt)
    std::int64_t aktuelleZeit;      // die aktuelle Zeit
    std::int64_t letzterFrame;      // Zeit des vorherigen Frames
    float MoveSpeed;                // Wird durch die aktuelle Framerate geteilt
    double ZeitFaktor;              // Skalierungsfaktor
    double DurchschnittFramerate;   // Durchschnittliche Framerate
    double FPSMinimum;              // kleinste gemessene Framerate
    double FPSMaximum;              // grösste  gemessene Framerate

  public:
    double vergangeneZeit;     // Zeit seit dem vorherigen Frame
    double aktuelleFramerate;  // Aktuelle Framerate
    int maxFPS;                // Maximum Framerate (Framebremse)
    float SpeedFaktor;         // Faktor, mit dem alle Werte verrechnet werden

    TimerClass();               // Konstruktor
    ~TimerClass() = default;    // Desktruktor
    void update();              // Timer updaten und Zeiten berechnen
    void wait();                // Maximum Framrate abwarten
    void wait(int Wert);            // Wartetet "Wert" Milli-Sekunden
    void SetMoveSpeed(float Wert);  // neue Movespeed setzen
    float GetMoveSpeed();       // MoveSpeed abfragen
    void SetMaxFPS(int Wert);       // Maximum Framerate setzen
    double getElapsed();            // vergagnge Zeit in Milli-Sekunden holen
    double getFrameRate();      // Framerate auslesen
    double getMinFrameRate();   // Minimale Framerate auslesen
    double getMaxFrameRate();   // Maximale Framerate auslesen
    double getAverageFPS();     // Durchschnittliche FPS auslesen
    double getSpeedFactor();    // Wert holen, mit dem die Bewegungen verrechnet werden
    void resetMaxMinFPS();      // Max und Min FPS resetten
    void WriteLogValues();      // Werte in Logdatei sichern
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern float SpeedFaktor;
extern TimerClass Timer;

#endif
