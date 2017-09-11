// --------------------------------------------------------------------------------------
// Der LaFassSpawner
//
// Spawnt in bestimmten Abständen (Value2) das LaFass =)
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_LaFassSpawner.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerLaFassSpawner::GegnerLaFassSpawner(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_STEHEN;
    BlickRichtung	= RECHTS;
    Energy			= 100;
    Value1			= Wert1;
    Value2			= Wert2;

    // default wert
    if (Value2 == 0)
        Value2 = 40;

    ChangeLight		= Light;
    Destroyable		= false;
    OwnDraw			= true;
    AnimCount		= (float)(Wert2);
    DontMove		= true;
    Active			= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerLaFassSpawner::DoKI(void)
{
    // Fass spawnen?
    if (PlayerAbstand() < 800)
        AnimCount -= 1.0f SYNC;

    if (AnimCount <= 0.0f)
    {
        AnimCount = (float)(Value2);

        // in die richtige Richtung schubsen
        if (Value1 == 0)
        {
            Gegner.PushGegner(xPos, yPos, LAFASS, -5, 0, false);
        }
        else
        {
            Gegner.PushGegner(xPos, yPos, LAFASS, 5, 0, false);
        }
    }
}

// --------------------------------------------------------------------------------------
// LaFassSpawner explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerLaFassSpawner::GegnerExplode(void)
{
}
