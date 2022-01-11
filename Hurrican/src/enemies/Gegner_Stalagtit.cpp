// --------------------------------------------------------------------------------------
// Der Stalagtit
//
// Wartet wie der fallende Stein, bis der Spieler drunter vorbei läuft, und fällt dann
// runter
// --------------------------------------------------------------------------------------

#include "Gegner_Stalagtit.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerStalagtit::GegnerStalagtit(int Wert1, int Wert2, bool Light) {
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

void GegnerStalagtit::DoKI() {
    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::FALLEN:  // Stein fällt runter
        {
            // Maximale Geschwindigkeitsbegrenzung
            if (ySpeed > 50.0f)
                yAcc = 0.0f;

            TestDamagePlayers(50.0f, true);

            // Stein ist am Boden gelandet
            if (blocku & BLOCKWERT_WAND) {
                Energy = 0;
            }
        } break;

        case GEGNER::LAUFEN:  // Stein wartet bis der Spieler vorbeikommt
        {
            if (pAim->xpos + pAim->CollideRect.right > xPos && pAim->xpos < xPos + 80 && pAim->ypos > yPos &&
                PlayerAbstand() < 640) {
                // Partikel erzeugen wo der Stein die Decke verlässt
                for (int i = 0; i < 3; i++) {
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(i * 10),
                                                yPos + 30.0f, ROCKSPLITTERSMALLBLUE);
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
// Stalagtit explodiert
// --------------------------------------------------------------------------------------

void GegnerStalagtit::GegnerExplode() {
    // und Splitter erzeugen Rauch
    for (int i = 0; i < 20; i++) {
        PartikelSystem.PushPartikel(xPos - 8.0f + static_cast<float>(random(40)),
                                    yPos - 8.0f + static_cast<float>(random(80)), SMOKE);
        PartikelSystem.PushPartikel(xPos - 8.0f + static_cast<float>(random(40)),
                                    yPos - 8.0f + static_cast<float>(random(80)), ROCKSPLITTERSMALLBLUE);
    }
    SoundManager.PlayWave(100, 128, 11025 + random(2000), SOUND::STONEEXPLODE);  // Sound ausgeben

    Player[0].Score += 100;
}
