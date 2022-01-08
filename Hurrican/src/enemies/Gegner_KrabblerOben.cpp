// --------------------------------------------------------------------------------------
// Die Deckenkrabbler
//
// krabbelt an der Decke entlang bis er auf eine (Gegner) Wand stösst und dreht dann um
// Schiesst in gleichen Abständen auf den Boden
// Hat Speed == Value2 und SchussDelay == Value1
// --------------------------------------------------------------------------------------

#include "Gegner_KrabblerOben.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerKrabblerOben::GegnerKrabblerOben(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_LAUFEN;
    Energy = 75;
    Value1 = Wert1;
    Value2 = Wert2;
    xSpeed = static_cast<float>(Value2);
    BlickRichtung = LINKS;
    ShotDelay = static_cast<float>(Value1);
    AnimSpeed = 0.5f;
    AnimStart = 0;
    AnimEnde = 9;
    ChangeLight = Light;
    Destroyable = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"

void GegnerKrabblerOben::DoKI() {
    BlickRichtung = LINKS;
    SimpleAnimation();

    // An der Seitenwand umdrehen
    if (blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND) {
        xSpeed = static_cast<float>(Value2);
        BlickRichtung = RECHTS;
    }

    if (blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND) {
        xSpeed = static_cast<float>(-Value2);
        BlickRichtung = LINKS;
    }

    // Schuss Delay verzögern
    ShotDelay -= 1.0f SYNC;

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER_LAUFEN:  // Krabbler krabbelt an der Decke entlang
        {
            // Schiessen ?
            if (ShotDelay <= 0.0f) {
                ShotDelay = static_cast<float>(Value1);
                Handlung = GEGNER_SCHIESSEN;
                Projectiles.PushProjectile(xPos + 24, yPos + 48, KRABBLERLASER1);
                PartikelSystem.PushPartikel(xPos - 1, yPos + 44, LASERFLAME);
                SoundManager.PlayWave3D(static_cast<int>(xPos + 25), static_cast<int>(yPos + 37), 44100, SOUND_LASERSHOT);
                AnimPhase = 10;
                AnimEnde = 14;
                AnimStart = 0;
                AnimSpeed = 0.75f;
            }
        } break;

        case GEGNER_SCHIESSEN: {
            // Fertig mit schiessen ?
            if (AnimPhase == AnimStart) {
                AnimEnde = 9;
                AnimPhase = 0;
                AnimSpeed = 0.5f;
                Handlung = GEGNER_LAUFEN;
            }
        } break;

        default:
            break;
    }  // switch
}

// --------------------------------------------------------------------------------------
// DeckenKrabbler explodiert
// --------------------------------------------------------------------------------------

void GegnerKrabblerOben::GegnerExplode() {
    // Explosion
    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(static_cast<float>(xPos - 15) + random(20), static_cast<float>(yPos - 15) + random(40), EXPLOSION_MEDIUM3);

    SoundManager.PlayWave(100, 128, -random(2000) + 11025, SOUND_EXPLOSION1);  // Sound ausgeben

    Player[0].Score += 300;
}
