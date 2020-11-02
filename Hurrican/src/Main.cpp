// Datei : Main.cpp

// --------------------------------------------------------------------------------------
//
// Hurrican
//
// Shoot em up ala Turrican
// benutzt die DirectX8.1 API für Grafik, Sound und Input
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

// DKS - Always show cracktro when debugging
//#ifndef _DEBUG
#define SHOW_CRACKTRO
//#endif

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include <algorithm>
#include <cstdio>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
namespace fs = std::experimental::filesystem::v1;

#include <iomanip>
#include <SDL.h>
#include "CCracktro.hpp"
#include "Console.hpp"
#include "DX8Sound.hpp"
#include "DX8Texture.hpp"
#include "GUISystem.hpp"
#include "Gameplay.hpp"
#include "HUD.hpp"
#include "Partikelsystem.hpp"
#include "Projectiles.hpp"

#ifdef USE_UNRARLIB
#include "unrarlib.h"
#endif

#if defined(ANDROID)
#include <android/log.h>
#endif

// Memory Leaks

//#include <stdlib.h>
//#include <crtdbg.h>

// --------------------------------------------------------------------------------------
// externe Variablen
// --------------------------------------------------------------------------------------

extern bool DEMORecording;
extern bool DEMOPlaying;
extern DirectGraphicsSprite PartikelGrafix[MAX_PARTIKELGFX];  // Grafiken der Partikel

// --------------------------------------------------------------------------------------
// globale Variablen
// --------------------------------------------------------------------------------------

bool FixedFramerate = false;  // true = Spiel mit 50 Frames laufen lassen
// false = Spiel so flüssig wie möglich laufen lassen
bool GameRunning = true;         // Spiel läuft :-)
bool GamePaused = false;         // Spiel eingefroren (wenn man zb das Fenster verlässt)
bool NochKeinFullScreen = true;  // Logo noch anzeigen in Paint ?
#ifdef _DEBUG
bool DebugMode = false;              // Debug Mode ein/aus
#endif                               //_DEBUG
float SpeedFaktor = 1.0f;            // Faktor, mit dem alle Bewegungen verrechnet werden
TexturesystemClass Textures;         // DKS - Added Texturesystem class (see DX8Sprite.cpp)
DirectGraphicsClass DirectGraphics;  // Grafik-Objekt
DirectInputClass DirectInput;        // Input-Objekt
TimerClass Timer;                    // Timer Klasse für die Framerate
#if defined(__AROS__)
Logdatei Protokoll("T:Game_Log.txt");  // Protokoll Datei
#else
Logdatei Protokoll("Game_Log.txt");  // Protokoll Datei
#endif
SoundManagerClass SoundManager;  // Sound Manager
DirectGraphicsFont *pDefaultFont = new (DirectGraphicsFont);
DirectGraphicsFont *pMenuFont = new (DirectGraphicsFont);
TileEngineClass TileEngine;          // Tile Engine
PartikelsystemClass PartikelSystem;  // Das coole Partikelsystem
ProjectileListClass Projectiles;     // Liste mit Schüssen
GegnerListClass Gegner;              // Liste mit Gegner
IntroClass *pIntro;                  // Intro-Objekt
OuttroClass *pOuttro;                // Outtro-Objekt
MenuClass *pMenu = nullptr;          // Hauptmenu-Objekt
ConsoleClass Console;                // Konsolen-Objekt
CGUISystem GUI;                      // GUI System
CCracktro *Cracktro;

char *g_storage_ext = nullptr;  // Where data files (levels, graphics, music, etc)
                                //      for the game are stored (read)
char *g_save_ext = nullptr;     // Where configuration files, logs, and save games
                                //      are written (-DKS) (write)

sCommandLineParams CommandLineParams;

// --------------------------------------------------------------------------------------
// Variablen für den Spielablauf
// --------------------------------------------------------------------------------------

// DKS - PlayerClass array is now static, not dynamically-allocated:
// PlayerClass				*pPlayer[2];					// Werte der Spieler
PlayerClass Player[2];  // Werte der Spieler

HUDClass HUD;                           // Das HUD
unsigned char SpielZustand = CRACKTRO;  // Aktueller Zustand des Spieles
char StringBuffer[100];                 // Für die Int / String Umwandlung

void FillCommandLineParams(int argc, char *args[]) {
    uint16_t i;

    // Set some sensible defaults
    CommandLineParams.RunWindowMode = false;
    CommandLineParams.TexFactor = 1;
    CommandLineParams.TexSizeMin = 1024;
    CommandLineParams.ScreenDepth = DEFAULT_SCREENBPP;
    CommandLineParams.VSync = true;
    CommandLineParams.ShowFPS = false;
    CommandLineParams.AllowNPotTextureSizes = false;
    CommandLineParams.LowRes = false;
    CommandLineParams.DataPath = nullptr;
    CommandLineParams.SavePath = nullptr;

    for (i = 1; i < argc; i++) {
        if ((strstr(args[i], "--help") != nullptr) || (strstr(args[i], "-?") != nullptr) ||
            (strstr(args[i], "-H") != nullptr) || (strstr(args[i], "-h") != nullptr)) {
            Protokoll << "Hurrican" << std::endl;
            Protokoll << "  Usage      : hurrican <arguments>" << std::endl;
            Protokoll << "  Arguments" << std::endl;
            Protokoll << "  -H,-?, --help           : Show this information" << std::endl;
            Protokoll << "  -W,    --windowmode     : Run in a window, not fullsreen" << std::endl;
            Protokoll << "  -F,    --showfps        : Show the current frames per second" << std::endl;
            Protokoll << "  -D x,  --depth x        : Set screen pixel depth to x (16, 24, 32)" << std::endl;
            Protokoll << "                            ( Default is " << DEFAULT_SCREENBPP << " )" << std::endl;
            Protokoll << "  -L,    --lowres         : Use " + LOWRES_SCREENWIDTH << "x" << LOWRES_SCREENHEIGHT
                      << " low-resolution screen dimensions" << std::endl;
            Protokoll << "  -NV,   --novsync        : Disable VSync / double-buffering" << std::endl;
            Protokoll << "  -NP,   --nonpot         : Allow non-power-of-two texture sizes" << std::endl;
            Protokoll << "                            Normally, GPUs require texture dimensions that are" << std::endl;
            Protokoll << "                            powers of two. If your GPU does not require that," << std::endl;
            Protokoll << "                            you can reduce VRAM usage with this switch." << std::endl;
            Protokoll << "  -TF x, --texfactor x    : Division factor for textures" << std::endl;
            Protokoll << "                            Valid values: 1, 2, 4" << std::endl;
            Protokoll << "                            If set to 2, textures dimensions will be halved." << std::endl;
            Protokoll << "                            If set to 4, textures dimensions will be quartered." << std::endl;
            Protokoll << "                            ( Default is 1 (no resizing) )" << std::endl;
            Protokoll << "  -TS x, --texsizemin x   : Size limitation for texture division factor" << std::endl;
            Protokoll << "                            Only textures with widths or heights above this" << std::endl;
            Protokoll << "                            value will be resized. MIN: 16  MAX: 1024" << std::endl;
            Protokoll << "                            ( Default is 1024 )" << std::endl;
            Protokoll << "  -PD x, --pathdata x     : Look in this path for the game's read-only data" << std::endl;
            Protokoll << "                            i.e. music, sound, graphics, levels, etc." << std::endl;
            Protokoll << "  -PS x, --pathsave x     : Use this path for the game's save data" << std::endl;
            Protokoll << "                            i.e. save-games, settings, high-scores, etc." << std::endl;
            exit(1);
        } else if ((strstr(args[i], "--windowmode") != nullptr) || (strstr(args[i], "-W") != nullptr)) {
            CommandLineParams.RunWindowMode = true;
            fprintf(stdout, "Window mode is enabled\n");
        } else if ((strstr(args[i], "--showfps") != nullptr) || (strstr(args[i], "-F") != nullptr)) {
            CommandLineParams.ShowFPS = true;
            fprintf(stdout, "FPS will be displayed\n");
        } else if ((strstr(args[i], "--depth") != nullptr) || (strstr(args[i], "-D") != nullptr)) {
            i++;
            if (i < argc) {
                CommandLineParams.ScreenDepth = std::clamp(atoi(args[i]), 16, 32);
                if (CommandLineParams.ScreenDepth >= 32)
                    CommandLineParams.ScreenDepth = 32;
                else if (CommandLineParams.ScreenDepth > 24 && CommandLineParams.ScreenDepth < 32)
                    CommandLineParams.ScreenDepth = 24;
                else if (CommandLineParams.ScreenDepth > 16 && CommandLineParams.ScreenDepth < 24)
                    CommandLineParams.ScreenDepth = 16;
                fprintf(stdout, "Screen depth (bpp) requested is %d\n", CommandLineParams.ScreenDepth);
            }
        } else if ((strstr(args[i], "--lowres") != nullptr) || (strstr(args[i], "-L") != nullptr)) {
            fprintf(stdout, "Low-resolution 320x240 screen dimensions are requested\n");
            CommandLineParams.LowRes = true;
        } else if ((strstr(args[i], "--novsync") != nullptr) || (strstr(args[i], "-NV") != nullptr)) {
            fprintf(stdout, "VSync / double-buffering will be disabled, if supported\n");
            CommandLineParams.VSync = false;
        } else if ((strstr(args[i], "--nonpot") != nullptr) || (strstr(args[i], "-NP") != nullptr)) {
            fprintf(stdout, "Non-power-of-two textures are allowed\n");
            CommandLineParams.AllowNPotTextureSizes = true;
        } else if ((strstr(args[i], "--texfactor") != nullptr) || (strstr(args[i], "-TF") != nullptr)) {
            i++;
            if (i < argc) {
                CommandLineParams.TexFactor = std::clamp(atoi(args[i]), 1, 4);
                if (CommandLineParams.TexFactor == 3)
                    CommandLineParams.TexFactor = 4;
                fprintf(stdout, "Texfactor set to %d\n", CommandLineParams.TexFactor);
            }
        } else if ((strstr(args[i], "--texsizemin") != nullptr) || (strstr(args[i], "-TS") != nullptr)) {
            i++;
            if (i < argc) {
                CommandLineParams.TexSizeMin = std::clamp(atoi(args[i]), 16, 1024);
                fprintf(stdout, "Texsizemin set to %d\n", CommandLineParams.TexSizeMin);
            }
        } else if ((strstr(args[i], "--pathdata") != nullptr) || (strstr(args[i], "-PD") != nullptr)) {
            i++;
            if (i < argc) {
                if (args[i] && strlen(args[i]) > 0 && !CommandLineParams.DataPath) {
                    CommandLineParams.DataPath = static_cast<char *>(malloc(strlen(args[i]) + 1));
                    strcpy(CommandLineParams.DataPath, args[i]);
                    if (fs::is_directory(CommandLineParams.DataPath)) {
                        fprintf(stdout, "Data path set to %s\n", CommandLineParams.DataPath);
                    } else {
                        fprintf(stdout, "ERROR: could not find data path %s\n", CommandLineParams.DataPath);
                        free(CommandLineParams.DataPath);
                        CommandLineParams.DataPath = nullptr;
                    }
                }
            }
        } else if ((strstr(args[i], "--pathsave") != nullptr) || (strstr(args[i], "-PS") != nullptr)) {
            i++;
            if (i < argc) {
                if (args[i] && strlen(args[i]) > 0 && !CommandLineParams.SavePath) {
                    CommandLineParams.SavePath = static_cast<char *>(malloc(strlen(args[i]) + 1));
                    strcpy(CommandLineParams.SavePath, args[i]);
                    if (fs::create_directory(CommandLineParams.SavePath)) {
                        fprintf(stdout, "Save path set to %s\n", CommandLineParams.SavePath);
                    } else {
                        fprintf(stdout, "ERROR: could not find save path %s\n", CommandLineParams.SavePath);
                        free(CommandLineParams.SavePath);
                        CommandLineParams.SavePath = nullptr;
                    }
                }
            }
        } else if ((strstr(args[i], "--npot") != nullptr) || (strstr(args[i], "-NP") != nullptr)) {
            fprintf(stdout, "Non-power-of-two textures are allowed\n");
            CommandLineParams.AllowNPotTextureSizes = true;
        } else if (strstr(args[i], "--custom") != nullptr) {
            i++;
            if (i < argc && strlen(args[i]) < 256) {
                strcpy(CommandLineParams.OwnLevelList, args[i]);
                CommandLineParams.RunOwnLevelList = true;
            }
        } else if (strstr(args[i], "--level") != nullptr) {
            // own single level?
            i++;
            if (i < argc && strlen(args[i]) < 256) {
                strcpy(CommandLineParams.UserLevelName, args[i]);
                CommandLineParams.RunUserLevel = true;
            }
        }
    }
}

// --------------------------------------------------------------------------------------
// Win-Main Funktion
// --------------------------------------------------------------------------------------
#undef main
#define main main
int main(int argc, char *argv[]) {
    GamePaused = false;

    FillCommandLineParams(argc, argv);

    // Set game's data path:
    g_storage_ext = nullptr;
    // First, see if a command line parameter was passed:
    if (CommandLineParams.DataPath) {
        g_storage_ext = static_cast<char *>(malloc(strlen(CommandLineParams.DataPath) + 1));
        strcpy(g_storage_ext, CommandLineParams.DataPath);
        free(CommandLineParams.DataPath);
        CommandLineParams.DataPath = nullptr;
    } else {
#if defined(ANDROID)
        g_storage_ext = (char *)malloc(strlen(SDL_AndroidGetExternalStoragePath() + 1));
        strcpy_s(g_storage_ext, SDL_AndroidGetExternalStoragePath());
#else  // NON-ANDROID:
#ifdef USE_STORAGE_PATH
        // A data-files storage path has been specified in the Makefile:
        g_storage_ext = static_cast<char *>(malloc(strlen(USE_STORAGE_PATH) + 1));
        strcpy_s(g_storage_ext, USE_STORAGE_PATH);
        // Attempt to locate the dir
        if (!fs::is_directory(g_storage_ext)) {
            // Failed, print message and use "." folder as fall-back
            Protokoll << "ERROR: Failed to locate data directory " << g_storage_ext << std::endl;
            Protokoll << "\tUsing '.' folder as fallback." << std::endl;
            free(g_storage_ext);
            g_storage_ext = static_cast<char *>(malloc(strlen(".") + 1));
            strcpy_s(g_storage_ext, ".");
        }
#else
        g_storage_ext = static_cast<char *>(malloc(strlen(".") + 1));
        strcpy(g_storage_ext, ".");
#endif
#endif  // ANDROID
    }

    // Set game's save path (save games, settings, logs, high-scores, etc)
    g_save_ext = nullptr;
    if (CommandLineParams.SavePath) {
        g_save_ext = static_cast<char *>(malloc(strlen(CommandLineParams.SavePath) + 1));
        strcpy(g_save_ext, CommandLineParams.SavePath);
        free(CommandLineParams.SavePath);
        CommandLineParams.SavePath = nullptr;
    } else {
#if defined(ANDROID)
        g_save_ext = (char *)malloc(strlen(SDL_AndroidGetExternalStoragePath() + 1));
        strcpy_s(g_save_ext, SDL_AndroidGetExternalStoragePath());
#else  // NON-ANDROID:
#ifdef USE_HOME_DIR
        // Makefile is specifying this is a UNIX machine and we should write saves,settings,etc to $HOME/.hurrican/ dir
        char *homedir = getenv("HOME");
        bool success = false;
        if (homedir) {
            const char *subdir = "/.hurrican";
            g_save_ext = static_cast<char *>(malloc(strlen(homedir) + strlen(subdir) + 1));
            strcpy(g_save_ext, homedir);
            strcat(g_save_ext, subdir);
            success = fs::is_directory(g_save_ext) || fs::create_directory(g_save_ext);
            if (!success) {
                // We weren't able to create the $HOME/.hurrican directory, or if it exists, it is
                // not a directory or is not accessible somehow..
                Protokoll << "ERROR: unable to create or access $HOME/.hurrican/ directory." << std::endl;
                Protokoll << "\tFull path that was tried: " << g_save_ext << std::endl;
                free(g_save_ext);
            }
        } else {
            // We weren't able to find the $HOME env var
            Protokoll << "ERROR: unable to find $HOME environment variable" << std::endl;
            success = false;
        }

        if (!success) {
            Protokoll << "\tUsing '.' folder as fallback." << std::endl;
            g_save_ext = static_cast<char *>(malloc(strlen(".") + 1));
            strcpy(g_save_ext, ".");
        }
#else
        g_save_ext = (char *)malloc(strlen(".") + 1);
        strcpy_s(g_save_ext, ".");
#endif  // USE_HOME_DIR
#endif  // ANDROID
    }

    Protokoll << "--> Using external storage path '" << g_storage_ext << "' <--" << std::endl;
    Protokoll << "--> Using save path '" << g_save_ext << "' <--\n" << std::endl;

    //----- Spiel-Initialisierung

    if (!GameInit()) {
        Protokoll << "\n-> GameInit error!\n" << std::endl;
        GameRunning = false;
    } else {
        Protokoll << "\n-> GameInit successful!\n" << std::endl;
    }

    //----- Main-Loop

    while (GameRunning) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                GameRunning = false;
        }

            // DKS - Exceptions can now be disabled, reducing unnecessary code-bloat:
#ifndef USE_NO_EXCEPTIONS
        try
#endif  // USE_NO_EXCEPTIONS
        {
            if (!GamePaused) {
                // Main Loop
                Heartbeat();

                // Eingabegeräte updaten
                DirectInput.UpdateTastatur();
                DirectInput.UpdateJoysticks();
                // DirectInput.UpdateMaus(false);

                // Soundchannels updaten
                SoundManager.Update();

                // Timer updaten
                Timer.update();

                // Feste Framerate ? (Spiel läuft in Zeitlupe, wenn zu langsam)
                //
                if (FixedFramerate) {
                    Timer.SetMaxFPS(60);
                    SpeedFaktor = 1.0f / 60.0f * Timer.GetMoveSpeed();
                } else {
                    // Timer.SetMaxFPS (0);
                    SpeedFaktor = Timer.SpeedFaktor;
                }

                Timer.wait();

                // Bei Demo immer gleichen Speedfaktor setzen
                //
                if (DEMORecording || DEMOPlaying)
                    SpeedFaktor = 0.28f;
            }
        }
            // DKS - Exceptions can now be disabled, reducing unnecessary code-bloat:
#ifndef USE_NO_EXCEPTIONS
        catch (const char *str) {
            Protokoll << "Failure! Unhandled exception\n" << str << std::endl;
            GameRunning = false;
        }
#endif  // USE_NO_EXCEPTIONS
    }

    //----- Spiel verlassen

    // Timer.WriteLogValues();

    if (!GameExit()) {
        Protokoll << "-> GameExit Fehler !" << std::endl;
        GameRunning = false;
    }

    Protokoll << "\n-> Hurrican closed !\n";
    Protokoll << "\nhttp://www.poke53280.de\n";
    Protokoll << "Bugreports, questions etc : information@poke53280.de\n";
    Protokoll << "\n-> logfile end" << std::endl;

    // Kein Fehler im Game? Dann Logfile löschen
    // FIXME: That doesn't belong here
    if (Protokoll.delLogFile)
        fs::remove(fs::path("Game_Log.txt"));

    free(g_storage_ext);
    free(g_save_ext);

    return 0;
}

// --------------------------------------------------------------------------------------
// GameInit, initialisiert die DX Objekte
// --------------------------------------------------------------------------------------

bool GameInit() {
    options_Detail = DETAIL_LOW;

    srand(SDL_GetTicks());

    // DKS - added fast RNG, this is to ensure it always gets seeded, though the above should already do so:
#ifdef USE_FAST_RNG
    seed_fast_rand(SDL_GetTicks());
#endif  // USE_FAST_RNG

    // DKS - Added language-translation files support to SDL port:
    char langfilepath[256];
    if (g_storage_ext) {
        strcpy(langfilepath, g_storage_ext);
        strcat(langfilepath, "/lang");
    } else {
        strcpy(langfilepath, "./lang");
    }

    FindLanguageFiles(langfilepath);

    // Try again if needed
    if (LanguageFiles.empty()) {
        strcpy(langfilepath, "./");
        FindLanguageFiles(langfilepath);
    }

    // One more time if needed
    if (LanguageFiles.empty()) {
        strcpy(langfilepath, "./lang");
        FindLanguageFiles(langfilepath);
    }

    if (LanguageFiles.empty()) {
        Protokoll << "ERROR: Failed to find any language files, aborting." << std::endl;
        return false;
    }

    Protokoll << "\n>--------------------<\n";
    Protokoll << "| GameInit started   |\n";
    Protokoll << ">--------------------<" << std::endl;

    // Direct3D initialisieren
    if (!DirectGraphics.Init(RENDERWIDTH, RENDERHEIGHT, CommandLineParams.ScreenDepth, CommandLineParams.VSync)) {
        Protokoll << "\n-> Direct3D Initialisierung Fehler ...!" << std::endl;
        GameRunning = false;
        return false;
    }

    // DirectInput initialisieren
    if (!DirectInput.Init()) {
        Protokoll << "\n-> DirectInput8 Initialisierung Fehler ...!" << std::endl;
        GameRunning = false;
        return false;
    }

    // DKS - Read texture scale factor files
    Textures.ReadScaleFactorsFiles();

#if defined(ANDROID)
    DirectInput.InitTouchBoxes(DirectGraphics.WindowView.w, DirectGraphics.WindowView.h);
#endif

    // DKS - Sound manager is now a static global, and initialized with Init()
    // Sound Manager initialisieren
    // pSoundManager = new CSoundManager();
    SoundManager.Init();

    // Splash-Screen nicht mehr anzeigen
    NochKeinFullScreen = false;

#ifdef SHOW_CRACKTRO
    Cracktro = new CCracktro();
    SpielZustand = CRACKTRO;
#endif

    return true;
}

// --------------------------------------------------------------------------------------
// GameInit2, initialisiert den Rest nach dem Cracktro
// --------------------------------------------------------------------------------------

bool GameInit2() {
    // DKS-Player[] is a static global now:
    // Player initialisieren
    // Player[0] = new PlayerClass(0);
    // Player[1] = new PlayerClass(1);

    // DKS - Now that the player sprites are stored in the class, I've disabled this
    //      in favor of actual constructors:
    // Player[0]->SoundOff = 0;
    // Player[1]->SoundOff = 1;
    // memset(Player[0], 0, sizeof(*Player[0]));
    // memset(Player[1], 0, sizeof(*Player[1]));

    // Konfiguration laden
    if (!LoadConfig()) {
        Protokoll << "\n-> No config found. Creating default" << std::endl;
        CreateDefaultConfig();
    }

    // Menumusik laden und spielen
    SoundManager.LoadSong("menu.it", MUSIC_MENU);

    // DKS - Renamed:
    // SoundManager.ResetAllSongVolumes();
    SoundManager.ResetAllSoundVolumes();

    SoundManager.PlaySong(MUSIC_MENU, false);

    // Menu initialisieren
    // DKS - Resized menufont.png and added missing glyphs to make Swedish translation work:
    // pMenuFont->LoadFont("menufont.png", 448, 256, 28, 28, 16, 7);
    pMenuFont->LoadFont("menufont.png", 448, 336, 28, 28, 16, 12, menufont_charwidths);
    pMenu = new MenuClass();

    // Fonts laden
    pDefaultFont->LoadFont("smallfont.png", 320, 84, 10, 12, 32, 7, smallfont_charwidths);

    // DKS - Added support for font scaling
    if (CommandLineParams.LowRes) {
        pDefaultFont->SetScaleFactor(2);  // On lower res, draw smallest font twice as large so it appears 1:1
    }

    pMenu->StartProgressBar(522);

    // GUISystem initialiseren
    GUI.InitGUISystem();

    // DKS Load PartikelsystemClass sprites:
    PartikelSystem.LoadSprites();

    // GegnerListe initialisieren
    // DKS - Load GegnerListClass sprites:
    Gegner.LoadSprites();

    // Tileengine initialisieren
    TileEngine.LoadSprites();  // DKS - Added this function to TileEngineClass
    TileEngine.SetScrollSpeed(1.0f, 0.0f);

    // DKS Load projectile sprites:
    Projectiles.LoadSprites();

    // DKS - Load HUD sprites:
    HUD.LoadSprites();

    InitReplacers();

    // Sounds laden
    SoundManager.LoadWave("spreadshot.wav", SOUND_SPREADSHOT, false);
    SoundManager.LoadWave("lasershot.wav", SOUND_LASERSHOT, false);
    SoundManager.LoadWave("bounceshot.wav", SOUND_BOUNCESHOT, false);
    SoundManager.LoadWave("explosion1.wav", SOUND_EXPLOSION1, false);
    SoundManager.LoadWave("explosion2.wav", SOUND_EXPLOSION2, false);
    SoundManager.LoadWave("explosion3.wav", SOUND_EXPLOSION3, false);
    SoundManager.LoadWave("explosion4.wav", SOUND_EXPLOSION4, false);
    SoundManager.LoadWave("walkergiggle.wav", SOUND_WALKERGIGGLE, false);
    SoundManager.LoadWave("collect.wav", SOUND_COLLECT, false);
    SoundManager.LoadWave("hit.wav", SOUND_SPREADHIT, false);
    SoundManager.LoadWave("canon.wav", SOUND_CANON, false);
    SoundManager.LoadWave("click.wav", SOUND_CLICK, false);
    SoundManager.LoadWave("blitzstart.wav", SOUND_BLITZSTART, false);
    SoundManager.LoadWave("blitzende.wav", SOUND_BLITZENDE, false);
    SoundManager.LoadWave("blitz.wav", SOUND_BLITZ, true);
    SoundManager.LoadWave("blitzstart.wav", SOUND_BLITZSTART_P2, false);
    SoundManager.LoadWave("blitzende.wav", SOUND_BLITZENDE_P2, false);
    SoundManager.LoadWave("blitz.wav", SOUND_BLITZ_P2, true);
    SoundManager.LoadWave("powerline.wav", SOUND_POWERLINE, false);
    SoundManager.LoadWave("landen.wav", SOUND_LANDEN, false);
    SoundManager.LoadWave("waterin.wav", SOUND_WATERIN, false);
    SoundManager.LoadWave("waterout.wav", SOUND_WATEROUT, false);
    SoundManager.LoadWave("dive.wav", SOUND_DIVE, false);
    SoundManager.LoadWave("feuerfalle.wav", SOUND_FEUERFALLE, false);
    SoundManager.LoadWave("abzug.wav", SOUND_ABZUG, false);
    SoundManager.LoadWave("abzug.wav", SOUND_ABZUG_P2, false);
    SoundManager.LoadWave("funke.wav", SOUND_FUNKE, false);
    SoundManager.LoadWave("funke2.wav", SOUND_FUNKE2, false);
    SoundManager.LoadWave("funke3.wav", SOUND_FUNKE3, false);
    SoundManager.LoadWave("funke4.wav", SOUND_FUNKE4, false);
    SoundManager.LoadWave("granate.wav", SOUND_GRANATE, false);
    SoundManager.LoadWave("stonefall.wav", SOUND_STONEFALL, false);
    SoundManager.LoadWave("stoneexplode.wav", SOUND_STONEEXPLODE, false);
    SoundManager.LoadWave("rocket.wav", SOUND_ROCKET, false);
    SoundManager.LoadWave("presse.wav", SOUND_PRESSE, false);
    SoundManager.LoadWave("ammo.wav", SOUND_AMMO, false);
    SoundManager.LoadWave("kotzen.wav", SOUND_KOTZEN, false);
    SoundManager.LoadWave("made.wav", SOUND_MADE, false);
    SoundManager.LoadWave("droneshot.wav", SOUND_DRONE, false);
    SoundManager.LoadWave("waterdrop.wav", SOUND_DROP, false);
    SoundManager.LoadWave("thunder.wav", SOUND_THUNDER, false);
    SoundManager.LoadWave("upgrade.wav", SOUND_UPGRADE, false);
    SoundManager.LoadWave("column.wav", SOUND_COLUMN, false);
    SoundManager.LoadWave("door.wav", SOUND_DOOR, true);
    SoundManager.LoadWave("doorstop.wav", SOUND_DOORSTOP, false);
    SoundManager.LoadWave("switch.wav", SOUND_SWITCH, false);
    SoundManager.LoadWave("schleim.wav", SOUND_SCHLEIM, false);
    SoundManager.LoadWave("message.wav", SOUND_MESSAGE, false);
    SoundManager.LoadWave("beamload.wav", SOUND_BEAMLOAD, true);
    SoundManager.LoadWave("beamload2.wav", SOUND_BEAMLOAD2, true);
    SoundManager.LoadWave("beamload.wav", SOUND_BEAMLOAD_P2, true);
    SoundManager.LoadWave("beamload2.wav", SOUND_BEAMLOAD2_P2, true);

    // DKS - This was commented out in original code, but I've added support for
    //      Trigger_Stampfstein.cpp's .hppain retraction sound back in:
    SoundManager.LoadWave("chain.wav", SOUND_CHAIN, true);

    SoundManager.LoadWave("mushroomjump.wav", SOUND_MUSHROOMJUMP, false);
    SoundManager.LoadWave("golemload.wav", SOUND_GOLEMLOAD, false);
    SoundManager.LoadWave("golemshot.wav", SOUND_GOLEMSHOT, false);
    SoundManager.LoadWave("dampf.wav", SOUND_STEAM, false);
    SoundManager.LoadWave("dampf2.wav", SOUND_STEAM2, false);
    SoundManager.LoadWave("hit2.wav", SOUND_HIT, false);
    SoundManager.LoadWave("hit3.wav", SOUND_HIT2, false);
    SoundManager.LoadWave("spiderlila.wav", SOUND_LILA, false);
    SoundManager.LoadWave("fireball.wav", SOUND_FIREBALL, false);
    SoundManager.LoadWave("takeoff.wav", SOUND_TAKEOFF, false);
    SoundManager.LoadWave("laugh.wav", SOUND_LAUGH, false);
    SoundManager.LoadWave("standup.wav", SOUND_STANDUP, false);
    SoundManager.LoadWave("gatling.wav", SOUND_GATLING, false);
    SoundManager.LoadWave("glassbreak.wav", SOUND_GLASSBREAK, false);
    SoundManager.LoadWave("mutant.wav", SOUND_MUTANT, false);
    SoundManager.LoadWave("heart1.wav", SOUND_HEART1, false);
    SoundManager.LoadWave("heart2.wav", SOUND_HEART2, false);
    SoundManager.LoadWave("secret.wav", SOUND_SECRET, false);
    SoundManager.LoadWave("mario.wav", SOUND_MARIO, false);
    SoundManager.LoadWave("flamethrower.wav", SOUND_FLAMETHROWER, true);
    SoundManager.LoadWave("flamethrower.wav", SOUND_FLAMETHROWER2, true);

    // Sound Trigger
    SoundManager.LoadWave("ambient_wasserfall.wav", SOUND_WASSERFALL, true);
    SoundManager.LoadWave("ambient_wind.wav", SOUND_WIND, true);

    // Voices laden
    SoundManager.LoadWave("v_spread.wav", SOUND_VOICE_SPREAD, false);
    SoundManager.LoadWave("v_laser.wav", SOUND_VOICE_LASER, false);
    SoundManager.LoadWave("v_bounce.wav", SOUND_VOICE_BOUNCE, false);
    SoundManager.LoadWave("v_lightning.wav", SOUND_VOICE_LIGHTNING, false);
    SoundManager.LoadWave("v_shield.wav", SOUND_VOICE_SHIELD, false);
    SoundManager.LoadWave("v_powerup.wav", SOUND_VOICE_POWERUP, false);
    SoundManager.LoadWave("v_wheel.wav", SOUND_VOICE_WHEELPOWER, false);
    SoundManager.LoadWave("v_grenade.wav", SOUND_VOICE_GRENADE, false);
    SoundManager.LoadWave("v_powerline.wav", SOUND_VOICE_POWERLINE, false);
    SoundManager.LoadWave("v_smartbomb.wav", SOUND_VOICE_SMARTBOMB, false);
    SoundManager.LoadWave("v_rapidfire.wav", SOUND_VOICE_RAPIDFIRE, false);
    SoundManager.LoadWave("v_supershot.wav", SOUND_VOICE_SUPERSHOT, false);
    SoundManager.LoadWave("v_extralife.wav", SOUND_VOICE_EXTRALIFE, false);

    // Endgegner Sounds
    SoundManager.LoadWave("pharaoramm.wav", SOUND_PHARAORAMM, false);
    SoundManager.LoadWave("pharaodie.wav", SOUND_PHARAODIE, false);
    SoundManager.LoadWave("spiderscream.wav", SOUND_SPIDERSCREAM, false);
    SoundManager.LoadWave("spiderwalk.wav", SOUND_SPIDERWALK, false);
    SoundManager.LoadWave("spiderlaser.wav", SOUND_SPIDERLASER, false);
    SoundManager.LoadWave("spidergrenade.wav", SOUND_SPIDERGRENADE, false);
    SoundManager.LoadWave("blitzhit.wav", SOUND_BLITZHIT, false);
    SoundManager.LoadWave("blitzhit2.wav", SOUND_BLITZHIT2, false);
    SoundManager.LoadWave("bratlaser.wav", SOUND_BRATLASER, false);
    SoundManager.LoadWave("metal.wav", SOUND_KLONG, false);

    // restliche musiken laden
    // DKS - Flugsack song is now loaded on-demand in Gegner_Helper.cpp:
    // SoundManager.LoadSong("flugsack.it",	MUSIC_FLUGSACK);

    SoundManager.LoadSong("credits.it", MUSIC_CREDITS);

    // DKS - New parameter specifies whether to loop, and stage-clear music shouldn't:
    SoundManager.LoadSong("stageclear.it", MUSIC_STAGECLEAR, false);

    // DKS - New parameter specifies whether to loop, and game-over music shouldn't:
    SoundManager.LoadSong("gameover.it", MUSIC_GAMEOVER, false);

    SoundManager.LoadSong("highscore.it", MUSIC_HIGHSCORE);

    // DKS - Punisher music is now loaded on-demand in Gegner_Puni.hpper.cpp
    // SoundManager.LoadSong("Punisher.it", MUSIC_PUNISHER);

    if (!GameRunning)
        return false;

    // Konsole initialisieren
    // DKS - Load console sprites:
    Console.LoadSprites();

    // DKS - renamed:
    // SoundManager.ResetAllSongVolumes();
    SoundManager.ResetAllSoundVolumes();

    return true;
}

// --------------------------------------------------------------------------------------
// GameExit, de-initialisieren der DX Objekte, Sprites etc.
// --------------------------------------------------------------------------------------

bool GameExit() {
    Protokoll << "\n>--------------------<\n";
    Protokoll << "| GameExit started   |\n";
    Protokoll << ">--------------------<\n" << std::endl;

    // Sprites freigeben
    delete (pDefaultFont);
    delete (pMenuFont);
    Protokoll << "-> Fonts released" << std::endl;

    // Menu beenden
    delete (pMenu);
    Protokoll << "-> Head menu released" << std::endl;

    // DKS - Sound manager is now a static global, and we use new Exit() method:
    SoundManager.Exit();
    Protokoll << "-> Sound system released" << std::endl;

    // DKS - Free any straggling textures in VRAM before closing down graphics:
    //      NOTE: this is important! Global systems that contain their own
    //      sprites might get destructed after graphics has been shut down.
    //      In the original code, many systems like TileEngineClass were accessed
    //      through global pointers to dynamically allocated classes, in the
    //      interest of speed and code-compactness, if not clarity.
    //      I changed them to be globally allocated static vars. Since some contain
    //      sprite variables, their destructors would then possible end up freeing
    //      textures after the graphics system already closed. Textures::Exit()
    //      below will prevent that.
    Textures.Exit();
    Protokoll << "-> Texture system released" << std::endl;

    DirectInput.Exit();  // DirectInput beenden

    DirectGraphics.Exit();  // Direct3D    beenden

    // PrintStatus();

    return true;
}

// --------------------------------------------------------------------------------------
// Heartbeat, der Mainloop. der bei jedem Frame durchlaufen wird
// --------------------------------------------------------------------------------------

bool Heartbeat() {
    switch (SpielZustand) {
        // Cracktro
        case CRACKTRO: {
#ifdef SHOW_CRACKTRO

            Cracktro->Run();

            if (!Cracktro->b_running) {
                delete (Cracktro);
                SpielZustand = MAINMENU;

                if (!GameInit2())
                    return false;
            }
#else
            if (!GameInit2())
                return false;

            SpielZustand = MAINMENU;

#endif
            //		pOuttro = new OuttroClass();
            //		SpielZustand = OUTTRO;

            goto jump;
        }

        //----- Intro anzeigen ?
        case INTRO: {
            // Laufen lassen, bis beendet
            if (pIntro->Zustand != INTRO_DONE) {
                pIntro->DoIntro();

                if (DirectInput.AnyKeyDown() || DirectInput.AnyButtonDown())
                    pIntro->EndIntro();
            } else {
                SoundManager.StopSong(MUSIC_INTRO, false);
                delete (pIntro);
                InitNewGame();
                InitNewGameLevel(1);
                SpielZustand = GAMELOOP;
            }
        } break;

        //----- Outtro anzeigen ?
        case OUTTRO: {
            pOuttro->DoOuttro();

            if (KeyDown(DIK_ESCAPE))  // Intro beenden ?
            {
                SoundManager.StopSong(MUSIC_OUTTRO, false);
                delete (pOuttro);
                Stage = MAX_LEVELS;
                pMenu->CheckForNewHighscore();
            }
        } break;

        //----- Hauptmenu
        case MAINMENU: {
            pMenu->DoMenu();
        } break;

        //---- Haupt-Gameloop
        case GAMELOOP: {
            GameLoop();
        } break;

        default:
            break;
    }

#ifdef _DEBUG

    // Debugmode ?
    if (DebugMode == true)
        ShowDebugInfo();

    // Debug-Mode ein/ausschalten
    if (KeyDown(DIK_F10)) {
        if (DebugMode == true)
            DebugMode = false;
        else
            DebugMode = true;
        while (KeyDown(DIK_F10))
            DirectInput.UpdateTastatur();
    }

#endif

    if (CommandLineParams.ShowFPS)
        ShowFPS();

    // GUI abhandeln
    GUI.Run();

    // Konsole abhandeln
    Console.DoConsole();

jump:

    DirectGraphics.DisplayBuffer();

    // Screenshot machen
#ifdef _DEBUG
    if (KeyDown(DIK_F12))
        DirectGraphics.TakeScreenshot("HurricanShot", 640, 480);

    // Screenshot machen
    if (KeyDown(DIK_F9))
        GUI.HideBox();
#endif
    return true;
}

    // --------------------------------------------------------------------------------------
    // So Firlefanz wie FPS usw anzeigen
    // --------------------------------------------------------------------------------------

#ifdef _DEBUG
void ShowDebugInfo() {
    // Blaues durchsichtiges Rechteck zeichnen
    RenderRect(0, 0, 320, 240, 0xA00000FF);
    pDefaultFont->ShowFPS();  // FPS anzeigen

    // Anzahl der aktuell aktiven Partikel anzeigen
    _itoa_s(PartikelSystem.GetNumPartikel(), StringBuffer, 10);
    pDefaultFont->DrawText(0, 60, "Partikel :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 60, StringBuffer, 0xFFFFFFFF);

    // Anzahl der aktuell aktiven Schüsse anzeigen
    _itoa_s(Projectiles.GetNumProjectiles(), StringBuffer, 10);
    pDefaultFont->DrawText(200, 60, "Projektile :", 0xFFFFFFFF);
    pDefaultFont->DrawText(300, 60, StringBuffer, 0xFFFFFFFF);

    // Benutzte Sound-Channels angeben
    _itoa_s(SoundManager.most_channels_used, StringBuffer, 10);
    pDefaultFont->DrawText(0, 75, "MaxChannels :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 75, StringBuffer, 0xFFFFFFFF);

    // Anzahl der Gegner im Level angeben
    _itoa_s(Gegner.GetNumGegner(), StringBuffer, 10);
    pDefaultFont->DrawText(200, 75, "Gegneranzahl :", 0xFFFFFFFF);
    pDefaultFont->DrawText(300, 75, StringBuffer, 0xFFFFFFFF);

    // MoveSpeed anzeigen
    _itoa_s(static_cast<int>(Timer.GetMoveSpeed()), StringBuffer, 10);
    pDefaultFont->DrawText(0, 90, "Move Speed :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 90, StringBuffer, 0xFFFFFFFF);

    // Blitzwinkel angeben
    //_itoa_s(static_cast<int>(Player->BlitzWinkel), StringBuffer, 10);
    pDefaultFont->DrawText(0, 135, "Blitzwinkel :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 135, StringBuffer, 0xFFFFFFFF);

    // Blitzwinkel angeben
    sprintf_s(StringBuffer, "%f", Timer.SpeedFaktor);
    pDefaultFont->DrawText(0, 150, "Speed :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 150, StringBuffer, 0xFFFFFFFF);

    // Blitzwinkel angeben
    // sprintf_s (StringBuffer, "%f", Player->JumpySave - Player->ypos);
    pDefaultFont->DrawText(0, 250, "yDiff :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 250, StringBuffer, 0xFFFFFFFF);

    // Blitzwinkel angeben
    // sprintf_s (StringBuffer, "%f", Player->JumpAdd);
    pDefaultFont->DrawText(0, 270, "yAdd :", 0xFFFFFFFF);
    pDefaultFont->DrawText(150, 270, StringBuffer, 0xFFFFFFFF);

    /*	for (int i=0; i<128; i++)
            for (int j=0; j<96; j++)
                if(TileEngineTiles[i][j].BackArt > 0)
                    pDefaultFont->DrawText(300+i, 100+j, ".", 0xFFFFFF00);*/
}
#endif  //_DEBUG

// DKS - added FPS reporting via command switch
void ShowFPS() {
    const unsigned int fps_update_freq_in_ticks = 500;
    static unsigned int ticks_fps_last_updated = 0;
    static int frame_ctr = 0;
    static std::stringstream char_buf;

    frame_ctr++;
    unsigned int cur_ticks = SDL_GetTicks();
    unsigned int ticks_elapsed = cur_ticks - ticks_fps_last_updated;
    if (ticks_elapsed > fps_update_freq_in_ticks && frame_ctr > 0) {
        char_buf.str("");
        float avg_fps = static_cast<float>(frame_ctr) * (1000.0f / static_cast<float>(fps_update_freq_in_ticks)) *
                        (static_cast<float>(fps_update_freq_in_ticks) / static_cast<float>(ticks_elapsed));
        char_buf << std::fixed << std::setprecision(1) << "FPS: " << avg_fps;
        std::cout << char_buf.str() << std::endl;
        frame_ctr = 0;
        ticks_fps_last_updated = cur_ticks;
    }
    pMenuFont->DrawText(0, 0, char_buf.str().c_str(), 0xFFFFFFFF);
}

//----------------------------------------------------------------------------
// Outtro starten
//----------------------------------------------------------------------------

void StartOuttro() {
    Stage = -1;
    pOuttro = new OuttroClass();
    SpielZustand = OUTTRO;
}

//----------------------------------------------------------------------------
// Intro starten
//----------------------------------------------------------------------------

void StartIntro() {
    pMenu->StartProgressBar(12);

    pIntro = new IntroClass();
    SpielZustand = INTRO;
}
