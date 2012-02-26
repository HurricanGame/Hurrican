// Datei : DX8Sound.h

// -------------------------------------------------------------------------------------- 
//
// DirectSound Klasse
// zum Abspielen von Wave-Dateien
//
// (c) 2002 J�rg M. Winterstein
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

#define MAX_CHANNELS			64						// Maximale Anzahl Sounds gleichzeitig
#define MAX_SOUNDS				128						// Anzahl verschiedener Soundeffekte
#define MAX_SONGS				32						// Anzahl verschiedener Musikst�cke

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// DirectSound Klasse
// --------------------------------------------------------------------------------------

class DirectSoundClass
{
	private:
		LPDIRECTSOUND       lpDirectSound;				// DirectSound Objekt
		LPDIRECTSOUNDBUFFER Sounds  [MAX_SOUNDS];		// Speicher f�r die Sonds
		LPDIRECTSOUNDBUFFER Channels[MAX_CHANNELS+1];	// beinhaltet die gerade gespielten Sounds
		FMUSIC_MODULE	   *Songs   [MAX_SONGS];		// Musikst�cke
		int					ChannelsUsed;				// wieviele Channels spielen gerade ?
		int					ChannelStats[MAX_CHANNELS+1]; // Status der channels (0 = frei, 1 = spielt gerade)
		int					GlobalSoundVolume;			// Globale Lautst�rke f�r
		int					GlobalMusicVolume;			// Sound und Musik
		
	public:
		DirectSoundClass(void);							// Konstruktor
	   ~DirectSoundClass(void);							// Desktruktor
		bool Init(HWND hwnd);							// DirectSound initialisieren
		void Exit(void);								// DirectSound beenden
		bool LoadWave(char *filename, unsigned int nr); // Wavdatei an Stelle "nr" im Array laden
		bool PlayWave(int Volume, int Panning, int nr); // Wavedatei mit best. Lautst�rke und Panning spielen
		void UpdateChannels(void);						// Sound zuende gespielt ? Dann freimachen
		void SetVolume(int sound, int music);			// Neue Lautst�rke setzen
		int  GetSoundVolume(void);						// aktuelle Sound-Lautst�rke abfragen
		int  GetMusicVolume(void);						// aktuelle Musik-Lautst�rke abfragen
		int  GetUsedChannels(void);						// wieviele Channels werden derzeit genutzt
		void SetAllSongVolumes(void);					// Alle Songs auf aktuelle Lautst�rke setzen
		bool LoadSong(char *filename, int nr);			// Song "filename" an Array-Pos "nr" laden
		void PlaySong(int nr);							// Song "nr" abspielen
};

#endif