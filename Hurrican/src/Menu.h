// Datei : Menu.h

// -------------------------------------------------------------------------------------- 
//
// Menu-Klasse f�r das 
// Hurrican Hauptmenu
//
// (c) 2002 J�rg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __Menu_h__
#define __Menu_h__

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "Globals.h"
#include "DX8Sprite.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define MAX_STARS		300							// Anzahl scrollender Sterne im Hintergrund
#define MAX_HIGHSCORES	10							// Anzahl der Highscore-Eintr�ge
#define MAX_SAVEGAMES	10							// Anzahl der Savegame Slots
#define MAX_COLORS		99 + MAX_HIGHSCORES			// Anzahl der Blink-Farben in der Highscore

// ----- Defines f�r die Menu-Zust�nde

#define MENUZUSTAND_MAINMENU	0
#define MENUZUSTAND_STARTGAME	1
#define MENUZUSTAND_VOLUMES		2
#define MENUZUSTAND_NEWGAME		6
#define MENUZUSTAND_PLAYERCOUNT	7
#define MENUZUSTAND_SELECTSKILL 8
#define MENUZUSTAND_LOADGAME	9
#define MENUZUSTAND_SAVEGAME	10
#define MENUZUSTAND_ENTERNAME	11
#define MENUZUSTAND_LANGUAGE	12
#define MENUZUSTAND_TASTEN		13
#define MENUZUSTAND_BUTTONS		14

// ----- Defines f�r die einzelnen Menu-Punkte

#define MENUPUNKT_STARTGAME		0
#define MENUPUNKT_CONTINUEGAME	1
#define MENUPUNKT_VOLUMES		2
#define MENUPUNKT_HIGHSCORES	3
#define MENUPUNKT_CREDITS		4
#define MENUPUNKT_END			5

#define MENUPUNKT_MUSIC			0
#define MENUPUNKT_SOUND			1
#define MENUPUNKT_BACK			2

// --------------------------------------------------------------------------------------
// Strukturen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Struktur eines Sternes im Hintergrund
// --------------------------------------------------------------------------------------

struct StarStruct
{
	float Count;			// Z�hler von 0 bis 2 PI
	float Abstand;			// Abstand zur Mitte
	int	  Ebene;			// Ebene (Helligkeit) = Drehgeschwindigkeit
};

// --------------------------------------------------------------------------------------
// Struktur zum Speichern eines Highscore Eintrages
// --------------------------------------------------------------------------------------

struct HighscoreStruct
{
	char		Name[30];									// Name
	long		Score;										// Punkte
	int			Stage;										// Level
	int			Skill;										// Schwierigkeitsgrad
	long		Pruefsumme;
};

// --------------------------------------------------------------------------------------
// Struktur zum Speichern und laden eines Savegames
// --------------------------------------------------------------------------------------

struct SavegameStruct
{	
	char	Name[40];					// Name des Savegames
	float	Energy[2];					// Energie
	float	Armour[2];					// Rad Energie
	float	Shield[2];					// Noch Schild ?
	float	AutoFire[2];				// Noch Dauerfeuer?
	float	RiesenShot[2];				// Noch Fettboller?
	int		Players;					// Playercount
	long	Score;						// Punkte des Spielers	
	int		Stage;						// Aktuelles Level
	int		Skill;						// 0 = easy, 1 = medium, 2 = hard
	int		NewStage;					// Neues Level
	int		CollectedDiamonds;			// Anzahl gesammelter Edelsteine
	int		Lives[2];					// Wieviele Leben noch ?
	int		SelectedWeapon[2];			// gew�hlte Waffe
	int		CurrentWeaponLevel[2][4];	// Aktuelles Level		 der Waffen 1-4
	int		CollectedPowerUps[2][4];	// Gesammelt Powerups    der Waffen 1-4
	int		NextWeaponLevel[2][4];		// N�chste   Waffenstufe der Waffen 1-4
	int		BlitzLength[2];				// L�nge des Blitzes
	int		PowerLines;					// Anzahl an Powerlines
	int		Grenades;					// Anzahl an Granaten
	int		SmartBombs;					// Anzahl an SmartBomben (Druckwellen)
	long    Pruefsumme;					// Pr�fsumme f�r die Savegames
};

// --------------------------------------------------------------------------------------
// Menu-Klasse
// --------------------------------------------------------------------------------------

class MenuClass
{
	private:
		int					Sprachgrafik;				// Welche Menugrafik verwenden ?
		float				xpos, ypos;					// Position  des Menus
		float				ScrollPos;					// Scrollposition des Hintergrundes
		int					BlinkOffset;				// Offset des Highscore Blinkens
		float				BlinkCounter;				// Offset des Blinkcounters
		int					CreditsOffset;				// Offset der Credits im StringArray
		float				CreditsPosition;			// Offset der Credits am Screen
		float				CreditsCounter;				// Counter der Credits
		bool				AuswahlPossible;			// Auswahl m�glich ?
		float				ShowLanguageInfoCounter;	// Wie lange wird das Fenster mit Infos zum Language-File angezeigt ?
		D3DCOLOR			HighscoreColors[MAX_COLORS];// Farbewerte der Highscore-Blink-Farben
		bool				PossibleKeys[2][MAX_AKTIONEN];

		StarStruct			Stars	  [MAX_STARS];		// Hintergrund-Sterne
		SavegameStruct		Savegames [MAX_SAVEGAMES];	// Die Savegames (f�r Load Game)

		DirectGraphicsSprite	MenuStar;				// Stern im Hintergrund
		DirectGraphicsSprite	MenuTitel;				// Hurrican Schriftzug
		DirectGraphicsSprite	MenuNebel;				// Nebel-Hintergrund des Menus
		DirectGraphicsSprite	MenuGfx;				// Grafiken f�r das Menu
		DirectGraphicsSprite	MenuKasten[2];
		DirectGraphicsSprite	Skills;

		void FillPossibleKeys(void);

	public:
		HighscoreStruct		Highscores[MAX_HIGHSCORES];	// Die Top-Twenty

		int					AktuellerPunkt;				// Aktuell gew�hlter Menupunkt
		int					AktuellerZustand;			// Aktueller Menu-Unterpunkt
		int					CurrentPlayer;				// F�rs Steuerung Konfigurieren		

		int					NewRank;					// Neuer Platz in der Highscore
		int					NewScore;					// Neue Punktezahl
		int					NewStage;					// Neue Stage
		int					NewSkill;					// Neuer Skill
		char				NewName[30];				// Neuer Highscore Name
		bool				locked;						// F�r Tasten / Button Konfiguration


		float				Rotation;					// Rotationsgrad
		int					RotationDir;				// Rotationsrichtung

		     MenuClass		 (void);					// Menu initialisieren und Grafiken laden
			~MenuClass		 (void);					// Menu freigeben

		void ShowMenuBack	 (void);					// Hintergrund des Menus
		void ShowMenu		 (void);					// Menu anzeigen
		void DoMenu		     (void);					// Alles machen, was da Menu betrifft =)
		void LoadSavegames	 (void);					// Savegame Structs mit Daten f�llen
		void ShowSavegames(int Highlight);				// Verf�gbare Savegames anzeigen
		void LoadHighscore	 (void);					// Highscoreliste laden
		void SaveHighscore	 (void);					// Highscoreliste speichern
		void ResetHighscore	 (void);					// Highscoreliste resetten
		void ShowLanguageInfo(void);					// Infos aus der Language file anzeigen
		void CheckForNewHighscore(void);
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern MenuClass			*pMenu;
extern char					*Credits[];
extern int					CreditsCount;

#endif