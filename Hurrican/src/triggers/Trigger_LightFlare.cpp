// --------------------------------------------------------------------------------------
// LightFlare
//
// Dreht sich langsam
// --------------------------------------------------------------------------------------

#include "Trigger_LightFlare.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerLightFlare::GegnerLightFlare(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_STEHEN;
    BlickRichtung = LINKS;
    Energy = 100;
    Value1 = Wert1;  // yPos der Plattform
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    rot = 0.0;
    rotspeed = static_cast<float>(random(10) + 1) / 3.0f;
    TestBlock = false;
    OwnDraw = true;

    // Farbe festlegen
    //
    r = g = b = 0;

    switch (Value2) {
        case 0:
            r = g = b = 255;
            break;

        case 1:
            r = 255;
            break;

        case 2:
            g = 255;
            break;

        case 3:
            b = 255;
            break;

        case 4:
            r = g = 255;
            break;

        case 5:
            r = b = 255;
            break;

        case 6:
            g = b = 255;
            break;

        default:
            r = g = b = 255;
            break;

    }  // switch

    Value1 = 255;
    Alpha = static_cast<float>(Value1);
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerLightFlare::DoDraw() {
    DirectGraphics.SetAdditiveMode();
    clampAngle(rot);

    D3DCOLOR Color = D3DCOLOR_RGBA(r, g, b, static_cast<int>(Alpha));

    pGegnerGrafix[GegnerArt]->RenderSpriteRotatedOffset(static_cast<float>(xPos - TileEngine.XOffset),
                                                        static_cast<float>(yPos - TileEngine.YOffset), rot, -4, -4, Color);
    DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// LightFlare KI
// --------------------------------------------------------------------------------------

void GegnerLightFlare::DoKI() {
    // rotieren
    rot += rotspeed SYNC;

    // Spieler "davor?" Dann ausfaden
    if (PlayerAbstand(true) < 20) {
        if (Alpha > 128.0f)
            Alpha -= 75.0f SYNC;
        if (Alpha < 128.0f)
            Alpha = 128.0f;
    } else {
        if (Alpha < static_cast<float>(Value1))
            Alpha += 75.0f SYNC;
        if (Alpha > static_cast<float>(Value1))
            Alpha = static_cast<float>(Value1);
    }
}

// --------------------------------------------------------------------------------------
// LightFlare explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerLightFlare::GegnerExplode() {}
