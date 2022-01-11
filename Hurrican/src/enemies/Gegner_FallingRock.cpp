// --------------------------------------------------------------------------------------
// Der fallende Felsblock
//
// ruht in der Decke und wartet bis der Spieler drunter durchläuft, umd dann runter
// zu fallen
// --------------------------------------------------------------------------------------

#include "Gegner_FallingRock.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFallingRock::GegnerFallingRock(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 10;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFallingRock::DoKI() {
    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::FALLEN:  // Stein fällt runter
        {
            // Maximale Geschwindigkeitsbegrenzung
            if (ySpeed > 50.0f)
                yAcc = 0.0f;

            // Spieler berührt Stein
            TestDamagePlayers(40.0f, true);

            // Stein ist am Boden gelandet
            if (ySpeed > 10.0f && (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM)) {
                Energy = 0;
            }
        } break;

        case GEGNER::LAUFEN:  // Stein wartet bis der Spieler vorbeikommt
        {
            for (int p = 0; p < NUMPLAYERS; p++)
                if (Player[p].xpos + Player[p].CollideRect.right > xPos && Player[p].xpos < xPos + 80 &&
                    Player[p].ypos > yPos && PlayerAbstand() < 480) {
                    Active = true;

                    // Rauch erzeugen wo der Stein die Decke verlässt
                    for (int i = 0; i < 3; i++) {
                        PartikelSystem.PushPartikel(xPos - 10.0f, yPos + static_cast<float>(i * 10), SMOKE);
                        PartikelSystem.PushPartikel(xPos + 63.0f, yPos + static_cast<float>(i * 10), SMOKE);
                    }

                    // Sound ausgeben
                    SoundManager.PlayWave(100, 128, 11025 + random(2000), SOUND::STONEFALL);

                    Handlung = GEGNER::FALLEN;
                    ySpeed = 0.0f;
                    yAcc = 12.0f;
                }
        } break;

        default:
            break;
    }  // switch
}

// --------------------------------------------------------------------------------------
// FallingRock explodiert
// --------------------------------------------------------------------------------------

void GegnerFallingRock::GegnerExplode() {
    // und Splitter erzeugen Rauch
    for (int i = 0; i < 10; i++) {
        PartikelSystem.PushPartikel(xPos - 12.0f + static_cast<float>(random(80)),
                                    yPos + 20.0f + static_cast<float>(random(20)), SMOKE);
        PartikelSystem.PushPartikel(xPos - 12.0f + static_cast<float>(random(80)),
                                    yPos + static_cast<float>(random(40)), ROCKSPLITTER);
        PartikelSystem.PushPartikel(xPos - 12.0f + static_cast<float>(random(80)),
                                    yPos + static_cast<float>(random(40)), ROCKSPLITTERSMALL);
    }
    SoundManager.PlayWave(100, 128, 11025 + random(2000), SOUND::STONEEXPLODE);  // Sound ausgeben

    Player[0].Score += 100;
}
