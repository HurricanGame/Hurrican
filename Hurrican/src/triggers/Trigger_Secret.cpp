// --------------------------------------------------------------------------------------
// Secret Area
//
// Wird sie vom Spieler berührt, so wird der "Secret per Level" Counter erhöht
// wird für die "Level-Endabrechnung/Summary Screen" benötigt
// --------------------------------------------------------------------------------------

#include "Trigger_Secret.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSecret::GegnerSecret(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    Energy = 100;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    TestBlock = false;
    DontMove = true;
}

// --------------------------------------------------------------------------------------
// "Secret KI"
// --------------------------------------------------------------------------------------

void GegnerSecret::DoKI() {
    // Secret nicht im Bild ?
    //
    if (xPos + 10.0f < TileEngine.XOffset || xPos > TileEngine.XOffset + RENDERWIDTH ||
        yPos + 10.0f < TileEngine.YOffset || yPos > TileEngine.YOffset + RENDERHEIGHT) {
        return;
    }

    switch (Handlung) {
        case GEGNER::STEHEN: {
            // Spieler berührt das Secret? Dann Counter erhöhen und deaktivieren
            //
            GegnerRect[GegnerArt].left = -(Value1 / 2) + 20;
            GegnerRect[GegnerArt].right = (Value1 / 2) + 20;
            GegnerRect[GegnerArt].top = -(Value2 / 2) + 20;
            GegnerRect[GegnerArt].bottom = (Value2 / 2) + 20;

            for (int p = 0; p < NUMPLAYERS; p++)
                if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], Player[p].xpos, Player[p].ypos,
                                    Player[p].CollideRect) == true) {
                    Player[0].SecretThisLevel++;
                    Player[0].SecretFullGame++;
                    Handlung = GEGNER::FALLEN;

                    SoundManager.PlayWave(100, 128, 11025, SOUND::SECRET);

                    for (int i = 0; i < 200; i++)
                        PartikelSystem.PushPartikel(xPos + 20.0f, yPos + 20.0f, KRINGELSECRET);

                    WinkelUebergabe = -1.0f;
                    PartikelSystem.PushPartikel(xPos + 20.0f, yPos + 12.0f, TEXTSECRET);
                    PartikelSystem.PushPartikel(xPos - 100.0f, yPos - 100.0f, GRENADEFLARE);
                    PartikelSystem.PushPartikel(xPos + 5.0f, yPos + 5.0f, EXPLOSION_KRINGEL);
                }
        } break;
    }
}

// --------------------------------------------------------------------------------------
// Secret explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerSecret::GegnerExplode() {}
