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

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include <stdio.h>
#include "DX8Sound.h"
#include "LogDatei.h"
#include "Globals.h"
#include "Gameplay.h"
#include "Main.h"
#include "Player.h"
#include "Timer.h"
#include "Unrarlib.h"

//---------------------------------------------------------------------------------------
// Funktionsdefinitionen
//---------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name			: GetFMODErrorString
// Aufgabe		: FMOD Error-Wert in String umwandeln
// Rückgabewert	: String mit Error-Text
// Parameter	: FMOD Error-Wert
//-----------------------------------------------------------------------------

char *GetFMODErrorString(int ErrorNr)
{
	char ErrorString[100];
	char *ReturnString;

	strcpy_s(ErrorString, strlen("Unknown Error") + 1, "Unknown Error");

	/*switch(ErrorNr)
	{
		case FMOD_ERR_NONE				: strcpy_s(ErrorString, "No errors"); break;
		case FMOD_ERR_BUSY				: strcpy_s(ErrorString, "Cannot call this command after FSOUND_Init. Call FSOUND_Close first. "); break;
		case FMOD_ERR_UNINITIALIZED		: strcpy_s(ErrorString, "This command failed because FSOUND_Init or FSOUND_SetOutput was not called "); break;
		case FMOD_ERR_INIT				: strcpy_s(ErrorString, "Error initializing output device. "); break; 
		case FMOD_ERR_ALLOCATED			: strcpy_s(ErrorString, "Error initializing output device, but more specifically, the output device is already in use and cannot be reused. "); break;
		case FMOD_ERR_PLAY				: strcpy_s(ErrorString, "Playing the sound failed. "); break;
 		case FMOD_ERR_OUTPUT_FORMAT		: strcpy_s(ErrorString, "Soundcard does not support the features needed for this soundsystem (16bit stereo output) "); break;
 		case FMOD_ERR_COOPERATIVELEVEL	: strcpy_s(ErrorString, "Error setting cooperative level for hardware. "); break;
 		case FMOD_ERR_CREATEBUFFER		: strcpy_s(ErrorString, "Error creating hardware sound buffer. "); break;
 		case FMOD_ERR_FILE_NOTFOUND		: strcpy_s(ErrorString, "File not found "); break;
 		case FMOD_ERR_FILE_FORMAT		: strcpy_s(ErrorString, "Unknown file format "); break;
 		case FMOD_ERR_FILE_BAD			: strcpy_s(ErrorString, "Error loading file "); break;
 		case FMOD_ERR_MEMORY			: strcpy_s(ErrorString, "Not enough memory or resources "); break;
 		case FMOD_ERR_VERSION			: strcpy_s(ErrorString, "The version number of this file format is not supported "); break;
 		case FMOD_ERR_INVALID_PARAM		: strcpy_s(ErrorString, "An invalid parameter was passed to this function "); break;
 		case FMOD_ERR_NO_EAX			: strcpy_s(ErrorString, "Tried to use an EAX command on a non EAX enabled channel or output. "); break;
 		case FMOD_ERR_CHANNEL_ALLOC		: strcpy_s(ErrorString, "Failed to allocate a new channel "); break;
 		case FMOD_ERR_RECORD			: strcpy_s(ErrorString, "Recording is not supported on this machine "); break;
 		case FMOD_ERR_MEDIAPLAYER		: strcpy_s(ErrorString, "Windows Media Player not installed so cannot play wma or use internet : streaming. "); break;
		default							: strcpy_s(ErrorString, "Unknown Error"); break;
	} // switch(ErrorNr)*/

	ReturnString = ErrorString;

	return ReturnString;
} // GetFMODErrorString

//---------------------------------------------------------------------------------------
// Klassen-Definition (CSong)
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// Name			: CSong (Konstruktor)
// Aufgabe		: Song initialisieren
// Rückgabewert	: Keiner
// Parameter	: Keine
//---------------------------------------------------------------------------------------

CSong::CSong()
{
	SongData = NULL;
} // Konstruktor

//---------------------------------------------------------------------------------------
// Name			: CSong (Destruktor)
// Aufgabe		: Song initialisieren
// Rückgabewert	: Keiner
// Parameter	: Keine
//---------------------------------------------------------------------------------------

CSong::~CSong()
{
	// Freigeben
	if (SongData != NULL)
		FMUSIC_FreeSong(SongData);

	SongData = NULL;
} // Destruktor

//---------------------------------------------------------------------------------------
// Name			: Update
// Aufgabe		: Songs ggf faden
// Rückgabewert	: Keiner
// Parameter	: Keine
//---------------------------------------------------------------------------------------

bool CSong::Update(void)
{
	// Spielt der Song garnicht ? Dann brauchen wir ihn auch nicht zu bearbeiten =)
	// 
	if (!FMUSIC_IsPlaying(SongData))
		return false;

	// Fadet der Song grade ?
	// 
	if (FadingVolume != 0.0f)
	{
		Volume += FadingVolume SYNC;

		// Grenze Überschritten ?
		if ((FadingVolume > 0.0f &&
		 	 Volume >= FadingEnd) ||
					
			(FadingVolume < 0.0f &&
			 Volume <= FadingEnd))
		{
			// Beim Anhalten des Songs ausschalten oder pausieren ?
			if (FadingVolume < 0.0f)
			{
				if (FadingPaused == false)
					FMUSIC_StopSong(SongData);			// Ganz anhalten ?
				else
					FMUSIC_SetPaused(SongData, true);	// oder nur pausieren ?
			}

			FadingVolume = 0.0f;
			Volume		 = float(FadingEnd);
		}
		FMUSIC_SetMasterVolume(SongData, (int)(Volume*pSoundManager->its_GlobalMusicVolume/100.0f*2.55f));
	}

	return true;
} // Update

//---------------------------------------------------------------------------------------
// Klassen-Definition (CWave)
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// Name			: CWave (Konstruktor)
// Aufgabe		: Sound initialisieren
// Rückgabewert	: Keiner
// Parameter	: Keine
//---------------------------------------------------------------------------------------

CWave::CWave()
{
	isPlaying = false;
	isLooped  = false;
	FadeMode = FADEMODE_NON;
	Channel   = -1;
	SoundData = NULL;
} // Konstruktor

//---------------------------------------------------------------------------------------
// Name			: ~CWave (Destruktor)
// Aufgabe		: Sound löschen
// Rückgabewert	: Keiner
// Parameter	: Keine
//---------------------------------------------------------------------------------------

CWave::~CWave()
{
	// Freigeben
	if (SoundData != NULL)
		FSOUND_Sample_Free(SoundData);	

	SoundData = NULL;
} // Destruktor

//---------------------------------------------------------------------------------------
// Name			: Update
// Aufgabe		: Channel prüfen
// Rückgabewert	: Keiner
// Parameter	: Keine
//---------------------------------------------------------------------------------------

bool CWave::Update()
{
	if (FSOUND_IsPlaying(Channel) == false)
	{
		isPlaying = false;
		FadeMode = FADEMODE_NON;		
	}

	float vol = (float)FSOUND_GetVolume(Channel);

	// fading?
	//
	switch (FadeMode)
	{
		// momentan nicht faden
		//
		case FADEMODE_NON:
		break;

		// einfaden
		//
		case FADEMODE_IN:
		{
			// Volume erhöhen
			//
			vol += 30.0f SYNC;

			// maximum? Dann fading anhalten
			//
			if (vol > pSoundManager->its_GlobalSoundVolume * 2.55f)
			{
				vol = pSoundManager->its_GlobalSoundVolume * 2.55f;
				FadeMode = FADEMODE_FULL;
			}

			// Lautstärke setzen
			//
			FSOUND_SetVolume(Channel, (int)vol);
		}
		break;

		// ausfaden
		//
		case FADEMODE_OUT:
		{
			// Volume verringern
			//
			vol -= 30.0f SYNC;

			// < 0 ? Dann anhalten
			//
			if (vol < 0.0f)
			{				
				FSOUND_StopSound(Channel);
				isPlaying = false;
				Channel = -1;
				FadeMode = FADEMODE_NON;
			}
			else
				FSOUND_SetVolume(Channel, (int)vol);
		}
		break;

		default:
			break;
	}

	return true;
} // Update

//---------------------------------------------------------------------------------------
// Klassen-Definition (SoundManager)
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// Name			: CSoundManager (Konstruktor)
// Aufgabe		: Standardmässige Initialisierung
// Rückgabewert	: Keiner
// Parameter	: Keine
//---------------------------------------------------------------------------------------

CSoundManager::CSoundManager()
{
	SOUNDMANAGER_PARAMETERS smpp;

	InitSuccessfull = false;

	its_LoadedSongs  = 0;
	its_LoadedSounds = 0;
	its_GlobalMusicVolume		= 100;
	its_GlobalSoundVolume		= 100;

	smpp.MaxSoftwareChannels	= 64;
	smpp.Mixrate				= 44100;
	smpp.Flags					= FSOUND_INIT_USEDEFAULTMIDISYNTH;

	// Sounds auf NULL setzen
	for (int i=0; i<MAX_SOUNDS; i++)
		its_Sounds[i] = NULL;

	// Songs auf NULL setzen
	for (int i=0; i<MAX_SONGS; i++)
		its_Songs[i] = NULL;

	InitSuccessfull = InitFMOD(smpp);

	if (true == InitSuccessfull)
		Protokoll.WriteText("Sound Manager created\n", false);
	else
		Protokoll.WriteText("\n-> Create Sound Manager failed !\n", false);

}; // Konstruktor

//---------------------------------------------------------------------------------------
// Name			: CSoundManager (Überladener Konstruktor)
// Aufgabe		: Initialisierung mit übergebenen Parametern
// Rückgabewert	: Keiner
// Parameter	: Keine
//---------------------------------------------------------------------------------------

CSoundManager::CSoundManager   (SOUNDMANAGER_PARAMETERS smpp)
{
	InitSuccessfull = false;

	its_LoadedSongs  = 0;
	its_LoadedSounds = 0;
	its_GlobalMusicVolume	= float(smpp.GlobalMusicVolume);
	its_GlobalSoundVolume	= float(smpp.GlobalSoundVolume);

	// Sounds auf NULL setzen
	for (int i=0; i<MAX_SOUNDS; i++)
		its_Sounds[i] = NULL;

	// Songs auf NULL setzen
	for (int i=0; i<MAX_SONGS; i++)
		its_Songs[i] = NULL;

	if (InitFMOD(smpp) == true)
	{
		Protokoll.WriteText("Sound Manager created\n", false);
	}
	else
	{
		Protokoll.WriteText("\n-> Create Sound Manager failed !\n", true);
	}
}; // Überladener Konstruktor


//---------------------------------------------------------------------------------------
// Name			: ~CSoundManager (Destruktor)
// Aufgabe		: Songs freigeben
// Rückgabewert	: Keiner
// Parameter	: Keine
//---------------------------------------------------------------------------------------

CSoundManager::~CSoundManager()
{
	Protokoll.WriteText("-> Shutting down Sound Manager\n", false);

	FMUSIC_StopAllSongs();

	// Songs freigeben
	//
	for (int i=0; i<MAX_SONGS; i++)
		if (its_Songs[i] != NULL)
		{
			delete(its_Songs[i]);	// Freigeben
			its_Songs[i] = NULL;
		}

	its_LoadedSongs = 0;

	// Sounds freigeben
	for (int i=0; i<MAX_SOUNDS; i++)
		if (its_Sounds[i] != NULL)
		{
			delete(its_Sounds[i]);	// Freigeben
			its_Sounds[i] = NULL;
		}

	its_LoadedSounds = 0;

	// FSOUND beenden
	//
	FSOUND_Close();

}; // Desktruktor

//---------------------------------------------------------------------------------------
// Name			: InitFMOD
// Aufgabe		: FMOD Initialisieren
// Rückgabewert	: true bei Erfolg, false bei nicht-Erfolg
// Parameter	: keine
//---------------------------------------------------------------------------------------

bool CSoundManager::InitFMOD(SOUNDMANAGER_PARAMETERS smpp)
{
	Protokoll.WriteText ("Initializing FMOD\n", false);

	FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);					// Output-Mode setzen
	FSOUND_SetDriver(0);									// Default-Soundkarte setzen
	FSOUND_SetMixer(FSOUND_MIXER_QUALITY_AUTODETECT);		// Mixer-Quality setzen

	InitSuccessfull = (TRUE == FSOUND_Init(smpp.Mixrate, smpp.MaxSoftwareChannels, smpp.Flags));

	if (false == InitSuccessfull)
	{	
		Protokoll.WriteText ("\n->", false);
		Protokoll.WriteText (GetFMODErrorString(FSOUND_GetError()), false);				
		return false;
	}
	
	MaxChannels = FSOUND_GetMaxChannels();
	Protokoll.WriteText ("Successfull\n", false);

	return true;
} // InitFMOD
	
//---------------------------------------------------------------------------------------
// Name			: SetVolumes
// Aufgabe		: neue globale Volumes setzen
// Rückgabewert	: keiner
// Parameter	: Sound		neue Sound-Lautstärke
//				  Musik		neue Musik-Lautstärke
//---------------------------------------------------------------------------------------

void CSoundManager::SetVolumes(float Sound, float Musik)
{
	its_GlobalSoundVolume = Sound;
	its_GlobalMusicVolume = Musik;
} // SetVolumes

//---------------------------------------------------------------------------------------
// Name			: LoadSong
// Aufgabe		: Song laden
// Rückgabewert	: Liefert Integer Wert der Arraypos des Songs zurück, -1 bei Fehler
// Parameter	: Filename	Name des zu ladenden Songs
//---------------------------------------------------------------------------------------

bool CSoundManager::LoadSong(char *Filename, int Nr)
{
	if (false == InitSuccessfull)
		return false;

	bool			fromrar;	
	char			*pData; 
	char			Temp[256];
	unsigned long	Size; 
	char Buffer[100];	

	fromrar = false;

	// Zuerst checken, ob sich der Song in einem MOD-Ordner befindet
	if (CommandLineParams.RunOwnLevelList == true)
	{
		sprintf_s(Temp, "levels/%s/%s", CommandLineParams.OwnLevelList, Filename);
		if (FileExists(Temp))
			goto loadfile;
	}

	// Dann checken, ob sich das File im Standard Ordner befindet
	sprintf_s(Temp, "data/%s", Filename);
	if (FileExists(Temp))
		goto loadfile;

	// Auch nicht? Dann ist es hoffentlich im RAR file
	if (urarlib_get(&pData, &Size, Filename, RARFILENAME, convertText(RARFILEPASSWORD)) == false)
	{		
		sprintf_s(Temp, "\n-> Error loading %s from Archive !\n", Filename);
		Protokoll.WriteText(Temp, true);
		return false;
	}
	else
		fromrar = true;

loadfile:

	int Nummer = Nr;
	if (its_Songs[Nummer] != NULL)
	{
		delete(its_Songs[Nummer]);
		its_Songs[Nummer] = NULL;
	}
		
	its_Songs[Nummer] = new CSong();

	if (fromrar == false)
		its_Songs[Nummer]->SongData = FMUSIC_LoadSong(Temp);
	else
	{
		its_Songs[Nummer]->SongData = FMUSIC_LoadSongEx(pData, 0, Size, FSOUND_LOADMEMORY, NULL, 0);
		free(pData);
	}

	// Fehler beim Laden ?
	//
	if (its_Songs[Nummer]->SongData == NULL)
	{
		strcpy_s(Buffer, strlen("\n-> Error loading ") + 1, "\n-> Error loading ");
		strcat_s(Buffer, strlen(Filename) + 1, Filename);
		strcat_s(Buffer, 3, "\n");
		Protokoll.WriteText(Buffer, true);

		//return (-1);
		return false;
	}

	its_LoadedSongs++;

	//its_Songs[Nummer].Name = Filename;
	its_Songs[Nummer]->Volume = 100;
	its_Songs[Nummer]->FadingVolume = 0.0f;

	// TODO FIX
	/*
	strcpy_s(Buffer, "Load Song : ");
	strcat_s(Buffer, Filename);
	strcat_s(Buffer, " successfull\n");
	Protokoll.WriteText(Buffer, false);
	*/

	//return Nummer;
	return true;
} // LoadSong

//---------------------------------------------------------------------------------------
// Name			: PlaySong
// Aufgabe		: Song abspielen
// Rückgabewert	: keiner
// Parameter	: Nr		Array-Nummer des Songs
//				  Paused	Neu starten oder Pause aufheben
//---------------------------------------------------------------------------------------

bool CSoundManager::PlaySong(int Nr, bool Paused)
{
	if (false == InitSuccessfull)
		return false;

	if (its_Songs[Nr] == NULL)
		return false;	

	if (Paused == false)
	{
		FMUSIC_PlaySong(its_Songs[Nr]->SongData);			// Von vorne abspielen ?
		its_Songs[Nr]->FadingVolume = 0.0f;
		SetSongVolume(Nr, its_GlobalMusicVolume);
	}
	else
	{
		SetSongVolume(Nr, its_Songs[Nr]->Volume);
		FMUSIC_SetPaused(its_Songs[Nr]->SongData, false);	// oder Pause aufheben ?
	}

	CurrentSongNr = Nr;

	return true;
} // PlaySong

//---------------------------------------------------------------------------------------
// Name			: StopSong
// Aufgabe		: Song anhalten
// Rückgabewert	: false, wenn der Song nicht exisitert, sonst true
// Parameter	: Nr		Array-Nummer des Songs
//				  Paused	Ganz anhalten oder pausieren
//---------------------------------------------------------------------------------------

bool CSoundManager::StopSong(int Nr, bool Paused)
{
	if (its_Songs[Nr] == NULL)
		return false;

	SetSongVolume(Nr, its_Songs[Nr]->Volume);

	if (Paused == false)
		FMUSIC_StopSong(its_Songs[Nr]->SongData);			// Ganz anhalten ?
	else
		FMUSIC_SetPaused(its_Songs[Nr]->SongData, true);		// oder nur pausieren ?

	return true;
} // StopSong

//---------------------------------------------------------------------------------------
// Name			: StopAllSongs
// Aufgabe		: Alle gespielten Songs werden angehalten
// Rückgabewert	: keiner
//---------------------------------------------------------------------------------------

void CSoundManager::StopAllSongs(bool Paused)
{
	for (int i = 0; i < MAX_SONGS; i++)
		if (its_Songs[i] != NULL &&
			FMUSIC_IsPlaying(its_Songs[i]->SongData))
			StopSong(i, Paused);
}

//---------------------------------------------------------------------------------------
// Name			: StopAllLoopedSounds
// Aufgabe		: Alle geloopten Sounds stoppen
// Rückgabewert	: keiner
//---------------------------------------------------------------------------------------

void CSoundManager::StopAllLoopedSounds(void)
{
	for (int i = 0; i < MAX_SOUNDS; i++)
		if (its_Sounds[i] != NULL &&
			its_Sounds[i]->isLooped == true)
			StopWave(i);
}

//---------------------------------------------------------------------------------------
// Name			: StopAllLoopedSounds
// Aufgabe		: Alle geloopten Sounds stoppen
// Rückgabewert	: keiner
//---------------------------------------------------------------------------------------

void CSoundManager::StopAllSounds(void)
{
	for (int i = 0; i < MAX_SOUNDS; i++)
		if (its_Sounds[i] != NULL)
			StopWave(i);
}

//---------------------------------------------------------------------------------------
// Name			: SetSongVolume
// Aufgabe		: Lautstärke eines Songs setzen
// Rückgabewert	: keiner
// Parameter	: Nr		Array-Nummer des Songs
//				  Volume	Zu setzende Lautstärke
//---------------------------------------------------------------------------------------

void CSoundManager::SetSongVolume(int Nr, float Volume)
{
	if (false == InitSuccessfull)
		return;

	its_Songs[Nr]->Volume = Volume;
	FMUSIC_SetMasterVolume(its_Songs[Nr]->SongData, (int)(Volume*its_GlobalMusicVolume/100.0f*2.55f));
} // SetSongVolume

//---------------------------------------------------------------------------------------
// Name			: SetAbsoluteSongVolume
// Aufgabe		: aboslute Lautstärke eines Songs setzen
// Rückgabewert	: keiner
// Parameter	: Nr		Array-Nummer des Songs
//				  Volume	Zu setzende Lautstärke
//---------------------------------------------------------------------------------------

void CSoundManager::SetAbsoluteSongVolume(int Nr, float Volume)
{	
	its_Songs[Nr]->Volume = Volume;
	FMUSIC_SetMasterVolume(its_Songs[Nr]->SongData, (int)(Volume));
} // SetSongVolume

//---------------------------------------------------------------------------------------
// Name			: SetAllSongVolumes
// Aufgabe		: Lautstärke aller Songs setzen
// Rückgabewert	: keiner
// Parameter	: keine
//---------------------------------------------------------------------------------------

void CSoundManager::SetAllSongVolumes(void)
{
	for (int i=0; i<MAX_SONGS; i++)
		if (its_Songs[i] != NULL)
			SetSongVolume(i, its_GlobalMusicVolume);
} // SetAllSongVolumes

//---------------------------------------------------------------------------------------
// Name			: Update
// Aufgabe		: Songs bei Bedarf faden und benutzte Channels zählen usw
// Rückgabewert	: keiner
// Parameter	: keine
//---------------------------------------------------------------------------------------

void CSoundManager::Update(void)
{
	// Sounds durchgehen und updaten
	//
	for (int i=0; i<MAX_SOUNDS; i++)
		if (its_Sounds[i] != NULL)
			its_Sounds[i]->Update();

	// Songs durchgehen und bei Bedarf faden
	//
	for (int i=0; i<MAX_SONGS; i++)
		if (its_Songs[i] != NULL)
			its_Songs[i]->Update();

	// Anzahl benutzter Channels zählen
	//
	ChannelsUsed = 0;

	for (int i=0; i < MaxChannels; i++)
		if (FSOUND_IsPlaying(i))
			ChannelsUsed++;

} // Update

//---------------------------------------------------------------------------------------
// Name			: FadeSong
// Aufgabe		: Fadet einen Song ein/aus und hält danach an bzw pausiert
// Rückgabewert	: keiner
// Parameter	: Nr			Nummer des Songs
//				  Speed			Geschwindigkeit und Richtung des Fades
//				  End			Grenze des Fadens
//---------------------------------------------------------------------------------------

void CSoundManager::FadeSong(int Nr, float Speed, int End, bool Paused)
{
	if (false == InitSuccessfull)
		return;

	its_Songs[Nr]->FadingVolume	= Speed;
	its_Songs[Nr]->FadingEnd	= (int)(its_GlobalMusicVolume * End / 100.0f);
	its_Songs[Nr]->FadingPaused = Paused;

	// Einfaden ? Dann ggf den Song erst mal starten, je nach Angabe
	// neu von Anfang an oder Pausierung aufheben
	if (Speed > 0.0f)
		PlaySong(Nr, Paused);

} // FadeSong

//---------------------------------------------------------------------------------------
// Name			: FadeWave
// Aufgabe		: Wave ein/ausfadem
// Rückgabewert	: keiner
// Parameter	: Nr			Nummer des Waves
//				  Mode			FadeMode
//---------------------------------------------------------------------------------------

void CSoundManager::FadeWave(int Nr, int Mode)
{	
	// Sound soll eingefadet werden?
	if (Mode == FADEMODE_IN)
	{
		// Fadet gerade garnicht? Dann abspielen
		if (its_Sounds[Nr]->FadeMode == FADEMODE_NON)
			PlayWave(1, 128, 11025, Nr);
	}

	// Sound soll ausgefadet werden?
	else
	if (Mode == FADEMODE_OUT)
	{
	}

	its_Sounds[Nr]->FadeMode = Mode;	

} // FadeWave

//---------------------------------------------------------------------------------------
// Name			: LoadWave
// Aufgabe		: Lädt eine Wave Datei
// Rückgabewert	: Nr der Wavedatei im Array
// Parameter	: Filename		Dateiname der Wave
//---------------------------------------------------------------------------------------

bool CSoundManager::LoadWave(char *Filename, int Nr, bool looped)
{
	if (false == InitSuccessfull)
		return false;

	if(GameRunning == false) 
		return false;

	bool			fromrar;	
	char			*pData; 
	char			Temp[256];
	unsigned long	Size; 
	char Buffer[100];	

	fromrar = false;

	int Nummer = Nr;
	if (its_Sounds[Nummer] != NULL)
	{
		delete(its_Sounds[Nummer]);
		its_Sounds[Nummer] = NULL;
	}

	// Zuerst checken, ob sich der Sound in einem MOD-Ordner befindet
	if (CommandLineParams.RunOwnLevelList == true)
	{
		sprintf_s(Temp, "levels/%s/%s", CommandLineParams.OwnLevelList, Filename);
		if (FileExists(Temp))
			goto loadfile;
	}

	// Dann checken, ob sich das File im Standard Ordner befindet
	sprintf_s(Temp, "data/%s", Filename);
	if (FileExists(Temp))
		goto loadfile;

	// Auch nicht? Dann ist es hoffentlich im RAR file
	if (urarlib_get(&pData, &Size, Filename, RARFILENAME, convertText(RARFILEPASSWORD)) == false)
	{		
		sprintf_s(Temp, "\n-> Error loading %s from Archive !\n", Filename);
		Protokoll.WriteText(Temp, true);
		return false;
	}
	else
		fromrar = true;

loadfile:	

	its_Sounds[Nummer] = new CWave();

	// von Platte laden
	if (fromrar == false)
	{
		if (looped == false)
			its_Sounds[Nummer]->SoundData = FSOUND_Sample_Load(Nr, Temp, FSOUND_LOOP_OFF, 0, Size);
		else // mit Loop laden
			its_Sounds[Nummer]->SoundData = FSOUND_Sample_Load(Nr, Temp, FSOUND_LOOP_NORMAL, 0, Size);
	}
	// aus Rarfile laden
	else
	{
		if (looped == false)
			its_Sounds[Nummer]->SoundData = FSOUND_Sample_Load(Nr, pData, FSOUND_LOADMEMORY | FSOUND_LOOP_OFF, 0, Size);
		else // mit Loop laden
			its_Sounds[Nummer]->SoundData = FSOUND_Sample_Load(Nr, pData, FSOUND_LOADMEMORY | FSOUND_LOOP_NORMAL, 0, Size);

		free(pData);
	}

	// Temp Datei wieder löschen und Speicher freigeben
	//DeleteFile("temp.dat");
	//free(pData);

	// Und bei bedarf den loop ausschalten
	//if (looped == false)
	//	FSOUND_Sample_SetLoopMode(its_Sounds[Nummer]->SoundData, FSOUND_LOOP_OFF);

	// Fehler beim Laden ?
	//
	if (its_Sounds[Nummer]->SoundData == NULL)
	{
		// TODO FIX
/*
		strcpy_s(Buffer, "\n-> Error loading ");
		strcat_s(Buffer, Filename);
		strcat_s(Buffer, "\n");
		Protokoll.WriteText(Buffer, true);
*/

	//	return (-1);
		return false;
	}

	its_Sounds[Nummer]->isLooped = looped;

	its_LoadedSounds++;

	//its_Sounds[Nummer].Name = Filename;

	// kurz anspielen und wieder stoppen
	int Channel = FSOUND_PlaySound   (FSOUND_FREE, its_Sounds[Nr]->SoundData);
	FSOUND_SetVolume(Channel, 0);
	FSOUND_StopSound(Channel);

	// TODO FIX
/*
		strcpy_s(Buffer, TextArray [TEXT_LADE_WAVE]);
		strcat_s(Buffer, Filename);
		strcat_s(Buffer, TextArray [TEXT_LADEN_ERFOLGREICH]);
		strcat_s(Buffer, " \n");
		Protokoll.WriteText(Buffer, false);*/
	

	DisplayLoadInfo(Buffer);

	//return Nummer;
	return true;
} // LoadWave

//---------------------------------------------------------------------------------------
// Name			: PlayWave
// Aufgabe		: Spielt eine Wave Datei
// Rückgabewert	: true, wenn der Sound gespielt wurde, sonst false
// Parameter	: Nr des Sounds im Array
//---------------------------------------------------------------------------------------

bool CSoundManager::PlayWave(int Vol, int Pan, int Freq, int Nr)
{
	if (false == InitSuccessfull)
		return false;

	int Channel;		// Benutzter Channel

	// hört man den Sound überhaupt ?
	//
	if(Vol == 0 ||
		its_GlobalSoundVolume == 0)
		return false;

	if(its_Sounds[Nr] == NULL)
		return false;

	// Sound spielen
	//
	Channel = FSOUND_PlaySound   (FSOUND_FREE, its_Sounds[Nr]->SoundData);

	// Kein freier Channel gefunden ?
	//
	if (Channel == -1)
		return false;

	its_Sounds[Nr]->Channel   = Channel;
	its_Sounds[Nr]->isPlaying = true;;

	// Und Werte für den Channel, in dem er gespielt wird, setzen
	//
	FSOUND_SetFrequency(Channel, Freq);
	FSOUND_SetVolume   (Channel, int(its_GlobalSoundVolume*Vol/100.0f*2.55f));
	FSOUND_SetPan	   (Channel, Pan);

	return true;
} // PlayWave

//---------------------------------------------------------------------------------------
// Name			: PlayWave3D
// Aufgabe		: Spielt eine Wave Datei, je nach Spieler Abstand lauter oder leiser
//				  und setzt das panning entsprechend der Position der Geräuschquelle
// Rückgabewert	: true, wenn der Sound gespielt wurde, sonst false
// Parameter	: Nr des Sounds im Array
//---------------------------------------------------------------------------------------

bool CSoundManager::PlayWave3D(int x, int y, int Freq, int Nr)
{
	int   vol, pan;
	float xdiff, ydiff, Abstand;

	xdiff = ((pPlayer[0]->xpos + 45)  - x);
	ydiff = ((pPlayer[0]->ypos + 45)  - y);

	Abstand = float(sqrt((xdiff * xdiff) + (ydiff * ydiff)));

	vol = int(100-float(Abstand/6.0f));
	if (vol < 0)
		vol = 0;
	else
	{
		// Sound links oder rechts vom Spieler ?
		if (x < pPlayer[0]->xpos + 45)
		{
			pan = 128 - (100 - vol);
			if (pan < 0)
				pan = 0;
		}
		else
		{
			pan = 128 + (100 - vol);
			if (pan > 255)
				pan = 255;
		}

		PlayWave(vol, pan, Freq, Nr);
	}

	return true;
} // PlaySound

// --------------------------------------------------------------------------------------
// Lautstärke eines 3D sounds ändern
// --------------------------------------------------------------------------------------

void CSoundManager::Update3D (int x, int y, int Nr)
{
	int   vol, pan;
	float xdiff, ydiff, Abstand;

	xdiff = ((pPlayer[0]->xpos + 45)  - x);
	ydiff = ((pPlayer[0]->ypos + 45)  - y);

	Abstand = float(sqrt((xdiff * xdiff) + (ydiff * ydiff)));

	vol = (int)((100-float(Abstand/6.0f)) / 100.0f * pSoundManager->its_GlobalSoundVolume);

	if (vol < 0)
		vol = 0;
	else

	// neuer Wert ist nicht leiser als der alte?

	{
		// Sound links oder rechts vom Spieler ?
		if (x < pPlayer[0]->xpos + 45)
		{
			pan = 128 - (100 - vol);
			if (pan < 0)
				pan = 0;
		}
		else
		{
			pan = 128 + (100 - vol);
			if (pan > 255)
				pan = 255;
		}

		
		FSOUND_SetVolume (its_Sounds[Nr]->Channel, vol);
		FSOUND_SetPan	 (its_Sounds[Nr]->Channel, 128);		
	}
}

// --------------------------------------------------------------------------------------
// Wavedatei anhalten (falls sie geloopt wird zb)
// --------------------------------------------------------------------------------------

bool CSoundManager::StopWave(int Nr)
{
	if (its_Sounds[Nr] == NULL)
		return false;

	if (its_Sounds[Nr]->isPlaying == false)
		return false;

	FSOUND_StopSound(its_Sounds[Nr]->Channel);

	its_Sounds[Nr]->isPlaying = false;
	its_Sounds[Nr]->Channel	 = -1;

	return true;
}

// --------------------------------------------------------------------------------------
// Alle Songs anhalten (Pause == true) oder wieder abspielen (Pause == false)
// --------------------------------------------------------------------------------------

void CSoundManager::PauseAllSongs(bool bPause)
{
	// alle pausieren
	//
	if (bPause == true)
	{
		// Musik stoppen
		//
		for (int i = 0; i < MAX_SONGS; i++)
		if (its_Songs[i] != NULL)
		{
			if (FMUSIC_IsPlaying(its_Songs[i]->SongData))
				FMUSIC_SetPaused(its_Songs[i]->SongData, true);
		}

		// Sounds stoppen
		//
		for (int i = 0; i < MAX_SOUNDS; i++)
		if (its_Sounds[i] != NULL)
			StopWave(i);			
	}

	// alle wieder spielen lassen
	//
	else
	{
		// Musik weiter spielen
		//
		for (int i = 0; i < MAX_SONGS; i++)
		if (its_Songs[i] != NULL)
		{
				FMUSIC_SetPaused(its_Songs[i]->SongData, false);
		}
	}
}

// --------------------------------------------------------------------------------------
// Alle Songs anhalten, die gerade spielen und diese merken
// --------------------------------------------------------------------------------------

void CSoundManager::PausePlaying(void)
{
	for (int i = 0; i < MAX_SONGS; i++)
	if (its_Songs[i] != NULL)
	{
		WasPlaying[i] = (FMUSIC_IsPlaying(its_Songs[i]->SongData) &&
						 FMUSIC_GetPaused(its_Songs[i]->SongData) == false);

		if (WasPlaying[i])
			FMUSIC_SetPaused(its_Songs[i]->SongData, true);
	}
}

// --------------------------------------------------------------------------------------
// Alle Songs wieder spielen, die vorher angehalten wurden
// --------------------------------------------------------------------------------------

void CSoundManager::PlayPaused(void)
{
	for (int i = 0; i < MAX_SONGS; i++)
	if (its_Songs[i] != NULL)
	{		
		if (WasPlaying[i])
			FMUSIC_SetPaused(its_Songs[i]->SongData, false);
	}
}