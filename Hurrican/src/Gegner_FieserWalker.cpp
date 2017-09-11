// --------------------------------------------------------------------------------------
// Der Fiese Walker mit dem Laser
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_FieserWalker.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFieserWalker::GegnerFieserWalker (int  Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_LAUFEN;
    Energy			= 100;
    Value1			= Wert1;
    Value2			= Wert2;
    ChangeLight		= Light;
    Destroyable		= true;
    AnimSpeed       = 0.5f;
    AnimEnde		= 14;
    xSpeed			= 5.0f;
    ShotDelay		= 0.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFieserWalker::DoKI(void)
{
    SimpleAnimation ();

    // Je nach Handlung anders verhalten
    //
    switch (Handlung)
    {
    // rumhopsen
    //
    case GEGNER_LAUFEN:
    {
        if (!(blocku & BLOCKWERT_WAND) &&
                !(blocku & BLOCKWERT_PLATTFORM))
        {
            Handlung = GEGNER_FALLEN;
            ySpeed   = 2.0f;
            yAcc	 = 3.0f;
        }

        // nahe genug zum schiessen?
        //
        if (ShotDelay <= 0.0f &&
                AnimPhase == 3 &&
                PlayerAbstand() < 400 &&
                PlayerAbstandVert () < 150 &&
                ((BlickRichtung == RECHTS && xPos < pAim->xpos) ||
                 (BlickRichtung == LINKS  && xPos > pAim->xpos))
           )
        {
            ShotDelay = 15.0f;
            Handlung  = GEGNER_STEHEN;
            xSpeed    = 0.0f;
            AnimEnde  = 0;
        }

        if (ShotDelay > 0.0f)
            ShotDelay -= 1.0f SYNC;

        TurnonShot();

    }
    break;

    case GEGNER_STEHEN:
    {
        if (ShotDelay > 0.0f)
            ShotDelay -= 1.0f SYNC;

        if (ShotDelay < 12.0f)
        {
            Handlung = GEGNER_LAUFEN;
            AnimEnde = 14;
            xSpeed = 5.0f * BlickRichtung;

            SoundManager.PlayWave (100, 128, 10000 + rand()%2000, SOUND_LASERSHOT);

            PartikelSystem.PushPartikel (xPos + 10 + BlickRichtung * 40, yPos + 6, LASERFLAME);
            Projectiles.PushProjectile  (xPos + 26 + BlickRichtung * 40, yPos + 23, SUCHSCHUSS);

        }

    }
    break;

    case GEGNER_FALLEN:
    {
        if (ySpeed > 35.0f)
        {
            ySpeed = 35.0f;
            yAcc   = 0.0f;
        }

        if (blocku & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_PLATTFORM)
        {
            ySpeed = 0.0f;
            yAcc   = 0.0f;
            Handlung = GEGNER_LAUFEN;
        }

    }
    break;

    }

    TurnonWall();

    // Spieler berührt den Gegner?
    TestDamagePlayers(4.0f SYNC);
}

// --------------------------------------------------------------------------------------
// FieserWalker explodiert
// --------------------------------------------------------------------------------------

void GegnerFieserWalker::GegnerExplode(void)
{
    PartikelSystem.PushPartikel(xPos - 30, yPos - 30, EXPLOSION_BIG);

    for (int i = 0; i < 8; i++)
        PartikelSystem.PushPartikel(xPos - 30 + rand ()% 60,
                                      yPos - 30 + rand ()% 60, EXPLOSION_MEDIUM2);
    for (int i = 0; i < 12; i++)
        PartikelSystem.PushPartikel(xPos + rand ()% 50,
                                      yPos + rand ()% 50, SPIDERSPLITTER);

    SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION4);	// Sound ausgeben

    Player[0].Score += 80;
}
