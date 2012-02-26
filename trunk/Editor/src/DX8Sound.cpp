// Datei : DX8Sound.cpp

// -------------------------------------------------------------------------------------- 
//
// DirectSound Klasse
// zum Abspielen von Wave-Dateien
//
// Wird eine Wave abgespielt, so wird erst eine Kopie von ihr erstellt und diese dann
// gespielt, damit es möglich ist, einen Sound "überlagernd" zu spielen
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "DX8Sound.h"
#include "LogDatei.h"
#include "Globals.h"

// --------------------------------------------------------------------------------------
// externe Variablen
// --------------------------------------------------------------------------------------

extern Logdatei				Protokoll;					// Protokoll Datei

// --------------------------------------------------------------------------------------
// Klassendefunktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// DirectSound Klasse
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

DirectSoundClass::DirectSoundClass(void)
{
	lpDirectSound	 = NULL;

	// Zu Beginn alle Channels "verfügbar" machen, da ja kein einziger Sound spielt
	for(int i=0; i<MAX_CHANNELS; i++)
	{
		ChannelStats[i] = 0;
	}

	ChannelsUsed = 0;

	// Songs auf NULL setzen
	for(i=0; i<MAX_SONGS; i++)
		Songs[i] = NULL;
}

// --------------------------------------------------------------------------------------
// Destruktor
// --------------------------------------------------------------------------------------

DirectSoundClass::~DirectSoundClass(void)
{
}

// --------------------------------------------------------------------------------------
// DirectSound initialisieren
// --------------------------------------------------------------------------------------

bool DirectSoundClass::Init(HWND hwnd)
{
	Protokoll.WriteText("\n--> DirectSound8 Initialisierung gestartet <--\n", false);
	Protokoll.WriteText(  "----------------------------------------------\n\n", false);

	if(DirectSoundCreate (NULL, &lpDirectSound, NULL) != DS_OK)
	{
		Protokoll.WriteText ("\n-> DirectSoundCreate Fehler !\n", true);
		return (false);
	}
	Protokoll.WriteText ("DirectSoundCreate			: erfolgreich !\n", false);

	// Cooperative Level setzen
	if(lpDirectSound->SetCooperativeLevel (hwnd, DSSCL_NORMAL) != DS_OK)
	{
		Protokoll.WriteText("\n-> SetCooperativeLevel Fehler !\n", true);
		return (false);
	}
	Protokoll.WriteText("SetCooperativeLevel			: erfolgreich !\n", false);
	Protokoll.WriteText("\n-> DirectSound8 Initialisierung erfolgreich beendet !\n\n", false);

	//Initialisieren von FMOD

	Protokoll.WriteText("\n-->  FMOD Initialisierung gestartet <--\n", false);
	Protokoll.WriteText(  "---------------------------------------\n\n", false);

	FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);					// Output-Mode setzen
	FSOUND_SetDriver(0);									// Default-Soundkarte setzen
	FSOUND_SetMixer(FSOUND_MIXER_QUALITY_AUTODETECT);		// Mixer-Quality setzen
		
	if (!FSOUND_Init(48000, 32, FSOUND_INIT_USEDEFAULTMIDISYNTH))
	{
		Protokoll.WriteText("\n-> FMOD Initialisierung Fehler !\n", true);
		return false;
	}

	Protokoll.WriteText("\n-> FMOD Initialisierung erfolgreich beendet !\n\n", false);
	
	return true;
}

// --------------------------------------------------------------------------------------
// Beenden
// --------------------------------------------------------------------------------------

void DirectSoundClass::Exit(void)
{
	// Sounds freigeben
	for(int i=0; i<MAX_SOUNDS; i++)
		SafeRelease(Sounds[i]);

	// DirectSound Hauptobjekt freigeben
	SafeRelease(lpDirectSound);
	Protokoll.WriteText("-> DirectSound8 erfolgreich beendet !\n", false);

	// FMOD aufräumen
	FMUSIC_StopAllSongs();

	for(i=0; i<MAX_SONGS; i++)
		FMUSIC_FreeSong(Songs[i]);

	FSOUND_Close();										
	Protokoll.WriteText("-> FMOD erfolgreich beendet !\n", false);
}

// --------------------------------------------------------------------------------------
// Wave Datei in Soundbuffer laden an Array Stelle "nr"
// --------------------------------------------------------------------------------------

bool DirectSoundClass::LoadWave(char *filename, unsigned int nr)
{
	char	Temp[140];
	HMMIO wavefile;

	// Wave-Datei öffnen
	wavefile = mmioOpen (filename, 0, MMIO_READ | MMIO_ALLOCBUF);
	if (wavefile == NULL)							// Fehler beim Öffnen ?
	{
		strcpy(Temp, "\n-> Wave laden Fehler : ");
		strcat(Temp, filename);
		strcat(Temp, " nicht gefuden !\n");
		Protokoll.WriteText(Temp, true);
	}

	// Wave-Format auslesen
	MMCKINFO parent;
	memset (&parent, 0, sizeof(MMCKINFO));
	parent.fccType = mmioFOURCC ('W', 'A', 'V', 'E');
	mmioDescend (wavefile, &parent, 0, MMIO_FINDRIFF);

	MMCKINFO child;
	memset (&child, 0, sizeof(MMCKINFO));
	child.fccType = mmioFOURCC ('f', 'm', 't', ' ');
	mmioDescend (wavefile, &child, &parent, 0);

	WAVEFORMATEX wavefmt;
	mmioRead (wavefile, (char*)&wavefmt, sizeof(wavefmt));

	// Wave-Format unbekannt ?
	if(wavefmt.wFormatTag != WAVE_FORMAT_PCM)
	{
		Protokoll.WriteText("\n-> Wave laden Fehler : Unbekanntes Waveformat !\n", true);
		return (false);
	}

	mmioAscend (wavefile, &child, 0);
	child.ckid = mmioFOURCC ('d', 'a', 't', 'a');
	mmioDescend (wavefile, &child, &parent, MMIO_FINDCHUNK); 

// Dann wird ein DirectSoundBuffer erstellt (die Variable dsbSound). 
// Er nimmt den Sound in einem DirectSound-Format auf:

	DSBUFFERDESC bufdesc;
	memset (&bufdesc, 0, sizeof(DSBUFFERDESC));
	bufdesc.dwSize = sizeof(DSBUFFERDESC);

	bufdesc.dwFlags = 0;
	bufdesc.dwBufferBytes = child.cksize;
	bufdesc.lpwfxFormat = &wavefmt;
	if((lpDirectSound->CreateSoundBuffer (&bufdesc, &Sounds[nr], NULL)) != DS_OK)
	{
		Protokoll.WriteText("\n-> Wave laden Fehler beim Erstellen des Soundbuffers !\n", true);
		return (false);
	} 

// Zum Schluß wird der Buffer mit den Daten gefüllt:

	void *write1 = 0, *write2 = 0;
	unsigned long length1,length2;
	Sounds[nr]->Lock (0, child.cksize, &write1, &length1, &write2, &length2, 0);
	if(write1 > 0)
		mmioRead (wavefile, (char*)write1, length1);
	if (write2 > 0)
		mmioRead (wavefile, (char*)write2, length2);
	Sounds[nr]->Unlock (write1, length1, write2, length2);

	// Wave-Datei schliessen
	mmioClose (wavefile, 0);

	// Wave korrekt geladen
	if (wavefile != NULL)
	{
		strcpy(Temp, "Wave laden : ");
		strcat(Temp, filename);
		strcat(Temp, " erfolgreich !\n");
		Protokoll.WriteText(Temp, false);
	}

	return true;
}

// --------------------------------------------------------------------------------------
// Wavedatei an Array-Position "nr" mit bestimmter Lautstärke und Panning spielen
// --------------------------------------------------------------------------------------

bool DirectSoundClass::PlayWave(int Volume, int Panning, int nr)
{
	if(Volume == 0)					// hört man den Sound überhaupt ? :)
		return false;

	// Channels durchgehen und prüfen, welcher frei ist
	int i=0;
	while(i<MAX_CHANNELS+1 && ChannelStats[i] == 1)
		i++;

	if(i>=MAX_CHANNELS)				// kein Channel mehr frei ?
		return false;

	ChannelStats[i] = 1;			// Channel als "in Benutzung" markieren
	ChannelsUsed++;					// und einen Channel mehr als "benutzt" setzten
	
	lpDirectSound->DuplicateSoundBuffer(Sounds[nr], &Channels[i]);	// Buffer duplizieren
	Channels[i]->SetVolume((DWORD)(-30*(100 - Volume)));			// Lautstärke setzen
	Channels[i]->SetCurrentPosition (0);							// an den Anfang springen
	Channels[i]->Play (0, 0, 0);									// und dann abgespielt

	return true;
}

// --------------------------------------------------------------------------------------
// Channels auf fertige Sounds prüfen und ggf. den Channel wieder freimachen
// --------------------------------------------------------------------------------------

void DirectSoundClass::UpdateChannels(void)
{
	DWORD		dwstatus;
	for(int i=0; i<MAX_CHANNELS; i++)
		if(ChannelStats[i] == 1)
		{
			Channels[i]->GetStatus(&dwstatus);			// Zustand holen
			if(!(dwstatus & DSBSTATUS_PLAYING))			// Fertig abgespielt ?
			{
				ChannelStats[i] = 0;					// Dann erneut verfügbar machen
				if(ChannelsUsed>0)						// und ein Channel weniger in Benutzung
					ChannelsUsed--;	
			}
		}
}

// --------------------------------------------------------------------------------------
// Neue Lautstärke setzen
// --------------------------------------------------------------------------------------

void DirectSoundClass::SetVolume(int sound, int music)
{
	GlobalSoundVolume = sound;
	GlobalMusicVolume = music;
}

// --------------------------------------------------------------------------------------
// aktuelle Sound-Lautstärke abfragen
// --------------------------------------------------------------------------------------

int DirectSoundClass::GetSoundVolume(void)
{
	return GlobalSoundVolume;
}

// --------------------------------------------------------------------------------------
// aktuelle Musik-Lautstärke abfragen
// --------------------------------------------------------------------------------------

int DirectSoundClass::GetMusicVolume(void)
{
	return GlobalMusicVolume;
}

// --------------------------------------------------------------------------------------
// Zurückliefern, wieviele Channels derzeit genutzt werden
// --------------------------------------------------------------------------------------

int DirectSoundClass::GetUsedChannels(void)
{
	return ChannelsUsed;
}

// --------------------------------------------------------------------------------------
// Alle Songs auf aktuelle Lautstärke setzen
// --------------------------------------------------------------------------------------

void DirectSoundClass::SetAllSongVolumes(void)
{
	for(int i=0; i<MAX_SONGS; i++)
		FMUSIC_SetMasterVolume(Songs[i], (int)(2.5*GlobalMusicVolume));
}

// --------------------------------------------------------------------------------------
// Song "filename" an Array-Pos "nr" laden
// --------------------------------------------------------------------------------------

bool DirectSoundClass::LoadSong(char *filename, int nr)
{
	char	Temp[140];

	Songs[nr] = FMUSIC_LoadSong(filename);
	if(FSOUND_GetError()  == FMOD_ERR_FILE_NOTFOUND)	// Song korrekt geladen ?
	{
		strcpy(Temp, "\n-> Song laden : ");
		strcat(Temp, filename);
		strcat(Temp, " Fehler !\n");
		Protokoll.WriteText(Temp, true);
		return false;
	}

	strcpy(Temp, "Song laden : ");
	strcat(Temp, filename);
	strcat(Temp, " erfolgreich !\n");
	Protokoll.WriteText(Temp, false);
	return true;
}

// --------------------------------------------------------------------------------------
// Song "nr" abspielen
// --------------------------------------------------------------------------------------

void DirectSoundClass::PlaySong(int nr)
{
	FMUSIC_PlaySong(Songs[nr]);
}