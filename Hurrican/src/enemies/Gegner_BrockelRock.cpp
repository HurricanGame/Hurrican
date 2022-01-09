// --------------------------------------------------------------------------------------
// Der bröckelnde Felsblock
//
// fängt an zu bröckeln sobald der Spieler darüber läuft und fällt dann runter
// --------------------------------------------------------------------------------------

#include "Gegner_BrockelRock.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerBrockelRock::GegnerBrockelRock(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_STEHEN;
    Energy = 10;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    OwnDraw = true;
    AnimCount = 255.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerBrockelRock::DoDraw() {
    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                           yPos - TileEngine.YOffset, 0,
                                           D3DCOLOR_RGBA(255, 255, 255, static_cast<int>(AnimCount)));

    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                           yPos - TileEngine.YOffset, 1,
                                           D3DCOLOR_RGBA(255, 255, 255, 255 - static_cast<int>(AnimCount)));
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerBrockelRock::DoKI() {
    PlattformTest(GegnerRect[GegnerArt]);

    // Je nach Handlung richtig verhalten
    //
    switch (Handlung) {
        // Stein wartet bis der Spieler darüber läuft
        case GEGNER_STEHEN: {
            for (int i = 0; i < NUMPLAYERS; i++)
                if (Player[i].AufPlattform == this)
                    Handlung = GEGNER_SPRINGEN;
        } break;

        // Counter zählt runter bis Stein fällt
        //
        case GEGNER_SPRINGEN: {
            AnimCount -= 50.0f SYNC;

            if (AnimCount < 0.0f)  // Langsam zerbröckeln
            {
                AnimCount = 0.0f;

                // Partikel erzeugen
                //
                for (int i = 0; i < 5; i++) {
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(60)),
                                                yPos + 20.0f, ROCKSPLITTERSMALL);
                    PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(i * 10),
                                                yPos, SMOKEBIG);
                }

                // Sound ausgeben
                //
                SoundManager.PlayWave(100, 128, 11025 + random(2000), SOUND_STONEFALL);
                Handlung = GEGNER_FALLEN;
                yAcc = 4.0f;
            }

        } break;

        // Stein fällt runter
        //
        case GEGNER_FALLEN: {
            // Maximale Geschwindigkeitsbegrenzung
            if (ySpeed > 50.0f)
                yAcc = 0.0f;

            // Stein ist am Boden gelandet
            if (blocku & BLOCKWERT_WAND) {
                Energy = 0;
            }
        } break;

        default:
            break;
    }  // switch
}

// --------------------------------------------------------------------------------------
// Explodieren
// --------------------------------------------------------------------------------------

void GegnerBrockelRock::GegnerExplode() {
    for (int i = 0; i < NUMPLAYERS; i++)
        if (Player[i].AufPlattform == this)
            Player[i].AufPlattform = nullptr;

    // Splitter erzeugen Rauch
    for (int i = 0; i < 10; i++) {
        PartikelSystem.PushPartikel(xPos - 12.0f + static_cast<float>(random(80)),
                                    yPos + 20.0f + static_cast<float>(random(20)), SMOKE);
        PartikelSystem.PushPartikel(xPos - 12.0f + static_cast<float>(random(80)),
                                    yPos + static_cast<float>(random(40)), ROCKSPLITTER);
        PartikelSystem.PushPartikel(xPos - 12.0f + static_cast<float>(random(80)),
                                    yPos + static_cast<float>(random(40)), ROCKSPLITTERSMALL);
    }

    SoundManager.PlayWave(100, 128, 11025 + random(2000), SOUND_STONEEXPLODE);  // Sound ausgeben
}
