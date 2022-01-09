// --------------------------------------------------------------------------------------
// Der Schwabbelsack
//
// Schwabbelt rum und ballert ab und zu Maden
// --------------------------------------------------------------------------------------

#include "Gegner_Schwabbel.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchwabbel::GegnerSchwabbel(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_STEHEN;
    HitSound = 1;
    Energy = 300;
    ChangeLight = Light;
    Destroyable = true;
    Value1 = Wert1;
    Value2 = Wert2;
    shotdelay = 5.0f;
    AnimSpeed = 0.3f;
    AnimEnde = 20;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSchwabbel::DoKI() {
    // Weniger Energie -> schneller schwabbeln
    //
    AnimSpeed = Energy / 1000.0f + 0.1f;

    // Animieren
    if (Handlung == GEGNER_STEHEN)  // Soll überhaupt animiert werden ?
    {
        AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;  // Dann wieder auf Null setzen
            AnimPhase++;    // Und nächste Animationsphase

            // Maden loskotzen
            //
            if (AnimPhase == 14) {
                shotdelay -= 1.0f;

                if (shotdelay <= 0.0f) {
                    Handlung = GEGNER_SCHIESSEN;
                    shotdelay = 40.0f;
                    AnimCount = 1.0f;
                    SoundManager.PlayWave(100, 128, 13000, SOUND_KOTZEN);
                }
            }

            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
                AnimPhase = AnimStart;  // Dann wieder von vorne beginnen
        }
    }  // animieren

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER_SCHIESSEN: {
            AnimCount -= 1.0f SYNC;

            if (AnimCount < 0.0f) {
                AnimCount += 1.0f;

                Gegner.PushGegner(xPos + static_cast<float>(random(6) + 16),
                                  yPos + static_cast<float>(random(4) - 10), MADE, 1, 0, false);
            }

            shotdelay -= 1.0f SYNC;

            if (shotdelay < 0.0f) {
                shotdelay = static_cast<float>(random(4) + 6);
                Handlung = GEGNER_STEHEN;
            }
        } break;
    }

    // Spieler berührt ?
    //
    TestDamagePlayers(2.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Schwabbel explodiert
// --------------------------------------------------------------------------------------

void GegnerSchwabbel::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 11025, SOUND_MADE);
    SoundManager.PlayWave(100, 128, 11025, SOUND_EXPLOSION1);

    for (int i = 0; i < 80; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(40)),
                                    yPos + static_cast<float>(random(60) - 10), MADEBLUT);

    for (int i = 0; i < 30; i++)
        Gegner.PushGegner(xPos + static_cast<float>(random(40)),
                          yPos + static_cast<float>(random(60) - 10), MADE, 1, 0, false);

    Player[0].Score += 400;
}
