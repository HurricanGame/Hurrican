// --------------------------------------------------------------------------------------
// Der Luke ... SkiWalker
//
// Rast auf Skiern durch die Gegend
// --------------------------------------------------------------------------------------

#include "Gegner_SkiWalker.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSkiWalker::GegnerSkiWalker(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 25;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    xSpeed = 20.0f;
    ySpeed = 0.0f;
    yAcc = 0.0f;
    AnimSpeed = 1.0f;
    AnimEnde = 11;
    ShotDelay = 0.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSkiWalker::DoKI() {
    SimpleAnimation();

    // And der Wand jeweils umdrehen
    if ((blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND) && xSpeed > 0.0f) {
        if (Handlung == GEGNER::WATSCHELN && blockr & BLOCKWERT_WAND)
            Energy = 0;

        BlickRichtung = LINKS;
        xSpeed = -xSpeed;
    }

    if ((blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND) && xSpeed < 0.0f) {
        if (Handlung == GEGNER::WATSCHELN && blockl & BLOCKWERT_WAND)
            Energy = 0;

        BlickRichtung = RECHTS;
        xSpeed = -xSpeed;
    }

    // Als Plattform dienen da man ja draufhopfen kann (man kann sogar darauf 'reiten')
    PlattformTest(GegnerRect[GegnerArt]);

    // Spieler draufgehüpft ?
    for (int i = 0; i < NUMPLAYERS; i++)
        if (Player[i].AufPlattform == this && (Handlung == GEGNER::LAUFEN || Handlung == GEGNER::FALLEN) &&
                Player[i].Handlung != PlayerActionEnum::RADELN &&
                Player[i].Handlung != PlayerActionEnum::RADELN_FALL &&
                Player[i].Handlung != PlayerActionEnum::SACKREITEN &&
                Player[i].Handlung != PlayerActionEnum::DREHEN && Player[i].yspeed > 0.0f) {
            // Spieler springen lassen
            Player[i].JumpPossible = false;
            Player[i].AnimPhase = 0;
            Player[i].Handlung = PlayerActionEnum::SPRINGEN;
            Player[i].JumpStart = Player[i].ypos;
            Player[i].yspeed = -PLAYER_MAXJUMPSPEED;
            Player[i].JumpAdd = 0.0f;

            AnimSpeed = 0.4f;
            Handlung = GEGNER::SPRINGEN;

            xSpeed = 0.0f;
            ySpeed = -30.0f;
            yAcc = 5.0f;

            SoundManager.PlayWave(100, 128, 11025, SOUND::WALKERGIGGLE);
        }

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::LAUFEN:  // Durch die Gegend rasen
        {
            // Runterfallen
            if (!(blocku & BLOCKWERT_WAND) && !(blocku & BLOCKWERT_PLATTFORM)) {
                yAcc = 5.0f;
                yPos--;
                Handlung = GEGNER::FALLEN;
            }

            // Bei bestimmten Mindestabstand schiessen lassen
            if (PlayerAbstand() <= 220 && ((BlickRichtung == LINKS && pAim->xpos + 45 <= xPos) ||
                                           (BlickRichtung == RECHTS && pAim->xpos - 45 >= xPos))) {
                ShotDelay -= 1.0f SYNC;

                if (ShotDelay <= 0.0f) {
                    ShotDelay = static_cast<float>(10 + random(5));
                    SoundManager.PlayWave(100, 128, 18000 + random(2000), SOUND::LASERSHOT);

                    if (BlickRichtung == LINKS)
                        Projectiles.PushProjectile(xPos - 18.0f, yPos + 9.0f, WALKER_LASER);
                    else
                        Projectiles.PushProjectile(xPos + 40.0f, yPos + 9.0f, WALKER_LASER2);
                }
            }
        } break;

        case GEGNER::FALLEN: {
            // Gelandet
            if (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM) {
                if (ySpeed > 0.0f)
                    ySpeed = -ySpeed / 2;
                if (ySpeed > -2.0f) {
                    TileEngine.BlockUnten(xPos, yPos, xSpeed, ySpeed, GegnerRect[GegnerArt]);
                    ySpeed = 0.0f;
                    yAcc = 0.0f;
                    Handlung = GEGNER::LAUFEN;
                }
            }
        } break;

        case GEGNER::SPRINGEN:  // Getroffen fallen
        {
            // Keine zu hohe Geschwindigkeit
            if (ySpeed > 38.0f)
                ySpeed = 38.0f;

            // Testen, ob der Walker auf den Boden kommt
            if (ySpeed > 0.0f && (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM)) {
                Handlung = GEGNER::WATSCHELN;
                yAcc = 0.0f;
                ySpeed = 0.0f;
                xSpeed = 43.0f * BlickRichtung;
                TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
            }
        } break;

        case GEGNER::WATSCHELN:  // Walker ist getroffen und haut ab
        {
            if (!(blocku & BLOCKWERT_WAND) && !(blocku & BLOCKWERT_PLATTFORM)) {
                yAcc = 1.6f;
                Handlung = GEGNER::SPRINGEN;
            }
        } break;

        default:
            break;
    }  // switch

    // Testen, ob der Spieler den Walker berührt hat
    if (Handlung != GEGNER::WATSCHELN && Handlung != GEGNER::SPRINGEN)
        TestDamagePlayers(4.0f SYNC, false);
}

// --------------------------------------------------------------------------------------
// SkiWalker explodiert
// --------------------------------------------------------------------------------------

void GegnerSkiWalker::GegnerExplode() {
    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(xPos - 20.0f + static_cast<float>(random(45)),
                                    yPos - 20.0f + static_cast<float>(random(45)), EXPLOSION_MEDIUM2);

    SoundManager.PlayWave(100, 128, -random(2000) + 11025, SOUND::EXPLOSION1);  // Sound ausgeben

    Player[0].Score += 100;

    for (int i = 0; i < NUMPLAYERS; i++)
        if (Player[i].AufPlattform == this)
            Player[i].AufPlattform = nullptr;
}
