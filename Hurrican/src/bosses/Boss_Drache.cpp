// --------------------------------------------------------------------------------------
// Die riesige Stahldrache
//
// fliegt Links und rechts vom Screen über den Hurri und schmeisst dabei Sachen ab
// --------------------------------------------------------------------------------------

#include "Boss_Drache.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerDrache::GegnerDrache(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_NOTVISIBLE;
    BlickRichtung = LINKS;
    Energy = 7000;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimPhase = 0;
    ChangeLight = Light;
    Destroyable = true;
    TestBlock = false;
    TailSinus = 0.0f;
    OwnDraw = true;
    HeadWinkel = 0.0f;
    SmokeCount = 0.0f;
    AnimWinkel = 0.0f;
    KieferWinkel = 0.0f;
    Growl = true;
    AlreadyDrawn = false;
    Attack = 0;
    Position = LINKS;
    HeadLocked = false;
    HeadX = 0.0f;
    HeadY = 0.0f;
    HeadXSpeed = 0.0f;
    HeadYSpeed = 0.0f;
    DrawYOffset = 0.0f;
    FlareRot = 0.0f;

    // Value2 != 0? Dann Starten die Spieler auf dem Rücken des Drachen!
    if (Value2 != 0) {
        Handlung = GEGNER_EINFLIEGEN;
        Attack = GEGNER_LAUFEN;
        xSpeed = -40.0f;

        for (int p = 0; p < NUMPLAYERS; p++) {
            Player[p].xpos = xPos - p * 30;
            Player[p].ypos = yPos;
            Player[p].AufPlattform = this;
            Player[p].DoFesteAktion = true;
            Player[p].FesteAktion = -1;
        }

        Active = true;
        Destroyable = false;
        StopStageMusicAtStart = true;
        HeadLocked = true;
    }

    Arm.LoadImage("drache_arm.png", 126, 107, 126, 107, 1, 1);
    Leg.LoadImage("drache_bein.png", 212, 85, 212, 85, 1, 1);
    Head[0].LoadImage("drache_head.png", 105, 64, 105, 64, 1, 1);
    Head[1].LoadImage("drache_head2.png", 93, 55, 93, 55, 1, 1);
    Tail.LoadImage("drache_tail.png", 12, 29, 12, 29, 1, 1);
    Spitze.LoadImage("drache_spitze.png", 46, 34, 46, 34, 1, 1);
}

    // --------------------------------------------------------------------------------------
    // Eigene Draw Funktion
    // --------------------------------------------------------------------------------------

constexpr int NUM_TAILS = 20;

void GegnerDrache::DoDraw() {
    if (PlayerAbstand() > 1000)
        return;

    int mirrored = 0;

    if (xSpeed < 0.0f) {
        mirrored = -1;
        mirrorOffset = -1;
        GegnerRect[DRACHE].left = -50;
        GegnerRect[DRACHE].right = 161;
    } else if (xSpeed > 0.0f) {
        mirrored = 1;
        mirrorOffset = 0;
        GegnerRect[DRACHE].left = 40;
        GegnerRect[DRACHE].right = 250;
    } else if (Position == LINKS) {
        mirrored = -1;
        mirrorOffset = -1;
        GegnerRect[DRACHE].left = -50;
        GegnerRect[DRACHE].right = 161;
    } else {
        mirrored = 1;
        mirrorOffset = 0;
        GegnerRect[DRACHE].left = 40;
        GegnerRect[DRACHE].right = 250;
    }

    if (Destroyable == false) {
        Position = LINKS;
        BlickRichtung = LINKS;
        mirrored = -1;
        mirrorOffset = -1;
    }

    // Kopf kann man treffen?
    if (Handlung == GEGNER_LAUFEN3) {
        GegnerRect[DRACHE].top = 20 + static_cast<int>(HeadY);
        GegnerRect[DRACHE].bottom = 130 + static_cast<int>(HeadY);

        if (Position == LINKS) {
            GegnerRect[DRACHE].left = static_cast<int>(HeadX) - 60;
            GegnerRect[DRACHE].right = static_cast<int>(HeadX) + 20;
        } else {
            GegnerRect[DRACHE].left = 180 + static_cast<int>(HeadX);
            GegnerRect[DRACHE].right = 270 + static_cast<int>(HeadX);
        }
    } else
        // Spieler kann draufhopsen?
        if (Handlung == GEGNER_WARTEN || Handlung == GEGNER_AUSFAHREN || Handlung == GEGNER_EINFLIEGEN) {
        GegnerRect[DRACHE].left = 50;
        GegnerRect[DRACHE].right = 150;
        GegnerRect[DRACHE].top = 15;
        GegnerRect[DRACHE].bottom = 135;
    } else {
        GegnerRect[DRACHE].top = 0;
        GegnerRect[DRACHE].bottom = 135;
    }

    BlickRichtung = mirrored;

    // Hinteres Bein rendern
    //
    Leg.RenderSpriteRotatedOffset(xPos - TileEngine.XOffset - 140.0f - static_cast<float>(mirrorOffset * 270),
                                  yPos - TileEngine.YOffset + 95.0f + DrawYOffset,
                                  static_cast<float>(cos(AnimWinkel)) * 10.0f, 87, -25, 0xFF888888, (mirrored < 0));

    // Hinterer Arm rendern
    //
    Arm.RenderSpriteRotatedOffset(xPos - TileEngine.XOffset + 140.0f + static_cast<float>(mirrorOffset * 210),
                                  yPos - TileEngine.YOffset + 100.0f + DrawYOffset,
                                  static_cast<float>(sin(AnimWinkel)) * 10.0f, -42, -35, 0xFF888888, (mirrored < 0));

    // Halswirbel?
    if ((Handlung == GEGNER_LAUFEN3 || Handlung == GEGNER_EXPLODIEREN) && Attack != GEGNER_EINFLIEGEN &&
        Attack != GEGNER_SCHIESSEN) {
        float xoff = 0.0f;
        float yoff = 0.0f;

        float winkel;

        if (Position == LINKS)
            winkel = HeadWinkel - 180.0f;
        else
            winkel = 180.0f - HeadWinkel;

        // DKS - converted to float:
        // render   = (int) sqrt(HeadX * HeadX + HeadY * HeadY) / 10;
        // float dist = (float)sqrt(HeadX * HeadX + HeadY * HeadY);
        int render = sqrtf(HeadX * HeadX + HeadY * HeadY) / 10;
        float dist = sqrtf(HeadX * HeadX + HeadY * HeadY);

        float xadd = HeadX / dist * 10.0f;
        float yadd = HeadY / dist * 10.0f;
        while (render-- > 0) {
            Tail.RenderSpriteRotated(
                xPos - TileEngine.XOffset + 200.0f + static_cast<float>(mirrorOffset * 220) + xoff * static_cast<float>(Position),
                yPos - TileEngine.YOffset + 60.0f + yoff + DrawYOffset, winkel, 0xFFFFFFFF);
            // ein wirbel weiter
            if (Position == LINKS) {
                xoff -= xadd;
                yoff += yadd;
            } else {
                xoff += xadd;
                yoff += yadd;
            }

            if (xoff > HeadX && yoff > HeadY)
                render = 0;
        }
    }

    // Körper rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSpriteRotated(xPos - TileEngine.XOffset,
                                                  yPos - TileEngine.YOffset + DrawYOffset, 0, 0,
                                                  0xFFFFFFFF, (mirrored < 0));

    // Knubbel am Hals rendern
    pGegnerGrafix[MINIDRAGON]->RenderSprite(xPos - TileEngine.XOffset + 170 + mirrorOffset * 210,
                                            yPos - TileEngine.YOffset + 55 + DrawYOffset, 4,
                                            0xFFFFFFFF);

    // Schwanz rendern
    //
    int i = 0;
    float tempx = xPos - TileEngine.XOffset + 10.0f - static_cast<float>(mirrorOffset * 165);

    for (i = 0; i < NUM_TAILS; i++) {
        Tail.RenderSpriteScaled(tempx,
                                yPos - TileEngine.YOffset + 20.0f +
                                    static_cast<float>(sin(TailSinus + i / 7.0f) * i) + 42.0f + DrawYOffset,
                                12 - i / 4, 29 - i / 2, 0, 0xFFFFFFFF);
        tempx -= (12 - i / 4) * mirrored;
    }

    // Schwanz Spitze
    //
    bool isMirrored = (mirrored < 0);
    Spitze.RenderSprite(tempx - 37 - mirrorOffset * 37.0f,
                        yPos - TileEngine.YOffset + 17.0f +
                            static_cast<float>(sin(TailSinus + static_cast<float>(i) / 7.0f) * i) + 36.0f + DrawYOffset,
                        0, 0xFFFFFFFF, isMirrored);

    // Unterkiefer
    //
    Head[1].RenderSpriteRotatedOffset(xPos - TileEngine.XOffset + 190.0f + static_cast<float>(mirrorOffset * 276) + HeadX,
                                      yPos - TileEngine.YOffset + 50.0f + HeadY + DrawYOffset,
                                      static_cast<float>(HeadWinkel + sin(KieferWinkel) * 10.0f) - 10.0f, -40, -10,
                                      0xFFFFFFFF, (mirrored < 0));
    // Kopf
    //
    Head[0].RenderSpriteRotatedOffset(xPos - TileEngine.XOffset + 180.0f + static_cast<float>(mirrorOffset * 276) + HeadX,
                                      yPos - TileEngine.YOffset + 15.0f + HeadY + DrawYOffset,
                                      static_cast<float>(HeadWinkel - sin(KieferWinkel) * 10.0f) + 10.0f, -35, 15,
                                      0xFFFFFFFF, (mirrored < 0));

    // Vorderes Bein rendern
    //
    Leg.RenderSpriteRotatedOffset(xPos - TileEngine.XOffset - 140.0f - static_cast<float>(mirrorOffset * 270),
                                  yPos - TileEngine.YOffset + 85 + DrawYOffset,
                                  static_cast<float>(sin(AnimWinkel)) * 12.0f, 87, -25, 0xFFFFFFFF, (mirrored < 0));

    // Vorderer Arm rendern
    //
    Arm.RenderSpriteRotatedOffset(xPos - TileEngine.XOffset + 140.0f + static_cast<float>(mirrorOffset * 210),
                                  yPos - TileEngine.YOffset + 90.0f + DrawYOffset,
                                  static_cast<float>(cos(AnimWinkel)) * 12.0f, -42, -35, 0xFFFFFFFF, (mirrored < 0));

    // Flare
    if (AlreadyDrawn == false)
        if (Handlung != GEGNER_EINFLIEGEN || (Handlung == GEGNER_EINFLIEGEN && Attack == GEGNER_LAUFEN))
            if ((Handlung != GEGNER_WARTEN && Handlung != GEGNER_EXPLODIEREN) || Attack == GEGNER_LAUFEN) {
                DirectGraphics.SetAdditiveMode();
                Projectiles.LavaFlare.RenderSpriteScaledRotated(
                    xPos - 100.0f - TileEngine.XOffset - mirrorOffset * 230,
                    yPos - 55.0f - TileEngine.YOffset + DrawYOffset, 180, 180, FlareRot, 0xFF66FF66);

                Projectiles.LavaFlare.RenderSpriteScaledRotated(
                    xPos - 40.0f - TileEngine.XOffset - mirrorOffset * 230,
                    yPos - 10.0f - TileEngine.YOffset + DrawYOffset, 90, 90, FlareRot * 2, 0xFF88AAFF);
                DirectGraphics.SetColorKeyMode();
            }

    // Pfeile anzeigen
    if (Handlung == GEGNER_WARTEN) {
        ArrowCount -= 0.2f SYNC;
        if (ArrowCount < 0.0f)
            ArrowCount = 2.0f;

        if (static_cast<int>(ArrowCount) % 2 == 0)
            HUD.Arrow.RenderMirroredSprite(xPos - TileEngine.XOffset + 50.0f,
                                           yPos - TileEngine.YOffset - 30.0f, 0xFF00FF00, false, true);
    }

    AlreadyDrawn = true;
}

// --------------------------------------------------------------------------------------
// Winkel des Kopfes zum Spieler hin ausrechnen
// --------------------------------------------------------------------------------------

void GegnerDrache::ComputeHeadWinkel() {
    if (HeadLocked == true)
        return;

    float ydiv = (pAim->ypos + 40) - yPos;
    if (ydiv == 0.0f)
        ydiv = 0.00001f;

    if (BlickRichtung == 1) {
        float xdiv = (pAim->xpos + 35) - (xPos + 210);
        // DKS - converted to float:
        // HeadWinkel = (float)atan(xdiv / ydiv) * 180.0f / PI - 90.0f;
        HeadWinkel = RadToDeg(atanf(xdiv / ydiv)) - 90.0f;
    } else {
        float xdiv = (pAim->xpos + 35) - (xPos - 80);
        // DKS - converted to float:
        // HeadWinkel = -(float)atan(xdiv / ydiv) * 180.0f / PI - 82.0f;
        HeadWinkel = -RadToDeg(atanf(xdiv / ydiv)) - 82.0f;
    }

    HeadWinkel = std::clamp(HeadWinkel, -90.0f, 90.0f);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerDrache::DoKI() {
    // Energie anzeigen
    if (Value2 == 0 && Handlung != GEGNER_NOTVISIBLE && Handlung != GEGNER_EXPLODIEREN)
        HUD.ShowBossHUD(7000, Energy);

    // Levelausschnitt auf den Drache zentrieren, sobald dieser sichtbar wird
    if (Value2 == 0 && Active == true && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        TileEngine.ScrollLevel(static_cast<float>(Value1), yPos,
                               TileStateEnum::SCROLLTOLOCK);  // Level auf den Drache zentrieren

        // Drache aus Screen bringen
        xPos = Value1 - 500.0f;
        SoundManager.FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
    }

    // Boss blinkt nicht so lange wie die restlichen Gegner
    if (DamageTaken > 0.0f)
        DamageTaken -= 100 SYNC;  // Rotwerden langsam ausfaden lassen
    else
        DamageTaken = 0.0f;  // oder ganz anhalten

    // Hat der Drache keine Energie mehr ? Dann explodiert er
    if (Energy <= 100.0f && Handlung != GEGNER_EXPLODIEREN && Destroyable == true) {
        Player[0].Score += 10000;
        Handlung = GEGNER_EXPLODIEREN;
        Attack = GEGNER_LAUFEN;
        yAcc = -1.0f;
        AnimCount = 50.0f;
        ShotDelay = 1.0f;
        Energy = 100.0f;
        HeadLocked = true;

        // Alle übrigen Mini-Drachen zerstören
        GegnerClass *pTemp = Gegner.pStart;
        while (pTemp != nullptr) {
            if (pTemp->GegnerArt == MINIDRAGON)
                pTemp->Energy = 0.0f;

            pTemp = pTemp->pNext;
        }

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC_BOSS, -2.0f, 0, false);
    }

    if (!(Handlung == GEGNER_EINFLIEGEN && Attack == GEGNER_STEHEN))
        AnimWinkel += 0.2f SYNC;

    constexpr float TWO_PI = 2 * PI;
    while (AnimWinkel > TWO_PI)
        AnimWinkel -= TWO_PI;

    // Headwinkel ausrechnen
    ComputeHeadWinkel();

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        // warten, bis der Hurri aufspringt
        case GEGNER_WARTEN: {
            if (KieferWinkel > PI / 2.0f)
                KieferWinkel -= 5.0f SYNC;
            else
                KieferWinkel = PI / 2.0f;

            if (HeadWinkel > -50)
                HeadWinkel -= 25.0f SYNC;
            else
                HeadWinkel = -50;

            // Rauchen
            AnimCount -= 1.0f;
            if (AnimCount < 0.0f) {
                AnimCount = 5.0f;
                PartikelSystem.PushPartikel(xPos - 100.0f + static_cast<float>(random(300)),
                                            yPos + DrawYOffset + static_cast<float>(random(100)), SMOKEBIG);
            }

            AnimWinkel = 0;
            BlickRichtung = LINKS;

            // testen ob spieler draufsteht
            PlattformTest(GegnerRect[GegnerArt]);

            // warten, bis alle Spieler draufstehen
            bool alledrauf = true;

            for (int p = 0; p < NUMPLAYERS; p++)
                if (Player[p].Handlung != PlayerActionEnum::TOT &&
                        (Player[p].AufPlattform != this ||
                        Player[p].Handlung == PlayerActionEnum::RADELN ||
                        Player[p].Handlung == PlayerActionEnum::RADELN_FALL))
                    alledrauf = false;

            if (alledrauf == true) {
                SoundManager.PlayWave(100, 128, 6000, SOUND_EXPLOSION1);
                SoundManager.PlayWave(100, 128, 8000, SOUND_ROCKET);
                SoundManager.PlayWave(100, 128, 11025, SOUND_ROCKET);
                Handlung = GEGNER_AUSFAHREN;
                xAcc = -2.0f;
                yAcc = -0.2f;

                for (int p = 0; p < NUMPLAYERS; p++) {
                    Player[p].DoFesteAktion = true;
                    Player[p].FesteAktion = -1;
                    Player[p].CanBeDamaged = false;
                }
            }
        } break;

        case GEGNER_AUSFAHREN: {
            PlattformTest(GegnerRect[GegnerArt]);

            if (HeadWinkel < 30.0f)
                HeadWinkel += 3.0f SYNC;

            if (KieferWinkel < PI)
                KieferWinkel += 0.5f SYNC;

            if (xPos < Value1 - 500.0f) {
                xPos = Value1 - 500.0f;
                xSpeed = 0.0f;
                ySpeed = 0.0f;
                xAcc = 0.0f;
                yAcc = 0.0f;
                Handlung = -1;

                NewStage = Stage + 1;

                for (int p = 0; p < NUMPLAYERS; p++) {
                    Player[p].FesteAktion = -1;
                    Player[p].DoFesteAktion = true;
                }

                StageClear();
            }
        } break;

        // Spieler stehen zu Beginn auf Drache drauf
        case GEGNER_EINFLIEGEN: {
            if (Attack != GEGNER_STEHEN) {
                for (int p = 0; p < NUMPLAYERS; p++) {
                    Player[p].xpos = xPos + 40.0f + p * 30.0f;
                    Player[p].ypos = yPos - 65.0f + DrawYOffset;
                    Player[p].Blickrichtung = LINKS;
                    Player[p].AufPlattform = this;
                }
            }

            // Rauchen
            if (PlayerAbstand() < 1000) {
                AnimCount -= 1.0f;

                // testen, ob Spieler noch draufsteht
                if (Attack == GEGNER_STEHEN)
                    PlattformTest(GegnerRect[GegnerArt]);
            }

            if (AnimCount < 0.0f) {
                AnimCount = 15.0f;
                PartikelSystem.PushPartikel(xPos - 120.0f + static_cast<float>(random(300)),
                                            yPos + static_cast<float>(random(100)) + DrawYOffset, SMOKEBIG);

                if (Attack != GEGNER_STEHEN && random(4) == 0) {
                    PartikelSystem.PushPartikel(xPos - 120.0f + static_cast<float>(random(300)),
                                                yPos + static_cast<float>(random(100)) + DrawYOffset,
                                                EXPLOSION_MEDIUM2);
                    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND_EXPLOSION1);
                }
            }

            switch (Attack) {
                // normal fliegen
                case GEGNER_LAUFEN: {
                    static float off = 0.0f;

                    off += 0.5f SYNC;

                    if (off > 2 * PI)
                        off -= 2 * PI;

                    DrawYOffset = static_cast<float>(sin(off)) * 8.0f;

                    if (Energy > 100.0f)
                        Energy -= 85 SYNC;
                    else {
                        yAcc = 2.0f;
                        Attack = GEGNER_LAUFEN2;
                    }
                } break;

                // abstürzen
                case GEGNER_LAUFEN2: {
                    if (DrawYOffset > 0.0f)
                        DrawYOffset -= 1.0f SYNC;
                    else
                        DrawYOffset = 0.0f;

                    if (ySpeed > 0.0f &&
                        TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]) & BLOCKWERT_WAND) {
                        ySpeed *= -0.3f;

                        for (int i = 0; i < 30; i++)
                            PartikelSystem.PushPartikel(xPos - 100.0f + static_cast<float>(random(260)),
                                                        yPos + 120.0f - static_cast<float>(random(10)),
                                                        SNOWFLUSH);

                        // nicht weiter abspringen?
                        if (ySpeed > -0.5f) {
                            ySpeed = 0.0f;
                            yAcc = 0.0f;
                            Attack = GEGNER_LAUFEN3;
                            xAcc = 2.0f;
                            SmokeCount = 0.0f;
                            HeadLocked = true;
                        }
                    }
                } break;

                // rutschen
                case GEGNER_LAUFEN3: {
                    if (KieferWinkel < PI / 2.0f)
                        KieferWinkel += 0.1f SYNC;
                    else
                        KieferWinkel = PI / 2.0f;

                    if (HeadWinkel > -70.0f)
                        HeadWinkel -= 2.0f SYNC;
                    else
                        HeadWinkel = -70.0f;

                    SmokeCount -= 1.0f SYNC;

                    if (SmokeCount < 0.0f) {
                        SmokeCount = 0.2f;
                        PartikelSystem.PushPartikel(xPos - 100.0f + static_cast<float>(random(260)),
                                                    yPos + 120.0f, SNOWFLUSH);
                    }

                    // liegenbleiben?
                    if (xSpeed > 0.0f) {
                        xSpeed = 0.0f;
                        xAcc = 0.0f;
                        Attack = GEGNER_STEHEN;

                        for (int p = 0; p < NUMPLAYERS; p++)
                            Player[p].DoFesteAktion = false;

                        SoundManager.PlaySong(MUSIC_STAGEMUSIC, false);
                    }
                } break;

                // liegen bleiben und rauchen
                case GEGNER_STEHEN: {
                    if (KieferWinkel > PI / 2.0f)
                        KieferWinkel -= 0.1f SYNC;
                    else
                        KieferWinkel = PI / 2.0f;

                    if (HeadWinkel > -70.0f)
                        HeadWinkel -= 2.0f SYNC;
                    else
                        HeadWinkel = -70.0f;
                } break;
            }

        } break;

        // explodieren lassen
        case GEGNER_EXPLODIEREN: {
            AnimWinkel = 0.0f;
            Energy = 100.0f;

            // Explosionen
            ShotDelay -= 1.0f SYNC;
            if (ShotDelay < 0.0f) {
                ShotDelay = 0.5f;

                SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND_EXPLOSION1);
                PartikelSystem.PushPartikel(xPos - 100.0f + static_cast<float>(random(500)),
                                            yPos + 20.0f + static_cast<float>(random(80)), EXPLOSION_MEDIUM2);

                if (random(3) == 0)
                    PartikelSystem.PushPartikel(xPos - 100.0f + static_cast<float>(random(500)),
                                                yPos + 20.0f + static_cast<float>(random(80)), EXPLOSION_BIG);
            }

            switch (Attack) {
                case GEGNER_LAUFEN: {
                    // Kopf nach unten und Maul öffnen
                    if (HeadWinkel > -70.0f)
                        HeadWinkel -= 5.0f SYNC;

                    if (KieferWinkel < 30.0f)
                        HeadWinkel += 1.0f SYNC;

                    // xSpeed verringern
                    if (xSpeed < 0.0f)
                        xAcc = 1.0f;
                    if (xSpeed > 0.0f)
                        xAcc = -1.0f;

                    // weit genug oben? Dann runterfallen lassen
                    if (yPos < StartPosY - 400.0f) {
                        xPos = Value1 + 450.0f;
                        xAcc = 0.0f;
                        yAcc = 0.0f;
                        xSpeed = 0.0f;
                        ySpeed = 40.0f;
                        HeadX = 0.0f;
                        HeadY = 0.0f;
                        HeadLocked = true;
                        HeadWinkel = 75.0f;
                        KieferWinkel = 0.0f;
                        Destroyable = false;
                        Attack = GEGNER_LAUFEN2;
                    }
                } break;

                case GEGNER_LAUFEN2: {
                    BlickRichtung = LINKS;

                    // Runtergefallen? Dann warten, bis der Hurri aufsteigt
                    if (yPos > StartPosY + 295.0f && ySpeed > 0.0f) {
                        for (int i = 0; i < 30; i++)
                            PartikelSystem.PushPartikel(xPos - 120.0f + static_cast<float>(random(300)),
                                                        yPos + 80.0f, SMOKEBIG);

                        ShakeScreen(5.0f);
                        SoundManager.PlayWave(100, 128, 10000, SOUND_DOORSTOP);

                        xSpeed = 0.0f;
                        ySpeed = 0.0f;
                        xAcc = 0.0f;
                        yAcc = 0.0f;
                        Handlung = GEGNER_WARTEN;
                        ArrowCount = 2.0f;
                        yPos = StartPosY + 295.0f;
                    }
                } break;
            }  // switch (Attacke)
        } break;

        case GEGNER_NOTVISIBLE:  // Warten bis der Screen zentriert wurde
        {
            StartPosY = yPos - 25.0f;
            if (TileEngine.Zustand == TileStateEnum::LOCKED) {
                // Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
                // DKS - Added function SongIsPlaying() to SoundManagerClass:
                if (!SoundManager.SongIsPlaying(MUSIC_BOSS))
                    SoundManager.PlaySong(MUSIC_BOSS, false);

                // Und Boss erscheinen lassen
                Handlung = GEGNER_EINFLIEGEN2;
            }
        } break;

        case GEGNER_EINFLIEGEN2:  // Gegner kommt DAS ERSTE MAL in den Screen geflogen
        {
            static bool drachevorbei = false;

            Energy = 7000;
            DamageTaken = 0.0f;

            // mindestens einmal vorbeigeflogen?
            if (TileEngine.pDragonHack->m_State == DragonState::FLY)
                drachevorbei = true;

            // Hinten vorbeigeflogen?
            if (TileEngine.pDragonHack->m_State == DragonState::WAIT && drachevorbei == true) {
                delete TileEngine.pDragonHack;
                TileEngine.pDragonHack = nullptr;

                Handlung = GEGNER_AUSWAHL;
            }
        } break;

        // Auswählen, welche Aktion gemacht wird
        case GEGNER_AUSWAHL: {
            HeadLocked = false;

            // Was macht unser kleiner Drache denn als Nächstes?
            int newmove = random(3);

            switch (newmove) {
                // Spieler überfliegen und dabei eine Aktion ausfühern?
                case 0: {
                    Handlung = GEGNER_LAUFEN;

                    if (xPos < Value1)
                        BlickRichtung = -1;
                    else
                        BlickRichtung = 1;

                    xPos = Value1 + 320.0f + 100 + 800.0f * BlickRichtung;
                    yPos = StartPosY - 80.0f;
                    xSpeed = -15.0f * BlickRichtung;
                    ySpeed = 5.0f;
                    yAcc = -0.1f;

                    // Handlung festlegen, was er während dem Überfliegen macht
                    int j = random(2);

                    switch (j) {
                        // Feuer spucken
                        case 0: {
                            Attack = GEGNER_SCHIESSEN;
                            KieferWinkel = 0.0;
                            AnimCount = PI - 1.0f;
                        } break;

                        // Eier legen =)
                        case 1: {
                            Attack = GEGNER_BOMBARDIEREN;
                            KieferWinkel = 0.0;
                            AnimCount = 10.0f;
                        } break;
                    }
                } break;

                // Vorbeizischen und Dinge abwerfen =)
                case 1: {
                    Handlung = GEGNER_LAUFEN2;

                    if (xPos < Value1)
                        BlickRichtung = -1;
                    else
                        BlickRichtung = 1;

                    xPos = Value1 + 320.0f + 100 + 800.0f * BlickRichtung;
                    yPos = StartPosY + 50.0f;
                    xSpeed = -50.0f * BlickRichtung;
                    ySpeed = 0.0f;
                    yAcc = 0.0f;

                    // Handlung festlegen, was er während dem Überfliegen macht
                    int j = random(2);

                    // REMOVE ???
                    j = 0;

                    switch (j) {
                        // Suchgegner werden
                        case 0: {
                            Attack = GEGNER_BOMBARDIEREN;
                            KieferWinkel = 0.0;
                            AnimCount = PI - 1.0f;
                            ShotDelay = 0.0f;
                        } break;

                        case 1: {
                        } break;
                    }
                } break;

                // An der Seite reinkommen
                case 2: {
                    Handlung = GEGNER_LAUFEN3;
                    Attack = GEGNER_EINFLIEGEN;
                    ShotCount = 2 + random(2);

                    if (xPos < Value1)
                        Position = RECHTS;
                    else
                        Position = LINKS;

                    yPos = StartPosY + 50.0f + static_cast<float>(random(50));

                    KieferWinkel = PI / 2.0f;
                    xSpeed = 0.0f;
                    ySpeed = 0.0f;
                    xAcc = 0.0f;
                    yAcc = 0.0f;
                    AnimCount = 0.0f;
                }
            }
        } break;

        case GEGNER_LAUFEN: {
            // Je nach "Unterhandlung" anders verhalten
            switch (Attack) {
                // Feuerbälle schiessen
                case GEGNER_SCHIESSEN: {
                    KieferWinkel += 0.75f SYNC;
                    while (KieferWinkel > 2 * PI)
                        KieferWinkel -= 2 * PI;

                    AnimCount += 0.75f SYNC;

                    while (AnimCount > 2 * PI) {
                        AnimCount -= 2 * PI;

                        if (BlickRichtung == 1) {
                            WinkelUebergabe = 90 - HeadWinkel;
                            Projectiles.PushProjectile(xPos + 240.0f + HeadWinkel / 2.0f,
                                                       yPos + 50.0f - HeadWinkel / 2.0f, FIREBALL_BIG);
                        } else {
                            WinkelUebergabe = 270.0f + HeadWinkel;
                            Projectiles.PushProjectile(xPos - 110.0f - HeadWinkel / 2.0f,
                                                       yPos + 50.0f - HeadWinkel / 2.0f, FIREBALL_BIG);
                        }

                        SoundManager.PlayWave(100, 128, 8000 + random(2000), SOUND_FIREBALL);
                    }

                } break;

                // Feuerbälle schiessen
                case GEGNER_BOMBARDIEREN: {
                    if (xPos + 100.0f - mirrorOffset > TileEngine.XOffset + 0.0f &&
                        xPos + 100.0f - mirrorOffset < TileEngine.XOffset + 640.0f)
                        AnimCount -= 1.0f SYNC;

                    if (AnimCount < 0.0f) {
                        AnimCount = 25.0f;
                        Gegner.PushGegner(xPos + 100.0f - mirrorOffset, yPos + 80.0f, MINIDRAGON, 0, 99, false, false);
                    }
                } break;
            }

            // Am Rand angekommen? Dann wieder neue Aktion wählen
            if ((xSpeed > 0.0f && xPos > Value1 + 1000.0f) || (xSpeed < 0.0f && xPos < Value1 - 500))
                Handlung = GEGNER_AUSWAHL;

        } break;

        // Vorbeiflitzen?
        case GEGNER_LAUFEN2: {
            // Je nach "Unterhandlung" anders verhalten
            switch (Attack) {
                // Suchgegner werden
                case GEGNER_BOMBARDIEREN: {
                    // Raketen gegner abwerfen
                    if (xPos + 100.0f - mirrorOffset > TileEngine.XOffset + 50.0f &&
                        xPos + 100.0f - mirrorOffset < TileEngine.XOffset + 600.0f)
                        ShotDelay -= 1.0f SYNC;

                    if (ShotDelay < 0.0f) {
                        ShotDelay = 3.0f;
                        Gegner.PushGegner(xPos + 100.0f - mirrorOffset, yPos + 80.0f, FETTERAKETE, 0, 99, false, false);
                    }

                    // Blätter spawnen
                    AnimCount -= 1.0f SYNC;
                    if (AnimCount < 0.0f) {
                        AnimCount = 0.3f;
                        if (BlickRichtung == RECHTS) {
                            WinkelUebergabe = 180.0f;

                            if (xPos > TileEngine.XOffset)
                                PartikelSystem.PushPartikel(TileEngine.XOffset + 10.0f,
                                                            TileEngine.YOffset + static_cast<float>(random(480)),
                                                            BLATT2);

                            for (int p = 0; p < NUMPLAYERS; p++)
                                Player[p].xpos += 8.0f SYNC;
                        } else {
                            WinkelUebergabe = 0.0f;

                            if (xPos < TileEngine.XOffset + 550.0f)
                                PartikelSystem.PushPartikel(TileEngine.XOffset + 640.0f,
                                                            TileEngine.YOffset + static_cast<float>(random(480)),
                                                            BLATT2);

                            for (int p = 0; p < NUMPLAYERS; p++)
                                Player[p].xpos -= 8.0f SYNC;
                        }
                    }
                } break;
            }

            // Am Rand angekommen? Dann wieder neue Aktion wählen
            if ((xSpeed > 0.0f && xPos > Value1 + 1000.0f) || (xSpeed < 0.0f && xPos < Value1 - 500))
                Handlung = GEGNER_AUSWAHL;

        } break;

        // An der Seite reinkommen
        case GEGNER_LAUFEN3: {
            // Welchen Angriff genau macht der Drache?
            switch (Attack) {
                // Reinkommen?
                case GEGNER_EINFLIEGEN: {
                    if (Position == RECHTS) {
                        xPos = Value1 - 300.0f;
                        xPos += static_cast<float>(sin(AnimCount)) * 150.0f;
                    } else {
                        xPos = Value1 + 740.0f;
                        xPos -= static_cast<float>(sin(AnimCount)) * 150.0f;
                    }

                    // Angekommen?
                    if (AnimCount >= PI / 2.0f) {
                        // schon genau am richtigen Punkt? Dann auf Angriff umschalten
                        if (AnimCount == PI / 2.0f) {
                            Attack = GEGNER_SCHIESSEN;
                        } else
                            AnimCount = PI / 2.0f;
                    } else
                        AnimCount += 0.075f SYNC;
                } break;

                // Kiefer öffnen
                case GEGNER_SCHIESSEN: {
                    KieferWinkel -= 0.5f SYNC;

                    // Kiefer offen? Dann Kopf losschiessen
                    if (KieferWinkel < -PI / 2.0f) {
                        KieferWinkel = -PI / 2.0f;
                        Attack = GEGNER_VERFOLGEN;

                        HeadLocked = true;

                        // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
                        // HeadXSpeed =  (float)cos(HeadWinkel / 180.0f * PI) * (40.0f + 10.0f * (Skill)) * Position;
                        // HeadYSpeed = -(float)sin(HeadWinkel / 180.0f * PI) * (30.0f +  8.0f * (Skill));
                        HeadXSpeed = cos_deg(HeadWinkel) * (40.0f + 10.0f * (Skill)) * Position;
                        HeadYSpeed = -sin_deg(HeadWinkel) * (30.0f + 8.0f * (Skill));

                        if (Position == LINKS && HeadXSpeed > -0.1f)
                            HeadXSpeed = -0.01f;

                        if (Position == RECHTS && HeadXSpeed < 0.1f)
                            HeadXSpeed = 0.01f;
                    }
                } break;

                // Kopf schiesst auf Spieler zu
                case GEGNER_VERFOLGEN: {
                    RECT_struct colliderect;

                    colliderect.top = 30;
                    colliderect.bottom = 50;
                    colliderect.left = 40;
                    colliderect.right = 50;

                    bool PlayerHit = false;

                    HeadX += HeadXSpeed SYNC;
                    HeadY += HeadYSpeed SYNC;

                    for (int p = 0; p < NUMPLAYERS; p++)
                        if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], Player[p].xpos, Player[p].ypos,
                                            colliderect) == true) {
                            ShotCount = 0;
                            PlayerHit = true;
                            Player[p].DamagePlayer(500.0f);
                        }

                    // Kopf am Boden? Dann sound, funken, speed umdrehen und kurz warten
                    if ((HeadYSpeed > 0.0f && yPos + 50 + HeadY > StartPosY + 380.0f) ||
                        (HeadXSpeed > 0.0f && xPos + HeadX > static_cast<float>(Value1) + 400.0f) ||
                        (HeadXSpeed < 0.0f && xPos + HeadX < static_cast<float>(Value1) + 100.0f) ||
                        PlayerHit == true) {
                        Attack = GEGNER_SCHLIESSEN;
                        AnimCount = 4.0f;
                        KieferWinkel = PI / 2.0f;
                        SoundManager.PlayWave(100, 128, 10000 + random(1000), SOUND_KLONG);
                        HeadYSpeed *= -1;
                        HeadXSpeed *= -1;

                        for (int i = 0; i < 30; i++)
                            PartikelSystem.PushPartikel(xPos + HeadX + 220.0f + static_cast<float>(random(20)) +
                                                            mirrorOffset * 275.0f,
                                                        yPos + HeadY + 70.0f + static_cast<float>(random(20)), FUNKE);
                    }

                } break;

                case GEGNER_SCHLIESSEN: {
                    if (AnimCount > 0.0f)
                        AnimCount -= 1.0f SYNC;

                    else {
                        AnimCount = 0.0f;
                        HeadX += HeadXSpeed SYNC;
                        HeadY += HeadYSpeed SYNC;

                        // Kopf wieder an Ausgansposition?
                        if (HeadY < 0.0f &&
                            ((HeadXSpeed < 0.0f && HeadX < 0.0f) || (HeadXSpeed > 0.0f && HeadX > 0.0f))) {
                            HeadLocked = false;

                            // noch einmal zuschnappen?
                            ShotCount--;

                            if (ShotCount > 0)
                                Attack = GEGNER_SCHIESSEN;
                            else
                            // oder wieder zurückziehen?
                            {
                                AnimCount = PI / 2.0f;
                                Attack = GEGNER_EINFLIEGEN2;
                            }
                        }
                    }
                } break;

                case GEGNER_EINFLIEGEN2: {
                    if (Position == RECHTS) {
                        xPos = Value1 - 300.0f;
                        xPos += static_cast<float>(sin(AnimCount)) * 150.0f;
                    } else {
                        xPos = Value1 + 740.0f;
                        xPos -= static_cast<float>(sin(AnimCount)) * 150.0f;
                    }

                    // Angekommen?
                    AnimCount -= 0.075f SYNC;
                    if (AnimCount <= -PI / 2.0f)
                        Handlung = GEGNER_AUSWAHL;
                } break;

            }  // switch(Attack)
        } break;

        default:
            break;
    }  // switch

    // Hat der Drache den Hurri getroffen ?
    // Testen, ob der Spieler den Boss berührt hat
    if (Destroyable == true)
        TestDamagePlayers(5.0f SYNC);

    // Partikel an der Düse erzeugen
    SmokeCount -= 1.0f SYNC;

    if (Handlung != GEGNER_EINFLIEGEN || (Handlung == GEGNER_EINFLIEGEN && Attack == GEGNER_LAUFEN))

        if ((Handlung != GEGNER_WARTEN && Handlung != GEGNER_EXPLODIEREN) || Attack == GEGNER_LAUFEN)
            if (SmokeCount < 0.0f) {
                SmokeCount = 0.2f;

                if (BlickRichtung == -1) {
                    WinkelUebergabe = 1.0f;
                    PartikelSystem.PushPartikel(xPos + 180.0f, yPos + 9.0f + DrawYOffset, DRACHE_SMOKE);
                } else {
                    WinkelUebergabe = 0.0f;
                    PartikelSystem.PushPartikel(xPos - 40.0f, yPos + 9.0f + DrawYOffset, DRACHE_SMOKE);
                }
            }

    // Flare drehen
    FlareRot += 1.0f SYNC;

    clampAngle(FlareRot);

    // Schwanz animieren
    if (!(Handlung == GEGNER_EINFLIEGEN && Attack == GEGNER_STEHEN))
        TailSinus += 1.0f SYNC;

    while (TailSinus > 2 * PI)
        TailSinus -= 2 * PI;
}

// --------------------------------------------------------------------------------------
// Drache explodiert
// --------------------------------------------------------------------------------------

void GegnerDrache::GegnerExplode() {}
