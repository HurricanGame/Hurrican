// --------------------------------------------------------------------------------------
// Der Wandkrabbler rechts
//
// krabbelt an der Wand entlang bis er auf eine (Gegner) Wand stösst und dreht dann um
// Schiesst in gleichen Abständen zur Seite
// Hat Speed == Value2 und SchussDelay == Value1
// --------------------------------------------------------------------------------------

#include "Gegner_KrabblerRechts.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerKrabblerRechts::GegnerKrabblerRechts(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 75;
    Value1 = Wert1;
    Value2 = Wert2;
    ySpeed = static_cast<float>(Value2);
    BlickRichtung = RECHTS;
    ShotDelay = static_cast<float>(Value1);
    AnimSpeed = 0.5f;
    AnimStart = 0;
    AnimEnde = 9;
    ChangeLight = Light;
    Destroyable = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerKrabblerRechts::DoKI() {
    BlickRichtung = LINKS;

    SimpleAnimation();

    // An der Decke bzw dem Boden
    if (blocko & BLOCKWERT_WAND || blocko & BLOCKWERT_GEGNERWAND)
        ySpeed = static_cast<float>(Value2);

    if (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_GEGNERWAND)
        ySpeed = static_cast<float>(-Value2);

    // Schuss Delay verzögern
    ShotDelay -= 1.0f SYNC;

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::LAUFEN:  // Krabbler krabbelt an der Decke entlang
        {
            // Schiessen ?
            if (ShotDelay <= 0.0f) {
                ShotDelay = static_cast<float>(Value1);
                Handlung = GEGNER::SCHIESSEN;
                Projectiles.PushProjectile(xPos - 24.0f, yPos + 23.0f, KRABBLERLASER3);
                PartikelSystem.PushPartikel(xPos - 12.0f, yPos + 3.0f, LASERFLAME);
                SoundManager.PlayWave3D(static_cast<int>(xPos + 25.0f),
                                        static_cast<int>(yPos + 37.0f), 44100, SOUND::LASERSHOT);
                AnimPhase = 10;
                AnimEnde = 14;
                AnimStart = 0;
                AnimSpeed = 0.75f;
            }
        } break;

        case GEGNER::SCHIESSEN: {
            // Fertig mit schiessen ?
            if (AnimPhase == AnimStart) {
                AnimEnde = 9;
                AnimPhase = 0;
                AnimSpeed = 0.5f;
                Handlung = GEGNER::LAUFEN;
            }
        } break;

        default:
            break;
    }  // switch
}

// --------------------------------------------------------------------------------------
// WandKrabbler explodiert
// --------------------------------------------------------------------------------------

void GegnerKrabblerRechts::GegnerExplode() {
    // Explosion
    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(xPos - 15.0f + static_cast<float>(random(20)),
                                    yPos - 15.0f + static_cast<float>(random(40)), EXPLOSION_MEDIUM3);

    SoundManager.PlayWave(100, 128, -random(2000) + 11025, SOUND::EXPLOSION1);  // Sound ausgeben

    Player[0].Score += 300;
}
