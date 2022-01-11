// --------------------------------------------------------------------------------------
// Die Rakete, die die Riesenspinnen spuckt
//
// --------------------------------------------------------------------------------------

#include "Gegner_FetteRakete.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFetteRakete::GegnerFetteRakete(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::INIT;
    Energy = 50;
    ChangeLight = Light;
    Destroyable = true;
    Value1 = Wert1;
    Value2 = Wert2;
    SmokeDelay = 0.0f;
    AnimEnde = 10;
    AnimSpeed = 0.3f;
    OwnDraw = true;
    rot = static_cast<float>(Wert1);
    BlickRichtung = LINKS;

    // Rakete kommt vom Drachen? Dann verhält sie sich anders
    if (Wert2 == 99) {
        xSpeed = 0.0f;
        ySpeed = -10.0f;
        yAcc = 2.0f;
        Handlung = GEGNER::DREHEN;
        AnimCount = 12.0f;
    }
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerFetteRakete::DoDraw() {
    // Rakete rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSpriteRotated(xPos - TileEngine.XOffset,
                                                  yPos - TileEngine.YOffset,
                                                  rot, AnimPhase, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFetteRakete::DoKI() {
    BlickRichtung = LINKS;

    switch (Handlung) {
        case GEGNER::INIT: {
            // Rakete kam nicht vom Drachen? Dann
            // enthält Wert1 den Winkel, in den die Raketen fliegen sollen

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed =  float (sin (Value1 * PI / 180.0f) * 35);
            // ySpeed = -float (cos (Value1 * PI / 180.0f) * 35);
            xSpeed = sin_deg(Value1) * 35.0f;
            ySpeed = -cos_deg(Value1) * 35.0f;

            Handlung = GEGNER::LAUFEN;
        } break;

        // Rakete dreht sich in Richtung Hurrican
        case GEGNER::DREHEN: {
            AnimCount -= Timer.sync(1.0f);

            // Gegner auf Spieler ausrichten
            //

            // Abstände berechnen
            float dx = (xPos + 12.0f) - (pAim->xpos + 35.0f);
            float dy = (yPos + 17.0f) - (pAim->ypos + 40.0f);

            // Division durch Null verhinden
            if (dy == 0.0f)
                dy = 0.01f;

            // DKS - fixed uninitialized var warning:
            // float w, winkel;

            // DKS - Use new Rad/Deg macros, converted to float:
            // w = static_cast<float>(atan(dx / dy) * 360.0f / (D3DX_PI * 2));
            float w = RadToDeg(atanf(dx / dy));
            float winkel = w;

            if (dx >= 0.0f && dy >= 0.0f)
                winkel = w;
            else if (dx > 0.0f && dy < 0.0f)
                winkel = 180.0f + w;
            else if (dx < 0.0f && dy > 0.0f)
                winkel = 360.0f + w;
            else if (dx < 0.0f && dy < 0.0f)
                winkel = 180.0f + w;

            winkel = 360.0f - winkel;

            if (rot < winkel)
                rot += Timer.sync(25.0f);
            if (rot > winkel)
                rot -= Timer.sync(25.0f);

            // zuende gedreht? Dann losfliegen
            if (AnimCount < 0.0f) {
                Handlung = GEGNER::LAUFEN;
                SoundManager.PlayWave(100, 128, 9000 + random(2000), SOUND::ROCKET);

                xAcc = 0;
                yAcc = 0;
                // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
                // xSpeed =  float (sin (rot * PI / 180.0f) * 35);
                // ySpeed = -float (cos (rot * PI / 180.0f) * 35);
                xSpeed = sin_deg(rot) * 35.0f;
                ySpeed = -cos_deg(rot) * 35.0f;
            }
        } break;

        // Rakete fliegt
        //
        case GEGNER::LAUFEN: {
            SimpleAnimation();

            SmokeDelay -= Timer.sync(1.0f);

            if (SmokeDelay < 0.0f) {
                SmokeDelay = 0.3f;

                PartikelSystem.PushPartikel(xPos + 2.0f, yPos + 5.0f, SMOKE);
                // PartikelSystem.PushPartikel(xPos + 8, yPos + 12, ROCKETSMOKE);
            }

            // Gegen die Wand geflogen? Dann explodieren lassen
            //
            if (blockl & BLOCKWERT_WAND || blockr & BLOCKWERT_WAND || blocko & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_WAND)
                Energy = 0.0f;

        } break;

        default:
            break;
    }  // switch

    // Spieler getroffen?
    // Dann explodieren lassen und Energy abziehen
    //
    TestDamagePlayers(25.0f, true);
}

// --------------------------------------------------------------------------------------
// FetteRakete explodiert
// --------------------------------------------------------------------------------------

void GegnerFetteRakete::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);

    PartikelSystem.PushPartikel(xPos - 10.0f, yPos - 20.0f, EXPLOSION_MEDIUM2);

    for (int i = 0; i < 5; i++) {
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(20)),
                                    yPos - 10.0f + static_cast<float>(random(20)), MINIFLARE);
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(20)),
                                    yPos - 10.0f + static_cast<float>(random(20)), SPIDERSPLITTER);
    }

    Player[0].Score += 50;
}
