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

#include "SDL_fmod.h"
#include "string.h"

static int  g_allocated = 0;
static bool g_music_loops = true;

//DKS - no need for this:
//static int  g_volume;

static MUSIC_MODULE* g_music_current = NULL;

#if defined(USE_MODPLUG)
static ModPlug_Settings settings;
#endif

signed char SOUND_Init( int mixrate, int maxsoftwarechannels, unsigned int flags )
{
    g_allocated = 0;

    int chunksize = 2048;
    if (Mix_OpenAudio( mixrate, MIX_DEFAULT_FORMAT, 2, chunksize) == -1)
    {
        printf("->ERROR: Mix_OpenAudio: %s\n", Mix_GetError());
        return 0;
    } else {
        //DKS - Check what we actually received:
        int      cur_freq = 0, cur_channels = 0;
        uint16_t cur_format = 0;
        Mix_QuerySpec(&cur_freq, &cur_format, &cur_channels);
        printf("SDL_mixer initialized. Buffer size:%d, Freq:%d, Channels:%d\n", chunksize, cur_freq, cur_channels);
        switch (cur_format) {
            case AUDIO_U8:
                printf("Unsigned 8-bit samples\n");
                break;
            case AUDIO_S8:
                printf("Signed 8-bit samples\n");
                break;
            case AUDIO_U16LSB:
                printf("Unsigned 16-bit samples (little-endian)\n");
                break;
            case AUDIO_S16LSB:
                printf("Signed 16-bit samples (little-endian)\n");
                break;
            case AUDIO_U16MSB:
                printf("Unsigned 16-bit samples (big-endian)\n");
                break;
            case AUDIO_S16MSB:
                printf("Signed 16-bit samples (big-endian)\n");
                break;
            default:
                printf("Unrecognized SDL_mixer sample format.\n");
                break;
        }
    }   
    g_allocated = Mix_AllocateChannels(maxsoftwarechannels);

#if defined(USE_MODPLUG)
    printf("Using libModPlug directly for ImpulseTracker music file decoding.\n");
    ModPlug_GetSettings( &settings );

    settings.mFlags             = MODPLUG_ENABLE_OVERSAMPLING | MODPLUG_ENABLE_NOISE_REDUCTION;
    settings.mResamplingMode    = MODPLUG_RESAMPLE_LINEAR;
    settings.mReverbDepth       = 0;
    settings.mReverbDelay       = 0;
    settings.mBassAmount        = 0;
    settings.mBassRange         = 0;
    settings.mSurroundDepth     = 0;
    settings.mSurroundDelay     = 0;
    settings.mFrequency         = mixrate;
    settings.mBits              = 16;
    settings.mChannels          = 2;
    //settings.mStereoSeparation  = 128;
    //settings.mMaxMixChannels    = g_allocated;

    ModPlug_SetSettings( &settings );
#else
    printf("Using SDL_mixer's default decoder for ImpulseTracker music file decoding.\n");
#endif

    return 1;
}

void SOUND_Close( void )
{
    Mix_CloseAudio();
}

int SOUND_GetError( void )
{
    // Do nothing
    return 1;
}

int SOUND_GetMaxChannels( void )
{
    return g_allocated;
}

signed char SOUND_SetFrequency( int channel, int freq )
{
    // Do nothing
    return 1;
}

signed char SOUND_SetPan( int channel, int pan )
{
    //DKS - If pan is 128, disable panning effect on the channel:
    if (pan == 128) {
        Mix_SetPanning( channel, 255, 255 );
    } else {
        Mix_SetPanning( channel, 255-pan, pan );
    }

    return 1;
}

MUSIC_MODULE* MUSIC_LoadSong( const char* filename )
{
    MUSIC_MODULE* music = NULL;
#if defined(USE_MODPLUG)
    uint32_t size;
    FILE* file = NULL;
    uint8_t* buffer = NULL;

    file = fopen( filename, "rb" );
    if (file == NULL)
    {
        printf( "ERROR Error file operation: File: %s\n", filename );
        return NULL;
    }

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	buffer = (uint8_t *)malloc(size+1);

	if (size != fread(buffer, sizeof(uint8_t), size, file))
	{
		free(buffer);
		return NULL;
	}
	fclose( file );
	buffer[size] = 0;

    music = ModPlug_Load( buffer, size );

    free( buffer );
#else
    music = Mix_LoadMUS( filename );
#endif

    return music;
}

MUSIC_MODULE* MUSIC_LoadSongEx( const char* filename, int offset, int length, unsigned int mode, const int *samplelist, int samplelistnum )
{
#if defined(USE_MODPLUG)
	return ModPlug_Load( filename, length);
#else
    // TODO Only need for unrar, but this will require cache the music file to disk
    return NULL;
#endif
}

//DKS - Added default parameter to specify if a song should loop ('gameover.it' and 'stageclear.it' songs shouldn't):
//signed char MUSIC_PlaySong( MUSIC_MODULE* music )
signed char MUSIC_PlaySong( MUSIC_MODULE* music, bool loop /*=true*/)
{
    //printf( "%8X MUSIC_PlaySong\n", music );
#if defined(USE_MODPLUG)
    ModPlug_SetMasterVolume( music, DEFAULT_VOLUME );
    ModPlug_Seek( music, 0 );
    Mix_HookMusic( &hookmusic, music );
#else
    Mix_PlayMusic( music, (loop ? -1 : 0));
#endif
    g_music_loops   = loop;
    g_music_current = music;

    return 0;
}

signed char MUSIC_IsPlaying( MUSIC_MODULE* music )
{
    //printf( "%8X MUSIC_IsPlaying\n", music );
    return g_music_current == music ? 1 : 0;
}

signed char MUSIC_FreeSong( MUSIC_MODULE* music )
{
    //printf( "%8X MUSIC_FreeSong\n", music );
#if defined(USE_MODPLUG)
    ModPlug_Unload( music );
#else
    Mix_FreeMusic( music );
#endif

    return 0;
}

signed char MUSIC_StopSong( MUSIC_MODULE* music )
{
    //printf( "%8X MUSIC_StopSong\n", music );
    if (g_music_current == music)
    {
#if defined(USE_MODPLUG)
        Mix_HookMusic( NULL, NULL );
#else
        Mix_HaltMusic();
#endif
        g_music_current = NULL;
    }
    return 0;
}

signed char MUSIC_SetPaused( MUSIC_MODULE* music, signed char pause )
{
    //printf( "%8X MUSIC_SetPaused %d\n", music, pause );
    if (g_music_current == music)
    {
        if (pause)
            Mix_PauseMusic();
        else
            Mix_ResumeMusic();
    } else if (!pause)
    {
#if defined(USE_MODPLUG)
        //DKS - When using modplug, we can handle pausing and unpausing various songs:
        g_music_current = music;
        Mix_HookMusic( &hookmusic, music ); //DKS - Be sure to reload our hook in case StopSong() was called
        Mix_ResumeMusic();
#else
        //DKS - Whereas with SDL_mixer, it can only handle one song at a time so if another
        //      gets played after a song was paused, we have to start the paused song over again:
        MUSIC_PlaySong( music );
#endif
    }
    return 0;
}

signed char MUSIC_GetPaused( MUSIC_MODULE* music )
{
    //printf( "%8X MUSIC_GetPaused\n", music );
    if (g_music_current == music)
    {
        return Mix_PausedMusic();
    }
    return 0;
}

signed char MUSIC_SetMasterVolume( MUSIC_MODULE* music, int volume )
{
    //printf( "%8X MUSIC_SetMasterVolume %d\n", music, volume );
#if defined(USE_MODPLUG)
    //DKS - Modplug supports setting volume for individual tracks
    if (music) {
        //DKS - Range of volumes for libmodplug is 0-512, not 0-255:
        //ModPlug_SetMasterVolume( music, volume );
        //DKS - I decided 3/2 of libmodplug's max was a better balance of the music's volume vs. sound effects:
        //ModPlug_SetMasterVolume( music, volume*2 );
        ModPlug_SetMasterVolume( music, volume*3/2 );
        //DKS - no need for this:
        //g_volume = volume;
    }
#else
    if (g_music_current == music)
        Mix_VolumeMusic( volume/2 );
#endif
    return 0;
}

void MUSIC_StopAllSongs( void )
{
    //printf( "MUSIC_StopAllSongs\n" );
    MUSIC_StopSong( g_music_current );
}

signed char MUSIC_IsFinished( MUSIC_MODULE* music )
{
    //printf( "%8X MUSIC_IsFinished\n", music );
    if (g_music_current == music)
    {
        return 1;
    }
    else
    {
        return Mix_PlayingMusic();
    }
}

#if defined(USE_MODPLUG)
void hookmusic( void* ptr, uint8_t* buffer, int size )
{
	int rsize;
    ModPlugFile* music = (ModPlugFile*)ptr;

	rsize = ModPlug_Read( music, (void*)buffer, size );

    //DKS - Added support for non-looped music files (i.e. gameover.it shouldn't loop)
    //      NOTE: I don't see the reason for the volume check, setting the volume to 0
    //            should be checked elsewhere, and music should be disabled entirely.
    //if (g_volume > 0 || (rsize < size))
    //{
    //    if (rsize < size)
    //    {
    //        ModPlug_Seek( music, 0 );
    //    }
    //}
    if (rsize < size) {
        if (g_music_loops) {
            // The song is over and it loops, so re-seek to beginning and fill rest of buffer
            ModPlug_Seek( music, 0 );
            ModPlug_Read( music, (void*)(buffer+rsize), size - rsize );

        } else {
            // The song is over and doesn't loop, so fill remaining part of the buffer with zeroes and stop it
            memset( (void*)(buffer+rsize), 0, size - rsize ); 
            MUSIC_StopSong( music );
        }
    }
}
#endif

Mix_Chunk* SOUND_Sample_Load( int index, const char *filename, unsigned int inputmode, int offset, int length )
{
    Mix_Chunk* chunk = NULL;

    // Load from memory
    if ((inputmode & FSOUND_LOADMEMORY) == FSOUND_LOADMEMORY)
    {
#if 1
        SDL_RWops* sdl_rw = SDL_RWFromConstMem( (const void*)filename, length );
		chunk = Mix_LoadWAV_RW(sdl_rw, 1);
#else
        chunk = Mix_QuickLoad_WAV( (Uint8*)filename );
#endif
    }
    else    // Load from file
    {
        chunk = Mix_LoadWAV( filename );
    }

    return chunk;
}

//DKS - Added looped boolean parameter, so we no longer have to keep track of which
//      sounds are looped (DX8Sound.cpp already does that)
int SOUND_PlaySound( int channel, Mix_Chunk* chunk, bool looped)
{
    return Mix_PlayChannel( channel, chunk, looped ? -1 : 0 );
}

void SOUND_Sample_Free( Mix_Chunk* chunk )
{
    Mix_FreeChunk( chunk );
}

signed char SOUND_IsPlaying( int channel )
{
    if (channel < 0) return 0;

    return Mix_Playing( channel );
}

int SOUND_GetVolume( int channel )
{
    if (channel < 0) return 0;

    float vol = (float)Mix_Volume( channel, -1 ) / 128.0f;
    //DKS - SDL volumes are 0-128, while fmod's are 0-255:
    return (int)(vol * 255.0f);
}

signed char SOUND_SetVolume( int channel, int volume )
{
    if (channel < 0) return 0;

    return Mix_Volume( channel, volume/2 ); // FMOD range is 0-255 SDL_mixer is 0-128
}

signed char SOUND_StopSound( int channel )
{
    return Mix_HaltChannel( channel );
}

//DKS - Added:
signed char SOUND_SetPaused(int channel, signed char paused)
{
    if (paused)
        Mix_Pause(channel);
    else
        Mix_Resume(channel);

    return 1;  
}

//DKS - Added:
signed char SOUND_GetPaused(int channel)
{
    return Mix_Paused(channel);
}
