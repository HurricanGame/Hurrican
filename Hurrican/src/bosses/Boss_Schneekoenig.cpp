// --------------------------------------------------------------------------------------
// Der Schneekoenig
//
// Ist ein Fetter Sack, der den Spieler des öfteren nervt
// --------------------------------------------------------------------------------------

#include "Boss_Schneekoenig.hpp"
#include "stdafx.hpp"

int DamageAnims[4] = {1, 2, 1, 0};

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchneeKoenig::GegnerSchneeKoenig(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_INIT;
    BlickRichtung = LINKS;
    Energy = 50000;
    LastEnergy = 50000;

    ChangeLight = Light;
    Destroyable = false;
    Value1 = Wert1;
    Value2 = Wert2;
    TestBlock = false;
    OwnDraw = true;
    yOffset = 0.0f;
    SmokeDelay = 0.0f;
    KnarreWinkel = 0.0f;
    KnarreY = 60.0f;
    SpezialAktion = Value2;
    ShotDelay = 0.0f;
    Action = -1;
    ShotCount = 0;
    GunSlide = 0.0f;
    WackelCount = 0.0f;
    WackelAnim = 0.0f;
    DrawNow = false;

    Knarre.LoadImage("schneekoenig_knarre.png", 21, 94, 21, 94, 1, 1);
    Laser.LoadImage("redlaser.png", 32, 32, 32, 32, 1, 1);
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerSchneeKoenig::DoDraw() {
    if (!DrawNow)
        return;

    BlickRichtung = LINKS;

    static float xoff = 0;

    D3DCOLOR Color = 0xFFFFFFFF;

    // int	Wert = 255-(static_cast<int>(DamageTaken));  // PICKLE not used

    if (Handlung == GEGNER_AUSFAHREN || WackelCount > 0.0f) {
        xoff += 15.0f SYNC;

        if (WackelCount > 0.0f)
            WackelCount -= 1.0f SYNC;
    }

    // Knarre
    float GunMod;

    if (KnarreWinkel > 0.0f)
        GunMod = 1.0f;
    else
        GunMod = -1.0f;

    Knarre.RenderSpriteRotatedOffset(xPos - TileEngine.XOffset + 95.0f + sin(xoff) * 1.5f,
                                     yPos - TileEngine.YOffset + yOffset - 36.0f + KnarreY, KnarreWinkel,
                                     GunSlide * GunMod, 10, Color, false);

    // Body
    if (WackelAnim > 0.0f) {
        AnimPhase = 10 - DamageAnims[static_cast<int>(WackelAnim) % 4];
        WackelAnim -= 2.0f SYNC;

        if (WackelAnim < 0.0f)
            AnimPhase = 10;
    }

    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset + sin(xoff) * 5.0f,
                                           yPos - TileEngine.YOffset + yOffset, AnimPhase, Color, false);

    //	RenderLaser();
}

// --------------------------------------------------------------------------------------
// Laser rendern
// --------------------------------------------------------------------------------------

void GegnerSchneeKoenig::RenderLaser() {

    // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
    // w = (KnarreWinkel + 180.0f) * PI / 180.0f;
    // Laser.RenderSpriteRotatedOffset(static_cast<float>(xPos - TileEngine.XOffset) + 90.0f + (float)sin(w) * 70.0f,
    //                                static_cast<float>(yPos - TileEngine.YOffset) + 5.0f + (float)cos(w) * 70.0f,
    //                                KnarreWinkel, 0, 0, 0xFFFFFFFF);
    float w = KnarreWinkel + 180.0f;
    Laser.RenderSpriteRotatedOffset(xPos - TileEngine.XOffset + 90.0f + sin_deg(w) * 70.0f,
                                    yPos - TileEngine.YOffset + 5.0f + cos_deg(w) * 70.0f, KnarreWinkel, 0, 0,
                                    0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// Winkel zum Spieler ausrechnen
// --------------------------------------------------------------------------------------

float GegnerSchneeKoenig::WinkelToPlayer() {

    // Abstände berechnen
    float dx = (xPos + 100.0f) - (pAim->xpos + 35.0f);

    return dx / 10.0f;
}

// --------------------------------------------------------------------------------------
// Neuen Angriff festlegen
// --------------------------------------------------------------------------------------

void GegnerSchneeKoenig::NextAction(int NewAction) {
    static int LastAttack = 0;
    static int NextAttack = 0;

    Handlung = GEGNER_EINFLIEGEN;

    // Neuen Angriff festgelegt?
    if (NewAction != -1)
        NextAttack = NewAction;

    // oder per Zufall?
    else {
        // if (Value1 > 0)
        NextAttack = LastAttack + 1;

        if (NextAttack > 3 || (NextAttack == 2 && Value1 == 1))
            NextAttack = 0;
    }

    switch (NextAttack) {
        // Schneekoppen ballern
        case 0: {
            Action = GEGNER_INIT2;
        } break;

        // Knarre einziehen und springen
        case 1: {
            Action = GEGNER_INIT3;
        } break;

        // Knarre einziehen, von links nach rechts hopsen und wieder zurück
        // und dabei Blaue Bomben abwerfen
        case 2: {
            Action = GEGNER_INIT2;
        } break;

        // Schneekoppen ballern
        case 3: {
            Action = GEGNER_INIT4;
        } break;
    }

    LastAttack = NextAttack;

    KnarreWinkel = 0.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSchneeKoenig::DoKI() {
    // Energie anzeigen
    if (Handlung != GEGNER_INIT && Handlung != GEGNER_EXPLODIEREN)
        HUD.ShowBossHUD(50000, Energy);

    // Boss aktivieren und Mucke laufen lassen
    //
    if (Active && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        SoundManager.StopSong(MUSIC_STAGEMUSIC, true);  // Ausfaden und pausieren
        SoundManager.PlaySong(MUSIC_BOSS, false);
        ySave = yPos;

        // kommt von oben in der mitte des screens runter
        xPos = TileEngine.XOffset + (640.0f - 140.0f) / 2.0f;
        yPos = TileEngine.YOffset - 300.0f;
        ySpeed = 50.0f;
        DrawNow = true;
        TileEngine.Zustand = TileStateEnum::LOCKED;
    }

    // Kanone zurückfahren
    if (GunSlide > 0.0f)
        GunSlide -= 1.0f SYNC;
    else
        GunSlide = 0.0f;

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        // Warten bis der Screen zentriert wurde
        case GEGNER_INIT: {
            // auf den Boden geknallt?
            if (yPos > ySave) {
                yPos = ySave;
                ShakeScreen(3.0f);
                Handlung = GEGNER_INIT2;
                ySpeed = 0.0f;
                AnimCount = 20.0f;

                for (int i = 0; i < 30; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(180) - 30),
                                                yPos + static_cast<float>(random(40) + 70), SNOWFLUSH);

                for (int i = 0; i < 10; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(130) - 10),
                                                yPos + static_cast<float>(random(40) + 60), SMOKEBIG);

                TileEngine.ScrollLevel(TileEngine.XOffset, yPos - 320.0f, TileStateEnum::SCROLLTOLOCK);

                SoundManager.PlayWave(50, 128, 11025, SOUND_DOORSTOP);
            }
        } break;

        // kurz abwarten nach dem Runterfallen und Kamera hochscrollen
        case GEGNER_INIT2: {
            AnimCount -= 1.0f SYNC;

            if (AnimCount < 0.0f) {
                Handlung = GEGNER_EINFLIEGEN;
                Action = GEGNER_EINFLIEGEN;
                AnimEnde = 12;
                AnimSpeed = 1.0f;

                SoundManager.PlayWave(100, 128, 11025, SOUND_STEAM);
            }
        } break;

        // Bein ausfahren
        case GEGNER_EINFLIEGEN: {
            switch (Action) {
                case GEGNER_EINFLIEGEN: {
                    SimpleAnimation();

                    yOffset = -AnimPhase * 8.0f;

                    if (yOffset < -40.0f)
                        yOffset = -40.0f;

                    if (AnimPhase >= AnimEnde - 1) {
                        yOffset = 0.0f;
                        yPos -= 40.0f;
                        AnimPhase = 10;
                        GegnerRect[GegnerArt].bottom = 150;

                        NextAction(0);
                    }

                } break;

                case GEGNER_INIT2: {
                    KnarreY -= 5.0f SYNC;

                    if (KnarreY < 0.0f) {
                        KnarreY = 0.0f;
                        Action = -1;
                        Handlung = GEGNER_VERFOLGEN;
                        ShotDelay = 1.0f;
                        ShotCount = 10 + random(6);
                    }
                } break;

                case GEGNER_INIT3: {
                    KnarreY -= 5.0f SYNC;

                    if (KnarreY < 0.0f) {
                        KnarreY = 0.0f;
                        Action = GEGNER_STEHEN;
                        Handlung = GEGNER_SPRINGEN;
                        AnimPhase = 10;
                        AnimCount = 1.0f;
                        ShotCount = 6;

                        SoundManager.PlayWave(100, 128, 11025, SOUND_STEAM);

                        for (int p = 0; p < NUMPLAYERS; p++)
                            if (Player[p].AufPlattform == this)
                                Player[p].AufPlattform = nullptr;
                    }
                } break;

                case GEGNER_INIT4: {
                    KnarreY -= 5.0f SYNC;

                    if (KnarreY < 0.0f) {
                        KnarreY = 0.0f;
                        Action = GEGNER_BOMBARDIEREN;
                        Handlung = GEGNER_SPRINGEN;
                        AnimPhase = 10;
                        AnimCount = 1.0f;
                        ShotCount = 6;
                        ShotDelay = 1.5f;

                        SoundManager.PlayWave(100, 128, 11025, SOUND_STEAM);

                        for (int p = 0; p < NUMPLAYERS; p++)
                            if (Player[p].AufPlattform == this)
                                Player[p].AufPlattform = nullptr;
                    }
                } break;

            }  // switch (Action)

        } break;

        case GEGNER_SPRINGEN: {
            // Kanone zurückfahren
            if (KnarreY < 60.0f)
                KnarreY += 5.0f SYNC;
            else {
                KnarreY = 60.0f;

                switch (Action) {
                    // abspringen
                    case GEGNER_STEHEN:
                    case GEGNER_BOMBARDIEREN: {
                        AnimCount -= 4.0f SYNC;
                        if (AnimCount <= 0.0f) {
                            AnimCount = 1.0f;
                            AnimPhase--;

                            if (AnimPhase < 5) {
                                // Auf Spieler springen?
                                if (Action == GEGNER_STEHEN) {
                                    Action = GEGNER_SPRINGEN;
                                    ySpeed = -80.0f;
                                    yAcc = 18.0f;
                                    xSpeed = PlayerAbstandHoriz() / 8.0f;

                                    if (xPos > pAim->xpos)
                                        xSpeed *= -1.0f;
                                }

                                // an den Rand springen?
                                else {
                                    float target;

                                    if (xPos + 100 > TileEngine.XOffset + 320.0f)
                                        target = TileEngine.XOffset + 100.0f;
                                    else
                                        target = TileEngine.XOffset + 540.0f;

                                    // zurück zur Mitte springen
                                    if (ShotCount == 1)
                                        target = static_cast<float>(TileEngine.XOffset) + 320.0f;

                                    Action = GEGNER_AUSSPUCKEN;
                                    ySpeed = -110.0f;
                                    yAcc = 16.0f;
                                    xSpeed = (target - (xPos + 100.0f)) / 12.0f;

                                    // if (xPos > target)
                                    //	xSpeed *= -1.0f;
                                }
                            }
                        }
                    } break;

                    // aufkommen
                    case GEGNER_SCHLIESSEN:
                    case GEGNER_AUSSPUCKENZWEI: {
                        AnimCount -= 4.0f SYNC;
                        if (AnimCount <= 0.0f) {
                            AnimCount = 1.0f;
                            AnimPhase++;

                            if (AnimPhase > 10) {
                                AnimPhase = 10;

                                if (Action == GEGNER_SCHLIESSEN)
                                    Action = GEGNER_STEHEN;
                                else
                                    Action = GEGNER_BOMBARDIEREN;

                                ShotCount--;

                                if (ShotCount <= 0) {
                                    Handlung = GEGNER_EINFLIEGEN;
                                    NextAction();
                                }
                            }
                        }
                    } break;

                    case GEGNER_SPRINGEN:
                    case GEGNER_AUSSPUCKEN: {
                        if (Action == GEGNER_AUSSPUCKEN && ShotCount > 1) {
                            ShotDelay -= 1.0f SYNC;

                            if (ShotDelay <= 0.0f) {
                                ShotDelay = 2.0f;

                                Projectiles.PushProjectile(xPos + 100.0f, yPos + 140.0f, BLUEBOMB);
                            }
                        }

                        if (ySpeed > 0.0f) {
                            // auf den Boden geknallt?
                            if (yPos > (ySave - 40.0f)) {
                                for (int i = 0; i < 30; i++)
                                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(140) + 10),
                                                                yPos + static_cast<float>(random(40) + 110),
                                                                SNOWFLUSH);

                                yPos = ySave - 40.0f;
                                ShakeScreen(3.0f);

                                xSpeed = 0.0f;
                                ySpeed = 0.0f;
                                xAcc = 0.0f;
                                yAcc = 0.0f;

                                if (Action == GEGNER_SPRINGEN)
                                    Action = GEGNER_SCHLIESSEN;
                                else
                                    Action = GEGNER_AUSSPUCKENZWEI;

                                // Spieler zerquetscht?
                                TestDamagePlayers(500.0f);

                                SoundManager.PlayWave(50, 128, 11025, SOUND_DOORSTOP);
                            }
                        }
                    } break;
                }
            }
        } break;

        case GEGNER_AUSFAHREN: {
            // Kanone zurück
            if (KnarreWinkel < 0.0f)
                KnarreWinkel += 5.0f SYNC;

            if (KnarreWinkel > 0.0f)
                KnarreWinkel -= 5.0f SYNC;

            if (KnarreY < 60.0f)
                KnarreY += 5.0f SYNC;
            else {
                KnarreY = 60.0f;
                AnimCount -= 1.0f SYNC;

                // losfliegen
                if (AnimCount < 0.0f && yAcc == 0.0f) {
                    ySpeed = -5.0f;
                    yAcc = -0.6f;
                }

                if (yPos < TileEngine.YOffset - 300.0f)
                    Energy = 0.0f;

                // Auf der Hälfte explodieren lassen?
                if (Value1 >= 2 && yPos < TileEngine.YOffset + 150.0f) {
                    SoundManager.PlayWave(100, 128, 11025, SOUND_EXPLOSION2);
                    ShakeScreen(5.0f);

                    for (int i = 0; i < 50; i++)
                        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(150) - 20),
                                                    yPos + static_cast<float>(random(70)), SMOKEBIG);

                    for (int i = 0; i < 20; i++)
                        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(200) - 40),
                                                    yPos + static_cast<float>(random(100)), EXPLOSION_MEDIUM2);

                    for (int i = 0; i < 20; i++)
                        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(150)),
                                                    yPos + static_cast<float>(random(70)), SCHROTT1);

                    for (int i = 0; i < 5; i++)
                        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(180) - 40),
                                                    yPos + static_cast<float>(random(100)), EXPLOSION_BIG);

                    for (int i = 0; i < 5; i++)
                        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(200)),
                                                    yPos + static_cast<float>(random(20) + 70), SPLITTER);

                    Energy = 0.0f;
                }

                // Dampf ablassen =)
                SmokeDelay -= 1.0f SYNC;

                if (SmokeDelay < 0.0f) {
                    SmokeDelay = 0.5f;
                    PartikelSystem.PushPartikel(xPos + 60.0f + static_cast<float>(random(20)),
                                                yPos + 120.0f + yOffset, SMOKEBIG);
                    PartikelSystem.PushPartikel(xPos + 95.0f + static_cast<float>(random(20)),
                                                yPos + 140.0f + yOffset, FUNKE);

                    Projectiles.PushProjectile(xPos + 75.0f + static_cast<float>(random(4)),
                                               yPos + 80.0f + yOffset, FEUERFALLE3);

                    // Explodieren?
                    if (Value1 >= 2 && random(2) == 0) {
                        SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND_EXPLOSION1);
                        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(150)),
                                                    yPos + static_cast<float>(random(100)), EXPLOSION_MEDIUM2);
                    }
                }

                int a = static_cast<int>((yPos - TileEngine.YOffset - 200.0f) / 10.0f);
                a = std::clamp(a, 0, 10);

                AnimPhase = a;
            }
        } break;

        // Auf Spieler ballern
        case GEGNER_VERFOLGEN: {
            // Winkel zum Spieler ausrechnen

            float const aim = WinkelToPlayer();

            if (KnarreWinkel < aim) {
                KnarreWinkel += 8.0f SYNC;

                if (KnarreWinkel > aim)
                    KnarreWinkel = aim;
            }

            if (KnarreWinkel > aim) {
                KnarreWinkel -= 8.0f SYNC;

                if (KnarreWinkel < aim)
                    KnarreWinkel = aim;
            }

            if (PlayerAbstandHoriz() < 100 && Value1 > 0)
                ShotCount--;
            else if (abs(static_cast<int>(aim - KnarreWinkel)) < 10.0f)
                ShotDelay -= 1.0f SYNC;

            if (ShotDelay <= 0.0f) {
                ShotDelay = 8.0f;

                float w;

                if (KnarreWinkel <= 0.0f)
                    w = 180 - KnarreWinkel;
                else
                    w = KnarreWinkel - 180;

                // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
                // for (int p = 0; p < 2; p++)
                //    PartikelSystem.PushPartikel(xPos + (float)sin((KnarreWinkel + 180.0f) * PI / 180.0f) * 70.0f + 80,
                //                                  yPos + (float)cos((KnarreWinkel + 180.0f) * PI / 180.0f) * 60.0f +
                //                                  KnarreY + yOffset, SMOKEBIG);

                // Gegner.PushGegner(xPos + (float)sin((KnarreWinkel + 180.0f) * PI / 180.0f) * 70.0f + 100,
                //                    yPos + (float)cos((KnarreWinkel + 180.0f) * PI / 180.0f) * 60.0f + KnarreY + 5.0f
                //                    + yOffset, SCHNEEKOPPE, -static_cast<int>(w) - 3 + rand()%7 , 40, false);
                for (int p = 0; p < 2; p++)
                    PartikelSystem.PushPartikel(xPos + sin_deg(KnarreWinkel + 180.0f) * 70.0f + 80.0f,
                                                yPos + cos_deg(KnarreWinkel + 180.0f) * 60.0f + KnarreY + yOffset,
                                                SMOKEBIG);

                Gegner.PushGegner(xPos + sin_deg(KnarreWinkel + 180.0f) * 70.0f + 100.0f,
                                  yPos + cos_deg(KnarreWinkel + 180.0f) * 60.0f + KnarreY + 5.0f + yOffset, SCHNEEKOPPE,
                                  -static_cast<int>(w) - 3 + random(7), 40, false);

                SoundManager.PlayWave(100, 128, 8000 + random(1000), SOUND_GRANATE);

                GunSlide = 5.0f;

                // Boss kommt nicht zum ersten mal? Dann Shusscount abziehen
                if (Value1 > 0)
                    ShotCount--;
            }

            if (ShotCount <= 0)
                NextAction();
        } break;

        default:
            break;
    }  // switch

    // Boss hat Schaden genommen?
    if (DamageTaken > 0.0f) {
        // War es eine seiner eigenen Bomben? Dann zieht das ganz schön was ab =)
        if (LastEnergy - Energy > 5000) {
        } else
            DamageTaken = 0.0f;

        LastEnergy = Energy;
    }

    GegnerRect[GegnerArt].top = 60 + static_cast<int>(yOffset);

    if (Handlung != GEGNER_SPRINGEN && Handlung != GEGNER_AUSFAHREN) {
        Wegschieben(GegnerRect[GegnerArt], 0);
        PlattformTest(GegnerRect[GegnerArt]);
    }

    // Checken, ob der Schneekönig von einer Schneekoppe getroffen wurde =)
    GegnerClass *pTemp = Gegner.pStart;

    if (Handlung != GEGNER_AUSFAHREN)
        while (pTemp != nullptr) {
            if (pTemp->GegnerArt == SCHNEEKOPPE)
                if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], pTemp->xPos, pTemp->yPos,
                                    GegnerRect[pTemp->GegnerArt])) {
                    pTemp->Energy = 0.0f;
                    Energy -= 5000.0f;

                    if (Handlung != GEGNER_SPRINGEN)
                        WackelAnim = 8.0f;
                }

            pTemp = pTemp->pNext;
        }

    // kaputt? Dann rausfliegen
    if (Energy < 100.0f && Handlung != GEGNER_AUSFAHREN) {
        // Spieler fliegt runter
        for (int p = 0; p < NUMPLAYERS; p++)
            if (Player[p].AufPlattform == this)
                Player[p].AufPlattform = nullptr;

        Energy = 100;
        Destroyable = false;
        Handlung = GEGNER_AUSFAHREN;
        AnimCount = 10.0f;

        xSpeed = 0.0f;
        ySpeed = 0.0;
        xAcc = 0.0f;
        yAcc = 0.0f;

        SoundManager.PlayWave(100, 128, 11025, SOUND_TAKEOFF);

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC_BOSS, -2.0f, 0, false);
    }
}

// --------------------------------------------------------------------------------------
// SchneeKoenig explodiert nicht, sondern bleibt kaputt stehen
// --------------------------------------------------------------------------------------

void GegnerSchneeKoenig::GegnerExplode() {
    // Endboss-Musik ausfaden und abschalten
    SoundManager.FadeSong(MUSIC_BOSS, -2.0f, 0, false);

    for (int p = 0; p < NUMPLAYERS; p++)
        if (Player[p].AufPlattform == this)
            Player[p].AufPlattform = nullptr;

    Player[0].Score += 2000 * Value1;
    HUD.BossHUDActive = false;

    ScrolltoPlayeAfterBoss();
}
