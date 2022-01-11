// --------------------------------------------------------------------------------------
// Die Made
//
// Wird vom Bratklops gekotzt und explodiert am Boden/Hurri oder bei Beschuss
// --------------------------------------------------------------------------------------

#include "Gegner_Made.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerMade::GegnerMade(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::INIT;
    HitSound = 1;
    Energy = 10;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimStart = 0;
    AnimEnde = 20;
    AnimSpeed = (random(10) + 5) / 20.0f;
    AnimCount = 0.0f;
    ChangeLight = Light;
    Destroyable = true;
    AnimPhase = 16;
    OwnDraw = true;
    movesin = 0.0f;

    // Nur nach rechts fliegen? (Bratklops)
    //
    if (Value1 == 98) {
        AnimPhase = random(20);
        yAcc = 4.0f;
        xSpeed = static_cast<float>(random(120)) / 3.0f;
        ySpeed = -((static_cast<float>(random(40)) / 3.0f) + 8.0f);
        Handlung = GEGNER::FALLEN;
    }

    // oder in alle Richtungen (Schwabbel)
    else if (Value1 == 99) {
        AnimPhase = random(20);
        yAcc = 4.0f;
        xSpeed = static_cast<float>(random(200) - 100) / 12.0f;
        ySpeed = -((static_cast<float>(random(40)) / 2.0f) + 12.0f);
        Handlung = GEGNER::FALLEN;
    }
}

// --------------------------------------------------------------------------------------
// Draw funktion
// --------------------------------------------------------------------------------------

void GegnerMade::DoDraw() {
    movesin += Timer.sync(0.8f);

    if (movesin > TWO_PI)
        movesin = 0.0f;

    switch (Handlung) {
        case GEGNER::LAUFEN: {
            pGegnerGrafix[GegnerArt]->RenderSpriteScaled(
                xPos - TileEngine.XOffset - static_cast<float>(sin(movesin) * 2.5f),
                yPos - TileEngine.YOffset,
                20 + static_cast<int>(sin(movesin) * 5.0f),
                20,
                15,
                0xFFFFFFFF);

        } break;

        default: {
            pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                                   yPos - TileEngine.YOffset, AnimPhase,
                                                   0xFFFFFFFF);
        } break;
    }
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerMade::DoKI() {
    // langsame zugrunde gehen :P
    Energy -= Timer.sync(0.1f);

    BlickRichtung = LINKS;

    if (Handlung == GEGNER::INIT) {
        Handlung = GEGNER::LAUFEN;

        if (xPos + 10 < pAim->xpos + 35)
            xSpeed = 0.8f;
        else
            xSpeed = -0.8f;
    }

    if (Handlung == GEGNER::FALLEN) {
        SimpleAnimation();

        if ((blocku & BLOCKWERT_WAND) || (blocku & BLOCKWERT_GEGNERWAND) || (blocku & BLOCKWERT_PLATTFORM)) {
            Handlung = GEGNER::LAUFEN;
        }
    } else {
        AnimPhase = 15;
        yAcc = 4.0f;
        ySpeed = 0.0f;

        if (!(blocku & BLOCKWERT_WAND) && !(blocku & BLOCKWERT_GEGNERWAND) && !(blocku & BLOCKWERT_PLATTFORM)) {
            Handlung = GEGNER::FALLEN;
            yAcc = 4.0f;
        }
    }

    // an der Wand umdrehen
    if ((xSpeed < 0.0f && (blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND)) ||

        (xSpeed > 0.0f && (blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND))) {
        xSpeed *= -1;
    }

    // Testen, ob der Spieler die Made berührt hat
    //
    TestDamagePlayers(Timer.sync(2.0f));
}

// --------------------------------------------------------------------------------------
// Made explodiert
// --------------------------------------------------------------------------------------

void GegnerMade::GegnerExplode() {
    SoundManager.PlayWave(100, random(200) + 20, 8000 + random(4000), SOUND::MADE);

    for (int i = 0; i < 10; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(24)),
                                    yPos - 12.0f + static_cast<float>(random(22)), MADEBLUT);
}
