// --------------------------------------------------------------------------------------
// Die Stachelbeere
//
// Rollt durch den Cave und klappt manchmal auf, um zu ballern =)
// --------------------------------------------------------------------------------------

#include "Gegner_Stachelbeere.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerStachelbeere::GegnerStachelbeere(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_LAUFEN;
    Energy = 120;
    BlickRichtung = LINKS;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimSpeed = 0.3f;
    AnimStart = 0;
    AnimEnde = 5;
    ChangeLight = Light;
    Destroyable = false;
    OwnDraw = true;
    RollCount = 0.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Drawroutine
// --------------------------------------------------------------------------------------

void GegnerStachelbeere::DoDraw() {

    D3DCOLOR color = 0xFFFFFFFF;

    bool mirrored = (BlickRichtung == RECHTS);

    switch (Handlung) {
        case GEGNER_LAUFEN:
        case GEGNER_LAUFEN2: {
            pGegnerGrafix[GegnerArt]->SetRect(AnimPhase * 60, 0, (AnimPhase + 1) * 60, 60);
            pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                                   yPos - TileEngine.YOffset, AnimPhase, color,
                                                   mirrored);

        } break;

        case GEGNER_OEFFNEN:
        case GEGNER_SCHLIESSEN:
        case GEGNER_SCHIESSEN: {
            pGegnerGrafix[GegnerArt]->SetRect((AnimPhase % 3) * 120, (AnimPhase / 3 + 1) * 60,
                                              (AnimPhase % 3) * 120 + 120, (AnimPhase / 3 + 2) * 60);

            if (!mirrored)
                pGegnerGrafix[GegnerArt]->RenderMirroredSprite(xPos - TileEngine.XOffset - 60.0f,
                                                               yPos - TileEngine.YOffset, color);
            else
                pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                                       yPos - TileEngine.YOffset, color);
        } break;
    }
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerStachelbeere::DoKI() {
    // Schrägen checken
    //
    if (ySpeed >= 0.0f)
        // DKS - Rewrote BlockSlopes function to only take the parameters it uses:
        // blocku = TileEngine.BlockSlopes(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt], ySpeed);
        blocku = TileEngine.BlockSlopes(xPos, yPos, GegnerRect[GegnerArt], ySpeed);

    // Runterfallen?
    if (!(blocku & BLOCKWERT_SCHRAEGE_R) && !(blocku & BLOCKWERT_SCHRAEGE_L)) {
        blocku = TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

        if (!(blocku & BLOCKWERT_WAND) && !(blocku & BLOCKWERT_PLATTFORM))
            yAcc = 8.0f;
    }

    // AnimSpeed passend zur Bewegugnsgeschwindigkeit setzen
    if (Handlung == GEGNER_LAUFEN || Handlung == GEGNER_LAUFEN2) {

        float temp = xSpeed;
        if (temp < 0.0f)
            temp *= -1;

        AnimSpeed = 2.8f - temp / 4.0f;
    }

    if (ySpeed > 0.0f) {
        if (ySpeed > 40.0f)
            ySpeed = 40.0f;

        // am Boden abhopsen?
        if (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM) {
            // Partikel erzeugen
            // int i = 0;
            for (int i = 0; i < 10; i++)
                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(60)),
                                            yPos + 50.0f, FUNKE);

            for (int i = 0; i < 5; i++)
                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(40)),
                                            yPos + 40.0f, SMOKE);

            // Geschwindigkeit umdrehen
            ySpeed *= -0.5f;

            if (ySpeed > -5.0f) {
                ySpeed = 0.0f;
                yAcc = 0.0f;
            }
        }
    }

    // Je nach Handlung richtig verhalten
    //
    switch (Handlung) {
        case GEGNER_OEFFNEN:  // Kugel klappt auf
        {
            SimpleAnimation();

            if (AnimPhase >= AnimEnde - 1) {
                ShotCount = 4;
                RollCount = 4.0f;

                Handlung = GEGNER_SCHIESSEN;
                AnimStart = -1;
                AnimPhase = AnimEnde - 2;
            }
        } break;

        case GEGNER_SCHIESSEN:  // Kugel ballert
        {
            RollCount -= 1.0f SYNC;

            if (RollCount < 0.0f) {
                int off = 25;

                if (BlickRichtung == LINKS)
                    off = -60;

                SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND_GRANATE);

                Gegner.PushGegner(xPos + static_cast<float>(ShotCount * 15 + off),
                                  yPos + 10.0f, MINIROCKET, 0, 99, false, false);

                RollCount = 2.0f;
                ShotCount--;

                if (ShotCount <= 0) {
                    Handlung = GEGNER_SCHLIESSEN;
                }
            }
        } break;

        case GEGNER_SCHLIESSEN:  // Kugel klappt wieder zu
        {
            SimpleAnimation(true);

            if (AnimPhase <= AnimStart + 1) {
                AnimStart = 0;
                AnimEnde = 5;
                Handlung = GEGNER_LAUFEN;
                RollCount = 50.0f;
                Destroyable = false;
            }
        } break;

        // abbremsen
        case GEGNER_LAUFEN2: {
            SimpleAnimation();
            bool stop = false;

            // abbremsen
            if (xSpeed > 0.0f) {
                xSpeed -= 1.0f SYNC;

                if (xSpeed < 0.0f)
                    stop = true;
            } else if (xSpeed < 0.0f) {
                xSpeed += 1.0f SYNC;

                if (xSpeed > 0.0f)
                    stop = true;
            } else
                stop = true;

            // angehalten?
            if (stop) {
                xSpeed = 0.0f;
                Handlung = GEGNER_OEFFNEN;
                Destroyable = true;
                AnimPhase = 0;
                AnimEnde = 16;
                AnimSpeed = 1.0f;
                SoundManager.PlayWave(50, 128, 12000 + random(1000), SOUND_STEAM);
            }
        } break;

        case GEGNER_LAUFEN:  // Kugel hüpft im Gang rum
        {
            if (RollCount > 0.0f)
                RollCount -= 1.0f SYNC;

            SimpleAnimation();

            if (BlickRichtung == LINKS)
                xAcc = -10.0f;
            else
                xAcc = 10.0f;

            xSpeed = std::clamp(xSpeed, -10.0f, 10.0f);

            // an der Wand umdrehen
            // oder am Screenrand, wenn der Screen gelockt ist
            if ((TileEngine.Zustand == TileStateEnum::LOCKED && BlickRichtung == LINKS && xPos < TileEngine.XOffset) ||

                (TileEngine.Zustand == TileStateEnum::LOCKED && BlickRichtung == RECHTS &&
                 xPos > TileEngine.XOffset + 640 - 50) ||

                (BlickRichtung == LINKS && (blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND)) ||

                (BlickRichtung == RECHTS && (blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND))) {
                xSpeed *= -1;
                BlickRichtung *= -1;
            }

            if (ySpeed == 0.0f && RollCount <= 0.0f && (blocku & BLOCKWERT_WAND) && PlayerAbstand() < 400) {
                Handlung = GEGNER_LAUFEN2;
                xAcc = 0.0f;
            }

        } break;

        default:
            break;
    }  // switch

    // Berührt die Kugel den Spieler ?
    //
    Wegschieben(GegnerRect[GegnerArt], 4.0f);
}

// --------------------------------------------------------------------------------------
// Stachelkugel explodiert
// --------------------------------------------------------------------------------------

void GegnerStachelbeere::GegnerExplode() {
    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(60)),
                                    yPos - 30.0f + static_cast<float>(random(60)), EXPLOSION_MEDIUM2);

    // Explosion
    PartikelSystem.PushPartikel(xPos - 15.0f,
                                yPos - 15.0f, EXPLOSION_BIG);

    SoundManager.PlayWave(100, 128, -random(2000) + 11025, SOUND_EXPLOSION4);  // Sound ausgeben

    Player[0].Score += 300;

    // noch einm paar Raketen spawnen
    Gegner.PushGegner(xPos + 15.0f, yPos + 35.0f, MINIROCKET, 0, 99, false, false);

    Gegner.PushGegner(xPos + 45.0f, yPos + 35.0f, MINIROCKET, 0, 99, false, false);

    Gegner.PushGegner(xPos + 30.0f, yPos + 20, MINIROCKET, 0, 99, false, false);
}
