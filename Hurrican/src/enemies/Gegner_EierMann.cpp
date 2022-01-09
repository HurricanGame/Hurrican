// --------------------------------------------------------------------------------------
// Der Spinnen Eiermann
//
// Legt Eier die auf Hurri zurollen und dann explodieren
// --------------------------------------------------------------------------------------

#include "Gegner_EierMann.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerEierMann::GegnerEierMann(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_LAUFEN;
    Energy = 50;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimStart = 0;
    AnimEnde = 10;

    switch (Skill) {
        case SKILL_EASY:     AnimSpeed = 1.9f; break;
        case SKILL_MEDIUM:   AnimSpeed = 1.6f; break;
        case SKILL_HARD:     AnimSpeed = 1.3f; break;
        case SKILL_HURRICAN: AnimSpeed = 1.0f; break;
    }

    ChangeLight = Light;
    Destroyable = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerEierMann::DoKI() {
    // Animieren
    if (AnimEnde > 0)  // Soll überhaupt anmiert werden ?
    {
        AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;              // Dann wieder auf Null setzen
            AnimPhase++;                // Und nächste Animationsphase
            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
            {
                AnimPhase = AnimStart;  // Dann wieder von vorne beginnen

                if (PlayerAbstand() <= 700)
                    Projectiles.PushProjectile(xPos + 16.0f, yPos + 40.0f, EIERBOMBE, pAim);
            }
        }
    }  // animieren

    // Je nach Handlung richtig verhalten
    //
    switch (Handlung) {
        case GEGNER_LAUFEN: {
        } break;

        default:
            break;

    }  // switch
}

// --------------------------------------------------------------------------------------
// EierMann explodiert
// --------------------------------------------------------------------------------------

void GegnerEierMann::GegnerExplode() {
    PartikelSystem.PushPartikel(xPos, yPos, EXPLOSION_GIANT);

    for (int i = 0; i < 10; i++) {
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(100)),
                                    yPos - 30.0f + static_cast<float>(random(80)), EXPLOSION_MEDIUM2);
        PartikelSystem.PushPartikel(xPos + 10.0f + static_cast<float>(random(40)),
                                    yPos + 10.0f + static_cast<float>(random(40)), SPIDERSPLITTER);
    }

    for (int i = 0; i < 4; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(80)),
                                    yPos + static_cast<float>(random(80)), SPLITTER);

    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND_EXPLOSION4);

    Player[0].Score += 1500;  // Punkte geben
}
