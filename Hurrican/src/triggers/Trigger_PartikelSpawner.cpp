// --------------------------------------------------------------------------------------
// Der Partikel Spawner
//
// Spawnt Partikel mit Abstand Value2
// Value1 gibt die Partikel Art an, wobei
// 0 = Regen, 1 = Schnee, 2 = Rauch, 3 = Funken, 4 = Luftblasen
// 5 = Aufsteigender Rauch, 6 = Aufsteigender Rauch, der ab und zu aussetzt
// 7 = Spritzer für den Wasserfall, 8 = Wassertropfen von der Decke
// 9 = Blitz und Donner, 10 = Scrollspeed
// 11 = Fog, 12 = Funken ohne "bzzzzzt" Sound =)
// 13 = Licht flackert (wie 9, nur in schwarz und mit Sound)
// 14 = Spinnenteile
// 15 = Blätter
// 16 = Staub
// 17 = Maden
// 18 = Lavafunken
// --------------------------------------------------------------------------------------

#include "Trigger_PartikelSpawner.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPartikelSpawner::GegnerPartikelSpawner(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_STEHEN;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    DontMove = true;
    Energy = 100;
    Aussetzer = 0.0f;  // für aussetzenden, auftreibenden Rauch
    AnimCount = static_cast<float>(Value2);
    Active = true;
    pAim = &Player[0];

    // Tropfen per Zufall anfangen
    //
    if (Value1 == 8)
        AnimCount = static_cast<float>(random(Value2));

    if (Value1 == 18)
        AnimCount = 3.0f;

    if (Value1 == 11 || Value1 == 19)
        AnimCount = 0.5f;

    // gleich aktivieren, damit er schon Partikel spuckt, bevor er im Bild ist ;)
    //
    Active = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerPartikelSpawner::DoKI() {
    // Smoke Emitter geben Auftrieb beim Springen bei geradem Rauch nach oben (Value2 == 0)
    if (Value1 == 19 || ((Value1 == 5 || (Value1 == 6 && Aussetzer > 6.0f)) && Value2 == 0)) {
        for (int p = 0; p < NUMPLAYERS; p++)
            if (Player[p].yspeed != 0.0f && Player[p].xpos + Player[p].CollideRect.left < xPos + 40 &&
                xPos < Player[p].xpos + Player[p].CollideRect.right &&
                Player[p].ypos + Player[p].CollideRect.top < yPos + 40 &&
                yPos - 300 < Player[p].ypos + Player[p].CollideRect.bottom) {
                if (Value1 == 19)
                    Player[p].yspeed -= Value2 SYNC;
                else
                    Player[p].yspeed -= 6.0f SYNC;
            }
    }

    Aussetzer += 0.2f SYNC;
    if (Aussetzer > 10.0f)
        Aussetzer = 0.0f;

    // Wird ein neuer Partikel gespwant ?
    AnimCount -= 1.0f SYNC;
    if (AnimCount < 0) {
        AnimCount = static_cast<float>(Value2);

        if (Value1 == 18)
            AnimCount = 3.0f;
        if (Value1 == 11)
            AnimCount = 2.0f;

        // Partikel, die auch gespawnt werden, wenn sie nicht im Bild sind (ab bestimmtem Abstand zum Spieler)
        //
        // float pa;
        if (PlayerAbstand() < 1000) {
            // Richtigen Partikel Spawnen
            switch (Value1) {
                // Regen
                case 0: {
                    // Wäre der Partikel überhaupt im Screen?
                    float sx, sy;

                    sx = xPos - 320.0f + static_cast<float>(random(640));
                    sy = TileEngine.YOffset - 16.0f - static_cast<float>(random(64));

                    if (sx + 20.0f < TileEngine.XOffset || sx - 180.0f > TileEngine.XOffset + 640.0f)
                        break;

                    PartikelSystem.PushPartikel(sx, sy, REGENTROPFEN);
                    AnimCount = Value2 / 4.0f;
                    if (AnimCount == 0.0f)
                        AnimCount = 1.0f;
                } break;

                // Schneeflocke
                case 1: {
                    PartikelSystem.PushPartikel(xPos - 320.0f + static_cast<float>(random(640)),
                                                TileEngine.YOffset - 16.0f - static_cast<float>(random(64)),
                                                SCHNEEFLOCKE);
                    AnimCount = 0.5f;
                } break;

                // Blubberblasen
                case 4: {
                    if (PlayerAbstand() < 800)
                        PartikelSystem.PushPartikel(xPos + 12.0f, yPos + 26.0f, BUBBLE);
                } break;

                // Aufsteigender Rauch
                case 5: {
                    PartikelSystem.PushPartikel(xPos + 8.0f + static_cast<float>(random(10)),
                                                yPos + 24.0f + static_cast<float>(random(10)), SMOKE3 + Value2);
                    AnimCount = 0.25f;
                } break;

                // Aufsteigender Rauch, der ab und zu aussetzt
                case 6: {
                    if (Aussetzer > 4.0f)
                        PartikelSystem.PushPartikel(xPos + 8.0f + static_cast<float>(random(10)),
                                                    yPos + 24.0f + static_cast<float>(random(10)), SMOKE3 + Value2);

                    AnimCount = 0.25f;
                } break;

                // Wasserdampf vom Wasserfall
                case 7: {
                    for (int i = 0; i < 3; i++)
                        PartikelSystem.PushPartikel(xPos - 26.0f + static_cast<float>(random(50)),
                                                    yPos - 26.0f + static_cast<float>(random(40)), WATERFLUSH);

                } break;  // Wasserdampf

                // Wassertropfen von der Decke
                case 8: {
                    float xdrop = xPos + random(40);

                    if (xdrop < TileEngine.XOffset || xdrop > TileEngine.XOffset + 640.0f ||
                        yPos < TileEngine.YOffset - 240.0f || yPos > TileEngine.YOffset + 480.0f) {
                    } else
                        PartikelSystem.PushPartikel(xdrop, yPos, SPAWNDROP, &Player[0]);

                    AnimCount -= random(5);
                } break;

                // Blitz (und Donner)
                case 9: {
                    PartikelSystem.ThunderAlpha = 128;
                    PartikelSystem.ThunderColor[0] = 228;
                    PartikelSystem.ThunderColor[1] = 242;
                    PartikelSystem.ThunderColor[2] = 255;
                    SoundManager.PlayWave(20 + random(60), 128, 8000 + random(4000), SOUND_THUNDER);

                    Value2 = random(80) + 10;
                } break;

                // Nebel
                case 11: {
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(Value2 * 2) - Value2),
                                                yPos - 20.0f + static_cast<float>(random(40)), FOG);
                } break;

                // Licht flackert (wie blitz, nur in schwarz)
                case 13: {
                    PartikelSystem.ThunderAlpha = 160;
                    PartikelSystem.ThunderColor[0] = 0;
                    PartikelSystem.ThunderColor[1] = 0;
                    PartikelSystem.ThunderColor[2] = 0;
                    SoundManager.PlayWave(80, 128, 11025 + random(2000), SOUND_FUNKE + random(4));

                    Value2 = random(20) + 10;
                } break;

                // Spinnenteil
                case 14: {
                    PartikelSystem.PushPartikel(xPos + 4.0f, yPos + 4.0f, SPIDERPARTS);

                    AnimCount = static_cast<float>(random(5) + 10);
                } break;

                // Blätter
                case 15: {
                    WinkelUebergabe = 0.0f;

                    if (Value2 == 0) {
                        PartikelSystem.PushPartikel(TileEngine.XOffset + 640.0f,
                                                    yPos - 240.0f + static_cast<float>(random(480)), BLATT2);
                        AnimCount = (random(5) + 1) / 5.0f;
                    } else {
                        PartikelSystem.PushPartikel(TileEngine.XOffset + 640.0f,
                                                    yPos - 240.0f + static_cast<float>(random(480)), DUST);
                        AnimCount = (random(5) + 1) / 2.0f;
                    }

                } break;

                // Staub
                case 16: {
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(200) - 100),
                                                yPos + static_cast<float>(random(200) - 100), DUST);

                    AnimCount = (random(5) + 1) / 5.0f;
                } break;

                case 18: {
                    AnimCount = 3.0f;
                    PartikelSystem.PushPartikel(xPos - (Value2) + rand() % (Value2 * 2), yPos, LAVADUST);
                } break;

                // Aufsteigender Rauch mit Auftrieb (Value2)
                case 19: {
                    if (random(3) == 0)
                        PartikelSystem.PushPartikel(xPos - 10.0f, yPos - 20.0f, SMOKEBIG);
                    else
                        PartikelSystem.PushPartikel(xPos + 8.0f + static_cast<float>(random(10)),
                                                    yPos + 24.0f + static_cast<float>(random(10)), SMOKE3);

                    AnimCount = 0.25f;
                }
            }  // switch
        }

        // Partikel, die nur gespwant werden, wenn der Trigger im Bild ist
        // Ist der Trigger im Bild ?
        if (xPos + 40 > TileEngine.XOffset && xPos < TileEngine.XOffset + 640 && yPos + 40 > TileEngine.YOffset &&
            yPos < TileEngine.YOffset + 480) {
            // Richtigen Partikel Spawnen
            switch (Value1) {
                // Rauch
                case 2: {
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(5)), yPos + 24.0f, SMOKE);
                } break;

                // Funken
                case 3:
                case 12: {
                    if (random(4) == 0) {
                        // mit Sound?
                        if (Value1 == 3)
                            SoundManager.PlayWave3D(static_cast<int>(xPos), static_cast<int>(yPos), 11025 + random(1000), SOUND_FUNKE);

                        PartikelSystem.PushPartikel(xPos, yPos - 24.0f, LASERFLAME);

                        for (int i = 0; i < 10; i++) {
                            PartikelSystem.PushPartikel(xPos + 19.0f, yPos, FUNKE);
                            PartikelSystem.PushPartikel(xPos + 19.0f, yPos, LASERFUNKE2);
                        }
                    }
                } break;

                // Scrollspeed setzen und danach Trigger entfernen
                case 10: {
                    Player[0].AutoScrollspeed = static_cast<float>(Value2);
                    Energy = 0.0f;
                } break;

                // Maden
                case 17: {
                    Gegner.PushGegner(xPos + static_cast<float>(random(20) - 10),
                                      yPos + 16.0f, MADE,
                                      0, 0, false, false);
                } break;
            }  // switch
        }
    }
}

// --------------------------------------------------------------------------------------
// PartikelSpawner explodiert (nicht *g*)
// --------------------------------------------------------------------------------------

void GegnerPartikelSpawner::GegnerExplode() {}
