// --------------------------------------------------------------------------------------
// Die Rakete, die am Rand hängt
//
// sobald der Hurri in die Nähe kommt, läuft der Time ab. Steht er auf null,
// fliegt sie los
// --------------------------------------------------------------------------------------

#include "Gegner_SideRocket.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSideRocket::GegnerSideRocket(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::INIT;
    Energy = 50;
    ChangeLight = Light;
    Destroyable = true;
    Value1 = Wert1;

    if (Wert2 == 0)
        Value2 = 10;
    else  // 10 = Default Wert für die Verzögerung
        Value2 = Wert2;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSideRocket::DoKI() {
    if (Value1 == 0)
        BlickRichtung = LINKS;
    else
        BlickRichtung = RECHTS;

    switch (Handlung) {
        // Jaeger wird "aktiviert", sprich, der Timer wird gestartet
        //
        case GEGNER::INIT: {
            AnimCount = static_cast<float>(Value2);

            if (PlayerAbstandVert() < 100 && PlayerAbstandHoriz() < 400 &&
                ((Value1 == 0 && xPos > pAim->xpos) || (Value1 != 0 && xPos < pAim->xpos)))
                Handlung = GEGNER::STEHEN;
        } break;

        // Timer läuft ab
        //
        case GEGNER::STEHEN: {
            AnimCount -= 1.0f SYNC;

            // Timer zu ende? Dann Rakete starten
            //
            if (AnimCount <= 0.0f) {
                AnimCount = 0.1f;
                Handlung = GEGNER::LAUFEN;
                SoundManager.PlayWave(75, 128, 8000 + random(4000), SOUND::ROCKET);

                if (Value1 == 0) {
                    xSpeed = -20.0f;
                    xAcc = -5.0f;
                } else {
                    xSpeed = 20.0f;
                    xAcc = 5.0f;
                }
            }

        } break;

        // Rakete fliegt
        //
        case GEGNER::LAUFEN: {
            if (xSpeed < -30.0f)
                xSpeed = -30.0f;
            else if (xSpeed > 30.0f)
                xSpeed = 30.0f;

            // Antriebspartikel erzeugen
            //
            AnimCount -= 1.0f SYNC;

            while (AnimCount <= 0.0f) {
                AnimCount += 0.05f;

                if (Value1 == 0)
                    PartikelSystem.PushPartikel(xPos + 35.0f, yPos + 3.0f, ROCKETSMOKE);
                else
                    PartikelSystem.PushPartikel(xPos - 5.0f, yPos + 3.0f, ROCKETSMOKE);
            }

            // Gegen die Wand geflogen? Dann explodieren lassen
            //
            if ((Value1 == 0 && blockl & BLOCKWERT_WAND) || (Value1 == 1 && blockr & BLOCKWERT_WAND))

                Energy = 0.0f;

        } break;

        default:
            break;
    }  // switch

    // Spieler getroffen?
    // Dann explodieren lassen und Energy abziehen
    //
    TestDamagePlayers(25.0f, true);
}

// --------------------------------------------------------------------------------------
// SideRocket explodiert
// --------------------------------------------------------------------------------------

void GegnerSideRocket::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);

    PartikelSystem.PushPartikel(xPos - 10, yPos - 20, EXPLOSION_MEDIUM2);

    for (int i = 0; i < 10; i++) {
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(20)),
                                    yPos - 10.0f + static_cast<float>(random(20)), MINIFLARE);
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(20)),
                                    yPos - 10.0f + static_cast<float>(random(20)), SPIDERSPLITTER);
    }

    Player[0].Score += 50;
}
