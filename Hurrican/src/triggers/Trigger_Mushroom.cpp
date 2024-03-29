// --------------------------------------------------------------------------------------
// Der Pilzkopp
//
// Kann vom Hurri als Trampolin benutzt werden
// --------------------------------------------------------------------------------------

#include "Trigger_Mushroom.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerMushroom::GegnerMushroom(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    BlickRichtung = DirectionEnum::LINKS;
    Energy = 100;
    Value1 = Wert1;  // yPos der Plattform
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    TestBlock = false;
    OwnDraw = true;
    ySize = 61.0f;
    count = 0.0f;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerMushroom::DoDraw() {
    // Mushroom rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSpriteScaled(xPos - TileEngine.XOffset,
                                                 yPos - TileEngine.YOffset + (61.0f - ySize) * 2.0f,
                                                 135, static_cast<int>(ySize), 0, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// Spieler springt ab
// --------------------------------------------------------------------------------------

void GegnerMushroom::PlayerJumps(PlayerClass *pPlayer) {
    if (count > 0.0f) {
        pPlayer->AufPlattform = nullptr;
        pPlayer->yspeed = -(PLAYER_MAXJUMPSPEED * (0.5f + count));

        if (pPlayer->yspeed > -PLAYER_MAXJUMPSPEED)
            pPlayer->yspeed = -PLAYER_MAXJUMPSPEED;

        pPlayer->yadd = PLAYER_JUMPADDSPEED;
        pPlayer->Handlung = PlayerActionEnum::SPRINGEN;

        SoundManager.PlayWave(100, 128, 11000, SOUND::MUSHROOMJUMP);
    }
}

// --------------------------------------------------------------------------------------
// Mushroom KI
// --------------------------------------------------------------------------------------

void GegnerMushroom::DoKI() {
    BlickRichtung = DirectionEnum::LINKS;

    switch (Handlung) {
        case GEGNER::STEHEN: {
            // Pilz wieder vergrößern
            ySize += Timer.sync(10.0f);
            if (ySize > 61.0f)
                ySize = 61.0f;

            for (int p = 0; p < NUMPLAYERS; p++)
                if (Player[p].AufPlattform == this) {
                    Handlung = GEGNER::SPRINGEN;
                    count = 2.0f;
                }
        } break;

        case GEGNER::SPRINGEN: {
            // Pilz verkleinern
            ySize -= Timer.sync(20.0f);
            if (ySize < 55.0f)
                ySize = 55.0f;

            count -= Timer.sync(1.0f);

            // Keiner mehr drauf? Dann wieder normalzustand
            bool beideweg = true;
            for (int p = 0; p < NUMPLAYERS; p++)
                if (Player[p].AufPlattform == this)
                    beideweg = false;

            if (beideweg)
                Handlung = GEGNER::STEHEN;
        } break;
    }

    PlattformTest(GegnerRect[GegnerArt]);
}

// --------------------------------------------------------------------------------------
// Mushroom explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerMushroom::GegnerExplode() {}
