// --------------------------------------------------------------------------------------
// Der Fiese Walker mit dem Laser
// --------------------------------------------------------------------------------------

#include "Gegner_FieserWalker.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFieserWalker::GegnerFieserWalker(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 100;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    AnimSpeed = 0.5f;
    AnimEnde = 14;
    xSpeed = 5.0f;
    ShotDelay = 0.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFieserWalker::DoKI() {
    SimpleAnimation();

    // Je nach Handlung anders verhalten
    //
    switch (Handlung) {
        // rumhopsen
        //
        case GEGNER::LAUFEN: {
            if (!(blocku & BLOCKWERT_WAND) && !(blocku & BLOCKWERT_PLATTFORM)) {
                Handlung = GEGNER::FALLEN;
                ySpeed = 2.0f;
                yAcc = 3.0f;
            }

            // nahe genug zum schiessen?
            //
            if (ShotDelay <= 0.0f && AnimPhase == 3 && PlayerAbstand() < 400 && PlayerAbstandVert() < 150 &&
                ((BlickRichtung == RECHTS && xPos < pAim->xpos) || (BlickRichtung == LINKS && xPos > pAim->xpos))) {
                ShotDelay = 15.0f;
                Handlung = GEGNER::STEHEN;
                xSpeed = 0.0f;
                AnimEnde = 0;
            }

            if (ShotDelay > 0.0f)
                ShotDelay -= Timer.sync(1.0f);

            TurnonShot();

        } break;

        case GEGNER::STEHEN: {
            if (ShotDelay > 0.0f)
                ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay < 12.0f) {
                Handlung = GEGNER::LAUFEN;
                AnimEnde = 14;
                xSpeed = 5.0f * BlickRichtung;

                SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND::LASERSHOT);

                PartikelSystem.PushPartikel(xPos + 10.0f + static_cast<float>(BlickRichtung * 40),
                                            yPos + 6.0f, LASERFLAME);
                Projectiles.PushProjectile(xPos + 26.0f + static_cast<float>(BlickRichtung * 40),
                                           yPos + 23.0f, SUCHSCHUSS);
            }

        } break;

        case GEGNER::FALLEN: {
            if (ySpeed > 35.0f) {
                ySpeed = 35.0f;
                yAcc = 0.0f;
            }

            if (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM) {
                ySpeed = 0.0f;
                yAcc = 0.0f;
                Handlung = GEGNER::LAUFEN;
            }

        } break;
    }

    TurnonWall();

    // Spieler berührt den Gegner?
    TestDamagePlayers(Timer.sync(4.0f));
}

// --------------------------------------------------------------------------------------
// FieserWalker explodiert
// --------------------------------------------------------------------------------------

void GegnerFieserWalker::GegnerExplode() {
    PartikelSystem.PushPartikel(xPos - 30.0f, yPos - 30.0f, EXPLOSION_BIG);

    for (int i = 0; i < 8; i++)
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(60)),
                                    yPos - 30.0f + static_cast<float>(random(60)), EXPLOSION_MEDIUM2);
    for (int i = 0; i < 12; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(50)),
                                    yPos + static_cast<float>(random(50)), SPIDERSPLITTER);

    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION4);  // Sound ausgeben

    Player[0].Score += 80;
}
