// Datei : Gameplay.h

// -------------------------------------------------------------------------------------- 
//
// Beinhaltet den Haupt Game-Loop
//
// (c) 2002 J�rg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _Gameplay_h_
#define _Gameplay_h_

#include "DX8Sprite.h"
#include "TileEngine.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define CONFIGFILE "Hurrican.cfg"				// Name der Konfigurationsdatei
#define TUTORIAL_END 99

#define DETAIL_LOW		0						// alles total billig
#define DETAIL_MEDIUM	1						// animiertes Wasser dazu
#define DETAIL_HIGH		2						// geiler Blitz, Leuchteffekte und Lightmaps
#define DETAIL_MAXIMUM	3						// Schockwelle

// --------------------------------------------------------------------------------------
// Prototypen von Funktionen
// --------------------------------------------------------------------------------------

void InitNewGame	(void);						// Neues Spiel initialisieren
void InitNewGameLevel(int Nr);					// Alles f�r den Levelstart in Stage NR vorbereiten
void GameLoop		(void);						// Haupt-Spielablauf
void ScreenWackeln  (void);						// Screen wackeln lassen		
void ShakeScreen	(float staerke);			// Angeben wie stark der Screen wackeln soll
void SetScreenShake (void);						// Geshakten Screen setzen bzw Matrix daf�r einstellen
void ExplodePlayer	(void);						// Spieler explodieren lassen
void StageClear		(bool PlaySong = true);		// Spieler aus dem Level abgehen lassen
void SummaryScreen	(void);						// Zusammenfassung des Levels
bool LoadConfig		(void);						// Konfigurationen laden
void SaveConfig		(void);						// Konfigurationen speichern
void CreateDefaultConfig	(void);				// DefaultKonfigurationen erstellen
bool DisplayLoadInfo(char Text[100]);			// Text beim Loading anzeigen
bool NewDemo		(char Filename[]);			// Neues Demo aufzeichnen
void RecordDemo		(void);						// Spielereingaben sichern
bool LoadDemo		(char Filename[]);			// Demo laden
void PlayDemo		(void);						// Demo laufen lassen
void EndDemo		(void);						// Demo anhalten
void LeaveGameLoop  (void);						// Game Loop verlassen
void ScrolltoPlayeAfterBoss(void);				// Der Name ist Programm
void ShowPissText	(void);
void ShowGameOver	(void);
char *convertText	(char *text);
PlayerClass* ChooseAim(void);

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern unsigned char		SpielZustand;
extern bool					NochKeinFullScreen;

//----- externe Variablen zum Screen-Wackeln lassen (zB bei SpinnenBombe)

extern int   MAX_LEVELS;
extern int	 NUMPLAYERS;
extern float FahrstuhlPos;							// Nur im Fahrstuhl Level
extern float WackelMaximum;							// Maximaler Screen-Wackel Ausschlag
extern float WackelValue;							// Aktueller Screen-Wackel Ausschlag
extern float WackelDir;								// Aktuelle Wackel-Richtung
extern float WackelSpeed;							// Aktuelle Wackel-Geschwindigkeit
extern float ScreenWinkel;							// in welchem zWinkel steht der Screen grad (f�r S�ulen, die das Level zum Kippen bringen)
extern float WarningCount;							// Counter, ob ein "Warning" angezeigt wird
extern bool	 RunningTutorial;
extern int	 options_Detail;
extern bool	 StopStageMusicAtStart;
extern bool	 UseForceFeedback;
extern bool	 JoystickFound;
extern bool  ShowSummary;
extern bool	 FlugsackFliesFree;
extern int	 DisplayHintNr;

extern const float ShockWaveX;
extern const float ShockWaveY;

extern char	StageReihenfolge[256][100];

#endif