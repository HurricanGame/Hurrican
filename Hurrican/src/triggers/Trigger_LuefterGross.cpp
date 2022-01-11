// --------------------------------------------------------------------------------------
// Der große Lüfter
// --------------------------------------------------------------------------------------

#include "Trigger_LuefterGross.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerLuefterGross::GegnerLuefterGross(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    ForceLight = true;
    OwnDraw = true;
    Energy = 100;
    BackGround = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerLuefterGross::DoDraw() {
    D3DCOLOR Color = TileEngine.LightValue(xPos, yPos, GegnerRect[GegnerArt], ForceLight);
    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                           yPos - TileEngine.YOffset, AnimPhase, Color, false);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerLuefterGross::DoKI() {}

// --------------------------------------------------------------------------------------
// LuefterGross explodiert
// --------------------------------------------------------------------------------------

void GegnerLuefterGross::GegnerExplode() {}
