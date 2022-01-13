// --------------------------------------------------------------------------------------
// Die Stahlpresse
//
// Bewegt sich mit Value1 als y-Speed hinunter
// und wartet dann solange wie es Value2 angibt
// --------------------------------------------------------------------------------------

#include "Trigger_Presse.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPresse::GegnerPresse(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    BlickRichtung = DirectionEnum::LINKS;
    Energy = 100;
    Value1 = Wert1;
    Value2 = Wert2;
    ySpeed = static_cast<float>(Wert2);
    AnimPhase = 0;
    ChangeLight = Light;
    Destroyable = false;
}

// --------------------------------------------------------------------------------------
// Bewegungs KI
// --------------------------------------------------------------------------------------

void GegnerPresse::DoKI() {
    // Ja nach Handlung richtig verhalten
    switch (Handlung) {
        // Pressen (während der Wehen ...)
        case GEGNER::LAUFEN: {
            // Am Boden ? Dann aufstampfen und wieder nach oben fahren
            if (ySpeed > 0.0f &&
                (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_GEGNERWAND || blocku & BLOCKWERT_WAND)) {
                ySpeed = -static_cast<float>(Value2) / 2;

                // Staub erzeugen
                for (int i = 0; i < 8; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(i * 10), yPos + 244.0f, SMOKE);

                if (PlayerAbstand() < 600)
                    SoundManager.PlayWave(100, 128, 11025, SOUND::PRESSE);
            }

            // An der Decke angekommen ? Dann warten
            if (blocko & BLOCKWERT_WAND) {
                Handlung = GEGNER::STEHEN;
                AnimCount = static_cast<float>(Value2);
                ySpeed = 0.0f;
            }

        } break;

        // Warten
        case GEGNER::STEHEN: {
            // Wieder nach unten fahren
            AnimCount -= Timer.sync(1.0f);

            if (AnimCount <= 0.0f) {
                AnimCount = 0.0f;
                Handlung = GEGNER::LAUFEN;
                ySpeed = static_cast<float>(Value2);

                if (PlayerAbstand() < 600)
                    SoundManager.PlayWave(100, 128, 11025, SOUND::PRESSE);

                PartikelSystem.PushPartikel(xPos + 14.0f, yPos + 230.0f, SMOKE);
                PartikelSystem.PushPartikel(xPos + 54.0f, yPos + 230.0f, SMOKE);
            }
        } break;
    }  // switch

    Wegschieben(GegnerRect[GegnerArt], 10.0f);
}

// --------------------------------------------------------------------------------------
// Presse explodiert (geht ja garnich *g*)
// --------------------------------------------------------------------------------------

void GegnerPresse::GegnerExplode() {}
