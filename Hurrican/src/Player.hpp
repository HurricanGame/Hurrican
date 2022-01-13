// Datei : Player.hpp

// --------------------------------------------------------------------------------------
//
// Funktionen für den Spieler
// Tasteneingaben bearbeiten und Spieler entsprechend animieren
// usw
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "DX8Graphics.hpp"
#include "DX8Input.hpp"
#include "DX8Sprite.hpp"
#include "GegnerClass.hpp"

#include <bitset>

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

constexpr float PLAYER_MOVESPEED = 20.0f;                     // Spieler Geschwindigkeit
constexpr float PLAYER_ANIMSPEED = 0.4f;                      // Geschwindigkeit der einzlenen Animationen
constexpr float PLAYER_MAXJUMP = 124.0f;                      // Höchster Sprung in Pixeln
constexpr float PLAYER_MAXJUMPSPEED = 43.0f;                  // Schnellste Bewegung beim Springen/Fallen
constexpr float PLAYER_JUMPADDSPEED = 12.5f;                  // Spring-Manipulation
constexpr float PLAYER_ICESPEED = (PLAYER_MOVESPEED / 5.0f);  // Geschwindigkeit auf dem Eis
constexpr float PLAYER_ICESSLOWDOWN = 0.995f;                 // Geschwindigkeitsänderung bei "Auslaufen" auf dem Eis

constexpr int PLAYER_MAXEXTRAS = 9;  // Maximale Anzahl an Granaten/Powerlines

// frames
constexpr int PLAYERANIM_IDLE = 0;
constexpr int PLAYERANIM_RAD_ENDE = 7;     // Anzah der Rad Animationsphasen
constexpr int PLAYERANIM_RUN_END = 18;     // Ende  der Lauf Animation
constexpr int PLAYERANIM_BLITZ_ENDE = 25;  // Ende   der Rundum-Blitz Animation
constexpr int PLAYERANIM_JUMP_END = 7;     // Ende  der Sprung Animation
constexpr int PLAYERANIM_PISSEN = 0;       // Pissen =)

constexpr float PLAYER_FLUGSACKSPEED = 4.0f;  // Speed beim Flugsack fliegen

// neue Frames
constexpr int FRAMES_IDLE = 19;
constexpr int FRAMES_RUN = 20;
constexpr int FRAMES_JUMP = 12;
constexpr int FRAMES_SURROUND = 36;
constexpr int FRAMES_DUCKEN = 4;

constexpr float PLAYER_RAD_SPEED = 0.5f;    // Geschwindigkeit der Rad Animation
constexpr float PLAYER_BLITZ_SPEED = 0.5f;  // Geschwindigkeit der Blitz-Animation
constexpr float PLAYER_BLITZ_START = 4.0f;  // Zeit die man benötigt um Blitzen zu können
constexpr float PLAYER_BEAM_MAX = 160.0f;   // Maximal aufladbare Beamstärke
constexpr float PLAYER_SHOTDELAY = 1.8f;    // Verzögerung beim normalen schiessen
constexpr float BLITZ_POWER = 4.0f;         // Was zieht der Blitz ab ?

//----- Definitionen der Aktionstasten

enum {
  AKTION_LINKS = 0,
  AKTION_RECHTS = 1,
  AKTION_DUCKEN = 2,
  AKTION_OBEN = 3,
  AKTION_UNTEN = 4,
  AKTION_JUMP = 5,
  AKTION_SHOOT = 6,
  AKTION_BLITZ = 7,
  AKTION_POWERLINE = 8,
  AKTION_GRANATE = 9,
  AKTION_SMARTBOMB = 10,
  AKTION_WAFFEN_CYCLE = 11,
  AKTION_WAFFE_SPREAD = 12,
  AKTION_WAFFE_LASER = 13,
  AKTION_WAFFE_BOUNCE = 14,
  MAX_AKTIONEN = 15
};

constexpr int NUM_COOL_LIGHTNINGS = 18;  // Anzahl der Blitze, aus denen der neue coole Blitz besteht

//----- Definitionen der "Handlungen"

enum class PlayerActionEnum {
  STEHEN,
  SCHIESSEN,
  SCHIESSEN_O,
  SCHIESSEN_RO,
  SCHIESSEN_LO,
//  SCHIESSEN_RU,
//  SCHIESSEN_LU,
  LAUFEN,
  SPRINGEN,
  DUCKEN,
  BLITZEN,      // Rundum-Blitz
  RADELN,       // Radeln =)
  RADELN_FALL,  // Als Rad fallen
  SACKREITEN,   // Auf dem FlugSack rumreiten
  DREHEN,       // Auf dem FlugSack umdrehen
  BEAMLADEN,    // Osama Beam (auf)laden ;)
  PISSEN,       // Der Spieler hat zu lange gewartet, und jetzt muss der arme Hurri übelst strullen
                // (Auch als "Charles Bronson"-Animation bekannt)
  SURFEN,       // Spieler surft auf Gegner
  SURFENCROUCH, // Ducken beim surfen
  SURFENJUMP,   // Springen beim surfen
  EXPLODIEREN,  // Hurri explodiert
  TOT           // Hurri ist kaputt
};

// Skill level
enum {
  SKILL_EASY = 0,
  SKILL_MEDIUM,
  SKILL_HARD,
  SKILL_HURRICAN
};

// --------------------------------------------------------------------------------------
// Klasse für den Spieler
// --------------------------------------------------------------------------------------

class PlayerClass {
  private:
    uint32_t bl, br, bo, bu;  // Blockwerte in diesem Frame, damit wir nicht jedesmal
    // die Funktion aufrufen müssen, sondern nur EINMAL pro Frame
    VERTEX2D v1, v2, v3, v4;                                   // Vertices zum Blitz rendern
    VERTEX2D TriangleStrip[4];                                 // Strip für ein Sprite
    VERTEX2D strahlen[NUM_COOL_LIGHTNINGS + 1][(30) * 2 + 1];  // Für den Blitz
    float AnimCount;                                           // Wann ist nächste Animation ?
    float BlitzStart;                                          // Wie lange braucht man zum Blitzen ?
    float FlameTime;                                           // Schussflamme zu sehen ?
    float AustrittX, AustrittY;                                // Austritspunkt des Schusses
    int AustrittAnim;
    int FlameAnim;      // Welche Schuss Flamme ? (Zufall)
    int randomwert;     // aktueller srand wert
    float changecount;  // zähler, wann die blitze sich ändern
    bool weaponswitchlock;
    void checkWeaponSwitch();

    float DamageCounter;        // Wie lange bei Schaden blinken ?
    int PlayerNumber;    // DKS - Added this that indicates whether this
                         // player class is for player 1 (0) or 2 (1)
  public:
    bool GegnerDran;
    bool AlreadyDrawn;
    int SoundOff;
    std::bitset<MAX_AKTIONEN> Aktion;  // Array mit Tasten für Aktionen
    int AktionKeyboard[MAX_AKTIONEN];  // Array mit Tasten  für Aktionen
    int AktionJoystick[MAX_AKTIONEN];  // Array mit Buttons für Aktionen
    bool Walk_UseAxxis;                // true = achsen / false = coolie hat
    bool Look_UseAxxis;                // true = achsen / false = coolie hat

    bool CanBeDamaged;  // Kann schaden nehmen? (Nicht nach LevelEnde etc)

    bool GodMode;     // Unverwundbar ?
    bool WheelMode;   // Unendlich Rad ?
    bool WasDamaged;  // verwundet diesen Frame?

    RECT_struct CollideRect;           // Rechteck für die Kollision
    GegnerClass *AufPlattform;  // Auf einem Lift etc ?
    bool InLiquid;              // Spieler in Einer Flüssigkeit?
    bool JumpPossible;          // Kann man springen ?
    bool JumpedOnSurfboard;     // Das Surfbrett geht anfangs EINmal tierisch ab
    bool PowerLinePossible;     // Kann man eine Powerline schiessen ?
    bool PunisherActive;        // Ist der Punisher aktiv?
    bool WalkLock;              // Beim Stehen schon diagonal geschossen? Dann muss der Spieler stehenbleiben
    int AnimPhase;              // Aktuelle Animationsphase
    int AnimEnde;               // Ende der Animation
    float GameOverTimer;        // Timer für GameOver ?
    float ShotDelay;            // Schuss Verzögerung
    float AutoFireCount;        // Wieviel 3er Autofire noch ?
    float AutoFireExtra;        // Wieviel Sekunde Autofire Extra noch ?
    float RiesenShotExtra;      // Spieler hat grad die Mega riesen Power Schüsse ?
    float xposold, yposold;     // Position vorherigen Frame
    float xpos, ypos;           // Position des Spielers
    float xspeed, yspeed;       // Aktuelle Spieler-Geschwindigkeit
    float xadd, yadd;           // Für das weiche fliegen beim "FlugSack-Reiten" :P
    float BeamX, BeamY;         // Position der Waffenmündung, für die Partikel beim Beam laden
    float BeamCount;            // Counter für Auflad-Partikel
    float AutoScrollspeed;      // Mit welcher Speed scrollen ?
    float JumpySave;            // Speichern der Koordinaten, falls
    float JumpxSave;            // der Spieler wo runterfällt
    float JumpStart;            // ypos des Startpunktes beim Sprung
    float JumpAdd;              // Dazuzählen beim wieder runterfallen
    float SmokeCount;           // Counter für das Rauchen beim FlugSack-Fliegen
    float BlitzWinkel;          // Aktuelle Richtung des Blitzes
    float BlitzCount;           // Zähler für die Blitzanimation
    int BlitzAnim;              // Aktuelle Blitzanimation
    PlayerActionEnum Handlung;  // Was macht der Spieler gerade ?
    DirectionEnum Blickrichtung; // Wohin kuckt der Spieler ?
    float Energy;               // Energie
    float Armour;               // Rüstung
    float Shield;               // Noch Schild ?
    float BlinkCounter;         // Spieler leuchtet auf ?
    int BlinkColor;             // In welcher Farbe? 0 = keine, 1 = rot, 2 = grün, 3 = blau, 4 = hellblau
    D3DCOLOR CurrentColor;      // In welcher Farbe?
    long Score;                 // Punkte des Spielers
    int CollectedDiamonds;      // Anzahl gesammelter Edelsteine
    int DiamondsThisLevel;      // Anzahl gesammelter Edelsteine in diesem Level
    int DiamondsFullGame;       // Anzahl gesammelter Edelsteine im ganzen Spiel
    int LivesThisLevel;         // Anzahl gesammelter OneUps in diesem Level
    int LivesFullGame;          // Anzahl gesammelter OneUps im ganzen Spiel
    int BlocksThisLevel;        // Anzahl gefundener Powerblöcke in diesem Level
    int BlocksFullGame;         // Anzahl gefundener Powerblöcke im ganzen Spiel
    int Lives;                  // Wieviele Leben noch ?
    int SelectedWeapon;         // gewählte Waffe
    int CurrentWeaponLevel
        [4];               // Aktuelles Level		 der Waffen 1-4
                           //		int						CollectedPowerUps[4];	// Gesammelt Powerups    der Waffen 1-4
                           //		int						NextWeaponLevel[4];		// Nächste   Waffenstufe der Waffen 1-4
    int BlitzLength;       // Länge des Blitzes
    int PowerLines;        // Anzahl an Powerlines
    int Grenades;          // Anzahl Granaten
    int SmartBombs;        // Anzahl Smart Bombs
    int SecretThisLevel;   // Anzahl Secrets im Level
    int SecretFullGame;    // Anzahl Secrets im ganzen Spiel
    float BronsonCounter;  // Counter, wann der Hurri anfängt zu pullern =)
    int FesteAktion;       // Was soll der Spieler von alleine Machen?
    bool DoFesteAktion;
    bool StageClearRunning;
    int JoystickIndex;       // Welcher Joystick wird benutzt?
    float JoystickSchwelle;  // Schwellenwert für Analog Stick
    int ControlType;
    int JoystickMode;
    float ExplodingTimer;

    // DKS - Moved the player sprites into the class (they were static globals and
    //      therefore we had no control over the order of their construction/
    //      deconstruction)
    bool SpritesLoaded;  // DKS - Added this to indicate if all the
                         // textures below are loaded yet. Player 2's
                         // textures get loaded on-demand.
    DirectGraphicsSprite PlayerBlitz;
    DirectGraphicsSprite PlayerCrouch;
    DirectGraphicsSprite PlayerKucken;
    DirectGraphicsSprite PlayerIdle;
    DirectGraphicsSprite PlayerIdle2;
    DirectGraphicsSprite PlayerJump;
    DirectGraphicsSprite PlayerJumpUp;
    DirectGraphicsSprite PlayerJumpDiagonal;
    DirectGraphicsSprite PlayerRun;
    // DirectGraphicsSprite	PlayerRunDiagonal[FRAMES_RUN];  //DKS - This was commented out in original list of sprites
    DirectGraphicsSprite PlayerDiagonal;
    DirectGraphicsSprite PlayerOben;
    // DKS - This appears never to have been implemented (no image for it) so disabled it:
    // DirectGraphicsSprite	PlayerSurf;
    DirectGraphicsSprite PlayerPiss;  // Grafiken für das Pissen, wenn man lange nix macht =)
    DirectGraphicsSprite PlayerRide;  // Grafiken für das Reiten auf dem FlugSack
    DirectGraphicsSprite PlayerRad;   // Grafiken für das Rad

    PlayerClass();             // Spieler initlaisieren
    ~PlayerClass() = default;  // Spieler freigeben
    // DKS - Added paramter to specify if the player being initialized is player 1 (0) or player 2 (1):
    void InitPlayer(int player_num);                           // Auf neues Spiel vorbereiten
    void InitNewLevel();                                       // Auf neues Level vorbereiten
    bool GetPlayerInput();                                     // Eingabegeräte abfragen
    void PlayerShoot();                                        // Spieler schiesst
    void PlayerGrenadeShoot();                                 // Granate ablassen
    void AnimatePlayer();                                      // Spieler animieren
    void MovePlayer();                                         // Spieler mit aktueller Speed bewegen
    bool DrawPlayer(bool leuchten, bool farbe);                // Spieler zeichnen
    bool DoLightning();                                        // Blitz animieren und zeichnen
    void DrawNormalLightning(int DrawLength);                  // Den normalen Blitz rendern
    void DrawCoolLightning(int DrawLength, float mul = 1.0f);  // Den neuen Blitz rendern =)
    bool LoadBeam();                                           // Beam aufladen
    void DamagePlayer(float amount,                            // Spieler erleidet Schaden
                      bool Override = false);
    void CenterLevel();  // Level auf Spieler zentrieren

    // Player Move Funktionen

    void runExplode();          // spieler explodiert gerade
    void checkShoot();          // Spieler will schiessen?
    void handleAutoFire();      // Autofire abhandeln
    void DoStuffWhenDamaged();  // Funken und Rauch, wenn der Spieler wenig Energie hat
    void DoPlattformStuff();    // Auf einer Plattform bewegen
    void CheckForExplode();     // Keine Energy mehr? Dann explodieren
    void CalcWeaponLevels();
    void CalcFlamePos();        // Position der Flamme berechnen
    void CalcAustrittsPunkt();  // Austrittspunkt des Schusses berechnen
    bool CheckLevelExit() const; // Spieler im Ausgang?
    void RunPlayerExit();       // Spieler läuft grad ins Exit
    void PlayerInExit();        // Spieler aus dem Level rausgelaufen?
    bool Riding() const;        // Alle Spieler auf einem Flugsack?
    void ScrollFlugsack();      // Im Flugsacklevel scrollen?

    // DKS - Player sprites are no longer static globals, but instead class member vars:
    //      This function will load the sprites specific to each player based on
    //      value of new member var PlayerNumber.
    void LoadSprites();
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

// DKS - PlayerClass array is now static, not dynamically-allocated:
// extern PlayerClass			*pPlayer[2];
extern PlayerClass Player[2];

extern int Stage;     // Aktuelles Level
extern int NewStage;  // Neues Level
extern int Skill;     // 0 = easy, 1 = medium, 2 = hard, 3 = Hurrican

#endif
