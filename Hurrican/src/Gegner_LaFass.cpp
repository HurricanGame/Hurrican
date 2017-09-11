// --------------------------------------------------------------------------------------
// Das Fass
//
// Gondelt hin und her, und lässt sich vom Spieler abschiessen
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_LaFass.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerLaFass::GegnerLaFass(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_INIT;
    Energy			= 1.0f;
    Value1			= Wert1;
    Value2			= Wert2;
    ChangeLight		= Light;
    Destroyable		= true;
    AnimPhase		= rand()%15;
    AnimSpeed		= 1.2f;
    AnimEnde		= 16;
    Active			= true;
    TestBlock		= true;

    xSpeed = (float)(Value1);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerLaFass::DoKI(void)
{
    SimpleAnimation();
    BlickRichtung = LINKS;

    switch (Handlung)
    {
    case GEGNER_LAUFEN:
    {
        if ((blockl & BLOCKWERT_WAND) ||
                (blockr & BLOCKWERT_WAND))
        {
            Energy = 0.0f;
            Value2 = 99;
        }

    }
    break;

    case GEGNER_INIT:
        Handlung = GEGNER_LAUFEN;
        break;
    }
}

// --------------------------------------------------------------------------------------
// LaFass explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerLaFass::GegnerExplode(void)
{
    // wurde abgeschossen? Dann Fass fallen lassen
    //
    if (Value2 != 99)
    {
        WinkelUebergabe = xSpeed;

        // Kettenteile
        PartikelSystem.PushPartikel(xPos, yPos + 78, KETTENTEILE);
        PartikelSystem.PushPartikel(xPos + 39, yPos + 78, KETTENTEILE2);
        PartikelSystem.PushPartikel(xPos + 16, yPos + 43, KETTENTEILE3);

        for (int i = 0; i < 3; i++)
            PartikelSystem.PushPartikel(xPos + 33, yPos + i*18, KETTENTEILE4);

        // Fass
        Projectiles.PushProjectile(xPos + 7, yPos + 90, LAFASSSHOT);
    }
}
