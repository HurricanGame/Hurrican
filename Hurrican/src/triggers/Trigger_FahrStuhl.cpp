// --------------------------------------------------------------------------------------
// Der Fahrstuhl
//
// Fährt nach unten, sobald der Spieler drauftritt
// Value1 == 0 : Fahrstuhl für das Fahrstuhl Level, sprich, wechselt das Tempo
// Value1 >  0 : Fahrtsuhl fährt ganz normal nach unten (Tempelausgang)
// --------------------------------------------------------------------------------------

#include "Trigger_Fahrstuhl.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFahrstuhl::GegnerFahrstuhl(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    BlickRichtung = LINKS;
    Energy = 100;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimPhase = 0;
    ChangeLight = Light;
    Destroyable = false;
    g_Fahrstuhl_Offset = 70.0f;
    new_ySpeed = 0.0f;
    TestBlock = false;
    OwnDraw = true;

    if (Value1 == 0) {
        TileEngine.ParallaxLayer[2].LoadImage("fahrstuhl_parallax.png", 400, 480, 400, 480, 1, 1);
        TileEngine.IsElevatorLevel = true;
    }
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerFahrstuhl::DoDraw() {
    DirectGraphics.SetFilterMode(true);

    // Schatten rendern
    float l = static_cast<float>(g_Fahrstuhl_yPos) - TileEngine.YOffset - 80.0f;
    pGegnerGrafix[GegnerArt]->RenderSpriteScaled(
        xPos - TileEngine.XOffset - 10.0f,
        static_cast<float>(g_Fahrstuhl_yPos) - TileEngine.YOffset,
        GegnerRect[GegnerArt].right + 20,
        static_cast<int>(static_cast<float>(GegnerRect[GegnerArt].bottom) + l / 5.0f),
        AnimPhase, 0x99000000);

    DirectGraphics.SetFilterMode(false);

    // Fahrstuhl rendern
    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                           g_Fahrstuhl_yPos - TileEngine.YOffset, AnimPhase,
                                           0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFahrstuhl::DoKI() {
    if (Value1 == 0)
        TileEngine.XOffset = xPos + 200 - 320.0f;

    // Feststellen ob der Hurri auf dem Fahrstuhl steht
    PlattformTest(GegnerRect[GegnerArt]);

    for (int p = 0; p < NUMPLAYERS; p++)
        if (Player[p].AufPlattform == this) {
            if (Handlung != GEGNER::FALLEN) {
                if (Value1 == 0) {
                    TileEngine.Zustand = TileStateEnum::LOCKED;
                } else if (TileEngine.Zustand != TileStateEnum::SCROLLTOPLAYER)
                    TileEngine.ScrollLevel(xPos - 50.0f, yPos - 400.0f, TileStateEnum::SCROLLTOLOCK);
            }

            Handlung = GEGNER::FALLEN;  // Fahrstuhl fährt los
        }

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        // Warten auf den Hurri =)
        case GEGNER::STEHEN: {
        } break;

        // Fahrstuhl fährt nach unten
        case GEGNER::FALLEN: {
            if (Value1 == 0)
                TileEngine.NewYOffset = yPos - 320.0f - g_Fahrstuhl_Offset;

            // Spieler "auf" den Fahrstuhl setzen
            if (Value1 != 0) {
                for (int p = 0; p < NUMPLAYERS; p++) {
                    Player[p].JumpPossible = false;
                    Player[p].ypos = yPos - Player[p].CollideRect.bottom;
                }

                if (TileEngine.Zustand == TileStateEnum::LOCKED)
                    new_ySpeed = 8.0f;
            } else
                new_ySpeed = Player[0].AutoScrollspeed;

            // Speed angleichen
            if (ySpeed < new_ySpeed)
                ySpeed += Timer.sync(0.3f);
            if (ySpeed > new_ySpeed)
                ySpeed -= Timer.sync(0.3f);

            // Funken sprühen
            static float sparkcount = 0.0f;

            sparkcount -= Timer.sync(1.0f);

            if (sparkcount < 0.0f) {
                sparkcount = 0.05f;

                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(5)),
                                            yPos + static_cast<float>(random(5)), FUNKE);
                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(5)) +
                                                static_cast<float>(GegnerRect[GegnerArt].right),
                                            yPos + static_cast<float>(random(5)), FUNKE);

                if (random(20) == 0)
                    PartikelSystem.PushPartikel(xPos - 20.0f, yPos - 10.0f, SMOKEBIG);
                if (random(20) == 0)
                    PartikelSystem.PushPartikel(xPos - 20.0f + static_cast<float>(GegnerRect[GegnerArt].right),
                                                yPos - 10.0f, SMOKEBIG);
            }

        } break;

        default:
            break;
    }  // switch

    // Endposition erreicht ? Dann explodieren (nur im Fahrstuhllevel)
    //
    if (Value1 == 0 && yPos >= Value2)
        Energy = 0.0f;

    g_Fahrstuhl_yPos = yPos;
    g_Fahrstuhl_Speed = ySpeed;
}

// --------------------------------------------------------------------------------------
// Fahrstuhl explodiert
// --------------------------------------------------------------------------------------

void GegnerFahrstuhl::GegnerExplode() {
    FahrstuhlPos = -1.0f;

    ShakeScreen(10);
    SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION2);

    for (int j = 0; j < 100; j++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(400)),
                                    yPos + static_cast<float>(random(100)), SPIDERSPLITTER);

    for (int j = 0; j < 50; j++) {
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(400)),
                                    yPos + static_cast<float>(random(100)), ROCKETSMOKE);
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(400)),
                                    yPos - 30.0f + static_cast<float>(random(100)), EXPLOSION_MEDIUM2);
    }

    for (int j = 0; j < 20; j++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(400)),
                                    yPos + static_cast<float>(random(100)), SPLITTER);

    TileEngine.Zustand = TileStateEnum::SCROLLBAR;

    // Spieler springt ab
    //
    for (int p = 0; p < NUMPLAYERS; p++) {
        Player[p].JumpPossible = false;
        Player[p].AnimPhase = 0;
        Player[p].Handlung = PlayerActionEnum::SPRINGEN;
        Player[p].JumpStart = Player[p].ypos;
        Player[p].yspeed = -PLAYER_MAXJUMPSPEED;
        Player[p].JumpAdd = 0.0f;
        Player[p].AufPlattform = nullptr;
    }
}
