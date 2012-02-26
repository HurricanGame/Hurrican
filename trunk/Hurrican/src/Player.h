// Datei : Player.h

// -------------------------------------------------------------------------------------- 
//
// Funktionen für den Spieler
// Tasteneingaben bearbeiten und Spieler entsprechend animieren
// usw
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __Player_h__
#define __Player_h__

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "DX8Graphics.h"
#include "DX8Input.h"
#include "DX8Sprite.h"
#include "GegnerClass.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define PLAYER_MOVESPEED		20.0f						// Spieler Geschwindigkeit
#define	PLAYER_ANIMSPEED		0.4f						// Geschwindigkeit der einzlenen Animationen
#define	PLAYER_MAXJUMP			124.0f						// Höchster Sprung in Pixeln
#define PLAYER_MAXJUMPSPEED		43.0f						// Schnellste Bewegung beim Springen/Fallen
#define PLAYER_JUMPADDSPEED		12.5f						// Spring-Manipulation
#define PLAYER_ICESPEED			(PLAYER_MOVESPEED / 5.0f)	// Geschwindigkeit auf dem Eis
#define PLAYER_ICESSLOWDOWN		0.995f						// Geschwindigkeitsänderung bei "Auslaufen" auf dem Eis

#define	PLAYER_MAXEXTRAS		9							// Maximale Anzahl an Granaten/Powerlines

// frames
#define PLAYERANIM_IDLE			0
#define PLAYERANIM_RAD_ENDE		7							// Anzah der Rad Animationsphasen
#define PLAYERANIM_RUN_END		18							// Ende  der Lauf Animation
#define PLAYERANIM_BLITZ_ENDE   25							// Ende   der Rundum-Blitz Animation
#define PLAYERANIM_JUMP_END		7							// Ende  der Sprung Animation
#define PLAYERANIM_PISSEN		0							// Pissen =)

#define PLAYER_FLUGSACKSPEED	4.0f						// Speed beim Flugsack fliegen

// neue Frames
#define FRAMES_IDLE				19
#define FRAMES_RUN				20
#define FRAMES_JUMP				12
#define FRAMES_SURROUND			36
#define FRAMES_DUCKEN			4
	
#define PLAYER_RAD_SPEED		0.5f						// Geschwindigkeit der Rad Animation
#define PLAYER_BLITZ_SPEED		0.5f						// Geschwindigkeit der Blitz-Animation
#define PLAYER_BLITZ_START		4.0f						// Zeit die man benötigt um Blitzen zu können
#define PLAYER_BEAM_MAX			160.0f						// Maximal aufladbare Beamstärke
#define PLAYER_SHOTDELAY		1.8f						// Verzögerung beim normalen schiessen
#define BLITZ_POWER				4.0f						// Was zieht der Blitz ab ?

//----- Definitionen der Aktionstasten

#define AKTION_LINKS			0
#define AKTION_RECHTS			1
#define AKTION_DUCKEN			2
#define AKTION_OBEN				3
#define AKTION_UNTEN			4
#define AKTION_JUMP				5
#define AKTION_SHOOT			6
#define AKTION_BLITZ			7
#define AKTION_POWERLINE		8
#define AKTION_GRANATE			9
#define AKTION_SMARTBOMB		10
#define AKTION_WAFFEN_CYCLE		11
#define AKTION_WAFFE_SPREAD		12
#define AKTION_WAFFE_LASER		13
#define AKTION_WAFFE_BOUNCE		14
#define MAX_AKTIONEN			15

#define NUM_COOL_LIGHTNINGS			18			// Anzahl der Blitze, aus denen der neue coole Blitz besteht


//----- Definitionen der Blickrichtungen

#define RECHTS  1
#define LINKS  -1

//----- Definitionen der "Handlungen"

#define STEHEN			0
#define SCHIESSEN		1
#define SCHIESSEN_O		2
#define SCHIESSEN_RO	3
#define SCHIESSEN_LO	4
//#define SCHIESSEN_RU	5
//#define SCHIESSEN_LU	6
#define LAUFEN			7
#define SPRINGEN		8
#define DUCKEN			9
#define BLITZEN			10				// Rundum-Blitz
#define RADELN			11				// Radeln =)
#define RADELN_FALL		12				// Als Rad fallen
#define SACKREITEN		13				// Auf dem FlugSack rumreiten
#define DREHEN			14				// Auf dem FlugSack umdrehen
#define BEAMLADEN		15				// Osama Beam (auf)laden ;)
#define PISSEN   		16				// Der Spieler hat zu lange gewartet, und jetzt muss der arme Hurri übelst strullen (Auch als "Charles Bronson"-Animation bekannt)
#define SURFEN   		17				// Spieler surft auf Gegner
#define SURFENCROUCH   	18				// Ducken beim surfen
#define SURFENJUMP   	19				// Springen beim surfen
#define EXPLODIEREN		20				// Hurri explodiert
#define TOT				21				// Hurri ist kaputt


// --------------------------------------------------------------------------------------
// Klasse für den Spieler
// --------------------------------------------------------------------------------------

class PlayerClass
{
	private:
		int						bl, br, bo, bu;			// Blockwerte in diesem Frame, damit wir nicht jedesmal
														// die Funktion aufrufen müssen, sondern nur EINMAL pro Frame
		VERTEX2D				v1, v2, v3, v4;			// Vertices zum Blitz rendern
		VERTEX2D				TriangleStrip[4];		// Strip für ein Sprite
		VERTEX2D				strahlen[NUM_COOL_LIGHTNINGS + 1][(30) * 2 + 1]; // Für den Blitz
		float					AnimCount;				// Wann ist nächste Animation ?
		float					BlitzStart;				// Wie lange braucht man zum Blitzen ?
		float					FlameTime;				// Schussflamme zu sehen ?
		float					AustrittX, AustrittY;	// Austritspunkt des Schusses
		int						AustrittAnim;
		int						FlameAnim;				// Welche Schuss Flamme ? (Zufall)				
		int						randomwert;				// aktueller srand wert
		float					changecount;			// zähler, wann die blitze sich ändern		
		bool					weaponswitchlock;
		void					checkWeaponSwitch(void);

	public:
		bool					GegnerDran;
		bool					AlreadyDrawn;
		int						SoundOff;
		bool					Aktion[MAX_AKTIONEN];			// Array mit Tasten für Aktionen
		int						AktionKeyboard[MAX_AKTIONEN];	// Array mit Tasten  für Aktionen
		int						AktionJoystick[MAX_AKTIONEN];	// Array mit Buttons für Aktionen
		bool					Walk_UseAxxis;					// true = achsen / false = coolie hat
		bool					Look_UseAxxis;					// true = achsen / false = coolie hat

		bool					CanBeDamaged;			// Kann schaden nehmen? (Nicht nach LevelEnde etc)

		bool					GodMode;				// Unverwundbar ?
		bool					WheelMode;				// Unendlich Rad ?
		bool					WasDamaged;				// verwundet diesen Frame?

		RECT					CollideRect;			// Rechteck für die Kollision
		GegnerClass				*AufPlattform;			// Auf einem Lift etc ?
		bool					InLiquid;				// Spieler in Einer Flüssigkeit?
		bool					JumpPossible;			// Kann man springen ?
		bool					JumpedOnSurfboard;		// Das Surfbrett geht anfangs EINmal tierisch ab
		bool					PowerLinePossible;		// Kann man eine Powerline schiessen ?
		bool					PunisherActive;			// Ist der Punisher aktiv?
		bool					WalkLock;				// Beim Stehen schon diagonal geschossen? Dann muss der Spieler stehenbleiben
		int						AnimPhase;				// Aktuelle Animationsphase
		int						AnimEnde;				// Ende der Animation
		float					GameOverTimer;			// Timer für GameOver ?
		float					ShotDelay;				// Schuss Verzögerung
		float					AutoFireCount;			// Wieviel 3er Autofire noch ?
		float					AutoFireExtra;			// Wieviel Sekunde Autofire Extra noch ?
		float					RiesenShotExtra;		// Spieler hat grad die Mega riesen Power Schüsse ?
		float					xposold, yposold;		// Position vorherigen Frame
		float					xpos,    ypos;			// Position des Spielers
		float					xspeed,  yspeed;		// Aktuelle Spieler-Geschwindigkeit
		float					xadd,    yadd;			// Für das weiche fliegen beim "FlugSack-Reiten" :P
		float					BeamX, BeamY;			// Position der Waffenmündung, für die Partikel beim Beam laden
		float					BeamCount;				// Counter für Auflad-Partikel
		float					AutoScrollspeed;		// Mit welcher Speed scrollen ?
		float					JumpySave;				// Speichern der Koordinaten, falls
		float					JumpxSave;				// der Spieler wo runterfällt
		float					JumpStart;				// ypos des Startpunktes beim Sprung
		float					JumpAdd;				// Dazuzählen beim wieder runterfallen
		float					SmokeCount;				// Counter für das Rauchen beim FlugSack-Fliegen
		float					BlitzWinkel;			// Aktuelle Richtung des Blitzes
		float					BlitzCount;				// Zähler für die Blitzanimation
		int						BlitzAnim;				// Aktuelle Blitzanimation
		int						Handlung;				// Was macht der Spieler gerade ?
		int						Blickrichtung;			// Wohin kuckt der Spieler ?
		float					Energy;					// Energie
		float					Armour;					// Rüstung
		float					Shield;					// Noch Schild ?
		float					DamageCounter;			// Wie lange bei Schaden blinken ?
		float					BlinkCounter;			// Spieler leuchtet auf ?
		int						BlinkColor;				// In welcher Farbe? 0 = keine, 1 = rot, 2 = grün, 3 = blau, 4 = hellblau
		D3DCOLOR				CurrentColor;			// In welcher Farbe?
		long					Score;					// Punkte des Spielers		
		int						CollectedDiamonds;		// Anzahl gesammelter Edelsteine
		int						DiamondsThisLevel;		// Anzahl gesammelter Edelsteine in diesem Level
		int						DiamondsFullGame;		// Anzahl gesammelter Edelsteine im ganzen Spiel
		int						LivesThisLevel;			// Anzahl gesammelter OneUps in diesem Level
		int						LivesFullGame;			// Anzahl gesammelter OneUps im ganzen Spiel
		int						BlocksThisLevel;		// Anzahl gefundener Powerblöcke in diesem Level
		int						BlocksFullGame;			// Anzahl gefundener Powerblöcke im ganzen Spiel
		int						Lives;					// Wieviele Leben noch ?
		int						SelectedWeapon;			// gewählte Waffe
		int						CurrentWeaponLevel[4];  // Aktuelles Level		 der Waffen 1-4
//		int						CollectedPowerUps[4];	// Gesammelt Powerups    der Waffen 1-4
//		int						NextWeaponLevel[4];		// Nächste   Waffenstufe der Waffen 1-4
		int						BlitzLength;			// Länge des Blitzes
		int						PowerLines;				// Anzahl an Powerlines
		int						Grenades;				// Anzahl Granaten
		int						SmartBombs;				// Anzahl Smart Bombs
		int						SecretThisLevel;		// Anzahl Secrets im Level
		int						SecretFullGame;			// Anzahl Secrets im ganzen Spiel
		float					BronsonCounter;			// Counter, wann der Hurri anfängt zu pullern =)
		int						FesteAktion;			// Was soll der Spieler von alleine Machen?
		bool					DoFesteAktion;
		bool					StageClearRunning;
		int						JoystickIndex;			// Welcher Joystick wird benutzt?
		float					JoystickSchwelle;		// Schwellenwert für Analog Stick
		int						ControlType;
		int						JoystickMode;
		float					ExplodingTimer;

		PlayerClass(void);								// Spieler initlaisieren
	   ~PlayerClass(void);								// Spieler freigeben
	   	void InitPlayer(void);							// Auf neues Spiel vorbereiten
		void InitNewLevel(void);						// Auf neues Level vorbereiten
		bool GetPlayerInput(void);						// Eingabegeräte abfragen
		void PlayerShoot  (void);						// Spieler schiesst
		void PlayerGrenadeShoot (void);					// Granate ablassen
		void AnimatePlayer(void);						// Spieler animieren
		void MovePlayer   (void);						// Spieler mit aktueller Speed bewegen
		bool DrawPlayer   (bool leuchten, bool farbe);	// Spieler zeichnen
		bool DoLightning  (void);						// Blitz animieren und zeichnen
		void DrawNormalLightning(int DrawLength);		// Den normalen Blitz rendern
		void DrawCoolLightning(int DrawLength, float mul = 1.0f);	// Den neuen Blitz rendern =)
		bool LoadBeam	  (void);						// Beam aufladen
		void DamagePlayer (float Ammount,				// Spieler erleidet Schaden
						   bool Override = false);		
		void CenterLevel  (void);						// Level auf Spieler zentrieren
		void PullItems	  (void);

		// Player Move Funktionen

		void runExplode			(void);					// spieler explodiert gerade
		void checkShoot			(void);					// Spieler will schiessen?
		void handleAutoFire		(void);					// Autofire abhandeln
		void DoStuffWhenDamaged (void);					// Funken und Rauch, wenn der Spieler wenig Energie hat
		void DoPlattformStuff   (void);					// Auf einer Plattform bewegen
		void CheckForExplode	(void);					// Keine Energy mehr? Dann explodieren
		void CalcWeaponLevels   (void);
		void CalcFlamePos		(void);					// Position der Flamme berechnen
		void CalcAustrittsPunkt (void);					// Austrittspunkt des Schusses berechnen		
		bool CheckLevelExit		(void);					// Spieler im Ausgang?
		void RunPlayerExit		(void);					// Spieler läuft grad ins Exit
		void PlayerInExit		(void);					// Spieler aus dem Level rausgelaufen?
		bool Riding				(void);					// Alle Spieler auf einem Flugsack?
		void ScrollFlugsack	    (void);					// Im Flugsacklevel scrollen?
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern PlayerClass			*pPlayer[2];
extern DirectGraphicsSprite	SchussFlamme[4];		// Grafiken für die SchussFlamme, für alle 3 Waffen
extern DirectGraphicsSprite	SchussFlammeFlare;		// Grafiken für das Leuchten der SchussFlamme
extern DirectGraphicsSprite	PlayerKucken[2];
extern DirectGraphicsSprite	PlayerBlitz[2];
extern DirectGraphicsSprite	PlayerCrouch[2];
extern DirectGraphicsSprite	PlayerIdle[2];
extern DirectGraphicsSprite	PlayerIdle2[2];
extern DirectGraphicsSprite	PlayerJump[2];
extern DirectGraphicsSprite	PlayerJumpUp[2];
extern DirectGraphicsSprite	PlayerJumpDiagonal[2];
extern DirectGraphicsSprite	PlayerRun[2];
extern DirectGraphicsSprite	PlayerDiagonal[2];
extern DirectGraphicsSprite	PlayerOben[2];
extern DirectGraphicsSprite	PlayerPiss[2];			// Grafiken für das Pissen, wenn man lange nix macht =)
extern DirectGraphicsSprite	PlayerRide[2];			// Grafiken für das Reiten auf dem FlugSack
extern DirectGraphicsSprite	PlayerRad[2];			// Grafiken für das Rad		
extern DirectGraphicsSprite	Blitzstrahl[4];			// Grafik   des Blitzes
extern DirectGraphicsSprite	Blitzflash[4];			// Grafik   des Leuchtens beim Blitz
extern DirectGraphicsSprite	BlitzTexture;			// Textur auf den Blitzen drauf
extern int Stage;									// Aktuelles Level
extern int NewStage;								// Neues Level
extern int	Skill;									// 0 = easy, 1 = medium, 2 = hard, 3 = Hurrican
		

#endif