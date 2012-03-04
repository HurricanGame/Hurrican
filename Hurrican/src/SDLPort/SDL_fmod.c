#include "SDL_fmod.h"

int g_allocated = 0;
int sound_loops_total = 0;
Mix_Chunk** sound_loops;
Mix_Music*  music_current = NULL;

signed char SOUND_Init( int mixrate, int maxsoftwarechannels, unsigned int flags )
{
    g_allocated = 0;

    if (Mix_OpenAudio( mixrate, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
        return 0;
    }

    g_allocated = Mix_AllocateChannels(maxsoftwarechannels);

    return 1;
}

void SOUND_Close( void )
{
    free( sound_loops );
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
    return 0;
}

signed char SOUND_SetPan( int channel, int pan )
{
    return Mix_SetPanning( channel, 255-pan, pan );
}

Mix_Music* MUSIC_LoadSong( const char* filename )
{
    return Mix_LoadMUS( filename );
}

Mix_Music* MUSIC_LoadSongEx( const char* filename, int offset, int length, unsigned int mode, const int *samplelist, int samplelistnum )
{
    // TODO Only need for unrar, but this will require cache the music file to disk
    return NULL;
}

signed char MUSIC_PlaySong( Mix_Music* music )
{
    music_current = music;

    return Mix_PlayMusic( music, -1 );
}

signed char MUSIC_IsPlaying( Mix_Music* music )
{
    if (music_current==music)
    {
        return Mix_PlayingMusic();
    }
    else /* if it doesnt match the currently played music then it cant be playing  */
    {
        return 0;
    }
}

signed char MUSIC_FreeSong( Mix_Music* music )
{
    Mix_FreeMusic( music );
    return 0;
}

signed char MUSIC_StopSong( Mix_Music* music )
{
    if (music_current!=music)
    {
        printf( "SDL_fmod: tried to halt music that is not currently being played\n" );
        return 0;
    }
    else
    {
        return Mix_HaltMusic();
    }
}

signed char MUSIC_SetPaused( Mix_Music* music, signed char pause )
{
    if (music_current!=music)
    {
        printf( "SDL_fmod: tried to pause music that is not currently being played\n" );
        return 1;
    }
    else
    {
        if (pause >= 1) {
            Mix_PauseMusic();
            return 0;
        }
        else return 1;
    }
}

signed char MUSIC_GetPaused( Mix_Music* music )
{
    if (music_current!=music)
    {
        printf( "SDL_fmod: tried to query pause music that is not currently being played\n" );
        return 0;
    }
    else
    {
        return Mix_PausedMusic();
    }
}

signed char MUSIC_SetMasterVolume( Mix_Music* music, int volume )
{
    if (music_current!=music)
    {
        printf( "SDL_fmod: tried to set volume for music that is not currently being played\n" );
        return 0;
    }
    else
    {
        return Mix_VolumeMusic( volume/2 ); // FMOD range is 0-255 SDL_mixer is 0-128
    }
}

void MUSIC_StopAllSongs( void )
{
    Mix_HaltMusic();
}

signed char MUSIC_IsFinished( Mix_Music* music )
{
    if (music_current!=music)
    {
        printf( "SDL_fmod: tried to query is finished for music that is not currently being played\n" );
        return 1;
    }
    else
    {
        return Mix_PlayingMusic();
    }
}

Mix_Chunk* SOUND_Sample_Load( int index, const char *filename, unsigned int inputmode, int offset, int length )
{
    Mix_Chunk* chunk = NULL;

    // Load from memory
    if ((inputmode & FSOUND_LOADMEMORY) == FSOUND_LOADMEMORY)
    {
        chunk = Mix_QuickLoad_WAV( (Uint8*)filename );
    }
    else    // Load from file
    {
        chunk = Mix_LoadWAV( filename );
    }

    if (chunk != NULL)
    {
        if ((inputmode & FSOUND_LOOP_NORMAL) == FSOUND_LOOP_NORMAL)
        {
            if (sound_loops_total==0)
            {
                sound_loops = (Mix_Chunk**)calloc( 1, sizeof(Mix_Chunk*) );
            }
            else
            {
                sound_loops = realloc( sound_loops, (sound_loops_total+1)*sizeof(Mix_Chunk*) );
            }
            //printf( "Adding %X\n", chunk );
            sound_loops[sound_loops_total] = chunk;

            sound_loops_total++;
        }
    }

    return chunk;
}

int SOUND_PlaySound( int channel, Mix_Chunk* chunk )
{
    int i;
    int loops = 0;

    for( i=0; i<sound_loops_total; i++)
    {
        //printf( "Checking %d, %X\n", i, sound_loops[i] );
        if (sound_loops[i] == chunk)
        {
            loops = -1;
            break;
        }
    }

    return Mix_PlayChannel( channel, chunk, loops );
}

void SOUND_Sample_Free( Mix_Chunk* chunk )
{
    Mix_FreeChunk( chunk );
}

signed char SOUND_IsPlaying( int channel )
{
    return Mix_Playing( channel );
}

int SOUND_GetVolume( int channel )
{
    return Mix_Volume( channel, -1 );
}

signed char SOUND_SetVolume( int channel, int volume )
{
    return Mix_Volume( channel, volume/2 ); // FMOD range is 0-255 SDL_mixer is 0-128
}

signed char SOUND_StopSound( int channel )
{
    return Mix_HaltChannel( channel );
}
