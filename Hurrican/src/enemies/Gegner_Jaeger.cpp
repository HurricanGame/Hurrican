// --------------------------------------------------------------------------------------
// Der Abfangjaeger
//
// flieget erst im Hintergrund vorbei und saust dann von oben runter
// --------------------------------------------------------------------------------------

#include "Gegner_Jaeger.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerJaeger::GegnerJaeger(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::INIT;
    ySpeed = -20.0f;
    Energy = 120;
    ChangeLight = Light;
    Destroyable = false;
    Value1 = Wert1;
    Value2 = Wert2;
    OwnDraw = true;
    AnimEnde = 15;
    AnimStart = 0;
    AnimSpeed = 0.5f;
    smokeDelay = 0.1f;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerJaeger::DoDraw() {
    // beim Hoch fliegen
    //
    if (Handlung == GEGNER::DREHEN)
        pGegnerGrafix[GegnerArt]->RenderSpriteScaled(xPos - TileEngine.XOffset,
                                                     yPos - TileEngine.YOffset, 35, 26, AnimPhase,
                                                     0xFFFFFFFF);
    // und beim runterfliegen
    //
    else if (Handlung == GEGNER::DREHEN2)
        pGegnerGrafix[GegnerArt]->RenderSpriteRotated(xPos - TileEngine.XOffset,
                                                      yPos - TileEngine.YOffset, 180, AnimPhase,
                                                      0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerJaeger::DoKI() {
    SimpleAnimation();

    switch (Handlung) {
        // Jaeger wird "aktiviert" (unter den Hurri gesetzt, damit er von dort hochfliegen kann)
        //
        case GEGNER::INIT: {
            Handlung = GEGNER::DREHEN;
            yPos += 512.0f;
        } break;

        // Gegner fliegt von unten nach oben
        //
        case GEGNER::DREHEN: {
            Destroyable = false;

            // Gegner oben aus dem Screeb raus? Dann kurz in Pausemodus setzen
            //
            if (yPos - TileEngine.YOffset < -53.0f) {
                Handlung = GEGNER::STEHEN;
                smokeDelay = 50.0f;
            }

        } break;

        // Gegner fliegt von oben nach unten
        //
        case GEGNER::DREHEN2: {
            // Gegner unten aus dem Screen raus? Dann explodieren lassen
            //
            if (yPos - TileEngine.YOffset > 480.0f) {
                Energy = 0.0f;
            }

            // Antriebspartikel erzeugen

            smokeDelay -= Timer.sync(1.0f);

            while (smokeDelay <= 0.0f) {
                smokeDelay += 0.1f;

                PartikelSystem.PushPartikel(xPos + 18.0f, yPos - 8.0f, ROCKETSMOKE);
                PartikelSystem.PushPartikel(xPos + 40.0f, yPos - 8.0f, ROCKETSMOKE);
            }

        } break;

        // warten, bis Counter abgelaufen ist, und dann nach unten fliegen
        //
        case GEGNER::STEHEN: {
            smokeDelay -= Timer.sync(10.0f);

            if (smokeDelay <= 0.0f) {
                smokeDelay = 0.1f;
                Destroyable = true;
                ySpeed = 22.0f;
                Handlung = GEGNER::DREHEN2;
            }

        } break;

        default:
            break;
    }  // switch

    // Spieler getroffen beim runterfliegen?
    // Dann explodieren lassen und Energy abziehen
    //
    if (Handlung == GEGNER::DREHEN2)
        TestDamagePlayers(50.0f, true);
}

// --------------------------------------------------------------------------------------
// Jaeger explodiert
// --------------------------------------------------------------------------------------

void GegnerJaeger::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION1);

    for (int i = 0; i < 6; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(30)),
                                    yPos - 8.0f + static_cast<float>(random(30)), EXPLOSION_MEDIUM2);

    for (int i = 0; i < 20; i++) {
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(30)),
                                    yPos + static_cast<float>(random(64)), MINIFLARE);
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(25)),
                                    yPos + static_cast<float>(random(50)), SPIDERSPLITTER);
    }

    Player[0].Score += 100;
}
