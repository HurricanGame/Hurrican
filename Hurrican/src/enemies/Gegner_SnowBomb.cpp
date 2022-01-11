// --------------------------------------------------------------------------------------
// Die große Schnee Kugel
// --------------------------------------------------------------------------------------

#include "Gegner_SnowBomb.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSnowBomb::GegnerSnowBomb(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::INIT;
    Energy = 30;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    AnimSpeed = 0.5f;
    AnimEnde = 19;
    OwnDraw = true;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerSnowBomb::DoDraw() {
    int anim;

    if (xSpeed < 0.0f)
        anim = AnimPhase;
    else
        anim = AnimEnde - AnimPhase;

    // Nur rendern, wenn die Kugel auch rollt
    //
    if (Handlung != GEGNER::INIT)
        pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                               yPos - TileEngine.YOffset, anim, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSnowBomb::DoKI() {
    PlattformTest(GegnerRect[GegnerArt]);
    SimpleAnimation();

    // Je nach Handlung richtig verhalten
    //
    switch (Handlung) {
        // Kommt zum ersten mal ins Bild? Dann an die gewünschte Position setzen (Value1/Value2)
        //
        case GEGNER::INIT: {
            // Schneebombe losrollen lassen, wenn Spieler in der Nähe ist
            //
            if (pAim->xpos > xPos)
                xSpeed = 15.0f;
            else
                xSpeed = -15.0f;

            Handlung = GEGNER::LAUFEN;
        } break;

        // Zur Seite kugeln
        //
        case GEGNER::LAUFEN: {
            if (!(blocku & BLOCKWERT_WAND)) {
                ySpeed = 10.0f;
                yAcc = 5.0f;
                Handlung = GEGNER::FALLEN;
            }
        } break;

        // Fallen und ggf am Boden abspringen
        //
        case GEGNER::FALLEN: {
            if (ySpeed > 0.0f && (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM)) {
                ySpeed *= -0.7f;

                if (ySpeed > -1.0f) {
                    ySpeed = 0.0f;
                    yAcc = 0.0f;
                    Handlung = GEGNER::LAUFEN;

                    // Position "begradigen"
                    //
                    blocku = TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
                }

                for (int i = 0; i < 10; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(50)),
                                                yPos + static_cast<float>(random(20)) + 50, WATERFLUSH2);

                SoundManager.PlayWave(100, 128, 6000 + random(2000), SOUND::LANDEN);
            }

            if (ySpeed > 30.0f)
                ySpeed = 30.0f;
        } break;

        default:
            break;
    }  // switch

    // Testen, ob der Spieler die Kugel berührt hat (nur, wenn er nicht drauf steht)
    //
    // TestDamagePlayers(4.0f SYNC);

    // an der Wand abprallen (und dabei Energie verlieren)
    //
    if ((xSpeed < 0.0f && blockl & BLOCKWERT_WAND) || (xSpeed > 0.0f && blockr & BLOCKWERT_WAND)) {
        Energy -= 1.0f;
        xSpeed *= -1.0f;
    }

    // Spieler seitlich verschieben, wenn auf Kugel drauf
    //
    // for (int i = 0; i < NUMPLAYERS; i++)
    //	if (Player[i].AufPlattform == this)
    //		Player[i].xpos += xSpeed / 2.0f SYNC;
}

// --------------------------------------------------------------------------------------
// SnowBomb explodiert
// --------------------------------------------------------------------------------------

void GegnerSnowBomb::GegnerExplode() {
    for (int i = 0; i < 8; i++)
        Projectiles.PushProjectile(xPos + static_cast<float>(random(60)),
                                   yPos + static_cast<float>(random(50)), SNOWBOMBSMALL);

    for (int i = 0; i < 20; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(70)),
                                    yPos - 10.0f + static_cast<float>(random(70)), SNOWFLUSH);

    SoundManager.PlayWave(100, 128, 6000 + random(2000), SOUND::LANDEN);

    for (int i = 0; i < NUMPLAYERS; i++)
        if (Player[i].AufPlattform == this)
            Player[i].AufPlattform = nullptr;
}
