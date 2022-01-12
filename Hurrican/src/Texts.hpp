// Datei : Texts.hpp

// --------------------------------------------------------------------------------------
//
// Beinhaltet die Defines und Funktionen für die Texte in Hurrican
// Es ist möglich, verschiedene Language-Files zu laden
//
// (c) 2002-2004 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _TEXTS_HPP_
#define _TEXTS_HPP_

#include <iostream>
#include <string>
#include "DX8Font.hpp"
#include "Main.hpp"
#include "Player.hpp"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

constexpr int MAX_CHEATS = 10;

namespace CHEAT {

enum {
  EXTRAS = 0,
  WAFFEN = 1,
  SCHILD = 2,
  ZEIT = 3,
  GOD = 4,
  RAD = 5,
  AUTOFIRE = 6,
  SUPERSHOT = 7,
  FLAMER = 8,
};

} // namespace

// Enum für die verschiedenen Texte
// es wird nie konkret ein String im Code angegeben, sondern immer nur eine Zahl, welche
// einen Array-Eintrag im Text Array angibt.

namespace TEXT {

enum {
    // Header des Language Files
  HEADER,
  HEADER2,
  HEADER3,
  HEADER4,
  HEADER5,
  HEADER6,
  HEADER7,
  HEADER8,
  HEADER9,

  SEPERATOR_STARTER,

  STARTER_FULLSCREEN,
  STARTER_STANDARD,
  STARTER_START,
  STARTER_CLOSE,
  STARTER_CANCEL,
  STARTER_LANG,
  STARTER_UPDATE,
  STARTER_NEWVERSION,
  STARTER_NONEWVERSION,
  STARTER_LEVELS,
  STARTER_LOADLIST,
  STARTER_CHOOSEPACK,
  STARTER_DOWNLOAD,

  SEPERATOR_MISC,

  BENUTZTE_GRAFIK,
  BENUTZTES_FILE,
  LADE_BITMAP,
  LADE_WAVE,
  LADE_MODULE,
  LADEN_ERFOLGREICH,
  LADE_LEVELDATEN,

  SEPERATOR_TUTORIAL,

  TUTORIAL1,
  TUTORIAL2,
  TUTORIAL3,
  TUTORIAL4,
  TUTORIAL5,
  TUTORIAL6,
  TUTORIAL7,
  TUTORIAL8,
  TUTORIAL9,
  TUTORIAL10,
  TUTORIAL11,
  TUTORIAL12,
  TUTORIAL13,
  TUTORIAL14,
  TUTORIAL15,
  TUTORIAL16,
  TUTORIAL17,
  TUTORIAL18,
  TUTORIAL19,
  TUTORIAL20,
  TUTORIAL21,
  TUTORIAL22,
  TUTORIAL23,
  TUTORIAL24,
  TUTORIAL25,
  TUTORIAL26,
  TUTORIAL27,
  TUTORIAL28,
  TUTORIAL29,
  TUTORIAL30,
  TUTORIAL31,
  TUTORIAL32,
  TUTORIAL33,
  TUTORIAL34,
  TUTORIAL35,
  TUTORIAL36,
  TUTORIAL37,
  TUTORIAL38,
  TUTORIAL39,

  SEPERATOR_CONSOLE,

  CONSOLE1,
  CONSOLE2,
  CONSOLE3,
  CONSOLE4,
  CONSOLE5,
  CONSOLE6,
  CONSOLE7,
  CONSOLE8,
  CONSOLE9,

  SEPERATOR_SUMMARY,

  SUMMARY_TITLE,
  SUMMARY_SECRETS,
  SUMMARY_CHEATUNLOCK,
  SUMMARY_PRESSFIRE,

  SEPERATOR_MENU,

  MENUE_LOADING,
  MENUE_SPIEL_SPEICHERN,
  MENUE_SPIEL_STARTEN,
  MENUE_SPIEL_FORTSETZEN,
  MENUE_EINSTELLUNGEN,
  MENUE_HIGHSCORES,
  MENUE_CREDITS,
  MENUE_QUIT,

  MENUE_TUTORIAL_SPIELEN,
  MENUE_NEUES_SPIEL_STARTEN,
  MENUE_ALTES_SPIEL_FORTSETZEN,

  MENUE_LEICHT,
  MENUE_MITTEL,
  MENUE_SCHWER,
  MENUE_TURRICAN,

  MENUE_ANLEITUNG,

  HIGHSCORE_PLATZ,
  HIGHSCORE_NAME,
  HIGHSCORE_PUNKTE,
  HIGHSCORE_STAGE,
  HIGHSCORE_SKILL,

  VOLUMES,
  SOUND,
  MUSIK,
  STEUERUNG,

  SPRACHE,
  JOYMODE,
  FORCEFEEDBACK,
  DEFAULTS,

  JOYMODE_PAD,
  JOYMODE_STICK,

  WAHNSINN,
  NEUE_HIGHSCORE,
  NAMEN_EINGEBEN,
  WEITER,
  ZURUECK,
  TASTEN_L,
  TASTEN_R,
  TASTEN_DUCKEN,
  TASTEN_OBEN,
  TASTEN_UNTEN,
  TASTEN_SPRUNG,
  TASTEN_SCHUSS,
  TASTEN_BLITZ,
  TASTEN_POWERLINE,
  TASTEN_GRANATE,
  TASTEN_SCHOCK,
  TASTEN_WAFFEWECHSELN,
  JOY_LAUFEN,
  JOY_SCHAUEN,
  JOY_ACHSE,
  JOY_COOLIE,
  TASTEN_NEU_T,
  TASTEN_NEU_B,
  DETAIL_LOW,
  DETAIL_MEDIUM,
  DETAIL_HIGH,
  DETAIL_MAXIMUM,
  ONE_PLAYER,
  TWO_PLAYERS,
  PLAYER_ONE,
  PLAYER_TWO,
  TYP,
  MODUS,
  EMPFINDLICHKEIT,
  KEYBOARD,
  NICHT_DEFINIERT,

  SAVE_STAGE,
  SAVE_SPIELER,
  SAVE_DATUM,
  SAVE_LEER,

  SECRET,

  LEFT,
  RIGHT,
  UP,
  DOWN,
  BUTTON,

  SEPERATOR_HINTS,

  HINT1,
  HINT2,
  HINT3,
  HINT4,
  HINT5,
  HINT6,
  HINT7,
  HINT8,
  HINT9,
  HINT10,
  HINT11,
  HINT12,
  HINT13,
  HINT14,
  HINT15,
  HINT16,
  HINT17,
  HINT18,
  HINT19,
  HINT20,
  HINT21,
  HINT22,
  HINT23,
  HINT24,
  HINT25,
  HINT26,
  HINT27,
  HINT28,
  HINT29,
  HINT30,

  SEPERATOR_PISSEN,

  PISS_1,
  PISS_2,
  PISS_3,
  PISS_4,
  PISS_5,
  PISS_6,
  PISS_7,
  PISS_8,
  PISS_9,
  PISS_10,
  PISS_11,
  PISS_12,
  PISS_13,
  PISS_14,
  PISS_15,
  PISS_16,
  PISS_17,
  PISS_18,

  SEPERAROT_INTRO,

  INTRO1,
  INTRO2,
  INTRO3,
  INTRO4,

  INTRO5,
  INTRO6,
  INTRO7,
  INTRO8,

  INTRO9,
  INTRO10,
  INTRO11,
  INTRO12,

  INTRO13,
  INTRO14,
  INTRO15,
  INTRO16,

  INTRO17,
  INTRO18,
  INTRO19,
  INTRO20,

  INTRO21,
  INTRO22,
  INTRO23,
  INTRO24,
  INTRO25,

  SEPERATOR_OUTTRO,

  OUTTRO1,
  OUTTRO2,
  OUTTRO3,
  OUTTRO4,
  OUTTRO5,
  OUTTRO6,
  OUTTRO7,
  OUTTRO8,
  OUTTRO9,
  OUTTRO10,
  OUTTRO11,

  SEPERATOR_MARIO,

  MARIO1,
  MARIO2,
  MARIO3,
  MARIO4,
  MARIO5,
  MARIO6,
  MARIO7,

  LASTTEXT
};

} // namespace

// --------------------------------------------------------------------------------------
// Funktionen
// --------------------------------------------------------------------------------------

bool LoadLanguage(char *filename);           // Bestimmte Sprachdatei laden
void InitReplacers();                    // Tasten ErsetzungsStrings für die TutorialTexte initialisieren

// DKS - Added cross-platform language-files handling
void FindLanguageFiles(const char *path);

// DKS - Added function to split a longer line into two shorter lines, for when
//      running on a lower-resolution device w/ scaled font
void SplitLine(char *dst1, char *dst2, const char *source);

// DKS - Added function to split a longer line into one, dst1, sized up to
//      width (in pixels) based on font passed, and one, dst2, is the remainder,
//      minus any leading whitespace. Returns true if line needed to be split.
bool ExtractStringOfLength(char *dst1, char *dst2, char *source, int width, DirectGraphicsFont *font);

// DKS - Added function to replace any instance of a substring with another substring,
//      primarily for use on gaming/phone systems where "key" should be replaced with
//      "button"
void ReplaceAll(std::string &str, const std::string &from, const std::string &to);

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern std::vector<std::string> LanguageFiles;
extern char TextArray[TEXT::LASTTEXT][1024];
extern char ActualLanguage[256];
extern char s_Replacers[MAX_AKTIONEN * 2][256];
extern char Cheats[MAX_CHEATS][256];

#endif
