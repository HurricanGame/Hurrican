// --------------------------------------------------------------------------------------
// Der ausfahrbare Mini Drachen
//
// Siehe Turrican 2 =)
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Minidragon.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerMiniDragon::GegnerMiniDragon(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_INIT;
    Energy			= 200;
    AnimSpeed		= 0.5f;
    ChangeLight		= Light;
    Destroyable		= false;
    Value1			= Wert1;
    Value2			= Wert2;
    OwnDraw			= true;
    Segments		= 6;
    AnimEnde		= 3;
    AnimSpeed		= 1.0f;
    ShotDelay		= 4.0f;

    // Vom Drache abgeworfen?
    if (Value2 == 99)
    {
        ySpeed = -(13.0f + rand()%4);
        yAcc = 3.0f;
        xSpeed = (float)(rand()%10 - 5) / 2.0f;
        Handlung = GEGNER_FALLEN;

        AnimCount = 0.0f;
        AnimStart = 5;
        AnimPhase = 5;
        AnimEnde  = 15;
        AnimSpeed = 0.5f;
    }
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerMiniDragon::DoDraw(void)
{
    bool mirrored;
    float mulx, muly;

    muly = (OldY - yPos) / 6.0f;
    mulx = (OldX - xPos) / 6.0f;

    mirrored = pAim->xpos + 35 < xPos + 30;

    int i = 0;
    for (int i = 0; i < Segments; i++)
        pGegnerGrafix[GegnerArt]->RenderSprite ((float)(xPos-TileEngine.XOffset) + (6-i) * mulx,
                                                (float)(yPos-TileEngine.YOffset) + (6-i) * muly,
                                                4, 0xFFFFFFFF);

    // Kopf
    if (mirrored)
        BlickRichtung = -1;
    else
        BlickRichtung = 1;

    int a = 0;

    switch (int(ShotDelay))
    {
    case 0 :
        a = 1;
        break;

    case 8 :
    case 7 :
        a = 2;
        break;

    case 6 :
        a = 1;
        break;

    default :
        a = 0;
    }

    if (Handlung == GEGNER_FALLEN)
    {
        a = AnimPhase;
        mirrored = xSpeed > 0.0f;
        BlickRichtung = 0;
    }

    if (Segments == 6)
    {
        i++;
        pGegnerGrafix[GegnerArt]->RenderSprite ((float)(xPos-TileEngine.XOffset) + BlickRichtung * 15.0f,
                                                (float)(yPos-TileEngine.YOffset),
                                                a, 0xFFFFFFFF, mirrored);
    }
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerMiniDragon::DoKI(void)
{

    if ((blocko & BLOCKWERT_WAND && ySpeed < 0.0f) ||
            (blocku & BLOCKWERT_WAND && ySpeed > 0.0f))
        ySpeed *= -1;

    if ((blockl & BLOCKWERT_WAND && xSpeed < 0.0f) ||
            (blockr & BLOCKWERT_WAND && xSpeed > 0.0f))
        xSpeed *= -1;

    // Je nach Handlung richtig verhalten
    switch (Handlung)
    {

    case GEGNER_FALLEN:
    {
        SimpleAnimation();

        if (ySpeed > 40.0f)
            ySpeed = 40.0f;

        // Am Boden angekommen?
        blocku = TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
        if (blocku & BLOCKWERT_WAND)
        {
            xSpeed = 0.0f;
            ySpeed = 0.0f;
            xAcc = 0.0f;
            yAcc = 0.0f;
            Handlung = GEGNER_INIT;

            AnimEnde = 0;
            AnimPhase = 0;

            SoundManager.PlayWave (100, 128, 8000 + rand()%8000, SOUND_EXPLOSION1);

            //int i = 0;
            for (int i = 0; i < 10; i++)
                PartikelSystem.PushPartikel(xPos + 10 + rand()%30, yPos + rand()%10, SPIDERSPLITTER);

            for (int i = 0; i < 3; i++)
                PartikelSystem.PushPartikel(xPos - 10 + rand()%30, yPos - 20 + rand()%20, EXPLOSION_MEDIUM3);
        }
    }
    break;

    // Position sichern
    case GEGNER_INIT:
    {
        OldX = xPos;
        OldY = yPos;

        // Spieler nah genug dran?
        for (int p = 0; p < NUMPLAYERS; p++)
            if (PlayerAbstandHoriz(&Player[p]) < 300 &&
                    PlayerAbstandVert(&Player[p]) < 300)
            {
                Handlung = GEGNER_INIT2;

                if (pAim->xpos + 35 < xPos + 30)
                {
                    xSpeed =  10.0f;
                    xAcc   = -2.0f;
                }
                else
                {
                    xSpeed = -10.0f;
                    xAcc   =  2.0f;
                }

                if (pAim->ypos + 40 < yPos + 20)
                {
                    ySpeed =  10.0f;
                    yAcc   = -2.0f;
                }
                else
                {
                    ySpeed = -10.0f;
                    yAcc   =  2.0f;
                }

                //DKS - Added function WaveIsPlaying() to SoundManagerClass:
                if (!SoundManager.WaveIsPlaying(SOUND_STEAM) &&
                        !SoundManager.WaveIsPlaying(SOUND_SPIDERSCREAM))
                {
                    SoundManager.PlayWave(80, 128, 11025, SOUND_STEAM);
                    SoundManager.PlayWave(80, 128, 18000, SOUND_SPIDERSCREAM);
                }

                Destroyable = true;

                AnimCount = 5.0f;

                break;
            }
    }
    break;

    // kurz nach oben "stechen"
    case GEGNER_INIT2:
    {
        AnimCount -= 1.0f SYNC;
        if (AnimCount <= 0.0f)
        {
            AnimCount = 0.0f;
            Handlung = GEGNER_STEHEN;
        }
    }
    break;

    // auf der stelle wackeln in Höhe des Hurris
    case GEGNER_STEHEN:
    {
        if (PlayerAbstand() < 600)
            ShotDelay -= 0.8f SYNC;

        if (ShotDelay < 0.0f)
        {
            ShotDelay = 9.0f;

            if (BlickRichtung == 1)
                WinkelUebergabe = 0.0f;
            else
                WinkelUebergabe = 1.0f;

            SoundManager.PlayWave(100, 128, 11000 + rand()%2000, SOUND_FIREBALL);

            WinkelUebergabe = 90.0f * BlickRichtung;
            Projectiles.PushProjectile(xPos + BlickRichtung * 10.0f, yPos, FIREBALL);
        }

        float xdiv = (pAim->xpos + 30.0f) - (OldX + 30.0f);
        float ydiv = (pAim->ypos + 35.0f) - (OldY + 20.0f);

        if (xdiv < -80)	xdiv = -80;
        if (xdiv >  80)	xdiv =  80;

        if (ydiv < -80)	ydiv = -80;
        if (ydiv >  80)	ydiv =  80;

        NewX = OldX + xdiv;
        NewY = OldY + ydiv;

        if (xPos < NewX)
            xAcc =  2.0f;
        else
            xAcc = -2.0f;

        if (yPos < NewY)
            yAcc =  2.0f;
        else
            yAcc = -2.0f;

        if (xSpeed < -8.0f) xSpeed = -8.0f;
        if (xSpeed >  8.0f) xSpeed =  8.0f;
        if (ySpeed < -8.0f) ySpeed = -8.0f;
        if (ySpeed >  8.0f) ySpeed =  8.0f;



    }
    break;

    // Glieder explodieren der Reihe nach
    case GEGNER_EXPLODIEREN:
    {
        Energy = 100.0f;
        Destroyable = false;
        ShotDelay -= 1.0f SYNC;

        // Ein Segment explodieren lassen
        if (ShotDelay < 0.0f)
        {
            float mulx, muly;

            muly = (OldY - yPos) / 6.0f;
            mulx = (OldX - xPos) / 6.0f;

            SoundManager.PlayWave (100, 128, 8000 + rand()%8000, SOUND_EXPLOSION1);
            PartikelSystem.PushPartikel(xPos + (6 - Segments) * mulx,
                                          yPos + (6 - Segments) * muly, EXPLOSION_MEDIUM2);


            for (int i=0; i < 3; i++)
                PartikelSystem.PushPartikel (xPos + (6 - Segments) * mulx + rand ()%40,
                                               yPos + (6 - Segments) * muly + rand()%30, SPIDERSPLITTER);

            for (int i=0; i < 10; i++)
                PartikelSystem.PushPartikel (xPos + (6 - Segments) * mulx + rand ()%40,
                                               yPos + (6 - Segments) * muly + rand()%30, FUNKE);

            ShotDelay = 2.0f;
            Segments--;

            if (Segments == 0)
                Energy = 0.0f;
        }
    }
    break;
    }

    if (Energy < 100.0f &&
            Handlung != GEGNER_EXPLODIEREN)
    {
        Handlung = GEGNER_EXPLODIEREN;
        Energy = 1.0f;
        ShotDelay = 0.0f;
        xSpeed = 0.0f;
        ySpeed = 0.0f;
        xAcc = 0.0f;
        yAcc = 0.0f;

        SoundManager.PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION3);
        PartikelSystem.PushPartikel(xPos - 26,
                                      yPos - 22, EXPLOSION_BIG);
    }

    // Spieler berührt ?
    //
    if (Handlung != GEGNER_EXPLODIEREN)
        TestDamagePlayers(4.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Drache explodiert
// --------------------------------------------------------------------------------------

void GegnerMiniDragon::GegnerExplode(void)
{
    Player[0].Score += 250;
}
