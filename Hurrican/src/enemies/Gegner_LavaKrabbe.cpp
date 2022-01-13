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
    Handlung = GEGNER::NOTVISIBLE;
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
    if (Handlung == GEGNER::FALLEN) {

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
        bool mirrored = (BlickRichtung == DirectionEnum::RECHTS);

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
    if (Handlung == GEGNER::STEHEN || Handlung == GEGNER::DREHEN || Handlung == GEGNER::DREHEN2) {
        yPos = g_Fahrstuhl_yPos - 34.0f;
    }

    switch (Handlung) {
        case GEGNER::NOTVISIBLE: {
            Handlung = GEGNER::FALLEN;
            yPos = TileEngine.YOffset + 200.0f;
            yAcc = 1.0f;
            ySpeed = g_Fahrstuhl_Speed - 15.05f;
        } break;

        // Krabbe ist gelandet und dreht sich in Richtung Spieler
        case GEGNER::DREHEN: {
            AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
            if (AnimCount > AnimSpeed)  // Grenze überschritten ?
            {
                AnimCount = 0.0f;    // Dann wieder auf Null setzen
                AnimPhase--;         // Und nächste Animationsphase
                if (AnimPhase <= 0)  // Animation von zu Ende	?
                {
                    AnimPhase = 0;
                    AnimCount = 0.0f;
                    Handlung = GEGNER::DREHEN2;
                    BlickRichtung = Direction::invert(BlickRichtung);
                }
            }
        } break;

        // Krabbe ist gelandet und dreht sich in Richtung Spieler
        case GEGNER::DREHEN2: {
            AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
            if (AnimCount > AnimSpeed)  // Grenze überschritten ?
            {
                AnimCount = 0.0f;    // Dann wieder auf Null setzen
                AnimPhase++;         // Und nächste Animationsphase
                if (AnimPhase >= 4)  // Animation von zu Ende	?
                {
                    AnimPhase = 5;
                    AnimCount = 0.0f;
                    Handlung = GEGNER::STEHEN;
                }
            }
        } break;

        // Krabbe ist gelandet und Krabbelt auf den Spieler zu
        case GEGNER::STEHEN: {
            xPos += Timer.sync(5.0f * static_cast<float>(Direction::asInt(Direction::invert(BlickRichtung))));

            AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
            if (AnimCount > AnimSpeed)  // Grenze überschritten ?
            {
                AnimCount = 0.0f;     // Dann wieder auf Null setzen
                AnimPhase++;          // Und nächste Animationsphase
                if (AnimPhase >= 14)  // Animation von zu Ende	?
                    AnimPhase = 5;
            }

            // Rumdrehen ?
            if ((BlickRichtung == DirectionEnum::LINKS && xPos + 35 > pAim->xpos + 35) ||
                (BlickRichtung == DirectionEnum::RECHTS && xPos + 35 < pAim->xpos + 35)) {
                Handlung = GEGNER::DREHEN;
                AnimCount = 0.0f;
                AnimPhase = 4;
            }
        } break;

        // Krabbe fliegt vom Hintergrund ein
        case GEGNER::FALLEN: {
            if (ySpeed > g_Fahrstuhl_Speed + 60.0f) {
                ySpeed = g_Fahrstuhl_Speed + 60.0f;
                yAcc = 0.0f;
            }

            zPos += Timer.sync(9.2f);
            if (zPos > 360.0f)
                zPos = 360.0f;

            if (zPos > 200.0f)
                Destroyable = true;

            // Auf Fahrstuhl gelandet ?
            if (yPos >= g_Fahrstuhl_yPos - 34 && ySpeed > 0.0f) {
                Handlung = GEGNER::DREHEN2;
                ySpeed = 0.0f;
                yAcc = 0.0f;
                AnimPhase = 1;
                AnimEnde = 4;
                AnimSpeed = 1.0f;
                Destroyable = true;

                if (xPos + 35 < pAim->xpos + 35)
                    BlickRichtung = DirectionEnum::LINKS;
                if (xPos + 35 > pAim->xpos + 35)
                    BlickRichtung = DirectionEnum::RECHTS;
            }
        } break;

        default:
            break;
    }  // switch

    // Testen, ob der Spieler die Krabbe berührt hat
    if (Handlung == GEGNER::STEHEN || Handlung == GEGNER::DREHEN || Handlung == GEGNER::DREHEN2)
        TestDamagePlayers(Timer.sync(8.0f));
}

// --------------------------------------------------------------------------------------
// LavaKrabbe explodiert
// --------------------------------------------------------------------------------------

void GegnerLavaKrabbe::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 9000 + random(2000), SOUND::EXPLOSION3);

    PartikelSystem.PushPartikel(xPos + 15.0f, yPos + 10.0f, LAVAKRABBE_KOPF);

    for (int i = 0; i < 4; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(30)),
                                    yPos - 10.0f + static_cast<float>(random(10)), EXPLOSION_MEDIUM2);

    for (int i = 0; i < 4; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(30)),
                                    yPos - 10.0f + static_cast<float>(random(10)), LAVAKRABBE_BEIN);
}
