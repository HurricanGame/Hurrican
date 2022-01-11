// --------------------------------------------------------------------------------------
// Die Decken Krabbe
//
// Hängt an der Decke und fällt bei Beschuss oder wenn der Spieler drunter ist runter
// --------------------------------------------------------------------------------------

#include "Gegner_DeckenKrabbe.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerDeckenKrabbe::GegnerDeckenKrabbe(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    Energy = 80;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    zRot = 180.0f;
    OwnDraw = true;
    AnimSpeed = 0.75f;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerDeckenKrabbe::DoDraw() {
    bool mirror = (BlickRichtung != LINKS);

    pGegnerGrafix[GegnerArt]->RenderSpriteRotated(xPos - TileEngine.XOffset,
                                                  yPos - TileEngine.YOffset, zRot, AnimPhase,
                                                  0xFFFFFFFF, mirror);

    // Testen, ob der Spieler die Krabbe berührt hat
    TestDamagePlayers(8.0f SYNC);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerDeckenKrabbe::DoKI() {
    clampAngle(zRot);

    // runterfallen?
    //
    if (Handlung != GEGNER::STEHEN) {
        if (!(blocku & BLOCKWERT_SCHRAEGE_R) && !(blocku & BLOCKWERT_SCHRAEGE_L)) {
            blocku = TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

            if (!(blocku & BLOCKWERT_WAND) && !(blocku & BLOCKWERT_PLATTFORM)) {
                if (ySpeed == 0.0f)
                    ySpeed = 5.0f;

                yAcc = 3.0f;
            }

            else {
                ySpeed = 0.0f;
                yAcc = 0.0f;
            }
        }
    }

    if (Handlung == GEGNER::LAUFEN || Handlung == GEGNER::DREHEN || Handlung == GEGNER::DREHEN2) {
        // Schrägen checken
        //
        if (ySpeed >= 0.0f)
            // DKS - Rewrote BlockSlopes function to only take the parameters it uses:
            // blocku = TileEngine.BlockSlopes(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt], ySpeed, true);
            blocku = TileEngine.BlockSlopes(xPos, yPos, GegnerRect[GegnerArt], ySpeed);
    }

    switch (Handlung) {
        // Krabbe hängt an der Decke
        case GEGNER::STEHEN: {
            // angeschossen oder Spieler drunter? Dann runterfallen
            //
            if (DamageTaken != 0.0 || (PlayerAbstandHoriz() < 100 && PlayerAbstandVert() < 800 && pAim->ypos > yPos)) {
                // Rotationsspeed errechnen, je nachdem, wie weit der Boden entfernt ist
                //
                int a = 0;
                uint32_t b =
                    TileEngine.TileAt(static_cast<int>(xPos / TILESIZE_X),
                                      static_cast<int>(yPos / TILESIZE_Y) + a)
                        .Block;

                while (a < 20 && !(b & BLOCKWERT_WAND)) {
                    b = TileEngine.TileAt(static_cast<int>(xPos / TILESIZE_X),
                                          static_cast<int>(yPos / TILESIZE_Y) + a)
                            .Block;
                    a++;
                }

                zSpeed = 50.0f - a * 1.8f;
                ySpeed = 5.0f;
                yAcc = 5.0f;
                Handlung = GEGNER::FALLEN;
            }
        } break;

        // Krabbe ist gelandet und dreht sich in Richtung Spieler
        case GEGNER::DREHEN: {
            zRot = 0.0f;
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
                    BlickRichtung *= -1;
                }
            }
        } break;

        // Krabbe ist gelandet und dreht sich in Richtung Spieler
        case GEGNER::DREHEN2: {
            zRot = 0.0f;
            AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
            if (AnimCount > AnimSpeed)  // Grenze überschritten ?
            {
                AnimCount = 0.0f;    // Dann wieder auf Null setzen
                AnimPhase++;         // Und nächste Animationsphase
                if (AnimPhase >= 4)  // Animation von zu Ende	?
                {
                    AnimPhase = 5;
                    AnimCount = 0.0f;
                    Handlung = GEGNER::LAUFEN;
                }
            }
        } break;

        // Krabbe ist gelandet und Krabbelt auf den Spieler zu
        case GEGNER::LAUFEN: {
            // an Schrägen drehen?
            //
            if (Handlung != GEGNER::STEHEN && Handlung != GEGNER::FALLEN) {
                // schräge links hochlaufen
                if (BlickRichtung == RECHTS) {
                    if (blockl & BLOCKWERT_SCHRAEGE_L) {
                        if (zRot < 45.0f) {
                            zRot += 20.0f SYNC;
                            if (zRot > 45.0f)
                                zRot = 45.0f;
                        }
                    } else if (zRot > 0.0f) {
                        zRot -= 20.0f SYNC;
                        if (zRot < 0.0f)
                            zRot = 0.0f;
                    }
                }

                if (BlickRichtung == LINKS) {
                    if (blockr & BLOCKWERT_SCHRAEGE_R) {
                        if (zRot < 45.0f) {
                            zRot += 20.0f SYNC;
                            if (zRot > 45.0f)
                                zRot = 45.0f;
                        }
                    } else if (zRot > 0.0f) {
                        zRot -= 20.0f SYNC;
                        if (zRot < 0.0f)
                            zRot = 0.0f;
                    }
                }
            }

            xPos += 7.0f * static_cast<float>(BlickRichtung) * -1.0f SYNC;

            AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
            if (AnimCount > AnimSpeed)  // Grenze überschritten ?
            {
                AnimCount = 0.0f;     // Dann wieder auf Null setzen
                AnimPhase++;          // Und nächste Animationsphase
                if (AnimPhase >= 14)  // Animation von zu Ende	?
                    AnimPhase = 5;
            }

            // Rumdrehen ?
            if ((BlickRichtung == LINKS && xPos + 35.0f > pAim->xpos + 35.0f) ||
                (BlickRichtung == RECHTS && xPos + 35.0f < pAim->xpos + 35.0f)) {
                Handlung = GEGNER::DREHEN;
                AnimCount = 0.0f;
                AnimPhase = 4;
            }
        } break;

        // Krabbe fliegt von der Decke runter
        case GEGNER::FALLEN: {
            if (ySpeed > 30.0f) {
                ySpeed = 30.0f;
                yAcc = 0.0f;
            }

            if (zRot >= 180.0f)
                zRot += zSpeed SYNC;
            else
                zRot = 0.0f;

            // Auf Boden gelandet ?
            if (blocku & BLOCKWERT_WAND) {
                Handlung = GEGNER::DREHEN2;
                ySpeed = 0.0f;
                yAcc = 0.0f;
                AnimPhase = 1;
                AnimEnde = 4;
                AnimSpeed = 0.75f;
                Destroyable = true;

                if (xPos + 35.0f < pAim->xpos + 35.0f)
                    BlickRichtung = LINKS;
                if (xPos + 35.0f > pAim->xpos + 35.0f)
                    BlickRichtung = RECHTS;
            }
        } break;

        default:
            break;
    }  // switch
}

// --------------------------------------------------------------------------------------
// DeckenKrabbe explodiert
// --------------------------------------------------------------------------------------

void GegnerDeckenKrabbe::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 9000 + random(2000), SOUND::EXPLOSION3);

    PartikelSystem.PushPartikel(xPos + 15.0f, yPos + 10.0f, LAVAKRABBE_KOPF);

    for (int i = 0; i < 4; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(30)),
                                    yPos - 10.0f + static_cast<float>(random(10)), EXPLOSION_MEDIUM2);

    for (int i = 0; i < 4; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(30)),
                                    yPos - 10.0f + static_cast<float>(random(10)), LAVAKRABBE_BEIN);
}
