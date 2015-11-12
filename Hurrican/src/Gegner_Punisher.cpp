// --------------------------------------------------------------------------------------
// Der Punisher
//
// Erscheint, wenn die Zeit abgelaufen ist.
// Der Punisher verfolgt den Spieler st�ndig, daf�r recht langsam.
// Er ist nahezu unbesiegbar, und bei Ber�hrung verliert der Spieler ein Leben und alle
// Waffen-Powerups, erh�lt daf�r aber wieder volle Zeit
// Besiegt der Spieler den Punisher, so erh�lt er daf�r ein Extraleben, voll Zeit
// und massenhaft Extras (muss ich mir noch ausdenken, welche)
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Punisher.h"
#include "HUD.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPunisher::GegnerPunisher(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_INIT;
    Energy			= 20000;

    ChangeLight		= Light;
    Destroyable		= false;
    Value1			= Wert1;
    Value2			= Wert2;
    OwnDraw			= true;
    TestBlock		= false;
    Active			= true;
    alpha			= 0.0f;
    ShotDelay		= 0.0f;

    AnimPhase		= 15;
    AnimCount		= 15.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerPunisher::DoDraw(void)
{
    // Je nach Handlung entsprechend anders rendern
    //
    switch (Handlung)
    {
    // in Schwabbelstreifen rendern
    case GEGNER_INIT2:
    case GEGNER_SPECIAL:
    {
        for (int i = 0; i < 170; i++)
        {
            pGegnerGrafix[GegnerArt]->SetRect(3 * 170, 2 * 170 + i, 4 * 170, 2 * 170 + i+1);
            pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-TileEngine.XOffset + (float)(sin((alpha / 20.0f) + i / 10.0f) * ((255.0f - alpha) / 255.0f * 200.0f))),
                                                   (float)(yPos-TileEngine.YOffset + i),
                                                   D3DCOLOR_RGBA (255, 255, 255, (int)alpha));
        }
    }
    break;

    // normal rendern
    case GEGNER_LAUFEN:
    {
        pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-TileEngine.XOffset),
                                               (float)(yPos-TileEngine.YOffset),
                                               AnimPhase, D3DCOLOR_RGBA (255, 255, 255, 255), false);
    }
    break;
    }
}

// --------------------------------------------------------------------------------------
// Spieler verfolgen
// --------------------------------------------------------------------------------------

void GegnerPunisher::FollowPlayer(void)
{
    if (xPos + 50 < pAim->xpos + 35) xAcc =  2.0f;
    if (xPos + 50 > pAim->xpos + 35) xAcc = -2.0f;

    if (yPos + 100 < pAim->ypos + 40) yAcc =  2.0f;
    if (yPos + 100 > pAim->ypos + 40) yAcc = -2.0f;

    if (xSpeed >  15.0f) xSpeed =  15.0f;
    if (xSpeed < -15.0f) xSpeed = -15.0f;
    if (ySpeed >  15.0f) ySpeed =  15.0f;
    if (ySpeed < -15.0f) ySpeed = -15.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerPunisher::DoKI(void)
{
    switch (Handlung)
    {
    // initialisieren
    case GEGNER_INIT:
    {
        // zentrieren
        xPos = (float)(TileEngine.XOffset + 320 - 100/2.0f);
        yPos = (float)(TileEngine.YOffset + 240 - 95/2.0f);

        alpha = 0.0f;
        Handlung = GEGNER_INIT2;

        SoundManager.StopSong(MUSIC_STAGEMUSIC, true);

        //DKS - Punisher music is now loaded on-demand:
        SoundManager.LoadSong("Punisher.it", MUSIC_PUNISHER);

        SoundManager.PlaySong(MUSIC_PUNISHER, false);

    }
    break;

    //einfaden
    case GEGNER_INIT2:
    {
        // einfaden
        alpha += 5.0f SYNC;

        if (alpha > 255.0f)
        {
            alpha = 255.0f;
            Handlung = GEGNER_LAUFEN;
            Destroyable	= true;
        }

    }
    break;

    // Spieler verfolgen
    case GEGNER_LAUFEN:
    {
        // Spieler verfolgen
        FollowPlayer();

        // Testen, ob der Spieler den Punisher ber�hrt hat
        for (int p = 0; p < NUMPLAYERS; p++)
            if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt],
                                Player[p].xpos, Player[p].ypos, Player[p].CollideRect) == true)
            {
                // Spieler stirbt
                Player[p].Energy = 0.0f;

                // und verliert Waffen
                for (int i = 0; i < 4; i++)
                    if (Player[p].CurrentWeaponLevel[i] > 1)
                        Player[p].CurrentWeaponLevel[i]--;

                Player[p].CalcWeaponLevels();

                // bekommt daf�r aber wieder Zeit
                TileEngine.Timelimit = TileEngine.TimelimitSave;

                // ausfaden
                Energy = 0.0f;
            }

        // Animphase setzen
        if (xAcc < 0.0f)
        {
            if (AnimCount > 20.0f)	AnimCount = 20.0f;
            AnimCount -= 1.0f SYNC;
            if (AnimCount < 0.0f)	AnimCount = 10.0f;
        }
        else
        {
            if (AnimCount < 10.0f)	AnimCount = 10.0f;
            AnimCount += 1.0f SYNC;
            if (AnimCount > 30.0f)	AnimCount = 20.0f;
        }

        AnimPhase = (int)(AnimCount);
    }
    break;

    // ausfaden
    case GEGNER_SPECIAL:
    {
        xAcc = 0.0f;
        yAcc = 0.0f;
        xSpeed = 0.0f;
        ySpeed = 0.0f;

        // ausfaden
        alpha -= 5.0f SYNC;

        if (alpha < 0.0f)
        {
            alpha = 0.0f;
            Energy = 0.0f;
        }

    }
    break;

    default :
        break;
    } // switch

    // Boss oder SummaryScreen ? Dann verschwinden
    if (HUD.BossHUDActive != 0.0f ||
            ShowSummary == true)
        Energy = 0.0f;

    if (Energy <= 0.0f &&
            alpha > 0.0f   &&
            Handlung != GEGNER_SPECIAL)
        Vanish();
}

// --------------------------------------------------------------------------------------
// Punisher verschwindet
// --------------------------------------------------------------------------------------

void GegnerPunisher::Vanish(void)
{
    if (ShowSummary == true)
        SoundManager.StopSong(MUSIC_PUNISHER, false);
    else
        SoundManager.FadeSong(MUSIC_PUNISHER, -2.0f, 0, false);

    Handlung = GEGNER_SPECIAL;
    Energy = 1.0f;
    Destroyable	= false;
}

// --------------------------------------------------------------------------------------
// Punisher explodiert
// --------------------------------------------------------------------------------------

void GegnerPunisher::GegnerExplode(void)
{
    TileEngine.Timelimit += 100;

    Player[0].PunisherActive = false;
    Player[1].PunisherActive = false;

    SoundManager.SetSongVolume(MUSIC_STAGEMUSIC, 0);
    SoundManager.FadeSong(MUSIC_STAGEMUSIC, 2.0f, 100, true);
}
