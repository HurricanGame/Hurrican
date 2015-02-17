// Datei : Gameplay.cpp

// --------------------------------------------------------------------------------------
//
// Beinhaltet den Haupt Game-Loop
//
// (c) 2002 J�rg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include <stdio.h>
#include "CCutScene.h"
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
float	ScreenWinkel   = 0.0f;								// in welchem zWinkel steht der Screen grad (f�r s�ulen, die das Level zum Kippen bringen)
float	WarningCount   = 0.0f;								// Counter, ob ein "Warning" angezeigt wird
bool	JoystickFound;
bool	UseForceFeedback;									// ForceFeedback benutzen?
bool	ShowSummary = false;

long    DEMOPress		= 0;								// Counter bis zum n�chsten Tastendruck
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
        pPlayer[p]->Handlung = STEHEN;
        pPlayer[p]->InitPlayer();
        pPlayer[p]->InitNewLevel();
        pPlayer[p]->SecretFullGame   = 0;
        pPlayer[p]->DiamondsFullGame = 0;
        pPlayer[p]->LivesFullGame	  = 0;
        pPlayer[p]->BlocksFullGame	  = 0;

        // nur im Tutorial Level
        if (RunningTutorial == true)
            pPlayer[p]->GodMode = true;
    }

    pHUD->BossHUDActive = 0.0f;
}

// --------------------------------------------------------------------------------------
// Ein neues Level initialisieren und Level "Stage'Nr'.map" laden
// --------------------------------------------------------------------------------------

void InitNewGameLevel(int Nr)
{
    char Name[100];

    pSoundManager->StopSong(MUSIC_STAGEMUSIC, false);

    LoadingItemsLoaded	= 0;
    LoadingItemsToLoad	= 75.0f;
    LoadingProgress		= 0.0f;
    WarningCount		= 0.0f;
    ScreenWinkel		= 0.0f;

    for (int p = 0; p < NUMPLAYERS; p++)
    {
        pPlayer[p]->ExplodingTimer	= 0.0f;
        pPlayer[p]->InitNewLevel();
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
    if (pTileEngine->LoadLevel(Name) == false)
    {
        SpielZustand = MAINMENU;
        pMenu->AktuellerZustand = MENUPUNKT_STARTGAME;
        Stage = -1;
        NewStage = -1;
        return;
    }

    // Songs mit Namen aus dem Levelfile Laden
    pSoundManager->LoadSong(pTileEngine->DateiAppendix.Songs[0], MUSIC_STAGEMUSIC);
    pSoundManager->LoadSong(pTileEngine->DateiAppendix.Songs[1], MUSIC_BOSS);

    pSoundManager->SetAllSongVolumes();

    LoadingProgress = 320.0f;
    DisplayLoadInfo ("");
    pTimer->wait  (1000);
    pTimer->update();

    // Menu Musik ausfaden
    //
    //pSoundManager->FadeSong (MUSIC_LOADING, -10.0f, 0, false);
    if (true == pSoundManager->InitSuccessfull &&
            MUSIC_IsPlaying(pSoundManager->its_Songs[MUSIC_MENU]->SongData))
    {
        pSoundManager->FadeSong (MUSIC_MENU, -10.0f, 0, false);

        while (pSoundManager->its_Songs[MUSIC_MENU]->FadingVolume != 0.0f)
        {
            // Timer updaten
            pTimer->update();
            pTimer->wait();
            SpeedFaktor = pTimer->SpeedFaktor;

            pSoundManager->Update();
        }
    }

    // Level-Musik abspielen
    if (StopStageMusicAtStart == false)
        pSoundManager->PlaySong(MUSIC_STAGEMUSIC, false);

    FahrstuhlPos = -1.0f;

    SpielZustand = GAMELOOP;
}

// --------------------------------------------------------------------------------------
// Game Over Schriftzug anzeigen
// --------------------------------------------------------------------------------------

void ShowGameOver(void)
{
    // Game Over Musik anhalten wenn sie beendet wurde
    if (MUSIC_IsFinished(pSoundManager->its_Songs[MUSIC_GAMEOVER]->SongData))
        pSoundManager->StopSong(MUSIC_GAMEOVER, false);

    int col;

    // Transparent Wert des Game Over Schriftzuges bestimmen
    col = int ((50.0f - pPlayer[0]->GameOverTimer)*10);
    if (col > 255)							// Obergrenze checken
        col = 255;

    pTileEngine->GameOver.RenderSprite((640 - 400) * 0.5f, (480 - 90) * 0.5f, D3DCOLOR_RGBA(255, 255, 255, col));

    pPlayer[0]->GameOverTimer -= 0.75f SYNC;

    // GameOver vorbei ?
    // Dann sch�n alle l�schen und auf ein neues Spiel vorbereiten
    if (pPlayer[0]->GameOverTimer <= 0.0f)
    {
        pPlayer[0]->GameOverTimer = 0.0f;
        pMenu->CheckForNewHighscore();

        pConsole->Hide();
        Stage = -1;
    }
}

// --------------------------------------------------------------------------------------
// Haupt-Spielablauf
// --------------------------------------------------------------------------------------

void GameLoop(void)
{
    pHUD->bShowArrow = false;

    // Total l�schen
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                       D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
#elif defined(PLATFORM_SDL)
    DirectGraphics.ClearBackBuffer();
#endif

    pTileEngine->NewXOffset = -1;
    pTileEngine->NewYOffset = -1;

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
            pPlayer[p]->WasDamaged = false;

            if (pConsole->Showing == false &&
                    pPlayer[p]->Handlung != TOT)
            {
                if (pPlayer[p]->GameOverTimer == 0.0f)
                {
                    // Spieler-Eingabe abfragen
                    pPlayer[p]->GetPlayerInput();
                    pPlayer[p]->AnimatePlayer();
                    pPlayer[p]->MovePlayer();
                }
            }

            pPlayer[p]->GegnerDran = false;

            // Spieler bewegen
            if (p == 0 &&
                    pConsole->Showing == false)
                pPlayer[p]->ScrollFlugsack();
        }

        // Spieler auf einer Plattform ?
        for (int p = 0; p < NUMPLAYERS; p++)
            if (pPlayer[p]->AufPlattform != NULL)
                pPlayer[p]->DoPlattformStuff();

        // Gegner bewegen
        if (pConsole->Showing == false)
            pGegner->RunAll();

        // Level checken
        if (pConsole->Showing == false)
            pTileEngine->UpdateLevel();

        pTileEngine->CheckBounds();
    }
    SpeedFaktor = SpeedFaktorMax;   // Restore the factor so other logic can stay in sync

    if (SpielZustand != GAMELOOP)
        return;

    // Hintergrund und Parallax Layer anzeigen
    DirectGraphics.SetColorKeyMode();
    pTileEngine->CalcRenderRange();
    pTileEngine->DrawBackground();

    // Drache zb
    pTileEngine->DrawSpecialLayer();

    // Evtl. rotieren, wenn Screen wackelt
    if (WackelMaximum > 0.0f)
        ScreenWackeln();

    // Level anzeigen
    pTileEngine->DrawBackLevel();
    pTileEngine->DrawFrontLevel();

    // LighMaps l�schen
    if (options_Detail >= DETAIL_HIGH)
        pTileEngine->ClearLightMaps();

    // Gegner anzeigen
    pGegner->RenderAll();

    // Spieler anzeigen
    for (int p = 0; p < NUMPLAYERS; p++)
    {
        pPlayer[p]->AlreadyDrawn = false;

        pPlayer[p]->DrawPlayer(false, false);


        if (pPlayer[p]->BlinkCounter > 0.0f)			// noch farbig blinken vom PowerUp?
        {
            if (pPlayer[p]->BlinkCounter < 4.0f)
            {
                int a = int ((pPlayer[p]->BlinkCounter) * 63.0f);

                if (pPlayer[p]->BlinkColor == 1) pPlayer[p]->CurrentColor = D3DCOLOR_RGBA (255,  64,  64, a);
                else if (pPlayer[p]->BlinkColor == 2) pPlayer[p]->CurrentColor = D3DCOLOR_RGBA ( 64,  64, 255, a);
                else if (pPlayer[p]->BlinkColor == 3) pPlayer[p]->CurrentColor = D3DCOLOR_RGBA ( 64, 255,  64, a);
                else if (pPlayer[p]->BlinkColor == 4) pPlayer[p]->CurrentColor = D3DCOLOR_RGBA (128, 192, 255, a);

                pPlayer[p]->DrawPlayer(true, true);
                pPlayer[p]->DrawPlayer(true, true);
                pPlayer[p]->DrawPlayer(false, true);
            }

            pPlayer[p]->BlinkCounter -= 0.5f SYNC;
        }
        else if (pPlayer[p]->WasDamaged == true)
            pPlayer[p]->DrawPlayer(true, false);
    }

    // Sch�sse abhandeln
    pProjectiles->DoProjectiles();

    // Partikel abhandeln
    pPartikelSystem->DoPartikel();

    // Overlay Tiles des Levels zeigen und Spieler und Objekte verdecken
    DirectGraphics.SetColorKeyMode();

    pTileEngine->DrawWater();
    pTileEngine->DrawBackLevelOverlay();
    pTileEngine->DrawOverlayLevel();
    pTileEngine->DrawShadow();

    // HUD anhandeln
    pHUD->DoHUD();

    // ggf. BossHUD anzeigen
    pHUD->RenderBossHUD();

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

    // Blitz und andere Partikel rendern, die alles �berlagern
    pPartikelSystem->DoThunder ();

    if (pConsole->Showing == false)
    {
        // Waffen 1-3 ausw�hlen
        if (KeyDown(DIK_1))
            pPlayer[0]->SelectedWeapon = 0;

        if (KeyDown(DIK_2) &&
                pPlayer[0]->CurrentWeaponLevel[1] > 0)
            pPlayer[0]->SelectedWeapon = 1;

        if (KeyDown(DIK_3) &&
                pPlayer[0]->CurrentWeaponLevel[2] > 0)
            pPlayer[0]->SelectedWeapon = 2;

        for (int p = 0; p < NUMPLAYERS; p++)
            if (pPlayer[p]->Handlung != TOT)
                pPlayer[p]->CheckForExplode();
    }

    // Game-Over Schrift anzeigen ?
    if (pPlayer[0]->GameOverTimer > 0.0f)
        ShowGameOver();

    // Gameloop verlassen ?
    if (KeyDown(DIK_ESCAPE) && pPlayer[0]->GameOverTimer == 0.0f)
        LeaveGameLoop();

    /*
    	if (KeyDown(DIK_F1)) pPlayer[0]->CurrentWeaponLevel[pPlayer[0]->SelectedWeapon] = 1;
    	if (KeyDown(DIK_F2)) pPlayer[0]->CurrentWeaponLevel[pPlayer[0]->SelectedWeapon] = 2;
    	if (KeyDown(DIK_F3)) pPlayer[0]->CurrentWeaponLevel[pPlayer[0]->SelectedWeapon] = 3;
    	if (KeyDown(DIK_F4)) pPlayer[0]->CurrentWeaponLevel[pPlayer[0]->SelectedWeapon] = 4;
    	if (KeyDown(DIK_F5)) pPlayer[0]->CurrentWeaponLevel[pPlayer[0]->SelectedWeapon] = 5;
    	if (KeyDown(DIK_F6)) pPlayer[0]->CurrentWeaponLevel[pPlayer[0]->SelectedWeapon] = 6;
    	if (KeyDown(DIK_F7)) pPlayer[0]->CurrentWeaponLevel[pPlayer[0]->SelectedWeapon] = 7;
    	if (KeyDown(DIK_F8)) pPlayer[0]->CurrentWeaponLevel[pPlayer[0]->SelectedWeapon] = 8;
    */

    if (DEMORecording == true) pDefaultFont->DrawText (10, 455, "Recording demo", D3DCOLOR_RGBA (255, 255, 255, 255));

} // GameLoop


// --------------------------------------------------------------------------------------
// Game Loop verlassen
// --------------------------------------------------------------------------------------

void LeaveGameLoop(void)
{
    // Movespeed wieder richtig setzen
    pTimer->SetMoveSpeed(10.0f);

    // Message Box verstecken
    pGUI->HideBoxFast();

    // Console verstecken
    pConsole->Hide();

    // Schrift im Menu neu einfaden
    pMenu->Rotation	   = 0.0f;
    pMenu->RotationDir = 1;

    // Ins Hauptmenu wechseln
    SpielZustand = MAINMENU;
    pMenu->AktuellerZustand = MENUZUSTAND_MAINMENU;
    pMenu->AktuellerPunkt	= 0;

    // Musik pausieren
    pSoundManager->StopSong(MUSIC_STAGEMUSIC, true);
    pSoundManager->StopSong(MUSIC_FLUGSACK, true);
    pSoundManager->StopAllSongs(true);
    pSoundManager->StopAllSounds();

    // Loop Sounds stoppen
    pSoundManager->StopAllLoopedSounds();

    // Menu Musik spielen
    pSoundManager->PlaySong(MUSIC_MENU, false);

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

    // Winkel angleichen, damit er immer zwischen 0� und 360� bleibt
    //
    if (Winkel > 360) Winkel -= 360;
    if (Winkel < 0)	  Winkel += 360;
    D3DXMatrixRotationZ  (&matRot, DegreetoRad[Winkel]);

    D3DXMatrixTranslation(&matTrans, -320.0f,-240.0f, 0.0f);			// Transformation zum Ursprung
    D3DXMatrixTranslation(&matTrans2, 320.0f, 240.0f, 0.0f);			// Transformation wieder zur�ck

    D3DXMatrixIdentity	 (&matView);
    D3DXMatrixMultiply	 (&matView, &matView, &matTrans);		// Verschieben
    D3DXMatrixMultiply	 (&matView, &matView, &matRot);			// rotieren
    D3DXMatrixMultiply	 (&matView, &matView, &matTrans2);		// und wieder zur�ck verschieben

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
    if (pConsole->Active == false)
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
            WackelMaximum  = 0.0f;					// Dann aufh�ren damit
    }

    SetScreenShake();
}

// --------------------------------------------------------------------------------------
// Angeben wie stark der Screen wackeln soll
// --------------------------------------------------------------------------------------

void ShakeScreen (float staerke)
{
    // Werte f�r das Screenwackeln setzen, um den Screen leicht zu sch�tteln
    WackelMaximum =  staerke;
    WackelValue   =  0.0f;
    WackelDir	  =  1.0f;
    WackelSpeed   = float (rand()%10 + 5);
}

// --------------------------------------------------------------------------------------
// Default Config erstellen
// --------------------------------------------------------------------------------------

void CreateDefaultConfig(void)
{
    Protokoll.WriteText( false, "Creating new configuration file\n");

    strcpy_s (ActualLanguage, strlen("english.lng") + 1, "english.lng");
    bool language_loaded = LoadLanguage (ActualLanguage);
    if (!language_loaded) {
        Protokoll.WriteText( false, "ERROR: Failed to find default language file.\n");
        return;
    }

    pSoundManager->SetVolumes(50, 60);

    pPlayer[0]->AktionKeyboard [AKTION_LINKS]			= DIK_LEFT;
    pPlayer[0]->AktionKeyboard [AKTION_RECHTS]			= DIK_RIGHT;
    pPlayer[0]->AktionKeyboard [AKTION_DUCKEN]			= DIK_DOWN;
    pPlayer[0]->AktionKeyboard [AKTION_OBEN]			= DIK_UP;
    pPlayer[0]->AktionKeyboard [AKTION_UNTEN]			= DIK_DOWN;
#if defined(PANDORA)
    pPlayer[0]->AktionKeyboard [AKTION_JUMP]			= DIK_NEXT;
    pPlayer[0]->AktionKeyboard [AKTION_SHOOT]			= DIK_HOME;
    pPlayer[0]->AktionKeyboard [AKTION_BLITZ]			= DIK_PRIOR;
    pPlayer[0]->AktionKeyboard [AKTION_POWERLINE]		= DIK_LSHIFT;
    pPlayer[0]->AktionKeyboard [AKTION_GRANATE]		    = DIK_END;
    pPlayer[0]->AktionKeyboard [AKTION_SMARTBOMB]		= DIK_RCONTROL;
    pPlayer[0]->AktionKeyboard [AKTION_WAFFE_SPREAD]	= DIK_1;
    pPlayer[0]->AktionKeyboard [AKTION_WAFFE_LASER]		= DIK_2;
    pPlayer[0]->AktionKeyboard [AKTION_WAFFE_BOUNCE]	= DIK_3;
    pPlayer[0]->AktionKeyboard [AKTION_WAFFEN_CYCLE]	= DIK_RETURN;
#elif defined(ANDROID)
    pPlayer[0]->AktionKeyboard [AKTION_JUMP]			= DIK_A;
    pPlayer[0]->AktionKeyboard [AKTION_SHOOT]			= DIK_B;
    pPlayer[0]->AktionKeyboard [AKTION_BLITZ]			= DIK_C;
    pPlayer[0]->AktionKeyboard [AKTION_POWERLINE]		= DIK_D;
    pPlayer[0]->AktionKeyboard [AKTION_GRANATE]		    = DIK_E;
    pPlayer[0]->AktionKeyboard [AKTION_SMARTBOMB]		= DIK_F;
    pPlayer[0]->AktionKeyboard [AKTION_WAFFE_SPREAD]	= DIK_G;
    pPlayer[0]->AktionKeyboard [AKTION_WAFFE_LASER]		= DIK_H;
    pPlayer[0]->AktionKeyboard [AKTION_WAFFE_BOUNCE]	= DIK_I;
    pPlayer[0]->AktionKeyboard [AKTION_WAFFEN_CYCLE]	= DIK_J;
#else
    pPlayer[0]->AktionKeyboard [AKTION_JUMP]			= DIK_LALT;
    pPlayer[0]->AktionKeyboard [AKTION_SHOOT]			= DIK_LCONTROL;
    pPlayer[0]->AktionKeyboard [AKTION_BLITZ]			= DIK_LSHIFT;
    pPlayer[0]->AktionKeyboard [AKTION_POWERLINE]		= DIK_SPACE;
    pPlayer[0]->AktionKeyboard [AKTION_GRANATE]		    = DIK_RCONTROL;
    pPlayer[0]->AktionKeyboard [AKTION_SMARTBOMB]		= DIK_RSHIFT;
    pPlayer[0]->AktionKeyboard [AKTION_WAFFE_SPREAD]	= 0;
    pPlayer[0]->AktionKeyboard [AKTION_WAFFE_LASER]		= 0;
    pPlayer[0]->AktionKeyboard [AKTION_WAFFE_BOUNCE]	= 0;
    pPlayer[0]->AktionKeyboard [AKTION_WAFFEN_CYCLE]	= DIK_RETURN;
#endif
    pPlayer[1]->AktionKeyboard [AKTION_LINKS]			= DIK_A;
    pPlayer[1]->AktionKeyboard [AKTION_RECHTS]			= DIK_D;
    pPlayer[1]->AktionKeyboard [AKTION_DUCKEN]			= DIK_S;
    pPlayer[1]->AktionKeyboard [AKTION_OBEN]			= DIK_W;
    pPlayer[1]->AktionKeyboard [AKTION_UNTEN]			= DIK_X;
    pPlayer[1]->AktionKeyboard [AKTION_JUMP]			= DIK_G;
    pPlayer[1]->AktionKeyboard [AKTION_SHOOT]			= DIK_H;
    pPlayer[1]->AktionKeyboard [AKTION_BLITZ]			= DIK_J;
    pPlayer[1]->AktionKeyboard [AKTION_POWERLINE]		= DIK_T;
    pPlayer[1]->AktionKeyboard [AKTION_GRANATE]			= DIK_Z;
    pPlayer[1]->AktionKeyboard [AKTION_SMARTBOMB]		= DIK_U;
    pPlayer[1]->AktionKeyboard [AKTION_WAFFE_SPREAD]	= 0;
    pPlayer[1]->AktionKeyboard [AKTION_WAFFE_LASER]		= 0;
    pPlayer[1]->AktionKeyboard [AKTION_WAFFE_BOUNCE]	= 0;
    pPlayer[1]->AktionKeyboard [AKTION_WAFFEN_CYCLE]	= DIK_Q;

    pPlayer[0]->AktionJoystick [AKTION_LINKS]			= -1;
    pPlayer[0]->AktionJoystick [AKTION_RECHTS]			= -1;
    pPlayer[0]->AktionJoystick [AKTION_DUCKEN]			= -1;
    pPlayer[0]->AktionJoystick [AKTION_OBEN]			= -1;
    pPlayer[0]->AktionJoystick [AKTION_UNTEN]			= -1;
    pPlayer[0]->AktionJoystick [AKTION_JUMP]			= 0;
    pPlayer[0]->AktionJoystick [AKTION_SHOOT]			= 1;
    pPlayer[0]->AktionJoystick [AKTION_BLITZ]			= 2;
    pPlayer[0]->AktionJoystick [AKTION_POWERLINE]		= 3;
    pPlayer[0]->AktionJoystick [AKTION_GRANATE]		= 4;
    pPlayer[0]->AktionJoystick [AKTION_SMARTBOMB]		= 5;
    pPlayer[0]->AktionJoystick [AKTION_WAFFE_SPREAD]	= -1;
    pPlayer[0]->AktionJoystick [AKTION_WAFFE_LASER]	= -1;
    pPlayer[0]->AktionJoystick [AKTION_WAFFE_BOUNCE]	= -1;
    pPlayer[0]->AktionJoystick [AKTION_WAFFEN_CYCLE]	= -1;

    pPlayer[1]->AktionJoystick [AKTION_LINKS]			= -1;
    pPlayer[1]->AktionJoystick [AKTION_RECHTS]		= -1;
    pPlayer[1]->AktionJoystick [AKTION_DUCKEN]		= -1;
    pPlayer[1]->AktionJoystick [AKTION_OBEN]			= -1;
    pPlayer[1]->AktionJoystick [AKTION_UNTEN]			= -1;
    pPlayer[1]->AktionJoystick [AKTION_JUMP]			= 0;
    pPlayer[1]->AktionJoystick [AKTION_SHOOT]			= 1;
    pPlayer[1]->AktionJoystick [AKTION_BLITZ]			= 2;
    pPlayer[1]->AktionJoystick [AKTION_POWERLINE]		= 3;
    pPlayer[1]->AktionJoystick [AKTION_GRANATE]		= 4;
    pPlayer[1]->AktionJoystick [AKTION_SMARTBOMB]		= 5;
    pPlayer[1]->AktionJoystick [AKTION_WAFFE_SPREAD]	= -1;
    pPlayer[1]->AktionJoystick [AKTION_WAFFE_LASER]	= -1;
    pPlayer[1]->AktionJoystick [AKTION_WAFFE_BOUNCE]	= -1;
    pPlayer[1]->AktionJoystick [AKTION_WAFFEN_CYCLE]	= -1;

    // TODO ?
//	if (CanForceFeedback == true)
    //UseForceFeedback = true;

    pPlayer[0]->Walk_UseAxxis = true;
    pPlayer[0]->Look_UseAxxis = true;

    pPlayer[1]->Walk_UseAxxis = true;
    pPlayer[1]->Look_UseAxxis = true;

    options_Detail = DETAIL_MAXIMUM;

    SaveConfig ();
}

// --------------------------------------------------------------------------------------
// Konfiguration mit den Sound-Lautst�rken laden
// Existiert diese Datei nicht, so werden die Lautst�rken auf den
// Defaut Wert gesetzt
// --------------------------------------------------------------------------------------

bool LoadConfig(void)
{
    float Sound, Musik;

    FILE *Datei = NULL;
    char temp[100];

    snprintf( temp, sizeof(temp), "%s/%s", g_storage_ext, CONFIGFILE );
    fopen_s(&Datei, temp, "rb");		// versuchen Datei zu �ffnen

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

    // Daten f�r Sound und Musik-Lautst�rke auslesen
    fread(&Sound, sizeof(Sound), 1, Datei);
    fread(&Musik, sizeof(Musik), 1, Datei);
    pSoundManager->SetVolumes(Sound, Musik);

    // Daten f�r Keyboard und Joystick auslesen
    fread(&pPlayer[0]->AktionKeyboard, sizeof(pPlayer[0]->AktionKeyboard), 1, Datei);
    fread(&pPlayer[0]->AktionJoystick, sizeof(pPlayer[0]->AktionJoystick), 1, Datei);
    fread(&pPlayer[0]->Walk_UseAxxis,  sizeof(pPlayer[0]->Walk_UseAxxis), 1, Datei);
    fread(&pPlayer[0]->Look_UseAxxis,  sizeof(pPlayer[0]->Look_UseAxxis), 1, Datei);

    fread(&pPlayer[1]->AktionKeyboard, sizeof(pPlayer[1]->AktionKeyboard), 1, Datei);
    fread(&pPlayer[1]->AktionJoystick, sizeof(pPlayer[1]->AktionJoystick), 1, Datei);
    fread(&pPlayer[1]->Walk_UseAxxis,  sizeof(pPlayer[1]->Walk_UseAxxis), 1, Datei);
    fread(&pPlayer[1]->Look_UseAxxis,  sizeof(pPlayer[1]->Look_UseAxxis), 1, Datei);

    fread(&UseForceFeedback, sizeof(UseForceFeedback), 1, Datei);

    // Sonstige Optionen laden
    fread(&options_Detail, sizeof(options_Detail), 1, Datei);

    fread(&pPlayer[0]->ControlType,		sizeof(pPlayer[0]->ControlType),	 1, Datei);
    fread(&pPlayer[0]->JoystickMode,    sizeof(pPlayer[0]->JoystickMode),	 1, Datei);
    fread(&pPlayer[0]->JoystickSchwelle,sizeof(pPlayer[0]->JoystickSchwelle),1, Datei);

    // Joystick nicht mehr da?
    if (DirectInput.Joysticks[pPlayer[0]->JoystickIndex].Active == false)
    {
        pPlayer[0]->ControlType = JOYMODE_KEYBOARD;
        pPlayer[0]->JoystickIndex = 0;
        pPlayer[0]->JoystickSchwelle = 500.0f;
    }

    if (DirectInput.Joysticks[pPlayer[1]->JoystickIndex].Active == false)
    {
        pPlayer[1]->ControlType = JOYMODE_KEYBOARD;
        pPlayer[1]->JoystickIndex = 0;
        pPlayer[1]->JoystickSchwelle = 500.0f;
    }

    pPlayer[1]->JoystickSchwelle = 500.0f;
    fread(&pPlayer[1]->ControlType,		sizeof(pPlayer[1]->ControlType),	 1, Datei);
    fread(&pPlayer[1]->JoystickMode,	sizeof(pPlayer[1]->JoystickMode),	 1, Datei);
    fread(&pPlayer[1]->JoystickSchwelle,sizeof(pPlayer[1]->JoystickSchwelle),1, Datei);

    Protokoll.WriteText( false, "Config file loading successful !\n" );

    fclose(Datei);							// Und Datei wieder schliessen

    if (pPartikelSystem)
        pPartikelSystem->SetParticleCount();

    if (JoystickFound == false)
    {
        pPlayer[0]->ControlType = JOYMODE_KEYBOARD;
        pPlayer[1]->ControlType = JOYMODE_KEYBOARD;
    }

    if (pPlayer[0]->JoystickSchwelle < 1.0f)
        pPlayer[0]->JoystickSchwelle = 500.0f;

    if (pPlayer[1]->JoystickSchwelle < 1.0f)
        pPlayer[1]->JoystickSchwelle = 500.0f;

    return true;
}

// --------------------------------------------------------------------------------------
// Aktuelle Konfiguration mit den Sound-Lautst�rken speichern
// --------------------------------------------------------------------------------------

void SaveConfig(void)
{
    float	Sound, Musik;

    FILE *Datei = NULL;
    char temp[100];

    snprintf( temp, sizeof(temp), "%s/%s", g_storage_ext, CONFIGFILE );
    fopen_s(&Datei, temp, "wb");
    if (Datei == NULL)
    {
        Protokoll.WriteText( false, "Config file saving failed !\n" );
        return;
    }

    // Spracheinstellung speichern
    fwrite(&ActualLanguage, sizeof(ActualLanguage), 1, Datei);

    // Daten f�r Sound und Musik-Lautst�rke schreiben
    Sound = float(pSoundManager->its_GlobalSoundVolume);
    Musik = float(pSoundManager->its_GlobalMusicVolume);

    fwrite(&Sound, sizeof(Sound), 1, Datei);
    fwrite(&Musik, sizeof(Musik), 1, Datei);

    // Daten f�r Keyboard und Joystick schreiben
    fwrite(&pPlayer[0]->AktionKeyboard, sizeof(pPlayer[0]->AktionKeyboard), 1, Datei);
    fwrite(&pPlayer[0]->AktionJoystick, sizeof(pPlayer[0]->AktionJoystick), 1, Datei);
    fwrite(&pPlayer[0]->Walk_UseAxxis,  sizeof(pPlayer[0]->Walk_UseAxxis), 1, Datei);
    fwrite(&pPlayer[0]->Look_UseAxxis,  sizeof(pPlayer[0]->Look_UseAxxis), 1, Datei);

    fwrite(&pPlayer[1]->AktionKeyboard, sizeof(pPlayer[1]->AktionKeyboard), 1, Datei);
    fwrite(&pPlayer[1]->AktionJoystick, sizeof(pPlayer[1]->AktionJoystick), 1, Datei);
    fwrite(&pPlayer[1]->Walk_UseAxxis,  sizeof(pPlayer[1]->Walk_UseAxxis), 1, Datei);
    fwrite(&pPlayer[1]->Look_UseAxxis,  sizeof(pPlayer[1]->Look_UseAxxis), 1, Datei);

    fwrite(&UseForceFeedback, sizeof(UseForceFeedback), 1, Datei);

    // Sonstige Optionen sichern
    fwrite(&options_Detail, sizeof(options_Detail), 1, Datei);

    fwrite(&pPlayer[0]->ControlType,		sizeof(pPlayer[0]->ControlType),	 1, Datei);
    fwrite(&pPlayer[0]->JoystickMode,		sizeof(pPlayer[0]->JoystickMode),	 1, Datei);
    fwrite(&pPlayer[0]->JoystickSchwelle,	sizeof(pPlayer[0]->JoystickSchwelle),1, Datei);

    fwrite(&pPlayer[1]->ControlType,		sizeof(pPlayer[1]->ControlType),	 1, Datei);
    fwrite(&pPlayer[1]->JoystickMode,		sizeof(pPlayer[1]->JoystickMode),	 1, Datei);
    fwrite(&pPlayer[1]->JoystickSchwelle,	sizeof(pPlayer[1]->JoystickSchwelle),1, Datei);

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
    	strnset(Text, ' ', 2);		// Ersten zwei (vorher letzten Zwei = \n) Buchstaben l�schen
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
    char buf[5];

    sprintf_s(buf, "%d", LoadingItemsLoaded);
    pDefaultFont->DrawText((700 - pDefaultFont->StringLength(TextArray[TEXT_MENUE_LOADING])) / 2.0f, 220,
                           buf, 0xFFFFFFFF);

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

    LoadingScreen.RenderSprite((640 - 360) / 2, (480 - 60) / 2 + 5, 0x88FFFFFF);

    LoadingBar.SetRect (0, 0, int (LoadingProgress), 19);
    LoadingBar.RenderSprite((640 - 318) / 2, (480 - 19) / 2 + 5, 0x88FFFFFF);

    /*for (i=0; i<24; i++)
    	pDefaultFont->DrawText(10, float(230+i*10), LoadInfoText[i], D3DCOLOR_RGBA(0, 255, 0, i*10));*/

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->EndScene();
#endif
    DirectGraphics.ShowBackBuffer();
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->BeginScene();
#endif

    LoadingItemsLoaded++;
    LoadingProgress += 318.0f / LoadingItemsToLoad;
    if (LoadingProgress > 318.0f)
        LoadingProgress = 318.0f;

    pTimer->wait (1);
    pTimer->update();

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
// Stage Clear Musik dudelt und Spieler l�uft aus dem Screen raus
// --------------------------------------------------------------------------------------

void StageClear(bool PlaySong)
{
    for (int p = 0; p < NUMPLAYERS; p++)
    {
        pPlayer[p]->StageClearRunning = true;
        pPlayer[p]->CanBeDamaged = false;
        pPlayer[p]->PunisherActive = false;
    }

    pGUI->HideBoxFast();

    pSoundManager->StopSong(MUSIC_STAGEMUSIC, false);

    if (PlaySong)
        pSoundManager->PlaySong(MUSIC_STAGECLEAR, false);

    // Punisher Musik stoppen
    if (MUSIC_IsPlaying(pSoundManager->its_Songs[MUSIC_PUNISHER]->SongData))
        pSoundManager->StopSong(MUSIC_PUNISHER, false);

}

// --------------------------------------------------------------------------------------
// Summary Screen anzeigen
// --------------------------------------------------------------------------------------

//DKS - Fixed display on low-res scaled-font devices, and made display on
//      normal-resolution devices be centered and spaced properly.
void SummaryScreen(void)
{
    bool leave      = false;
    bool keypressed = false;
    bool reveal_cheat = (RunningTutorial == false) && (pPlayer[0]->DiamondsThisLevel == pTileEngine->MaxDiamonds);

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
    pGUI->ShowBox(box_x, box_y, box_w, box_h);

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

        pTileEngine->DrawBackground	();				// Level abhandeln
        pTileEngine->DrawBackLevel	();
        pTileEngine->DrawFrontLevel	();

        pGegner->RunAll();
        pGegner->RenderAll();
        pProjectiles->DoProjectiles	();				// Sch�sse abhandeln

        // Overlay Tiles des Levels zeigen und Spieler und Objekte verdecken
        pTileEngine->DrawOverlayLevel();
        pPartikelSystem->DoPartikel();				// Partikel abhandeln

        pTileEngine->DrawWater();
        pTileEngine->DrawBackLevelOverlay();
        pTileEngine->DrawOverlayLevel();
        pTileEngine->DrawShadow();

        pHUD->DoHUD();								// HUD anhandeln

        // Blitz und andere Partikel rendern, die alles �berlagern
        pPartikelSystem->DoThunder ();

        // Summary Screen rendern
        pGUI->Run();
        D3DCOLOR color = D3DCOLOR_RGBA(0, 255, 0, (int)(pGUI->m_FadingAlpha));
        pDefaultFont->DrawText((float)(RENDERWIDTH-pDefaultFont->StringLength(TextArray[TEXT_SUMMARY_TITLE]))/2,
                title_txt_y, TextArray[TEXT_SUMMARY_TITLE], color);

        std::string str_pressanykey(TextArray[TEXT_SUMMARY_PRESSFIRE]);

        // If player 1 is controlled with joystick, replace all references to 'key' with 'button'
        if (pPlayer[0]->ControlType == JOYMODE_STICK) {
            ReplaceAll(str_pressanykey, "key", "button");
        }

        pDefaultFont->DrawText((float)(RENDERWIDTH-pDefaultFont->StringLength(str_pressanykey.c_str()))/2,
                pressanykey_txt_y, str_pressanykey.c_str(), color);

        pGegnerGrafix[POWERBLOCK]->RenderSpriteScaled(sprite1_x-16, sprites_y-16, 32, 32, 1, color);
        pGegnerGrafix[DIAMANT]->RenderSprite(sprite2_x-14, sprites_y-14, 0, color, false);
        pGegnerGrafix  [ONEUP]->RenderSpriteScaled(sprite3_x-16, sprites_y-16, 32, 32, 0, color);
        pDefaultFont->DrawText(float(secrets_x - pDefaultFont->StringLength(TextArray[TEXT_SUMMARY_SECRETS])/2), 
                                float(sprites_y-pDefaultFont->GetYCharSize()/2),
                                TextArray[TEXT_SUMMARY_SECRETS], color);

        char buf[100];
        sprintf_s(buf, "%i/%i", pPlayer[0]->BlocksThisLevel, pTileEngine->MaxBlocks);
        pDefaultFont->DrawText((float)(sprite1_x - pDefaultFont->StringLength(buf) / 2), stats_txt_y, buf, color);

        sprintf_s(buf, "%i/%i", pPlayer[0]->DiamondsThisLevel, pTileEngine->MaxDiamonds);
        pDefaultFont->DrawText((float)(sprite2_x - pDefaultFont->StringLength(buf) / 2), stats_txt_y, buf, color);

        sprintf_s(buf, "%i/%i", pPlayer[0]->LivesThisLevel, pTileEngine->MaxOneUps);
        pDefaultFont->DrawText((float)(sprite3_x - pDefaultFont->StringLength(buf) / 2), stats_txt_y, buf, color);

        sprintf_s(buf, "%i/%i", pPlayer[0]->SecretThisLevel, pTileEngine->MaxSecrets);
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

        pTimer->update();
        pTimer->wait();
        SpeedFaktor = pTimer->SpeedFaktor;

        DirectInput.UpdateTastatur();

        for (int i = 0; i < 255; i++)
            if (TastaturPuffer[i] != 0)
                keypressed = true;

        if (keypressed == true &&
                DirectInput.AreAllKeysReleased())
            leave = true;

        // Musik zuende ?
        if (MUSIC_IsFinished(pSoundManager->its_Songs[MUSIC_STAGECLEAR]->SongData))
            pSoundManager->StopSong(MUSIC_STAGECLEAR, false);
    }

#if defined(PLATFORM_DIRECTX) /* SDL_mixer does not work this kind of check, as far as i can tell */
    // Musik noch nicht zu Ende ? Dann nochmal warten
    while (!MUSIC_IsFinished(pSoundManager->its_Songs[MUSIC_STAGECLEAR]->SongData))
        ;
#elif defined(PLATFORM_SDL)
    SDL_Delay( 3000 );
#endif

    pSoundManager->StopSong(MUSIC_STAGECLEAR, false);

    pGUI->HideBoxFast();

    ShowSummary = false;
}


// --------------------------------------------------------------------------------------
// Demo Aufnahme starten
// --------------------------------------------------------------------------------------

bool NewDemo (const char Filename[])
{
    char temp[100];

    snprintf( temp, sizeof(temp), "%s/%s", g_storage_ext, Filename );
    fopen_s(&DEMOFile, temp, "wb");

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
        pPlayer[0]->Aktion[i] = false;

    // Level neu initialisieren und dann gehts los
    int l = Stage;

    srand (5);

    InitNewGame  ();
    Stage	  = l;
    NewStage = l;
    InitNewGameLevel (Stage);

    // Timer auf 60 fps f�r Demo setzen
    pTimer->SetMaxFPS(40);

    return true;

} // NewDemo

// --------------------------------------------------------------------------------------
// Demo Laden
// --------------------------------------------------------------------------------------

bool LoadDemo (const char Filename[])
{
    char Kennung[20];

    pTileEngine->XOffset = 0;
    pTileEngine->YOffset = 0;

    // File �ffnen
    char temp[100];

    snprintf( temp, sizeof(100), "%s/%s", g_storage_ext, Filename );
    fopen_s(&DEMOFile, temp, "rb");

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
        pPlayer[0]->Aktion[i] = false;

    // Level neu initialisieren und dann gehts los
    int l = NewStage;

    srand (5);

    InitNewGame  ();
    Stage    = l;
    NewStage = l;
    InitNewGameLevel (Stage);

    // Timer auf 60 fps f�r Demo setzen
    pTimer->SetMaxFPS(40);

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

    pTimer->SetMaxFPS(0);

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
        fwrite (&pPlayer[0]->Aktion[i], sizeof (pPlayer[0]->Aktion[i]), 1, DEMOFile);

    // FPS speichern
    //
    int fps;

    fps = int (pTimer->getFrameRate());
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
        fread (&pPlayer[0]->Aktion[i], sizeof (pPlayer[0]->Aktion[i]), 1, DEMOFile);

    // FPS laden
    //
    int fps;

    fread (&fps, sizeof (fps), 1, DEMOFile);
    pTimer->SetMaxFPS (fps);

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
    pTileEngine->Zustand = ZUSTAND_SCROLLBAR;
    pTileEngine->MustCenterPlayer = true;

    // Level Musik wieder einfaden lassen (aus Pause Zustand)
    pSoundManager->FadeSong(MUSIC_STAGEMUSIC, 2.0f, 100, true);
}

// --------------------------------------------------------------------------------------
// Piss-Text anzeigen =)
// --------------------------------------------------------------------------------------

void ShowPissText(void)
{
    if (pPlayer[0]->BronsonCounter > 220.0f)
    {
        int TextNr = int ((pPlayer[0]->BronsonCounter - 220.0f) / 50.0f);

        pGUI->ShowBox(TextArray[TEXT_PISS_1 + TextNr],
                      (int)(pPlayer[0]->ypos - 70 - pTileEngine->YOffset),
                      (int)(pPlayer[0]->xpos - pTileEngine->XOffset) - 10);

        if (pPlayer[0]->BronsonCounter > 220.0f + 50.0f * 18)
            pPlayer[0]->BronsonCounter = 270.0f;
    }
}

// --------------------------------------------------------------------------------------
// Zuf�llig ein Ziel aussuchen
// --------------------------------------------------------------------------------------

PlayerClass* ChooseAim(void)
{
    PlayerClass* pAim;

    pAim = pPlayer[rand()%NUMPLAYERS];

    if (pAim == pPlayer[0] &&
            pPlayer[0]->Handlung == TOT)
        pAim = pPlayer[1];

    if (pAim == pPlayer[1] &&
            pPlayer[1]->Handlung == TOT)
        pAim = pPlayer[0];

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
