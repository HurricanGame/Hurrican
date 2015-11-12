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

#include <stdio.h>
//#include "CCutScene.h"    //DKS - CCutScene code was never used in original game; disabled it
#include "Gameplay.h"
#include "Console.h"
#include "DX8Font.h"
#include "DX8Graphics.h"
#include "DX8Input.h"
#include "DX8Sprite.h"
#include "DX8Sound.h"
#include "Gegner_Helper.h"
#include "GUISystem.h"
#include "HUD.h"
#include "Logdatei.h"
#include "Main.h"
#include "Menu.h"
#include "Partikelsystem.h"
#include "Player.h"
#include "Projectiles.h"
#include "Tileengine.h"
#include "Timer.h"

// --------------------------------------------------------------------------------------
// Gameplay Variablen
// --------------------------------------------------------------------------------------

int		MAX_LEVELS	   = 0;
int		NUMPLAYERS	   = 1;
float	FahrstuhlPos   = -1.0f;
float	WackelMaximum  = 0.0f;								// Maximaler Screen-Wackel Ausschlag
float	WackelValue    = 0.0f;								// Aktueller Screen-Wackel Ausschlag
float	WackelDir	   = 0.0f;								// Aktuelle Wackel-Richtung
float	WackelSpeed	   = 0.0f;								// Aktuelle Wackel-Geschwindigkeit
float	ScreenWinkel   = 0.0f;								// in welchem zWinkel steht der Screen grad (für säulen, die das Level zum Kippen bringen)
float	WarningCount   = 0.0f;								// Counter, ob ein "Warning" angezeigt wird
bool	JoystickFound;
bool	UseForceFeedback = false;							// ForceFeedback benutzen?
bool	ShowSummary = false;

long    DEMOPress		= 0;								// Counter bis zum nächsten Tastendruck
bool	DEMORecording	= false;							// demo wird grad aufgenommen
bool	DEMOPlaying		= false;							// demo spielt gradf ab
FILE   *DEMOFile		= NULL;								// Datei in der das Demo gespeichert wird

bool	FlameThrower = false;
bool	HasCheated = false;

bool	RunningTutorial = false;

#define DEMO_ID	"Hurrican Demo File"						// Kennung

char	StageReihenfolge[256][100];

int 	options_Detail;
bool	StopStageMusicAtStart;
bool	FlugsackFliesFree;

int		DisplayHintNr = -1;

// --------------------------------------------------------------------------------------
// Ein neues Spiel initialisieren
// --------------------------------------------------------------------------------------

void InitNewGame(void)
{
    FlameThrower = false;
    HasCheated = false;

    for (int p = 0; p < NUMPLAYERS; p++)
    {
        Player[p].Handlung = STEHEN;
        Player[p].InitPlayer(p);      // DKS: InitPlayer now takes argument specifying which
                                      //      player number each player is. InitPlayer will
                                      //      also load each player's set of sprites if they
                                      //      haven't been already.
        Player[p].InitNewLevel();
        Player[p].SecretFullGame   = 0;
        Player[p].DiamondsFullGame = 0;
        Player[p].LivesFullGame	  = 0;
        Player[p].BlocksFullGame	  = 0;

        // nur im Tutorial Level
        if (RunningTutorial == true)
            Player[p].GodMode = true;
    }

    HUD.BossHUDActive = 0.0f;
}

// --------------------------------------------------------------------------------------
// Ein neues Level initialisieren und Level "Stage'Nr'.map" laden
// --------------------------------------------------------------------------------------

void InitNewGameLevel(int Nr)
{
    char Name[100];

    SoundManager.StopSong(MUSIC_STAGEMUSIC, false);
    //DKS - Might as well stop any boss music too:
    SoundManager.StopSong(MUSIC_BOSS, false);

    pMenu->LoadingItemsLoaded	= 0;
    pMenu->LoadingItemsToLoad	= 75.0f;
    pMenu->LoadingProgress		= 0.0f;
    WarningCount		= 0.0f;
    ScreenWinkel		= 0.0f;

    for (int p = 0; p < NUMPLAYERS; p++)
    {
        Player[p].ExplodingTimer	= 0.0f;
        Player[p].InitNewLevel();
    }

    // Externes Level aus Command Line laden ?
    //
    if (CommandLineParams.RunUserLevel == false)
    {
        // Nein, dann normales Level in der Reihenfolge laden oder Tutorial Level
        if (RunningTutorial == true)
            strcpy_s(Name, strlen("tutorial.map") + 1, "tutorial.map");
        else
            strcpy_s(Name, strlen(StageReihenfolge[Stage-1]) + 1, StageReihenfolge[Stage-1]);
    }
    else
        strcpy_s (Name, strlen(CommandLineParams.UserLevelName) + 1, CommandLineParams.UserLevelName);

    // und Level endlich laden
    if (TileEngine.LoadLevel(Name) == false)
    {
        SpielZustand = MAINMENU;
        pMenu->AktuellerZustand = MENUPUNKT_STARTGAME;
        Stage = -1;
        NewStage = -1;
        return;
    }

    // Songs mit Namen aus dem Levelfile Laden
    SoundManager.LoadSong(TileEngine.DateiAppendix.Songs[0], MUSIC_STAGEMUSIC);
    SoundManager.LoadSong(TileEngine.DateiAppendix.Songs[1], MUSIC_BOSS);

    //DKS - Renamed
    //SoundManager.ResetAllSongVolumes();
    SoundManager.ResetAllSoundVolumes();

    pMenu->LoadingProgress = 320.0f;
    DisplayLoadInfo ("");
    Timer.wait  (1000);
    Timer.update();

    // Menu Musik ausfaden
    //DKS - Was already commented out:
    //SoundManager.FadeSong (MUSIC_LOADING, -10.0f, 0, false);

    //DKS - Added function SongIsPlaying() to SoundManagerClass:
    if (SoundManager.SongIsPlaying(MUSIC_MENU))
    {
        SoundManager.FadeSong (MUSIC_MENU, -10.0f, 0, false);

        //DKS - FadeSong above will stop the song once fade is complete, with its false parameter (last one)
        //while (SoundManager.its_Songs[MUSIC_MENU]->FadingVolume != 0.0f)
        while (SoundManager.SongIsPlaying(MUSIC_MENU))
        {
            // Timer updaten
            Timer.update();
            Timer.wait();
            SpeedFaktor = Timer.SpeedFaktor;

            SoundManager.Update();
        }
    }

    // Level-Musik abspielen
    if (StopStageMusicAtStart == false)
        SoundManager.PlaySong(MUSIC_STAGEMUSIC, false);

    FahrstuhlPos = -1.0f;

    SpielZustand = GAMELOOP;
}

// --------------------------------------------------------------------------------------
// Game Over Schriftzug anzeigen
// --------------------------------------------------------------------------------------

void ShowGameOver(void)
{
    //DKS - This is not needed, and SDLPort/SDL_fmod.cpp now supports specifying if a song
    //      is to be played looped in a new parameter to PlaySong(). This was causing
    //      game-over music to never be heard in the SDL port.
    //// Game Over Musik anhalten wenn sie beendet wurde
    //if (MUSIC_IsFinished(SoundManager.its_Songs[MUSIC_GAMEOVER]->SongData))
    //    SoundManager.StopSong(MUSIC_GAMEOVER, false);

    int col;

    // Transparent Wert des Game Over Schriftzuges bestimmen
    col = int ((50.0f - Player[0].GameOverTimer)*10);
    if (col > 255)							// Obergrenze checken
        col = 255;

    TileEngine.GameOver.RenderSprite((640 - 400) * 0.5f, (480 - 90) * 0.5f, D3DCOLOR_RGBA(255, 255, 255, col));

    Player[0].GameOverTimer -= 0.75f SYNC;

    // GameOver vorbei ?
    // Dann schön alle löschen und auf ein neues Spiel vorbereiten
    if (Player[0].GameOverTimer <= 0.0f)
    {
        Player[0].GameOverTimer = 0.0f;
        pMenu->CheckForNewHighscore();

        Console.Hide();
        Stage = -1;
    }
}

// --------------------------------------------------------------------------------------
// Haupt-Spielablauf
// --------------------------------------------------------------------------------------

void GameLoop(void)
{
    HUD.bShowArrow = false;

    // Total löschen
#if defined(PLATFORM_DIRECTX)
    //DKS - Since I removed all use of the Z-coordinate, this should be changed too. Note: DirectX is entirely untested.
    //lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
    //                   D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
    lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET,
                       D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
#elif defined(PLATFORM_SDL)
    DirectGraphics.ClearBackBuffer();
#endif

    TileEngine.NewXOffset = -1;
    TileEngine.NewYOffset = -1;

#define SPD_INC 0.3f
    float i = 0;
    float SpeedFaktorMax = SpeedFaktor;

    while (i<SpeedFaktorMax)
    {
        // If the hardware can not render fast enough the logic catch up becomes too large
        // In this case the logic needs to be broken up into chunks
        if (SpeedFaktorMax < SPD_INC)           // hardware is fast and sync does not need to be split
        {
            SpeedFaktor = SpeedFaktorMax;
            i = SpeedFaktorMax;
        }
        else if (SpeedFaktorMax-i < SPD_INC)    // sync has been split and only a small value is needed to meet the total sync request
        {
            SpeedFaktor = SpeedFaktorMax-i;
            i = SpeedFaktorMax;
        }
        else                                    // hardware is not fast and sync does need to be split into chunks
        {
            SpeedFaktor = SPD_INC;
            i += SPD_INC;
        }

        for (int p = 0; p < NUMPLAYERS; p++)
        {
            Player[p].WasDamaged = false;

            if (Console.Showing == false &&
                    Player[p].Handlung != TOT)
            {
                if (Player[p].GameOverTimer == 0.0f)
                {
                    // Spieler-Eingabe abfragen
                    Player[p].GetPlayerInput();
                    Player[p].AnimatePlayer();
                    Player[p].MovePlayer();
                }
            }

            Player[p].GegnerDran = false;

            // Spieler bewegen
            if (p == 0 &&
                    Console.Showing == false)
                Player[p].ScrollFlugsack();
        }

        // Spieler auf einer Plattform ?
        for (int p = 0; p < NUMPLAYERS; p++)
            if (Player[p].AufPlattform != NULL)
                Player[p].DoPlattformStuff();

        // Gegner bewegen
        if (Console.Showing == false)
            Gegner.RunAll();

        // Level checken
        if (Console.Showing == false)
            TileEngine.UpdateLevel();

        TileEngine.CheckBounds();
    }
    SpeedFaktor = SpeedFaktorMax;   // Restore the factor so other logic can stay in sync

    if (SpielZustand != GAMELOOP)
        return;

    // Hintergrund und Parallax Layer anzeigen
    DirectGraphics.SetColorKeyMode();
    TileEngine.CalcRenderRange();
    TileEngine.DrawBackground();

    // Drache zb
    //DKS - this was an empty function in the original code, disabling it:
    //TileEngine.DrawSpecialLayer();

    // Evtl. rotieren, wenn Screen wackelt
    if (WackelMaximum > 0.0f)
        ScreenWackeln();

    // Level anzeigen
    TileEngine.DrawBackLevel();
    TileEngine.DrawFrontLevel();

    //DKS - Lightmap code in original game was never used and all related code has now been disabled:
    //// LighMaps löschen
    //if (options_Detail >= DETAIL_HIGH)
    //    TileEngine.ClearLightMaps();

    // Gegner anzeigen
    Gegner.RenderAll();

    // Spieler anzeigen
    for (int p = 0; p < NUMPLAYERS; p++)
    {
        Player[p].AlreadyDrawn = false;

        Player[p].DrawPlayer(false, false);


        if (Player[p].BlinkCounter > 0.0f)			// noch farbig blinken vom PowerUp?
        {
            if (Player[p].BlinkCounter < 4.0f)
            {
                int a = int ((Player[p].BlinkCounter) * 63.0f);

                if (Player[p].BlinkColor == 1) Player[p].CurrentColor = D3DCOLOR_RGBA (255,  64,  64, a);
                else if (Player[p].BlinkColor == 2) Player[p].CurrentColor = D3DCOLOR_RGBA ( 64,  64, 255, a);
                else if (Player[p].BlinkColor == 3) Player[p].CurrentColor = D3DCOLOR_RGBA ( 64, 255,  64, a);
                else if (Player[p].BlinkColor == 4) Player[p].CurrentColor = D3DCOLOR_RGBA (128, 192, 255, a);

                Player[p].DrawPlayer(true, true);
                Player[p].DrawPlayer(true, true);
                Player[p].DrawPlayer(false, true);
            }

            Player[p].BlinkCounter -= 0.5f SYNC;
        }
        else if (Player[p].WasDamaged == true)
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
    if (WarningCount > 0.0f)
    {
        D3DCOLOR Col;
        int		 a;
        int		 off;

        a = int (WarningCount * 2.55f);
        Col = D3DCOLOR_RGBA (255, 255, 255, a);

        DirectGraphics.SetFilterMode (true);

        off = 100 - int (WarningCount);

        pGegnerGrafix [WARNING]->SetRect (0, 0, 180, 40);
        pGegnerGrafix [WARNING]->RenderSpriteScaled(float (230 - off * 4.5f / 4.0f),
                float (390 - off / 4.0f),
                int (180 + off * 4.5f / 2.0f),
                int (40  + off * 1.0f / 2.0f), Col);

        DirectGraphics.SetFilterMode (false);

        DirectGraphics.SetColorKeyMode();
    }

    // Blitz und andere Partikel rendern, die alles überlagern
    PartikelSystem.DoThunder ();

    if (Console.Showing == false)
    {
        // Waffen 1-3 auswählen
        if (KeyDown(DIK_1))
            Player[0].SelectedWeapon = 0;

        if (KeyDown(DIK_2) &&
                Player[0].CurrentWeaponLevel[1] > 0)
            Player[0].SelectedWeapon = 1;

        if (KeyDown(DIK_3) &&
                Player[0].CurrentWeaponLevel[2] > 0)
            Player[0].SelectedWeapon = 2;

        for (int p = 0; p < NUMPLAYERS; p++)
            if (Player[p].Handlung != TOT)
                Player[p].CheckForExplode();
    }

    // Game-Over Schrift anzeigen ?
    if (Player[0].GameOverTimer > 0.0f)
        ShowGameOver();

    // Gameloop verlassen ?
    if (KeyDown(DIK_ESCAPE) && Player[0].GameOverTimer == 0.0f)
        LeaveGameLoop();

#if defined(GCW)
    // On GCW Zero, same check as above, but using the internal controls
    if (DirectInput.InternalJoystickMainMenuButtonDown() && Player[0].GameOverTimer == 0.0f)
        LeaveGameLoop();
#endif //GCW

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

    if (DEMORecording == true) pDefaultFont->DrawText (10, 455, "Recording demo", D3DCOLOR_RGBA (255, 255, 255, 255));

} // GameLoop


// --------------------------------------------------------------------------------------
// Game Loop verlassen
// --------------------------------------------------------------------------------------

void LeaveGameLoop(void)
{
    // Movespeed wieder richtig setzen
    Timer.SetMoveSpeed(10.0f);

    // Message Box verstecken
    GUI.HideBoxFast();

    // Console verstecken
    Console.Hide();

    // Schrift im Menu neu einfaden
    pMenu->Rotation	   = 0.0f;
    pMenu->RotationDir = 1;

    // Ins Hauptmenu wechseln
    SpielZustand = MAINMENU;
    pMenu->AktuellerZustand = MENUZUSTAND_MAINMENU;
    if (Player[0].Lives == -1 && Player[1].Lives == -1)
        //DKS - If game is over, make main menu selection be "Start New Game"
        pMenu->AktuellerPunkt = MENUPUNKT_STARTGAME;
    else
        //DKS - If the game was not over yet, make the main menu selection be "Back To Game"
        pMenu->AktuellerPunkt = MENUPUNKT_CONTINUEGAME;

    // Musik pausieren
    //DKS - Now we use use the sound manager's already-present pause functions:
    //SoundManager.StopSong(MUSIC_STAGEMUSIC, true);
    //SoundManager.StopSong(MUSIC_FLUGSACK, true);
    //SoundManager.StopAllSongs(true);
    SoundManager.PauseSongs();
    SoundManager.PauseSounds();

    //DKS
    // Loop Sounds stoppen
    //SoundManager.StopAllLoopedSounds();

    // Menu Musik spielen
    SoundManager.PlaySong(MUSIC_MENU, false);

    // TODO
    // Alle Joysticks anhalten
    //DirectInput.StopForceFeedbackEffect(FFE_BLITZ);
    //GameRunning = false;
}

// --------------------------------------------------------------------------------------
// Screen wackeln lassen
//
// Den Screen entsprechend den aktuellen Shake-Werten drehen
// Hierbei wird einfach eine Rotationsmatrix als View Matrix gesetzt mit abnehmenden Werten
// sodass alles verdreht gerendert wird ... echt clever =)
// --------------------------------------------------------------------------------------

void SetScreenShake (void)
{
    D3DXMATRIX	matView, matRot, matTrans, matTrans2;	// Rotations und Translations Matrizen
    int			Winkel;									// Rotationswinkel

    float f = (ScreenWinkel + WackelValue);

    MYMATH_FTOL(f, Winkel);

    // Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
    //
    if (Winkel > 360) Winkel -= 360;
    if (Winkel < 0)	  Winkel += 360;
    D3DXMatrixRotationZ  (&matRot, DegreetoRad[Winkel]);

    D3DXMatrixTranslation(&matTrans, -320.0f,-240.0f, 0.0f);			// Transformation zum Ursprung
    D3DXMatrixTranslation(&matTrans2, 320.0f, 240.0f, 0.0f);			// Transformation wieder zurück

    D3DXMatrixIdentity	 (&matView);
    D3DXMatrixMultiply	 (&matView, &matView, &matTrans);		// Verschieben
    D3DXMatrixMultiply	 (&matView, &matView, &matRot);			// rotieren
    D3DXMatrixMultiply	 (&matView, &matView, &matTrans2);		// und wieder zurück verschieben

    // rotierte Matrix setzen
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTransform(D3DTS_VIEW, &matView);
#elif defined(PLATFORM_SDL)
    g_matView = matView;
#endif
}

// --------------------------------------------------------------------------------------
// ScreenWackel werte bearbeiten
// --------------------------------------------------------------------------------------

void ScreenWackeln(void)
{
    // Weiterwackeln
    //
    if (Console.Active == false)
    {
        WackelValue += WackelDir SYNC * WackelSpeed;

        if (WackelValue < -WackelMaximum ||			// An der aktuellen oberen  Grenze oder
                WackelValue >  WackelMaximum)			//				    unteren Grenze angekommen ?
        {
            WackelMaximum -=  0.5f;					// Dann Grenze verkleinern
            WackelDir	   = -WackelDir;			// Und Richtung umkehren
            WackelValue	   = WackelMaximum*(-WackelDir);// Und an der neuen Grenze anfangen
        }

        if (WackelMaximum <= 0.0f)					// Wackeln zuende ?
            WackelMaximum  = 0.0f;					// Dann aufhören damit
    }

    SetScreenShake();
}

// --------------------------------------------------------------------------------------
// Angeben wie stark der Screen wackeln soll
// --------------------------------------------------------------------------------------

void ShakeScreen (float staerke)
{
    // Werte für das Screenwackeln setzen, um den Screen leicht zu schütteln
    WackelMaximum =  staerke;
    WackelValue   =  0.0f;
    WackelDir	  =  1.0f;
    WackelSpeed   = float (rand()%10 + 5);
}

// --------------------------------------------------------------------------------------
// Default Config erstellen
// --------------------------------------------------------------------------------------
//DKS - New function that creates a per-player default controls configuration
void CreateDefaultControlsConfig(int player)
{
    if (player < 0 || player > 1)
        return;

    if (player == 0) {
        Player[0].AktionKeyboard [AKTION_LINKS]			= DIK_LEFT;
        Player[0].AktionKeyboard [AKTION_RECHTS]			= DIK_RIGHT;
        Player[0].AktionKeyboard [AKTION_DUCKEN]			= DIK_DOWN;
        Player[0].AktionKeyboard [AKTION_OBEN]			= DIK_UP;
        Player[0].AktionKeyboard [AKTION_UNTEN]			= DIK_DOWN;
#if defined(PANDORA)
        Player[0].AktionKeyboard [AKTION_JUMP]			= DIK_NEXT;
        Player[0].AktionKeyboard [AKTION_SHOOT]			= DIK_HOME;
        Player[0].AktionKeyboard [AKTION_BLITZ]			= DIK_PRIOR;
        Player[0].AktionKeyboard [AKTION_POWERLINE]		= DIK_LSHIFT;
        Player[0].AktionKeyboard [AKTION_GRANATE]		    = DIK_END;
        Player[0].AktionKeyboard [AKTION_SMARTBOMB]		= DIK_RCONTROL;
        Player[0].AktionKeyboard [AKTION_WAFFE_SPREAD]	= DIK_1;
        Player[0].AktionKeyboard [AKTION_WAFFE_LASER]		= DIK_2;
        Player[0].AktionKeyboard [AKTION_WAFFE_BOUNCE]	= DIK_3;
        Player[0].AktionKeyboard [AKTION_WAFFEN_CYCLE]	= DIK_RETURN;
#elif defined(ANDROID)
        Player[0].AktionKeyboard [AKTION_JUMP]			= DIK_A;
        Player[0].AktionKeyboard [AKTION_SHOOT]			= DIK_B;
        Player[0].AktionKeyboard [AKTION_BLITZ]			= DIK_C;
        Player[0].AktionKeyboard [AKTION_POWERLINE]		= DIK_D;
        Player[0].AktionKeyboard [AKTION_GRANATE]		    = DIK_E;
        Player[0].AktionKeyboard [AKTION_SMARTBOMB]		= DIK_F;
        Player[0].AktionKeyboard [AKTION_WAFFE_SPREAD]	= DIK_G;
        Player[0].AktionKeyboard [AKTION_WAFFE_LASER]		= DIK_H;
        Player[0].AktionKeyboard [AKTION_WAFFE_BOUNCE]	= DIK_I;
        Player[0].AktionKeyboard [AKTION_WAFFEN_CYCLE]	= DIK_J;
#else
        Player[0].AktionKeyboard [AKTION_JUMP]			= DIK_LALT;
        Player[0].AktionKeyboard [AKTION_SHOOT]			= DIK_LCONTROL;
        Player[0].AktionKeyboard [AKTION_BLITZ]			= DIK_LSHIFT;
        Player[0].AktionKeyboard [AKTION_POWERLINE]		= DIK_SPACE;
        Player[0].AktionKeyboard [AKTION_GRANATE]		    = DIK_RCONTROL;
        Player[0].AktionKeyboard [AKTION_SMARTBOMB]		= DIK_RSHIFT;
        Player[0].AktionKeyboard [AKTION_WAFFE_SPREAD]	= 0;
        Player[0].AktionKeyboard [AKTION_WAFFE_LASER]		= 0;
        Player[0].AktionKeyboard [AKTION_WAFFE_BOUNCE]	= 0;
        Player[0].AktionKeyboard [AKTION_WAFFEN_CYCLE]	= DIK_RETURN;

        Player[0].AktionJoystick [AKTION_LINKS]			= -1;
        Player[0].AktionJoystick [AKTION_RECHTS]			= -1;
        Player[0].AktionJoystick [AKTION_DUCKEN]			= -1;
        Player[0].AktionJoystick [AKTION_OBEN]			= -1;
        Player[0].AktionJoystick [AKTION_UNTEN]			= -1;
        Player[0].AktionJoystick [AKTION_JUMP]			= 0;
        Player[0].AktionJoystick [AKTION_SHOOT]			= 1;
        Player[0].AktionJoystick [AKTION_BLITZ]			= 2;
        Player[0].AktionJoystick [AKTION_POWERLINE]		= 3;
        Player[0].AktionJoystick [AKTION_GRANATE]		    = 4;
        Player[0].AktionJoystick [AKTION_SMARTBOMB]		= 5;
        Player[0].AktionJoystick [AKTION_WAFFE_SPREAD]	= -1;
        Player[0].AktionJoystick [AKTION_WAFFE_LASER]	    = -1;
        Player[0].AktionJoystick [AKTION_WAFFE_BOUNCE]	= -1;
        Player[0].AktionJoystick [AKTION_WAFFEN_CYCLE]	= 6;
#endif
        Player[0].Walk_UseAxxis = false;      // By default, use the HAT switch (DPAD) for movement..
        Player[0].Look_UseAxxis = false;      // and also looking.

        //DKS - Added missing default settings, and made player2's default joy index 1 instead of both being 0
        Player[0].JoystickIndex = 0;
        Player[0].JoystickSchwelle = 500.0f;
        Player[0].JoystickMode = JOYMODE_JOYPAD;
        Player[0].ControlType = CONTROLTYPE_KEYBOARD;

#if defined(GCW)
        // On GCW Zero, the Player 1 default joy index is the internal controls and both players use joystick:
        Player[0].JoystickIndex = DirectInput.GetInternalJoystickIndex();
        Player[0].ControlType = CONTROLTYPE_JOY;
        // Default button map for GCW Zero:
        Player[0].AktionJoystick [AKTION_JUMP]			= 3;
        Player[0].AktionJoystick [AKTION_SHOOT]			= 2;
        Player[0].AktionJoystick [AKTION_BLITZ]			= 1;
        Player[0].AktionJoystick [AKTION_POWERLINE]		= 6;
        Player[0].AktionJoystick [AKTION_GRANATE]		    = 7;
        Player[0].AktionJoystick [AKTION_SMARTBOMB]		= 4;
        Player[0].AktionJoystick [AKTION_WAFFE_SPREAD]	= -1;
        Player[0].AktionJoystick [AKTION_WAFFE_LASER]	    = -1;
        Player[0].AktionJoystick [AKTION_WAFFE_BOUNCE]	= -1;
        Player[0].AktionJoystick [AKTION_WAFFEN_CYCLE]	= 0;
#endif //GCW

    } else {
        Player[1].AktionKeyboard [AKTION_LINKS]			= DIK_A;
        Player[1].AktionKeyboard [AKTION_RECHTS]			= DIK_D;
        Player[1].AktionKeyboard [AKTION_DUCKEN]			= DIK_S;
        Player[1].AktionKeyboard [AKTION_OBEN]			= DIK_W;
        Player[1].AktionKeyboard [AKTION_UNTEN]			= DIK_X;
        Player[1].AktionKeyboard [AKTION_JUMP]			= DIK_G;
        Player[1].AktionKeyboard [AKTION_SHOOT]			= DIK_H;
        Player[1].AktionKeyboard [AKTION_BLITZ]			= DIK_J;
        Player[1].AktionKeyboard [AKTION_POWERLINE]		= DIK_T;
        Player[1].AktionKeyboard [AKTION_GRANATE]			= DIK_Z;
        Player[1].AktionKeyboard [AKTION_SMARTBOMB]		= DIK_U;
        Player[1].AktionKeyboard [AKTION_WAFFE_SPREAD]	= 0;
        Player[1].AktionKeyboard [AKTION_WAFFE_LASER]		= 0;
        Player[1].AktionKeyboard [AKTION_WAFFE_BOUNCE]	= 0;
        Player[1].AktionKeyboard [AKTION_WAFFEN_CYCLE]	= DIK_Q;

        Player[1].AktionJoystick [AKTION_LINKS]			= -1;
        Player[1].AktionJoystick [AKTION_RECHTS]		    = -1;
        Player[1].AktionJoystick [AKTION_DUCKEN]		    = -1;
        Player[1].AktionJoystick [AKTION_OBEN]			= -1;
        Player[1].AktionJoystick [AKTION_UNTEN]			= -1;
        Player[1].AktionJoystick [AKTION_JUMP]			= 0;
        Player[1].AktionJoystick [AKTION_SHOOT]			= 1;
        Player[1].AktionJoystick [AKTION_BLITZ]			= 2;
        Player[1].AktionJoystick [AKTION_POWERLINE]		= 3;
        Player[1].AktionJoystick [AKTION_GRANATE]		    = 4;
        Player[1].AktionJoystick [AKTION_SMARTBOMB]		= 5;
        Player[1].AktionJoystick [AKTION_WAFFE_SPREAD]	= -1;
        Player[1].AktionJoystick [AKTION_WAFFE_LASER]	    = -1;
        Player[1].AktionJoystick [AKTION_WAFFE_BOUNCE]	= -1;
        Player[1].AktionJoystick [AKTION_WAFFEN_CYCLE]	= 6;

        Player[1].Walk_UseAxxis = false;      // By default, use the HAT switch (DPAD) for movement
        Player[1].Look_UseAxxis = false;      // and the analog stick for looking    -DKS

        //DKS - Added missing default settings, and made player2's default joy index 1 instead of both being 0
        Player[1].JoystickIndex = 1;
        Player[1].JoystickSchwelle = 500.0f;
        Player[1].JoystickMode = JOYMODE_JOYPAD;
        Player[1].ControlType = CONTROLTYPE_KEYBOARD;

#if defined(GCW)
        // On GCW Zero, both players use joystick by default:
        Player[1].ControlType = CONTROLTYPE_JOY;
#endif //GCW
    }
}

void CreateDefaultConfig(void)
{
    Protokoll.WriteText( false, "Creating new configuration file\n");

    strcpy_s (ActualLanguage, strlen("english.lng") + 1, "english.lng");
    bool language_loaded = LoadLanguage (ActualLanguage);
    if (!language_loaded) {
        Protokoll.WriteText( false, "ERROR: Failed to find default language file.\n");
        return;
    }

    SoundManager.SetVolumes(50, 60);

    UseForceFeedback = false;
    CreateDefaultControlsConfig(0);     // Load default controls for Player 1
    CreateDefaultControlsConfig(1);     // Load default controls for Player 2

    options_Detail = DETAIL_MAXIMUM;

#if defined(GCW)
    // Max detail is too much for GCW Zero:
    options_Detail = DETAIL_HIGH;   
#endif //GCW

    SaveConfig ();
}

// --------------------------------------------------------------------------------------
// Konfiguration mit den Sound-Lautstärken laden
// Existiert diese Datei nicht, so werden die Lautstärken auf den
// Defaut Wert gesetzt
// --------------------------------------------------------------------------------------

bool LoadConfig(void)
{
    float Sound, Musik;

    FILE *Datei = NULL;

    //DKS - Full paths can now be longer:
    //char temp[100];
    //snprintf( temp, sizeof(temp), "%s/%s", g_save_ext, CONFIGFILE );
    char *temp = (char *)malloc(strlen(g_save_ext) + 1 + strlen(CONFIGFILE) + 1);
    sprintf_s( temp, "%s/%s", g_save_ext, CONFIGFILE );

    fopen_s(&Datei, temp, "rb");		// versuchen Datei zu öffnen
    free((void *)temp);

    if (Datei == NULL)
        return false;

    // Spracheinstellung laden
    fread(&ActualLanguage, sizeof(ActualLanguage), 1, Datei);
    //DKS - Made language loading default back to english if saved language not found:
    bool language_loaded = LoadLanguage (ActualLanguage);
    if (!language_loaded) {
        Protokoll.WriteText( false, "ERROR loading %s, reverting to default language file.\n", ActualLanguage );
        strcpy_s(ActualLanguage, "english.lng");
        LoadLanguage(ActualLanguage);
    }

    // Daten für Sound und Musik-Lautstärke auslesen
    fread(&Sound, sizeof(Sound), 1, Datei);
    fread(&Musik, sizeof(Musik), 1, Datei);
    SoundManager.SetVolumes(Sound, Musik);

    // Daten für Keyboard und Joystick auslesen
    fread(&Player[0].AktionKeyboard, sizeof(Player[0].AktionKeyboard), 1, Datei);
    fread(&Player[0].AktionJoystick, sizeof(Player[0].AktionJoystick), 1, Datei);
    fread(&Player[0].Walk_UseAxxis,  sizeof(Player[0].Walk_UseAxxis), 1, Datei);
    fread(&Player[0].Look_UseAxxis,  sizeof(Player[0].Look_UseAxxis), 1, Datei);

    fread(&Player[1].AktionKeyboard, sizeof(Player[1].AktionKeyboard), 1, Datei);
    fread(&Player[1].AktionJoystick, sizeof(Player[1].AktionJoystick), 1, Datei);
    fread(&Player[1].Walk_UseAxxis,  sizeof(Player[1].Walk_UseAxxis), 1, Datei);
    fread(&Player[1].Look_UseAxxis,  sizeof(Player[1].Look_UseAxxis), 1, Datei);

    fread(&UseForceFeedback, sizeof(UseForceFeedback), 1, Datei);

    // Sonstige Optionen laden
    fread(&options_Detail, sizeof(options_Detail), 1, Datei);

    fread(&Player[0].ControlType,		sizeof(Player[0].ControlType),	 1, Datei);
    fread(&Player[0].JoystickMode,    sizeof(Player[0].JoystickMode),	 1, Datei);
    fread(&Player[0].JoystickSchwelle,sizeof(Player[0].JoystickSchwelle),1, Datei);

    // Joystick nicht mehr da?
    if (DirectInput.Joysticks[Player[0].JoystickIndex].Active == false)
    {
#if defined(GCW)
        //GCW Zero player 1 defaults:
        Player[0].ControlType = CONTROLTYPE_JOY;
        Player[0].JoystickMode = JOYMODE_JOYPAD;
        Player[0].JoystickIndex = DirectInput.GetInternalJoystickIndex();
        Player[0].JoystickSchwelle = 500.0f;
#else
        Player[0].ControlType = CONTROLTYPE_KEYBOARD;
        Player[0].JoystickIndex = 0;
        Player[0].JoystickSchwelle = 500.0f;
#endif //GCW
    }

    if (DirectInput.Joysticks[Player[1].JoystickIndex].Active == false)
    {
#if defined(GCW)
        //GCW Zero player 2 defaults:
        Player[1].ControlType = CONTROLTYPE_JOY;
        Player[1].JoystickMode = JOYMODE_JOYPAD;
        Player[1].JoystickIndex = 1;
        Player[1].JoystickSchwelle = 500.0f;
#else
        Player[1].ControlType = CONTROLTYPE_KEYBOARD;
        Player[1].JoystickIndex = 1;      //DKS - Changed player 2's default joy index to 1
        Player[1].JoystickSchwelle = 500.0f;
#endif //GCW
    }

    fread(&Player[1].ControlType,		sizeof(Player[1].ControlType),	 1, Datei);
    fread(&Player[1].JoystickMode,	sizeof(Player[1].JoystickMode),	 1, Datei);
    fread(&Player[1].JoystickSchwelle,sizeof(Player[1].JoystickSchwelle),1, Datei);

    Protokoll.WriteText( false, "Config file loading successful !\n" );

    fclose(Datei);							// Und Datei wieder schliessen

    PartikelSystem.SetParticleCount();

    if (JoystickFound == false)
    {
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

void SaveConfig(void)
{
    float	Sound, Musik;

    FILE *Datei = NULL;

    //DKS - Full paths can now be longer:
    //char temp[100];
    //snprintf( temp, sizeof(temp), "%s/%s", g_save_ext, CONFIGFILE );
    char *temp = (char *)malloc(strlen(g_save_ext) + 1 + strlen(CONFIGFILE) + 1);
    sprintf_s( temp, "%s/%s", g_save_ext, CONFIGFILE );

    fopen_s(&Datei, temp, "wb");
    free((void *)temp);

    if (Datei == NULL)
    {
        Protokoll.WriteText( false, "Config file saving failed !\n" );
        return;
    }

    // Spracheinstellung speichern
    fwrite(&ActualLanguage, sizeof(ActualLanguage), 1, Datei);

    // Daten für Sound und Musik-Lautstärke schreiben
    Sound = float(SoundManager.g_sound_vol);
    Musik = float(SoundManager.g_music_vol);

    fwrite(&Sound, sizeof(Sound), 1, Datei);
    fwrite(&Musik, sizeof(Musik), 1, Datei);

    // Daten für Keyboard und Joystick schreiben
    fwrite(&Player[0].AktionKeyboard, sizeof(Player[0].AktionKeyboard), 1, Datei);
    fwrite(&Player[0].AktionJoystick, sizeof(Player[0].AktionJoystick), 1, Datei);
    fwrite(&Player[0].Walk_UseAxxis,  sizeof(Player[0].Walk_UseAxxis), 1, Datei);
    fwrite(&Player[0].Look_UseAxxis,  sizeof(Player[0].Look_UseAxxis), 1, Datei);

    fwrite(&Player[1].AktionKeyboard, sizeof(Player[1].AktionKeyboard), 1, Datei);
    fwrite(&Player[1].AktionJoystick, sizeof(Player[1].AktionJoystick), 1, Datei);
    fwrite(&Player[1].Walk_UseAxxis,  sizeof(Player[1].Walk_UseAxxis), 1, Datei);
    fwrite(&Player[1].Look_UseAxxis,  sizeof(Player[1].Look_UseAxxis), 1, Datei);

    fwrite(&UseForceFeedback, sizeof(UseForceFeedback), 1, Datei);

    // Sonstige Optionen sichern
    fwrite(&options_Detail, sizeof(options_Detail), 1, Datei);

    fwrite(&Player[0].ControlType,		sizeof(Player[0].ControlType),	 1, Datei);
    fwrite(&Player[0].JoystickMode,		sizeof(Player[0].JoystickMode),	 1, Datei);
    fwrite(&Player[0].JoystickSchwelle,	sizeof(Player[0].JoystickSchwelle),1, Datei);

    fwrite(&Player[1].ControlType,		sizeof(Player[1].ControlType),	 1, Datei);
    fwrite(&Player[1].JoystickMode,		sizeof(Player[1].JoystickMode),	 1, Datei);
    fwrite(&Player[1].JoystickSchwelle,	sizeof(Player[1].JoystickSchwelle),1, Datei);

    fclose(Datei);							// Und Datei wieder schliessen
}

// --------------------------------------------------------------------------------------
// Beim Loading Screen anzeigen, was alles geladen wurde, dabei
// mehrere Zeilen anzeigen und immer bei neuem Text eins nach oben rutschen
// --------------------------------------------------------------------------------------

bool DisplayLoadInfo(const char Text[100])
{
    if (NochKeinFullScreen == true ||
            pMenu == NULL)
        return false;
    // TODO FIX
    /*
    	strrev (Text);				// String umdrehen
    	strnset(Text, ' ', 2);		// Ersten zwei (vorher letzten Zwei = \n) Buchstaben löschen
    	strrev (Text);				// Wieder richtig herum drehen
    	*/

    // Anzeigen im Loadingscreen
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->EndScene();

    lpD3DDevice->BeginScene();
    lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,	D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
#elif defined(PLATFORM_SDL)
    DirectGraphics.ClearBackBuffer();
#endif

    pMenu->ShowMenuBack();

    DirectGraphics.SetAdditiveMode();

    pMenuFont->DrawTextCenterAlign(320,
                                   200,
                                   TextArray[TEXT_MENUE_LOADING], 0xFFFFFFFF);

    // Anzahl anzeigen
#ifdef _DEBUG
    //DKS - Disabled this, it was causing crashes when _DEBUG was enabled (perhaps font was
    //      not loaded at this point)
#if 0
    char buf[5];

    sprintf_s(buf, "%d", LoadingItemsLoaded);
    pDefaultFont->DrawText((700 - pDefaultFont->StringLength(TextArray[TEXT_MENUE_LOADING])) / 2.0f, 220,
                           buf, 0xFFFFFFFF);
#endif //0
#endif

    // Hint anzeigen
    //DKS - Added support for displaying hints on low-resolution devices:
    if (DisplayHintNr > -1) {
        if (CommandLineParams.LowRes) {
            const char *text = TextArray[TEXT_HINT1 + DisplayHintNr];
            float y_pos = 270.0f;
            float y_inc = 28.0f;
            int max_width = RENDERWIDTH-20;
            if (pDefaultFont->StringLength(text, 0) > max_width) {
                // Split the line in two if too long to display on low-res device:
                char text1[255];
                char text2[255];
                SplitLine(text1, text2, (char *)text);
                pDefaultFont->DrawTextCenterAlign(320.0f, y_pos, text1, 0xFFFFFFFF, 0);
                pDefaultFont->DrawTextCenterAlign(320.0f, y_pos+y_inc, text2, 0xFFFFFFFF, 0);
            } else {
                pDefaultFont->DrawTextCenterAlign(320.0f, y_pos, text, 0xFFFFFFFF, 0);
            }
        } else {
            pDefaultFont->DrawTextCenterAlign(320.0f, 270.0f,TextArray[TEXT_HINT1 + DisplayHintNr], 0xFFFFFFFF, 0);
        }
    }

    pMenu->LoadingScreen.RenderSprite((640 - 360) / 2, (480 - 60) / 2 + 5, 0x88FFFFFF);

    pMenu->LoadingBar.SetRect (0, 0, int (pMenu->LoadingProgress), 19);
    pMenu->LoadingBar.RenderSprite((640 - 318) / 2, (480 - 19) / 2 + 5, 0x88FFFFFF);

    /*for (i=0; i<24; i++)
    	pDefaultFont->DrawText(10, float(230+i*10), LoadInfoText[i], D3DCOLOR_RGBA(0, 255, 0, i*10));*/

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->EndScene();
#endif
    DirectGraphics.ShowBackBuffer();
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->BeginScene();
#endif

    pMenu->LoadingItemsLoaded++;
    pMenu->LoadingProgress += 318.0f / pMenu->LoadingItemsToLoad;
    if (pMenu->LoadingProgress > 318.0f)
        pMenu->LoadingProgress = 318.0f;

    Timer.wait (1);
    Timer.update();

    return true;
}

// --------------------------------------------------------------------------------------
// Spieler explodieren lassen
// --------------------------------------------------------------------------------------

void ExplodePlayer(void)
{
//	static float delay = 0.0f;

    // alte Darstellung beenden
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->EndScene();
#endif
}

// --------------------------------------------------------------------------------------
// Stage Clear Musik dudelt und Spieler läuft aus dem Screen raus
// --------------------------------------------------------------------------------------

void StageClear(bool PlaySong)
{
    for (int p = 0; p < NUMPLAYERS; p++)
    {
        Player[p].StageClearRunning = true;
        Player[p].CanBeDamaged = false;
        Player[p].PunisherActive = false;
    }

    GUI.HideBoxFast();

    //DKS - Stop all songs instead of just individual ones..
    //SoundManager.StopSong(MUSIC_STAGEMUSIC, false);

    //// Punisher Musik stoppen
    //if (MUSIC_IsPlaying(SoundManager.its_Songs[MUSIC_PUNISHER]->SongData)) {
    //    SoundManager.StopSong(MUSIC_PUNISHER, false);
    //}

    SoundManager.StopSongs(); //DKS - Added, see above

    if (PlaySong)
        SoundManager.PlaySong(MUSIC_STAGECLEAR, false);

}

// --------------------------------------------------------------------------------------
// Summary Screen anzeigen
// --------------------------------------------------------------------------------------

//DKS - Fixed display on low-res scaled-font devices, and made display on
//      normal-resolution devices be centered and spaced properly.
void SummaryScreen(void)
{
    bool leave      = false;
    bool all_controls_unpressed_yet = false;
    bool reveal_cheat = (RunningTutorial == false) && (Player[0].DiamondsThisLevel == TileEngine.MaxDiamonds);
    
    //DKS - Added counter to prevent accidental early-exit:
    const float delay_can_leave = 400.0f;
    const float delay_inc = 30.0f;
    float delay_ctr = 0.0f;

    ShowSummary = true;

    //DKS - Note: boxes dimensions must be multiple of TILESIZE (20) due to quirks in GUISystem.cpp
    int box_x, box_y, box_w, box_h;

    // Normal screen dimensions:
    box_w = 360;
    if (reveal_cheat) {
        box_h = 140;
    } else {
        box_h = 120;
    }

    box_x = RENDERWIDTH/2 - box_w/2;
    box_y = RENDERHEIGHT/2 - box_h/2;
    
    int title_txt_y = box_y - 10;        // Note that borders of boxes mean drawable region is a bit higher than specified
    int pressanykey_txt_y = box_y + box_h - 8;   
    const int sprite_spacing = 96;
    int sprites_y = box_y + 40;
    int sprite1_x = (RENDERWIDTH/2) - sprite_spacing - sprite_spacing/2;
    int sprite2_x = (RENDERWIDTH/2) - sprite_spacing/2;
    int sprite3_x = (RENDERWIDTH/2) + sprite_spacing/2;
    int secrets_x = (RENDERWIDTH/2) + sprite_spacing + sprite_spacing/2;
    int stats_txt_y = sprites_y + 30;
    int cheat_txt_y = (stats_txt_y + pressanykey_txt_y) / 2;

    // Summary Box erzeugen
    // DKS - NOTE: boxes are drawn less-than intuitively, you must use dimensions multiples of TILESIZE
    GUI.ShowBox(box_x, box_y, box_w, box_h);

    while (leave == false)
    {
        // alte Darstellung beenden
#if defined(PLATFORM_DIRECTX)
        lpD3DDevice->EndScene();
#endif

        // Mit dem Darstellen beginnen
#if defined(PLATFORM_DIRECTX)
        lpD3DDevice->BeginScene();
#endif

        TileEngine.DrawBackground	();				// Level abhandeln
        TileEngine.DrawBackLevel	();
        TileEngine.DrawFrontLevel	();

        Gegner.RunAll();
        Gegner.RenderAll();
        Projectiles.DoProjectiles	();				// Schüsse abhandeln

        // Overlay Tiles des Levels zeigen und Spieler und Objekte verdecken
        TileEngine.DrawOverlayLevel();
        PartikelSystem.DoPartikel();				// Partikel abhandeln

        TileEngine.DrawWater();
        TileEngine.DrawBackLevelOverlay();
        TileEngine.DrawOverlayLevel();
        TileEngine.DrawShadow();

        HUD.DoHUD();								// HUD anhandeln

        // Blitz und andere Partikel rendern, die alles überlagern
        PartikelSystem.DoThunder ();

        // Summary Screen rendern
        GUI.Run();
        D3DCOLOR color = D3DCOLOR_RGBA(0, 255, 0, (int)(GUI.m_FadingAlpha));
        pDefaultFont->DrawText((float)(RENDERWIDTH-pDefaultFont->StringLength(TextArray[TEXT_SUMMARY_TITLE]))/2,
                title_txt_y, TextArray[TEXT_SUMMARY_TITLE], color);

        if (delay_ctr >= delay_can_leave) {
            std::string str_pressanykey(TextArray[TEXT_SUMMARY_PRESSFIRE]);

            // If player 1 is controlled with joystick, replace all references to 'key' with 'button'
            if (Player[0].ControlType == CONTROLTYPE_JOY) {
                ReplaceAll(str_pressanykey, "key", "button");
            }

            pDefaultFont->DrawText((float)(RENDERWIDTH-pDefaultFont->StringLength(str_pressanykey.c_str()))/2,
                    pressanykey_txt_y, str_pressanykey.c_str(), color);
        }

        pGegnerGrafix[POWERBLOCK]->RenderSpriteScaled(sprite1_x-16, sprites_y-16, 32, 32, 1, color);
        pGegnerGrafix[DIAMANT]->RenderSprite(sprite2_x-14, sprites_y-14, 0, color, false);
        pGegnerGrafix  [ONEUP]->RenderSpriteScaled(sprite3_x-16, sprites_y-16, 32, 32, 0, color);
        pDefaultFont->DrawText(float(secrets_x - pDefaultFont->StringLength(TextArray[TEXT_SUMMARY_SECRETS])/2), 
                                float(sprites_y-pDefaultFont->GetYCharSize()/2),
                                TextArray[TEXT_SUMMARY_SECRETS], color);

        char buf[100];
        sprintf_s(buf, "%i/%i", Player[0].BlocksThisLevel, TileEngine.MaxBlocks);
        pDefaultFont->DrawText((float)(sprite1_x - pDefaultFont->StringLength(buf) / 2), stats_txt_y, buf, color);

        sprintf_s(buf, "%i/%i", Player[0].DiamondsThisLevel, TileEngine.MaxDiamonds);
        pDefaultFont->DrawText((float)(sprite2_x - pDefaultFont->StringLength(buf) / 2), stats_txt_y, buf, color);

        sprintf_s(buf, "%i/%i", Player[0].LivesThisLevel, TileEngine.MaxOneUps);
        pDefaultFont->DrawText((float)(sprite3_x - pDefaultFont->StringLength(buf) / 2), stats_txt_y, buf, color);

        sprintf_s(buf, "%i/%i", Player[0].SecretThisLevel, TileEngine.MaxSecrets);
        pDefaultFont->DrawText((float)(secrets_x - pDefaultFont->StringLength(buf) / 2), stats_txt_y, buf, color);

        // Cheat freigespielt? -> Wenn alle Diamanten gefunden
        if (reveal_cheat)
        {
            char buf2[50];
            strcpy_s(buf2, Cheats[Stage - 1]);
            for (unsigned int p = 0; p < strlen(buf2); p++)
                buf2[p] ^= 64;

            sprintf_s(buf, "%s: %s", TextArray[TEXT_SUMMARY_CHEATUNLOCK], buf2);
            pDefaultFont->DrawText(RENDERWIDTH/2-pDefaultFont->StringLength(buf, 0)/2, 
                                    cheat_txt_y, buf, color, 0);
        }

        // Darstellung beenden
#if defined(PLATFORM_DIRECTX)
        lpD3DDevice->EndScene();
#endif

        // Backbuffer mit Frontbuffer tauschen
        DirectGraphics.ShowBackBuffer();

        Timer.update();
        Timer.wait();
        SpeedFaktor = Timer.SpeedFaktor;

        DirectInput.UpdateTastatur();
        DirectInput.UpdateJoysticks();
        bool keypressed = DirectInput.AnyKeyDown();
        bool buttonpressed = DirectInput.AnyButtonDown();

        if (!keypressed && !buttonpressed) {
            all_controls_unpressed_yet = true;
        }

        if (all_controls_unpressed_yet && delay_ctr >= delay_can_leave) {
            if (buttonpressed || keypressed)
                leave = true;
        }

        // Musik zuende ?
        //DKS - Theere's no need to stop the song, as it isn't looped
        //if (MUSIC_IsFinished(SoundManager.its_Songs[MUSIC_STAGECLEAR]->SongData))
        //    SoundManager.StopSong(MUSIC_STAGECLEAR, false);

        delay_ctr += delay_inc SYNC;
        if (delay_ctr > delay_can_leave)
            delay_ctr = delay_can_leave;
    }

//#if defined(PLATFORM_DIRECTX) /* SDL_mixer does not work this kind of check, as far as i can tell */
//    // Musik noch nicht zu Ende ? Dann nochmal warten
//    while (!MUSIC_IsFinished(SoundManager.its_Songs[MUSIC_STAGECLEAR]->SongData))
//        ;
//#elif defined(PLATFORM_SDL)
//    //DKS - I revamped this summary screen code quite a bit so that no delay should ever be necessary
//    //    SDL_Delay( 3000 );
//#endif
    while (SoundManager.SongIsPlaying(MUSIC_STAGECLEAR))
        ;

    //DKS - There's no need to stop it, it will stop on its own as it's not looped:
    //SoundManager.StopSong(MUSIC_STAGECLEAR, false);

    GUI.HideBoxFast();

    ShowSummary = false;
}


// --------------------------------------------------------------------------------------
// Demo Aufnahme starten
// --------------------------------------------------------------------------------------

bool NewDemo (const char Filename[])
{
    //DKS - Full paths can now be longer:
    //char temp[100];
    //snprintf( temp, sizeof(temp), "%s/%s", g_save_ext, Filename );
    char *temp = (char *)malloc(strlen(g_save_ext) + 1 + strlen(Filename) + 1);
    sprintf_s( temp, "%s/%s", g_save_ext, Filename );

    fopen_s(&DEMOFile, temp, "wb");
    free((void *)temp);

    if(!DEMOFile)
    {
        Protokoll.WriteText( true, "\n-> Error opening Demo File !\n" );
        return false;
    }

    // DateiHeader schreiben
    //
    fwrite (&DEMO_ID, sizeof(DEMO_ID), 1, DEMOFile);

    // Stage Nr schreiben
    fwrite (&Stage, sizeof (Stage), 1, DEMOFile);

    DEMORecording = true;
    DEMOPress	  = 0;

    // Tasten auf false setzen
    for (int i=0; i<MAX_AKTIONEN; i++)
        Player[0].Aktion[i] = false;

    // Level neu initialisieren und dann gehts los
    int l = Stage;

    srand (5);

    InitNewGame  ();
    Stage	  = l;
    NewStage = l;
    InitNewGameLevel (Stage);

    // Timer auf 60 fps für Demo setzen
    Timer.SetMaxFPS(40);

    return true;

} // NewDemo

// --------------------------------------------------------------------------------------
// Demo Laden
// --------------------------------------------------------------------------------------

bool LoadDemo (const char Filename[])
{
    char Kennung[20];

    TileEngine.XOffset = 0;
    TileEngine.YOffset = 0;

    //DKS - Fixed bug in handling size of full demo path: sizeof(100) returns something very different than 100
    //      (Thank you to Alexander Troosh for the bug report.)
    //DKS - Full paths can also now be longer:
    //// File öffnen
    //char temp[100];
    //snprintf( temp, sizeof(100), "%s/%s", g_save_ext, Filename );

    char *temp = (char *)malloc(strlen(g_save_ext) + 1 + strlen(Filename) + 1);
    sprintf_s(temp, "%s/%s", g_save_ext, Filename );

    fopen_s(&DEMOFile, temp, "rb");
    free((void *)temp);

    if(!DEMOFile)
        return false;

    // DateiHeader lesen
    //
    fread (&Kennung, sizeof(DEMO_ID), 1, DEMOFile);

    if (strcmp (DEMO_ID, Kennung) != 0)
        return false;

    // Stage Nr laden
    fread (&Stage, sizeof (Stage), 1, DEMOFile);
    NewStage = Stage;

    DEMOPlaying = true;
    DEMOPress   = 0;

    // Tasten auf false setzen
    for (int i=0; i<MAX_AKTIONEN; i++)
        Player[0].Aktion[i] = false;

    // Level neu initialisieren und dann gehts los
    int l = NewStage;

    srand (5);

    InitNewGame  ();
    Stage    = l;
    NewStage = l;
    InitNewGameLevel (Stage);

    // Timer auf 60 fps für Demo setzen
    Timer.SetMaxFPS(40);

    return true;

} // LoadDemo

// --------------------------------------------------------------------------------------
// Demo beenden
// --------------------------------------------------------------------------------------

void EndDemo (void)
{
    // File schliessen
    fclose (DEMOFile);

    DEMOPlaying   = false;
    DEMORecording = false;

    Timer.SetMaxFPS(0);

    //DKS - added this to ensure RNG is always properly seeded
    srand(timeGetTime());

} // EndDemo

// --------------------------------------------------------------------------------------
// Demo aufnehmen
// --------------------------------------------------------------------------------------

void RecordDemo (void)
{
    /*srand (DEMOPress++);
    if (DEMOPress > 32000)
    	DEMOPress = 0;*/

    // Tasten speichern
    //
    for (int i=0; i<MAX_AKTIONEN; i++)
        fwrite (&Player[0].Aktion[i], sizeof (Player[0].Aktion[i]), 1, DEMOFile);

    // FPS speichern
    //
    int fps;

    fps = int (Timer.getFrameRate());
    fwrite (&fps, sizeof (fps), 1, DEMOFile);

} // RecordDemo

// --------------------------------------------------------------------------------------
// Demo abspielen
// --------------------------------------------------------------------------------------

void PlayDemo (void)
{
    /*srand (DEMOPress++);
    if (DEMOPress > 32000)
    	DEMOPress = 0;*/

    // Tasten laden
    //
    for (int i=0; i<MAX_AKTIONEN; i++)
        fread (&Player[0].Aktion[i], sizeof (Player[0].Aktion[i]), 1, DEMOFile);

    // FPS laden
    //
    int fps;

    fread (&fps, sizeof (fps), 1, DEMOFile);
    Timer.SetMaxFPS (fps);

    // Demo Ende ?
    if (feof (DEMOFile))
    {
        EndDemo  ();
        LoadDemo ("Demo.dem");
    }
} // PlayDemo

// --------------------------------------------------------------------------------------
// Nach dem Boss den Screen wieder auf den Spieler zentrieren und Musik einfaden etc.
// --------------------------------------------------------------------------------------

void ScrolltoPlayeAfterBoss(void)
{
    // Level wieder zum Spieler scrollen und dann weiterscrollen lassen
    TileEngine.Zustand = ZUSTAND_SCROLLBAR;
    TileEngine.MustCenterPlayer = true;

    // Level Musik wieder einfaden lassen (aus Pause Zustand)
    SoundManager.FadeSong(MUSIC_STAGEMUSIC, 2.0f, 100, true);
}

// --------------------------------------------------------------------------------------
// Piss-Text anzeigen =)
// --------------------------------------------------------------------------------------

void ShowPissText(void)
{
    if (Player[0].BronsonCounter > 220.0f)
    {
        int TextNr = int ((Player[0].BronsonCounter - 220.0f) / 50.0f);

        //DKS - Trying to get the infamous "piss text" centered on the player..
        //      I had gotten the boxes centered in the tutorial, only to find
        //      them off-centered here. I had to offset by TILESIZE*2 to get
        //      them centered properly horizontally, which I found frustrating.
        //      Oh well..
//        GUI.ShowBox(TextArray[TEXT_PISS_1 + TextNr],
//                      (int)(Player[0].ypos - 70 - TileEngine.YOffset),
//                      (int)(Player[0].xpos - TileEngine.XOffset) - 10);
        GUI.ShowBox(TextArray[TEXT_PISS_1 + TextNr],
                      (int)(Player[0].ypos - 70 - TileEngine.YOffset),
                      (int)(Player[0].xpos - TileEngine.XOffset + TILESIZE*2));

        if (Player[0].BronsonCounter > 220.0f + 50.0f * 18)
            Player[0].BronsonCounter = 270.0f;
    }
}

// --------------------------------------------------------------------------------------
// Zufällig ein Ziel aussuchen
// --------------------------------------------------------------------------------------

PlayerClass* ChooseAim(void)
{
    PlayerClass* pAim;

    pAim = &Player[rand()%NUMPLAYERS];

    if (pAim == &Player[0] &&
            Player[0].Handlung == TOT)
        pAim = &Player[1];

    if (pAim == &Player[1] &&
            Player[1].Handlung == TOT)
        pAim = &Player[0];

    return pAim;
}


const char *convertText(const char *text)
{
    // Text konvertieren
    static char temp[MAX_CHARS];

    unsigned int i = 0;
    for (i = 0; i < strlen(text); i++)
        temp[i] = text[i] ^ 64;
    temp[i] = 0;

    return temp;
}
