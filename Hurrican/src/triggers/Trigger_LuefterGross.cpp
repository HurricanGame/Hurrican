// --------------------------------------------------------------------------------------
// Der große Lüfter
// --------------------------------------------------------------------------------------

#include "stdafx.hpp"
#include "Trigger_LuefterGross.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerLuefterGross::GegnerLuefterGross(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_STEHEN;
    Value1			= Wert1;
    Value2			= Wert2;
    ChangeLight		= Light;
    Destroyable		= false;
    ForceLight		= true;
    OwnDraw			= true;
    Energy			= 100;
    BackGround		= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerLuefterGross::DoDraw(void)
{
    D3DCOLOR Color = TileEngine.LightValue(xPos, yPos, GegnerRect[GegnerArt], ForceLight);
    pGegnerGrafix[GegnerArt]->RenderSprite(static_cast<float>(xPos-TileEngine.XOffset),
                                           static_cast<float>(yPos-TileEngine.YOffset),
                                           AnimPhase, Color, false);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerLuefterGross::DoKI(void)
{
}

// --------------------------------------------------------------------------------------
// LuefterGross explodiert
// --------------------------------------------------------------------------------------

void GegnerLuefterGross::GegnerExplode(void)
{
}
