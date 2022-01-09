// --------------------------------------------------------------------------------------
// Der Lavamann
//
// Rennt auf den Hurri zu und explodiert dann, zieht eine Feuerspur hinter sich her
// --------------------------------------------------------------------------------------

#include "Gegner_Lavamann.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerLavamann::GegnerLavamann(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_INIT;
    AnimStart = 0;
    AnimEnde = 20;
    AnimSpeed = 0.15f;
    xSpeed = 24.0f;
    ySpeed = 0.0f;
    Energy = 20;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = false;
    Destroyable = true;
    OwnDraw = true;
    FlameCount = 0.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerLavamann::DoDraw() {
    bool mirror = false;

    if (BlickRichtung == LINKS)
        mirror = true;
    else
        mirror = false;

    // gegner leuchtend rendern
    //
    DirectGraphics.SetAdditiveMode();
    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                           yPos - TileEngine.YOffset, AnimPhase, 0xFFFFFFFF,
                                           mirror);
    DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerLavamann::DoKI() {
    SimpleAnimation();

    // Schrägen checken
    //
    if (ySpeed >= 0.0f)
        // DKS - Rewrote BlockSlopes function to only take the parameters it uses:
        // blocku = TileEngine.BlockSlopes(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt], ySpeed);
        blocku = TileEngine.BlockSlopes(xPos, yPos, GegnerRect[GegnerArt], ySpeed);

    // Flammenwand entstehen lassen
    //
    FlameCount -= 1.0f SYNC;
    if (FlameCount < 0.0f) {
        FlameCount = 0.5f;
        Projectiles.PushProjectile(xPos - 10 - xSpeed * 0.75f, yPos, FEUERFALLE_LAVAMANN);
    }

    blocku = TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

    // In Richtung Spieler laufen, wenn angeschossen
    //
    if (DamageTaken > 0) {
        if (pAim->xpos < xPos) {
            BlickRichtung = LINKS;
            xSpeed = -24.0f;
        } else {
            BlickRichtung = RECHTS;
            xSpeed = 24.0f;
        }
    }

    // runterfallen?
    //
    if (Handlung != GEGNER_STEHEN) {
        if (!(blocku & BLOCKWERT_SCHRAEGE_R) && !(blocku & BLOCKWERT_SCHRAEGE_L)) {
            blocku = TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

            if (!(blocku & BLOCKWERT_WAND) && !(blocku & BLOCKWERT_PLATTFORM)) {
                if (ySpeed == 0.0f)
                    ySpeed = 5.0f;

                yAcc = 3.0f;
            }

            else {
                ySpeed = 0.0f;
                yAcc = 0.0f;
            }

            // Nach links bzw rechts auf Kollision prüfen und ggf umdrehen
            if (BlickRichtung == LINKS)
                if (blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND) {
                    BlickRichtung = RECHTS;
                    xSpeed *= -1.0f;
                }

            if (BlickRichtung == RECHTS)
                if (blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND) {
                    BlickRichtung = LINKS;
                    xSpeed *= -1.0f;
                }
        }
    }

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        // Zum ersten mal im Screen?
        // Dann Sound ausgeben
        //
        case GEGNER_INIT: {
            SoundManager.PlayWave(100, 128, 10000 + random(1000), SOUND_FEUERFALLE);
            Handlung = GEGNER_LAUFEN;
        } break;

        case GEGNER_FALLEN:  // Normal runterfallen
        {
            // Keine zu hohe Geschwindigkeit
            if (ySpeed > 25.0f)
                ySpeed = 25.0f;

            // Testen, ob der Lavamann auf den Boden kommt
            if (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM) {
                Handlung = GEGNER_LAUFEN;
                yAcc = 0.0f;
                ySpeed = 0.0f;
            }
        } break;

        default:
            break;
    }  // switch

    // Testen, ob der Spieler den Lavamann berührt hat
    if (Handlung != GEGNER_WATSCHELN && Handlung != GEGNER_SPRINGEN)
        TestDamagePlayers(35.0f, true);
}

// --------------------------------------------------------------------------------------
// Lavamann explodiert
// --------------------------------------------------------------------------------------

void GegnerLavamann::GegnerExplode() {
    for (int i = 0; i < 8; i++) {
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(40) - 30),
                                    yPos + static_cast<float>(random(50) - 30), SMOKEBIG);

        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(40) - 10),
                                    yPos + static_cast<float>(random(50) - 10), SMOKE3);

        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(80) - 60),
                                    yPos + static_cast<float>(random(60) - 50), EXPLOSIONFLARE);

        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(40) - 40),
                                    yPos + static_cast<float>(random(50) - 30), EXPLOSION_MEDIUM2);

        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(50) - 20),
                                    yPos + static_cast<float>(random(50) - 20), MINIFLARE);
    }

    SoundManager.PlayWave(100, 128, -random(2000) + 11025, SOUND_EXPLOSION1);  // Sound ausgeben

    Player[0].Score += 350;
}
