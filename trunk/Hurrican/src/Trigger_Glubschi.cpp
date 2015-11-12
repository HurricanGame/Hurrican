// --------------------------------------------------------------------------------------
// Das Glubschauge
//
// Steht nur rum und schaut dem Hurri nach (Value1 == 0) oder schaut sich langsam um (Value1 == 1)
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_Glubschi.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerGlubschi::GegnerGlubschi(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_STEHEN;
    BlickRichtung	= LINKS;
    Energy			= 100;
    Value1			= Wert1;			// yPos der Plattform
    Value2			= Wert2;
    ChangeLight		= Light;
    Destroyable		= false;
    rot				= 0.0;
    rotspeed		= float (rand()%10+1) / 5.0f;
    TestBlock		= false;
    OwnDraw			= true;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerGlubschi::DoDraw(void)
{
    BlickRichtung	= LINKS;

    // Je nach horizontalem Spielerabstand richtige Animationsphase setzen
    //
    float a;
    int anim = 0;

    a = xPos - pAim->xpos;

    if (a < 200)
    {
        anim = int ((200-a) / 18);

        if (anim > 20)
            anim = 20;

        AnimPhase = anim;
    }

    // Glubschi rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-TileEngine.XOffset),
                                           (float)(yPos-TileEngine.YOffset),
                                           AnimPhase, 0xFFFFFFFF);

    // Corona rendern
    DirectGraphics.SetAdditiveMode ();
    Projectiles.LavaFlare.RenderSpriteScaledRotated (float (xPos - TileEngine.XOffset - 36 + anim * 1.5f),
                                         float (yPos - TileEngine.YOffset - 33), 92, 92, rot, 0x60FFFFFF);
    DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// Glubschi KI
// --------------------------------------------------------------------------------------

void GegnerGlubschi::DoKI(void)
{
    // Corona rotieren
    //
    rot += rotspeed SYNC;
    while (rot > 360.0f) rot -= 360.0f;
}

// --------------------------------------------------------------------------------------
// Glubschi explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerGlubschi::GegnerExplode(void)
{
}
