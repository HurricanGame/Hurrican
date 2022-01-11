// --------------------------------------------------------------------------------------
// Der kleine Stern im Flugsack level
//
// Prallt von der Wand ab
// --------------------------------------------------------------------------------------

#include "enemies/Gegner_StarSmall.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerStarSmall::GegnerStarSmall(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 20;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimStart = 0;
    AnimEnde = 4;
    AnimSpeed = 0.8f;
    AnimCount = 0.0f;
    ChangeLight = Light;
    Destroyable = true;

    ySpeed = 8.0f;
    yAcc = 0.0f;
    xAcc = 0.0f;

    // in Richtung Spieler fliegen
    //
    if (pAim->xpos + 80 < xPos)
        xSpeed = -8.0f;
    else
        xSpeed = 8.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerStarSmall::DoKI() {
    if (xSpeed > 0.0f)
        BlickRichtung = RECHTS;
    else
        BlickRichtung = LINKS;

    SimpleAnimation();

    // Je nach Handlung richtig verhalten
    //
    switch (Handlung) {
        case GEGNER::LAUFEN: {
            // An den Wänden umdrehen
            //
            if ((xSpeed < 0.0f && blockl & BLOCKWERT_WAND) || (xSpeed > 0.0f && blockr & BLOCKWERT_WAND))
                xSpeed *= -1.0f;

            if ((ySpeed < 0.0f && blocko & BLOCKWERT_WAND) || (ySpeed > 0.0f && blocku & BLOCKWERT_WAND) ||
                (ySpeed < 0.0f && yPos - TileEngine.YOffset < 0.0f) ||
                (ySpeed > 0.0f && yPos - TileEngine.YOffset > 440.0f))
                ySpeed *= -1.0f;
        } break;

        default:
            break;

    }  // switch

    // Testen, ob der Spieler den Sack berührt hat
    TestDamagePlayers(2.0f SYNC);
}

// --------------------------------------------------------------------------------------
// StarSmall explodiert
// --------------------------------------------------------------------------------------

void GegnerStarSmall::GegnerExplode() {
    for (int i = 0; i < 8; i++)
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(40)),
                                    yPos - 30.0f + static_cast<float>(random(40)), EXPLOSION_MEDIUM2);

    for (int i = 0; i < 8; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(40)),
                                    yPos + static_cast<float>(random(40)), LONGFUNKE);

    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);

    Player[0].Score += 150;  // Punkte geben
}
