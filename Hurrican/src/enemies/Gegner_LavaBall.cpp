// --------------------------------------------------------------------------------------
// Der Lava Ball
//
// Hopst aus der Lava raus und wieder rein
// --------------------------------------------------------------------------------------

#include "Gegner_LavaBall.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerLavaBall::GegnerLavaBall(int Wert1, int Wert2, bool Light) {
    Active = true;
    Handlung = GEGNER_EINFLIEGEN;
    Energy = 100;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimStart = 0;
    AnimEnde = 19;
    AnimSpeed = 0.5f;
    ChangeLight = Light;
    Destroyable = false;
    ySpeed = static_cast<float>(-Value1);
    yAcc = 4.0f;
    SmokeDelay = 0.0f;
    FlareDelay = 0.0f;
    InLava = true;
    OwnDraw = true;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerLavaBall::DoDraw() {
    // Render
    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                           yPos - TileEngine.YOffset, AnimPhase, 0xFFFFFFFF);

    // Flare rendern
    DirectGraphics.SetAdditiveMode();
    float w = yPos / 4;
    Projectiles.LavaFlare.RenderSpriteScaledRotated(xPos - 60.0f - TileEngine.XOffset,
                                                    yPos - 60.0f - TileEngine.YOffset, 180, 180, w, 0xFFFFCC88);
    DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerLavaBall::DoKI() {
    SimpleAnimation();

    // erst erschienen? Dann yPos merken
    //
    if (Handlung == GEGNER_EINFLIEGEN) {
        yStart = yPos;
        Handlung = GEGNER_LAUFEN;
        return;
    }

    if (TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]) & BLOCKWERT_LIQUID) {
        // eben erst reingefallen?
        //
        if (InLava == false) {
            for (int i = 0; i < 12; i++)
                PartikelSystem.PushPartikel(xPos + 16.0f + static_cast<float>(random(20)),
                                            yPos + 8.0f + static_cast<float>(GegnerRect[GegnerArt].bottom),
                                            LAVA_SPRITZER2);

            SoundManager.PlayWave3D(static_cast<int>(xPos) + 30,
                                    static_cast<int>(yPos) + 30, 10000 + random(2050),
                                    SOUND_WATERIN);
        }

        InLava = true;
    } else {
        // eben erst rausgekommen?
        //
        if (InLava == true) {
            for (int i = 0; i < 12; i++)
                PartikelSystem.PushPartikel(xPos + 16.0f + static_cast<float>(random(20)),
                                            yPos + 8.0f + static_cast<float>(GegnerRect[GegnerArt].bottom),
                                            LAVA_SPRITZER);

            SoundManager.PlayWave3D(static_cast<int>(xPos) + 30,
                                    static_cast<int>(yPos) + 30, 10000 + random(2050),
                                    SOUND_WATEROUT);
        }

        InLava = false;
    }

    // Rauchen lassen und Funken erzeugen
    //
    SmokeDelay -= 1.0f SYNC;

    if (SmokeDelay < 0.0f) {
        SmokeDelay = 0.5f;

        if (InLava == false)
            PartikelSystem.PushPartikel(xPos - 20.0f + static_cast<float>(random(50)),
                                        yPos + static_cast<float>(random(40)), SMOKEBIG);
    }

    FlareDelay -= 1.0f SYNC;

    if (FlareDelay < 0.0f) {
        FlareDelay = 2.0f;
        // PartikelSystem.PushPartikel (xPos - 16 + rand ()%56, yPos  - 16 + rand ()%56, MINIFLARE);
    }

    // Maximale Fall-Geschwindigkeit
    //
    if (ySpeed > 50.0f)
        ySpeed = 50.0f;

    // verschwinden?
    //
    if (ySpeed > 0.0f && yPos > yStart)
        Energy = 0.0f;

    // Spieler berührt
    // Explosion passiert schon hier und nicht erst im Destruktor, da der Ball auch, wenn er wieder in die Lava
    // zurückfliegt, zerstört wird, aber dabei nicht explodieren soll
    //
    for (int i = 0; i < NUMPLAYERS; i++)
        if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], Player[i].xpos, Player[i].ypos, Player[i].CollideRect) ==
            true) {
            Player[i].DamagePlayer(50.0f);
            Energy = 0.0f;

            SoundManager.PlayWave(100, 128, 9000 + random(2000), SOUND_EXPLOSION1);

            PartikelSystem.PushPartikel(xPos - 10.0f, yPos - 30.0f, EXPLOSION_GIANT);

            for (int j = 0; j < 4; j++)
                PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(40)),
                                            yPos - 10.0f + static_cast<float>(random(40)), EXPLOSION_MEDIUM2);

            for (int j = 0; j < 20; j++)
                PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(50)),
                                            yPos - 16.0f + static_cast<float>(random(56)), MINIFLARE);

            for (int j = 0; j < 20; j++)
                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(40)),
                                            yPos + static_cast<float>(random(40)), SMOKE2);
        }
}

// --------------------------------------------------------------------------------------
// LavaBall explodiert
// --------------------------------------------------------------------------------------

void GegnerLavaBall::GegnerExplode() {}
