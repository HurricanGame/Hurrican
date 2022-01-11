// --------------------------------------------------------------------------------------
// Der Raketenwerfer
//
// Befindet sich an der Decke oder am Boden und schiesst Raketen wenn der Spieler zu
// nahe kommt
// --------------------------------------------------------------------------------------

#include "Gegner_Raketenwerfer.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerRaketenwerfer::GegnerRaketenwerfer(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 60;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    AnimSpeed = 1.3f;
    AnimEnde = 9;
    TestBlock = false;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerRaketenwerfer::DoKI() {
    // animieren wenn der Spieler in der Nähe ist

    if (PlayerAbstand() < 600 && pAim->ypos > yPos)
        AnimCount += SpeedFaktor;  // Animationscounter weiterzählen

    if (AnimCount > AnimSpeed)  // Grenze überschritten ?
    {
        AnimCount = 0;  // Dann wieder auf Null setzen
        AnimPhase++;    // Und nächste Animationsphase

        // Rakete schiessen
        if (AnimPhase == 6) {
            SoundManager.PlayWave(100, 128, 11025, SOUND::GRANATE);
            Projectiles.PushProjectile(xPos + 2.0f, yPos + 5.0f, ROCKETWERFER, pAim);
        }

        if (AnimPhase > AnimEnde)  // Animation von Vorne ?
        {
            AnimPhase = AnimStart;  // dann von vorne
            AnimCount = -2.0f;      // und kurze Pause
        }
    }
}

// --------------------------------------------------------------------------------------
// Raketenwerfer explodiert
// --------------------------------------------------------------------------------------

void GegnerRaketenwerfer::GegnerExplode() {
    PartikelSystem.PushPartikel(xPos - 20.0f, yPos - 15.0f, EXPLOSION_BIG);

    for (int i = 0; i < 20; i++) {
        PartikelSystem.PushPartikel(xPos - 8.0f + static_cast<float>(random(40)),
                                    yPos - 8.0f + static_cast<float>(random(20)), SMOKE);
    }

    SoundManager.PlayWave(100, 128, 11025 + random(2000), SOUND::EXPLOSION4);  // Sound ausgeben

    Player[0].Score += 400;
}
