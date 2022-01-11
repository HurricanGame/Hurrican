// --------------------------------------------------------------------------------------
// Das Glubschauge
//
// Steht nur rum und schaut dem Hurri nach (Value1 == 0) oder schaut sich langsam um (Value1 == 1)
// --------------------------------------------------------------------------------------

#include "Trigger_Glubschi.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerGlubschi::GegnerGlubschi(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    BlickRichtung = LINKS;
    Energy = 100;
    Value1 = Wert1;  // yPos der Plattform
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    rot = 0.0;
    rotspeed = static_cast<float>(random(10) + 1) / 5.0f;
    TestBlock = false;
    OwnDraw = true;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerGlubschi::DoDraw() {
    BlickRichtung = LINKS;

    // Je nach horizontalem Spielerabstand richtige Animationsphase setzen
    //
    int anim = 0;

    float a = xPos - pAim->xpos;

    if (a < 200.0f) {
        anim = static_cast<int>((200.0f - a) / 18.0f);

        if (anim > 20)
            anim = 20;

        AnimPhase = anim;
    }

    // Glubschi rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                           yPos - TileEngine.YOffset, AnimPhase, 0xFFFFFFFF);

    // Corona rendern
    DirectGraphics.SetAdditiveMode();
    Projectiles.LavaFlare.RenderSpriteScaledRotated(xPos - TileEngine.XOffset - 36.0f + static_cast<float>(anim) * 1.5f,
                                                    yPos - TileEngine.YOffset - 33.0f, 92, 92, rot, 0x60FFFFFF);
    DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// Glubschi KI
// --------------------------------------------------------------------------------------

void GegnerGlubschi::DoKI() {
    // Corona rotieren
    //
    rot += rotspeed SYNC;
    clampAngle(rot);
}

// --------------------------------------------------------------------------------------
// Glubschi explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerGlubschi::GegnerExplode() {}
