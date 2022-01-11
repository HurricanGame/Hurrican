// --------------------------------------------------------------------------------------
// Der Alien Schleimboller
//
// Hängt nur in der Gegend rum als KanonenFutter
// Besitzt evtl einen Pointer auf seinen Spawner, damit der
// Spawner weiss, wieviele children er gespawnt hat
// --------------------------------------------------------------------------------------

#include "Gegner_SchleimAlien.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchleimAlien::GegnerSchleimAlien(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    Energy = 30;
    HitSound = 1;

    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    AnimSpeed = (static_cast<float>(random(4) + 2)) / 5.0f;
    AnimEnde = 15;
    OwnDraw = true;

    // Hängt nur im Level rum? Dann schon Maximal Größe
    if (Wert1 == 0)
        Size = 60.0f;

    // ansonsten langsam wachsen
    else
        Size = static_cast<float>(Wert1);
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerSchleimAlien::DoDraw() {
    // Je nach Größe anders gestrecht rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSpriteScaled(xPos - TileEngine.XOffset + 30.0f - Size / 2.0f,
                                                 yPos - TileEngine.YOffset + 30.0f - Size / 2.0f,
                                                 static_cast<int>(Size), static_cast<int>(Size), AnimPhase, 0xAAFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSchleimAlien::DoKI() {
    SimpleAnimation();

    // Wachsen lassen
    if (Size < 60.0f) {
        DamageTaken = 0.0f;
        Energy = 2.0f;
        Destroyable = false;
        Size += 2.5f SYNC;
    } else {
        Destroyable = true;
        Size = 60.0f;

        PlattformTest(GegnerRect[GegnerArt]);

        for (int i = 0; i < NUMPLAYERS; i++)
            if (Player[i].AufPlattform == this) {
                int off;

                if (AnimPhase < 8)
                    off = -AnimPhase;
                else
                    off = AnimPhase - 15;

                Player[i].ypos = yPos - Player[i].CollideRect.bottom + GegnerRect[GegnerArt].top - off;
            } else
                Wegschieben(GegnerRect[GegnerArt], 0.0f);
    }

    // Energie anziehen
    TestDamagePlayers(4.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Alien SchleimBoller explodiert
// --------------------------------------------------------------------------------------

void GegnerSchleimAlien::GegnerExplode() {
    for (int i = 0; i < NUMPLAYERS; i++)
        if (Player[i].AufPlattform == this)
            Player[i].AufPlattform = nullptr;

    PartikelSystem.PushPartikel(xPos, yPos, EXPLOSION_ALIEN);

    for (int i = 0; i < 16; i++)
        PartikelSystem.PushPartikel(xPos + 15.0f + static_cast<float>(random(20)),
                                    yPos + 15.0f + static_cast<float>(random(20)), SCHLEIM2);

    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::SCHLEIM);  // Sound ausgeben

    Player[0].Score += 120;
}
