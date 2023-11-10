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
// Includes
// --------------------------------------------------------------------------------------
#include "SDL_port.hpp"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Klassen Deklaration
// --------------------------------------------------------------------------------------

// The Timer class needs the global SpeedFactor variable
extern float SpeedFaktor;

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

    double vergangeneZeit;     // Zeit seit dem vorherigen Frame
    double aktuelleFramerate;  // Aktuelle Framerate
    int maxFPS;                // Maximum Framerate (Framebremse)

  public:
    TimerClass();                   // Konstruktor
    ~TimerClass() = default;        // Desktruktor
    void update();                  // Timer updaten und Zeiten berechnen
    void wait();                    // Maximum Framrate abwarten
    void wait(int Wert);            // Wartetet "Wert" Milli-Sekunden
    void SetMoveSpeed(float Wert);  // neue Movespeed setzen
    float GetMoveSpeed() const;     // MoveSpeed abfragen
    void SetMaxFPS(int Wert);       // Maximum Framerate setzen
    int GetMaxFPS() const;          // Maximum Framerate setzen
    double getElapsed() const;      // vergagnge Zeit in Milli-Sekunden holen
    double getFrameRate() const;    // Framerate auslesen
    double getMinFrameRate() const; // Minimale Framerate auslesen
    double getMaxFrameRate() const; // Maximale Framerate auslesen
    double getAverageFPS() const;   // Durchschnittliche FPS auslesen
    double getSpeedFactor() const;  // Wert holen, mit dem die Bewegungen verrechnet werden
    void resetMaxMinFPS();          // Max und Min FPS resetten
    void WriteLogValues();          // Werte in Logdatei sichern

    // Variable mit der aktuellen FrameRate synchronisieren
    inline float sync(float val) const { return val * SpeedFaktor; }
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern TimerClass Timer;

#endif
