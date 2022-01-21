// --------------------------------------------------------------------------------------
// Die Säule
//
// Steht in Level rum und fällt beim abschiessen um. Dabei dreht sich das Level
// --------------------------------------------------------------------------------------

#include "Trigger_Column.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerColumn::GegnerColumn(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    BlickRichtung = DirectionEnum::LINKS;
    Energy = 120;
    Value1 = Wert1;  // yPos der Plattform
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    TestBlock = false;
    OwnDraw = true;
    FallSpeed = 0.0f;
    FallValue = 0.0f;
}

// --------------------------------------------------------------------------------------
// Column KI
// --------------------------------------------------------------------------------------

void GegnerColumn::DoDraw() {
    // Rotationswinkel
    int Winkel = static_cast<int>(AnimCount);

    // Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
    //
    clampAngle(Winkel);

    glm::mat4x4 matRot = glm::rotate(glm::mat4x4(1.0f), DegreetoRad[Winkel], glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4x4 matTrans, matTrans2;  // Rotations und Translations Matrizen

    D3DXMatrixTranslation(&matTrans,
                          -(xPos - TileEngine.XOffset + 40.0f),
                          -(yPos - TileEngine.YOffset + 100.0f), 0.0f);  // Transformation zum Ursprung
    D3DXMatrixTranslation(&matTrans2,
                          xPos - TileEngine.XOffset + 40.0f,
                          yPos - TileEngine.YOffset + 100.0f,
                          0.0f);  // Transformation wieder zurück

    glm::mat4x4 matWorldLocal = glm::mat4x4(1.0f);
    matWorldLocal = matTrans * matWorldLocal;   // Verschieben
    matWorldLocal = matRot * matWorldLocal;     // rotieren
    matWorldLocal = matTrans2 * matWorldLocal;  // und wieder zurück verschieben

    // rotierte Matrix setzen
    g_matModelView = matWorldLocal * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                           yPos - TileEngine.YOffset, 0, 0xFFFFFFFF);

    // Normale Projektions-Matrix wieder herstellen
    matWorldLocal = glm::mat4x4(1.0f);
    g_matModelView = matWorldLocal * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

    SetScreenShake();
}

// --------------------------------------------------------------------------------------
// Column KI
// --------------------------------------------------------------------------------------

void GegnerColumn::DoKI() {
    // Säule wird umgeschossen ?
    //
    if (Energy < 100.0f && Handlung != GEGNER::EXPLODIEREN) {
        Handlung = GEGNER::EXPLODIEREN;

        for (int i = 0; i < 20; i++)
            PartikelSystem.PushPartikel(xPos + static_cast<float>(random(40)),
                                        yPos - 20.0f + static_cast<float>(random(15)), SMOKE);

        for (int i = 0; i < 10; i++)
            PartikelSystem.PushPartikel(xPos + static_cast<float>(random(40)),
                                        yPos - 5.0f + static_cast<float>(random(10)), ROCKSPLITTERSMALL);

        FallSpeed = 2.0f;

        SoundManager.PlayWave(100, 128, 11025 + random(2000), SOUND::STONEFALL);
        SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::COLUMN);
        PartikelSystem.SetThunderColor(255, 255, 255, 255);
    }

    switch (Handlung) {
        case GEGNER::STEHEN: {
        } break;

        // Säule fällt gerade um ?
        //
        case GEGNER::EXPLODIEREN: {
            FallSpeed += Timer.sync(0.5f);
            AnimCount += Timer.sync(FallSpeed);

            FallValue += Timer.sync(0.5f);
            if (FallValue < 6.0f)
                ScreenWinkel -= Timer.sync(0.5f);

            Energy = 100.0f;

            // Komplett umgefallen ?
            //
            if (AnimCount >= 90 || AnimCount <= -90.0f)
                Energy = 0.0f;
        } break;
    }

    // Testen, ob der Spieler die Säule berührt hat
    //
    if (Handlung == GEGNER::STEHEN)
        Wegschieben(GegnerRect[GegnerArt], 0.0f);
}

// --------------------------------------------------------------------------------------
// Column fliegt auseinander
// --------------------------------------------------------------------------------------

void GegnerColumn::GegnerExplode() {
    for (int i = 0; i < 30; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(100)),
                                    yPos + 80.0f + static_cast<float>(random(40)), SMOKE);

    for (int i = 0; i < 40; i++) {
        PartikelSystem.PushPartikel(xPos + 10.0f + static_cast<float>(random(90)),
                                    yPos + 60.0f + static_cast<float>(random(40)), ROCKSPLITTERSMALL);
        PartikelSystem.PushPartikel(xPos + 10.0f + static_cast<float>(random(90)),
                                    yPos + 60.0f + static_cast<float>(random(40)), ROCKSPLITTER);
    }

    SoundManager.PlayWave(100, 128, 11025 + random(2000), SOUND::STONEEXPLODE);

    ShakeScreen(2.0f);
}
