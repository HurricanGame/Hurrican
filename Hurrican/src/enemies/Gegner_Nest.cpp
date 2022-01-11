// --------------------------------------------------------------------------------------
// Das Wespennest
//
// Bringt in unregelmässigen Abständen Wespen hervor
// --------------------------------------------------------------------------------------

#include "Gegner_Nest.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerNest::GegnerNest(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    Energy = 120;
    AnimSpeed = 1.0f;
    ChangeLight = Light;
    Destroyable = true;
    AnimCount = 50.0f;
    Value1 = Wert1;
    Value2 = Wert2;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerNest::DoKI() {
    // Wespe spawnen?
    //

    if (PlayerAbstand() < 800 && Value2 != 99)  // Im Tutorial keine Wespen spawnen
    {
        AnimCount -= 1.0f SYNC;

        if (AnimCount <= 0.0f) {
            Gegner.PushGegner(xPos + 20.0f, yPos - 10.0f, STAHLMUECKE, 0, 0, false);
            AnimCount = static_cast<float>(random(30) + 30);
        }
    }

    // Testen, ob der Spieler das Nest berührt
    //
    TestDamagePlayers(0.5f SYNC);
}

// --------------------------------------------------------------------------------------
// Nest explodiert
// --------------------------------------------------------------------------------------

void GegnerNest::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION4);

    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(xPos - 25.0f + static_cast<float>(random(20)),
                                    yPos - 30.0f + static_cast<float>(random(50)), EXPLOSION_MEDIUM2);

    PartikelSystem.PushPartikel(xPos - 5.0f, yPos - 5.0f, NESTLUFT);

    Player[0].Score += 500;
}
