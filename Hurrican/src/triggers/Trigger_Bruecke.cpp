// --------------------------------------------------------------------------------------
// Die Hängebrücke
//
// Ein Teil der Brücke
// Hängt je nach Abstand zum Spieler oder einem Objekt unterschiedlich weit runter
// --------------------------------------------------------------------------------------

#include "Trigger_Bruecke.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerBruecke::GegnerBruecke(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    BlickRichtung = DirectionEnum::RECHTS;
    Energy = 100;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    AnimCount = 0.0f;
    TestBlock = false;
}

// --------------------------------------------------------------------------------------
// "Bruecke KI"
// --------------------------------------------------------------------------------------

void GegnerBruecke::DoKI() {
    // Brücke nicht im Bild ?
    //
    if (xPos + 10.0f < TileEngine.XOffset || xPos > TileEngine.XOffset + RENDERWIDTH ||
        yPos + 10.0f < TileEngine.YOffset || yPos > TileEngine.YOffset + RENDERHEIGHT) {
        return;
    }

    // Ausgangsposition
    //

    yPos = static_cast<float>(Value1);

    if (options_Detail == DETAIL_MAXIMUM) {
        // Irgendein Gegner steht auf der Brücke
        //

        for (auto& enemy: Gegner.enemies)
        {
            if (enemy->Active == true &&
                enemy->GegnerArt != BRUECKE &&
                enemy->GegnerArt != BRUECKE &&
                enemy->GegnerArt != STAHLMUECKE &&
                enemy->ySpeed == 0.0f &&
                SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], enemy->xPos, enemy->yPos,
                                GegnerRect[enemy->GegnerArt]) == true) {

                // DKS - Optimized
                // w = (GegnerRect[enemy->GegnerArt].right  - GegnerRect[enemy->GegnerArt].left) / 2.0f;
                // h = (GegnerRect[enemy->GegnerArt].bottom - GegnerRect[enemy->GegnerArt].top)  / 2.0f;
                float w = (GegnerRect[enemy->GegnerArt].right - GegnerRect[enemy->GegnerArt].left) * 0.5f;
                float h = (GegnerRect[enemy->GegnerArt].bottom - GegnerRect[enemy->GegnerArt].top) * 0.5f;

                if (enemy->GegnerArt == DIAMANT)
                    h = 2;

                float dx = enemy->xPos + w - (xPos + 5);
                float dy = enemy->yPos + h - (yPos + 5);

                // DKS - converted to float:
                // double a = sqrt ((dx * dx) + (dy * dy));
                float a = sqrtf((dx * dx) + (dy * dy));

                if (a > 40.0f)
                    a = 40.0f;

                if (enemy->GegnerArt == SPITTER)
                    a += 13.0f;

                // DKS - Optimized
                // yPos = float (enemy->yPos + GegnerRect[enemy->GegnerArt].bottom - a / 2.5f + 13.0f);
                yPos = enemy->yPos + static_cast<float>(GegnerRect[enemy->GegnerArt].bottom) - a * (1.0f / 2.5f) + 13.0f;
            }
        }

        // Spieler steht auf der Brücke
        // dann das Brückenteil je nach Entfernung zum Spieler nach unten verschieben
        //
        for (int p = 0; p < NUMPLAYERS; p++)
            if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], Player[p].xpos, Player[p].ypos,
                                Player[p].CollideRect) == true &&
                Player[p].yspeed >= 0.0f) {

                float dx = (Player[p].xpos + 35.0f) - (xPos + 5.0f);
                float dy = (Player[p].ypos + 40.0f) - (yPos + 5.0f);

                // DKS - converted to float, optimized:
                // double a = sqrt ((dx * dx) + (dy * dy));
                // yPos = float (Value1 + (32 - a / 1.5f));
                float a = sqrtf((dx * dx) + (dy * dy));
                yPos = static_cast<float>(Value1) + (32.0f - a * (1.0f / 1.5f));
            }
    } else
        yPos = static_cast<float>(Value1 + 3);
}

// --------------------------------------------------------------------------------------
// Bruecke explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerBruecke::GegnerExplode() {}
