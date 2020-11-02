// Datei : Menu.hpp

// --------------------------------------------------------------------------------------
//
// Menu-Klasse für das
// Hurrican Hauptmenu
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _MENU_HPP_
#define _MENU_HPP_

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "DX8Sprite.hpp"
#include "Globals.hpp"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define MAX_STARS 300                   // Anzahl scrollender Sterne im Hintergrund
#define MAX_HIGHSCORES 10               // Anzahl der Highscore-Einträge
#define MAX_SAVEGAMES 10                // Anzahl der Savegame Slots
#define MAX_COLORS 99 + MAX_HIGHSCORES  // Anzahl der Blink-Farben in der Highscore

// ----- Defines für die Menu-Zustände

#define MENUZUSTAND_MAINMENU 0
#define MENUZUSTAND_STARTGAME 1
#define MENUZUSTAND_VOLUMES 2
#define MENUZUSTAND_NEWGAME 6
#define MENUZUSTAND_PLAYERCOUNT 7
#define MENUZUSTAND_SELECTSKILL 8
#define MENUZUSTAND_LOADGAME 9
#define MENUZUSTAND_SAVEGAME 10
#define MENUZUSTAND_ENTERNAME 11
#define MENUZUSTAND_LANGUAGE 12
#define MENUZUSTAND_TASTEN 13
#define MENUZUSTAND_BUTTONS 14

// ----- Defines für die einzelnen Menu-Punkte

#define MENUPUNKT_STARTGAME 0
#define MENUPUNKT_CONTINUEGAME 1
#define MENUPUNKT_VOLUMES 2
#define MENUPUNKT_HIGHSCORES 3
#define MENUPUNKT_CREDITS 4
#define MENUPUNKT_END 5

// DKS - Added missing defines (In the correct order!) for VOLUMES sub-menu, a.k.a. PREFERENCES sub-menu
#define MENUPUNKT_VOLUMES_SOUND 0
#define MENUPUNKT_VOLUMES_MUSIC 1
#define MENUPUNKT_VOLUMES_TASTEN 2  // Controls  reconfiguration
#define MENUPUNKT_VOLUMES_LANGUAGE 3
#define MENUPUNKT_VOLUMES_DETAILS 4

// DKS - Added missing defines for NEWGAME sub-menu
#define MENUPUNKT_NEWGAME_TUTORIAL 0
#define MENUPUNKT_NEWGAME_STARTNEWGAME 1
#define MENUPUNKT_NEWGAME_LOADGAME 2

// DKS - Controls-reconfiguration menu: (Menu "tasten")
#define MENU_TASTEN_FORCEFEEDBACK_LINE 0
#define MENU_TASTEN_DEFAULTS_LINE 1
#define MENU_TASTEN_PLAYER_LINE 2
#define MENU_TASTEN_TYPE_LINE 3
#define MENU_TASTEN_MODE_LINE 4
#define MENU_TASTEN_SENSITIVITY_LINE 5
#define MENU_TASTEN_NUM_NON_CONTROLS 6  // Number of non-controls lines (before assignable controls begin)
#define MENU_TASTEN_NUM_CONTROLS 12     // Number of assignable-controls lines
#define MENU_TASTEN_NUM_LINES (MENU_TASTEN_NUM_CONTROLS + MENU_TASTEN_NUM_NON_CONTROLS)

// --------------------------------------------------------------------------------------
// Strukturen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Struktur eines Sternes im Hintergrund
// --------------------------------------------------------------------------------------

struct StarStruct {
    float Count;    // Zähler von 0 bis 2 PI
    float Abstand;  // Abstand zur Mitte
    int Ebene;      // Ebene (Helligkeit) = Drehgeschwindigkeit
};

// --------------------------------------------------------------------------------------
// Struktur zum Speichern eines Highscore Eintrages
// --------------------------------------------------------------------------------------

// DKS - Changed size of Name[] char array from 30 to 32 here, to reflect actual padding
//      that always occurred inside the struct. Also changed data types to explicitly
//      match their original binary representation to make loading/saving files robust.
struct HighscoreStruct {
    char Name[32];  // Name
    int32_t Score;  // Punkte
    int32_t Stage;  // Level
    int32_t Skill;  // Schwierigkeitsgrad
    int32_t Pruefsumme;
};

// --------------------------------------------------------------------------------------
// Struktur zum Speichern und laden eines Savegames
// --------------------------------------------------------------------------------------

// DKS - Made data types explicitly represent their binary size
//      to make loading/saving files more robust:
struct SavegameStruct {
    char Name[40];                     // Name des Savegames
    float Energy[2];                   // Energie
    float Armour[2];                   // Rad Energie
    float Shield[2];                   // Noch Schild ?
    float AutoFire[2];                 // Noch Dauerfeuer?
    float RiesenShot[2];               // Noch Fettboller?
    int32_t Players;                   // Playercount
    int32_t Score;                     // Punkte des Spielers
    int32_t Stage;                     // Aktuelles Level
    int32_t Skill;                     // 0 = easy, 1 = medium, 2 = hard
    int32_t NewStage;                  // Neues Level
    int32_t CollectedDiamonds;         // Anzahl gesammelter Edelsteine
    int32_t Lives[2];                  // Wieviele Leben noch ?
    int32_t SelectedWeapon[2];         // gewählte Waffe
    int32_t CurrentWeaponLevel[2][4];  // Aktuelles Level       der Waffen 1-4
    int32_t CollectedPowerUps[2][4];   // Gesammelt Powerups    der Waffen 1-4
    int32_t NextWeaponLevel[2][4];     // Nächste   Waffenstufe der Waffen 1-4
    int32_t BlitzLength[2];            // Länge des Blitzes
    int32_t PowerLines;                // Anzahl an Powerlines
    int32_t Grenades;                  // Anzahl an Granaten
    int32_t SmartBombs;                // Anzahl an SmartBomben (Druckwellen)
    int32_t Pruefsumme;                // Prüfsumme für die Savegames
};

// --------------------------------------------------------------------------------------
// Menu-Klasse
// --------------------------------------------------------------------------------------

class MenuClass {
  private:
    int Sprachgrafik;                      // Welche Menugrafik verwenden ?
    float xpos, ypos;                      // Position  des Menus
    float ScrollPos;                       // Scrollposition des Hintergrundes
    int BlinkOffset;                       // Offset des Highscore Blinkens
    float BlinkCounter;                    // Offset des Blinkcounters
    int CreditsOffset;                     // Offset der Credits im StringArray
    float CreditsPosition;                 // Offset der Credits am Screen
    float CreditsCounter;                  // Counter der Credits
    bool AuswahlPossible;                  // Auswahl möglich ?
    float ShowLanguageInfoCounter;         // Wie lange wird das Fenster mit Infos zum Language-File angezeigt ?
    D3DCOLOR HighscoreColors[MAX_COLORS];  // Farbewerte der Highscore-Blink-Farben
    float LoadingProgress;
    float LoadingItemsToLoad;
    int LoadingItemsLoaded;

    StarStruct Stars[MAX_STARS];              // Hintergrund-Sterne
    SavegameStruct Savegames[MAX_SAVEGAMES];  // Die Savegames (für Load Game)

    DirectGraphicsSprite MenuStar;   // Stern im Hintergrund
    DirectGraphicsSprite MenuTitel;  // Hurrican Schriftzug
    DirectGraphicsSprite MenuNebel;  // Nebel-Hintergrund des Menus
    DirectGraphicsSprite MenuGfx;    // Grafiken für das Menu
    DirectGraphicsSprite MenuKasten[2];
    DirectGraphicsSprite Skills;

  public:
    // DKS - These are now members here instead of being globals in Main.cpp:
    DirectGraphicsSprite LoadingScreen;  // Splash Screen im Fullscreen
    DirectGraphicsSprite LoadingBar;     // Fortschrittsbalken

    HighscoreStruct Highscores[MAX_HIGHSCORES];  // Die Top-Twenty

    int AktuellerPunkt;    // Aktuell gewählter Menupunkt
    int AktuellerZustand;  // Aktueller Menu-Unterpunkt
    int CurrentPlayer;     // Fürs Steuerung Konfigurieren

    int NewRank;                         // Neuer Platz in der Highscore
    int NewScore;                        // Neue Punktezahl
    int NewStage;                        // Neue Stage
    int NewSkill;                        // Neuer Skill
    char NewName[30];                    // Neuer Highscore Name
    bool control_reassignment_occuring;  // Für Tasten / Button Konfiguration

    float Rotation;   // Rotationsgrad
    int RotationDir;  // Rotationsrichtung

    MenuClass();   // Menu initialisieren und Grafiken laden
    ~MenuClass();  // Menu freigeben

    void ShowMenuBack();            // Hintergrund des Menus
    void ShowMenu();                // Menu anzeigen
    void DoMenu();                  // Alles machen, was da Menu betrifft =)
    void LoadSavegames();           // Savegame Structs mit Daten füllen
    void ShowSavegames(int Highlight);  // Verfügbare Savegames anzeigen
    void LoadHighscore();           // Highscoreliste laden
    void SaveHighscore();           // Highscoreliste speichern
    void ResetHighscore();          // Highscoreliste resetten
    void ShowLanguageInfo();        // Infos aus der Language file anzeigen
    void CheckForNewHighscore();
    void ResetProgressBar();
    void StartProgressBar(int items);
    void UpdateProgressBar();
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern MenuClass *pMenu;
extern const char *Credits[];
extern int CreditsCount;
// DKS - Made a version of the above credits text array meant for use on
//      low resolution displays that use text scaling. Long lines are split,
//      and there's less repeated blank lines.
extern const char *LowResCredits[];
extern int LowResCreditsCount;

#endif
