// --------------------------------------------------------------------------------------
// Der kleine Luefter
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_LuefterKlein.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerLuefterKlein::GegnerLuefterKlein(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_STEHEN;
    Value1			= Wert1;
    Value2			= Wert2;
    ChangeLight		= Light;
    Destroyable		= false;
    ForceLight		= true;
    OwnDraw			= true;
    Energy			= 100;
    AnimEnde		= 11;
    BackGround		= true;

    if (Value1 == 0)
        AnimSpeed = 0.5f;
    else
        AnimSpeed = Value1 / 10.0f;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerLuefterKlein::DoDraw(void)
{
    D3DCOLOR Color = TileEngine.LightValue(xPos, yPos, GegnerRect[GegnerArt], ForceLight);
    pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-TileEngine.XOffset),
                                           (float)(yPos-TileEngine.YOffset),
                                           AnimPhase, Color, false);
}


// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerLuefterKlein::DoKI(void)
{
    SimpleAnimation();
}

// --------------------------------------------------------------------------------------
// LuefterKlein explodiert

void GegnerLuefterKlein::GegnerExplode(void)
{
}
