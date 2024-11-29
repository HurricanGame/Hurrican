// Datei : Gameplay.cpp

// --------------------------------------------------------------------------------------
//
// Beinhaltet den Haupt Game-Loop
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "Gameplay.hpp"
#include <stdio.h>
#include "Console.hpp"
#include "DX8Font.hpp"
#include "DX8Graphics.hpp"
#include "DX8Input.hpp"
#include "DX8Sound.hpp"
#include "DX8Sprite.hpp"
#include "GUISystem.hpp"
#include "Gegner_Helper.hpp"
#include "HUD.hpp"
#include "Logdatei.hpp"
#include "Main.hpp"
#include "Menu.hpp"
#include "Partikelsystem.hpp"
#include "Player.hpp"
#include "Projectiles.hpp"
#include "Tileengine.hpp"
#include "Timer.hpp"

// --------------------------------------------------------------------------------------
// Gameplay Variablen
// --------------------------------------------------------------------------------------

int MAX_LEVELS = 0;
int NUMPLAYERS = 1;
float FahrstuhlPos = -1.0f;
float WackelMaximum = 0.0f;  // Maximaler Screen-Wackel Ausschlag
float WackelValue = 0.0f;    // Aktueller Screen-Wackel Ausschlag
float WackelDir = 0.0f;      // Aktuelle Wackel-Richtung
float WackelSpeed = 0.0f;    // Aktuelle Wackel-Geschwindigkeit
float ScreenWinkel = 0.0f;  // in welchem zWinkel steht der Screen grad (für säulen, die das Level zum Kippen bringen)
float WarningCount = 0.0f;  // Counter, ob ein "Warning" angezeigt wird
bool JoystickFound;
bool UseForceFeedback = false;  // ForceFeedback benutzen?
bool ShowSummary = false;

long DEMOPress = 0;          // Counter bis zum nächsten Tastendruck
bool DEMORecording = false;  // demo wird grad aufgenommen
bool DEMOPlaying = false;    // demo spielt gradf ab
std::fstream DEMOFile;       // Datei in der das Demo gespeichert wird

bool FlameThrower = false;
bool HasCheated = false;

bool RunningTutorial = false;

constexpr const char* DEMO_ID = "Hurrican Demo File";  // Kennung
constexpr const char* CONFIGFILE = "Hurrican.cfg";  // Name der Konfigurationsdatei

constexpr const char* DEFAULT_LANG_FILE = "english.lng";

std::string StageReihenfolge[256];

int options_Detail;
bool StopStageMusicAtStart;
bool FlugsackFliesFree;

int DisplayHintNr = -1;

const int TextureCount[10] = {
  117, // Tutorial
  106, // Stages
  109,
   87,
   82,
  130,
  118,
  135,
   88,
  127
};

// --------------------------------------------------------------------------------------
// Ein neues Spiel initialisieren
// --------------------------------------------------------------------------------------

void InitNewGame() {
    FlameThrower = false;
    HasCheated = false;

    int NumTextures = 0;
    for (int p = 0; p < NUMPLAYERS; p++) {
        if (!Player[p].SpritesLoaded)
            NumTextures += 28;
    }

    pMenu->StartProgressBar(NumTextures);

    for (int p = 0; p < NUMPLAYERS; p++) {
        Player[p].Handlung = PlayerActionEnum::STEHEN;
        Player[p].InitPlayer(p);  // DKS: InitPlayer now takes argument specifying which
                                  //      player number each player is. InitPlayer will
                                  //      also load each player's set of sprites if they
                                  //      haven't been already.
        Player[p].InitNewLevel();
        Player[p].SecretFullGame = 0;
        Player[p].DiamondsFullGame = 0;
        Player[p].LivesFullGame = 0;
        Player[p].BlocksFullGame = 0;

        // nur im Tutorial Level
        if (RunningTutorial)
            Player[p].GodMode = true;
    }

    HUD.deactivateBossHUD();
}

// --------------------------------------------------------------------------------------
// Ein neues Level initialisieren und Level "Stage'Nr'.map" laden
// --------------------------------------------------------------------------------------

void InitNewGameLevel() {
    SoundManager.StopSong(MUSIC::STAGEMUSIC, false);
    // DKS - Might as well stop any boss music too:
    SoundManager.StopSong(MUSIC::BOSS, false);

    WarningCount = 0.0f;
    ScreenWinkel = 0.0f;

    for (int p = 0; p < NUMPLAYERS; p++) {
        Player[p].ExplodingTimer = 0.0f;
        Player[p].InitNewLevel();
    }

    // Externes Level aus Command Line laden ?
    //
    int NumTextures = 75; // unknown, use a default
    std::string Name;

    if (CommandLineParams.StartLevelPath) {
        // Directly load into level? (--startlevel)
        Name = CommandLineParams.StartLevelPath;
    } else if (CommandLineParams.RunUserLevel) {
        // Load a user level? (--level)
        Name = CommandLineParams.UserLevelName;
    } else {
        // Nein, dann normales Level in der Reihenfolge laden oder Tutorial Level
        if (RunningTutorial) {
            Name = "tutorial.map";
            NumTextures = TextureCount[0];
        } else {
            Name = StageReihenfolge[Stage - 1];
            NumTextures = TextureCount[Stage];
        }
    }

    pMenu->StartProgressBar(NumTextures);

    // und Level endlich laden
    if (!TileEngine.LoadLevel(Name)) {
        SpielZustand = GameStateEnum::MAINMENU;
        pMenu->AktuellerZustand = MENUPUNKT_STARTGAME;
        Stage = -1;
        NewStage = -1;
        return;
    }

    // Songs mit Namen aus dem Levelfile Laden
    SoundManager.LoadSong(TileEngine.GetSong(0), MUSIC::STAGEMUSIC);
    SoundManager.LoadSong(TileEngine.GetSong(1), MUSIC::BOSS);

    // DKS - Renamed
    // SoundManager.ResetAllSongVolumes();
    SoundManager.ResetAllSoundVolumes();

    pMenu->ResetProgressBar();
    DisplayLoadInfo("");
    Timer.wait(1000);
    Timer.update();

    // Menu Musik ausfaden
    // DKS - Was already commented out:
    // SoundManager.FadeSong (MUSIC::LOADING, -10.0f, 0, false);

    // DKS - Added function SongIsPlaying() to SoundManagerClass:
    if (SoundManager.SongIsPlaying(MUSIC::MENU)) {
        SoundManager.FadeSong(MUSIC::MENU, -10.0f, 0, false);

        // DKS - FadeSong above will stop the song once fade is complete, with its false parameter (last one)
        // while (SoundManager.its_Songs[MUSIC::MENU]->FadingVolume != 0.0f)
        while (SoundManager.SongIsPlaying(MUSIC::MENU)) {
            // Timer updaten
            Timer.update();
            Timer.wait();

            SoundManager.Update();
        }
    }

    // Level-Musik abspielen
    if (!StopStageMusicAtStart)
        SoundManager.PlaySong(MUSIC::STAGEMUSIC, false);

    FahrstuhlPos = -1.0f;

    SpielZustand = GameStateEnum::GAMELOOP;
}

// --------------------------------------------------------------------------------------
// Game Over Schriftzug anzeigen
// --------------------------------------------------------------------------------------

void ShowGameOver() {
    // DKS - This is not needed, and SDLPort/SDL_fmod.cpp now supports specifying if a song
    //      is to be played looped in a new parameter to PlaySong(). This was causing
    //      game-over music to never be heard in the SDL port.
    //// Game Over Musik anhalten wenn sie beendet wurde
    // if (MUSIC::IsFinished(SoundManager.its_Songs[MUSIC::GAMEOVER]->SongData))
    //    SoundManager.StopSong(MUSIC::GAMEOVER, false);

    // Transparent Wert des Game Over Schriftzuges bestimmen
    int col = static_cast<int>((50.0f - Player[0].GameOverTimer) * 10.0f);
    if (col > 255)  // Obergrenze checken
        col = 255;

    TileEngine.GameOver.RenderSprite((RENDERWIDTH - 400) * 0.5f, (RENDERHEIGHT - 90) * 0.5f, D3DCOLOR_RGBA(255, 255, 255, col));

    Player[0].GameOverTimer -= Timer.sync(0.75f);

    // GameOver vorbei ?
    // Dann schön alle löschen und auf ein neues Spiel vorbereiten
    if (Player[0].GameOverTimer <= 0.0f) {
        Player[0].GameOverTimer = 0.0f;
        pMenu->CheckForNewHighscore();

        Console.Hide();
        Stage = -1;
    }
}

// --------------------------------------------------------------------------------------
// Haupt-Spielablauf
// --------------------------------------------------------------------------------------

void GameLoop() {
    HUD.bShowArrow = false;

    // DKS - There is no need to clear the screen in the game loop, as a full-screen
    //      non-transparent background texture is always drawn first, and other
    //      portions of the game like the Cracktro or main menu have already
    //      cleared any larger-than-640x480 rendering textures in use if FBOs
    //      are enabled. Disabling screen-clear here helps embedded platforms
    //      which have very limited fill-rate.
#if 0   // DKS-DISABLED SCREEN CLEAR IN MAIN GAME LOOP
    // Total löschen
    DirectGraphics.ClearBackBuffer();
#endif  // 0

    TileEngine.NewXOffset = -1;
    TileEngine.NewYOffset = -1;

    constexpr float SPD_INC = 0.3f;
    float const SpeedFaktorMax = Timer.getSpeedFactor();

    int chunks = 1;

    // If the hardware can not render fast enough the logic catch up becomes too large
    // In this case the logic needs to be broken up into chunks
    if (SpeedFaktorMax > SPD_INC) {
        chunks = ceilf(SpeedFaktorMax / SPD_INC);
        Timer.setSpeedFactor(SpeedFaktorMax / chunks);
    }

    // Run the Logic
    for (int i = 0; i < chunks; i++) {
        for (int p = 0; p < NUMPLAYERS; p++) {
            Player[p].WasDamaged = false;

            if (!Console.Showing && Player[p].Handlung != PlayerActionEnum::TOT) {
                if (Player[p].GameOverTimer == 0.0f) {
                    // Spieler-Eingabe abfragen
                    Player[p].GetPlayerInput();
                    Player[p].AnimatePlayer();
                    Player[p].MovePlayer();
                }
            }

            Player[p].GegnerDran = false;

            // Spieler bewegen
            if (p == 0 && !Console.Showing)
                Player[p].ScrollFlugsack();
        }

        // Spieler auf einer Plattform ?
        for (int p = 0; p < NUMPLAYERS; p++)
            if (Player[p].AufPlattform != nullptr)
                Player[p].DoPlattformStuff();

        // Gegner bewegen
        if (!Console.Showing)
            Gegner.RunAll();

        // Level checken
        if (!Console.Showing)
            TileEngine.UpdateLevel();

        TileEngine.CheckBounds();
    }
    Timer.setSpeedFactor(SpeedFaktorMax);  // Restore the factor so other logic can stay in sync

    if (SpielZustand != GameStateEnum::GAMELOOP)
        return;

    // Hintergrund und Parallax Layer anzeigen
    DirectGraphics.SetColorKeyMode();
    TileEngine.CalcRenderRange();
    TileEngine.DrawBackground();

    // Evtl. rotieren, wenn Screen wackelt
    if (WackelMaximum > 0.0f)
        ScreenWackeln();

    // Level anzeigen
    TileEngine.DrawBackLevel();
    TileEngine.DrawFrontLevel();

    // Gegner anzeigen
    Gegner.RenderAll();

    // Spieler anzeigen
    for (int p = 0; p < NUMPLAYERS; p++) {
        Player[p].AlreadyDrawn = false;

        Player[p].DrawPlayer(false, false);

        if (Player[p].BlinkCounter > 0.0f)  // noch farbig blinken vom PowerUp?
        {
            if (Player[p].BlinkCounter < 4.0f) {
                int a = static_cast<int>((Player[p].BlinkCounter) * 63.0f);

                if (Player[p].BlinkColor == 1)
                    Player[p].CurrentColor = D3DCOLOR_RGBA(255, 64, 64, a);
                else if (Player[p].BlinkColor == 2)
                    Player[p].CurrentColor = D3DCOLOR_RGBA(64, 64, 255, a);
                else if (Player[p].BlinkColor == 3)
                    Player[p].CurrentColor = D3DCOLOR_RGBA(64, 255, 64, a);
                else if (Player[p].BlinkColor == 4)
                    Player[p].CurrentColor = D3DCOLOR_RGBA(128, 192, 255, a);

                Player[p].DrawPlayer(true, true);
                Player[p].DrawPlayer(true, true);
                Player[p].DrawPlayer(false, true);
            }

            Player[p].BlinkCounter -= Timer.sync(0.5f);
        } else if (Player[p].WasDamaged)
            Player[p].DrawPlayer(true, false);
    }

    // Schüsse abhandeln
    Projectiles.DoProjectiles();

    // Partikel abhandeln
    PartikelSystem.DoPartikel();

    // Overlay Tiles des Levels zeigen und Spieler und Objekte verdecken
    DirectGraphics.SetColorKeyMode();

    TileEngine.DrawWater();
    TileEngine.DrawBackLevelOverlay();
    TileEngine.DrawOverlayLevel();
    TileEngine.DrawShadow();

    // HUD anhandeln
    HUD.DoHUD();

    // ggf. BossHUD anzeigen
    HUD.RenderBossHUD();

    // Pisstext anzeigen
    ShowPissText();

    // evtl Warning Schild rendern
    if (WarningCount > 0.0f) {

        int a = static_cast<int>(WarningCount * 2.55f);
        D3DCOLOR Col = D3DCOLOR_RGBA(255, 255, 255, a);

        DirectGraphics.SetFilterMode(true);

        float off = 100.0f - WarningCount;

        pGegnerGrafix[WARNING]->SetRect(0, 0, 180, 40);
        pGegnerGrafix[WARNING]->RenderSpriteScaled(230.0f - off * 4.5f / 4.0f, 390.0f - off / 4.0f,
                                                   180 + static_cast<int>(off * 4.5f / 2.0f),
                                                   40 + static_cast<int>(off * 1.0f / 2.0f), Col);

        DirectGraphics.SetFilterMode(false);

        DirectGraphics.SetColorKeyMode();
    }

    // Blitz und andere Partikel rendern, die alles überlagern
    PartikelSystem.DoThunder();

    if (!Console.Showing) {
        // Waffen 1-3 auswählen
        if (KeyDown(DIK_1))
            Player[0].SelectedWeapon = 0;

        if (KeyDown(DIK_2) && Player[0].CurrentWeaponLevel[1] > 0)
            Player[0].SelectedWeapon = 1;

        if (KeyDown(DIK_3) && Player[0].CurrentWeaponLevel[2] > 0)
            Player[0].SelectedWeapon = 2;

        for (int p = 0; p < NUMPLAYERS; p++)
            if (Player[p].Handlung != PlayerActionEnum::TOT)
                Player[p].CheckForExplode();
    }

    // Game-Over Schrift anzeigen ?
    if (Player[0].GameOverTimer > 0.0f)
        ShowGameOver();

    // Gameloop verlassen ?
    if ((KeyDown(DIK_ESCAPE) || DirectInput.Joysticks[Player[0].JoystickIndex].ButtonStartPressed())
            && Player[0].GameOverTimer == 0.0f)
        LeaveGameLoop();

#if defined(GCW)
    // On GCW Zero, same check as above, but using the internal controls
    if (DirectInput.InternalJoystickMainMenuButtonDown() && Player[0].GameOverTimer == 0.0f)
        LeaveGameLoop();
#endif  // GCW

    /*
        if (KeyDown(DIK_F1)) Player[0].CurrentWeaponLevel[Player[0].SelectedWeapon] = 1;
        if (KeyDown(DIK_F2)) Player[0].CurrentWeaponLevel[Player[0].SelectedWeapon] = 2;
        if (KeyDown(DIK_F3)) Player[0].CurrentWeaponLevel[Player[0].SelectedWeapon] = 3;
        if (KeyDown(DIK_F4)) Player[0].CurrentWeaponLevel[Player[0].SelectedWeapon] = 4;
        if (KeyDown(DIK_F5)) Player[0].CurrentWeaponLevel[Player[0].SelectedWeapon] = 5;
        if (KeyDown(DIK_F6)) Player[0].CurrentWeaponLevel[Player[0].SelectedWeapon] = 6;
        if (KeyDown(DIK_F7)) Player[0].CurrentWeaponLevel[Player[0].SelectedWeapon] = 7;
        if (KeyDown(DIK_F8)) Player[0].CurrentWeaponLevel[Player[0].SelectedWeapon] = 8;
    */

    if (DEMORecording)
        pDefaultFont->DrawText(10, 455, "Recording demo", D3DCOLOR_RGBA(255, 255, 255, 255));

}  // GameLoop

// --------------------------------------------------------------------------------------
// Game Loop verlassen
// --------------------------------------------------------------------------------------

void LeaveGameLoop() {
    // Movespeed wieder richtig setzen
    Timer.SetMoveSpeed(10.0f);

    // Message Box verstecken
    GUI.HideBoxFast();

    // Console verstecken
    Console.Hide();

    // Schrift im Menu neu einfaden
    pMenu->Rotation = 0.0f;
    pMenu->RotationDir = 1;

    // Ins Hauptmenu wechseln
    SpielZustand = GameStateEnum::MAINMENU;
    pMenu->AktuellerZustand = MENUZUSTAND_MAINMENU;
    if (Player[0].Lives == -1 && Player[1].Lives == -1)
        // DKS - If game is over, make main menu selection be "Start New Game"
        pMenu->AktuellerPunkt = MENUPUNKT_STARTGAME;
    else
        // DKS - If the game was not over yet, make the main menu selection be "Back To Game"
        pMenu->AktuellerPunkt = MENUPUNKT_CONTINUEGAME;

    // Musik pausieren
    // DKS - Now we use use the sound manager's already-present pause functions:
    // SoundManager.StopSong(MUSIC::STAGEMUSIC, true);
    // SoundManager.StopSong(MUSIC::FLUGSACK, true);
    // SoundManager.StopAllSongs(true);
    SoundManager.PauseSongs();
    SoundManager.PauseSounds();

    // DKS
    // Loop Sounds stoppen
    // SoundManager.StopAllLoopedSounds();

    // Menu Musik spielen
    SoundManager.PlaySong(MUSIC::MENU, false);

    // TODO
    // Alle Joysticks anhalten
    // DirectInput.StopForceFeedbackEffect(FFE_BLITZ);
    // GameRunning = false;
}

// --------------------------------------------------------------------------------------
// Screen wackeln lassen
//
// Den Screen entsprechend den aktuellen Shake-Werten drehen
// Hierbei wird einfach eine Rotationsmatrix als View Matrix gesetzt mit abnehmenden Werten
// sodass alles verdreht gerendert wird ... echt clever =)
// --------------------------------------------------------------------------------------

void SetScreenShake() {
    int Winkel = static_cast<int>(ScreenWinkel + WackelValue);  // Rotationswinkel

    // Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
    //
    clampAngle(Winkel);
    glm::mat4x4 matRot = glm::rotate(glm::mat4x4(1.0f), DegreetoRad[Winkel], glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4x4 matTrans, matTrans2;  // Rotations und Translations Matrizen

    D3DXMatrixTranslation(&matTrans, -320.0f, -240.0f, 0.0f);  // Transformation zum Ursprung
    D3DXMatrixTranslation(&matTrans2, 320.0f, 240.0f, 0.0f);   // Transformation wieder zurück

    glm::mat4x4 matView = glm::mat4x4(1.0f);
    matView = matTrans * matView;   // Verschieben
    matView = matRot * matView;     // rotieren
    matView = matTrans2 * matView;  // und wieder zurück verschieben

    // rotierte Matrix setzen
    g_matView = matView;
}

// --------------------------------------------------------------------------------------
// ScreenWackel werte bearbeiten
// --------------------------------------------------------------------------------------

void ScreenWackeln() {
    // Weiterwackeln
    //
    if (!Console.Active) {
        WackelValue += Timer.sync(WackelDir * WackelSpeed);

        if (WackelValue < -WackelMaximum ||  // An der aktuellen oberen  Grenze oder
            WackelValue > WackelMaximum)     //				    unteren Grenze angekommen ?
        {
            WackelMaximum -= 0.5f;                       // Dann Grenze verkleinern
            WackelDir = -WackelDir;                      // Und Richtung umkehren
            WackelValue = WackelMaximum * (-WackelDir);  // Und an der neuen Grenze anfangen
        }

        if (WackelMaximum <= 0.0f)  // Wackeln zuende ?
            WackelMaximum = 0.0f;   // Dann aufhören damit
    }

    SetScreenShake();
}

// --------------------------------------------------------------------------------------
// Angeben wie stark der Screen wackeln soll
// --------------------------------------------------------------------------------------

void ShakeScreen(float staerke) {
    // Werte für das Screenwackeln setzen, um den Screen leicht zu schütteln
    WackelMaximum = staerke;
    WackelValue = 0.0f;
    WackelDir = 1.0f;
    WackelSpeed = static_cast<float>(GetRandom(10) + 5);
}

// --------------------------------------------------------------------------------------
// Default Config erstellen
// --------------------------------------------------------------------------------------
// DKS - New function that creates a per-player default controls configuration
void CreateDefaultControlsConfig(int player) {
    if (player < 0 || player > 1)
        return;

    if (player == 0) {
        Player[0].AktionKeyboard[AKTION_LINKS] = DIK_LEFT;
        Player[0].AktionKeyboard[AKTION_RECHTS] = DIK_RIGHT;
        Player[0].AktionKeyboard[AKTION_DUCKEN] = DIK_DOWN;
        Player[0].AktionKeyboard[AKTION_OBEN] = DIK_UP;
        Player[0].AktionKeyboard[AKTION_UNTEN] = DIK_DOWN;
#if defined(PANDORA)
        Player[0].AktionKeyboard[AKTION_JUMP] = DIK_NEXT;
        Player[0].AktionKeyboard[AKTION_SHOOT] = DIK_HOME;
        Player[0].AktionKeyboard[AKTION_BLITZ] = DIK_PRIOR;
        Player[0].AktionKeyboard[AKTION_POWERLINE] = DIK_LSHIFT;
        Player[0].AktionKeyboard[AKTION_GRANATE] = DIK_END;
        Player[0].AktionKeyboard[AKTION_SMARTBOMB] = DIK_RCONTROL;
        Player[0].AktionKeyboard[AKTION_WAFFE_SPREAD] = DIK_1;
        Player[0].AktionKeyboard[AKTION_WAFFE_LASER] = DIK_2;
        Player[0].AktionKeyboard[AKTION_WAFFE_BOUNCE] = DIK_3;
        Player[0].AktionKeyboard[AKTION_WAFFEN_CYCLE] = DIK_RETURN;
#elif defined(ANDROID)
        Player[0].AktionKeyboard[AKTION_JUMP] = DIK_A;
        Player[0].AktionKeyboard[AKTION_SHOOT] = DIK_B;
        Player[0].AktionKeyboard[AKTION_BLITZ] = DIK_C;
        Player[0].AktionKeyboard[AKTION_POWERLINE] = DIK_D;
        Player[0].AktionKeyboard[AKTION_GRANATE] = DIK_E;
        Player[0].AktionKeyboard[AKTION_SMARTBOMB] = DIK_F;
        Player[0].AktionKeyboard[AKTION_WAFFE_SPREAD] = DIK_G;
        Player[0].AktionKeyboard[AKTION_WAFFE_LASER] = DIK_H;
        Player[0].AktionKeyboard[AKTION_WAFFE_BOUNCE] = DIK_I;
        Player[0].AktionKeyboard[AKTION_WAFFEN_CYCLE] = DIK_J;
#else
        Player[0].AktionKeyboard[AKTION_JUMP] = DIK_LALT;
        Player[0].AktionKeyboard[AKTION_SHOOT] = DIK_LCONTROL;
        Player[0].AktionKeyboard[AKTION_BLITZ] = DIK_LSHIFT;
        Player[0].AktionKeyboard[AKTION_POWERLINE] = DIK_SPACE;
        Player[0].AktionKeyboard[AKTION_GRANATE] = DIK_RCONTROL;
        Player[0].AktionKeyboard[AKTION_SMARTBOMB] = DIK_RSHIFT;
        Player[0].AktionKeyboard[AKTION_WAFFE_SPREAD] = 0;
        Player[0].AktionKeyboard[AKTION_WAFFE_LASER] = 0;
        Player[0].AktionKeyboard[AKTION_WAFFE_BOUNCE] = 0;
        Player[0].AktionKeyboard[AKTION_WAFFEN_CYCLE] = DIK_RETURN;

        Player[0].AktionJoystick[AKTION_LINKS] = -1;
        Player[0].AktionJoystick[AKTION_RECHTS] = -1;
        Player[0].AktionJoystick[AKTION_DUCKEN] = -1;
        Player[0].AktionJoystick[AKTION_OBEN] = -1;
        Player[0].AktionJoystick[AKTION_UNTEN] = -1;
        Player[0].AktionJoystick[AKTION_JUMP] = 0;
        Player[0].AktionJoystick[AKTION_SHOOT] = 1;
        Player[0].AktionJoystick[AKTION_BLITZ] = 2;
        Player[0].AktionJoystick[AKTION_POWERLINE] = 3;
        Player[0].AktionJoystick[AKTION_GRANATE] = 4;
        Player[0].AktionJoystick[AKTION_SMARTBOMB] = 5;
        Player[0].AktionJoystick[AKTION_WAFFE_SPREAD] = -1;
        Player[0].AktionJoystick[AKTION_WAFFE_LASER] = -1;
        Player[0].AktionJoystick[AKTION_WAFFE_BOUNCE] = -1;
        Player[0].AktionJoystick[AKTION_WAFFEN_CYCLE] = 6;
#endif
        Player[0].Walk_UseAxxis = false;  // By default, use the HAT switch (DPAD) for movement..
        Player[0].Look_UseAxxis = false;  // and also looking.

        // DKS - Added missing default settings, and made player2's default joy index 1 instead of both being 0
        Player[0].JoystickIndex = 0;
        Player[0].JoystickSchwelle = 500.0f;
        Player[0].JoystickMode = JOYMODE_JOYPAD;
        Player[0].ControlType = CONTROLTYPE_KEYBOARD;

#if defined(GCW)
        // On GCW Zero, the Player 1 default joy index is the internal controls and both players use joystick:
        Player[0].JoystickIndex = DirectInput.GetInternalJoystickIndex();
        Player[0].ControlType = CONTROLTYPE_JOY;
        // Default button map for GCW Zero:
        Player[0].AktionJoystick[AKTION_JUMP] = 3;
        Player[0].AktionJoystick[AKTION_SHOOT] = 2;
        Player[0].AktionJoystick[AKTION_BLITZ] = 1;
        Player[0].AktionJoystick[AKTION_POWERLINE] = 6;
        Player[0].AktionJoystick[AKTION_GRANATE] = 7;
        Player[0].AktionJoystick[AKTION_SMARTBOMB] = 4;
        Player[0].AktionJoystick[AKTION_WAFFE_SPREAD] = -1;
        Player[0].AktionJoystick[AKTION_WAFFE_LASER] = -1;
        Player[0].AktionJoystick[AKTION_WAFFE_BOUNCE] = -1;
        Player[0].AktionJoystick[AKTION_WAFFEN_CYCLE] = 0;
#endif  // GCW

    } else {
        Player[1].AktionKeyboard[AKTION_LINKS] = DIK_A;
        Player[1].AktionKeyboard[AKTION_RECHTS] = DIK_D;
        Player[1].AktionKeyboard[AKTION_DUCKEN] = DIK_S;
        Player[1].AktionKeyboard[AKTION_OBEN] = DIK_W;
        Player[1].AktionKeyboard[AKTION_UNTEN] = DIK_X;
        Player[1].AktionKeyboard[AKTION_JUMP] = DIK_G;
        Player[1].AktionKeyboard[AKTION_SHOOT] = DIK_H;
        Player[1].AktionKeyboard[AKTION_BLITZ] = DIK_J;
        Player[1].AktionKeyboard[AKTION_POWERLINE] = DIK_T;
        Player[1].AktionKeyboard[AKTION_GRANATE] = DIK_Z;
        Player[1].AktionKeyboard[AKTION_SMARTBOMB] = DIK_U;
        Player[1].AktionKeyboard[AKTION_WAFFE_SPREAD] = 0;
        Player[1].AktionKeyboard[AKTION_WAFFE_LASER] = 0;
        Player[1].AktionKeyboard[AKTION_WAFFE_BOUNCE] = 0;
        Player[1].AktionKeyboard[AKTION_WAFFEN_CYCLE] = DIK_Q;

        Player[1].AktionJoystick[AKTION_LINKS] = -1;
        Player[1].AktionJoystick[AKTION_RECHTS] = -1;
        Player[1].AktionJoystick[AKTION_DUCKEN] = -1;
        Player[1].AktionJoystick[AKTION_OBEN] = -1;
        Player[1].AktionJoystick[AKTION_UNTEN] = -1;
        Player[1].AktionJoystick[AKTION_JUMP] = 0;
        Player[1].AktionJoystick[AKTION_SHOOT] = 1;
        Player[1].AktionJoystick[AKTION_BLITZ] = 2;
        Player[1].AktionJoystick[AKTION_POWERLINE] = 3;
        Player[1].AktionJoystick[AKTION_GRANATE] = 4;
        Player[1].AktionJoystick[AKTION_SMARTBOMB] = 5;
        Player[1].AktionJoystick[AKTION_WAFFE_SPREAD] = -1;
        Player[1].AktionJoystick[AKTION_WAFFE_LASER] = -1;
        Player[1].AktionJoystick[AKTION_WAFFE_BOUNCE] = -1;
        Player[1].AktionJoystick[AKTION_WAFFEN_CYCLE] = 6;

        Player[1].Walk_UseAxxis = false;  // By default, use the HAT switch (DPAD) for movement
        Player[1].Look_UseAxxis = false;  // and the analog stick for looking    -DKS

        // DKS - Added missing default settings, and made player2's default joy index 1 instead of both being 0
        Player[1].JoystickIndex = 1;
        Player[1].JoystickSchwelle = 500.0f;
        Player[1].JoystickMode = JOYMODE_JOYPAD;
        Player[1].ControlType = CONTROLTYPE_KEYBOARD;

#if defined(GCW)
        // On GCW Zero, both players use joystick by default:
        Player[1].ControlType = CONTROLTYPE_JOY;
#endif  // GCW
    }
}

void CreateDefaultConfig() {
    Protokoll << "Creating new configuration file" << std::endl;

    ActualLanguage = DEFAULT_LANG_FILE;
    bool language_loaded = LoadLanguage(ActualLanguage);
    if (!language_loaded) {
        Protokoll << "ERROR: Failed to find default language file." << std::endl;
        return;
    }

    SoundManager.SetVolumes(50, 60);

    UseForceFeedback = false;
    CreateDefaultControlsConfig(0);  // Load default controls for Player 1
    CreateDefaultControlsConfig(1);  // Load default controls for Player 2

    options_Detail = DETAIL_MAXIMUM;

#if defined(GCW)
    // Max detail is too much for GCW Zero:
    options_Detail = DETAIL_HIGH;
#endif  // GCW

    SaveConfig();
}

// --------------------------------------------------------------------------------------
// Konfiguration mit den Sound-Lautstärken laden
// Existiert diese Datei nicht, so werden die Lautstärken auf den
// Defaut Wert gesetzt
// --------------------------------------------------------------------------------------

bool LoadConfig() {
    std::string filename = g_config_ext + "/" + CONFIGFILE;

    std::ifstream Datei(filename, std::ifstream::binary);  // versuchen Datei zu öffnen

    if (!Datei)
        return false;

    // Spracheinstellung laden
    char lang[256];
    Datei.read(reinterpret_cast<char *>(&lang), sizeof(lang));
    ActualLanguage = lang;
    // DKS - Made language loading default back to english if saved language not found:
    bool language_loaded = LoadLanguage(ActualLanguage);
    if (!language_loaded) {
        Protokoll << "ERROR loading " << ActualLanguage << ", reverting to default language file." << std::endl;
        ActualLanguage = DEFAULT_LANG_FILE;
        LoadLanguage(ActualLanguage);
    }

    float Sound, Musik;

    // Daten für Sound und Musik-Lautstärke auslesen
    Datei.read(reinterpret_cast<char *>(&Sound), sizeof(Sound));
    Datei.read(reinterpret_cast<char *>(&Musik), sizeof(Musik));
    SoundManager.SetVolumes(Sound, Musik);

    // Daten für Keyboard und Joystick auslesen
    Datei.read(reinterpret_cast<char *>(&Player[0].AktionKeyboard), sizeof(Player[0].AktionKeyboard));
    Datei.read(reinterpret_cast<char *>(&Player[0].AktionJoystick), sizeof(Player[0].AktionJoystick));
    Datei.read(reinterpret_cast<char *>(&Player[0].Walk_UseAxxis), sizeof(Player[0].Walk_UseAxxis));
    Datei.read(reinterpret_cast<char *>(&Player[0].Look_UseAxxis), sizeof(Player[0].Look_UseAxxis));

    Datei.read(reinterpret_cast<char *>(&Player[1].AktionKeyboard), sizeof(Player[1].AktionKeyboard));
    Datei.read(reinterpret_cast<char *>(&Player[1].AktionJoystick), sizeof(Player[1].AktionJoystick));
    Datei.read(reinterpret_cast<char *>(&Player[1].Walk_UseAxxis), sizeof(Player[1].Walk_UseAxxis));
    Datei.read(reinterpret_cast<char *>(&Player[1].Look_UseAxxis), sizeof(Player[1].Look_UseAxxis));

    Datei.read(reinterpret_cast<char *>(&UseForceFeedback), sizeof(UseForceFeedback));

    // Sonstige Optionen laden
    Datei.read(reinterpret_cast<char *>(&options_Detail), sizeof(options_Detail));

    Datei.read(reinterpret_cast<char *>(&Player[0].ControlType), sizeof(Player[0].ControlType));
    Datei.read(reinterpret_cast<char *>(&Player[0].JoystickMode), sizeof(Player[0].JoystickMode));
    Datei.read(reinterpret_cast<char *>(&Player[0].JoystickSchwelle), sizeof(Player[0].JoystickSchwelle));

    // Joystick nicht mehr da?
    if (!DirectInput.Joysticks[Player[0].JoystickIndex].Active) {
#if defined(GCW)
        // GCW Zero player 1 defaults:
        Player[0].ControlType = CONTROLTYPE_JOY;
        Player[0].JoystickMode = JOYMODE_JOYPAD;
        Player[0].JoystickIndex = DirectInput.GetInternalJoystickIndex();
        Player[0].JoystickSchwelle = 500.0f;
#else
        Player[0].ControlType = CONTROLTYPE_KEYBOARD;
        Player[0].JoystickIndex = 0;
        Player[0].JoystickSchwelle = 500.0f;
#endif  // GCW
    }

    if (!DirectInput.Joysticks[Player[1].JoystickIndex].Active) {
#if defined(GCW)
        // GCW Zero player 2 defaults:
        Player[1].ControlType = CONTROLTYPE_JOY;
        Player[1].JoystickMode = JOYMODE_JOYPAD;
        Player[1].JoystickIndex = 1;
        Player[1].JoystickSchwelle = 500.0f;
#else
        Player[1].ControlType = CONTROLTYPE_KEYBOARD;
        Player[1].JoystickIndex = 1;  // DKS - Changed player 2's default joy index to 1
        Player[1].JoystickSchwelle = 500.0f;
#endif  // GCW
    }

    Datei.read(reinterpret_cast<char *>(&Player[1].ControlType), sizeof(Player[1].ControlType));
    Datei.read(reinterpret_cast<char *>(&Player[1].JoystickMode), sizeof(Player[1].JoystickMode));
    Datei.read(reinterpret_cast<char *>(&Player[1].JoystickSchwelle), sizeof(Player[1].JoystickSchwelle));

    Protokoll << "Config file loading successful !" << std::endl;

    Datei.close();  // Und Datei wieder schliessen

    PartikelSystem.SetParticleCount();

    if (!JoystickFound) {
        Player[0].ControlType = CONTROLTYPE_KEYBOARD;
        Player[1].ControlType = CONTROLTYPE_KEYBOARD;
    }

    if (Player[0].JoystickSchwelle < 1.0f)
        Player[0].JoystickSchwelle = 500.0f;

    if (Player[1].JoystickSchwelle < 1.0f)
        Player[1].JoystickSchwelle = 500.0f;

    return true;
}

// --------------------------------------------------------------------------------------
// Aktuelle Konfiguration mit den Sound-Lautstärken speichern
// --------------------------------------------------------------------------------------

void SaveConfig() {

    std::string filename = g_config_ext + "/" + CONFIGFILE;

    std::ofstream Datei(filename, std::ifstream::binary);

    if (!Datei) {
        Protokoll << "Config file saving failed !" << std::endl;
        return;
    }

    // Spracheinstellung speichern
    char lang[256];
    std::size_t length = ActualLanguage.copy(lang, 255);
    lang[length]='\0';
    Datei.write(reinterpret_cast<char *>(&lang), sizeof(lang));

    // Daten für Sound und Musik-Lautstärke schreiben
    float Sound = SoundManager.g_sound_vol;
    float Musik = SoundManager.g_music_vol;

    Datei.write(reinterpret_cast<char *>(&Sound), sizeof(Sound));
    Datei.write(reinterpret_cast<char *>(&Musik), sizeof(Musik));

    // Daten für Keyboard und Joystick schreiben
    Datei.write(reinterpret_cast<char *>(&Player[0].AktionKeyboard), sizeof(Player[0].AktionKeyboard));
    Datei.write(reinterpret_cast<char *>(&Player[0].AktionJoystick), sizeof(Player[0].AktionJoystick));
    Datei.write(reinterpret_cast<char *>(&Player[0].Walk_UseAxxis), sizeof(Player[0].Walk_UseAxxis));
    Datei.write(reinterpret_cast<char *>(&Player[0].Look_UseAxxis), sizeof(Player[0].Look_UseAxxis));

    Datei.write(reinterpret_cast<char *>(&Player[1].AktionKeyboard), sizeof(Player[1].AktionKeyboard));
    Datei.write(reinterpret_cast<char *>(&Player[1].AktionJoystick), sizeof(Player[1].AktionJoystick));
    Datei.write(reinterpret_cast<char *>(&Player[1].Walk_UseAxxis), sizeof(Player[1].Walk_UseAxxis));
    Datei.write(reinterpret_cast<char *>(&Player[1].Look_UseAxxis), sizeof(Player[1].Look_UseAxxis));

    Datei.write(reinterpret_cast<char *>(&UseForceFeedback), sizeof(UseForceFeedback));

    // Sonstige Optionen sichern
    Datei.write(reinterpret_cast<char *>(&options_Detail), sizeof(options_Detail));

    Datei.write(reinterpret_cast<char *>(&Player[0].ControlType), sizeof(Player[0].ControlType));
    Datei.write(reinterpret_cast<char *>(&Player[0].JoystickMode), sizeof(Player[0].JoystickMode));
    Datei.write(reinterpret_cast<char *>(&Player[0].JoystickSchwelle), sizeof(Player[0].JoystickSchwelle));

    Datei.write(reinterpret_cast<char *>(&Player[1].ControlType), sizeof(Player[1].ControlType));
    Datei.write(reinterpret_cast<char *>(&Player[1].JoystickMode), sizeof(Player[1].JoystickMode));
    Datei.write(reinterpret_cast<char *>(&Player[1].JoystickSchwelle), sizeof(Player[1].JoystickSchwelle));

    Datei.close();  // Und Datei wieder schliessen
}

// --------------------------------------------------------------------------------------
// Beim Loading Screen anzeigen, was alles geladen wurde, dabei
// mehrere Zeilen anzeigen und immer bei neuem Text eins nach oben rutschen
// --------------------------------------------------------------------------------------

bool DisplayLoadInfo(const char Text[100]) {
    if (NochKeinFullScreen || pMenu == nullptr)
        return false;
    // TODO FIX
    /*
        strrev (Text);				// String umdrehen
        strnset(Text, ' ', 2);		// Ersten zwei (vorher letzten Zwei = \n) Buchstaben löschen
        strrev (Text);				// Wieder richtig herum drehen
        */

    // Anzeigen im Loadingscreen
    DirectGraphics.ClearBackBuffer();

    pMenu->ShowMenuBack();

    DirectGraphics.SetAdditiveMode();

    pMenuFont->DrawTextCenterAlign(320, 200, TextArray[TEXT::MENUE_LOADING], 0xFFFFFFFF);

    // Anzahl anzeigen
#ifndef NDEBUG
    std::string buf = std::to_string(pMenu->ItemsLoaded());

    pDefaultFont->DrawText((RENDERWIDTH - pDefaultFont->StringLength(buf.c_str())) / 2.0f, 300,
                           buf.c_str(), 0xFFFFFFFF);
#endif

    // Hint anzeigen
    // DKS - Added support for displaying hints on low-resolution devices:
    if (DisplayHintNr > -1) {
        if (CommandLineParams.LowRes) {
            const char *text = TextArray[TEXT::HINT1 + DisplayHintNr];
            constexpr float Y_POS = 270.0f;
            constexpr float Y_INC = 28.0f;
            constexpr int MAX_WIDTH = RENDERWIDTH - 20;
            if (pDefaultFont->StringLength(text, 0) > MAX_WIDTH) {
                // Split the line in two if too long to display on low-res device:
                char text1[255];
                char text2[255];
                SplitLine(text1, text2, text);
                pDefaultFont->DrawTextCenterAlign(320.0f, Y_POS, text1, 0xFFFFFFFF, 0);
                pDefaultFont->DrawTextCenterAlign(320.0f, Y_POS + Y_INC, text2, 0xFFFFFFFF, 0);
            } else {
                pDefaultFont->DrawTextCenterAlign(320.0f, Y_POS, text, 0xFFFFFFFF, 0);
            }
        } else {
            pDefaultFont->DrawTextCenterAlign(320.0f, 270.0f, TextArray[TEXT::HINT1 + DisplayHintNr], 0xFFFFFFFF, 0);
        }
    }

    pMenu->LoadingScreen.RenderSprite((RENDERWIDTH - 360) / 2, (RENDERHEIGHT - 60) / 2 + 5, 0x88FFFFFF);

    pMenu->UpdateProgressBar();

    /*for (i=0; i<24; i++)
        pDefaultFont->DrawText(10, static_cast<float>(230+i*10), LoadInfoText[i], D3DCOLOR_RGBA(0, 255, 0, i*10));*/

    DirectGraphics.ShowBackBuffer();

    Timer.wait(1);
    Timer.update();

    return true;
}

// --------------------------------------------------------------------------------------
// Stage Clear Musik dudelt und Spieler läuft aus dem Screen raus
// --------------------------------------------------------------------------------------

void StageClear(bool PlaySong) {
    for (int p = 0; p < NUMPLAYERS; p++) {
        Player[p].StageClearRunning = true;
        Player[p].CanBeDamaged = false;
        Player[p].PunisherActive = false;
    }

    GUI.HideBoxFast();

    // DKS - Stop all songs instead of just individual ones..
    // SoundManager.StopSong(MUSIC::STAGEMUSIC, false);

    //// Punisher Musik stoppen
    // if (MUSIC::IsPlaying(SoundManager.its_Songs[MUSIC::PUNISHER]->SongData)) {
    //    SoundManager.StopSong(MUSIC::PUNISHER, false);
    //}

    SoundManager.StopSongs();  // DKS - Added, see above

    if (PlaySong)
        SoundManager.PlaySong(MUSIC::STAGECLEAR, false);
}

// --------------------------------------------------------------------------------------
// Summary Screen anzeigen
// --------------------------------------------------------------------------------------

// DKS - Fixed display on low-res scaled-font devices, and made display on
//      normal-resolution devices be centered and spaced properly.
void SummaryScreen() {
    bool reveal_cheat = !RunningTutorial && (Player[0].DiamondsThisLevel == TileEngine.MaxDiamonds);

    // DKS - Added counter to prevent accidental early-exit:
    constexpr float DELAY_CAN_LEAVE = 400.0f;
    constexpr float DELAY_INC = 30.0f;
    float delay_ctr = 0.0f;

    ShowSummary = true;

    // DKS - Note: boxes dimensions must be multiple of TILESIZE (20) due to quirks in GUISystem.cpp

    // Normal screen dimensions:
    int const box_w = 360;
    int const box_h = reveal_cheat ? 140 : 120;
    int const box_x = RENDERWIDTH / 2 - box_w / 2;
    int const box_y = RENDERHEIGHT / 2 - box_h / 2;

    int const title_txt_y = box_y - 10;  // Note that borders of boxes mean drawable region is a bit higher than specified
    int const pressanykey_txt_y = box_y + box_h - 8;
    constexpr int SPRITE_SPACING = 96;
    int const sprites_y = box_y + 40;
    int const sprite1_x = (RENDERWIDTH / 2) - SPRITE_SPACING - SPRITE_SPACING / 2;
    int const sprite2_x = (RENDERWIDTH / 2) - SPRITE_SPACING / 2;
    int const sprite3_x = (RENDERWIDTH / 2) + SPRITE_SPACING / 2;
    int const secrets_x = (RENDERWIDTH / 2) + SPRITE_SPACING + SPRITE_SPACING / 2;
    int const stats_txt_y = sprites_y + 30;
    int const cheat_txt_y = (stats_txt_y + pressanykey_txt_y) / 2;

    // Summary Box erzeugen
    // DKS - NOTE: boxes are drawn less-than intuitively, you must use dimensions multiples of TILESIZE
    GUI.ShowBox(box_x, box_y, box_w, box_h);

    bool all_controls_unpressed_yet = false;
    bool leave = false;
    while (!leave) {
        // alte Darstellung beenden

        // Mit dem Darstellen beginnen

        TileEngine.DrawBackground();  // Level abhandeln
        TileEngine.DrawBackLevel();
        TileEngine.DrawFrontLevel();

        Gegner.RunAll();
        Gegner.RenderAll();
        Projectiles.DoProjectiles();  // Schüsse abhandeln

        // Overlay Tiles des Levels zeigen und Spieler und Objekte verdecken
        TileEngine.DrawOverlayLevel();
        PartikelSystem.DoPartikel();  // Partikel abhandeln

        TileEngine.DrawWater();
        TileEngine.DrawBackLevelOverlay();
        TileEngine.DrawOverlayLevel();
        TileEngine.DrawShadow();

        HUD.DoHUD();  // HUD anhandeln

        // Blitz und andere Partikel rendern, die alles überlagern
        PartikelSystem.DoThunder();

        // Summary Screen rendern
        GUI.Run();
        D3DCOLOR color = D3DCOLOR_RGBA(0, 255, 0, static_cast<int>(GUI.GetFadingAlpha()));
        pDefaultFont->DrawText(
            static_cast<float>((RENDERWIDTH - pDefaultFont->StringLength(TextArray[TEXT::SUMMARY_TITLE])) / 2),
            static_cast<float>(title_txt_y), TextArray[TEXT::SUMMARY_TITLE], color);

        if (delay_ctr >= DELAY_CAN_LEAVE) {
            std::string str_pressanykey(TextArray[TEXT::SUMMARY_PRESSFIRE]);

            // If player 1 is controlled with joystick, replace all references to 'key' with 'button'
            if (Player[0].ControlType == CONTROLTYPE_JOY) {
                ReplaceAll(str_pressanykey, "key", "button");
            }

            pDefaultFont->DrawText(
                static_cast<float>((RENDERWIDTH - pDefaultFont->StringLength(str_pressanykey.c_str())) / 2),
                static_cast<float>(pressanykey_txt_y), str_pressanykey.c_str(), color);
        }

        pGegnerGrafix[POWERBLOCK]->RenderSpriteScaled(static_cast<float>(sprite1_x - 16),
                                                      static_cast<float>(sprites_y - 16), 32, 32, 1, color);
        pGegnerGrafix[DIAMANT]->RenderSprite(static_cast<float>(sprite2_x - 14),
                                             static_cast<float>(sprites_y - 14), 0, color);
        pGegnerGrafix[ONEUP]->RenderSpriteScaled(static_cast<float>(sprite3_x - 16),
                                                 static_cast<float>(sprites_y - 16), 32, 32, 0, color);
        pDefaultFont->DrawText(
            static_cast<float>(secrets_x - pDefaultFont->StringLength(TextArray[TEXT::SUMMARY_SECRETS]) / 2),
            static_cast<float>(sprites_y - pDefaultFont->GetYCharSize() / 2), TextArray[TEXT::SUMMARY_SECRETS],
            color);

        std::string buf;
        buf = std::to_string(Player[0].BlocksThisLevel)
            .append("/")
            .append(std::to_string(TileEngine.MaxBlocks));
        pDefaultFont->DrawText(static_cast<float>(sprite1_x - pDefaultFont->StringLength(buf.c_str()) / 2), 
                               static_cast<float>(stats_txt_y), buf.c_str(), color);

        buf = std::to_string(Player[0].DiamondsThisLevel)
            .append("/")
            .append(std::to_string(TileEngine.MaxDiamonds));
        pDefaultFont->DrawText(static_cast<float>(sprite2_x - pDefaultFont->StringLength(buf.c_str()) / 2),
                               static_cast<float>(stats_txt_y), buf.c_str(), color);

        buf = std::to_string(Player[0].LivesThisLevel)
            .append("/")
            .append(std::to_string(TileEngine.MaxOneUps));
        pDefaultFont->DrawText(static_cast<float>(sprite3_x - pDefaultFont->StringLength(buf.c_str()) / 2),
                               static_cast<float>(stats_txt_y), buf.c_str(), color);

        buf = std::to_string(Player[0].SecretThisLevel)
            .append("/")
            .append(std::to_string(TileEngine.MaxSecrets));
        pDefaultFont->DrawText(static_cast<float>(secrets_x - pDefaultFont->StringLength(buf.c_str()) / 2),
                               static_cast<float>(stats_txt_y), buf.c_str(), color);

        // Cheat freigespielt? -> Wenn alle Diamanten gefunden
        if (reveal_cheat) {
            char buf2[50];
            strcpy_s(buf2, Cheats[Stage - 1]);
            unsigned int const len = strlen(buf2);
            for (unsigned int p = 0; p < len; p++)
                buf2[p] ^= 64;

            buf = std::string(TextArray[TEXT::SUMMARY_CHEATUNLOCK])
                .append(": ")
                .append(buf2);
            pDefaultFont->DrawText(static_cast<float>(RENDERWIDTH / 2 - pDefaultFont->StringLength(buf.c_str(), 0) / 2),
                                   static_cast<float>(cheat_txt_y), buf.c_str(), color,
                                   0);
        }

        // Darstellung beenden

        // Backbuffer mit Frontbuffer tauschen
        DirectGraphics.ShowBackBuffer();

        Timer.update();
        Timer.wait();

        DirectInput.UpdateTastatur();
        DirectInput.UpdateJoysticks();
        bool keypressed = DirectInput.AnyKeyDown();
        bool buttonpressed = DirectInput.AnyButtonDown();

        if (!keypressed && !buttonpressed) {
            all_controls_unpressed_yet = true;
        }

        if (all_controls_unpressed_yet && delay_ctr >= DELAY_CAN_LEAVE) {
            if (buttonpressed || keypressed)
                leave = true;
        }

        // Musik zuende ?
        // DKS - Theere's no need to stop the song, as it isn't looped
        // if (MUSIC::IsFinished(SoundManager.its_Songs[MUSIC::STAGECLEAR]->SongData))
        //    SoundManager.StopSong(MUSIC::STAGECLEAR, false);

        delay_ctr += Timer.sync(DELAY_INC);
        if (delay_ctr > DELAY_CAN_LEAVE)
            delay_ctr = DELAY_CAN_LEAVE;
    }

    //    //DKS - I revamped this summary screen code quite a bit so that no delay should ever be necessary
    //    //    SDL_Delay( 3000 );
    while (SoundManager.SongIsPlaying(MUSIC::STAGECLEAR))
        ;

    // DKS - There's no need to stop it, it will stop on its own as it's not looped:
    // SoundManager.StopSong(MUSIC::STAGECLEAR, false);

    GUI.HideBoxFast();

    ShowSummary = false;
}

// --------------------------------------------------------------------------------------
// Demo Aufnahme starten
// --------------------------------------------------------------------------------------

bool NewDemo(const char Filename[]) {
    std::string fullpath = g_save_ext + "/" + Filename;

    DEMOFile.open(fullpath, std::ofstream::binary);

    if (!DEMOFile) {
        Protokoll << "\n-> Error opening Demo File !" << std::endl;
        GameRunning = false;
        return false;
    }

    // DateiHeader schreiben
    //
    DEMOFile.write(reinterpret_cast<const char *>(&DEMO_ID), sizeof(DEMO_ID));

    // Stage Nr schreiben
    DEMOFile.write(reinterpret_cast<char *>(&Stage), sizeof(Stage));

    DEMORecording = true;
    DEMOPress = 0;

    // Tasten auf false setzen
    Player[0].Aktion.reset();

    // Level neu initialisieren und dann gehts los
    int const l = Stage;

    srand(5);

    InitNewGame();
    Stage = l;
    NewStage = l;
    InitNewGameLevel();

    // Timer auf 60 fps für Demo setzen
    Timer.SetMaxFPS(40);

    return true;

}  // NewDemo

// --------------------------------------------------------------------------------------
// Demo Laden
// --------------------------------------------------------------------------------------

bool LoadDemo(const char Filename[]) {
    TileEngine.XOffset = 0;
    TileEngine.YOffset = 0;

    std::string fullpath = g_save_ext + "/" + Filename;

    DEMOFile.open(fullpath, std::fstream::in | std::fstream::binary);

    if (!DEMOFile)
        return false;

    char Kennung[20];

    // DateiHeader lesen
    //
    DEMOFile.read(reinterpret_cast<char *>(&Kennung), sizeof(DEMO_ID));

    if (strcmp(DEMO_ID, Kennung) != 0)
        return false;

    // Stage Nr laden
    DEMOFile.read(reinterpret_cast<char *>(&Stage), sizeof(Stage));
    NewStage = Stage;

    DEMOPlaying = true;
    DEMOPress = 0;

    // Tasten auf false setzen
    Player[0].Aktion.reset();

    // Level neu initialisieren und dann gehts los
    int const l = NewStage;

    srand(5);

    InitNewGame();
    Stage = l;
    NewStage = l;
    InitNewGameLevel();

    // Timer auf 60 fps für Demo setzen
    Timer.SetMaxFPS(40);

    return true;

}  // LoadDemo

// --------------------------------------------------------------------------------------
// Demo beenden
// --------------------------------------------------------------------------------------

void EndDemo() {
    // File schliessen
    DEMOFile.close();

    DEMOPlaying = false;
    DEMORecording = false;

    Timer.SetMaxFPS(0);

    // DKS - added this to ensure RNG is always properly seeded
    srand(SDL_GetTicks());

}  // EndDemo

// --------------------------------------------------------------------------------------
// Demo aufnehmen
// --------------------------------------------------------------------------------------

void RecordDemo() {
    /*srand (DEMOPress++);
    if (DEMOPress > 32000)
        DEMOPress = 0;*/

    // Tasten speichern
    //
    for (int i = 0; i < MAX_AKTIONEN; i++) {
        char aktion = Player[0].Aktion[i];
        DEMOFile.write(reinterpret_cast<char *>(&aktion), sizeof(aktion));
    }

    // FPS speichern
    //
    int fps = static_cast<int>(Timer.getFrameRate());
    DEMOFile.write(reinterpret_cast<char *>(&fps), sizeof(fps));

}  // RecordDemo

// --------------------------------------------------------------------------------------
// Demo abspielen
// --------------------------------------------------------------------------------------

void PlayDemo() {
    /*srand (DEMOPress++);
    if (DEMOPress > 32000)
        DEMOPress = 0;*/

    // Tasten laden
    //
    for (int i = 0; i < MAX_AKTIONEN; i++) {
        char aktion;
        DEMOFile.read(reinterpret_cast<char *>(&aktion), sizeof(aktion));
        Player[0].Aktion[i] = aktion;
    }

    // FPS laden
    //
    int fps;

    DEMOFile.read(reinterpret_cast<char *>(&fps), sizeof(fps));
    Timer.SetMaxFPS(fps);

    // Demo Ende ?
    if (DEMOFile.eof()) {
        EndDemo();
        LoadDemo("Demo.dem");
    }
}  // PlayDemo

// --------------------------------------------------------------------------------------
// Nach dem Boss den Screen wieder auf den Spieler zentrieren und Musik einfaden etc.
// --------------------------------------------------------------------------------------

void ScrolltoPlayeAfterBoss() {
    // Level wieder zum Spieler scrollen und dann weiterscrollen lassen
    TileEngine.Zustand = TileStateEnum::SCROLLBAR;
    TileEngine.MustCenterPlayer = true;

    // Level Musik wieder einfaden lassen (aus Pause Zustand)
    SoundManager.FadeSong(MUSIC::STAGEMUSIC, 2.0f, 100, true);
}

// --------------------------------------------------------------------------------------
// Piss-Text anzeigen =)
// --------------------------------------------------------------------------------------

void ShowPissText() {
    if (Player[0].BronsonCounter > 220.0f) {
        int TextNr = static_cast<int>((Player[0].BronsonCounter - 220.0f) / 50.0f);

        // DKS - Trying to get the infamous "piss text" centered on the player..
        //      I had gotten the boxes centered in the tutorial, only to find
        //      them off-centered here. I had to offset by TILESIZE*2 to get
        //      them centered properly horizontally, which I found frustrating.
        //      Oh well..
        //        GUI.ShowBox(TextArray[TEXT::PISS_1 + TextNr],
        //                      static_cast<int>(Player[0].ypos - 70 - TileEngine.YOffset),
        //                      static_cast<int>(Player[0].xpos - TileEngine.XOffset) - 10);
        GUI.ShowBox(TextArray[TEXT::PISS_1 + TextNr],
                    static_cast<int>(Player[0].ypos - 70.0f - TileEngine.YOffset),
                    static_cast<int>(Player[0].xpos - TileEngine.XOffset) + TILESIZE * 2);

        if (Player[0].BronsonCounter > 220.0f + 50.0f * 18.0f)
            Player[0].BronsonCounter = 270.0f;
    }
}

// --------------------------------------------------------------------------------------
// Zufällig ein Ziel aussuchen
// --------------------------------------------------------------------------------------

PlayerClass *ChooseAim() {

    PlayerClass *pAim = &Player[rand() % NUMPLAYERS];

    if (pAim == &Player[0] && Player[0].Handlung == PlayerActionEnum::TOT)
        pAim = &Player[1];

    if (pAim == &Player[1] && Player[1].Handlung == PlayerActionEnum::TOT)
        pAim = &Player[0];

    return pAim;
}

const char *convertText(const char *text) {
    // Text konvertieren
    static char temp[MAX_CHARS];

    unsigned int const len = strlen(text);
    unsigned int i = 0;
    for (i = 0; i < len; i++)
        temp[i] = text[i] ^ 64;
    temp[i] = 0;

    return temp;
}
