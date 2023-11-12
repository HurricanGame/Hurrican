// --------------------------------------------------------------------------------------
// Vertikale Schleuse
//
// Blockiert das Level
// --------------------------------------------------------------------------------------

#include "Trigger_SchleuseV.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchleuseV::GegnerSchleuseV(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    BlickRichtung = DirectionEnum::LINKS;
    Energy = 100;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    TestBlock = false;
}

// --------------------------------------------------------------------------------------
// "SchleuseV KI"
// --------------------------------------------------------------------------------------

void GegnerSchleuseV::DoKI() {
    Energy = 100;
    DamageTaken = 0.0f;

    // Testen, ob der Spieler die Schleuse berührt hat
    //
    for (int p = 0; p < NUMPLAYERS; p++)
        if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], Player[p].xpos, Player[p].ypos, Player[p].CollideRect) ==
            true) {
            // Spieler wegschieben
            if (Player[p].xpos < xPos)
                Player[p].xpos = xPos + GegnerRect[GegnerArt].left - Player[p].CollideRect.right;
            if (Player[p].xpos > xPos)
                Player[p].xpos = xPos + GegnerRect[GegnerArt].right - Player[p].CollideRect.left;
        }

    // Testen, ob ein Gegner die Schleuse berührt hat
    for (auto& pEnemy: Gegner.enemies)
    {
        if (pEnemy->Active &&
            SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], pEnemy->xPos, pEnemy->yPos,
                                                     GegnerRect[pEnemy->GegnerArt])) {
            if (pEnemy->xPos < xPos)
                pEnemy->xPos = xPos + GegnerRect[GegnerArt].left - GegnerRect[pEnemy->GegnerArt].right;
            if (pEnemy->xPos > xPos)
                pEnemy->xPos = xPos + GegnerRect[GegnerArt].right - GegnerRect[pEnemy->GegnerArt].left;
        }
    }

    switch (Handlung) {
        // Schleuse wird geöffnet
        //
        case GEGNER::OEFFNEN: {
            Handlung = GEGNER::LAUFEN;
            Value1 = static_cast<int>(yPos);
            yAcc = -0.5f;
            // DKS - Added function WaveIsPlaying() to SoundManagerClass:
            if (!SoundManager.WaveIsPlaying(SOUND::DOOR))
                SoundManager.PlayWave(100, 128, 11025, SOUND::DOOR);
        } break;

        // Schleuse öffnet sich
        //
        case GEGNER::LAUFEN: {
            if (ySpeed < -5.0f) {
                ySpeed = -5.0f;
                yAcc = 0.0f;
            }

            // Ganz geöffnet?
            if (yPos < static_cast<float>(Value1) - 201.0f) {
                ySpeed = 0.0f;
                yAcc = 0.0f;
                Handlung = GEGNER::STEHEN;
                SoundManager.StopWave(SOUND::DOOR);
                SoundManager.PlayWave(100, 128, 11025, SOUND::DOORSTOP);
            }
        } break;
    }  // switch
}

// --------------------------------------------------------------------------------------
// SchleuseV explodiert
// --------------------------------------------------------------------------------------

void GegnerSchleuseV::GegnerExplode() {}
