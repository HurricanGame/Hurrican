// Datei : DX8Sound.h

// -------------------------------------------------------------------------------------- 
//
// DirectSound Klasse
// zum Abspielen von Wave-Dateien
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __DX8Sound_h__
#define __DX8Sound_h__

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include <dsound.h>
#include "fmod.h"				
#include "fmod_errors.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define MAX_SOUNDS				140 					// Anzahl verschiedener Soundeffekte
#define MAX_SONGS				15						// Anzahl verschiedener Musikstücke

#define FREE					0						// Channel frei
#define PLAYING					1						// Channel spielt gerade

// Definitionen der einzelnen Soundeffekte

#define SOUND_SPREADSHOT		0						// Spreadshot
#define SOUND_LASERSHOT			1						// Lasershot
#define SOUND_BOUNCESHOT		2						// Bounceshot
#define SOUND_EXPLOSION1		3						// Explosion 1 leicht
#define SOUND_EXPLOSION2		4						// Explosion 2 fett
#define SOUND_EXPLOSION3		5						// Explosion 3 mittel
#define SOUND_EXPLOSION4		6						// Explosion 4 mittel
#define SOUND_WALKERGIGGLE		7						// Walker wird zerstampft
#define SOUND_CANON				8						// Kanonenschuss
#define SOUND_CLICK				9						// Klicken
#define SOUND_COLLECT			10						// Extra eingesammelt
#define SOUND_SPREADHIT			11						// Spread Shot Hit
#define SOUND_POWERLINE			12						// Powerline Sound
#define SOUND_LANDEN			13						// Hurri landet nach Sprung
#define SOUND_WATERIN			14						// Ins Wasser springen
#define SOUND_WATEROUT			15						// Aus dem Wasser springen
#define SOUND_FEUERFALLE		16						// Feuerfalle geht an
#define SOUND_DIVE				17						// Tauchgeräusch
#define SOUND_ABZUG				18						// Spieler verliert Energie
#define SOUND_ABZUG_P2			19						// Spieler verliert Energie
#define SOUND_FUNKE				20						// Bruzel Sound für die Funken Partikel
#define SOUND_FUNKE2			21						// Bruzel Sound für die Funken Partikel
#define SOUND_FUNKE3			22						// Bruzel Sound für die Funken Partikel
#define SOUND_FUNKE4			23						// Bruzel Sound für die Funken Partikel
#define SOUND_GRANATE			24						// Granate schiessen
#define SOUND_STONEFALL			25						// Stein fällt runter
#define SOUND_STONEEXPLODE		26						// Stein geht kaputt =)
#define SOUND_ROCKET			27						// Rakete wird abgeschossen
#define SOUND_PRESSE			28						// Stahlpresse geht los
#define SOUND_AMMO				29						// Granate zb gesammelt
#define SOUND_KOTZEN			30						// Bratklops kotzt
#define SOUND_MADE				31						// Made Explodiert
#define SOUND_DRONE				32						// Schuss der Drone
#define SOUND_DROP				33						// Wassertropfen ins Wasser getropft =)
#define SOUND_THUNDER			34						// Donner (zum Blitz)
#define SOUND_UPGRADE			35						// Waffen Upgrade
#define SOUND_COLUMN			36						// Säule fällt um
#define SOUND_SWITCH			37						// Schalter wird umgelegt
#define SOUND_DOOR				38						// Tür Schleuse fährt hoch
#define SOUND_DOORSTOP			39						// Tür ist hochgefahren
#define SOUND_SCHLEIM			40						// Tür ist hochgefahren
#define SOUND_MESSAGE			41						// MessageBox
#define SOUND_MUSHROOMJUMP		42						// Auf dem Pilzkopf abspringen
#define SOUND_STEAM				43						// Dampfsound
#define SOUND_STEAM2			44						// Dampfsound2
#define SOUND_HIT				45						// Gegner getroffen, Metall
#define SOUND_HIT2				46						// Gegner getroffen, Organisch
#define SOUND_LILA				47						// Lila Laser
#define SOUND_FIREBALL			48						// Was wohl?
#define SOUND_TAKEOFF			49						// "irgendwas" startet
#define SOUND_LAUGH				50						// Fiese Lache
#define SOUND_STANDUP			51						// Skeletor steht auf
#define SOUND_GATLING			52						// Skeletor Gun
#define SOUND_GLASSBREAK		53						// Mutantenröhre explodiert
#define SOUND_MUTANT			54						// Mutant springt / stirbt
#define SOUND_HEART1			55						// Heartbeat
#define SOUND_HEART2			56						// Heartbeat2 =)
#define SOUND_SECRET			57
#define SOUND_FLAMETHROWER		58
#define SOUND_FLAMETHROWER2		59

// Voices bei eingesammelten Extras
#define SOUND_VOICE_EXTRALIFE	60	
#define SOUND_VOICE_SPREAD		61						
#define SOUND_VOICE_LASER		62						
#define SOUND_VOICE_BOUNCE		63						
#define SOUND_VOICE_LIGHTNING	64						
#define SOUND_VOICE_SHIELD		65						
#define SOUND_VOICE_POWERUP		66						
#define SOUND_VOICE_WHEELPOWER	67					
#define SOUND_VOICE_POWERLINE	68
#define SOUND_VOICE_GRENADE		69						
#define SOUND_VOICE_SMARTBOMB	70						
#define SOUND_VOICE_RAPIDFIRE	71						
#define SOUND_VOICE_SUPERSHOT	72

// Endgegner Sounds
#define SOUND_PHARAORAMM		73						// Pharao Kopf rammt die Wand
#define SOUND_PHARAODIE			74						// Pharao Kopf stirb
#define SOUND_SPIDERSCREAM		75						// Schrei der Spinne
#define SOUND_SPIDERWALK		76						// Spinne Laufgeräusch
#define SOUND_SPIDERLASER		77						// Spinne Riesen Laser auflade Geräusch
#define SOUND_SPIDERGRENADE		78						// Riesenspinne schiesst Granate
#define SOUND_BLITZHIT			79						// Evil Blitz trifft
#define SOUND_BLITZHIT2			80						// Evil Blitz trifft
#define SOUND_BRATLASER			81						// Bratklops Laser
#define SOUND_KLONG				82						// Metall Schädel beisst zu
#define SOUND_GOLEMLOAD			83						// Ladesound vom Golemklops
#define SOUND_GOLEMSHOT			84						// Schuss vom Golemklops

#define SOUND_MARIO				85						// Mario Röhre im Tutorial Level


// geloopte Sounds
// der Blitzsound muss für beide Spieler geladen werden
#define SOUND_BLITZSTART		100						// Blitzwaffe aufladen
#define SOUND_BLITZSTART_P2		101						// Blitzwaffe aufladen
#define SOUND_BLITZENDE			102						// Blitzwaffe losgelassen
#define SOUND_BLITZENDE_P2		103						// Blitzwaffe losgelassen
#define SOUND_BLITZ				104						// Blitzwaffe, wird geloopt
#define SOUND_BLITZ_P2			105						// Blitzwaffe, wird geloopt
#define SOUND_BEAMLOAD			106
#define SOUND_BEAMLOAD_P2		107
#define SOUND_BEAMLOAD2			108
#define SOUND_BEAMLOAD2_P2		109
#define SOUND_CHAIN				110

// Sounds für Sound-Trigger
//
#define SOUND_TRIGGER_START		120

#define SOUND_WASSERFALL		120						// Sound Trigger 0
#define SOUND_DAMPF				121						// Sound Trigger 1
#define SOUND_WIND				122						// Sound Trigger 2

// Definitionen der einzelnen Musik-Stücke
#define MUSIC_STAGEMUSIC		0						// Level-Musik
#define MUSIC_INTRO				1						// Intro-Mucke
#define MUSIC_CREDITS			2						// Credits-Mucke
#define MUSIC_MENU				3						// Menu-Mucke
#define MUSIC_VORSPANN			4						// Vorspann Musik
#define MUSIC_VORSPANN2			5						// Vorspann Musik 2
#define MUSIC_STAGECLEAR		6						// Stage Clear Mucke
#define MUSIC_GAMEOVER			7						// Game  Over  Mucke
#define MUSIC_LOADING			8						// Loading Mucke
#define MUSIC_HIGHSCORE			9						// Highscore Mucke
#define MUSIC_BOSS				10						// Boss Musik (in jedem Level anders)
#define MUSIC_CRACKTRO			11						// Crack Intro Musik
#define MUSIC_PUNISHER			12						// Musik für den Punisher
#define MUSIC_OUTTRO			13						// Musik für Abspann
#define MUSIC_FLUGSACK			14						// Musik für Flugsack Modus

// Fadezustände für in Wave
#define FADEMODE_NON 0
#define FADEMODE_IN 1
#define FADEMODE_OUT 2
#define FADEMODE_FULL 3

//---------------------------------------------------------------------------------------
// Strukturen
//---------------------------------------------------------------------------------------

struct SOUNDMANAGER_PARAMETERS
{
	int		GlobalMusicVolume;		// Globale Musik-Lautstärke (0-100)
	int		GlobalSoundVolume;		// Globale Sound-Lautstärke (0-100)
	int		Mixrate;			
	int		MaxSoftwareChannels;
	int		Flags;
};

//---------------------------------------------------------------------------------------
// Funktionsdeklarationen
//---------------------------------------------------------------------------------------

char *GetFMODErrorString(int ErrorNr);

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// Klassen-Deklaration (Song)
//---------------------------------------------------------------------------------------

class CSong
{
	public :
		FMUSIC_MODULE	   *SongData;		// MOD-Daten
		float				Volume;			// Lautstärke
		float				FadingVolume;	// Aktuelle Fading Speed und Richtung
		int					FadingEnd;		// Fading Grenze
		bool				FadingPaused;	// Pause vor/nach Fading ?

				CSong();					// Konstruktor
		       ~CSong();					// Destruktor
		bool	Update(void);				// Songs faden
};

//---------------------------------------------------------------------------------------
// Klassen-Deklaration (Soundeffect Wave Datei)
//---------------------------------------------------------------------------------------

class CWave
{
	public :
		FSOUND_SAMPLE		*SoundData;		// Daten des Sounds
		int					Channel;		// Channel in dem es gerade gespielt wird
		bool				isPlaying;		// Spielt der Sound gerade ?
		bool				isLooped;
		int					FadeMode;		// Fadet der Sound gerade ?

				CWave();					// Konstruktor
			   ~CWave();					// Destruktor
	    bool	Update();					// Updaten und isPlaying checken
};

//---------------------------------------------------------------------------------------
// Klassen-Deklaration (SoundManager)
//---------------------------------------------------------------------------------------

class CSoundManager
{
	private :
		int		its_LoadedSongs;			// Anzahl geladener Songs
		int		its_LoadedSounds;			// Anzahl geladener Sounds
		signed char WasPlaying[MAX_SONGS];

	public :
		CSong 	*its_Songs [MAX_SONGS+1];	// Die Songs
		CWave	*its_Sounds[MAX_SOUNDS+1];	// Die Sounds
		float	its_GlobalMusicVolume;		// Globale Musik-Lautstärke (0-100)
		float	its_GlobalSoundVolume;		// Globale Sound-Lautstärke (0-100)
		int		CurrentSongNr;				// Aktueller Song

		bool	InitSuccessfull;
		int 	MaxChannels;				// Maximal nutzbare Channels
		int		ChannelsUsed;				// Anzahl benutzter Sound-Channels

		CSoundManager	();								// Konstruktor
		CSoundManager	(SOUNDMANAGER_PARAMETERS smpp);	// Überladener Konstruktor
	   ~CSoundManager	();								// Destruktor

		bool InitFMOD	(SOUNDMANAGER_PARAMETERS smpp);	// FMOD Init
		void SetVolumes (float Sound, float Musik);		// neue global Volumes setzen
		bool LoadSong	(char *Filename, int Nr);		// Song laden
		bool PlaySong	(int Nr, bool Paused);			// Song abspielen (Von Pause oder neu)
		bool StopSong	(int Nr, bool Paused);			// Song anhalten  (Pause oder ganz)
		void StopAllSongs	(bool Paused);				// Alle Songs anhalten
		void StopAllSounds  (void);
		void StopAllLoopedSounds(void);
		void SetSongVolume(int Nr, float Volume);		// Volume im Song setzen
		void SetAbsoluteSongVolume(int Nr, float Volume);
		void SetAllSongVolumes(void);					// Volume aller Songs setzen
		void Update		(void);							// Channel und Fades bearbeiten
		void FadeSong	(int Nr, float Speed, int End,	// Song ein/aus faden
						 bool Paused);	
		void FadeWave(int Nr, int Mode);				// Fade Mode
		bool LoadWave	(char *Filename, int Nr,		// Sound laden
						 bool looped);	
		bool PlayWave	(int Vol,  int Pan,				// Sound spielen
						 int Freq, int Nr);
		bool PlayWave3D	(int x,    int y, 				// Sound spielen abhängig von der Spieler
						 int Freq, int Nr);				// position lauter oder leiser
		void Update3D   (int x, int y, int Nr);			// 3D Sound updaten
		bool StopWave	(int Nr);						// Wave anhalten

		void PauseAllSongs(bool bPause);				// Alle Songs anhalten, wieder abspielen
		void PausePlaying(void);						// Alle spielenden Songs anhalten
		void PlayPaused(void);							// Alle angehaltenen Songs abspielen
};

//---------------------------------------------------------------------------------------
// Externals
//---------------------------------------------------------------------------------------

extern CSoundManager		*pSoundManager;

#endif