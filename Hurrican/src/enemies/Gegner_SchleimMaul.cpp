// --------------------------------------------------------------------------------------
// Der Schleimboller mit Maul
//
// Hängt in der Gegend rum und spawnt maximal 6 Baby Alien-Schleimboller
// --------------------------------------------------------------------------------------

#include "Gegner_SchleimMaul.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchleimMaul::GegnerSchleimMaul(float x, float y, int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::INIT;
    HitSound = 1;
    Energy = 200;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    AnimSpeed = (static_cast<float>(random(4) + 2)) / 10.0f;
    AnimEnde = 15;
    ShotDelay = 0.0f;
    SpawnCount = 0.0f;
    OwnDraw = true;

    // 8 Baby Schleimbollen spawnen ;)
    //
    for (int i = 0; i < 8; i++) {
        pChildren[i] = nullptr;

        // Zufallspunkt um die Mama rum
        //
        float r = static_cast<float>(random(360));

        float r1 = static_cast<float>(random(40)) + 30.0f;
        float r2 = static_cast<float>(random(40)) + 30.0f;

        // DKS - Obvious bug I am fixing here: passing a value between 0-359 to sin()/cos(), which take
        //      radian inputs, while also adding support of deg/rad cos/sin w/ lookup table support:
        // Gegner.PushGegner (x + static_cast<float>(sin(r)) * r1, y + static_cast<float>(cos(r)) * r2, SCHLEIMALIEN, 0,
        // 0, ChangeLight);
        Gegner.PushGegner(x + sin_deg(r) * r1, y + cos_deg(r) * r2, SCHLEIMALIEN, 0, 0, ChangeLight);
        pChildren[i] = Gegner.pEnd;
    }
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerSchleimMaul::DoDraw() {
    // etwas durchsichtig rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSprite(static_cast<float>(xPos - TileEngine.XOffset),
                                           static_cast<float>(yPos - TileEngine.YOffset), AnimPhase, 0xAAFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSchleimMaul::DoKI() {
    SimpleAnimation();

    // Ein Child weg? Dann neu spawnen
    for (int i = 0; i < 8; i++) {
        if (pChildren[i]->Energy <= 0.0f) {

            // Zufallspunkt um die Mama rum
            //
            float r = static_cast<float>(random(360));

            float r1 = static_cast<float>(random(40)) + 30.0f;
            float r2 = static_cast<float>(random(40)) + 30.0f;

            // DKS - Obvious bug I am fixing here: passing a value between 0-359 to sin()/cos(), which take
            //      radian inputs, while also adding support of deg/rad cos/sin w/ lookup table support:
            // Gegner.PushGegner (xPos + static_cast<float>(sin(r)) * r1, yPos + static_cast<float>(cos(r)) * r2,
            // SCHLEIMALIEN, 20, 0, ChangeLight);
            Gegner.PushGegner(xPos + sin_deg(r) * r1, yPos + cos_deg(r) * r2, SCHLEIMALIEN, 20, 0, ChangeLight);
            pChildren[i] = Gegner.pEnd;
        }
    }

    ShotDelay -= 1.0f SYNC;

    if (ShotDelay <= 0.0f) {
        ShotDelay = 5.0f + (4 - Skill) * 3.0f;

        Projectiles.PushProjectile(xPos + 30, yPos + 18, SCHLEIMSHOT);
    }

    Wegschieben(GegnerRect[GegnerArt], 4.0f);
}

// --------------------------------------------------------------------------------------
// SchleimMaul explodiert
// --------------------------------------------------------------------------------------

void GegnerSchleimMaul::GegnerExplode() {
    PartikelSystem.PushPartikel(xPos, yPos, EXPLOSION_ALIEN);

    for (int i = 0; i < 16; i++)
        PartikelSystem.PushPartikel(xPos + 15.0f + static_cast<float>(random(20)),
                                    yPos + 15.0f + static_cast<float>(random(20)), SCHLEIM2);

    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::SCHLEIM);  // Sound ausgeben

    Player[0].Score += 120;
}
