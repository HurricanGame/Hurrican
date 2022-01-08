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
    Handlung = GEGNER_STEHEN;
    BlickRichtung = LINKS;
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
    glm::mat4x4 matWorldLocal, matRot, matTrans, matTrans2;  // Rotations und Translations Matrizen
    int Winkel;                                             // Rotationswinkel

    Winkel = static_cast<int>(AnimCount);

    // Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
    //
    clampAngle(Winkel);

    matRot = glm::rotate(glm::mat4x4(1.0f), DegreetoRad[Winkel], glm::vec3(0.0f, 0.0f, 1.0f));

    D3DXMatrixTranslation(&matTrans, static_cast<float>(-(xPos - TileEngine.XOffset + 40)),
                          static_cast<float>(-(yPos - TileEngine.YOffset + 100)), 0.0f);  // Transformation zum Ursprung
    D3DXMatrixTranslation(&matTrans2, static_cast<float>(xPos - TileEngine.XOffset + 40), static_cast<float>(yPos - TileEngine.YOffset + 100),
                          0.0f);  // Transformation wieder zurück

    matWorldLocal = glm::mat4x4(1.0f);
    matWorldLocal = matTrans * matWorldLocal;   // Verschieben
    matWorldLocal = matRot * matWorldLocal;     // rotieren
    matWorldLocal = matTrans2 * matWorldLocal;  // und wieder zurück verschieben

    // rotierte Matrix setzen
    g_matModelView = matWorldLocal * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

    pGegnerGrafix[GegnerArt]->RenderSprite(static_cast<float>(xPos - TileEngine.XOffset),
                                           static_cast<float>(yPos - TileEngine.YOffset), 0, 0xFFFFFFFF);

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
    if (Energy < 100.0f && Handlung != GEGNER_EXPLODIEREN) {
        Handlung = GEGNER_EXPLODIEREN;

        for (int i = 0; i < 20; i++)
            PartikelSystem.PushPartikel(xPos + random(40), yPos + random(15) - 20, SMOKE);

        for (int i = 0; i < 10; i++)
            PartikelSystem.PushPartikel(xPos + random(40), yPos + random(10) - 5, ROCKSPLITTERSMALL);

        FallSpeed = 2.0f;

        SoundManager.PlayWave(100, 128, 11025 + random(2000), SOUND_STONEFALL);
        SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND_COLUMN);
        PartikelSystem.ThunderAlpha = 255;
        PartikelSystem.ThunderColor[0] = 255;
        PartikelSystem.ThunderColor[1] = 255;
        PartikelSystem.ThunderColor[2] = 255;
        PartikelSystem.ThunderAlpha = 255;
    }

    switch (Handlung) {
        case GEGNER_STEHEN: {
        } break;

        // Säule fällt gerade um ?
        //
        case GEGNER_EXPLODIEREN: {
            FallSpeed += 0.5f SYNC;
            AnimCount += FallSpeed SYNC;

            FallValue += 0.5f SYNC;
            if (FallValue < 6.0f)
                ScreenWinkel -= 0.5f SYNC;

            Energy = 100.0f;

            // Komplett umgefallen ?
            //
            if (AnimCount >= 90 || AnimCount <= -90.0f)
                Energy = 0.0f;
        } break;
    }

    // Testen, ob der Spieler die Säule berührt hat
    //
    if (Handlung == GEGNER_STEHEN)
        Wegschieben(GegnerRect[GegnerArt], 0.0f);
}

// --------------------------------------------------------------------------------------
// Column fliegt auseinander
// --------------------------------------------------------------------------------------

void GegnerColumn::GegnerExplode() {
    for (int i = 0; i < 30; i++)
        PartikelSystem.PushPartikel(xPos + random(100), yPos + 80 + random(40), SMOKE);

    for (int i = 0; i < 40; i++) {
        PartikelSystem.PushPartikel(xPos + random(90) + 10, yPos + 60 + random(40), ROCKSPLITTERSMALL);
        PartikelSystem.PushPartikel(xPos + random(90) + 10, yPos + 60 + random(40), ROCKSPLITTER);
    }

    SoundManager.PlayWave(100, 128, 11025 + random(2000), SOUND_STONEEXPLODE);

    ShakeScreen(2.0f);
}
