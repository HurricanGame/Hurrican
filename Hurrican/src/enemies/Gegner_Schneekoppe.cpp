// --------------------------------------------------------------------------------------
// Der Schuss des Schneekönigs
//
// kann vom Spieler angeschossen werden und ändert dann die Richtung
// --------------------------------------------------------------------------------------

#include "Gegner_Schneekoppe.hpp"
#include "math.h"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchneekoppe::GegnerSchneekoppe(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_LAUFEN;
    Energy = 600;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;

    // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
    // xSpeed = (float)sin(Wert1 * PI / 180.0f) * Value2;
    // ySpeed = (float)cos(Wert1 * PI / 180.0f) * Value2;
    xSpeed = sin_deg(static_cast<float>(Wert1)) * static_cast<float>(Value2);
    ySpeed = cos_deg(static_cast<float>(Wert1)) * static_cast<float>(Value2);

    yAcc = 6.0f;
    //	AnimSpeed = 0.5f;
    //	AnimEnde  = 20;
    isHit = false;
    OwnDraw = true;
}

// --------------------------------------------------------------------------------------
// Eigene Drawroutine
// --------------------------------------------------------------------------------------

void GegnerSchneekoppe::DoDraw() {
    // Drehwinkel aus der Geschwindigkeit errechnen

    // DKS - converted to float, optimized:
    // float w = 180.0f - static_cast<float>(atan(xSpeed / ySpeed) * 360.0f / (D3DX_PI * 2));
    float w = 180.0f - RadToDeg(atanf(xSpeed / ySpeed));

    if (xSpeed >= 0.0f && ySpeed >= 0.0f)
        w = w;
    else if (xSpeed > 0.0f && ySpeed < 0.0f)
        w = 180.0f + w;
    else if (xSpeed < 0.0f && ySpeed > 0.0f)
        w = 360.0f + w;
    else if (xSpeed < 0.0f && ySpeed < 0.0f)
        w = 180.0f + w;

    pGegnerGrafix[GegnerArt]->RenderSpriteRotated(xPos - TileEngine.XOffset,
                                                  yPos - TileEngine.YOffset, w, AnimPhase,
                                                  0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSchneekoppe::DoKI() {
    BlickRichtung = RECHTS;

    SimpleAnimation();

    if (DamageTaken > 0.0f)
        isHit = true;

    // Links oder rechts an der Wand abgeprallt ?
    //
    if ((blockl & BLOCKWERT_WAND) || (blockr & BLOCKWERT_WAND) || (blocko & BLOCKWERT_WAND) ||
        (blocku & BLOCKWERT_WAND) || (blocku & BLOCKWERT_PLATTFORM)) {
        Energy = 0.0f;
    }

    // Spieler verletzen ?
    //
    TestDamagePlayers(20.0f, true);
}

// --------------------------------------------------------------------------------------
// Auge explodiert
// --------------------------------------------------------------------------------------

void GegnerSchneekoppe::GegnerExplode() {
    for (int i = 0; i < 10; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(20)),
                                    yPos - 10.0f + static_cast<float>(random(20)), BLUE_EXPLOSION);

    SoundManager.PlayWave(100, 128, -random(2000) + 11025, SOUND_EXPLOSION1);  // Sound ausgeben
}
