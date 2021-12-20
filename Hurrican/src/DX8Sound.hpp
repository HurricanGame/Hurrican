// Datei : DX8Sound.hpp

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

#ifndef _DX8SOUND_HPP_
#define _DX8SOUND_HPP_

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------
#include <string>
#include <vector>

#include "SDL_fmod.hpp"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

constexpr int MAX_SOUNDS = 140;  // Anzahl verschiedener Soundeffekte
constexpr int MAX_SONGS = 15;    // Anzahl verschiedener Musikstücke

// Definitionen der einzelnen Soundeffekte

enum {
  SOUND_SPREADSHOT = 0,     // Spreadshot
  SOUND_LASERSHOT = 1,      // Lasershot
  SOUND_BOUNCESHOT = 2,     // Bounceshot
  SOUND_EXPLOSION1 = 3,     // Explosion 1 leicht
  SOUND_EXPLOSION2 = 4,     // Explosion 2 fett
  SOUND_EXPLOSION3 = 5,     // Explosion 3 mittel
  SOUND_EXPLOSION4 = 6,     // Explosion 4 mittel
  SOUND_WALKERGIGGLE = 7,   // Walker wird zerstampft
  SOUND_CANON = 8,          // Kanonenschuss
  SOUND_CLICK = 9,          // Klicken
  SOUND_COLLECT = 10,       // Extra eingesammelt
  SOUND_SPREADHIT = 11,     // Spread Shot Hit
  SOUND_POWERLINE = 12,     // Powerline Sound
  SOUND_LANDEN = 13,        // Hurri landet nach Sprung
  SOUND_WATERIN = 14,       // Ins Wasser springen
  SOUND_WATEROUT = 15,      // Aus dem Wasser springen
  SOUND_FEUERFALLE = 16,    // Feuerfalle geht an
  SOUND_DIVE = 17,          // Tauchgeräusch
  SOUND_ABZUG = 18,         // Spieler verliert Energie
  SOUND_ABZUG_P2 = 19,      // Spieler verliert Energie
  SOUND_FUNKE = 20,         // Bruzel Sound für die Funken Partikel
  SOUND_FUNKE2 = 21,        // Bruzel Sound für die Funken Partikel
  SOUND_FUNKE3 = 22,        // Bruzel Sound für die Funken Partikel
  SOUND_FUNKE4 = 23,        // Bruzel Sound für die Funken Partikel
  SOUND_GRANATE = 24,       // Granate schiessen
  SOUND_STONEFALL = 25,     // Stein fällt runter
  SOUND_STONEEXPLODE = 26,  // Stein geht kaputt =)
  SOUND_ROCKET = 27,        // Rakete wird abgeschossen
  SOUND_PRESSE = 28,        // Stahlpresse geht los
  SOUND_AMMO = 29,          // Granate zb gesammelt
  SOUND_KOTZEN = 30,        // Bratklops kotzt
  SOUND_MADE = 31,          // Made Explodiert
  SOUND_DRONE = 32,         // Schuss der Drone
  SOUND_DROP = 33,          // Wassertropfen ins Wasser getropft =)
  SOUND_THUNDER = 34,       // Donner (zum Blitz)
  SOUND_UPGRADE = 35,       // Waffen Upgrade
  SOUND_COLUMN = 36,        // Säule fällt um
  SOUND_SWITCH = 37,        // Schalter wird umgelegt
  SOUND_DOOR = 38,          // Tür Schleuse fährt hoch
  SOUND_DOORSTOP = 39,      // Tür ist hochgefahren
  SOUND_SCHLEIM = 40,       // Tür ist hochgefahren
  SOUND_MESSAGE = 41,       // MessageBox
  SOUND_MUSHROOMJUMP = 42,  // Auf dem Pilzkopf abspringen
  SOUND_STEAM = 43,         // Dampfsound
  SOUND_STEAM2 = 44,        // Dampfsound2
  SOUND_HIT = 45,           // Gegner getroffen, Metall
  SOUND_HIT2 = 46,          // Gegner getroffen, Organisch
  SOUND_LILA = 47,          // Lila Laser
  SOUND_FIREBALL = 48,      // Was wohl?
  SOUND_TAKEOFF = 49,       // "irgendwas" startet
  SOUND_LAUGH = 50,         // Fiese Lache
  SOUND_STANDUP = 51,       // Skeletor steht auf
  SOUND_GATLING = 52,       // Skeletor Gun
  SOUND_GLASSBREAK = 53,    // Mutantenröhre explodiert
  SOUND_MUTANT = 54,        // Mutant springt / stirbt
  SOUND_HEART1 = 55,        // Heartbeat
  SOUND_HEART2 = 56,        // Heartbeat2 =)
  SOUND_SECRET = 57,
  SOUND_FLAMETHROWER = 58,
  SOUND_FLAMETHROWER2 = 59,

  // Voices bei eingesammelten Extras
  SOUND_VOICE_EXTRALIFE = 60,
  SOUND_VOICE_SPREAD = 61,
  SOUND_VOICE_LASER = 62,
  SOUND_VOICE_BOUNCE = 63,
  SOUND_VOICE_LIGHTNING = 64,
  SOUND_VOICE_SHIELD = 65,
  SOUND_VOICE_POWERUP = 66,
  SOUND_VOICE_WHEELPOWER = 67,
  SOUND_VOICE_POWERLINE = 68,
  SOUND_VOICE_GRENADE = 69,
  SOUND_VOICE_SMARTBOMB = 70,
  SOUND_VOICE_RAPIDFIRE = 71,
  SOUND_VOICE_SUPERSHOT = 72,

  // Endgegner Sounds
  SOUND_PHARAORAMM = 73,     // Pharao Kopf rammt die Wand
  SOUND_PHARAODIE = 74,      // Pharao Kopf stirb
  SOUND_SPIDERSCREAM = 75,   // Schrei der Spinne
  SOUND_SPIDERWALK = 76,     // Spinne Laufgeräusch
  SOUND_SPIDERLASER = 77,    // Spinne Riesen Laser auflade Geräusch
  SOUND_SPIDERGRENADE = 78,  // Riesenspinne schiesst Granate
  SOUND_BLITZHIT = 79,       // Evil Blitz trifft
  SOUND_BLITZHIT2 = 80,      // Evil Blitz trifft
  SOUND_BRATLASER = 81,      // Bratklops Laser
  SOUND_KLONG = 82,          // Metall Schädel beisst zu
  SOUND_GOLEMLOAD = 83,      // Ladesound vom Golemklops
  SOUND_GOLEMSHOT = 84,      // Schuss vom Golemklops

  SOUND_MARIO = 85,  // Mario Röhre im Tutorial Level

  // geloopte Sounds
  // der Blitzsound muss für beide Spieler geladen werden
  SOUND_BLITZSTART = 100,     // Blitzwaffe aufladen
  SOUND_BLITZSTART_P2 = 101,  // Blitzwaffe aufladen
  SOUND_BLITZENDE = 102,      // Blitzwaffe losgelassen
  SOUND_BLITZENDE_P2 = 103,   // Blitzwaffe losgelassen
  SOUND_BLITZ = 104,          // Blitzwaffe, wird geloopt
  SOUND_BLITZ_P2 = 105,       // Blitzwaffe, wird geloopt
  SOUND_BEAMLOAD = 106,
  SOUND_BEAMLOAD_P2 = 107,
  SOUND_BEAMLOAD2 = 108,
  SOUND_BEAMLOAD2_P2 = 109,
  SOUND_CHAIN = 110,

  // Sounds für Sound-Trigger
  //
  SOUND_TRIGGER_START = 120,

  SOUND_WASSERFALL = 120,  // Sound Trigger 0
  SOUND_DAMPF = 121,       // Sound Trigger 1
  SOUND_WIND = 122,        // Sound Trigger 2
};

// Definitionen der einzelnen Musik-Stücke
enum {
  MUSIC_STAGEMUSIC = 0,  // Level-Musik
  MUSIC_INTRO = 1,       // Intro-Mucke
  MUSIC_CREDITS = 2,     // Credits-Mucke
  MUSIC_MENU = 3,        // Menu-Mucke
  MUSIC_VORSPANN = 4,    // Vorspann Musik
  MUSIC_VORSPANN2 = 5,   // Vorspann Musik 2
  MUSIC_STAGECLEAR = 6,  // Stage Clear Mucke
  MUSIC_GAMEOVER = 7,    // Game  Over  Mucke
  MUSIC_LOADING = 8,     // Loading Mucke
  MUSIC_HIGHSCORE = 9,   // Highscore Mucke
  MUSIC_BOSS = 10,       // Boss Musik (in jedem Level anders)
  MUSIC_CRACKTRO = 11,   // Crack Intro Musik
  MUSIC_PUNISHER = 12,   // Musik für den Punisher
  MUSIC_OUTTRO = 13,     // Musik für Abspann
  MUSIC_FLUGSACK = 14    // Musik für Flugsack Modus
};

// Fadezustände für in Wave
enum class FadeModeEnum {
  NON,
  IN,
  OUT,
  FULL
};

//---------------------------------------------------------------------------------------
// Strukturen
//---------------------------------------------------------------------------------------

struct SOUNDMANAGER_PARAMETERS {
    int GlobalMusicVolume;  // Globale Musik-Lautstärke (0-100)
    int GlobalSoundVolume;  // Globale Sound-Lautstärke (0-100)
    int Mixrate;
    int MaxSoftwareChannels;
    int Flags;
};

// DKS - SDL_mixer lacks pitch-change feature that original game used through FMOD.
//      This macro masks-out the the many random frequency requests the game makes
//      throughout, which otherwise would result in unnecessary calls to rand()
//      and integer modulus.
#define PlayWave(vol, pan, freq, nr) PlayWave_SDL(vol, pan, nr)
#define PlayWave3D(x, y, freq, nr) PlayWave3D_SDL(x, y, nr)

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// Klassen-Deklaration (Song)
//---------------------------------------------------------------------------------------

// DKS - Added:
class SongClass {
  public:
    MUSIC_MODULE *data;         // MOD-Daten
    float vol;                  // Lautstärke
    float fade_speed;           // Aktuelle Fading Speed und Richtung
    int fade_end_vol;           // Fading Grenze
    bool pause_when_fade_ends;  // Pause vor/nach Fading ?

    // DKS - Added boolean to specify if the song plays looped or not:
    bool looped;

    // DKS - Added boolean to specify if a song is paused or not:
    bool paused;

    bool was_playing;  // Was this song playing before calling PausePlaying()?

    SongClass()
        : data(nullptr),
          vol(100),
          fade_speed(0),
          fade_end_vol(0),
          pause_when_fade_ends(false),
          looped(true),
          paused(false),
          was_playing(false) {}
    ~SongClass() = default;
};

//---------------------------------------------------------------------------------------
// Klassen-Deklaration (Soundeffect Wave Datei)
//---------------------------------------------------------------------------------------

// DKS - Added:
class SampleClass {
  public:
    SOUND_SAMPLE *data;
    bool looped;
    SampleClass() : data(nullptr), looped(false) {}
    ~SampleClass() = default;
};

// DKS - Added:
class ChannelClass {
  public:
    FadeModeEnum fade_mode;  // Fadet der Sound gerade ?
    uint8_t panning;
    uint8_t pending_pan;
    int16_t pending_vol;  // If -1, there is no pending volume/pan changes (from Trigger_SoundTrigger.cpp presumably)
    int16_t sound_num;    // If sound_num is -1, the channel is not in use.
    bool paused;
    bool is3D;  // Is this a channel whose panning/volume gets updated based on distance to player?
    float vol;
    int xpos, ypos;  // If this is a 3D channel, this stores its origin point

    ChannelClass()
        : fade_mode(FadeModeEnum::NON),
          panning(128),
          pending_pan(128),
          pending_vol(-1),
          sound_num(-1),
          paused(false),
          is3D(false),
          vol(100.0f),
          xpos(0),
          ypos(0) {}
};

//---------------------------------------------------------------------------------------
// Klassen-Deklaration (SoundManager)
//---------------------------------------------------------------------------------------

class SoundManagerClass {
  private:
    // DKS - Added a vector to that tracks channel usage:
    std::vector<ChannelClass> channels;

    SongClass songs[MAX_SONGS];
    SampleClass sounds[MAX_SOUNDS];

    bool InitFMOD(SOUNDMANAGER_PARAMETERS smpp);  // FMOD Init

    // DKS - Added these:
    void UpdateChannels();
    void Update3DChannel(int ch);
    void UpdateSongs();

  public:
    float g_music_vol;  // Globale Musik-Lautstärke (0-100)
    float g_sound_vol;  // Globale Sound-Lautstärke (0-100)

    int num_channels;        // Number of sound channels sound library actually gave us
    int most_channels_used;  // Anzahl benutzter Sound-Channels

    SoundManagerClass() : g_music_vol(100), g_sound_vol(100), num_channels(0), most_channels_used(0) {}

    // DKS - Class longer needs a destructor, it now uses Init() and Exit()
    //~SoundManagerClass	();								// Destruktor

    // DKS - Added these two to support a single, static global SoundManagerClass:
    void Init();
    void Exit();

    void SetVolumes(float sound_vol, float music_vol);  // neue global Volumes setzen

    // DKS - Added default parameter loop to specify if the song should loop:
    // bool LoadSong	(const char *Filename, int Nr);		// Song laden
    void LoadSong(const std::string &filename, int nr, bool loop = true);  // Song laden

    void PlaySong(int nr, bool resuming_paused_song);  // Song abspielen (Von Pause oder neu)
    void StopSong(int nr, bool paused);                // Song anhalten  (Pause oder ganz)

    // DKS - Added function to unload a song (game was leaving some songs loaded, like the
    //      Cracktro,Outtro and Intro music)
    void UnloadSong(int nr);  // Free a song's data (stopping it first)

    void StopSongs();  // Alle Songs anhalten
    void StopSounds();
    void SetSongVolume(int nr, float volume);  // Volume im Song setzen
    void SetAbsoluteSongVolume(int nr, float volume);

    // DKS - Added:
    void ResetAllSoundVolumes();

    void Update();  // Channel und Fades bearbeiten
    void FadeSong(int nr,
                  float speed,
                  int end,  // Song ein/aus faden
                  bool pause_when_fade_ends);
    void FadeWave(int nr, FadeModeEnum mode);  // Fade Mode
    void LoadWave(const std::string &filename,
                  int nr,  // Sound laden
                  bool looped);

    // via macro replacement of instances of PlayWave() PlayWave3D()
    // (This masks out many unnecessary calls to rand() and int modulus)
    int PlayWave_SDL(int vol, int pan, int nr);  // Sound spielen
    int PlayWave3D_SDL(int x, int y, int nr);    // Sound spielen abhängig von der Spieler
                                                 // position lauter oder leiser

    void StopWave(int nr);  // Wave anhalten

    // DKS - Added these:
    void UnloadWave(int nr);
    void PauseSounds();
    void UnpauseSounds();

    void PauseSongs();    // Alle spielenden Songs anhalten
    void UnpauseSongs();  // Alle angehaltenen Songs abspielen

    // DKS - Added all these, primarily to prevent the rest of the game from having to
    //      call the underlying sound API directly, as it was before.
    bool SongIsPlaying(int nr);
    bool SongIsPaused(int nr);
    void StopChannel(int ch);
    void SetChannelVolume(int ch, float new_vol);
    void SetChannelPanning(int ch, int pan);
    bool WaveIsPlaying(int nr);
    bool WaveIsPlayingOnChannel(int nr, int ch);
    int GetChannelWaveIsPlayingOn(int nr);
    void SetPendingChannelVolumeAndPanning(int ch, int new_vol, int new_pan);

    // DKS - Added #ifdef block around features SDL_mixer lacks:
};

// Externs
extern SoundManagerClass SoundManager;
#endif
