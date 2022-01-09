// --------------------------------------------------------------------------------------
// Die WasserMine
//
// Dümpelt im Wasser rum und wartet auf eine Kollision ;)
// --------------------------------------------------------------------------------------

#include "Gegner_WasserMine.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerWasserMine::GegnerWasserMine(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_LAUFEN;
    Energy = 20;
    AnimSpeed = 3.0f;
    ChangeLight = Light;
    Destroyable = false;
    AnimSpeed = 4.0f;
    AnimEnde = 10;
    Value1 = static_cast<int>(Wert1);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerWasserMine::DoKI() {
    SimpleAnimation();

    // Spieler berührt ?
    TestDamagePlayers(50.0f, true);
}

// --------------------------------------------------------------------------------------
// WasserMine explodiert
// --------------------------------------------------------------------------------------

void GegnerWasserMine::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 8000, SOUND_EXPLOSION3);
    SoundManager.PlayWave(100, 128, 14000, SOUND_EXPLOSION4);

    ShakeScreen(5.0f);

    for (int i = 0; i < 50; i++)
        PartikelSystem.PushPartikel(xPos - 5.0f + static_cast<float>(random(40)),
                                    yPos - 5.0f + static_cast<float>(random(60)), WATERFLUSH_HIGH);

    for (int i = 0; i < 25; i++)
        PartikelSystem.PushPartikel(xPos - 5.0f + static_cast<float>(random(40)),
                                    yPos - 50.0f + static_cast<float>(random(110)), SPIDERSPLITTER);

    PartikelSystem.PushPartikel(xPos - 40.0f, yPos - 55.0f, EXPLOSION_GIANT);
}
