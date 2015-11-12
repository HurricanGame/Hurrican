// --------------------------------------------------------------------------------------
// Die rumfliegende Fett Drone
//
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_BallerDrone.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerBallerdrone::GegnerBallerdrone(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_LAUFEN;
    Energy			= 120;
    ChangeLight		= Light;
    Destroyable		= true;
    Value1			= Wert1;
    Value2			= Wert2;
    shotdelay		= 10.0f;
    AnimCount		= 0.1f;
    OwnDraw			= true;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerBallerdrone::DoDraw(void)
{
    if (Handlung == GEGNER_FALLEN)
        pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-TileEngine.XOffset),
                                               (float)(yPos-TileEngine.YOffset),
                                               AnimPhase, 0xFFFF0000);
    else
        pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-TileEngine.XOffset),
                                               (float)(yPos-TileEngine.YOffset),
                                               AnimPhase, 0xFFFFFFFF);

    // Flare rendern
    if (AlreadyDrawn == false)
    {
        DirectGraphics.SetAdditiveMode ();
        Projectiles.LavaFlare.RenderSprite (float (xPos - TileEngine.XOffset - 15),
                                float (yPos - TileEngine.YOffset - 45), 0,
                                0xFFFF0000);
        DirectGraphics.SetColorKeyMode ();
    }

    AlreadyDrawn = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerBallerdrone::DoKI(void)
{
    BlickRichtung = LINKS;
    AnimCount -= 1.0f SYNC;

    if (AnimCount < 0.0f)
    {
        AnimCount += 0.2f;

        pPartikelSystem->PushPartikel (xPos + 37 + rand()%3, yPos + 35, ROBOMANSMOKE);
    }

    if (xSpeed >  20.0f) xSpeed =  20.0f;
    if (xSpeed < -20.0f) xSpeed = -20.0f;
    if (ySpeed < -10.0f) ySpeed = -10.0f;

    // Animationsphase je nach Speed festlegen
    AnimPhase = 6 + int (xSpeed / 2.0f);

    if (AnimPhase < 0)  AnimPhase = 0;
    if (AnimPhase > 12) AnimPhase = 12;


    // Je nach Handlung richtig verhalten
    switch (Handlung)
    {
    // fliegen und evtl. ballern ?
    //
    case GEGNER_LAUFEN:
    {
        //in Richtung Spieler fliegen
        if (xPos + 45 < pAim->xpos + 35) xAcc =  2.5f;
        if (xPos + 45 > pAim->xpos + 35) xAcc = -2.5f;
        if (yPos < pAim->ypos - 140) yAcc =  2.5f;
        if (yPos > pAim->ypos - 140) yAcc = -2.5f;
        if (ySpeed >  10.0f) ySpeed =  10.0f;

        if (PlayerAbstand() < 800)
            shotdelay -= 1.0f SYNC;

        if (shotdelay <= 0.0f)
        {
            /*				if ((xPos + 45 > pAim->xpos + 35 &&
            					 AnimPhase > 6) ||

            					(xPos + 45 < pAim->xpos + 35 &&
            					 AnimPhase < 6))
            */
            {
                shotdelay = 15.0f;
                Projectiles.PushProjectile(xPos + 12, yPos + 50, SUCHSCHUSS2);
                Projectiles.PushProjectile(xPos + 64, yPos + 50, SUCHSCHUSS2);
                SoundManager.PlayWave(50, 128, 14000 + rand()%2000, SOUND_GOLEMSHOT);
            }
        }
    }
    break;

    // rumfliegen, weil abgeschossen ?
    //
    case GEGNER_FALLEN:
    {
        // An Wand gestossen ?
        //
        if (blocko & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_WAND ||
                blockl & BLOCKWERT_WAND ||
                blockr & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_PLATTFORM)
            Energy = 0.0f;

        shotdelay -= 1.0f SYNC;

        if (shotdelay < 0.0f)
        {
            shotdelay = 5.0f SYNC;
            pPartikelSystem->PushPartikel (xPos + 35 + rand()%5, yPos + 20 + rand ()%5, ROCKETSMOKE);
            pPartikelSystem->PushPartikel (xPos + 30 + rand()%5, yPos + 20 + rand ()%5, SMOKE3);
            pPartikelSystem->PushPartikel (xPos + 30 + rand()%5, yPos + 20 + rand ()%5, FUNKE);
        }
    }
    break;
    }

    // Spieler berührt ?
    //

    if (Handlung != GEGNER_FALLEN)
        TestDamagePlayers(4.0f SYNC);

    // abgeknallt ?
    // Dann Explosion erzeugen und lossegeln lassen ;)
    //
    if (Energy <= 0.0f && Handlung != GEGNER_FALLEN)
    {
        Energy   = 100.0f;
        Handlung = GEGNER_FALLEN;
        yAcc     = 3.0f;
        SoundManager.PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);
        pPartikelSystem->PushPartikel (xPos - 20, yPos - 40, EXPLOSION_BIG);
        shotdelay = 1.0f;
    }
}

// --------------------------------------------------------------------------------------
// Ballerdrone explodiert
// --------------------------------------------------------------------------------------

void GegnerBallerdrone::GegnerExplode(void)
{
    SoundManager.PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION3);

    int i;

    for (i=0; i < 10; i++)
        pPartikelSystem->PushPartikel (xPos - 30 + rand ()%90, yPos - 30 + rand()%60, EXPLOSION_MEDIUM2);

    for (i=0; i < 30; i++)
        pPartikelSystem->PushPartikel (xPos - 40 + rand ()%90, yPos - 30 + rand()%60, SMOKEBIG);

    for (i=0; i < 40; i++)
        pPartikelSystem->PushPartikel (xPos + rand ()%90, yPos + rand()%60, FUNKE);

    Player[0].Score += 300;
}
