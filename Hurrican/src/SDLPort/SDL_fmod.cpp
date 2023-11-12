/**
 *
 *  Copyright (C) 2011-2012 Scott R. Smith
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 *
 */

#include "SDL_fmod.hpp"
#include <cstring>
#include <iostream>
#if defined(USE_OPENMPT)
#  include <cmath>
#endif

static int g_allocated = 0;
static bool g_music_loops = true;
#if defined(USE_OPENMPT)
static int g_mixrate = 0;
#endif

static MUSIC_MODULE *g_music_current = nullptr;

#if defined(USE_OPENMPT)
    static void log_func( const char * message, void * userdata ) {}
    static int error_func( int error, void * userdata ) {
          (void)userdata;
          (void)error;
          return OPENMPT_ERROR_FUNC_RESULT_DEFAULT & ~OPENMPT_ERROR_FUNC_RESULT_LOG;
        }
    double getDeciBel(int volume) { return 20*log10(volume/100.); }
#endif

signed char SOUND_Init(int mixrate, int maxsoftwarechannels, unsigned int flags) {
    g_allocated = 0;

    int chunksize = 2048;
    if (Mix_OpenAudio(mixrate, MIX_DEFAULT_FORMAT, 2, chunksize) == -1) {
        std::cerr << "->ERROR: Mix_OpenAudio: " << Mix_GetError() << std::endl;
        return 0;
    } else {
        // DKS - Check what we actually received:
        int cur_freq = 0, cur_channels = 0;
        uint16_t cur_format = 0;
        Mix_QuerySpec(&cur_freq, &cur_format, &cur_channels);
        std::cout << "SDL_mixer initialized. Buffer size:" << chunksize << ", Freq:" << cur_freq
                  << ", Channels:" << cur_channels << std::endl;
        switch (cur_format) {
            case AUDIO_U8:
                std::cout << "Unsigned 8-bit samples " << std::endl;
                break;
            case AUDIO_S8:
                std::cout << "Signed 8-bit samples " << std::endl;
                break;
            case AUDIO_U16LSB:
                std::cout << "Unsigned 16-bit samples (little-endian) " << std::endl;
                break;
            case AUDIO_S16LSB:
                std::cout << "Signed 16-bit samples (little-endian) " << std::endl;
                break;
            case AUDIO_U16MSB:
                std::cout << "Unsigned 16-bit samples (big-endian) " << std::endl;
                break;
            case AUDIO_S16MSB:
                std::cout << "Signed 16-bit samples (big-endian) " << std::endl;
                break;
            default:
                std::cout << "Unrecognized SDL_mixer sample format. " << std::endl;
                break;
        }

#if defined(USE_OPENMPT)
        g_mixrate = cur_freq;
#endif
    }
    g_allocated = Mix_AllocateChannels(maxsoftwarechannels);

#if defined(USE_OPENMPT)
    std::cout << "Using libopenmpt for ImpulseTracker music file decoding. " << std::endl;
#else
    std::cout << "Using SDL_mixer's default decoder for ImpulseTracker music file decoding. " << std::endl;
#endif

    return 1;
}

void SOUND_Close() {
    Mix_CloseAudio();
}

int SOUND_GetMaxChannels() {
    return g_allocated;
}

signed char SOUND_SetPan(int channel, int pan) {
    // DKS - If pan is 128, disable panning effect on the channel:
    if (pan == 128) {
        Mix_SetPanning(channel, 255, 255);
    } else {
        Mix_SetPanning(channel, 255 - pan, pan);
    }

    return 1;
}

MUSIC_MODULE *MUSIC_LoadSong(const char *filename) {
    MUSIC_MODULE *music = nullptr;
#if defined(USE_OPENMPT)
    std::vector<char> buffer = LoadFileToMemory(filename);

    if (buffer.empty()) {
        std::cerr << "ERROR Error file operation: File: " << filename << std::endl;
        return nullptr;
    }

    music = openmpt_module_create_from_memory2(buffer.data(), size_t(buffer.size() - 1), &log_func, NULL, &error_func, NULL, NULL, NULL, NULL);
#else
    music = Mix_LoadMUS(filename);
#endif

    return music;
}

MUSIC_MODULE *MUSIC_LoadSongEx(const char *filename,
                               int offset,
                               int length,
                               unsigned int mode,
                               const int *samplelist,
                               int samplelistnum) {
    return NULL;
}

// DKS - Added default parameter to specify if a song should loop ('gameover.it' and 'stageclear.it' songs shouldn't):
// signed char MUSIC_PlaySong( MUSIC_MODULE* music )
signed char MUSIC_PlaySong(MUSIC_MODULE *music, bool loop /*=true*/) {
// printf( "%8X MUSIC_PlaySong\n", music );
#if defined(USE_OPENMPT)
    openmpt_module_set_render_param(music, OPENMPT_MODULE_RENDER_MASTERGAIN_MILLIBEL, getDeciBel(DEFAULT_VOLUME)/100.);
    openmpt_module_set_position_seconds(music, 0.);
    Mix_HookMusic(&hookmusic, music);
#else
    Mix_HookMusicFinished(hookmusicFinished);
    Mix_PlayMusic(music, (loop ? -1 : 0));
#endif
    g_music_loops = loop;
    g_music_current = music;

    return 0;
}

signed char MUSIC_IsPlaying(MUSIC_MODULE *music) {
    // printf( "%8X MUSIC_IsPlaying\n", music );
    return g_music_current == music ? 1 : 0;
}

signed char MUSIC_FreeSong(MUSIC_MODULE *music) {
// printf( "%8X MUSIC_FreeSong\n", music );
#if defined(USE_OPENMPT)
    openmpt_module_destroy(music);
#else
    Mix_FreeMusic(music);
#endif

    return 0;
}

signed char MUSIC_StopSong(MUSIC_MODULE *music) {
    // printf( "%8X MUSIC_StopSong\n", music );
    if (g_music_current == music) {
#if defined(USE_OPENMPT)
        Mix_HookMusic(nullptr, nullptr);
#else
        Mix_HookMusicFinished(nullptr);
        Mix_HaltMusic();
#endif
        g_music_current = nullptr;
    }
    return 0;
}

signed char MUSIC_SetPaused(MUSIC_MODULE *music, signed char pause) {
    // printf( "%8X MUSIC_SetPaused %d\n", music, pause );
    if (g_music_current == music) {
        if (pause)
            Mix_PauseMusic();
        else
            Mix_ResumeMusic();
    } else if (!pause) {
#if defined(USE_OPENMPT)
        // DKS - When using modplug, we can handle pausing and unpausing various songs:
        g_music_current = music;
        Mix_HookMusic(&hookmusic, music);  // DKS - Be sure to reload our hook in case StopSong() was called
        Mix_ResumeMusic();
#else
        // DKS - Whereas with SDL_mixer, it can only handle one song at a time so if another
        //      gets played after a song was paused, we have to start the paused song over again:
        MUSIC_PlaySong(music);
#endif
    }
    return 0;
}

signed char MUSIC_GetPaused(MUSIC_MODULE *music) {
    // printf( "%8X MUSIC_GetPaused\n", music );
    if (g_music_current == music) {
        return Mix_PausedMusic();
    }
    return 0;
}

signed char MUSIC_SetMasterVolume(MUSIC_MODULE *music, int volume) {
// printf( "%8X MUSIC_SetMasterVolume %d\n", music, volume );
#if defined(USE_OPENMPT)
    // DKS - Modplug supports setting volume for individual tracks
    if (music) {
        openmpt_module_set_render_param(music, OPENMPT_MODULE_RENDER_MASTERGAIN_MILLIBEL, getDeciBel(volume)/100.);
        // DKS - no need for this:
        // g_volume = volume;
    }
#else
    if (g_music_current == music)
        Mix_VolumeMusic(volume / 2);
#endif
    return 0;
}

void MUSIC_StopAllSongs() {
    // printf( "MUSIC_StopAllSongs\n" );
    MUSIC_StopSong(g_music_current);
}

signed char MUSIC_IsFinished(MUSIC_MODULE *music) {
    // printf( "%8X MUSIC_IsFinished\n", music );
    if (g_music_current == music) {
        return 1;
    } else {
        return Mix_PlayingMusic();
    }
}

#if defined(USE_OPENMPT)
void hookmusic(void *ptr, uint8_t *buffer, int size) {
    const int frames = size / 4;
    int rsize;
    MUSIC_MODULE *music = reinterpret_cast<MUSIC_MODULE *>(ptr);

    rsize = static_cast<int>(openmpt_module_read_interleaved_stereo(music, g_mixrate, frames, reinterpret_cast<int16_t *>(buffer)));

    // DKS - Added support for non-looped music files (i.e. gameover.it shouldn't loop)
    //      NOTE: I don't see the reason for the volume check, setting the volume to 0
    //            should be checked elsewhere, and music should be disabled entirely.
    // if (g_volume > 0 || (rsize < size))
    //{
    //    if (rsize < size)
    //    {
    //        ModPlug_Seek( music, 0 );
    //    }
    //}
    if (rsize < frames) {
        if (g_music_loops) {
            // The song is over and it loops, so re-seek to beginning and fill rest of buffer
            openmpt_module_set_position_seconds(music, 0.);
            openmpt_module_read_interleaved_stereo(music, g_mixrate, frames - rsize, reinterpret_cast<int16_t *>(buffer + rsize*4));

        } else {
            // The song is over and doesn't loop, so fill remaining part of the buffer with zeroes and stop it
            std::memset(buffer + rsize*4, 0, size - rsize*4);
            MUSIC_StopSong(music);
        }
    }
}
#else
void hookmusicFinished() {

    if (g_music_loops) {
        // The song is over and it loops, so re-seek to beginning
        Mix_RewindMusic();
    } else {
        // The song is over and doesn't loop, so stop it
        Mix_HaltMusic();
        g_music_current = nullptr;
    }
}
#endif

Mix_Chunk *SOUND_Sample_Load(int index, const char *filename, unsigned int inputmode, int offset, int length) {
    Mix_Chunk *chunk = nullptr;

    // Load from memory
    if ((inputmode & FSOUND_LOADMEMORY) == FSOUND_LOADMEMORY) {
#if 1
        SDL_RWops *sdl_rw = SDL_RWFromConstMem(reinterpret_cast<const void *>(filename), length);
        chunk = Mix_LoadWAV_RW(sdl_rw, 1);
#else
        chunk = Mix_QuickLoad_WAV((Uint8 *)filename);
#endif
    } else  // Load from file
    {
        chunk = Mix_LoadWAV(filename);
    }

    return chunk;
}

// DKS - Added looped boolean parameter, so we no longer have to keep track of which
//      sounds are looped (DX8Sound.cpp already does that)
int SOUND_PlaySound(int channel, Mix_Chunk *chunk, bool looped) {
    return Mix_PlayChannel(channel, chunk, looped ? -1 : 0);
}

void SOUND_Sample_Free(Mix_Chunk *chunk) {
    Mix_FreeChunk(chunk);
}

signed char SOUND_IsPlaying(int channel) {
    if (channel < 0)
        return 0;

    return Mix_Playing(channel);
}

int SOUND_GetVolume(int channel) {
    if (channel < 0)
        return 0;

    float vol = static_cast<float>(Mix_Volume(channel, -1)) / 128.0f;
    // DKS - SDL volumes are 0-128, while fmod's are 0-255:
    return static_cast<int>(vol * 255.0f);
}

signed char SOUND_SetVolume(int channel, int volume) {
    if (channel < 0)
        return 0;

    return Mix_Volume(channel, volume / 2);  // FMOD range is 0-255 SDL_mixer is 0-128
}

signed char SOUND_StopSound(int channel) {
    return Mix_HaltChannel(channel);
}

// DKS - Added:
signed char SOUND_SetPaused(int channel, signed char paused) {
    if (paused)
        Mix_Pause(channel);
    else
        Mix_Resume(channel);

    return 1;
}

// DKS - Added:
signed char SOUND_GetPaused(int channel) {
    return Mix_Paused(channel);
}
