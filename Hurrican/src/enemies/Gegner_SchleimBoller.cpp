// --------------------------------------------------------------------------------------
// Der Schleimboller
//
// Springt auf den Spieler zu
// Wird er zerstört, so teilt er sich in zwei kleinere Bollen auf
// Value1 gibt die größe an (0 - 60)
// Value2 gibt die neue xSpeed an
// Ist ein Bollen kleiner als 30, so teilt er sich nicht mehr.
// --------------------------------------------------------------------------------------

#include "Gegner_SchleimBoller.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchleimBoller::GegnerSchleimBoller(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::FALLEN;
    HitSound = 1;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;

    if (Value1 != 0)
        Size = Value1;
    else
        Size = 60;

    Energy = static_cast<float>(Size);

    xSpeed = static_cast<float>(Value2);
    OwnDraw = true;

    if (Value2 == 0)
        ySpeed = 0.0f;
    else
        ySpeed = -40.0f;

    AnimSpeed = 2.0f;
    AnimEnde = 2;

    yAcc = 8.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerSchleimBoller::DoDraw() {
    // Je nach Größe anders gestrecht rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSpriteScaled(xPos - TileEngine.XOffset + static_cast<float>(30 - Size / 2),
                                                 yPos - TileEngine.YOffset + static_cast<float>(60 - Size), Size,
                                                 Size, AnimPhase, 0xFFFFFFFF);

    // Leuchten noch dazurendern?
    //
    if (AlreadyDrawn == false) {
        if (options_Detail >= DETAIL_HIGH) {
            DirectGraphics.SetAdditiveMode();
            Projectiles.LavaFlare.RenderSpriteScaled(xPos - TileEngine.XOffset + static_cast<float>(30 - Size),
                                                     yPos - TileEngine.YOffset + static_cast<float>(40 - Size),
                                                     Size * 2, Size * 2, 0x8888FF88);
            DirectGraphics.SetColorKeyMode();
        }
    }

    AlreadyDrawn = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSchleimBoller::DoKI() {
    BlickRichtung = LINKS;

    if (Handlung == GEGNER::FALLEN)
        SimpleAnimation();

    // nach Animation am Boden wieder abspringen
    //
    else {
        AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;              // Dann wieder auf Null setzen
            AnimPhase++;                // Und nächste Animationsphase
            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
            {
                Handlung = GEGNER::FALLEN;
                ySpeed = -static_cast<float>(random(10) + 30);
                yAcc = 8.0f;
                AnimPhase = 0;
                AnimEnde = 2;
                AnimSpeed = 2.0f;

                if (pAim->xpos + 35 > xPos + 30)
                    xSpeed = static_cast<float>(random(8) + 6);
                else
                    xSpeed = -static_cast<float>(random(8) + 6);
            }
        }
    }

    switch (Handlung) {
        // rumhopsen
        //
        case GEGNER::FALLEN: {
            if (ySpeed > 50.0f) {
                ySpeed = 50.0f;
                yAcc = 0.0f;
            }

            // am Boden abhopfen?
            //
            if (ySpeed > 0.0f && (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM)) {
                Handlung = GEGNER::STEHEN;
                AnimEnde = 12;
                AnimSpeed = 0.5f;
                ySpeed = 0.0f;
                yAcc = 0.0f;
                xSpeed = 0.0f;
            }

            // an der Decke abprallen
            if (ySpeed < 0.0f && (blocko & BLOCKWERT_WAND || blocko & BLOCKWERT_GEGNERWAND))
                ySpeed *= -1;

        } break;

        case GEGNER::STEHEN: {
        } break;
    }

    // Testen, ob der Spieler den SchleimBoller berührt hat
    TestDamagePlayers(Timer.sync(2.0f));
    Wegschieben(GegnerRect[GegnerArt], 2.0f);
}

// --------------------------------------------------------------------------------------
// SchleimBoller explodiert
// --------------------------------------------------------------------------------------

void GegnerSchleimBoller::GegnerExplode() {
    // PartikelSystem.PushPartikel(xPos, yPos, EXPLOSION_GREEN);

    for (int i = 0; i < 3 + Size / 8; i++)
        PartikelSystem.PushPartikel(xPos + 15.0f + static_cast<float>(random(20)),
                                    yPos + 15.0f + static_cast<float>(random(20)), SCHLEIM);

    // DKS - Added function WaveIsPlaying() to SoundManagerClass:
    if (!SoundManager.WaveIsPlaying(SOUND::SCHLEIM))
        SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::SCHLEIM);  // Sound ausgeben

    Player[0].Score += 150;

    // Zwei kleinere Bobbel spawnen, wenn noch groß genug
    //
    if (Size >= 30) {
        int mul = 1;

        if (xPos + 30 - Size / 2 > pAim->xpos + 35)
            mul = -1;

        Gegner.PushGegner(xPos + static_cast<float>(20 * mul), yPos, SCHLEIMBOLLER, Size - 8, 5, ChangeLight);
        Gegner.PushGegner(xPos - static_cast<float>(20 * mul), yPos, SCHLEIMBOLLER, Size - 8, -5, ChangeLight);
    }
}
