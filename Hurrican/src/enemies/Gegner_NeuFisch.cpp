// --------------------------------------------------------------------------------------
// Der neue Fisch
//
// Schwimmt wie der alte Fisch im Wasser, und geht auf den Hurri los
// --------------------------------------------------------------------------------------

#include "Gegner_NeuFisch.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerNeuFisch::GegnerNeuFisch(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    HitSound = 1;
    Energy = 60;
    AnimEnde = 9;
    AnimSpeed = 0.5f;
    AnimCount = 0.0f;
    ChangeLight = Light;
    Destroyable = true;
    pFest = nullptr;
    MoveSpeed = 5.0f + static_cast<float>(random(50)) / 10.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerNeuFisch::DoKI() {
    // Animieren
    //
    AnimCount += SpeedFaktor;  // Animationscounter weiterzählen
    if (Handlung == GEGNER::LAUFEN || Handlung == GEGNER::SPECIAL) {
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;              // Dann wieder auf Null setzen
            AnimPhase++;                // Und nächste Animationsphase
            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
                AnimPhase = AnimStart;  // Dann wieder von vorne beginnen
        }
    } else if (Handlung == GEGNER::DREHEN) {
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;              // Dann wieder auf Null setzen
            AnimPhase++;                // Und nächste Animationsphase
            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
            {
                AnimCount = 0.0f;
                Handlung = GEGNER::DREHEN2;
                BlickRichtung *= -1;
            }
        }
    } else if (Handlung == GEGNER::DREHEN2) {
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;       // Dann wieder auf Null setzen
            AnimPhase--;         // Und nächste Animationsphase
            if (AnimPhase <= 9)  // Animation von zu Ende	?
            {
                AnimPhase = AnimStart;  // Dann wieder von vorne beginnen
                AnimEnde = 9;
                Handlung = GEGNER::LAUFEN;
                AnimSpeed = 0.5f;

                xSpeed = BlickRichtung * MoveSpeed;
            }
        }
    }

    // Spieler verfolgen
    //
    ySpeed = 0.0f;

    if (Handlung != GEGNER::SPECIAL) {
        // verfolgen
        xSpeed = BlickRichtung * MoveSpeed;

        if (pAim->InLiquid == true) {
            if (yPos < pAim->ypos - 8 && (blocku & BLOCKWERT_WASSER))
                ySpeed = static_cast<float>(random(50) + 50) / 10.0f;
            if (yPos > pAim->ypos - 8 && (blocko & BLOCKWERT_WASSER))
                ySpeed = -static_cast<float>(random(50) + 50) / 10.0f;
        }
    }

    switch (Handlung) {
        // In der Suppe rumdümpeln
        //
        case GEGNER::LAUFEN: {

            bool onWall = (BlickRichtung == LINKS && ((blockl & BLOCKWERT_WAND) || (blockl & BLOCKWERT_GEGNERWAND))) ||
                          (BlickRichtung == RECHTS && ((blockr & BLOCKWERT_WAND) || (blockr & BLOCKWERT_GEGNERWAND)));

            if (onWall || (pAim->InLiquid == true && ((xPos + 30 < pAim->xpos + 35 && BlickRichtung == LINKS) ||
                                                      (xPos + 30 > pAim->xpos + 35 && BlickRichtung == RECHTS)))) {
                Handlung = GEGNER::DREHEN;
                AnimPhase = 9;
                AnimCount = 0.0f;
                AnimEnde = 14;
                AnimSpeed = 0.3f;
                xSpeed = 0.0f;
                ySpeed = 0.0f;
            }

            blockl = blockr = 0;

            RECT_struct rect;
            rect.top = 32;
            rect.bottom = 40;

            if (BlickRichtung == LINKS) {
                rect.left = 10;
                rect.right = 20;
            } else {
                rect.left = 70;
                rect.right = 80;
            }

            // festgebissen?
            for (int p = 0; p < NUMPLAYERS; p++)
                if (SpriteCollision(Player[p].xpos, Player[p].ypos, Player[p].CollideRect, xPos, yPos, rect) == true) {
                    pFest = &Player[p];
                    TestBlock = false;
                    Handlung = GEGNER::SPECIAL;
                    AnimSpeed = 0.3f;

                    Value1 = static_cast<int>(pAim->xpos - xPos);
                    Value2 = static_cast<int>(pAim->ypos - yPos);

                    AnimPhase = 0;
                    AnimEnde = 9;
                    AnimSpeed = 0.5f;
                }
        } break;

        // Fisch hängt am Spieler fest
        case GEGNER::SPECIAL: {
            pFest->GegnerDran = true;
            AnimPhase = 0;
            TestDamagePlayers(Timer.sync(1.5f));

            xPos = pFest->xpos - Value1;
            yPos = pFest->ypos - Value2;

            if (pFest->Handlung == PlayerActionEnum::RADELN ||
                    pFest->Handlung == PlayerActionEnum::RADELN_FALL) {
                Handlung = GEGNER::LAUFEN;
                ;
                AnimEnde = 9;
                AnimSpeed = 0.5f;
                AnimCount = 0.0f;
                TestBlock = true;

                if (pFest->InLiquid == false)
                    Energy = 0.0f;
            }
        } break;
    }
}

// --------------------------------------------------------------------------------------
// NeuFisch explodiert
// --------------------------------------------------------------------------------------

void GegnerNeuFisch::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);

    for (int i = 0; i < 10; i++) {
        // Fetzen erzeugen
        PartikelSystem.PushPartikel(xPos - 20.0f + static_cast<float>(random(90)),
                                    yPos - 5.0f + static_cast<float>(random(70)), PIRANHATEILE);

        // und noch n paar Luftblässchen dazu
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(90)),
                                    yPos + 10.0f + static_cast<float>(random(70)), BUBBLE);
    }

    // Blutwolke dazu
    //
    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(60)),
                                    yPos + static_cast<float>(random(40)), PIRANHABLUT);

    Player[0].Score += 250;
}
