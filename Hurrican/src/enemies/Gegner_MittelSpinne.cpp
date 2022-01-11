// --------------------------------------------------------------------------------------
// Die mittelgroße Spinne
//
// Klettert beim Wuxe Spinnenansturm von der Decke
// --------------------------------------------------------------------------------------

#include "Gegner_MittelSpinne.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerMittelSpinne::GegnerMittelSpinne(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 50;
    AnimSpeed = static_cast<float>(random(8) + 5) / 8.0f;
    ChangeLight = Light;
    Destroyable = true;
    AnimEnde = 8;
    Value1 = Wert1;
    Value2 = Wert2;
    rot = static_cast<float>(random(80) + 140);
    OwnDraw = true;
    TestBlock = false;
    shotdelay = 0.0f;
    Active = true;
    yStart = TileEngine.YOffset;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerMittelSpinne::DoDraw() {
    // Helligkeit abhängig vom der Entfernung zum oberen Rand berechnen, damit
    // es aussieht, als kämen die Spinnen aus der Dunkelheit
    // Ich bin schon ein Fux ;)
    //

    // DKS - Removed superfluous "/1.0f"
    // light = (yPos - 20) - yStart / 1.0f;
    float light = (yPos - 20) - yStart;
    light = std::clamp(light, 0.0f, 255.0f);

    float lightred = 255 - DamageTaken;

    if (lightred > light)
        lightred = light;

    D3DCOLOR Color =
        D3DCOLOR_RGBA(static_cast<int>(light),
                      static_cast<int>(lightred),
                      static_cast<int>(lightred),
                      255);

    // Schatten rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSpriteRotated(xPos - TileEngine.XOffset + 6.0f,
                                                  yPos - TileEngine.YOffset + 6.0f, rot, AnimPhase,
                                                  0x55000000);

    // Spinne rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSpriteRotated(xPos - TileEngine.XOffset,
                                                  yPos - TileEngine.YOffset, rot, AnimPhase, Color);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerMittelSpinne::DoKI() {
    SimpleAnimation();

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::LAUFEN: {
            // Gegner auf Spieler ausrichten
            //

            // Abstände berechnen
            float dx = (xPos + 25) - (pAim->xpos + 35);
            float dy = (yPos + 18) - (pAim->ypos + 40);

            // Division durch Null verhinden
            if (dy == 0.0f)
                dy = 0.01f;

            // DKS - fixed uninitialized var warning:
            // float w, winkel;

            // DKS - converted to float, optimized:
            // w = static_cast<float>(atan(dx / dy) * 360.0f / (D3DX_PI * 2));
            float w = RadToDeg(atanf(dx / dy));
            float winkel = w;

            if (dx >= 0.0f && dy >= 0.f)
                winkel = w;
            else if (dx > 0.0f && dy < 0.0f)
                winkel = 180.0f + w;
            else if (dx < 0.0f && dy > 0.0f)
                winkel = 360.0f + w;
            else if (dx < 0.0f && dy < 0.0f)
                winkel = 180.0f + w;

            winkel = 360.0f - winkel;

            // Spinne rotieren
            //
            float inc = 0.3f;

            if (yPos > pAim->ypos)
                inc = 0.0f;

            if (rot < winkel)
                rot += Timer.sync(inc);
            if (rot > winkel)
                rot -= Timer.sync(inc);

            // Bewegen
            //
            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed = float ( sin(rot * D3DX_PI / 180.0f) * 15.0f * (5.0f - AnimSpeed) SYNC);
            // ySpeed = float (-cos(rot * D3DX_PI / 180.0f) * 15.0f * (5.0f - AnimSpeed) SYNC);
            xSpeed = Timer.sync(sin_deg(rot) * 15.0f * (5.0f - AnimSpeed));
            ySpeed = Timer.sync(-cos_deg(rot) * 15.0f * (5.0f - AnimSpeed));

        } break;

        case GEGNER::FALLEN: {
            rot += Timer.sync(static_cast<float>(Value2));

            clampAngle(rot);

            // An Wand gestossen ?
            //
            if (blocko & BLOCKWERT_WAND || blocku & BLOCKWERT_WAND || blockl & BLOCKWERT_WAND ||
                blockr & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM)
                Energy = 0.0f;

            shotdelay -= Timer.sync(1.0f);

            if (shotdelay < 0.0f) {
                shotdelay = Timer.sync(0.2f);
                PartikelSystem.PushPartikel(xPos + 20.0f + static_cast<float>(random(5)),
                                            yPos + 15.0f + static_cast<float>(random(5)), ROCKETSMOKE);
            }
        } break;
    }

    // Spieler berührt ?
    //
    if (Handlung != GEGNER::FALLEN)
        TestDamagePlayers(Timer.sync(4.0f));
}

// --------------------------------------------------------------------------------------
// Explodieren
// --------------------------------------------------------------------------------------

void GegnerMittelSpinne::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);
    PartikelSystem.PushPartikel(xPos + 5.0f, yPos, EXPLOSION_MEDIUM3);

    for (int i = 0; i < 10; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(80)),
                                    yPos + static_cast<float>(random(50)), SPIDERSPLITTER);

    for (int i = 0; i < 30; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(80)),
                                    yPos + static_cast<float>(random(50)), FUNKE);

    for (int i = 0; i < 7; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(60)),
                                    yPos - 10.0f + static_cast<float>(random(60)), EXPLOSION_MEDIUM2);

    Player[0].Score += 250;
}
