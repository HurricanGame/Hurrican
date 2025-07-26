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

#include "Timer.hpp"
#include <algorithm>
#include "DX8Font.hpp"
#include "Globals.hpp"
#include "Logdatei.hpp"

// --------------------------------------------------------------------------------------
// Klassen Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor, setzt diverse Werte auf den Startwert
// --------------------------------------------------------------------------------------

TimerClass::TimerClass() {
    aktuelleZeit = 0;  // Aktuelle Zeit auf 0 setzen
    letzterFrame = 0;  // die des letzten Frames auch
    maxFPS = 0;        // best mögliche Peformance fordern
    vergangeneZeit = 1.0f;
    vergangeneFrames = 0;       // Gesamtzahl der Frames
    FPSMinimum = 10000.0f;      // kleinste Framerate setzen
    FPSMaximum = 0.0f;          // grösste Framerate setzen
    DurchschnittFramerate = 0;  // durchscnittliche Framerate
    MoveSpeed = 10.0f;          // so moven wie bei 60 fps
    SpeedFaktor = 1.0f;

    letzterFrame = SDL_GetTicks();
    ZeitFaktor = 0.001f;
}

// --------------------------------------------------------------------------------------
// Timer updaten und Zeiten berechnen
// --------------------------------------------------------------------------------------

void TimerClass::update() {
    vergangeneFrames++;             // für die Schnittberechnung
    aktuelleZeit = SDL_GetTicks();  // wir SDL_GetTicks

    vergangeneZeit =
        (std::max<std::int64_t>(0, aktuelleZeit - letzterFrame)) * ZeitFaktor;  // vergangene Zeit neu setzen
    letzterFrame = aktuelleZeit;                                                // letzten Frame aktualisieren

    aktuelleFramerate = 1 / vergangeneZeit;  // Framerate berechnen
    if (aktuelleFramerate > FPSMaximum)      // neue Maximale Framerate ?
        FPSMaximum = aktuelleFramerate;
    if (aktuelleFramerate < FPSMinimum)  // neue Minimale Framerate ?
        FPSMinimum = aktuelleFramerate;

    // Durschnitt der Framerates berechnen
    DurchschnittFramerate = (vergangeneFrames * DurchschnittFramerate + aktuelleFramerate) / (vergangeneFrames + 1);

    // Speedfaktor errechnen
    SpeedFaktor = static_cast<float>(static_cast<double>(MoveSpeed) * vergangeneZeit);

    // begrenzen
    constexpr float MAX_FACTOR = 2.0f;
    if (SpeedFaktor > MAX_FACTOR)
        SpeedFaktor = MAX_FACTOR;
}

// --------------------------------------------------------------------------------------
// Warten bis die maximal gesetzte Framerate erreicht ist
// --------------------------------------------------------------------------------------

void TimerClass::wait() {
    if (maxFPS == 0)  // bei Framerate = 0 gleich wieder zurück
        return;       // da wir da nichts abwarten müssen :-)

    // Diese Schleife wird solange durchlaufen, bis die gewünschte Framerate erreicht ist
    do {
        // Zeit holen
        aktuelleZeit = SDL_GetTicks();
    } while (maxFPS < 1 / ((aktuelleZeit - letzterFrame) * ZeitFaktor));
}

// --------------------------------------------------------------------------------------
// Wartetet "Wert" Milli-Sekunden
// --------------------------------------------------------------------------------------

void TimerClass::wait(int Wert) {
    do {
        // Zeit holen
        aktuelleZeit = SDL_GetTicks();
    } while (Wert > (aktuelleZeit - letzterFrame) * ZeitFaktor * 1000);
}

// --------------------------------------------------------------------------------------
// Neue Movespeed setzen, die durch die FPS geteilt wird
// --------------------------------------------------------------------------------------

void TimerClass::SetMoveSpeed(float Wert) {
    MoveSpeed = Wert;
}

// --------------------------------------------------------------------------------------
// Movespeed zurückliefern
// --------------------------------------------------------------------------------------

float TimerClass::GetMoveSpeed() const {
    return MoveSpeed;
}

// --------------------------------------------------------------------------------------
// Maximum Framerate setzen
// --------------------------------------------------------------------------------------

void TimerClass::SetMaxFPS(int Wert) {
    maxFPS = Wert;  // Wert setzen
}

int TimerClass::GetMaxFPS() const {
    return maxFPS;
}

// --------------------------------------------------------------------------------------
// vergangene Zeit in Milli-Sekunden holen
// --------------------------------------------------------------------------------------

double TimerClass::getElapsed() const {
    return vergangeneZeit;  // Den Bahnhof in die Luft jagen, was sonst ?
}

// --------------------------------------------------------------------------------------
// Framerate auslesen
// --------------------------------------------------------------------------------------

double TimerClass::getFrameRate() const {
    return (aktuelleFramerate);  // Framerate berechnen und zurückgeben
}

// --------------------------------------------------------------------------------------
// Minimale Framerate auslesen
// --------------------------------------------------------------------------------------

double TimerClass::getMinFrameRate() const {
    return (FPSMinimum);
}

// --------------------------------------------------------------------------------------
// Maximale Framerate auslesen
// --------------------------------------------------------------------------------------

double TimerClass::getMaxFrameRate() const {
    return (FPSMaximum);
}

// --------------------------------------------------------------------------------------
// Durchschnittliche FPS auslesen
// --------------------------------------------------------------------------------------

double TimerClass::getAverageFPS() const {
    return DurchschnittFramerate;
}

// --------------------------------------------------------------------------------------
// Wert holen, mit dem die Bewegungen verrechnet werden
// --------------------------------------------------------------------------------------
float TimerClass::getSpeedFactor() const {
    return SpeedFaktor;
}

// --------------------------------------------------------------------------------------
// Set the speed factor
// --------------------------------------------------------------------------------------
void TimerClass::setSpeedFactor(float Wert) {
    SpeedFaktor = Wert;
}

// --------------------------------------------------------------------------------------
// Max und Min FPS resetten
// --------------------------------------------------------------------------------------

void TimerClass::resetMaxMinFPS() {
    FPSMaximum = 0.0f;
    FPSMinimum = 10000.0f;
    DurchschnittFramerate = aktuelleFramerate;
}

// --------------------------------------------------------------------------------------
// Max und Min und Durchschnitts FPS in Logdatei sichern
// --------------------------------------------------------------------------------------

void TimerClass::WriteLogValues() {
    Protokoll << "\nMaximum  FPS : " << static_cast<int>(FPSMaximum) << std::endl;
    Protokoll << "Minimum  FPS : " << static_cast<int>(FPSMinimum) << std::endl;
    Protokoll << "Average  FPS : " << static_cast<int>(DurchschnittFramerate) << std::endl;
}

// --------------------------------------------------------------------------------------
// Get the current time
// --------------------------------------------------------------------------------------

std::int64_t TimerClass::getTime() const {
    return aktuelleZeit;
}
