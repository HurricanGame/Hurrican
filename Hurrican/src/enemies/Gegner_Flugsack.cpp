// --------------------------------------------------------------------------------------
// Der fliegende Fettsack
//
// Fliegt rum, ist fett und ballert auf den Hurri
// --------------------------------------------------------------------------------------

#include "Gegner_Flugsack.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFlugsack::GegnerFlugsack(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 50;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    xSpeed = 0.0f;
    ySpeed = 0.0f;
    xAcc = 2.0f;
    yAcc = 2.0f;
    AnimSpeed = 1.5f;
    AnimEnde = 10;
    ShotCount = 1.0f;
    SmokeCount = 0.2f;

    if (Value1 == 99)
        Energy = 0.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFlugsack::DoKI() {
    SimpleAnimation();

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::LAUFEN:  // Normal fliegen und dabei ab und zu schiessen
        {
            if (pAim->xpos + 45 < xPos + GegnerRect[GegnerArt].left)
                BlickRichtung = LINKS;

            if (pAim->xpos + 45 > xPos + GegnerRect[GegnerArt].right)
                BlickRichtung = RECHTS;

            // umherfliegen
            if (ySpeed > 6.0f)
                yAcc = -2.0f;
            if (ySpeed < -6.0f)
                yAcc = 2.0f;
            if (xSpeed > 5.0f)
                xAcc = -2.0f;
            if (xSpeed < -5.0f)
                xAcc = 2.0f;

            // Spieler verfolgen
            if (pAim->xpos + 45 < xPos + 40 && !(blockl & BLOCKWERT_WAND))
                xPos -= 4.0f SYNC;
            if (pAim->xpos + 45 > xPos + 40 && !(blockr & BLOCKWERT_WAND))
                xPos += 4.0f SYNC;
            if (pAim->ypos + 45 < yPos + 45 && !(blocko & BLOCKWERT_WAND))
                yPos -= 4.0f SYNC;
            if (pAim->ypos + 45 > yPos + 45 && !(blocku & BLOCKWERT_WAND))
                yPos += 4.0f SYNC;

            if (blocko & BLOCKWERT_WAND || blocko & BLOCKWERT_GEGNERWAND)
                ySpeed = 4.0f;
            if (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_GEGNERWAND)
                ySpeed = -4.0f;
            if (blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND)
                xSpeed = 4.0f;
            if (blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND)
                xSpeed = -4.0f;

            // Rauch ausgeben
            SmokeCount -= 1.0f SYNC;
            if (SmokeCount < 0.0f) {
                SmokeCount += 0.1f;

                if (BlickRichtung == LINKS)
                    PartikelSystem.PushPartikel(xPos + 66.0f, yPos + 50.0f, FLUGSACKSMOKE2);
                else
                    PartikelSystem.PushPartikel(xPos, yPos + 50.0f, FLUGSACKSMOKE);
            }

            // Bei bestimmten Mindestabstand schiessen lassen
            if (PlayerAbstand() <= 400 && ((BlickRichtung == LINKS && pAim->xpos + 45 <= xPos) ||
                                           (BlickRichtung == RECHTS && pAim->xpos - 45 >= xPos))) {
                ShotCount -= 1.0f SYNC;
                if (ShotCount < 0.0f) {
                    SoundManager.PlayWave(100, 128, 11025, SOUND::CANON);

                    if (BlickRichtung == LINKS) {
                        PartikelSystem.PushPartikel(xPos + 10.0f, yPos + 2.0f, SMOKE);
                        Projectiles.PushProjectile(xPos + 17.0f, yPos + 10.0f, SUCHSCHUSS);
                    } else {
                        PartikelSystem.PushPartikel(xPos + 45.0f, yPos + 2.0f, SMOKE);
                        Projectiles.PushProjectile(xPos + 53.0f, yPos + 10.0f, SUCHSCHUSS);
                    }

                    switch (Skill) {
                        case SKILL_EASY:     ShotCount += 20.0f; break;
                        case SKILL_MEDIUM:   ShotCount += 15.0f; break;
                        case SKILL_HARD:     ShotCount += 12.0f; break;
                        case SKILL_HURRICAN: ShotCount +=  8.0f; break;
                    }
                }
            }
        } break;

        // Sack stürzt ab
        case GEGNER::FALLEN: {
            // An die Wand gekracht ?
            if (blockl & BLOCKWERT_WAND || blockr & BLOCKWERT_WAND || blocko & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM)
                Energy = 0.0f;

            // Grenze der Fallgeschwindigkeit
            if (ySpeed > 35.0f)
                yAcc = 0.0f;

            // FlugSack rauchen lassen
            if (AnimCount == 0.0f) {
                PartikelSystem.PushPartikel(xPos + 20.0f + static_cast<float>(random(40)),
                                            yPos + 20.0f + static_cast<float>(random(30)), SMOKE);
            }

            if (PlayerAbstand() <= 600 && AnimCount == 0.0f && AnimPhase % 2 == 0 && random(2) == 0) {
                PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(80)),
                                            yPos - 30.0f + static_cast<float>(random(70)), EXPLOSION_MEDIUM2);
                SoundManager.PlayWave(100, 128, 11025 + random(2000), SOUND::EXPLOSION1);
            }
        } break;

        default:
            break;
    }  // switch

    // Soviel Energie verloren, dass der Spacko abstürzt ?
    if (Energy <= 0.0f && Handlung != GEGNER::FALLEN) {
        Handlung = GEGNER::FALLEN;
        Energy = 40.0f;
        ySpeed = 3.0f;
        yAcc = 2.0f;
        xAcc = 0.0f;
        AnimSpeed = 0.5f;

        if (BlickRichtung == LINKS)
            xSpeed = -5.0f;
        else
            xSpeed = 5.0f;

        if (Value1 == 99)
            xSpeed = 0.0f;
    }

    // Testen, ob der Spieler den Sack berührt hat
    if (Handlung != GEGNER::FALLEN)
        TestDamagePlayers(4.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Explodieren
// --------------------------------------------------------------------------------------

void GegnerFlugsack::GegnerExplode() {
    for (int i = 0; i < 10; i++)
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(80)),
                                    yPos - 30.0f + static_cast<float>(random(70)), EXPLOSION_MEDIUM2);

    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(xPos + 20.0f + static_cast<float>(random(40)),
                                    yPos + 20.0f + static_cast<float>(random(30)), SPLITTER);

    SoundManager.PlayWave(75, 128, 11025 + random(2000), SOUND::EXPLOSION4);  // Sound ausgeben

    Player[0].Score += 225;
}
