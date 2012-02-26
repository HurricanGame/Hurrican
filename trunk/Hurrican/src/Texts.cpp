// Datei : Texts.cpp

// -------------------------------------------------------------------------------------- 
//
// Beinhaltet die Defines und Funktionen für die Texte in Hurrican
// Es ist möglich, verschiedene Language-Files zu laden
//
// (c) 2002-2004 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "Texts.h"
#include <stdio.h>
#include <string.h>
#include "Logdatei.h"
#include "Main.h"
#include "Gameplay.h"
#include "GetKeyName.h"
#include "Player.h"
#include "TileEngine.h"
#include "Unrarlib.h"

#include <iostream>
#include <fstream>

using namespace std;

extern Logdatei Protokoll;

char TextArray  [TEXT_LASTTEXT][1024];
char s_Replacers[MAX_AKTIONEN*2][256];

char Cheats[MAX_CHEATS][256] = 
			{"$.345&&",				// 99 Extras = dnstuff
			 ")$&!",				// Alle Waffen = idfa
			 "\"%.+%.752-53",		// Shield = Benkenwurmus			 
			 "42!6%,,%2`).`4)-%",	// 999 Zeit = traveller in time
			 "$%2&.!-`:.%24",		// Godmode = derfnam znert
			 "42!),\",!:%2"	,		// Radmode = trailblazer
			 "$/.4`34/0`-%`./7",	// Autofire = dont stop me now
			 "350%23):%`-%",		// Riesenshot = supersize me
			 "42!.4/2`7!3`(%2%"};	// Flammenwerfer = trantor was here


// --------------------------------------------------------------------------------------
// Bestimmte Sprachdatei laden
// --------------------------------------------------------------------------------------
//
bool LoadLanguage (char *filename)
{
	bool fromrar = false;
	FILE *Datei = NULL;
	ifstream in;
	char c;
	int  count;
	char temp[256];

	if (CommandLineParams.RunOwnLevelList == true)
	{
		sprintf_s(temp, "levels/%s/custom.lng", CommandLineParams.OwnLevelList);
		if (FileExists(temp))
			goto loadfile;
	}

	sprintf_s(temp, "%s", filename);

loadfile:

	fopen_s(&Datei, temp, "r");												// Language File öffnen
	if (Datei == NULL)															// Fehler beim öffnen ?
	{
		char buf[100];

		sprintf_s (buf, "-> Error opening language-file\n'%s'", filename);
		Protokoll.WriteText(buf, true);
		return false;
	}

	// Texte auslesen
	//
	for (int i = 0; i < TEXT_LASTTEXT - 2; i++)
	{		
		strcpy_s (TextArray [i], 1, "");
		count = 0;
		c	  = getc (Datei);

		// Zeichen für Zeichen auslesen, bis ein c==10 gelesen wird. Dies markiert das Zeilenende
		//
		while (c != 10 && 
			   count < 1023)
		{
			TextArray [i][count]= c;
			c = getc (Datei);
			count++;
		}

		TextArray [i][count]= '\0';
	} 

	fclose (Datei);


	//
	// Stage Reihenfolge laden
	//
	//
	if (CommandLineParams.RunOwnLevelList == true)
	{
		char buf[256];

		sprintf_s(buf, "levels/%s/levellist.dat", CommandLineParams.OwnLevelList);

		fopen_s (&Datei, buf, "rt");							// Reihenfolge Level laden
		if (Datei == NULL)				
		{
			// Fehler beim öffnen ? Dann standard Liste öffnen			
			CommandLineParams.RunOwnLevelList = false;
			Protokoll.WriteText("-> Error opening level-order file\n", true);
			return false;
		}
	}
	else
	{		
		char			*pData; 
		char			Temp[256];
		unsigned long	Size; 

		fromrar = false;

		// Checken, ob sich das File im Standard Ordner befindet
		sprintf_s(Temp, "%s", "data/levellist.dat");
		if (FileExists(Temp))
			goto loadfilelevel;

		// Nicht? Dann ist es hoffentlich im RAR file
		sprintf_s(Temp, "%s", "levellist.dat");
		if (urarlib_get(&pData, &Size, Temp, RARFILENAME, convertText(RARFILEPASSWORD)) == false)
		{		
			sprintf_s(Temp, "\n-> Error loading %s from Archive !\n", Temp);
			Protokoll.WriteText(Temp, true);
			return false;
		}
		else
			fromrar = true;

loadfilelevel:

		// Aus RAR laden? Dann müssen wir das ganze unter temporärem Namen entpacken
		if (fromrar == true)
		{
			// Zwischenspeichern
			//
			FILE *TempFile = NULL;
			fopen_s (&TempFile, "temp.dat", "wb");	// Datei öffnen
			fwrite (pData, Size, 1, TempFile);			// speichern
			fclose (TempFile);							// und schliessen
			
			sprintf_s(Temp, "%s", "temp.dat");			// Name anpassen		
			free(pData);								// und Speicher freigeben
		}		
		
		in.open(Temp); //Datei öffnen
	}

	// Levels auslesen
	//
	bool eof = false;
	MAX_LEVELS = 0;

	int index = 0;
	char* line = new char[256];
	while(!in.eof())
	{
		in.getline(line, 256);
		strcpy_s(StageReihenfolge[MAX_LEVELS], strlen(line) + 1, line);
		MAX_LEVELS++;
		index++;
	}
	in.close();

	if (fromrar)
		DeleteFile("temp.dat");

	return true;

} // LoadLanguage

// --------------------------------------------------------------------------------------
// Tasten ErsetzungsStrings für die TutorialTexte initialisieren
// --------------------------------------------------------------------------------------

void InitReplacers(void)
{	
	strcpy_s(s_Replacers[0],  strlen("KEY_LEFT") + 1, "KEY_LEFT");
	strcpy_s(s_Replacers[2],  strlen("KEY_RIGHT") + 1, "KEY_RIGHT");			
	strcpy_s(s_Replacers[4],  strlen("KEY_CROUCH") + 1, "KEY_CROUCH");			
	strcpy_s(s_Replacers[6],  strlen("KEY_UP") + 1, "KEY_UP");				
	strcpy_s(s_Replacers[8],  strlen("KEY_DOWN") + 1, "KEY_DOWN");			
	strcpy_s(s_Replacers[10], strlen("KEY_JUMP") + 1, "KEY_JUMP");			
	strcpy_s(s_Replacers[12], strlen("KEY_SHOOT") + 1, "KEY_SHOOT");			
	strcpy_s(s_Replacers[14], strlen("KEY_BLITZ") + 1, "KEY_BLITZ");			
	strcpy_s(s_Replacers[16], strlen("KEY_POWERLINE") + 1, "KEY_POWERLINE");		
	strcpy_s(s_Replacers[18], strlen("KEY_GRANATE") + 1, "KEY_GRANATE");		
	strcpy_s(s_Replacers[20], strlen("KEY_SHOCKWAVE") + 1, "KEY_SHOCKWAVE");		
	strcpy_s(s_Replacers[22], strlen("KEY_WAFFE_CYCLE") + 1, "KEY_WAFFE_CYCLE");
	strcpy_s(s_Replacers[24], strlen("KEY_WAFFE_SPREAD") + 1, "KEY_WAFFE_SPREAD");	
	strcpy_s(s_Replacers[26], strlen("KEY_WAFFE_LASER") + 1, "KEY_WAFFE_LASER");	
	strcpy_s(s_Replacers[28], strlen("KEY_WAFFE_BOUNCE") + 1, "KEY_WAFFE_BOUNCE");	

	// TODO FIX

/*
	if (pPlayer[0]->ControlType == JOYMODE_KEYBOARD)
	{
		strcpy (s_Replacers[1],  GetKeyName(pPlayer[0]->AktionKeyboard[AKTION_LINKS]));
		strcpy (s_Replacers[3],  GetKeyName(pPlayer[0]->AktionKeyboard[AKTION_RECHTS]));
		strcpy (s_Replacers[5],  GetKeyName(pPlayer[0]->AktionKeyboard[AKTION_DUCKEN]));
		strcpy (s_Replacers[7],  GetKeyName(pPlayer[0]->AktionKeyboard[AKTION_OBEN]));
		strcpy (s_Replacers[9],  GetKeyName(pPlayer[0]->AktionKeyboard[AKTION_UNTEN]));
		strcpy (s_Replacers[11], GetKeyName(pPlayer[0]->AktionKeyboard[AKTION_JUMP]));
		strcpy (s_Replacers[13], GetKeyName(pPlayer[0]->AktionKeyboard[AKTION_SHOOT]));
		strcpy (s_Replacers[15], GetKeyName(pPlayer[0]->AktionKeyboard[AKTION_BLITZ]));
		strcpy (s_Replacers[17], GetKeyName(pPlayer[0]->AktionKeyboard[AKTION_POWERLINE]));
		strcpy (s_Replacers[19], GetKeyName(pPlayer[0]->AktionKeyboard[AKTION_GRANATE]));
		strcpy (s_Replacers[21], GetKeyName(pPlayer[0]->AktionKeyboard[AKTION_SMARTBOMB]));
		strcpy (s_Replacers[23], GetKeyName(pPlayer[0]->AktionKeyboard[AKTION_WAFFEN_CYCLE]));
		strcpy (s_Replacers[25], GetKeyName(pPlayer[0]->AktionKeyboard[AKTION_WAFFE_SPREAD]));
		strcpy (s_Replacers[27], GetKeyName(pPlayer[0]->AktionKeyboard[AKTION_WAFFE_LASER]));
		strcpy (s_Replacers[29], GetKeyName(pPlayer[0]->AktionKeyboard[AKTION_WAFFE_BOUNCE]));		
	}
	else
	{
		char buf[256];
		sprintf_s(buf, "%s %s", TextArray[TEXT_JOYMODE_PAD + pPlayer[0]->JoystickMode - 1], TextArray[TEXT_LEFT]);
		strcpy (s_Replacers[1], buf);

		sprintf_s(buf, "%s %s", TextArray[TEXT_JOYMODE_PAD + pPlayer[0]->JoystickMode - 1], TextArray[TEXT_RIGHT]);
		strcpy (s_Replacers[3], buf);

		sprintf_s(buf, "%s %s", TextArray[TEXT_JOYMODE_PAD + pPlayer[0]->JoystickMode - 1], TextArray[TEXT_DOWN]);
		strcpy (s_Replacers[5], buf);

		sprintf_s(buf, "%s %s", TextArray[TEXT_JOYMODE_PAD + pPlayer[0]->JoystickMode - 1], TextArray[TEXT_UP]);
		strcpy (s_Replacers[7], buf);

		sprintf_s(buf, "%s %s", TextArray[TEXT_JOYMODE_PAD + pPlayer[0]->JoystickMode - 1], TextArray[TEXT_DOWN]);
		strcpy (s_Replacers[9], buf);

		if (pPlayer[0]->JoystickMode == JOYMODE_STICK)
			sprintf_s(buf, "%s %s", TextArray[TEXT_JOYMODE_PAD + pPlayer[0]->JoystickMode - 1], TextArray[TEXT_UP]);
		else
			sprintf_s(buf, "%s %d", TextArray[TEXT_BUTTON], pPlayer[0]->AktionJoystick[AKTION_JUMP]);

		strcpy (s_Replacers[11], buf);

		sprintf_s(buf, "%s %d", TextArray[TEXT_BUTTON], pPlayer[0]->AktionJoystick[AKTION_SHOOT]);
		strcpy (s_Replacers[13], buf);

		sprintf_s(buf, "%s %d", TextArray[TEXT_BUTTON], pPlayer[0]->AktionJoystick[AKTION_BLITZ]);
		strcpy (s_Replacers[15], buf);

		sprintf_s(buf, "%s %d", TextArray[TEXT_BUTTON], pPlayer[0]->AktionJoystick[AKTION_POWERLINE]);
		strcpy (s_Replacers[17], buf);

		sprintf_s(buf, "%s %d", TextArray[TEXT_BUTTON], pPlayer[0]->AktionJoystick[AKTION_GRANATE]);
		strcpy (s_Replacers[19], buf);

		sprintf_s(buf, "%s %d", TextArray[TEXT_BUTTON], pPlayer[0]->AktionJoystick[AKTION_SMARTBOMB]);
		strcpy (s_Replacers[21], buf);

		sprintf_s(buf, "%s %d", TextArray[TEXT_BUTTON], pPlayer[0]->AktionJoystick[AKTION_WAFFEN_CYCLE]);
		strcpy (s_Replacers[23], buf);

		sprintf_s(buf, "%s %d", TextArray[TEXT_BUTTON], pPlayer[0]->AktionJoystick[AKTION_WAFFE_SPREAD]);
		strcpy (s_Replacers[25], buf);

		sprintf_s(buf, "%s %d", TextArray[TEXT_BUTTON], pPlayer[0]->AktionJoystick[AKTION_WAFFE_LASER]);
		strcpy (s_Replacers[27], buf);

		sprintf_s(buf, "%s %d", TextArray[TEXT_BUTTON], pPlayer[0]->AktionJoystick[AKTION_WAFFE_BOUNCE]);
		strcpy (s_Replacers[29], buf);		
	}
*/
}

// --------------------------------------------------------------------------------------
// Dec Value einer Hex zahl ermitteln
// --------------------------------------------------------------------------------------

int GetDecValue(char *pair, int len)
{
	int r = 0;
	int f = 0;
	char c;

	for (int i = 0; i < len; i++)
	{
		c = pair[i];

		switch (c)
		{
			case '0' : f = 0; break;
			case '1' : f = 1; break;
			case '2' : f = 2; break;
			case '3' : f = 3; break;
			case '4' : f = 4; break;
			case '5' : f = 5; break;
			case '6' : f = 6; break;
			case '7' : f = 7; break;
			case '8' : f = 8; break;
			case '9' : f = 9; break;
			case 'A' : f = 10; break;
			case 'B' : f = 11; break;
			case 'C' : f = 12; break;
			case 'D' : f = 13; break;
			case 'E' : f = 14; break;
			case 'F' : f = 15; break;			
		}

		for (int j = 0; j < i; j++)
			f *= 16;

		r += f;
	}

	return r;
}