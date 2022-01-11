// --------------------------------------------------------------------------------------
// Die Spitter Bombe
//
// Bombe, aus der der Spitter rauskommt.
// geile Erklärung, oder ? ;)
// --------------------------------------------------------------------------------------

#include "Gegner_Spitterbombe.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSpitterbombe::GegnerSpitterbombe(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 20;
    AnimEnde = 6;
    AnimSpeed = 0.5;
    ChangeLight = Light;
    Destroyable = true;
    ySpeed = 15.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSpitterbombe::DoKI() {
    // Animieren
    //
    AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
    if (AnimCount > AnimSpeed)  // Grenze überschritten ?
    {
        AnimCount = 0;  // Dann wieder auf Null setzen
        AnimPhase++;    // Und nächste Animationsphase

        PartikelSystem.PushPartikel(xPos - 5 + random(10), yPos, SMOKE);

        if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
            AnimPhase = AnimStart;
    }

    // Testen, ob der Spieler die Bombe berührt hat
    //
    TestDamagePlayers(Timer.sync(1.0f));

    // Auf den Boden aufgekommen ? Dann Spitter rauslassen
    if ((blocku & BLOCKWERT_WAND) || (blocku & BLOCKWERT_PLATTFORM)) {
        Energy = 0.0f;
        Gegner.PushGegner(xPos + 4.0f, yPos + 20.0f, SPITTER, 0, 0, false);
    }
}

// --------------------------------------------------------------------------------------
// Bombe explodiert
// --------------------------------------------------------------------------------------

void GegnerSpitterbombe::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);

    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(xPos - 25.0f + static_cast<float>(random(20)),
                                    yPos - 30.0f + static_cast<float>(random(50)), EXPLOSION_MEDIUM2);

    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(20)),
                                    yPos + static_cast<float>(random(40)), SPIDERSPLITTER);

    // Nicht am Boden explodiert ? Dann wurde er abgeschossen
    if (!(blocku & BLOCKWERT_WAND) && !(blocku & BLOCKWERT_PLATTFORM)) {
        Projectiles.PushProjectile(xPos + 10.0f, yPos + 30.0f, SPITTERBOMBESHOTLO);
        Projectiles.PushProjectile(xPos + 10.0f, yPos + 30.0f, SPITTERBOMBESHOTLM);
        Projectiles.PushProjectile(xPos + 10.0f, yPos + 30.0f, SPITTERBOMBESHOTLU);
        Projectiles.PushProjectile(xPos + 10.0f, yPos + 30.0f, SPITTERBOMBESHOTRO);
        Projectiles.PushProjectile(xPos + 10.0f, yPos + 30.0f, SPITTERBOMBESHOTRM);
        Projectiles.PushProjectile(xPos + 10.0f, yPos + 30.0f, SPITTERBOMBESHOTRU);
        Player[0].Score += 200;
    }
}
