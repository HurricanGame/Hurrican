// --------------------------------------------------------------------------------------
// Der Luke ... SkiWalker
//
// Rast auf Skiern durch die Gegend
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_SkiWalker.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSkiWalker::GegnerSkiWalker(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_LAUFEN;
    Energy			= 25;
    Value1			= Wert1;
    Value2			= Wert2;
    ChangeLight		= Light;
    Destroyable		= true;
    xSpeed			= 20.0f;
    ySpeed			= 0.0f;
    yAcc			= 0.0f;
    AnimSpeed       = 1.0f;
    AnimEnde		= 11;
    ShotDelay		= 0.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSkiWalker::DoKI(void)
{
    SimpleAnimation();

    // And der Wand jeweils umdrehen
    if ((blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND) &&
            xSpeed > 0.0f)
    {
        if (Handlung == GEGNER_WATSCHELN && blockr & BLOCKWERT_WAND)
            Energy = 0;

        BlickRichtung = LINKS;
        xSpeed = -xSpeed;
    }

    if ((blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND) &&
            xSpeed < 0.0f)
    {
        if (Handlung == GEGNER_WATSCHELN && blockl & BLOCKWERT_WAND)
            Energy = 0;

        BlickRichtung = RECHTS;
        xSpeed = -xSpeed;
    }

    // Als Plattform dienen da man ja draufhopfen kann (man kann sogar darauf 'reiten')
    PlattformTest(GegnerRect[GegnerArt]);

    // Spieler draufgehüpft ?
    for (int i = 0; i < NUMPLAYERS; i++)
        if (Player[i].AufPlattform == this    &&
                (Handlung == GEGNER_LAUFEN        ||
                 Handlung == GEGNER_FALLEN)		 &&
                Player[i].Handlung != RADELN      &&
                Player[i].Handlung != RADELN_FALL &&
                Player[i].Handlung != SACKREITEN  &&
                Player[i].Handlung != DREHEN	     &&
                Player[i].yspeed > 0.0f)
        {
            // Spieler springen lassen
            Player[i].JumpPossible = false;
            Player[i].AnimPhase    = 0;
            Player[i].Handlung     = SPRINGEN;
            Player[i].JumpStart    = Player[i].ypos;
            Player[i].yspeed       = -PLAYER_MAXJUMPSPEED;
            Player[i].JumpAdd	  =  0.0f;

            AnimSpeed = 0.4f;
            Handlung  = GEGNER_SPRINGEN;

            xSpeed	  =   0.0f;
            ySpeed	  = -30.0f;
            yAcc	  =   5.0f;

            SoundManager.PlayWave(100, 128, 11025, SOUND_WALKERGIGGLE);
        }


// Je nach Handlung richtig verhalten
    switch (Handlung)
    {
    case GEGNER_LAUFEN:					// Durch die Gegend rasen
    {
        // Runterfallen
        if (!(blocku & BLOCKWERT_WAND) && !(blocku & BLOCKWERT_PLATTFORM))
        {
            yAcc   = 5.0f;
            yPos--;
            Handlung = GEGNER_FALLEN;
        }

        // Bei bestimmten Mindestabstand schiessen lassen
        if (PlayerAbstand() <= 220 &&
                ((BlickRichtung == LINKS  && pAim->xpos+45 <= xPos) ||
                 (BlickRichtung == RECHTS && pAim->xpos-45 >= xPos)))
        {
            ShotDelay -= 1.0f SYNC;

            if (ShotDelay <= 0.0f)
            {
                ShotDelay = (float)(10 + rand()%5);
                SoundManager.PlayWave(100, 128, 18000 + rand()%2000, SOUND_LASERSHOT);

                if (BlickRichtung == LINKS)
                    Projectiles.PushProjectile(xPos-18, yPos + 9, WALKER_LASER);
                else
                    Projectiles.PushProjectile(xPos+40, yPos + 9, WALKER_LASER2);
            }
        }
    }
    break;

    case GEGNER_FALLEN:
    {
        // Gelandet
        if (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM)
        {
            if (ySpeed > 0.0f)	ySpeed = -ySpeed / 2;
            if (ySpeed > -2.0f)
            {
                TileEngine.BlockUnten(xPos, yPos, xSpeed, ySpeed, GegnerRect[GegnerArt]);
                ySpeed = 0.0f;
                yAcc   = 0.0f;
                Handlung = GEGNER_LAUFEN;
            }
        }
    }
    break;

    case GEGNER_SPRINGEN:					// Getroffen fallen
    {
        // Keine zu hohe Geschwindigkeit
        if (ySpeed > 38.0f)
            ySpeed = 38.0f;

        // Testen, ob der Walker auf den Boden kommt
        if (ySpeed > 0.0f && (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM))
        {
            Handlung  = GEGNER_WATSCHELN;
            yAcc	  =  0.0f;
            ySpeed	  =  0.0f;
            xSpeed	  = 43.0f * BlickRichtung;
            TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
        }
    }
    break;

    case GEGNER_WATSCHELN:					// Walker ist getroffen und haut ab
    {
        if (!(blocku & BLOCKWERT_WAND) && !(blocku & BLOCKWERT_PLATTFORM))
        {
            yAcc   = 1.6f;
            Handlung = GEGNER_SPRINGEN;
        }
    }
    break;

    default :
        break;
    } // switch

    // Testen, ob der Spieler den Walker berührt hat
    if (Handlung != GEGNER_WATSCHELN &&
            Handlung != GEGNER_SPRINGEN)
        TestDamagePlayers(4.0f SYNC, false);
}

// --------------------------------------------------------------------------------------
// SkiWalker explodiert
// --------------------------------------------------------------------------------------

void GegnerSkiWalker::GegnerExplode(void)
{
    for (int i=0; i<5; i++)
        PartikelSystem.PushPartikel(float(xPos - 20 + rand()%45),
                                      float(yPos - 20 + rand()%45), EXPLOSION_MEDIUM2);

    SoundManager.PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION1);	// Sound ausgeben

    Player[0].Score += 100;

    for (int i = 0; i < NUMPLAYERS; i++)
        if (Player[i].AufPlattform == this)
            Player[i].AufPlattform = NULL;
}
