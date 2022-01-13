// --------------------------------------------------------------------------------------
// Horizontale Schleuse
//
// Blockiert das Level
// --------------------------------------------------------------------------------------

#include "Trigger_SchleuseH.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchleuseH::GegnerSchleuseH(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    BlickRichtung = DirectionEnum::LINKS;
    Energy = 50;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    TestBlock = false;
}

// --------------------------------------------------------------------------------------
// "SchleuseH KI"
// --------------------------------------------------------------------------------------

void GegnerSchleuseH::DoKI() {
    Energy = 100;
    DamageTaken = 0.0f;
    PlattformTest(GegnerRect[GegnerArt]);

    switch (Handlung) {
        // Schleuse wird geöffnet
        //
        case GEGNER::OEFFNEN: {
            Handlung = GEGNER::LAUFEN;
            Value1 = static_cast<int>(xPos);
            xAcc = -0.5f;
            // DKS - Added function WaveIsPlaying() to SoundManagerClass:
            if (!SoundManager.WaveIsPlaying(SOUND::DOOR))
                SoundManager.PlayWave(100, 128, 11025, SOUND::DOOR);
        } break;

        // Schleuse öffnet sich
        //
        case GEGNER::LAUFEN: {
            if (xSpeed < -5.0f) {
                xSpeed = -5.0f;
                xAcc = 0.0f;
            }

            // Ganz geöffnet?
            if (xPos < static_cast<float>(Value1) - 201.0f) {
                xSpeed = 0.0f;
                xAcc = 0.0f;
                Handlung = GEGNER::STEHEN;
                SoundManager.PlayWave(100, 128, 11025, SOUND::DOORSTOP);
                SoundManager.StopWave(SOUND::DOOR);
            }
        } break;
    }  // switch
}

// --------------------------------------------------------------------------------------
// SchleuseH explodiert
// --------------------------------------------------------------------------------------

void GegnerSchleuseH::GegnerExplode() {}
