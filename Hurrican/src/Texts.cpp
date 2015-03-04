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

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "Texts.h"
#include "Logdatei.h"
#include "Main.h"
#include "Gameplay.h"
#include "GetKeyName.h"
#include "Player.h"
#include "Tileengine.h"
#include "unrarlib.h"

//DKS - Added cross-platform language-files handling:
//      Uses Windows/UNIX-compatible tinydir library to parse directories
//      https://github.com/cxong/tinydir 
//      Copyright (c) 2013, Cong Xu  Simplified BSD License
#include <tinydir/tinydir.h>

using namespace std;

extern Logdatei Protokoll;

char TextArray  [TEXT_LASTTEXT][1024];
char s_Replacers[MAX_AKTIONEN*2][256];

char Cheats[MAX_CHEATS][256] =
{
    "$.345&&",				// 99 Extras = dnstuff
    ")$&!",				// Alle Waffen = idfa
    "\"%.+%.752-53",		// Shield = Benkenwurmus
    "42!6%,,%2`).`4)-%",	// 999 Zeit = traveller in time
    "$%2&.!-`:.%24",		// Godmode = derfnam znert
    "42!),\",!:%2"	,		// Radmode = trailblazer
    "$/.4`34/0`-%`./7",	// Autofire = dont stop me now
    "350%23):%`-%",		// Riesenshot = supersize me
    "42!.4/2`7!3`(%2%"
};	// Flammenwerfer = trantor was here


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
        sprintf_s(temp, "%s/levels/%s/custom.lng", g_storage_ext, CommandLineParams.OwnLevelList);
        if (FileExists(temp))
            goto loadfile;
    }

    //DKS - Fixed language loading (it did not look in lang/ folder at all)
    // First, always try the lang/ folder
    sprintf_s(temp, "%s/lang/%s", g_storage_ext, filename);

    if (FileExists(temp))
        goto loadfile;

    // If not found in the lang/ folder, try the root game folder
    sprintf_s(temp, "%s/%s", g_storage_ext, filename);
    if (!FileExists(temp))
        return false;

loadfile:

    fopen_s(&Datei, temp, "r");												// Language File öffnen
    if (Datei == NULL)															// Fehler beim öffnen ?
    {
        Protokoll.WriteText( true, "-> Error opening language-file\n'%s'", temp );
        return false;
    } else {
        Protokoll.WriteText( false, "Using language-file '%s'\n", temp );
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

        sprintf_s(buf, "%s/levels/%s/levellist.dat", g_storage_ext, CommandLineParams.OwnLevelList);

        fopen_s (&Datei, buf, "rt");							// Reihenfolge Level laden
        if (Datei == NULL)
        {
            // Fehler beim öffnen ? Dann standard Liste öffnen
            CommandLineParams.RunOwnLevelList = false;
            Protokoll.WriteText( true, "-> Error opening level-order file\n" );
            return false;
        }
    }
    else
    {
        char			Temp[256];

        fromrar = false;

        // Checken, ob sich das File im Standard Ordner befindet
        sprintf_s(Temp, "%s/%s", g_storage_ext, "data/levellist.dat");
        if (FileExists(Temp))
            goto loadfilelevel;

#if defined(USE_UNRARLIB)
        char			*pData;
        unsigned long	Size;
        // Nicht? Dann ist es hoffentlich im RAR file
        sprintf_s(Temp, "%s", "levellist.dat");
        if (urarlib_get(&pData, &Size, Temp, RARFILENAME, convertText(RARFILEPASSWORD)) == false)
        {
            Protokoll.WriteText( true, "\n-> Error loading %s from Archive !\n", Temp );
            return false;
        }
        else
            fromrar = true;
#else
        Protokoll.WriteText( true, "\n-> Error loading %s!\n", Temp );
        return false;
#endif // USE_UNRARLIB

loadfilelevel:

#if defined(USE_UNRARLIB)
        // Aus RAR laden? Dann müssen wir das ganze unter temporärem Namen entpacken
        if (fromrar == true)
        {
            // Zwischenspeichern
            //
            FILE *TempFile = NULL;
            fopen_s (&TempFile, TEMP_FILE_PREFIX "temp.dat", "wb");	// Datei öffnen
            fwrite (pData, Size, 1, TempFile);			// speichern
            fclose (TempFile);							// und schliessen

            sprintf_s(Temp, "%s", TEMP_FILE_PREFIX "temp.dat");			// Name anpassen
            free(pData);								// und Speicher freigeben
        }
#endif // USE_UNRARLIB

        in.open(Temp); //Datei öffnen
    }

    // Levels auslesen
    //
    //bool eof = false;
    MAX_LEVELS = 0;

    int index = 0;
    char* line = new char[256];
    while(!in.eof())
    {
        in.getline(line, 256);

        /* CHECKME: Removing CR from line ending (stegerg) */

        char *cr;

        if ((cr = strchr(line, '\r')))
        {
            *cr = '\0';
        }

        strcpy_s(StageReihenfolge[MAX_LEVELS], strlen(line) + 1, line);
        MAX_LEVELS++;
        index++;
    }
    in.close();

    if (fromrar)
    {
        DeleteFile(TEMP_FILE_PREFIX "temp.dat");
    }

    return true;

} // LoadLanguage

//DKS - Added cross-platform language-files handling
// Returns number of matched files
int FindLanguageFiles(char *path)
{
    tinydir_dir dir;
    tinydir_file file;
    int num_matches = 0;

    if (!path) {
        Protokoll.WriteText( false, "ERROR: NULL path passed to find_language_files() in %s\n", __FILE__);
        return 0;
    }

    Protokoll.WriteText( false, "Searching for language files in %s\n", path );
    tinydir_open(&dir, path);

    while (dir.has_next) {
        if (tinydir_readfile(&dir, &file) != -1)
        {
            if (file.is_dir) {
                Protokoll.WriteText( false, "File %s is directory, skipping\n", file.name );
            } else {
                if (strcasecmp(file.extension, "lng") == 0) {
                    if (strlen(file.name) < MAX_LANGUAGE_FILENAME_LENGTH) {
                        Protokoll.WriteText( false, "Found language file %s in path %s\n", file.name, file.path );

                        // If we've found a file more than 5 characters in length, accept it..
                        if (strlen(file.name) > 4) {
                            // Copy filename with extension (no leading path)
                            strcpy_s(LanguageFiles[num_matches], file.name); 
                            num_matches++;
                        }
                    } else {
                        // Either the full pathname was too long or the filename without extension was too long:
                        Protokoll.WriteText( false, "Skipping file with too long a name or full path:\n%s\n", file.path );
                    }
                }
            }
        } else {
            Protokoll.WriteText( false, "Error examining file, skipping\n" );
        }

        tinydir_next(&dir);
    }

    tinydir_close(&dir);
    return num_matches;
}

//DKS - Added function to split a longer line into two shorter lines, for when
//      running on a lower-resolution device w/ scaled font
void SplitLine(char *dst1, char *dst2, char *source)
{
    if (!source || strlen(source) < 10)
        return;

    int split_point = strlen(source) / 2;   // Begin in the middle
    int source_length = strlen(source);
    int i;

    // Find the first space past the middle
    while (split_point < source_length && source[split_point] != ' ') {
        split_point++;
    }
    
    // Copy source string to dst1, up to the split point
    for (i = 0; i < split_point; i++) {
        dst1[i] = source[i];
    }
    dst1[i] = '\0';

    // Copy rest of source string starting one char past the split-point
    int dst2_ctr = 0;
    for (i = split_point+1; i < source_length; i++, dst2_ctr++) {
        dst2[dst2_ctr] = source[i];
    }
    dst2[dst2_ctr] = '\0';
}

//DKS - Added function to split a longer line into one, dst1, sized up to
//      width (in pixels) based on font passed, and one, dst2, is the remainder,
//      minus any leading whitespace. Returns true if line needed to be split.
bool ExtractStringOfLength(char *dst1, char *dst2, char *source, int width, DirectGraphicsFont *font)
{

    if (!source || !dst1 || !dst2 || !font || width <= 0)
        return false;

    if (font->StringLength(source) <= width)
        return false;

    int split_pos = 0;
    int source_len = strlen(source);
    dst1[0] = '\0';
    dst2[0] = '\0';

    while (split_pos < source_len && font->StringLength(dst1) <= width) {
        dst1[split_pos] = source[split_pos];
        dst1[split_pos+1] = '\0';
        split_pos++;
    }

    split_pos--;
    
    // Now, see if we've stopped at a space and if not, backtrack to the previous space.
    while (dst1[split_pos] != ' ' && split_pos >= 0) {
        split_pos--;
    }

    // Replace the space with a newline and NULL terminator
    dst1[split_pos] = '\n';
    dst1[split_pos+1] = '\0';

    // Now, copy the remainder, minus the leading space and including the NULL terminator, into dst2
    strcpy_s(dst2, &source[split_pos+1]);

    return true;
}

//DKS - Added function to replace any instance of a substring with another substring,
//      primarily for use on gaming/phone systems where "key" should be replaced with
//      "button"
void ReplaceAll(std::string& str, const std::string& from, const std::string& to)
{
    if(from.empty())
        return;
    std::string new_str;
    new_str.reserve(str.length());
    size_t start_pos = 0, pos;
    while((pos = str.find(from, start_pos)) != std::string::npos) {
        new_str += str.substr(start_pos, pos - start_pos);
        new_str += to;
        pos += from.length();
        start_pos = pos;
    }
    new_str += str.substr(start_pos);
    str.swap(new_str); 
}

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

    // DKS - Uncommented the replacement text fillers below, the seem to work just fine,
    //          despite having been labelled "TODO FIX"
    // Personal TODO: customize for GCW Zero / display button 0 as button 1 and so forth
    //                  on other devices
    if (pPlayer[0]->ControlType == CONTROLTYPE_KEYBOARD)
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
        int joy_idx = pPlayer[0]->JoystickIndex;
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

        if (pPlayer[0]->JoystickMode == JOYMODE_JOYSTICK)
            sprintf_s(buf, "%s %s", TextArray[TEXT_JOYMODE_PAD + pPlayer[0]->JoystickMode - 1], TextArray[TEXT_UP]);
        else
            sprintf_s(buf, "%s %s", TextArray[TEXT_BUTTON], 
                    DirectInput.MapButtonToString(joy_idx, pPlayer[0]->AktionJoystick[AKTION_JUMP]));

        strcpy (s_Replacers[11], buf);

        sprintf_s(buf, "%s %s", TextArray[TEXT_BUTTON], 
                DirectInput.MapButtonToString(joy_idx, pPlayer[0]->AktionJoystick[AKTION_SHOOT]));
        strcpy (s_Replacers[13], buf);

        sprintf_s(buf, "%s %s", TextArray[TEXT_BUTTON], 
                DirectInput.MapButtonToString(joy_idx, pPlayer[0]->AktionJoystick[AKTION_BLITZ]));
        strcpy (s_Replacers[15], buf);

        sprintf_s(buf, "%s %s", TextArray[TEXT_BUTTON], 
                DirectInput.MapButtonToString(joy_idx, pPlayer[0]->AktionJoystick[AKTION_POWERLINE]));
        strcpy (s_Replacers[17], buf);

        sprintf_s(buf, "%s %s", TextArray[TEXT_BUTTON], 
                DirectInput.MapButtonToString(joy_idx, pPlayer[0]->AktionJoystick[AKTION_GRANATE]));
        strcpy (s_Replacers[19], buf);

        sprintf_s(buf, "%s %s", TextArray[TEXT_BUTTON], 
                DirectInput.MapButtonToString(joy_idx, pPlayer[0]->AktionJoystick[AKTION_SMARTBOMB]));
        strcpy (s_Replacers[21], buf);

        sprintf_s(buf, "%s %s", TextArray[TEXT_BUTTON], 
                DirectInput.MapButtonToString(joy_idx, pPlayer[0]->AktionJoystick[AKTION_WAFFEN_CYCLE]));
        strcpy (s_Replacers[23], buf);

        sprintf_s(buf, "%s %s", TextArray[TEXT_BUTTON], 
                DirectInput.MapButtonToString(joy_idx, pPlayer[0]->AktionJoystick[AKTION_WAFFE_SPREAD]));
        strcpy (s_Replacers[25], buf);

        sprintf_s(buf, "%s %s", TextArray[TEXT_BUTTON], 
                DirectInput.MapButtonToString(joy_idx, pPlayer[0]->AktionJoystick[AKTION_WAFFE_LASER]));
        strcpy (s_Replacers[27], buf);

        sprintf_s(buf, "%s %s", TextArray[TEXT_BUTTON], 
                DirectInput.MapButtonToString(joy_idx, pPlayer[0]->AktionJoystick[AKTION_WAFFE_BOUNCE]));
        strcpy (s_Replacers[29], buf);
    }
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
        case '0' :
            f = 0;
            break;
        case '1' :
            f = 1;
            break;
        case '2' :
            f = 2;
            break;
        case '3' :
            f = 3;
            break;
        case '4' :
            f = 4;
            break;
        case '5' :
            f = 5;
            break;
        case '6' :
            f = 6;
            break;
        case '7' :
            f = 7;
            break;
        case '8' :
            f = 8;
            break;
        case '9' :
            f = 9;
            break;
        case 'A' :
            f = 10;
            break;
        case 'B' :
            f = 11;
            break;
        case 'C' :
            f = 12;
            break;
        case 'D' :
            f = 13;
            break;
        case 'E' :
            f = 14;
            break;
        case 'F' :
            f = 15;
            break;
        }

        for (int j = 0; j < i; j++)
            f *= 16;

        r += f;
    }

    return r;
}
