#ifndef _SDL_FMOD_H_
#define _SDL_FMOD_H_

#include "SDL_mixer.h"
#if defined(USE_MODPLUG)
#include "modplug.h"
#endif
#include <stdio.h>

#define SOUND_SAMPLE            Mix_Chunk
#if defined(USE_MODPLUG)
#define MUSIC_MODULE            ModPlugFile
#else
#define MUSIC_MODULE            Mix_Music
#endif

#define DEFAULT_VOLUME 100

#ifdef __cplusplus
extern "C" {
#endif

enum FSOUND_DEFINES {
    FSOUND_FREE                     = -1,
    FSOUND_LOADMEMORY               = 0x01,
    FSOUND_LOOP_OFF                 = 0x02,
    FSOUND_LOOP_NORMAL              = 0x04,
    FSOUND_INIT_USEDEFAULTMIDISYNTH = 0x08
};

signed char     SOUND_Init              ( int mixrate, int maxsoftwarechannels, unsigned int flags );
void            SOUND_Close             ( void );
int             SOUND_GetError          ( void );
int             SOUND_GetMaxChannels    ( void );
signed char     SOUND_SetFrequency      ( int channel, int freq );
signed char     SOUND_SetPan            ( int channel, int pan );

MUSIC_MODULE*   MUSIC_LoadSong          ( const char* filename );
MUSIC_MODULE*   MUSIC_LoadSongEx        ( const char* filename, int offset, int length, unsigned int mode, const int *samplelist, int samplelistnum );
signed char     MUSIC_PlaySong          ( MUSIC_MODULE* music );
signed char     MUSIC_IsPlaying         ( MUSIC_MODULE* music );
signed char     MUSIC_FreeSong          ( MUSIC_MODULE* music );
signed char     MUSIC_StopSong          ( MUSIC_MODULE* music );
signed char     MUSIC_SetPaused         ( MUSIC_MODULE* music, signed char pause );
signed char     MUSIC_GetPaused         ( MUSIC_MODULE* music );
signed char     MUSIC_SetMasterVolume   ( MUSIC_MODULE* music, int volume );
void            MUSIC_StopAllSongs      ( void );
signed char     MUSIC_IsFinished        ( MUSIC_MODULE* music );

Mix_Chunk*      SOUND_Sample_Load       ( int index, const char *filename, unsigned int inputmode, int offset, int length );
int             SOUND_PlaySound         ( int channel, Mix_Chunk* chunk );
void            SOUND_Sample_Free       ( Mix_Chunk* chunk );
signed char     SOUND_IsPlaying         ( int channel );
int             SOUND_GetVolume         ( int channel );
signed char     SOUND_SetVolume         ( int channel, int volume );
signed char     SOUND_StopSound         ( int channel );

void            hookmusic               (void* ptr, uint8_t* buffer, int size);

#ifdef __cplusplus
}
#endif

#endif /* _SDL_FMOD_H_ */
