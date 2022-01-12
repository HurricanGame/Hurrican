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

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
namespace fs = std::filesystem;

#include "Gameplay.hpp"
#include "GetKeyName.hpp"
#include "Logdatei.hpp"
#include "Main.hpp"
#include "Player.hpp"
#include "Texts.hpp"
#include "Tileengine.hpp"

#ifdef USE_UNRARLIB
#include "unrarlib.h"
#endif

extern Logdatei Protokoll;

std::vector<std::string> LanguageFiles;
char ActualLanguage[256];  // Gewählte Language
char TextArray[TEXT::LASTTEXT][1024];
char s_Replacers[MAX_AKTIONEN * 2][256];

char Cheats[MAX_CHEATS][256] = {"$.345&&",            // 99 Extras = dnstuff
                                ")$&!",               // Alle Waffen = idfa
                                "\"%.+%.752-53",      // Shield = Benkenwurmus
                                "42!6%,,%2`).`4)-%",  // 999 Zeit = traveller in time
                                "$%2&.!-`:.%24",      // Godmode = derfnam znert
                                "42!),\",!:%2",       // Radmode = trailblazer
                                "$/.4`34/0`-%`./7",   // Autofire = dont stop me now
                                "350%23):%`-%",       // Riesenshot = supersize me
                                "42!.4/2`7!3`(%2%"};  // Flammenwerfer = trantor was here

// --------------------------------------------------------------------------------------
// Bestimmte Sprachdatei laden
// --------------------------------------------------------------------------------------
//
bool LoadLanguage(char *filename) {
    bool fromrar = false;
    std::ifstream in;
    char c;
    int count;
    std::string temp;

    if (CommandLineParams.RunOwnLevelList) {
        temp = g_storage_ext +  "/data/levels/" + CommandLineParams.OwnLevelList + "/custom.lng";
        if (fs::exists(temp) && fs::is_regular_file(temp))
            goto loadfile;
    }

    // DKS - Fixed language loading (it did not look in lang/ folder at all)
    // First, always try the lang/ folder
    temp = g_storage_ext + "/lang/" + filename;

    if (fs::exists(temp) && fs::is_regular_file(temp))
        goto loadfile;

    // If not found in the lang/ folder, try the root game folder
    temp = g_storage_ext + "/" + filename;
    if (!fs::exists(temp) && fs::is_regular_file(temp))
        return false;

loadfile:

    std::ifstream Datei(temp);  // Language File öffnen
    if (!Datei)                 // Fehler beim öffnen ?
    {
        Protokoll << "-> Error opening language-file\n";
        Protokoll << "'" << temp << "'" << std::endl;
        GameRunning = false;
        return false;
    } else {
        Protokoll << "Using language-file '" << temp << "'" << std::endl;
    }

    // Texte auslesen
    //
    for (int i = 0; i < TEXT::LASTTEXT - 2; i++) {
        strcpy_s(TextArray[i], 1, "");
        count = 0;
        c = Datei.get();

        // Zeichen für Zeichen auslesen, bis ein c==10 gelesen wird. Dies markiert das Zeilenende
        //
        while (c != 10 && count < 1023) {
            TextArray[i][count] = c;
            c = Datei.get();
            count++;
        }

        TextArray[i][count] = '\0';
    }

    Datei.close();

    //
    // Stage Reihenfolge laden
    //
    //
    if (CommandLineParams.RunOwnLevelList) {
        std::string levelfile = g_storage_ext + "/data/levels/" + CommandLineParams.OwnLevelList + "/levellist.dat";

        in.open(levelfile);  // Reihenfolge Level laden
        if (!in) {
            // Fehler beim öffnen ? Dann standard Liste öffnen
            CommandLineParams.RunOwnLevelList = false;
            Protokoll << "-> Error opening level-order file" << levelfile << std::endl;
            GameRunning = false;
            return false;
        }
    } else {
        std::string Temp;

        fromrar = false;

        // DKS - levellist.dat now resides in its new subfolder, data/levels/levellist.dat, along with
        //      the rest of the level data.
        // Checken, ob sich das File im Standard Ordner befindet
        Temp = g_storage_ext + "/data/levels/levellist.dat";
        if (fs::exists(Temp) && fs::is_regular_file(Temp))
            goto loadfilelevel;

#if defined(USE_UNRARLIB)
        char *pData;
        unsigned long Size;
        // Nicht? Dann ist es hoffentlich im RAR file
        sprintf_s(Temp, "%s", "levellist.dat");
        if (urarlib_get(&pData, &Size, Temp, RARFILENAME, convertText(RARFILEPASSWORD)) == false) {
            Protokoll << "\n-> Error loading " << Temp << " from Archive !" << std::endl;
            GameRunning = false;
            return false;
        } else
            fromrar = true;
#else
        Protokoll << "\n-> Error loading " << Temp << "!" << std::endl;
        GameRunning = false;
        return false;
#endif  // USE_UNRARLIB

    loadfilelevel:

#if defined(USE_UNRARLIB)
        // Aus RAR laden? Dann müssen wir das ganze unter temporärem Namen entpacken
        if (fromrar == true) {
            // Zwischenspeichern
            //
            FILE *TempFile = nullptr;
            fopen_s(&TempFile, TEMP_FILE_PREFIX "temp.dat", "wb");  // Datei öffnen
            fwrite(pData, Size, 1, TempFile);                       // speichern
            fclose(TempFile);                                       // und schliessen

            sprintf_s(Temp, "%s", TEMP_FILE_PREFIX "temp.dat");  // Name anpassen
            free(pData);                                         // und Speicher freigeben
        }
#endif  // USE_UNRARLIB

        in.open(Temp);  // Datei öffnen
    }

    // Levels auslesen
    //
    MAX_LEVELS = 0;

    while (!in.eof()) {
        char tmpchar[256];
        in.getline(tmpchar, 256);

        /* CHECKME: Removing CR from line ending (stegerg) */
        char *cr;
        if ((cr = strchr(tmpchar, '\r'))) {
            *cr = '\0';
        }

        if (strlen(tmpchar) > 0) {  // DKS - Added check for strlen > 0
            StageReihenfolge[MAX_LEVELS] = tmpchar;
            MAX_LEVELS++;
        }
    }
    in.close();

    if (fromrar) {
        fs::remove(fs::path(TEMP_FILE_PREFIX "temp.dat"));
    }

    return true;

}  // LoadLanguage

void FindLanguageFiles(const char *path) {
    if (!path) {
        Protokoll << "ERROR: NULL path passed to find_language_files() in " << __FILE__ << std::endl;
    }

    Protokoll << "Searching for language files in " << path << std::endl;

    for (auto &file : fs::directory_iterator(path)) {
        if (!fs::is_regular_file(file)) {
            Protokoll << "File " << file.path().filename() << " is not a regular file, skipping" << std::endl;
            continue;
        }

        if (file.path().extension() == ".lng") {
            Protokoll << "Found language file " << file.path().filename() << " in path " << path << std::endl;
            LanguageFiles.push_back(file.path().filename().string());
        }
    }
}

// DKS - Added function to split a longer line into two shorter lines, for when
//      running on a lower-resolution device w/ scaled font
void SplitLine(char *dst1, char *dst2, const char *source) {
    if (!source || strlen(source) < 10)
        return;

    int split_point = strlen(source) / 2;  // Begin in the middle
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
    for (i = split_point + 1; i < source_length; i++, dst2_ctr++) {
        dst2[dst2_ctr] = source[i];
    }
    dst2[dst2_ctr] = '\0';
}

// DKS - Added function to split a longer line into one, dst1, sized up to
//      width (in pixels) based on font passed, and one, dst2, is the remainder,
//      minus any leading whitespace. Returns true if line needed to be split.
bool ExtractStringOfLength(char *dst1, char *dst2, char *source, int width, DirectGraphicsFont *font) {
    if (!source || !dst1 || !dst2 || !font || width <= 0)
        return false;

    if (font->StringLength(source) <= width)
        return false;

    int split_pos = 0;
    int const source_len = strlen(source);
    dst1[0] = '\0';
    dst2[0] = '\0';

    while (split_pos < source_len && font->StringLength(dst1) <= width) {
        dst1[split_pos] = source[split_pos];
        dst1[split_pos + 1] = '\0';
        split_pos++;
    }

    split_pos--;

    // Now, see if we've stopped at a space and if not, backtrack to the previous space.
    while (dst1[split_pos] != ' ' && split_pos >= 0) {
        split_pos--;
    }

    // Replace the space with a newline and NULL terminator
    dst1[split_pos] = '\n';
    dst1[split_pos + 1] = '\0';

    // Now, copy the remainder, minus the leading space and including the NULL terminator, into dst2
    strcpy(dst2, &source[split_pos + 1]);

    return true;
}

// DKS - Added function to replace any instance of a substring with another substring,
//      primarily for use on gaming/phone systems where "key" should be replaced with
//      "button"
void ReplaceAll(std::string &str, const std::string &from, const std::string &to) {
    if (str.empty() || from.empty())
        return;
    std::string new_str;
    new_str.reserve(str.length());
    size_t start_pos = 0, pos;
    while ((pos = str.find(from, start_pos)) != std::string::npos) {
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

void InitReplacers() {
    strcpy_s(s_Replacers[0], strlen("KEY_LEFT") + 1, "KEY_LEFT");
    strcpy_s(s_Replacers[2], strlen("KEY_RIGHT") + 1, "KEY_RIGHT");
    strcpy_s(s_Replacers[4], strlen("KEY_CROUCH") + 1, "KEY_CROUCH");
    strcpy_s(s_Replacers[6], strlen("KEY_UP") + 1, "KEY_UP");
    strcpy_s(s_Replacers[8], strlen("KEY_DOWN") + 1, "KEY_DOWN");
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
    if (Player[0].ControlType == CONTROLTYPE_KEYBOARD) {
        strcpy(s_Replacers[1], GetKeyName(Player[0].AktionKeyboard[AKTION_LINKS]));
        strcpy(s_Replacers[3], GetKeyName(Player[0].AktionKeyboard[AKTION_RECHTS]));
        strcpy(s_Replacers[5], GetKeyName(Player[0].AktionKeyboard[AKTION_DUCKEN]));
        strcpy(s_Replacers[7], GetKeyName(Player[0].AktionKeyboard[AKTION_OBEN]));
        strcpy(s_Replacers[9], GetKeyName(Player[0].AktionKeyboard[AKTION_UNTEN]));
        strcpy(s_Replacers[11], GetKeyName(Player[0].AktionKeyboard[AKTION_JUMP]));
        strcpy(s_Replacers[13], GetKeyName(Player[0].AktionKeyboard[AKTION_SHOOT]));
        strcpy(s_Replacers[15], GetKeyName(Player[0].AktionKeyboard[AKTION_BLITZ]));
        strcpy(s_Replacers[17], GetKeyName(Player[0].AktionKeyboard[AKTION_POWERLINE]));
        strcpy(s_Replacers[19], GetKeyName(Player[0].AktionKeyboard[AKTION_GRANATE]));
        strcpy(s_Replacers[21], GetKeyName(Player[0].AktionKeyboard[AKTION_SMARTBOMB]));
        strcpy(s_Replacers[23], GetKeyName(Player[0].AktionKeyboard[AKTION_WAFFEN_CYCLE]));
        strcpy(s_Replacers[25], GetKeyName(Player[0].AktionKeyboard[AKTION_WAFFE_SPREAD]));
        strcpy(s_Replacers[27], GetKeyName(Player[0].AktionKeyboard[AKTION_WAFFE_LASER]));
        strcpy(s_Replacers[29], GetKeyName(Player[0].AktionKeyboard[AKTION_WAFFE_BOUNCE]));
    } else {
        int joy_idx = Player[0].JoystickIndex;
        std::string buf;
        buf.assign(TextArray[TEXT::JOYMODE_PAD + Player[0].JoystickMode - 1]).append(" ").append(TextArray[TEXT::LEFT]);
        strcpy(s_Replacers[1], buf.c_str());

        buf.assign(TextArray[TEXT::JOYMODE_PAD + Player[0].JoystickMode - 1]).append(" ").append(TextArray[TEXT::RIGHT]);
        strcpy(s_Replacers[3], buf.c_str());

        buf.assign(TextArray[TEXT::JOYMODE_PAD + Player[0].JoystickMode - 1]).append(" ").append(TextArray[TEXT::DOWN]);
        strcpy(s_Replacers[5], buf.c_str());

        buf.assign(TextArray[TEXT::JOYMODE_PAD + Player[0].JoystickMode - 1]).append(" ").append(TextArray[TEXT::UP]);
        strcpy(s_Replacers[7], buf.c_str());

        buf.assign(TextArray[TEXT::JOYMODE_PAD + Player[0].JoystickMode - 1]).append(" ").append(TextArray[TEXT::DOWN]);
        strcpy(s_Replacers[9], buf.c_str());

        if (Player[0].JoystickMode == JOYMODE_JOYSTICK)
            buf.assign(TextArray[TEXT::JOYMODE_PAD + Player[0].JoystickMode - 1]).append(" ").append(TextArray[TEXT::UP]);
        else
            buf.assign(TextArray[TEXT::BUTTON]).append(" ").append(
                      DirectInput.MapButtonToString(joy_idx, Player[0].AktionJoystick[AKTION_JUMP]));

        strcpy(s_Replacers[11], buf.c_str());

        buf.assign(TextArray[TEXT::BUTTON]).append(" ").append(
                  DirectInput.MapButtonToString(joy_idx, Player[0].AktionJoystick[AKTION_SHOOT]));
        strcpy(s_Replacers[13], buf.c_str());

        buf.assign(TextArray[TEXT::BUTTON]).append(" ").append(
                  DirectInput.MapButtonToString(joy_idx, Player[0].AktionJoystick[AKTION_BLITZ]));
        strcpy(s_Replacers[15], buf.c_str());

        buf.assign(TextArray[TEXT::BUTTON]).append(" ").append(
                  DirectInput.MapButtonToString(joy_idx, Player[0].AktionJoystick[AKTION_POWERLINE]));
        strcpy(s_Replacers[17], buf.c_str());

        buf.assign(TextArray[TEXT::BUTTON]).append(" ").append(
                  DirectInput.MapButtonToString(joy_idx, Player[0].AktionJoystick[AKTION_GRANATE]));
        strcpy(s_Replacers[19], buf.c_str());

        buf.assign(TextArray[TEXT::BUTTON]).append(" ").append(
                  DirectInput.MapButtonToString(joy_idx, Player[0].AktionJoystick[AKTION_SMARTBOMB]));
        strcpy(s_Replacers[21], buf.c_str());

        buf.assign(TextArray[TEXT::BUTTON]).append(" ").append(
                  DirectInput.MapButtonToString(joy_idx, Player[0].AktionJoystick[AKTION_WAFFEN_CYCLE]));
        strcpy(s_Replacers[23], buf.c_str());

        buf.assign(TextArray[TEXT::BUTTON]).append(" ").append(
                  DirectInput.MapButtonToString(joy_idx, Player[0].AktionJoystick[AKTION_WAFFE_SPREAD]));
        strcpy(s_Replacers[25], buf.c_str());

        buf.assign(TextArray[TEXT::BUTTON]).append(" ").append(
                  DirectInput.MapButtonToString(joy_idx, Player[0].AktionJoystick[AKTION_WAFFE_LASER]));
        strcpy(s_Replacers[27], buf.c_str());

        buf.assign(TextArray[TEXT::BUTTON]).append(" ").append(
                  DirectInput.MapButtonToString(joy_idx, Player[0].AktionJoystick[AKTION_WAFFE_BOUNCE]));
        strcpy(s_Replacers[29], buf.c_str());
    }
}
