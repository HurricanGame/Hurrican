// --------------------------------------------------------------------------------------
// Die Qualle
//
// Schwappt unter wasser rum und schwimmt in Richtung Spieler ;)
// --------------------------------------------------------------------------------------

#include "Gegner_Qualle.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerQualle::GegnerQualle(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::SPRINGEN;
    HitSound = 1;
    Energy = 35;
    Value1 = Wert1;
    Value2 = Wert2;

    AnimSpeed = static_cast<float>(8 + random(3)) / 10.0f;
    AnimPhase = 1;
    AnimStart = 0;
    AnimEnde = 16;
    ChangeLight = Light;
    Destroyable = true;
    ySpeed = -7.0f;
    yAcc = 0.6f;
    ShotDelay = 1.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerQualle::DoKI() {
    if (AnimPhase != AnimStart)
        SimpleAnimation();

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::SPRINGEN:  // Nach oben abgestossen zum Schwimmen
        {
            // Geschwindigkeits-Begrenzung
            if (ySpeed > 0.0f)
                yAcc = 0.25f;

            if (ySpeed > 7.0f)
                yAcc = 0.0f;

            if (AnimPhase == AnimStart)  // Animation von zu Ende	?
            {
                // Dann absinken
                AnimPhase = 16;
                AnimStart = 16;
                AnimEnde = 17;
                AnimSpeed = 4.0f;
                Handlung = GEGNER::LAUFEN;
            }
        } break;

        case GEGNER::LAUFEN:  // Qualle sinkt ab
        {
            if (pAim->InLiquid == true && yPos - 20 > pAim->ypos) {
                // Bläschen erzeugen
                PartikelSystem.PushPartikel(xPos + 20, yPos + 30, BUBBLE);

                // rechts / links bewegen
                if (xPos < pAim->xpos)
                    xSpeed = 1.2f;
                else
                    xSpeed = -1.2f;

                AnimPhase = 0;
                AnimEnde = 16;
                AnimSpeed = static_cast<float>(8 + random(3)) / 10.0f;
                AnimCount = 0.0f;
                ySpeed = -7.0f;
                yAcc = 0.6f;
                Handlung = GEGNER::SPRINGEN;
            } else if ((pAim->InLiquid == false && random(100) == 0) || blocku & BLOCKWERT_WAND ||
                       blocku & BLOCKWERT_GEGNERWAND) {
                xSpeed = 1.2f;
                if (random(2) == 0)
                    xSpeed *= -1;

                AnimPhase = 0;
                AnimEnde = 16;
                AnimSpeed = static_cast<float>(8 + random(3)) / 10.0f;
                AnimCount = 0.0f;
                ySpeed = -7.0f;
                yAcc = 0.6f;
                Handlung = GEGNER::SPRINGEN;
            }
        } break;

        default:
            break;
    }  // switch

    if (!(blocko & BLOCKWERT_LIQUID) && ySpeed < 0.0f) {
        ySpeed = 0.0f;
    }

    if (pAim->InLiquid == true) {
        if (ShotDelay > 0.0f)
            ShotDelay -= 1.0f SYNC;
        else {
            ShotDelay = 20.0f + random(10);
            Projectiles.PushProjectile(xPos + 10.0f, yPos + 10.0f, SUCHSCHUSS);
        }
    }

    // Testen, ob der Spieler die Qualle berührt hat
    TestDamagePlayers(2.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Qualle explodiert
// --------------------------------------------------------------------------------------

void GegnerQualle::GegnerExplode() {
    for (int i = 0; i < 5; i++) {
        // ein paar Luftblässchen erzeugen
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(45)),
                                    yPos + 10.0f + static_cast<float>(random(30)), BUBBLE);
    }

    // Blutwolke dazu
    PartikelSystem.PushPartikel(xPos + 2.0f,
                                yPos - 5.0f, PIRANHABLUT);

    SoundManager.PlayWave(100, 128, -random(2000) + 11025, SOUND::EXPLOSION1);  // Sound ausgeben

    Player[0].Score += 200;
}
