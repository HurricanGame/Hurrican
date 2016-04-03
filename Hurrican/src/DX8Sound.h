// Datei : DX8Sound.h

// --------------------------------------------------------------------------------------
//
// DirectSound Klasse
// zum Abspielen von Wave-Dateien
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------


/********************************************************************************
  DKS - IMPORTANT NOTE:
  I overhauled this and DX8Sound.h, cleaning up and adding class interfaces,
  fixing bugs, and improving SDL support throughout. The most fundamental
  change is that SoundManagerClass now monitors sound channels individually.
********************************************************************************/


#ifndef __DX8Sound_h__
#define __DX8Sound_h__

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------
#include <vector>
#include <string>

#if defined(PLATFORM_DIRECTX)
#include <dsound.h>
#include "fmod.h"
#include "fmod_errors.h"

#define MUSIC_MODULE            FMUSIC_MODULE
#define SOUND_SAMPLE            FSOUND_SAMPLE

#define MUSIC_PlaySong          FMUSIC_PlaySong
#define MUSIC_IsPlaying         FMUSIC_IsPlaying
#define MUSIC_FreeSong          FMUSIC_FreeSong
#define MUSIC_StopSong          FMUSIC_StopSong
#define MUSIC_SetPaused         FMUSIC_SetPaused
#define MUSIC_GetPaused         FMUSIC_GetPaused
#define MUSIC_SetMasterVolume   FMUSIC_SetMasterVolume
#define MUSIC_StopAllSongs      FMUSIC_StopAllSongs
#define MUSIC_LoadSong          FMUSIC_LoadSong
#define MUSIC_LoadSongEx        FMUSIC_LoadSongEx
#define MUSIC_IsFinished        FMUSIC_IsFinished

#define SOUND_Init              FSOUND_Init
#define SOUND_GetError          FSOUND_GetError
#define SOUND_GetMaxChannels    FSOUND_GetMaxChannels
#define SOUND_SetFrequency      FSOUND_SetFrequency
#define SOUND_SetPan            FSOUND_SetPan
#define SOUND_Close             FSOUND_Close
#define SOUND_PlaySound         FSOUND_PlaySound
#define SOUND_Sample_Free       FSOUND_Sample_Free
#define SOUND_IsPlaying         FSOUND_IsPlaying
#define SOUND_GetVolume         FSOUND_GetVolume
#define SOUND_SetVolume         FSOUND_SetVolume
#define SOUND_StopSound         FSOUND_StopSound

//DKS - Added:
#define SOUND_SetPaused         FSOUND_SetPaused
#define SOUND_GetPaused         FSOUND_GetPaused

#elif defined(PLATFORM_SDL)
#include "SDL_fmod.h"
#else
#error no sound system selected
#endif

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

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// Klassen-Deklaration (Song)
//---------------------------------------------------------------------------------------

//DKS - Added:
class SongClass
{
public:
    MUSIC_MODULE       *data;                  // MOD-Daten
    float               vol;                   // Lautstärke
    float               fade_speed;            // Aktuelle Fading Speed und Richtung
    int                 fade_end_vol;          // Fading Grenze
    bool                pause_when_fade_ends;  // Pause vor/nach Fading ?

    //DKS - Added boolean to specify if the song plays looped or not:
    bool                looped;

    //DKS - Added boolean to specify if a song is paused or not:
    bool                paused;

    bool                was_playing;    // Was this song playing before calling PausePlaying()?

    SongClass() : data(NULL), vol(100), fade_speed(0), fade_end_vol(0), pause_when_fade_ends(false), 
                  looped(true), paused(false), was_playing(false) {}
    ~SongClass() {}
};

//---------------------------------------------------------------------------------------
// Klassen-Deklaration (Soundeffect Wave Datei)
//---------------------------------------------------------------------------------------

//DKS - Added:
class SampleClass
{
public:
    SOUND_SAMPLE    *data;
    bool            looped;
    SampleClass() : data(NULL), looped(false) {}
    ~SampleClass() {}
};

//DKS - Added:
class ChannelClass
{
public:
    uint8_t sound_num;      // If sound_num is -1, the channel is not in use.
    uint8_t panning;
    uint8_t fade_mode;		// Fadet der Sound gerade ?
    uint8_t pending_pan;
    int16_t pending_vol;    // If -1, there is no pending volume/pan changes (from Trigger_SoundTrigger.cpp presumably)
    bool    paused;
    bool    is3D;           // Is this a channel whose panning/volume gets updated based on distance to player?
    float   vol;
    int     xpos, ypos;     // If this is a 3D channel, this stores its origin point

    ChannelClass() : sound_num(-1), panning(128), fade_mode(FADEMODE_NON),
                     pending_pan(128), pending_vol(-1), 
                     paused(false), is3D(false), vol(100.0f), xpos(0), ypos(0) {}
};

//---------------------------------------------------------------------------------------
// Klassen-Deklaration (SoundManager)
//---------------------------------------------------------------------------------------

class SoundManagerClass
{
private :
    //DKS - Added a vector to that tracks channel usage:
    std::vector<ChannelClass> channels;

    SongClass                 songs[MAX_SONGS];
    SampleClass               sounds[MAX_SOUNDS];

    bool InitFMOD(SOUNDMANAGER_PARAMETERS smpp);	// FMOD Init

    //DKS - Added these:
    void UpdateChannels();
    void Update3DChannel(int ch);
    void UpdateSongs();

public :
    float	g_music_vol;		// Globale Musik-Lautstärke (0-100)
    float	g_sound_vol;		// Globale Sound-Lautstärke (0-100)

    int 	num_channels;           // Number of sound channels sound library actually gave us
    int		most_channels_used;				// Anzahl benutzter Sound-Channels

    SoundManagerClass() : g_music_vol(100), g_sound_vol(100), num_channels(0), most_channels_used(0) {}

    //DKS - Class longer needs a destructor, it now uses Init() and Exit()
    //~SoundManagerClass	();								// Destruktor

    //DKS - Added these two to support a single, static global SoundManagerClass:
    void Init();
    void Exit();

    void SetVolumes (float sound_vol, float music_vol);		// neue global Volumes setzen

    //DKS - Added default parameter loop to specify if the song should loop:
    //bool LoadSong	(const char *Filename, int Nr);		// Song laden
    void LoadSong	(const std::string &filename, int nr, bool loop=true);		// Song laden

    void PlaySong	(int nr, bool resuming_paused_song);			// Song abspielen (Von Pause oder neu)
    void StopSong	(int nr, bool paused);			// Song anhalten  (Pause oder ganz)

    //DKS - Added function to unload a song (game was leaving some songs loaded, like the
    //      Cracktro,Outtro and Intro music)
    void UnloadSong(int nr);                        // Free a song's data (stopping it first)

    void StopSongs();                                   // Alle Songs anhalten
    void StopSounds();
    void SetSongVolume(int nr, float volume);           // Volume im Song setzen
    void SetAbsoluteSongVolume(int nr, float volume);

    //DKS - Added:
    void ResetAllSoundVolumes();

    void Update();                                      // Channel und Fades bearbeiten
    void FadeSong(int nr, float speed, int end,         // Song ein/aus faden
                  bool pause_when_fade_ends);
    void FadeWave(int nr, int mode);                    // Fade Mode
    void LoadWave(const std::string &filename, int nr,  // Sound laden
                  bool looped);

#if defined(PLATFORM_DIRECTX)
    int  PlayWave(int vol, int pan,                 // Sound spielen
                  int freq, int nr);
    int  PlayWave3D(int x, int y,                   // Sound spielen abhängig von der Spieler
                     int freq, int nr);             // position lauter oder leiser
#else   //SDL inline wrappers that drop the unsupported freq parameter, and
        // pass the params onto versions taking only the params actually used:
    int  PlayWave(int vol, int pan, int freq, int nr)
    {
        return PlayWave(vol, pan, nr);
    }

    int  PlayWave3D(int x, int y, int freq, int nr)
    {
        return PlayWave3D(x, y, nr);
    }

    int  PlayWave(int vol, int pan, int nr);        // Sound spielen
    int  PlayWave3D(int x, int y, int nr);          // Sound spielen abhängig von der Spieler
                                                    // position lauter oder leiser
#endif

    void StopWave(int nr);                          // Wave anhalten

    //DKS - Added these:
    void UnloadWave(int nr);
    void PauseSounds();
    void UnpauseSounds();

    void PauseSongs();                              // Alle spielenden Songs anhalten
    void UnpauseSongs();                            // Alle angehaltenen Songs abspielen

    //DKS - Added all these, primarily to prevent the rest of the game from having to
    //      call the underlying sound API directly, as it was before.
    bool SongIsPlaying(int nr);
    bool SongIsPaused(int nr);
    void StopChannel(int ch);
    void SetChannelVolume(int ch, float new_vol);
    void SetChannelPanning(int ch, int pan);
    bool WaveIsPlaying(int nr);
    bool WaveIsPlayingOnChannel(int nr, int ch);
    int  GetChannelWaveIsPlayingOn(int nr);
    void SetPendingChannelVolumeAndPanning(int ch, int new_vol, int new_pan);

    //DKS - added.. (Playing waves and varying freq's not supported under SDL, for now anyway)
#if defined(PLATFORM_DIRECTX)
    void SetWaveFrequency(int nr, int freq);
#endif

};

//Externs
extern SoundManagerClass SoundManager;
#endif
