// --------------------------------------------------------------------------------------
// Die Super-Mario Pflanze
//
// Spuckt Feuerbälle, wenn der Spieler in der Nähe ist
// --------------------------------------------------------------------------------------

#include "Gegner_Pflanze.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPflanze::GegnerPflanze(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_LAUFEN;
    HitSound = 1;
    Energy = 80;
    AnimPhase = random(8);
    AnimEnde = 9;
    AnimSpeed = 2.2f - Skill * 0.4f;
    AnimCount = 0.0f;
    ChangeLight = Light;
    Destroyable = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerPflanze::DoKI() {
    // Animieren
    //
    AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
    if (AnimCount > AnimSpeed)  // Grenze überschritten ?
    {
        AnimCount = 0;  // Dann wieder auf Null setzen
        AnimPhase++;    // Und nächste Animationsphase

        if (AnimPhase == 14)  // Schuss abgeben
        {
            if (BlickRichtung == RECHTS)
                Projectiles.PushProjectile(xPos + 84, yPos + 16, PFLANZESHOT, pAim);
            else
                Projectiles.PushProjectile(xPos + 12, yPos + 16, PFLANZESHOT, pAim);
        }

        if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
        {
            // Schiessen ?
            //
            if (RunningTutorial == false && (PlayerAbstand() < 300 || random(2) == 0)) {
                Handlung = GEGNER_SCHIESSEN;
                AnimEnde = 17;
                AnimSpeed = 2.2f - Skill * 0.4f;
                AnimPhase = 9;
            }

            // oder normal wippen
            //
            else {
                Handlung = GEGNER_LAUFEN;
                AnimEnde = 8;
                AnimSpeed = 1.8f;
                AnimPhase = 0;
            }
        }
    }

    // In Richtung Spieler kucken
    //
    if (pAim->xpos + 35 < xPos + 55)
        BlickRichtung = LINKS;
    else
        BlickRichtung = RECHTS;

    switch (Handlung) {
        // Nur rumwackeln
        //
        case GEGNER_LAUFEN:
            break;
    }

    // Testen, ob der Spieler die Pflanze berührt hat
    //
    TestDamagePlayers(1.5f SYNC);
}

// --------------------------------------------------------------------------------------
// Pflanze explodiert
// --------------------------------------------------------------------------------------

void GegnerPflanze::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND_EXPLOSION1);

    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(xPos + random(50), yPos + random(40), EXPLOSION_GREEN);

    for (int i = 0; i < 30; i++)
        // Blätter erzeugen
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(100)),
                                    yPos + static_cast<float>(10 + random(50)), BLATT);

    Player[0].Score += 300;
}
