// Datei : Gameplay.hpp

// --------------------------------------------------------------------------------------
//
// Beinhaltet den Haupt Game-Loop
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _GAMEPLAY_HPP_
#define _GAMEPLAY_HPP_

#include "DX8Sprite.hpp"
#include "Tileengine.hpp"

#include <string>

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

enum {
  DETAIL_LOW = 0,      // alles total billig
  DETAIL_MEDIUM = 1,   // animiertes Wasser dazu
  DETAIL_HIGH = 2,     // geiler Blitz, Leuchteffekte und Lightmaps
  DETAIL_MAXIMUM = 3   // Schockwelle
};

// --------------------------------------------------------------------------------------
// Prototypen von Funktionen
// --------------------------------------------------------------------------------------

void InitNewGame();                        // Neues Spiel initialisieren
void InitNewGameLevel();                   // Alles für den Levelstart in Stage NR vorbereiten
void GameLoop();                           // Haupt-Spielablauf
void ScreenWackeln();                      // Screen wackeln lassen
void ShakeScreen(float staerke);               // Angeben wie stark der Screen wackeln soll
void SetScreenShake();                     // Geshakten Screen setzen bzw Matrix dafür einstellen
void StageClear(bool PlaySong = true);         // Spieler aus dem Level abgehen lassen
void SummaryScreen();                      // Zusammenfassung des Levels
bool LoadConfig();                         // Konfigurationen laden
void SaveConfig();                         // Konfigurationen speichern
void CreateDefaultControlsConfig(int player);  // Load per-player default controls configuration
void CreateDefaultConfig();                // DefaultKonfigurationen erstellen
bool DisplayLoadInfo(std::string Text);    // Text beim Loading anzeigen
bool NewDemo(const char Filename[]);           // Neues Demo aufzeichnen
void RecordDemo();                         // Spielereingaben sichern
bool LoadDemo(const char Filename[]);          // Demo laden
void PlayDemo();                           // Demo laufen lassen
void EndDemo();                            // Demo anhalten
void LeaveGameLoop();                      // Game Loop verlassen
void ScrolltoPlayeAfterBoss();             // Der Name ist Programm
void ShowPissText();
void ShowGameOver();
const char *convertText(const char *text);
PlayerClass *ChooseAim();

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern GameStateEnum SpielZustand;
extern bool NochKeinFullScreen;

//----- externe Variablen zum Screen-Wackeln lassen (zB bei SpinnenBombe)

extern int MAX_LEVELS;
extern int NUMPLAYERS;
extern float FahrstuhlPos;   // Nur im Fahrstuhl Level
extern float WackelMaximum;  // Maximaler Screen-Wackel Ausschlag
extern float WackelValue;    // Aktueller Screen-Wackel Ausschlag
extern float WackelDir;      // Aktuelle Wackel-Richtung
extern float WackelSpeed;    // Aktuelle Wackel-Geschwindigkeit
extern float ScreenWinkel;  // in welchem zWinkel steht der Screen grad (für Säulen, die das Level zum Kippen bringen)
extern float WarningCount;  // Counter, ob ein "Warning" angezeigt wird
extern bool RunningTutorial;
extern int options_Detail;
extern bool StopStageMusicAtStart;
extern bool UseForceFeedback;
extern bool JoystickFound;
extern bool ShowSummary;
extern bool FlugsackFliesFree;
extern int DisplayHintNr;

extern const float ShockWaveX;
extern const float ShockWaveY;

extern std::string StageReihenfolge[256];

#endif
