// --------------------------------------------------------------------------------------
// Der Mutant, der aus der Röhre kam... Ein Film von Poke53280
//
// Kann wahlweise ins Level gesetzt werden oder kommt aus der Röhre, wenn
// der Spieler nahe genug dransteht
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Mutant.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerMutant::GegnerMutant(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_STEHEN;
    HitSound		= 1;
    Value1			= Wert1;
    Value2			= Wert2;
    ChangeLight		= Light;
    Destroyable		= true;
    Energy			= 100;
    TestBlock		= true;
    JumpDelay		= 0;

    if (Value1 == 0)
    {
        Handlung = GEGNER_LAUFEN;
        AnimSpeed		= 0.8f;
        AnimStart = 4;
        AnimPhase = 4;
        AnimEnde = 17;
    }
    else
    {
        Handlung = GEGNER_EINFLIEGEN;
        AnimSpeed = 3.0f;
        AnimStart = 1;
        AnimPhase = 1;
        AnimEnde = 99;

        ySpeed = 2.0f;
        yAcc   = 3.0f;
    }

    zRot = 0.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerMutant::DoKI(void)
{
    switch (Handlung)
    {
    case GEGNER_LAUFEN:
    {
        if (PlayerAbstandHoriz() > 20)
        {
            SimpleAnimation();

            // in Richtung Spieler kriechen
            if (xPos + 50 < pAim->xpos + 35)
                BlickRichtung = RECHTS;
            else
                BlickRichtung = LINKS;

            xSpeed = 5.0f * BlickRichtung;
        }
        else
        {
            xSpeed = 0.0f;
            AnimPhase = 3;
        }

        // Runterfallen
        if (!(blocku & BLOCKWERT_WAND) &&
                !(blocku & BLOCKWERT_PLATTFORM))
        {
            yAcc = 5.0f;
        }
        else
        {
            ySpeed = 0.0f;
            yAcc = 0.0f;

            if (PlayerAbstand() < 400.0f)
                JumpDelay -= 1.0f SYNC;

            if (JumpDelay <= 0.0f)
            {
                Handlung = GEGNER_SPRINGEN;
                xSpeed = 15.0f * BlickRichtung;
                AnimStart = 18;
                AnimPhase = 18;
                AnimEnde = 26;
                AnimSpeed = 1.5f;
                JumpDelay = 8.0f + rand()%5;
                ySpeed = -30.0f;
                yAcc = 6.0f;

                //DKS - Sound was barely audible with volume of 10, pretty sure this was a typo..
                //      I went ahead and added 3D panning effect while fixing volume problem:
                //SoundManager.PlayWave(10, 128, 12000 + rand()%2000, SOUND_MUTANT);
                SoundManager.PlayWave3D((int)xPos+50, int(yPos)+45, 12000 + rand()%2000, SOUND_MUTANT);
            }
        }

    }
    break;

    case GEGNER_SPRINGEN:
    {
        if (AnimPhase < 24)
            SimpleAnimation();
        else
        {
            AnimPhase = 24;
            yAcc = 15.0f;
        }

        // wieder laufen?
        if (ySpeed > 0.0f &&
                (blocku & BLOCKWERT_WAND ||
                 blocku & BLOCKWERT_PLATTFORM))
        {
            Handlung = GEGNER_LAUFEN;
            AnimSpeed = 0.8f;
            AnimStart = 4;
            AnimPhase = 4;
            AnimEnde = 17;
        }
    }
    break;

    case GEGNER_EINFLIEGEN:
    {
        SimpleAnimation();

        // Am Boden kurz warten
        if (AnimPhase > 2)
            AnimSpeed = 4.0f;

        // Aufgekommen?
        if (blocku & BLOCKWERT_WAND &&
                ySpeed > 0.0f)
        {
            //DKS - While fixing volume of jumping (look at note further above), made this 3D too:
            //SoundManager.PlayWave(100, 128, 11025, SOUND_SCHLEIM);
            SoundManager.PlayWave3D((int)xPos+50, int(yPos)+45, 11025, SOUND_SCHLEIM);
            ySpeed = 0.0f;
            yAcc = 0.0f;
        }

        // Jetzt loskrabbeln
        if (AnimPhase > 3)
        {
            Handlung = GEGNER_LAUFEN;
            AnimSpeed = 0.8f;
            AnimStart = 4;
            AnimPhase = 4;
            AnimEnde = 17;
        }
    }
    break;
    }

    if (Handlung != GEGNER_EINFLIEGEN)
        TestDamagePlayers(6.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Mutant explodiert
// --------------------------------------------------------------------------------------

void GegnerMutant::GegnerExplode(void)
{
    for (int i = 0; i < 10; i++)
        PartikelSystem.PushPartikel(xPos + rand()%60 - 10, yPos + rand()%60, EXPLOSION_GREEN);

    for (int i = 0; i < 10; i++)
        PartikelSystem.PushPartikel(xPos + rand()%60 + 15, yPos + 10 + rand()%60, SPIDERSPLITTER);

    for (int i = 0; i < 15; i++)
        PartikelSystem.PushPartikel(xPos + rand()%60 + 15, yPos + 10 + rand()%60, SCHLEIM);

    //DKS - While fixing volume of jumping (look at note further above), made this 3D too:
    //SoundManager.PlayWave(75, 128, 8000 + rand()%4000, SOUND_MUTANT);
    SoundManager.PlayWave3D((int)xPos+50, (int)yPos+45, 8000 + rand()%4000, SOUND_MUTANT);

    SoundManager.PlayWave(40, 128, 11025, SOUND_EXPLOSION4);
    SoundManager.PlayWave(40, 128, 6000 + rand()%4000, SOUND_SCHLEIM);

    Player[0].Score += 500;
}
