// --------------------------------------------------------------------------------------
// Der Mutant, der aus der Röhre kam... Ein Film von Poke53280
//
// Kann wahlweise ins Level gesetzt werden oder kommt aus der Röhre, wenn
// der Spieler nahe genug dransteht
// --------------------------------------------------------------------------------------

#include "Gegner_Mutant.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerMutant::GegnerMutant(int Wert1, int Wert2, bool Light) {
    //Handlung = GEGNER::STEHEN;
    HitSound = 1;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    Energy = 100;
    TestBlock = true;
    JumpDelay = 0;

    if (Value1 == 0) {
        Handlung = GEGNER::LAUFEN;
        AnimSpeed = 0.8f;
        AnimStart = 4;
        AnimPhase = 4;
        AnimEnde = 17;
    } else {
        Handlung = GEGNER::EINFLIEGEN;
        AnimSpeed = 3.0f;
        AnimStart = 1;
        AnimPhase = 1;
        AnimEnde = 99;

        ySpeed = 2.0f;
        yAcc = 3.0f;
    }

    zRot = 0.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerMutant::DoKI() {
    switch (Handlung) {
        case GEGNER::LAUFEN: {
            if (PlayerAbstandHoriz() > 20) {
                SimpleAnimation();

                // in Richtung Spieler kriechen
                if (xPos + 50 < pAim->xpos + 35)
                    BlickRichtung = DirectionEnum::RECHTS;
                else
                    BlickRichtung = DirectionEnum::LINKS;

                xSpeed = 5.0f * Direction::asInt(BlickRichtung);
            } else {
                xSpeed = 0.0f;
                AnimPhase = 3;
            }

            // Runterfallen
            if (!(blocku & BLOCKWERT_WAND) && !(blocku & BLOCKWERT_PLATTFORM)) {
                yAcc = 5.0f;
            } else {
                ySpeed = 0.0f;
                yAcc = 0.0f;

                if (PlayerAbstand() < 400.0f)
                    JumpDelay -= Timer.sync(1.0f);

                if (JumpDelay <= 0.0f) {
                    Handlung = GEGNER::SPRINGEN;
                    xSpeed = 15.0f * Direction::asInt(BlickRichtung);
                    AnimStart = 18;
                    AnimPhase = 18;
                    AnimEnde = 26;
                    AnimSpeed = 1.5f;
                    JumpDelay = 8.0f + random(5);
                    ySpeed = -30.0f;
                    yAcc = 6.0f;

                    // DKS - Sound was barely audible with volume of 10, pretty sure this was a typo..
                    //      I went ahead and added 3D panning effect while fixing volume problem:
                    // SoundManager.PlayWave(10, 128, 12000 + rand()%2000, SOUND::MUTANT);
                    SoundManager.PlayWave3D(static_cast<int>(xPos) + 50, static_cast<int>(yPos) + 45, 12000 + random(2000),
                                            SOUND::MUTANT);
                }
            }

        } break;

        case GEGNER::SPRINGEN: {
            if (AnimPhase < 24)
                SimpleAnimation();
            else {
                AnimPhase = 24;
                yAcc = 15.0f;
            }

            // wieder laufen?
            if (ySpeed > 0.0f && (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM)) {
                Handlung = GEGNER::LAUFEN;
                AnimSpeed = 0.8f;
                AnimStart = 4;
                AnimPhase = 4;
                AnimEnde = 17;
            }
        } break;

        case GEGNER::EINFLIEGEN: {
            SimpleAnimation();

            // Am Boden kurz warten
            if (AnimPhase > 2)
                AnimSpeed = 4.0f;

            // Aufgekommen?
            if (blocku & BLOCKWERT_WAND && ySpeed > 0.0f) {
                // DKS - While fixing volume of jumping (look at note further above), made this 3D too:
                // SoundManager.PlayWave(100, 128, 11025, SOUND::SCHLEIM);
                SoundManager.PlayWave3D(static_cast<int>(xPos) + 50,
                                        static_cast<int>(yPos) + 45, 11025, SOUND::SCHLEIM);
                ySpeed = 0.0f;
                yAcc = 0.0f;
            }

            // Jetzt loskrabbeln
            if (AnimPhase > 3) {
                Handlung = GEGNER::LAUFEN;
                AnimSpeed = 0.8f;
                AnimStart = 4;
                AnimPhase = 4;
                AnimEnde = 17;
            }
        } break;
    }

    if (Handlung != GEGNER::EINFLIEGEN)
        TestDamagePlayers(Timer.sync(6.0f));
}

// --------------------------------------------------------------------------------------
// Mutant explodiert
// --------------------------------------------------------------------------------------

void GegnerMutant::GegnerExplode() {
    for (int i = 0; i < 10; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(60)),
                                    yPos + static_cast<float>(random(60)), EXPLOSION_GREEN);

    for (int i = 0; i < 10; i++)
        PartikelSystem.PushPartikel(xPos + 15.0f + static_cast<float>(random(60)),
                                    yPos + 10.0f + static_cast<float>(random(60)), SPIDERSPLITTER);

    for (int i = 0; i < 15; i++)
        PartikelSystem.PushPartikel(xPos + 15.0f + static_cast<float>(random(60)),
                                    yPos + 10.0f + static_cast<float>(random(60)), SCHLEIM);

    // DKS - While fixing volume of jumping (look at note further above), made this 3D too:
    // SoundManager.PlayWave(75, 128, 8000 + rand()%4000, SOUND::MUTANT);
    SoundManager.PlayWave3D(static_cast<int>(xPos) + 50,
                            static_cast<int>(yPos) + 45, 8000 + random(4000),
                            SOUND::MUTANT);

    SoundManager.PlayWave(40, 128, 11025, SOUND::EXPLOSION4);
    SoundManager.PlayWave(40, 128, 6000 + random(4000), SOUND::SCHLEIM);

    Player[0].Score += 500;
}
