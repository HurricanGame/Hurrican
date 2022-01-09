// --------------------------------------------------------------------------------------
// Der Button für die Shootplattform
//
// Ein Button, der bei Beschuss eine ShootPlattform nach oben befördert
// je länger man schiesst, um so mehr beschleunigt die Plattform
// --------------------------------------------------------------------------------------

#include "Trigger_ShootButton.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerShootButton::GegnerShootButton(GegnerClass *Plattform) {
    Handlung = GEGNER_STEHEN;
    BlickRichtung = RECHTS;
    Energy = 100;
    Value1 = 0;
    Value2 = 0;
    ChangeLight = false;
    Destroyable = true;
    AnimEnde = 2;
    AnimSpeed = 5.0f;
    pPlattForm = Plattform;
    SmokeCount = 0.1f;
    OwnDraw = true;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerShootButton::DoDraw() {
    // Button rendern
    pGegnerGrafix[GegnerArt]->RenderSprite(pPlattForm->xPos - TileEngine.XOffset + 41.0f,
                                           pPlattForm->yPos - TileEngine.YOffset - 8.0f, AnimPhase,
                                           0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// ShootButton KI
// --------------------------------------------------------------------------------------

void GegnerShootButton::DoKI() {
    SimpleAnimation();

    yPos = pPlattForm->yPos - 9.0f + pPlattForm->ySpeed SYNC;

    // beschossen ?
    //
    if (Energy < 100.0f) {
        Energy = 100.0f;
        pPlattForm->yAcc = -3.0f;

        pPlattForm->Value2 = 1;

        // Flamme ausgeben
        //
        SmokeCount -= 1.0f SYNC;
        if (SmokeCount <= 0.0f) {
            SmokeCount = 0.2f;
            Projectiles.PushProjectile(pPlattForm->xPos + 23.0f + static_cast<float>(random(4)),
                                       pPlattForm->yPos - 15.0f, FEUERFALLE3);
            PartikelSystem.PushPartikel(pPlattForm->xPos + 33.0f + static_cast<float>(random(2)),
                                        pPlattForm->yPos + 40.0f, FLUGSACKSMOKE);
            PartikelSystem.PushPartikel(pPlattForm->xPos + 51.0f + static_cast<float>(random(2)),
                                        pPlattForm->yPos + 40.0f, FLUGSACKSMOKE2);
            PartikelSystem.PushPartikel(pPlattForm->xPos + 48.0f + static_cast<float>(random(2)),
                                        pPlattForm->yPos + 40.0f, FUNKE);
        }
    } else if (static_cast<int>(pPlattForm->yPos) < pPlattForm->Value1)
        pPlattForm->yAcc = 3.0f;
}

// --------------------------------------------------------------------------------------
// ShootButton explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerShootButton::GegnerExplode() {}
