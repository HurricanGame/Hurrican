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
    Handlung = GEGNER_STEHEN;
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
    float a;
    int anim = 0;

    a = xPos - pAim->xpos;

    if (a < 200) {
        anim = static_cast<int>((200 - a) / 18);

        if (anim > 20)
            anim = 20;

        AnimPhase = anim;
    }

    // Glubschi rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSprite(static_cast<float>(xPos - TileEngine.XOffset),
                                           static_cast<float>(yPos - TileEngine.YOffset), AnimPhase, 0xFFFFFFFF);

    // Corona rendern
    DirectGraphics.SetAdditiveMode();
    Projectiles.LavaFlare.RenderSpriteScaledRotated(static_cast<float>(xPos - TileEngine.XOffset - 36 + anim * 1.5f),
                                                    static_cast<float>(yPos - TileEngine.YOffset - 33), 92, 92, rot, 0x60FFFFFF);
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
