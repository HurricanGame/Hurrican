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
    Handlung = GEGNER::INIT;
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
    bool mirror =  (BlickRichtung == DirectionEnum::LINKS);

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
    FlameCount -= Timer.sync(1.0f);
    if (FlameCount < 0.0f) {
        FlameCount = 0.5f;
        Projectiles.PushProjectile(xPos - 10.0f - xSpeed * 0.75f, yPos, FEUERFALLE_LAVAMANN);
    }

    blocku = TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

    // In Richtung Spieler laufen, wenn angeschossen
    //
    if (DamageTaken > 0) {
        if (pAim->xpos < xPos) {
            BlickRichtung = DirectionEnum::LINKS;
            xSpeed = -24.0f;
        } else {
            BlickRichtung = DirectionEnum::RECHTS;
            xSpeed = 24.0f;
        }
    }

    // runterfallen?
    //
    if (Handlung != GEGNER::STEHEN) {
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
            if (BlickRichtung == DirectionEnum::LINKS)
                if (blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND) {
                    BlickRichtung = DirectionEnum::RECHTS;
                    xSpeed *= -1.0f;
                }

            if (BlickRichtung == DirectionEnum::RECHTS)
                if (blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND) {
                    BlickRichtung = DirectionEnum::LINKS;
                    xSpeed *= -1.0f;
                }
        }
    }

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        // Zum ersten mal im Screen?
        // Dann Sound ausgeben
        //
        case GEGNER::INIT: {
            SoundManager.PlayWave(100, 128, 10000 + GetRandom(1000), SOUND::FEUERFALLE);
            Handlung = GEGNER::LAUFEN;
        } break;

        case GEGNER::FALLEN:  // Normal runterfallen
        {
            // Keine zu hohe Geschwindigkeit
            if (ySpeed > 25.0f)
                ySpeed = 25.0f;

            // Testen, ob der Lavamann auf den Boden kommt
            if (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM) {
                Handlung = GEGNER::LAUFEN;
                yAcc = 0.0f;
                ySpeed = 0.0f;
            }
        } break;

        default:
            break;
    }  // switch

    // Testen, ob der Spieler den Lavamann berührt hat
    if (Handlung != GEGNER::WATSCHELN && Handlung != GEGNER::SPRINGEN)
        TestDamagePlayers(35.0f, true);
}

// --------------------------------------------------------------------------------------
// Lavamann explodiert
// --------------------------------------------------------------------------------------

void GegnerLavamann::GegnerExplode() {
    for (int i = 0; i < 8; i++) {
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(GetRandom(40)),
                                    yPos - 30.0f + static_cast<float>(GetRandom(50)), SMOKEBIG);

        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(GetRandom(40)),
                                    yPos - 10.0f + static_cast<float>(GetRandom(50)), SMOKE3);

        PartikelSystem.PushPartikel(xPos - 60.0f + static_cast<float>(GetRandom(80)),
                                    yPos - 50.0f + static_cast<float>(GetRandom(60)), EXPLOSIONFLARE);

        PartikelSystem.PushPartikel(xPos - 40.0f + static_cast<float>(GetRandom(40)),
                                    yPos - 30.0f + static_cast<float>(GetRandom(50)), EXPLOSION_MEDIUM2);

        PartikelSystem.PushPartikel(xPos - 20.0f + static_cast<float>(GetRandom(50)),
                                    yPos - 20.0f + static_cast<float>(GetRandom(50)), MINIFLARE);
    }

    SoundManager.PlayWave(100, 128, -GetRandom(2000) + 11025, SOUND::EXPLOSION1);  // Sound ausgeben

    Player[0].Score += 350;
}
