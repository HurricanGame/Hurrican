// Datei : DX8Sound.cpp

// --------------------------------------------------------------------------------------
//
// SoundManager
// zum Abspielen von Wave-Dateien und Musikstücken mit Hilfe der FMOD Library
//
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

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include <cstdio>
#include <filesystem>
namespace fs = std::filesystem;

#include "DX8Sound.hpp"
#include "Gameplay.hpp"
#include "Globals.hpp"
#include "Logdatei.hpp"
#include "Main.hpp"
#include "Player.hpp"
#include "Timer.hpp"

#ifdef USE_UNRARLIB
#include "unrarlib.h"
#endif

constexpr int MIXRATE = 48000;

//---------------------------------------------------------------------------------------
// Funktionsdefinitionen
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// Name			: InitFMOD
// Aufgabe		: FMOD Initialisieren
// Rückgabewert	: true bei Erfolg, false bei nicht-Erfolg
// Parameter	: keine
//---------------------------------------------------------------------------------------
bool SoundManagerClass::InitFMOD(SOUNDMANAGER_PARAMETERS smpp) {
    Protokoll << "Initializing SDL_mixer" << std::endl;
    bool success = SOUND_Init(smpp.Mixrate, smpp.MaxSoftwareChannels, smpp.Flags);

    return success;
}

// DKS - Added:
void SoundManagerClass::UpdateChannels() {
    // Only update 3D channels every other call, to limit CPU usage:
    static bool updated_3D_channels_last_call = false;

    for (int i = 0; i < num_channels; ++i) {
        if (channels[i].sound_num != -1) {
            if (!SOUND_IsPlaying(i) && !channels[i].paused) {
                channels[i].sound_num = -1;
                StopChannel(i);  // StopChannel() will reset the channel's values even if sound_num == -1
            } else {
                // If channel is a 3D sound, update its vol/pan based on player position:
                if (!updated_3D_channels_last_call && channels[i].is3D)
                    Update3DChannel(i);

                if (channels[i].pending_vol >= 0) {
                    if (channels[i].pending_vol > 100)
                        channels[i].pending_vol = 100;

                    if (channels[i].pending_vol == 0) {
                        StopChannel(i);
                    } else {
                        if (channels[i].vol != channels[i].pending_vol)
                            SetChannelVolume(i, channels[i].pending_vol);

                        if (channels[i].panning != channels[i].pending_pan)
                            SetChannelPanning(i, channels[i].pending_pan);
                    }

                    channels[i].pending_vol = -1;
                    channels[i].pending_pan = 128;
                }

                switch (channels[i].fade_mode) {
                    // einfaden
                    case FadeModeEnum::IN: {
                        // Volume erhöhen
                        float new_vol = channels[i].vol;

                        // DKS - In the place where this is used (mainly wind sound in 4th level,
                        //      the sounded faded much too fast compared to the song fade rate,
                        //      2.5 seems a better value than 30 here:)
                        //      See Trigger_FadeMusic.cpp and its fade-rate of just 1.0 for .hppe song)
                        // channels[i].vol += Timer.sync(30.0f);
                        new_vol += Timer.sync(2.5f);

                        // maximum? Dann fading anhalten
                        //
                        if (new_vol > 100.0f) {
                            new_vol = 100.0f;
                            channels[i].fade_mode = FadeModeEnum::NON;
                        }

                        // Lautstärke setzen
                        SetChannelVolume(i, new_vol);
                    } break;

                    // ausfaden
                    case FadeModeEnum::OUT: {
                        // Volume verringern

                        float new_vol = channels[i].vol;
                        // DKS - In the place where this is used (mainly wind sound in 4th level,
                        //      the sounded faded much too fast compared to the song fade rate,
                        //      2.5 seems a better value than 30 here:)
                        //      See Trigger_FadeMusic.cpp and its fade-rate of just 1.0 for .hppe song)
                        // new_vol -= Timer.sync(30.0f);
                        new_vol -= Timer.sync(2.5f);

                        // < 0 ? Dann anhalten
                        if (new_vol < 0.0f) {
                            StopChannel(i);
                        } else {
                            SetChannelVolume(i, new_vol);
                        }
                    } break;

                    default:
                        break;
                }
            }
        }
    }

    updated_3D_channels_last_call = !updated_3D_channels_last_call;
}

// DKS - Added:
void SoundManagerClass::Update3DChannel(int ch) {

    float xdiff = ((Player[0].xpos + 45) - channels[ch].xpos);
    float ydiff = ((Player[0].ypos + 45) - channels[ch].ypos);

    float Abstand = sqrtf((xdiff * xdiff) + (ydiff * ydiff));

    int vol = static_cast<int>(100.0f - Abstand / 6.0f);
    if (vol < 0)
        vol = 0;
    else {
        int pan;
        // Sound links oder rechts vom Spieler ?
        if (channels[ch].xpos < Player[0].xpos + 45) {
            pan = 128 - (100 - vol);
            if (pan < 0)
                pan = 0;
        } else {
            pan = 128 + (100 - vol);
            if (pan > 255)
                pan = 255;
        }

        if (channels[ch].vol != vol)
            SetChannelVolume(ch, vol);

        if (channels[ch].panning != pan)
            SetChannelPanning(ch, pan);
    }
}

// DKS - Added:
void SoundManagerClass::UpdateSongs() {
    // Songs durchgehen und bei Bedarf faden
    for (int i = 0; i < MAX_SONGS; ++i) {
        // Spielt der Song garnicht ? Dann brauchen wir ihn auch nicht zu bearbeiten =)

        // Fadet der Song grade ?
        if (songs[i].fade_speed != 0.0f) {
            if (SongIsPlaying(i)) {
                songs[i].vol += Timer.sync(songs[i].fade_speed);

                // Grenze Überschritten ?
                if ((songs[i].fade_speed > 0.0f && songs[i].vol >= songs[i].fade_end_vol) ||
                    (songs[i].fade_speed < 0.0f && songs[i].vol <= songs[i].fade_end_vol)) {
                    // Beim Anhalten des Songs ausschalten oder pausieren ?
                    if (songs[i].fade_speed < 0.0f)
                        StopSong(i, songs[i].pause_when_fade_ends);

                    songs[i].fade_speed = 0.0f;
                    songs[i].vol = songs[i].fade_end_vol;
                    songs[i].fade_end_vol = 100;
                    songs[i].pause_when_fade_ends = false;
                }
                SetSongVolume(i, songs[i].vol);
                // MUSIC_SetMasterVolume(songs[i].data, static_cast<int>(songs[i].vol * g_music_vol * (2.55f/100.0f)));
            } else {
                // If a song is fading and is now over, reset its values:
                songs[i].fade_speed = 0.0f;
                songs[i].fade_end_vol = 100;
                songs[i].paused = songs[i].pause_when_fade_ends;
                songs[i].pause_when_fade_ends = false;
            }
        } else if (SongIsPlaying(i)) {
#ifndef USE_OPENMPT
            // DKS - Added this, because when SDL_mixer loops a music file, at least in some versions or
            //      configurations, it resets the song's volume to a default value. So, we must
            //      set the volume continuously.
            SetSongVolume(i, songs[i].vol);
#endif  // !USE_OPENMPT
        }
    }
}

//---------------------------------------------------------------------------------------
// Klassen-Definition (SoundManager)
//---------------------------------------------------------------------------------------

// DKS - added
void SoundManagerClass::Init() {
    SOUNDMANAGER_PARAMETERS smpp;

    // DKS - 64 is a very high number of channels, reducing to something more sensible:
    // smpp.MaxSoftwareChannels	= 64;
    smpp.MaxSoftwareChannels = 24;

    smpp.Mixrate = MIXRATE;
    smpp.Flags = FSOUND_INIT_USEDEFAULTMIDISYNTH;

    if (InitFMOD(smpp)) {
        num_channels = SOUND_GetMaxChannels();
        Protokoll << "Requested " << smpp.MaxSoftwareChannels << " sound channels and got " << num_channels
                  << std::endl;
        channels.resize(num_channels);
    } else {
        Protokoll << "-> ERROR: Create Sound Manager failed !" << std::endl;
        GameRunning = false;
    }
}

// DKS - Added:
void SoundManagerClass::Exit() {
    Protokoll << "-> Shutting down Sound Manager" << std::endl;

    StopSongs();
    StopSounds();

    // Songs freigeben
    for (int i = 0; i < MAX_SONGS; ++i)
        UnloadSong(i);

    // Sounds freigeben
    for (int i = 0; i < MAX_SOUNDS; ++i)
        UnloadWave(i);

    // FSOUND beenden
    SOUND_Close();
}

//---------------------------------------------------------------------------------------
// Name			: SetVolumes
// Aufgabe		: neue globale Volumes setzen
// Rückgabewert	: keiner
// Parameter	: Sound		neue Sound-Lautstärke
//				  Musik		neue Musik-Lautstärke
//---------------------------------------------------------------------------------------

void SoundManagerClass::SetVolumes(float sound_vol, float music_vol) {
    if (sound_vol < 0.0f)
        sound_vol = 0.0f;
    else if (sound_vol > 100.0f)
        sound_vol = 100.0f;

    if (music_vol < 0.0f)
        music_vol = 0.0f;
    else if (music_vol > 100.0f)
        music_vol = 100.0f;

    g_sound_vol = sound_vol;
    g_music_vol = music_vol;

    // DKS - added to ensure all channels / songs playing reflect the new volume change:
    ResetAllSoundVolumes();
}

//---------------------------------------------------------------------------------------
// Name			: LoadSong
// Aufgabe		: Song laden
// Rückgabewert	: Liefert Integer Wert der Arraypos des Songs zurück, -1 bei Fehler
// Parameter	: Filename	Name des zu ladenden Songs
//---------------------------------------------------------------------------------------

// DKS - Added default parameter 'loop' to specify if the song should loop, and
//      converted to use strings:
void SoundManagerClass::LoadSong(const std::string &filename, int nr, bool loop /*=true*/) {
    if (songs[nr].data) {
        StopSong(nr, false);
        UnloadSong(nr);
    }

    bool fromrar = false;
    char *pData = nullptr;
    // DKS - Changed from unsigned int to unsigned long to fix unrarlib compilation error:
    unsigned long buf_size = 0;
    std::string fullpath;

    // Zuerst checken, ob sich der Song in einem MOD-Ordner befindet
    if (CommandLineParams.RunOwnLevelList) {
        fullpath =
            g_storage_ext + "/levels/" + std::string(CommandLineParams.OwnLevelList) + "/" + filename;
        if (fs::exists(fullpath) && fs::is_regular_file(fullpath))
            goto loadfile;
    }

    // Dann checken, ob sich das File im Standard Ordner befindet
    fullpath = g_storage_ext + "/data/music/" + filename;
    if (fs::exists(fullpath) && fs::is_regular_file(fullpath))
        goto loadfile;

#if defined(USE_UNRARLIB)
    // Auch nicht? Dann ist es hoffentlich im RAR file
    if (urarlib_get(&pData, &buf_size, filename.c_str(), RARFILENAME, convertText(RARFILEPASSWORD)) == false) {
        Protokoll << "\n-> Error loading song file " << filename << " from Archive !" << std::endl;
        return;
    } else
        fromrar = true;
#else
    Protokoll << "\n-> Error: Could not locate song file " << fullpath << std::endl;
    GameRunning = false;
    return;
#endif  // USE_UNRARLIB

loadfile:
    // DKS - Added loop boolean to specify if the song plays looped or not:
    songs[nr].looped = loop;

    if (fromrar) {
        songs[nr].data = MUSIC_LoadSongEx(pData, 0, buf_size, FSOUND_LOADMEMORY, nullptr, 0);
        free(pData);
    } else {
        songs[nr].data = MUSIC_LoadSong(fullpath.c_str());
    }

    // Fehler beim Laden ?
    //
    if (!songs[nr].data) {
        Protokoll << "\n-> NULL ptr returned from MUSIC_LoadSong() loading song file\n\t" << fullpath << std::endl;
        return;
    }

    Protokoll << "Loaded song file " << fullpath << std::endl;

    // Set some sensible defaults
    songs[nr].vol = 100;
    songs[nr].fade_speed = 0;
    songs[nr].fade_end_vol = 0;
    songs[nr].pause_when_fade_ends = false;
    songs[nr].paused = false;
    songs[nr].was_playing = false;
}

//---------------------------------------------------------------------------------------
// Name			: PlaySong
// Aufgabe		: Song abspielen
// Rückgabewert	: keiner
// Parameter	: nr		Array-Nummer des Songs
//				  resuming_paused_song      Neu starten oder Pause aufheben
//---------------------------------------------------------------------------------------

void SoundManagerClass::PlaySong(int nr, bool resuming_paused_song) {
    if (!songs[nr].data)
        return;

    songs[nr].paused = false;

    if (resuming_paused_song) {
        MUSIC_SetPaused(songs[nr].data, false);  // oder Pause aufheben ?
        SetSongVolume(nr, songs[nr].vol);
    } else {
        // DKS - Added looped parameter under SDL to specify if the song plays looped or not:
        MUSIC_PlaySong(songs[nr].data, songs[nr].looped);  // Von vorne abspielen ?
        SetSongVolume(nr, 100.0f);
    }
}

//---------------------------------------------------------------------------------------
// Name			: StopSong
// Aufgabe		: Song anhalten
// Rückgabewert	: false, wenn der Song nicht exisitert, sonst true
// Parameter	: Nr		Array-Nummer des Songs
//				  Paused	Ganz anhalten oder pausieren
//---------------------------------------------------------------------------------------

void SoundManagerClass::StopSong(int nr, bool paused) {
    if (!songs[nr].data)
        return;

    SetSongVolume(nr, songs[nr].vol);

    // DKS - Added check for SongIsPlaying:
    if (SongIsPlaying(nr)) {
        if (paused)
            MUSIC_SetPaused(songs[nr].data, true);  // oder nur pausieren ?
        else
            MUSIC_StopSong(songs[nr].data);  // Ganz anhalten ?
    }

    // DKS - Added boolean to specify if a song is paused or not:
    songs[nr].paused = paused;
}

// DKS - Added function to unload a song (game was leaving some songs loaded, like the
//      Cracktro and Intro music, credits music, etc)
void SoundManagerClass::UnloadSong(int nr) {
    if (songs[nr].data) {
        if (SongIsPlaying(nr))
            StopSong(nr, false);

        MUSIC_FreeSong(songs[nr].data);
        songs[nr].data = nullptr;
    }
}

//---------------------------------------------------------------------------------------
// Name			: StopAllSongs
// Aufgabe		: Alle gespielten Songs werden angehalten
// Rückgabewert	: keiner
//---------------------------------------------------------------------------------------

void SoundManagerClass::StopSongs() {
    for (int i = 0; i < MAX_SONGS; ++i)
        StopSong(i, false);
}

//---------------------------------------------------------------------------------------
// Name			: StopAllSounds
// Aufgabe		: Alle Sounds stoppen
// Rückgabewert	: keiner
//---------------------------------------------------------------------------------------

void SoundManagerClass::StopSounds() {
    // DKS - Now, we iterate over channels instead:
    // for (int i = 0; i < MAX_SOUNDS; i++)
    //    if (its_Sounds[i] != nullptr)
    //        StopWave(i);

    for (int i = 0; i < num_channels; ++i)
        StopChannel(i);
}

//---------------------------------------------------------------------------------------
// Name			: SetSongVolume
// Aufgabe		: Lautstärke eines Songs setzen
// Rückgabewert	: keiner
// Parameter	: Nr		Array-Nummer des Songs
//				  Volume	Zu setzende Lautstärke
//---------------------------------------------------------------------------------------

void SoundManagerClass::SetSongVolume(int nr, float volume) {
    if (!songs[nr].data)
        return;

    songs[nr].vol = volume;
    MUSIC_SetMasterVolume(songs[nr].data, static_cast<int>(volume * g_music_vol * (2.55f / 100.0f)));
}

//---------------------------------------------------------------------------------------
// Name			: SetAbsoluteSongVolume
// Aufgabe		: aboslute Lautstärke eines Songs setzen
// Rückgabewert	: keiner
// Parameter	: Nr		Array-Nummer des Songs
//				  Volume	Zu setzende Lautstärke
//---------------------------------------------------------------------------------------

void SoundManagerClass::SetAbsoluteSongVolume(int nr, float volume) {
    if (!songs[nr].data)
        return;

    songs[nr].vol = volume;
    MUSIC_SetMasterVolume(songs[nr].data, static_cast<int>(volume));
}

// DKS - Added:
void SoundManagerClass::ResetAllSoundVolumes() {
    for (int i = 0; i < MAX_SONGS; i++)
        SetSongVolume(i, 100.0f);

    for (int i = 0; i < num_channels; ++i)
        SetChannelVolume(i, channels[i].vol);
}

//---------------------------------------------------------------------------------------
// Name			: Update
// Aufgabe		: Songs bei Bedarf faden und benutzte Channels zählen usw
// Rückgabewert	: keiner
// Parameter	: keine
//---------------------------------------------------------------------------------------

void SoundManagerClass::Update() {
    UpdateChannels();
    UpdateSongs();

#ifndef NDEBUG
    // Anzahl benutzter Channels zählen
    //
    int channels_in_use = 0;
    for (int i = 0; i < num_channels; ++i)
        if (channels[i].sound_num != -1)
            channels_in_use++;

    if (channels_in_use > most_channels_used)
        most_channels_used = channels_in_use;
#endif
}

//---------------------------------------------------------------------------------------
// Name			: FadeSong
// Aufgabe		: Fadet einen Song ein/aus und hält danach an bzw pausiert
// Rückgabewert	: keiner
// Parameter	: Nr			Nummer des Songs
//				  Speed			Geschwindigkeit und Richtung des Fades
//				  End			Grenze des Fadens
//---------------------------------------------------------------------------------------
// DKS - Added parameter pause_when_fade_ends
void SoundManagerClass::FadeSong(int nr, float speed, int end, bool pause_when_fade_ends) {
    // Einfaden ? Dann ggf den Song erst mal starten, je nach Angabe
    // neu von Anfang an oder Pausierung aufheben

    // DKS - If speed of fade is positive, that indicates a song is fading back in and it
    //      must either be paused or restarted:
    if (speed > 0.0f && !SongIsPlaying(nr)) {
        PlaySong(nr, songs[nr].paused);
        SetSongVolume(nr, 1.0f);
    }

    songs[nr].fade_speed = speed;
    songs[nr].fade_end_vol = end;
    songs[nr].pause_when_fade_ends = (speed < 0.0f) ? pause_when_fade_ends : false;
}

//---------------------------------------------------------------------------------------
// Name			: FadeWave
// Aufgabe		: Wave ein/ausfadem
// Rückgabewert	: keiner
// Parameter	: Nr			Nummer des Waves
//				  Mode			FadeMode
//---------------------------------------------------------------------------------------
// DKS - altered extensively, it now fades waves of specified "nr" on all channels:
void SoundManagerClass::FadeWave(int nr, FadeModeEnum mode) {
    int channel = -1;

    if (mode == FadeModeEnum::IN) {
        // First, find if the sound is already playing
        for (int i = 0; i < num_channels; ++i) {
            if (channels[i].sound_num == nr)
                channel = i;
        }

        if (channel == -1) {
            // The sound is not yet playing, so start it with a volume of 1
            channel = PlayWave(1, 128, 11025, nr);
        }

        if (channel >= 0)
            channels[channel].fade_mode = mode;
    } else {
        // We are fading out a wave, so find any channels it's currently playing on
        //  and set their mode:
        for (int i = 0; i < num_channels; ++i) {
            if (channels[i].sound_num == nr)
                channels[i].fade_mode = mode;
        }
    }
}

// DKS - Altered extensively, added string support:
void SoundManagerClass::LoadWave(const std::string &filename, int nr, bool looped) {
    if (!GameRunning)
        return;

    bool fromrar = false;
    char *pData = nullptr;
    // DKS - Changed from unsigned int to unsigned long to fix unrarlib compilation error:
    unsigned long buf_size = 0;
    std::string fullpath;

    if (sounds[nr].data)
        UnloadWave(nr);

    // Zuerst checken, ob sich der Sound in einem MOD-Ordner befindet
    if (CommandLineParams.RunOwnLevelList) {
        // sprintf_s(Temp, "%s/levels/%s/%s", g_storage_ext, CommandLineParams.OwnLevelList, Filename);
        fullpath =
            g_storage_ext + "/levels/" + std::string(CommandLineParams.OwnLevelList) + "/" + filename;

        if (fs::exists(fullpath) && fs::is_regular_file(fullpath))
            goto loadfile;
    }

    fullpath = g_storage_ext + "/data/sfx/" + filename;
    if (fs::exists(fullpath) && fs::is_regular_file(fullpath))
        goto loadfile;

#if defined(USE_UNRARLIB)
    // Auch nicht? Dann ist es hoffentlich im RAR file
    if (urarlib_get(&pData, &buf_size, filename.c_str(), RARFILENAME, convertText(RARFILEPASSWORD)) == false) {
        Protokoll << "\n-> Error loading " << filename << " from Archive !" << std::endl;
        GameRunning = false;
        return;
    } else
        fromrar = true;
#else
    Protokoll << "\n-> Error: could not find WAV file " << fullpath << std::endl;
    GameRunning = false;
    return;
#endif  // USE_UNRARLIB

loadfile:

    if (fromrar) {
        sounds[nr].data = SOUND_Sample_Load(
            nr, pData, (looped ? FSOUND_LOOP_NORMAL : FSOUND_LOOP_OFF) | FSOUND_LOADMEMORY, 0, buf_size);
        free(pData);
    } else {
        sounds[nr].data =
            SOUND_Sample_Load(nr, fullpath.c_str(), (looped ? FSOUND_LOOP_NORMAL : FSOUND_LOOP_OFF), 0, 0);
    }

    // Fehler beim Laden ?
    if (!sounds[nr].data)
        Protokoll << "\n-> Error: NULL returned from SOUND_Sample_Load, loading " << filename << std::endl;

    sounds[nr].looped = looped;

    // DisplayLoadInfo(Buffer);
    std::string loadinfo_str = std::string(TextArray[TEXT::LADE_WAVE]) + filename +
                               std::string(TextArray[TEXT::LADEN_ERFOLGREICH]) + std::string("\n");
    DisplayLoadInfo(loadinfo_str.c_str());
}

// DKS - Altered extensively:
// NOTE: there are two versions here , one for DirectX/FMod that uses the
//       freq parameter, and another that doesn't take the freq
//       parameter, since SDL_mixer doesn't support pitch-changing.
//       Macro in DX8Sound.h converts PlayWave() calls in the game.
int SoundManagerClass::PlayWave_SDL(int vol, int pan, int nr) {
    // hört man den Sound überhaupt ?
    if (g_sound_vol == 0 || !sounds[nr].data)
        return -1;

    // Sound spielen
    // DKS - Added looped boolean parameter for SDL port's fmod wrapper, so that
    //       it no longer has to keep track of which sounds are looped:
    int channel = SOUND_PlaySound(FSOUND_FREE, sounds[nr].data, sounds[nr].looped);

    if (channel < 0) {
#ifndef NDEBUG
        Protokoll << "Warning: could not find free channel to play sound #" << nr << std::endl;
#endif
        return -1;
    } else if (channel >= num_channels) {
#ifndef NDEBUG
        Protokoll << "Warning: SOUND_PlaySound returned channel " << channel << ", >= num_channels (" << num_channels
                  << ")\n"
                  << std::endl;
#endif
        channels.resize(channel + 1);  // We should never have to do this if the sound library we're using
                                       //  numbers channels contiguously & starting at 0, but let's be safe.
        num_channels = channel + 1;
    }

    // Und Werte für den Channel, in dem er gespielt wird, setzen
    // DKS - SetFrequency not supported under SDL:

    channels[channel].paused = false;
    channels[channel].sound_num = nr;
    channels[channel].fade_mode = FadeModeEnum::NON;
    channels[channel].pending_vol = -1;
    channels[channel].pending_pan = 128;
    channels[channel].paused = false;
    channels[channel].is3D = false;  // This will be set to true after returning, if called by PlayWave3D()

    if (channels[channel].vol != vol)
        SetChannelVolume(channel, vol);

    if (channels[channel].panning != pan)
        SetChannelPanning(channel, pan);

    return channel;
}

//---------------------------------------------------------------------------------------
// Name			: PlayWave3D
// Aufgabe		: Spielt eine Wave Datei, je nach Spieler Abstand lauter oder leiser
//				  und setzt das panning entsprechend der Position der Geräuschquelle
// Rückgabewert	: true, wenn der Sound gespielt wurde, sonst false
// Parameter	: Nr des Sounds im Array
//---------------------------------------------------------------------------------------
// NOTE: there are two versions here , one for DirectX/FMod that uses the
//       freq parameter, and another that doesn't take the freq
//       parameter, since SDL_mixer doesn't support pitch-changing.
//       Macro in DX8Sound.h converts PlayWave3D() calls in the game.
int SoundManagerClass::PlayWave3D_SDL(int x, int y, int nr) {
    int channel = -1;

    // DKS - Functionality here also copied into Update3DChannel()

    float xdiff = ((Player[0].xpos + 45) - x);
    float ydiff = ((Player[0].ypos + 45) - y);

    // DKS - converted to float:
    float Abstand = sqrtf((xdiff * xdiff) + (ydiff * ydiff));

    int vol = static_cast<int>(100.0f - Abstand / 6.0f);
    if (vol < 0)
        vol = 0;
    else {
        int pan;
        // Sound links oder rechts vom Spieler ?
        if (x < Player[0].xpos + 45) {
            pan = 128 - (100 - vol);
            if (pan < 0)
                pan = 0;
        } else {
            pan = 128 + (100 - vol);
            if (pan > 255)
                pan = 255;
        }

        channel = PlayWave_SDL(vol, pan, nr);
    }

    if (channel != -1) {
        channels[channel].is3D = true;
        channels[channel].xpos = x;
        channels[channel].ypos = y;
    }

    return channel;
}

// --------------------------------------------------------------------------------------
// Wavedatei anhalten (falls sie geloopt wird zb)
// --------------------------------------------------------------------------------------
void SoundManagerClass::StopWave(int nr) {
    for (int i = 0; i < num_channels; ++i)
        if (channels[i].sound_num == nr)
            StopChannel(i);
}

// DKS - Added:
void SoundManagerClass::UnloadWave(int nr) {
    if (sounds[nr].data) {
        SOUND_Sample_Free(sounds[nr].data);
        sounds[nr].data = nullptr;
    }
}

// DKS - Added:
void SoundManagerClass::PauseSounds() {
    for (int i = 0; i < num_channels; ++i) {
        channels[i].paused = channels[i].sound_num != -1 && SOUND_IsPlaying(i);

        if (channels[i].paused)
            SOUND_SetPaused(i, true);
    }
}

// DKS - Added:
void SoundManagerClass::UnpauseSounds() {
    for (int i = 0; i < num_channels; ++i) {
        if (channels[i].paused) {
            // DKS - In case sound levels were adjusted after this channel had been paused:
            SOUND_SetVolume(i, static_cast<int>(g_sound_vol * channels[i].vol * (2.55f / 100.0f)));
            SOUND_SetPaused(i, false);
            channels[i].paused = false;
        }
    }
}

// --------------------------------------------------------------------------------------
// Alle Songs anhalten, die gerade spielen und diese merken
// --------------------------------------------------------------------------------------
void SoundManagerClass::PauseSongs() {
    for (int i = 0; i < MAX_SONGS; ++i) {
        songs[i].was_playing = !songs[i].paused && SongIsPlaying(i);

        if (songs[i].was_playing)
            StopSong(i, true);  // DKS - Pause the song (2nd param to StopSong() is true)
    }
}

// --------------------------------------------------------------------------------------
// Alle Songs wieder spielen, die vorher angehalten wurden
// --------------------------------------------------------------------------------------
void SoundManagerClass::UnpauseSongs() {
    for (int i = 0; i < MAX_SONGS; ++i)
        if (songs[i].was_playing)
            PlaySong(i, true);  // DKS - Unpause the song (2nd param to PlaySong() is true)
}

// DKS - Added new function to check if a song is playing. Before, many parts of the
//      game were calling MUSIC_IsPlaying() directly against SoundData, which would
//      segfault if music wasn't initialized or a specific song wasn't loaded and
//      SoundData was thus nullptr.
bool SoundManagerClass::SongIsPlaying(int nr) {
    return songs[nr].data && !songs[nr].paused && MUSIC_IsPlaying(songs[nr].data);
}

// DKS - Added:
bool SoundManagerClass::SongIsPaused(int nr) {
    return songs[nr].paused;
}

// DKS - Added:
void SoundManagerClass::StopChannel(int ch) {
    if (channels[ch].sound_num != -1) {
        SOUND_StopSound(ch);
        channels[ch].sound_num = -1;
    }

    channels[ch].fade_mode = FadeModeEnum::NON;
    channels[ch].pending_vol = -1;
    channels[ch].pending_pan = 128;
    channels[ch].paused = false;
    channels[ch].is3D = false;
}

// DKS - Added:
void SoundManagerClass::SetChannelVolume(int ch, float new_vol) {
    channels[ch].vol = new_vol;
    SOUND_SetVolume(ch, static_cast<int>(g_sound_vol * new_vol * (2.55f / 100.0f)));
}

// DKS - Added:
void SoundManagerClass::SetChannelPanning(int ch, int pan) {
    channels[ch].panning = pan;
    SOUND_SetPan(ch, pan);
}

// DKS - Added:
bool SoundManagerClass::WaveIsPlaying(int nr) const {
    for (int i = 0; i < num_channels; ++i)
        if (channels[i].sound_num == nr)
            return true;
    return false;
}

// DKS - Added, primarily to allow Trigger_Stampfstein to have its .hppain sound.
bool SoundManagerClass::WaveIsPlayingOnChannel(int nr, int ch) const {
    if (ch == -1 || nr == -1)
        return false;

    return channels[ch].sound_num == nr && SOUND_IsPlaying(ch);
}

// DKS - Added:
int SoundManagerClass::GetChannelWaveIsPlayingOn(int nr) const {
    int channel = -1;

    for (int i = 0; i < num_channels; ++i)
        if (channels[i].sound_num == nr)
            channel = i;

    return channel;
}

// DKS - There are often multiple sound triggers close to one another and they
//      interfere with one another. (An example being dozens of triggers leading
//      to a single waterfall, or several waterfalls being close together).
//      I added this function that will accept these multiple requests
//      and create an averaged result from all of them.
void SoundManagerClass::SetPendingChannelVolumeAndPanning(int ch, int new_vol, int new_pan) {
    static int num_pans = 0;
    static int accumulated_pan = 0;

    // pending_vol is set to -1 in UpdateChannels(), and remains so until this function
    //  gets called at least once:
    if (channels[ch].pending_vol < 0) {
        channels[ch].pending_vol = 0;

        // This is the first vol/pan request for this channel since channels were last updated,
        //  so reset the pan-request counter and any accumulated value:
        num_pans = 0;
        accumulated_pan = 0;
    }

    if (new_vol > channels[ch].pending_vol)
        channels[ch].pending_vol = new_vol;

    if (channels[ch].pending_vol > 0) {
        // If this pan request had a non-zero volume, average the total pans together with it:
        num_pans++;
        accumulated_pan += new_pan;
        channels[ch].pending_pan = static_cast<int>(static_cast<float>(accumulated_pan) / static_cast<float>(num_pans));
    }
}
