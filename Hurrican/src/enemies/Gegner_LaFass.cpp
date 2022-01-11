// --------------------------------------------------------------------------------------
// Das Fass
//
// Gondelt hin und her, und lässt sich vom Spieler abschiessen
// --------------------------------------------------------------------------------------

#include "Gegner_LaFass.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerLaFass::GegnerLaFass(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::INIT;
    Energy = 1.0f;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    AnimPhase = random(15);
    AnimSpeed = 1.2f;
    AnimEnde = 16;
    Active = true;
    TestBlock = true;

    xSpeed = static_cast<float>(Value1);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerLaFass::DoKI() {
    SimpleAnimation();
    BlickRichtung = LINKS;

    switch (Handlung) {
        case GEGNER::LAUFEN: {
            if ((blockl & BLOCKWERT_WAND) || (blockr & BLOCKWERT_WAND)) {
                Energy = 0.0f;
                Value2 = 99;
            }

        } break;

        case GEGNER::INIT:
            Handlung = GEGNER::LAUFEN;
            break;
    }
}

// --------------------------------------------------------------------------------------
// LaFass explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerLaFass::GegnerExplode() {
    // wurde abgeschossen? Dann Fass fallen lassen
    //
    if (Value2 != 99) {
        WinkelUebergabe = xSpeed;

        // Kettenteile
        PartikelSystem.PushPartikel(xPos, yPos + 78.0f, KETTENTEILE);
        PartikelSystem.PushPartikel(xPos + 39.0f, yPos + 78.0f, KETTENTEILE2);
        PartikelSystem.PushPartikel(xPos + 16.0f, yPos + 43.0f, KETTENTEILE3);

        for (int i = 0; i < 3; i++)
            PartikelSystem.PushPartikel(xPos + 33.0f,
                                        yPos + static_cast<float>(i * 18), KETTENTEILE4);

        // Fass
        Projectiles.PushProjectile(xPos + 7.0f, yPos + 90.0f, LAFASSSHOT);
    }
}
