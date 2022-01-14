// --------------------------------------------------------------------------------------
// Der ausfahrbare Mini Drachen
//
// Siehe Turrican 2 =)
// --------------------------------------------------------------------------------------

#include "Gegner_Minidragon.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerMiniDragon::GegnerMiniDragon(int Wert1, int Wert2, bool Light) {
    Energy = 200;
    //AnimSpeed = 0.5f;
    ChangeLight = Light;
    Destroyable = false;
    Value1 = Wert1;
    Value2 = Wert2;
    OwnDraw = true;
    Segments = 6;
    ShotDelay = 4.0f;

    // Vom Drache abgeworfen?
    if (Value2 == 99) {
        ySpeed = -(13.0f + static_cast<float>(random(4)));
        yAcc = 3.0f;
        xSpeed = static_cast<float>(random(10) - 5) / 2.0f;
        Handlung = GEGNER::FALLEN;

        AnimCount = 0.0f;
        AnimStart = 5;
        AnimPhase = 5;
        AnimEnde = 15;
        AnimSpeed = 0.5f;
    } else {
        Handlung = GEGNER::INIT;
        AnimEnde = 3;
        AnimSpeed = 1.0f;
    }
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerMiniDragon::DoDraw() {

    float muly = (OldY - yPos) / 6.0f;
    float mulx = (OldX - xPos) / 6.0f;

    bool mirrored = pAim->xpos + 35.0f < xPos + 30.0f;

    for (int i = 0; i < Segments; i++)
        pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset + static_cast<float>(6 - i) * mulx,
                                               yPos - TileEngine.YOffset + static_cast<float>(6 - i) * muly, 4,
                                               0xFFFFFFFF);

    // Kopf
    if (mirrored)
        BlickRichtung = DirectionEnum::LINKS;
    else
        BlickRichtung = DirectionEnum::RECHTS;

    int a = 0;

    switch (static_cast<int>(ShotDelay)) {
        case 0:
            a = 1;
            break;

        case 8:
        case 7:
            a = 2;
            break;

        case 6:
            a = 1;
            break;

        default:
            a = 0;
    }

    if (Handlung == GEGNER::FALLEN) {
        a = AnimPhase;
        mirrored = xSpeed > 0.0f;
        BlickRichtung = DirectionEnum::LINKS; // was = 0
    }

    if (Segments == 6) {
        pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset + Direction::asInt(BlickRichtung) * 15.0f,
                                               yPos - TileEngine.YOffset, a, 0xFFFFFFFF, mirrored);
    }
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerMiniDragon::DoKI() {
    if ((blocko & BLOCKWERT_WAND && ySpeed < 0.0f) || (blocku & BLOCKWERT_WAND && ySpeed > 0.0f))
        ySpeed *= -1;

    if ((blockl & BLOCKWERT_WAND && xSpeed < 0.0f) || (blockr & BLOCKWERT_WAND && xSpeed > 0.0f))
        xSpeed *= -1;

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::FALLEN: {
            SimpleAnimation();

            if (ySpeed > 40.0f)
                ySpeed = 40.0f;

            // Am Boden angekommen?
            blocku = TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
            if (blocku & BLOCKWERT_WAND) {
                xSpeed = 0.0f;
                ySpeed = 0.0f;
                xAcc = 0.0f;
                yAcc = 0.0f;
                Handlung = GEGNER::INIT;

                AnimEnde = 0;
                AnimPhase = 0;

                SoundManager.PlayWave(100, 128, 8000 + random(8000), SOUND::EXPLOSION1);

                // int i = 0;
                for (int i = 0; i < 10; i++)
                    PartikelSystem.PushPartikel(xPos + 10.0f + static_cast<float>(random(30)),
                                                yPos + static_cast<float>(random(10)), SPIDERSPLITTER);

                for (int i = 0; i < 3; i++)
                    PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(30)),
                                                yPos - 20.0f + static_cast<float>(random(20)), EXPLOSION_MEDIUM3);
            }
        } break;

        // Position sichern
        case GEGNER::INIT: {
            OldX = xPos;
            OldY = yPos;

            // Spieler nah genug dran?
            for (int p = 0; p < NUMPLAYERS; p++)
                if (PlayerAbstandHoriz(&Player[p]) < 300 && PlayerAbstandVert(&Player[p]) < 300) {
                    Handlung = GEGNER::INIT2;

                    if (pAim->xpos + 35 < xPos + 30) {
                        xSpeed = 10.0f;
                        xAcc = -2.0f;
                    } else {
                        xSpeed = -10.0f;
                        xAcc = 2.0f;
                    }

                    if (pAim->ypos + 40 < yPos + 20) {
                        ySpeed = 10.0f;
                        yAcc = -2.0f;
                    } else {
                        ySpeed = -10.0f;
                        yAcc = 2.0f;
                    }

                    // DKS - Added function WaveIsPlaying() to SoundManagerClass:
                    if (!SoundManager.WaveIsPlaying(SOUND::STEAM) && !SoundManager.WaveIsPlaying(SOUND::SPIDERSCREAM)) {
                        SoundManager.PlayWave(80, 128, 11025, SOUND::STEAM);
                        SoundManager.PlayWave(80, 128, 18000, SOUND::SPIDERSCREAM);
                    }

                    Destroyable = true;

                    AnimCount = 5.0f;

                    break;
                }
        } break;

        // kurz nach oben "stechen"
        case GEGNER::INIT2: {
            AnimCount -= Timer.sync(1.0f);
            if (AnimCount <= 0.0f) {
                AnimCount = 0.0f;
                Handlung = GEGNER::STEHEN;
            }
        } break;

        // auf der stelle wackeln in Höhe des Hurris
        case GEGNER::STEHEN: {
            if (PlayerAbstand() < 600)
                ShotDelay -= Timer.sync(0.8f);

            if (ShotDelay < 0.0f) {
                ShotDelay = 9.0f;

                if (BlickRichtung == DirectionEnum::RECHTS)
                    WinkelUebergabe = 0.0f;
                else
                    WinkelUebergabe = 1.0f;

                SoundManager.PlayWave(100, 128, 11000 + random(2000), SOUND::FIREBALL);

                WinkelUebergabe = static_cast<float>(90 * Direction::asInt(BlickRichtung));
                Projectiles.PushProjectile(xPos + static_cast<float>(Direction::asInt(BlickRichtung) * 10), yPos, FIREBALL);
            }

            float xdiv = (pAim->xpos + 30.0f) - (OldX + 30.0f);
            float ydiv = (pAim->ypos + 35.0f) - (OldY + 20.0f);

            xdiv = std::clamp(xdiv, -80.0f, 80.0f);
            ydiv = std::clamp(ydiv, -80.0f, 80.0f);

            NewX = OldX + xdiv;
            NewY = OldY + ydiv;

            if (xPos < NewX)
                xAcc = 2.0f;
            else
                xAcc = -2.0f;

            if (yPos < NewY)
                yAcc = 2.0f;
            else
                yAcc = -2.0f;

            xSpeed = std::clamp(xSpeed, -8.0f, 8.0f);
            ySpeed = std::clamp(ySpeed, -8.0f, 8.0f);

        } break;

        // Glieder explodieren der Reihe nach
        case GEGNER::EXPLODIEREN: {
            Energy = 100.0f;
            Destroyable = false;
            ShotDelay -= Timer.sync(1.0f);

            // Ein Segment explodieren lassen
            if (ShotDelay < 0.0f) {

                float muly = (OldY - yPos) / 6.0f;
                float mulx = (OldX - xPos) / 6.0f;

                SoundManager.PlayWave(100, 128, 8000 + random(8000), SOUND::EXPLOSION1);
                PartikelSystem.PushPartikel(xPos + (6 - Segments) * mulx, yPos + (6 - Segments) * muly,
                                            EXPLOSION_MEDIUM2);

                for (int i = 0; i < 3; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(6 - Segments) * mulx +
                                                    static_cast<float>(random(40)),
                                                yPos + static_cast<float>(6 - Segments) * muly +
                                                    static_cast<float>(random(30)),
                                                SPIDERSPLITTER);

                for (int i = 0; i < 10; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(6 - Segments) * mulx +
                                                    static_cast<float>(random(40)),
                                                yPos + static_cast<float>(6 - Segments) * muly +
                                                    static_cast<float>(random(30)),
                                                FUNKE);

                ShotDelay = 2.0f;
                Segments--;

                if (Segments == 0)
                    Energy = 0.0f;
            }
        } break;
    }

    if (Energy < 100.0f && Handlung != GEGNER::EXPLODIEREN) {
        Handlung = GEGNER::EXPLODIEREN;
        Energy = 1.0f;
        ShotDelay = 0.0f;
        xSpeed = 0.0f;
        ySpeed = 0.0f;
        xAcc = 0.0f;
        yAcc = 0.0f;

        SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION3);
        PartikelSystem.PushPartikel(xPos - 26.0f, yPos - 22.0f, EXPLOSION_BIG);
    }

    // Spieler berührt ?
    //
    if (Handlung != GEGNER::EXPLODIEREN)
        TestDamagePlayers(Timer.sync(4.0f));
}

// --------------------------------------------------------------------------------------
// Drache explodiert
// --------------------------------------------------------------------------------------

void GegnerMiniDragon::GegnerExplode() {
    Player[0].Score += 250;
}
