// --------------------------------------------------------------------------------------
// Die Lava Krabbe
//
// Fliegt aus dem Hintergrund auf den Fahrstuhl
// --------------------------------------------------------------------------------------

#include "Gegner_LavaKrabbe.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerLavaKrabbe::GegnerLavaKrabbe(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_NOTVISIBLE;
    Energy = 90;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    zPos = 0.0f;
    OwnDraw = true;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerLavaKrabbe::DoDraw() {
    // rotierend runterfallen?
    //
    if (Handlung == GEGNER_FALLEN) {

        // Farbe setzen (leichte orangetönung)
        int r = 60 + static_cast<int>(zPos);
        int g = 20 + static_cast<int>(zPos);
        int b = 20 + static_cast<int>(zPos);

        if (r > 255)
            r = 255;
        if (g > 255)
            g = 255;
        if (b > 255)
            b = 255;

        int w = static_cast<int>(255 - DamageTaken);

        if (g > w)
            g = w;
        if (b > w)
            b = w;

        D3DCOLOR Color = D3DCOLOR_RGBA(r, g, b, 255);
        float size = zPos / 5.0f;

        size = std::clamp(size, 0.0f, 70.0f);

        size = 70.0f - size;

        pGegnerGrafix[GegnerArt]->SetRect(0, 0, 70, 34);
        pGegnerGrafix[GegnerArt]->RenderSpriteScaledRotated(xPos - TileEngine.XOffset + size / 2.0f,
                                                            yPos - TileEngine.YOffset, 70.0f - size,
                                                            34.0f - size / 2.0f, zPos * 4.0f, Color);
    }

    // oder schon auf dem Fahrstuhl?
    //
    else {
        int a = static_cast<int>(255 - DamageTaken);
        bool mirrored = (BlickRichtung == RECHTS);

        D3DCOLOR Color = D3DCOLOR_RGBA(255, a, a, 255);
        pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                               yPos - TileEngine.YOffset, AnimPhase, Color,
                                               mirrored);
    }
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerLavaKrabbe::DoKI() {
    if (Handlung == GEGNER_STEHEN || Handlung == GEGNER_DREHEN || Handlung == GEGNER_DREHEN2) {
        yPos = g_Fahrstuhl_yPos - 34.0f;
    }

    switch (Handlung) {
        case GEGNER_NOTVISIBLE: {
            Handlung = GEGNER_FALLEN;
            yPos = TileEngine.YOffset + 200.0f;
            yAcc = 1.0f;
            ySpeed = g_Fahrstuhl_Speed - 15.05f;
        } break;

        // Krabbe ist gelandet und dreht sich in Richtung Spieler
        case GEGNER_DREHEN: {
            AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
            if (AnimCount > AnimSpeed)  // Grenze überschritten ?
            {
                AnimCount = 0.0f;    // Dann wieder auf Null setzen
                AnimPhase--;         // Und nächste Animationsphase
                if (AnimPhase <= 0)  // Animation von zu Ende	?
                {
                    AnimPhase = 0;
                    AnimCount = 0.0f;
                    Handlung = GEGNER_DREHEN2;
                    BlickRichtung *= -1;
                }
            }
        } break;

        // Krabbe ist gelandet und dreht sich in Richtung Spieler
        case GEGNER_DREHEN2: {
            AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
            if (AnimCount > AnimSpeed)  // Grenze überschritten ?
            {
                AnimCount = 0.0f;    // Dann wieder auf Null setzen
                AnimPhase++;         // Und nächste Animationsphase
                if (AnimPhase >= 4)  // Animation von zu Ende	?
                {
                    AnimPhase = 5;
                    AnimCount = 0.0f;
                    Handlung = GEGNER_STEHEN;
                }
            }
        } break;

        // Krabbe ist gelandet und Krabbelt auf den Spieler zu
        case GEGNER_STEHEN: {
            xPos += 5.0f * BlickRichtung * -1 SYNC;

            AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
            if (AnimCount > AnimSpeed)  // Grenze überschritten ?
            {
                AnimCount = 0.0f;     // Dann wieder auf Null setzen
                AnimPhase++;          // Und nächste Animationsphase
                if (AnimPhase >= 14)  // Animation von zu Ende	?
                    AnimPhase = 5;
            }

            // Rumdrehen ?
            if ((BlickRichtung == LINKS && xPos + 35 > pAim->xpos + 35) ||
                (BlickRichtung == RECHTS && xPos + 35 < pAim->xpos + 35)) {
                Handlung = GEGNER_DREHEN;
                AnimCount = 0.0f;
                AnimPhase = 4;
            }
        } break;

        // Krabbe fliegt vom Hintergrund ein
        case GEGNER_FALLEN: {
            if (ySpeed > g_Fahrstuhl_Speed + 60.0f) {
                ySpeed = g_Fahrstuhl_Speed + 60.0f;
                yAcc = 0.0f;
            }

            zPos += 9.2f SYNC;
            if (zPos > 360.0f)
                zPos = 360.0f;

            if (zPos > 200.0f)
                Destroyable = true;

            // Auf Fahrstuhl gelandet ?
            if (yPos >= g_Fahrstuhl_yPos - 34 && ySpeed > 0.0f) {
                Handlung = GEGNER_DREHEN2;
                ySpeed = 0.0f;
                yAcc = 0.0f;
                AnimPhase = 1;
                AnimEnde = 4;
                AnimSpeed = 1.0f;
                Destroyable = true;

                if (xPos + 35 < pAim->xpos + 35)
                    BlickRichtung = LINKS;
                if (xPos + 35 > pAim->xpos + 35)
                    BlickRichtung = RECHTS;
            }
        } break;

        default:
            break;
    }  // switch

    // Testen, ob der Spieler die Krabbe berührt hat
    if (Handlung == GEGNER_STEHEN || Handlung == GEGNER_DREHEN || Handlung == GEGNER_DREHEN2)
        TestDamagePlayers(8.0f SYNC);
}

// --------------------------------------------------------------------------------------
// LavaKrabbe explodiert
// --------------------------------------------------------------------------------------

void GegnerLavaKrabbe::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 9000 + random(2000), SOUND_EXPLOSION3);

    PartikelSystem.PushPartikel(xPos + 15.0f, yPos + 10.0f, LAVAKRABBE_KOPF);

    for (int i = 0; i < 4; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(30)),
                                    yPos - 10.0f + static_cast<float>(random(10)), EXPLOSION_MEDIUM2);

    for (int i = 0; i < 4; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(30)),
                                    yPos - 10.0f + static_cast<float>(random(10)), LAVAKRABBE_BEIN);
}
