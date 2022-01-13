// --------------------------------------------------------------------------------------
// Der Walker
//
// Läuft bis er an eine Wand kommt und kehrt dann um und schiesst, sobald er einen
// bestimmten Abstand zum Spiel hat, geradeaus, läuft einen Schritt weiter, schiesst
// wieder usw
// --------------------------------------------------------------------------------------

#include "Gegner_Walker.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerWalker::GegnerWalker(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    AnimStart = 0;
    AnimEnde = 11;
    AnimSpeed = 0.5f;
    xSpeed = 10.0f;
    ySpeed = 0.0f;
    xAcc = 0.0f;
    yAcc = 0.0f;
    Energy = 10;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    ShotDelay = 0.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerWalker::DoKI() {
    SimpleAnimation();

    // Nach links bzw rechts auf Kollision prüfen und dann ggf umkehren
    if (BlickRichtung == DirectionEnum::LINKS)
        if (blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND) {
            BlickRichtung = DirectionEnum::RECHTS;
            xSpeed = 10.0f;

            if (Handlung == GEGNER::WATSCHELN)
                Energy = 0;
        }

    if (BlickRichtung == DirectionEnum::RECHTS)
        if (blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND) {
            BlickRichtung = DirectionEnum::LINKS;
            xSpeed = -10.0f;

            if (Handlung == GEGNER::WATSCHELN)
                Energy = 0;
        }

    blocku = TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

    // In Richtung Spieler laufen, wenn angeschossen
    //
    if (DamageTaken > 0 && Handlung != GEGNER::WATSCHELN && Handlung != GEGNER::SPRINGEN) {
        if (pAim->xpos < xPos) {
            BlickRichtung = DirectionEnum::LINKS;
            xSpeed = -10.0f;
        } else {
            BlickRichtung = DirectionEnum::RECHTS;
            xSpeed = 10.0f;
        }
    }

    // Spieler kann Walker auf die Birne hopsen
    //
    if (Handlung != GEGNER::WATSCHELN && Handlung != GEGNER::SPRINGEN)
        PlattformTest(GegnerRect[GegnerArt]);

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::LAUFEN:  // Normal laufen und dabei ab und zu schiessen
        {
            // Testen, ob der Walker runterfällt
            if (!(blocku & BLOCKWERT_WAND) && !(blocku & BLOCKWERT_PLATTFORM)) {
                Handlung = GEGNER::FALLEN;
                yAcc = 4.0f;
            }

            // Bei bestimmten Mindestabstand schiessen lassen
            if (PlayerAbstand() <= 220 && ((BlickRichtung == DirectionEnum::LINKS && pAim->xpos + 45 <= xPos) ||
                                           (BlickRichtung == DirectionEnum::RECHTS && pAim->xpos - 45 >= xPos))) {
                ShotDelay -= Timer.sync(1.0f);

                if (ShotDelay <= 0.0f) {
                    ShotDelay = static_cast<float>(10 + random(5));
                    AnimStart = 12;
                    AnimPhase = 12;
                    AnimEnde = 20;
                    xSpeed = 0.0f;
                    Handlung = GEGNER::SCHIESSEN;
                }
            }
        } break;

        case GEGNER::SCHIESSEN:  // gegner schiesst auf den Spieler und läuft dann
        {
            xSpeed = 0.0f;

            if (AnimPhase == AnimStart &&  // Weiterlaufen
                AnimCount == 0.0f) {
                Handlung = GEGNER::LAUFEN;
                AnimStart = 0;
                AnimPhase = 0;
                AnimEnde = 11;
                xSpeed = static_cast<float>(10 * Direction::asInt(BlickRichtung));
            }

            // Schuss abgeben
            if (AnimPhase == 17 && AnimCount == 0.0f) {
                SoundManager.PlayWave(100, 128, 18000 + random(2000), SOUND::LASERSHOT);

                if (BlickRichtung == DirectionEnum::LINKS)
                    Projectiles.PushProjectile(xPos - 18.0f, yPos + 23.0f, WALKER_LASER);
                else
                    Projectiles.PushProjectile(xPos + 30.0f, yPos + 23.0f, WALKER_LASER2);
            }
        } break;

        case GEGNER::WATSCHELN:  // Walker ist getroffen und haut ab
        {
            // Testen, ob der Walker runterfällt
            if (!(blocku & BLOCKWERT_WAND) && !(blocku & BLOCKWERT_PLATTFORM)) {
                Handlung = GEGNER::SPRINGEN;
                yAcc = 3.0f;
            }
        } break;

        case GEGNER::FALLEN:  // Normal runterfallen
        {
            // Keine zu hohe Geschwindigkeit
            if (ySpeed > 25.0f)
                ySpeed = 25.0f;

            // Testen, ob der Walker auf den Boden kommt
            if (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM) {
                Handlung = GEGNER::LAUFEN;
                yAcc = 0.0f;
                ySpeed = 0.0f;
            }
        } break;

        case GEGNER::SPRINGEN:  // Getroffen fallen
        {
            // Keine zu hohe Geschwindigkeit
            if (ySpeed > 30.0f)
                ySpeed = 30.0f;

            // Testen, ob der Walker auf den Boden kommt
            if (ySpeed > 0.0f && (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM)) {
                Handlung = GEGNER::WATSCHELN;
                yAcc = 0.0f;
                ySpeed = 0.0f;
                xSpeed = static_cast<float>(25 * Direction::asInt(BlickRichtung));
            }
        } break;

        default:
            break;
    }  // switch

    // Spieler kann dem Walker auf den Kopf springen
    for (int i = 0; i < NUMPLAYERS; i++)
        if (Player[i].Handlung != PlayerActionEnum::RADELN &&
                Player[i].Handlung != PlayerActionEnum::RADELN_FALL &&
                Player[i].yspeed >= 0.0f) {
            if (Player[i].AufPlattform == this) {
                // Spieler springen lassen
                Player[i].AufPlattform = nullptr;
                Player[i].JumpPossible = false;
                Player[i].AnimPhase = 2;
                Player[i].Handlung = PlayerActionEnum::SPRINGEN;
                Player[i].JumpStart = Player[i].ypos;
                Player[i].yspeed = -PLAYER_MAXJUMPSPEED;
                Player[i].JumpAdd = 0.0f;

                AnimSpeed = 0.3f;
                AnimPhase = 20;
                AnimStart = 20;
                AnimEnde = 31;
                Handlung = GEGNER::SPRINGEN;

                xSpeed = 0.0f;
                ySpeed = -30.0f;
                yAcc = 5.0f;

                SoundManager.PlayWave(100, 128, 11025, SOUND::WALKERGIGGLE);
                blocku = TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
                yPos -= 5.0f;
            }
        }

    // Testen, ob der Spieler den Walker berührt hat
    if (Handlung != GEGNER::WATSCHELN && Handlung != GEGNER::SPRINGEN)
        TestDamagePlayers(Timer.sync(4.0f), false);
}

// --------------------------------------------------------------------------------------
// Walker explodiert
// --------------------------------------------------------------------------------------

void GegnerWalker::GegnerExplode() {
    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(xPos - 20.0f + static_cast<float>(random(45)),
                                    yPos - 20.0f + static_cast<float>(random(45)), EXPLOSION_MEDIUM2);

    SoundManager.PlayWave(100, 128, -random(2000) + 11025, SOUND::EXPLOSION1);  // Sound ausgeben

    Player[0].Score += 100;
}
