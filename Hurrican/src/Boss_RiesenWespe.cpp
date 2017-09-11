// --------------------------------------------------------------------------------------
// Die Riesen Wespe
//
// fliegt durch die Gegend und schiesst (weiss noch nicht, was)
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_RiesenWespe.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerRiesenWasp::GegnerRiesenWasp(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_LAUFEN;
    Energy			= 125;
    Value1			= Wert1;
    Value2			= Wert2;
    ChangeLight		= Light;
    Destroyable		= true;
    AnimEnde		= 5;
    AnimSpeed		= 1.0f;
    BlickRichtung   = LINKS;
    ShotDelay		= 10.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerRiesenWasp::DoKI(void)
{
    // animieren
    AnimCount += SpeedFaktor;		// Animationscounter weiterzählen
    if (AnimCount > AnimSpeed)			// Grenze überschritten ?
    {
        AnimCount = 0;					// Dann wieder auf Null setzen
        AnimPhase++;					// Und nächste Animationsphase

        if (AnimPhase > AnimEnde)		// Animation von zu Ende	?
        {
            AnimPhase = AnimStart;
        }
    }

    // in richtige Richtung kucken
    //
    if (xSpeed > 0.0f)
        BlickRichtung = RECHTS;
    else
        BlickRichtung = LINKS;

    //in Richtung Spieler fliegen
    //
    if (Handlung != GEGNER_FALLEN)
    {
        if (xPos + 50 < pAim->xpos + 35) xAcc =  2.5f;
        if (xPos + 50 > pAim->xpos + 35) xAcc = -2.5f;

        if (yPos < pAim->ypos - 100) yAcc =  1.5f;
        if (yPos > pAim->ypos - 100) yAcc = -1.5f;

        if (xSpeed >  22.0f) xSpeed =  22.0f;
        if (xSpeed < -22.0f) xSpeed = -22.0f;
        if (ySpeed >  10.0f) ySpeed =  10.0f;
        if (ySpeed < -10.0f) ySpeed = -10.0f;

        // An der Wand umdrehen
        //
        if ((blocko & BLOCKWERT_WAND ||blocko & BLOCKWERT_GEGNERWAND) && ySpeed < 0.0f) ySpeed *= 1.0f;
        if ((blocku & BLOCKWERT_WAND ||blocku & BLOCKWERT_GEGNERWAND) && ySpeed > 0.0f) ySpeed *= 1.0f;
        if ((blockl & BLOCKWERT_WAND ||blockl & BLOCKWERT_GEGNERWAND) && xSpeed < 0.0f) xSpeed *= 1.0f;
        if ((blockr & BLOCKWERT_WAND ||blockr & BLOCKWERT_GEGNERWAND) && xSpeed > 0.0f) xSpeed *= 1.0f;
    }

    // evtl schiessen lassen
    //
    if (PlayerAbstand () < 500 &&
            PlayerAbstand () > 100 &&
            ((BlickRichtung == RECHTS && xPos + 50 < pAim->xpos + 35) ||
             (BlickRichtung == LINKS  && xPos + 50 > pAim->xpos + 35)))
    {
        ShotDelay -= 1.0f SYNC;

        if (ShotDelay <= 0.0f)
        {
            ShotDelay = 10.0f;

            SoundManager.PlayWave (100, 128, 8000 + rand()%1000, SOUND_CANON);
            Projectiles.PushProjectile  (xPos + 25 + BlickRichtung * 56, yPos + 51, SUCHSCHUSS2);

            for (int i = 0; i < 5; i++)
                PartikelSystem.PushPartikel (xPos + rand ()%5 +  50 + BlickRichtung * 60, yPos + 75 + rand ()% 5, SMOKE3);
        }
    }

    // Je nach Handlung richtig verhalten
    //
    switch (Handlung)
    {
    // Wespe stürzt ab
    //
    case GEGNER_FALLEN:
    {
        // An die Wand gekracht ?
        if (blockl & BLOCKWERT_WAND ||
                blockr & BLOCKWERT_WAND ||
                blocko & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_PLATTFORM)
            Energy = 0.0f;

        // Grenze der Fallgeschwindigkeit
        if (ySpeed > 35.0f)
            yAcc = 0.0f;

        // Wespe rauchen lassen
        if (AnimCount >= 0.5f)
        {
            AnimCount = 0.0f;
            PartikelSystem.PushPartikel(xPos+rand()%80 + 30, yPos+30+rand()%50, SMOKE);
            PartikelSystem.PushPartikel(xPos+rand()%80 + 30, yPos+30+rand()%50, SMOKE3);
        }
    }
    break;
    }

    // Testen, ob der Spieler die Wespe berührt hat
    TestDamagePlayers(5.0f SYNC);

    // Wespe stürzt ab ?
    //
    if (Energy <= 0.0f && Handlung != GEGNER_FALLEN)
    {
        Handlung = GEGNER_FALLEN;
        AnimCount = 0.0f;
        Energy	 = 50.0f;
        ySpeed   = 1.0f;
        yAcc	 = 2.0f;
        xAcc     = 0.0f;
    }
}

// --------------------------------------------------------------------------------------
// RiesenWasp explodiert
// --------------------------------------------------------------------------------------

void GegnerRiesenWasp::GegnerExplode(void)
{
    // Explosionen erzeugen
    for (int i=0; i<10; i++)
        PartikelSystem.PushPartikel(xPos + rand()%110-30, yPos + rand()%60 - 30, EXPLOSION_BIG);

    for (int i=0; i<20; i++)
        PartikelSystem.PushPartikel(xPos + rand()%110-10, yPos + rand()%60 - 10, EXPLOSION_MEDIUM2);

    for (int i=0; i<200; i++)
        PartikelSystem.PushPartikel(xPos + rand()%110-10, yPos + rand()%50, ROCKETSMOKE);

    for (int i = 0; i < 10; i++)
        PartikelSystem.PushPartikel (xPos + rand ()%100, yPos + 30 + rand ()%50, SPLITTER);

    SoundManager.PlayWave(100, 128, 11025 + rand()%2000, SOUND_EXPLOSION1);	// Sound ausgeben

    Player[0].Score += 250;
}
