// --------------------------------------------------------------------------------------
// Horizontale Schleuse
//
// Blockiert das Level
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_SchleuseH.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchleuseH::GegnerSchleuseH(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_STEHEN;
    BlickRichtung	= LINKS;
    Energy			= 50;
    Value1			= Wert1;
    Value2			= Wert2;
    ChangeLight		= Light;
    Destroyable		= true;
    TestBlock		= false;
}

// --------------------------------------------------------------------------------------
// "SchleuseH KI"
// --------------------------------------------------------------------------------------

void GegnerSchleuseH::DoKI(void)
{
    Energy = 100;
    DamageTaken = 0.0f;
    PlattformTest(GegnerRect[GegnerArt]);

    switch (Handlung)
    {
    // Schleuse wird geöffnet
    //
    case GEGNER_OEFFNEN:
    {
        Handlung = GEGNER_LAUFEN;
        Value1   = int (xPos);
        xAcc     = -0.5f;
        //DKS - Added function WaveIsPlaying() to SoundManagerClass:
        if (!SoundManager.WaveIsPlaying(SOUND_DOOR))
            SoundManager.PlayWave (100, 128, 11025, SOUND_DOOR);
    }
    break;

    // Schleuse öffnet sich
    //
    case GEGNER_LAUFEN:
    {
        if (xSpeed < -5.0f)
        {
            xSpeed = -5.0f;
            xAcc   =   0.0f;
        }

        // Ganz geöffnet?
        if (xPos < float (Value1) - 201.0f)
        {
            xSpeed   = 0.0f;
            xAcc	 = 0.0f;
            Handlung = GEGNER_STEHEN;
            SoundManager.PlayWave (100, 128, 11025, SOUND_DOORSTOP);
            SoundManager.StopWave (SOUND_DOOR);
        }
    }
    break;
    } // switch
}

// --------------------------------------------------------------------------------------
// SchleuseH explodiert
// --------------------------------------------------------------------------------------

void GegnerSchleuseH::GegnerExplode(void)
{
}
