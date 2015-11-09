// --------------------------------------------------------------------------------------
// LightFlare
//
// Dreht sich langsam
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_LightFlare.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerLightFlare::GegnerLightFlare(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_STEHEN;
    BlickRichtung	= LINKS;
    Energy			= 100;
    Value1			= Wert1;			// yPos der Plattform
    Value2			= Wert2;
    ChangeLight		= Light;
    Destroyable		= false;
    rot				= 0.0;
    rotspeed		= float (rand()%10+1) / 3.0f;
    TestBlock		= false;
    OwnDraw			= true;

    // Farbe festlegen
    //
    r = g = b = 0;

    switch (Value2)
    {
    case 0 :
        r = g = b = 255;
        break;

    case 1 :
        r = 255;
        break;

    case 2 :
        g = 255;
        break;

    case 3 :
        b = 255;
        break;

    case 4 :
        r = g = 255;
        break;

    case 5 :
        r = b = 255;
        break;

    case 6 :
        g = b = 255;
        break;

    default:
        r = g = b = 255;
        break;

    } // switch

    Value1 = 255;
    Alpha = float (Value1);
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerLightFlare::DoDraw(void)
{
    DirectGraphics.SetAdditiveMode ();
    while (rot > 360.0f) rot -= 360.0f;
    while (rot <   0.0f) rot += 360.0f;

    D3DCOLOR Color = D3DCOLOR_RGBA(r, g, b, int (Alpha));

    pGegnerGrafix[GegnerArt]->RenderSpriteRotatedOffset (float (xPos - TileEngine.XOffset),
            float (yPos - TileEngine.YOffset), rot,
            -4, -4,
            Color);
    DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// LightFlare KI
// --------------------------------------------------------------------------------------

void GegnerLightFlare::DoKI(void)
{
    // rotieren
    rot += rotspeed SYNC;

    // Spieler "davor?" Dann ausfaden
    if (PlayerAbstand(true) < 20)
    {
        if (Alpha > 128.0f) Alpha -= 75.0f SYNC;
        if (Alpha < 128.0f) Alpha  = 128.0f;
    }
    else
    {
        if (Alpha < float (Value1)) Alpha += 75.0f SYNC;
        if (Alpha > float (Value1)) Alpha  = float (Value1);
    }
}

// --------------------------------------------------------------------------------------
// LightFlare explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerLightFlare::GegnerExplode(void)
{
}
