// --------------------------------------------------------------------------------------
// Der Robo Schädel
//
// liegt auf dem Schrotthaufen in dem Cave Level rum erhebt sich daraus.
// Schmeisst ab und zu die Turbine an, um Hurri einzusaugen
//
// Der Hals des Viehs wird wie in den Konsolenspielen durch mehrere einzelne Teile
// dargestellt. Hammer, was eine schweine Arbeit, echt :D
// --------------------------------------------------------------------------------------

#include "Boss_MetalHead.hpp"
#include "Tileengine.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerMetalHead::GegnerMetalHead(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::NOTVISIBLE;
    BlickRichtung = LINKS;
    Energy = 4000;
    ChangeLight = Light;
    Destroyable = false;
    Value1 = Wert1;
    Value2 = Wert2;
    TestBlock = false;
    OwnDraw = true;
    Akt = SK_AUSGANG;
    SmokeCount = 0.0f;
    Moving = false;
    TurbineOff = 0.0f;

    KieferSpeed = 10.0f;
    KieferPos = 0.0f;

    Eye_Alpha = 0.0f;
    Eye_Winkel = 0;
    Turbine_dran = true;
    GunWinkel = 0.0f;

    ShotDelay = 0.0f;
    ShotCount = 0;

    MoveSpeed = 5.0f;
    g_Fahrstuhl_Speed = 0.0f;

    // Zusätzliche Grafiken laden
    //
    Kiefer.LoadImage("metalhead_kiefer.png", 102, 153, 102, 153, 1, 1);
    Kiefer2.LoadImage("metalhead_kiefer2.png", 131, 88, 131, 88, 1, 1);
    Turbine.LoadImage("metalhead_turbine.png", 98, 49, 98, 49, 1, 1);
    Turbine2.LoadImage("metalhead_turbine2.png", 98, 49, 98, 49, 1, 1);
    HalsGfx.LoadImage("metalhead_halsteil.png", 56, 16, 56, 16, 1, 1);
    Flare.LoadImage("giantspiderflare.png", 128, 128, 128, 128, 1, 1);
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerMetalHead::DoDraw() {
    if (AlreadyDrawn == true)
        return;

    // am explodieren? Dann nicht rendern
    if (Handlung == GEGNER::EXPLODIEREN) {
        // Kopf wirbelt herum
        //
        if (AnimEnde > MAXWIRBEL - 5)
            pGegnerGrafix[GegnerArt]->RenderSpriteRotated(xPos - TileEngine.XOffset,
                                                          yPos - TileEngine.YOffset, Eye_Alpha / PI * 180.0f,
                                                          0xFFFFFFFF);
        else {
            xSpeed = 0.0f;
            ySpeed = 0.0f;
            xPos = TileEngine.XOffset;
            yPos = TileEngine.YOffset;
        }

        // Halsteile rendern
        for (int i = 0; i < AnimEnde; i++)
            HalsGfx.RenderSpriteRotated(Hals[i].x, Hals[i].y - TileEngine.YOffset, Hals[i].w,
                                        0xFFFFFFFF);

        return;
    }

    D3DCOLOR Color;

    int Wert = 255 - (static_cast<int>(DamageTaken));

    if (Turbine_dran == false) {
        Color = D3DCOLOR_RGBA(255, Wert, Wert, 255);
    } else
        Color = D3DCOLOR_RGBA(255, 255, 255, 255);

    Kiefer.RenderSprite(xPos - TileEngine.XOffset + 87.0f, yPos - TileEngine.YOffset + 107.0f + KieferPos,
                        Color);

    // Hals rendern ... uiuiu =)
    // Zuerst die Wirbel richtig positionieren
    // Dazu Strecke vom linken Screenrand bis zum linken Ende des Kopfes nehmen.
    // Diese dann durch Anzahl der Wirbel teilen

    // Strecke linker Rand bis zum Kopf
    float dx = xPos - static_cast<float>(Value1 + 40);

    // Strecke ypos linker Rand bis zum Kopf
    float dy = yPos - static_cast<float>(Value2 + 250);

    // Halsteile an richtige Position setzen
    for (int i = 0; i < MAXWIRBEL; i++) {
        Hals[i].x = static_cast<float>(Value1 - 40 + i * dx / (MAXWIRBEL - 1)) - TileEngine.XOffset;
        Hals[i].y = static_cast<float>(Value2 + 400);

        // Die Strecke von linkem Rand Punkt zum Kopf soll der Strecke 0 - PI entsprechen
        float dummy = (i * dy / MAXWIRBEL * PI) / dy;

        Hals[i].y += dy / 2.0f - cos(dummy) * dy / 2.0f;
        Hals[i].w = 0.0f;
    }

    // Halsteile richtig drehen
    for (int i = 0; i < MAXWIRBEL - 1; i++) {
        float ax = Hals[i + 1].x - Hals[i].x;
        float ay = Hals[i + 1].y - Hals[i].y;

        Hals[i].w = static_cast<float>(tanh(ay / ax));
        Hals[i].w = Hals[i].w / PI * 180.0f + 90.0f;

        clampAngle(Hals[i].w);
    }

    // Halsteile rendern
    for (int i = 0; i < MAXWIRBEL - 1; i++)
        HalsGfx.RenderSpriteRotated(Hals[i].x, Hals[i].y - TileEngine.YOffset, Hals[i].w,
                                    0xFFFFFFFF);

    // Turbine läuft? Dann Orange leuchten lassen
    //
    if (Handlung == GEGNER::SPECIAL2) {
        // Anglühen und abglühen lassen
        //
        int a = AnimPhase;

        if (a > 1500) {
            a = a - 1500;
            if (a > 255)
                a = 255;

            a = 255 - a;
        }

        else if (a > 255)
            a = 255;

        DirectGraphics.SetAdditiveMode();
        Turbine2.RenderSprite(xPos - TileEngine.XOffset + 20.0f, yPos - TileEngine.YOffset + 130.0f,
                              D3DCOLOR_RGBA(255, 144, 80, a));
        DirectGraphics.SetColorKeyMode();
    }

    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset, yPos - TileEngine.YOffset, Color);

    if (Turbine_dran == true) {
        Color = D3DCOLOR_RGBA(255, Wert, Wert, 255);
        Turbine.RenderSpriteRotatedOffset(xPos - TileEngine.XOffset + 85.0f + TurbineOff,
                                          yPos - TileEngine.YOffset + 202.0f, 360 - GunWinkel, -30, 0, Color);
    }

    if (Turbine_dran == true)
        Color = D3DCOLOR_RGBA(255, 255, 255, 255);

    Kiefer2.RenderSprite(xPos - TileEngine.XOffset + 78.0f, yPos - TileEngine.YOffset + 180.0f + KieferPos,
                         Color);

    // Corona des Auges rendern
    if (AlreadyDrawn == false) {
        int a = static_cast<int>(Eye_Alpha);
        DirectGraphics.SetAdditiveMode();
        Flare.RenderSpriteRotated(xPos + 100.0f - TileEngine.XOffset, yPos + 50.0f - TileEngine.YOffset,
                                  Eye_Winkel, D3DCOLOR_RGBA(255, 255, 255, a));
        AlreadyDrawn = true;
    }

    DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// Neuen MoveTo Punkt setzen
// --------------------------------------------------------------------------------------

void GegnerMetalHead::MoveToNewPoint(float x, float y, float s, int Aktion) {
    Moving = true;

    NewX = x;
    NewY = y;

    xSpeed = ySpeed = 0.0f;

    if (xPos < x)
        xSpeed = 10.0f;
    if (xPos > x)
        xSpeed = -10.0f;
    if (yPos < y)
        ySpeed = 10.0f;
    if (yPos > y)
        ySpeed = -10.0f;

    Handlung = Aktion;
}

// --------------------------------------------------------------------------------------
// GunWinkel ausrechnen
// --------------------------------------------------------------------------------------

void GegnerMetalHead::WinkelToPlayer() {

    float ydiv = (pAim->ypos + 40) - (yPos + 200);
    if (ydiv == 0.0f)
        ydiv = 0.00001f;

    float xdiv = (pAim->xpos + 35) - (xPos + 100);
    // DKS - converted to float, used new macros:
    // neww = (float)atan(ydiv / xdiv) * 180.0f / PI;
    float neww;

    if (Akt != GEGNER::SCHIESSEN)
        neww = 0.0f;
    else {
        neww = RadToDeg(atanf(ydiv / xdiv));
        neww = std::clamp(neww, -20.0f, 50.0f);
    }

    if (GunWinkel < neww)
        GunWinkel += 8.0f SYNC;

    if (GunWinkel > neww)
        GunWinkel -= 8.0f SYNC;
}

// --------------------------------------------------------------------------------------
// Moven
// --------------------------------------------------------------------------------------

void GegnerMetalHead::DoMove() {
    if (xSpeed < 0.0f && NewX > xPos) {
        xSpeed += 1.0f SYNC;

        if (xSpeed > 0.0f)
            xSpeed = 0.0f;
    }

    if (xSpeed > 0.0f && NewX < xPos) {
        xSpeed -= 1.0f SYNC;

        if (xSpeed < 0.0f)
            xSpeed = 0.0f;
    }

    if (ySpeed < 0.0f && NewY > yPos) {
        ySpeed += 1.0f SYNC;

        if (ySpeed > 0.0f)
            ySpeed = 0.0f;
    }

    if (ySpeed > 0.0f && NewY < yPos) {
        ySpeed -= 1.0f SYNC;

        if (ySpeed < 0.0f)
            ySpeed = 0.0f;
    }

    if (xSpeed == 0.0f && ySpeed == 0.0f) {
        Moving = false;
    }
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerMetalHead::DoKI() {
    if (Handlung != GEGNER::EXPLODIEREN) {
        if (ShadowAlpha > 0.0f)
            ShadowAlpha -= 5.0f SYNC;
        else
            ShadowAlpha = 0.0f;
    } else {
        if (ShadowAlpha < 255.0f)
            ShadowAlpha += 5.0f SYNC;
        else
            ShadowAlpha = 255.0f;
    }

    if (Turbine_dran == false) {
        GegnerRect[METALHEAD].left = 140;
        GegnerRect[METALHEAD].right = 180;
        GegnerRect[METALHEAD].top = 90;
        GegnerRect[METALHEAD].bottom = 130;
    }

    // Spieler unter Kopf? Dann runtercrashen
    if (Handlung != GEGNER::CRUSHENERHOLEN && Handlung != GEGNER::EINFLIEGEN && Handlung != GEGNER::EXPLODIEREN &&
        Handlung != GEGNER::NOTVISIBLE && Handlung != GEGNER::CRUSHEN && Handlung != GEGNER::CRUSHEN2 &&
        yPos < static_cast<float>(Value2 + 230))
        for (int p = 0; p < NUMPLAYERS; p++) {
            if (Player[p].xpos + 35 < xPos + 200) {
                ySpeed = 50.0f;
                Handlung = GEGNER::CRUSHEN;

                SoundManager.PlayWave(100, 128, 11025, SOUND::STONEFALL);
                yAcc = 10.0f;

                Moving = false;
            }
        }

    // Energie anzeigen
    if (Handlung != GEGNER::NOTVISIBLE && Handlung != GEGNER::EXPLODIEREN)
        HUD.ShowBossHUD(4000, Energy);

    // Boss aktivieren und Mucke laufen lassen
    //
    if (Active == true && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        TileEngine.ScrollLevel(static_cast<float>(Value1), static_cast<float>(Value2),
                               TileStateEnum::SCROLLTOLOCK);  // Level auf den Boss zentrieren

        SoundManager.FadeSong(MUSIC::STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
    }

    // Zwischenboss blinkt nicht so lange wie die restlichen Gegner
    if (DamageTaken > 0.0f) {
        // Turbine noch dran? Dann verliert Sie Energie, nicht der Gegner
        //
        if (Turbine_dran == true) {
            // Turbine kaputt? Dann wegfliegen lassen
            //
            if (Energy < 2000.0f) {
                Turbine_dran = false;

                for (int i = 0; i < 20; i++)
                    PartikelSystem.PushPartikel(xPos + 20.0f + static_cast<float>(random(80)),
                                                yPos + 130.0f + static_cast<float>(random(20)), EXPLOSION_MEDIUM2);

                PartikelSystem.PushPartikel(xPos + 85.0f + TurbineOff,
                                            yPos + 202.0f, KAPUTTETURBINE);

                SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION2);
                ShakeScreen(3.0f);
            }
        }

        DamageTaken -= 10 SYNC;  // Rotwerden langsam ausfaden lassen
    } else
        DamageTaken = 0.0f;  // oder ganz anhalten

    // Wenig Energie? Dann Hals dampfen lassen und ggf Funken erzeugen
    if (Energy < 2500 && Handlung != GEGNER::EXPLODIEREN) {
        SmokeCount -= 1.0f SYNC;

        while (SmokeCount < 0.0f) {
            SmokeCount += 0.8f;

            PartikelSystem.PushPartikel(xPos + 50.0f + static_cast<float>(random(15)),
                                        yPos + 160.0f + static_cast<float>(random(4)), SMOKE3);

            if (random(3) == 0)
                PartikelSystem.PushPartikel(xPos + 40.0f + static_cast<float>(random(10)),
                                            yPos + 140.0f + static_cast<float>(random(4)), SMOKEBIG);

            // Zufällig funken sprühen
            if (random(10) == 0) {
                for (int i = 0; i < 16; i++) {
                    PartikelSystem.PushPartikel(xPos + 55.0f + static_cast<float>(random(15)),
                                                yPos + 160.0f + static_cast<float>(random(15)), FUNKE);
                    PartikelSystem.PushPartikel(xPos + 55.0f + static_cast<float>(random(15)),
                                                yPos + 160.0f + static_cast<float>(random(15)), LASERFUNKE2);
                }

                PartikelSystem.PushPartikel(xPos + 30.0f + static_cast<float>(random(20)),
                                            yPos + 140.0f, LASERFLAME);
                PartikelSystem.PushPartikel(xPos + 5.0f, yPos + 110.0f, EXPLOSIONFLARE);
                PartikelSystem.PushPartikel(xPos + 5.0f, yPos + 110.0f, EXPLOSIONFLARE);
                SoundManager.PlayWave(100, 128, 11025, SOUND::FUNKE);
            }
        }
    }

    // Zielpunkt für die Turbine-Partikel setzen
    PartikelSystem.xtarget = xPos + 20.0f + 110.0f;
    PartikelSystem.ytarget = yPos + 130.0f + 55.0f;

    // Hat der Boss keine Energie mehr ? Dann explodiert er
    if (Energy <= 100.0f && Handlung != GEGNER::EXPLODIEREN) {
        Destroyable = false;
        Energy = 100.0f;
        Handlung = GEGNER::EXPLODIEREN;
        AnimEnde = MAXWIRBEL - 1;
        xSpeed = 20.0f;
        ySpeed = -30.0f;
        xAcc = 0.0f;
        yAcc = 5.0f;
        AnimCount = 8.0f;
        Eye_Alpha = 2 * PI;

        for (int p = 0; p < NUMPLAYERS; p++)
            if (Player[p].AufPlattform == this)
                Player[p].AufPlattform = nullptr;

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC::BOSS, -2.0f, 0, false);

        // Kopf fliegt wech (mit Explosion)
        for (int i = 0; i < 60; i++)
            PartikelSystem.PushPartikel(xPos + static_cast<float>(random(10)),
                                        yPos + 90.0f + static_cast<float>(random(10)), FUNKE);

        PartikelSystem.PushPartikel(xPos - 40.0f, yPos + 40.0f, EXPLOSION_GIANT);

        SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION2);
    }

    // Corona des Auges drehen
    Eye_Winkel = static_cast<float>(static_cast<int>(yPos) % 10 % 360);

    // Bei diversen Handlungen den Kiefer bewegen
    //
    if (Handlung == GEGNER::SPECIAL || Handlung == GEGNER::SPECIAL2 || Handlung == GEGNER::SPECIAL3) {
        KieferSpeed -= 2.0f SYNC;
        KieferPos += KieferSpeed SYNC;

        // Oben umdrehen?
        //
        if (KieferPos < 0 && KieferSpeed < 0.0f) {
            KieferSpeed = 10.0f;
            SoundManager.PlayWave(25, 128, 10000 + random(2000), SOUND::KLONG);

            for (int i = 0; i < 10; i++)
                PartikelSystem.PushPartikel(xPos + 130.0f + static_cast<float>(i * 4), yPos + 160.0f, LONGFUNKE);
        }
    }

    // Je nach Handlung richtig verhalten
    //
    if (Moving)
        DoMove();
    else
        switch (Handlung) {
            case GEGNER::CRUSHEN:
            case GEGNER::CRUSHEN2: {
                TurbineOff = 0.0f;
                if (GunWinkel > 0.0f)
                    GunWinkel -= 50.0f SYNC;

                if (GunWinkel < 0.0f)
                    GunWinkel += 50.0f SYNC;

                if (KieferPos > 0.0f)
                    KieferPos -= 50.0f SYNC;
                else
                    KieferPos = 0.0f;

                if (yPos > Value2 + 200) {
                    xSpeed = 0.0f;
                    ySpeed = 0.0f;
                    xAcc = 0.0f;
                    yAcc = -1.0f;

                    ShakeScreen(5.0f);
                    SoundManager.PlayWave(100, 128, 11025, SOUND::STONEEXPLODE);

                    // Schleimboller spawnen?
                    if (Handlung == GEGNER::CRUSHEN2) {
                        int size = 32;

                        switch (Skill) {
                            case SKILL_EASY:     size = 32; break;
                            case SKILL_MEDIUM:   size = 40; break;
                            case SKILL_HARD:     size = 48; break;
                            case SKILL_HURRICAN: size = 56; break;
                        }

                        Gegner.PushGegner(static_cast<float>(Value1 + 50.0f + static_cast<float>(random(550))),
                                          static_cast<float>(Value2 - 40), SCHLEIMBOLLER, size, 0, false);
                    }

                    Handlung = GEGNER::CRUSHENERHOLEN;

                    for (int i = 0; i < 20; i++)
                        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(180)),
                                                    yPos + 200.0f, SMOKEBIG);

                    // Spieler noch drunter? Dann wars das
                    for (int p = 0; p < NUMPLAYERS; p++)
                        if (Player[p].xpos + 35.0f < xPos + 200.0f)
                            Player[p].DamagePlayer(500.0f);
                }
            } break;

            case GEGNER::CRUSHENERHOLEN: {
                if (ySpeed < 0.0f && yPos < Value2 + 50) {
                    ySpeed = 0.0f;
                    yAcc = 0.0f;
                    MoveToNewPoint(static_cast<float>(Value1 + 130),
                                   static_cast<float>(Value2 + 0), 10.0f, GEGNER::SCHIESSEN);
                    ShotArt = 1;
                }
            } break;

            case GEGNER::NOTVISIBLE:  // Warten bis der Screen zentriert wurde
            {
                Handlung = GEGNER::INIT;
            } break;

            case GEGNER::INIT:  // Warten bis der Screen zentriert wurde
            {
                if (TileEngine.Zustand == TileStateEnum::LOCKED) {
                    // Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
                    //
                    // DKS - Added function SongIsPlaying() to SoundManagerClass:
                    if (!SoundManager.SongIsPlaying(MUSIC::BOSS)) {
                        SoundManager.PlaySong(MUSIC::BOSS, false);

                        // Und Boss erscheinen lassen
                        //
                        Handlung = GEGNER::EINFLIEGEN;
                    }
                }
            } break;

            case GEGNER::EINFLIEGEN:  // Kopf erhebt sich aus dem Schrotthaufen
            {
                Energy = 4000;
                DamageTaken = 0.0f;

                Eye_Alpha += 10.0f SYNC;

                if (Eye_Alpha > 255.0f) {
                    Eye_Alpha = 255.0f;

                    MoveToNewPoint(static_cast<float>(Value1 + 70),
                                   static_cast<float>(Value2 + 130), 10.0f, GEGNER::SCHIESSEN);

                    ShotArt = 0;
                    Akt = GEGNER::OEFFNEN;
                    SinCount = 0.0f;
                }
            } break;

            case GEGNER::SCHIESSEN: {
                if (ShotArt == 2) {
                    if (GunWinkel > 0.0f)
                        GunWinkel -= 50.0f SYNC;

                    if (GunWinkel < 0.0f)
                        GunWinkel += 50.0f SYNC;
                } else
                    WinkelToPlayer();

                if (ShotArt == 1) {
                    yPos += 1.5f SYNC;
                    xPos -= 1.0f SYNC;
                }

                KieferPos = static_cast<float>(sin(SinCount)) * 50.0f;

                switch (Akt) {
                    case GEGNER::OEFFNEN: {
                        TurbineOff = static_cast<float>(sin(SinCount)) * 30.0f;
                        SinCount += 0.2f SYNC;

                        if (SinCount >= PI / 2.0f) {
                            Destroyable = true;
                            GegnerRect[GegnerArt].left = static_cast<int>(85.0f + TurbineOff);
                            GegnerRect[GegnerArt].top = 210;
                            GegnerRect[GegnerArt].right = GegnerRect[GegnerArt].left + Turbine.itsXFrameSize;
                            GegnerRect[GegnerArt].bottom = GegnerRect[GegnerArt].top + Turbine.itsYFrameSize + 20;

                            Akt = GEGNER::SCHIESSEN;

                            if (Turbine_dran) {
                                if (ShotArt == 0)
                                    ShotCount = random(4) + 4;
                                else
                                    ShotCount = 15;

                                ShotDelay = 5.0f;
                            } else {
                                ShotCount = 100;
                                ShotDelay = 1.0f;
                            }
                        }

                    } break;

                    case GEGNER::SCHIESSEN: {
                        // DKS - sin(pi/2) is 1.0, so I am optimizing this math:
                        // if (TurbineOff < (float)sin(PI / 2.0f) * 30.0f)
                        //    TurbineOff += 5.0f SYNC;
                        // else
                        //    TurbineOff = (float)sin(PI / 2.0f) * 30.0f;
                        if (TurbineOff < 30.0f)
                            TurbineOff += 5.0f SYNC;
                        else
                            TurbineOff = 30.0f;

                        ShotDelay -= 1.0f SYNC;

                        if (ShotDelay <= 0.0f) {
                            if (ShotCount <= 0) {
                                Akt = GEGNER::SCHLIESSEN;
                                Destroyable = !(Turbine_dran == true);
                            } else {
                                if (Turbine_dran) {
                                    SoundManager.PlayWave(100, 128, 8000 + random(2000), SOUND::FIREBALL);

                                    ShotDelay = 9.0f;

                                    if (ShotArt == 0) {
                                        WinkelUebergabe = GunWinkel + 90;

                                        // DKS - Support new trig sin/cos lookup table and use deg/rad versions of
                                        // sin/cos:  float w = (450.0f - GunWinkel) / 180.0f * PI;
                                        // Projectiles.PushProjectile(xPos + 110 + (float)sin(w) * 70.0f,
                                        //                             yPos + 195 + (float)cos(w) * 70.0f,
                                        //                             FIREBALL_BIG);
                                        float w = 450.0f - GunWinkel;
                                        Projectiles.PushProjectile(xPos + 110.0f + sin_deg(w) * 70.0f,
                                                                   yPos + 195.0f + cos_deg(w) * 70.0f, FIREBALL_BIG);
                                    }

                                    if (ShotArt == 1) {
                                        SoundManager.PlayWave(100, 128, 14000 + random(2000), SOUND::FIREBALL);

                                        for (int i = -40; i <= 40; i += 40) {
                                            WinkelUebergabe = GunWinkel + 90 + i;
                                            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of
                                            // sin/cos:  float w = (450.0f - GunWinkel) / 180.0f * PI;
                                            // Projectiles.PushProjectile(xPos + 130 + (float)sin(w) * 70.0f,
                                            //                             yPos + 200 + (float)cos(w) * 70.0f,
                                            //                             FIREBALL);
                                            float w = 450.0f - GunWinkel;
                                            Projectiles.PushProjectile(xPos + 130.0f + sin_deg(w) * 70.0f,
                                                                       yPos + 200.0f + cos_deg(w) * 70.0f, FIREBALL);
                                        }

                                        if (yPos > (Value2 + 240.0f))
                                            ShotCount = 0;
                                    }

                                    if (ShotArt == 2) {
                                        SoundManager.PlayWave(100, 128, 14000 + random(2000), SOUND::FIREBALL);
                                        Projectiles.PushProjectile(xPos + 180.0f, yPos + 195.0f, FIREBALL_BOMB, pAim);
                                    }

                                    TurbineOff = 15.0f;
                                } else {
                                    ShotDelay = 0.5f;

                                    if (ShotArt == 1)
                                        Projectiles.PushProjectile(xPos + 95.0f,
                                                                   yPos + 185.0f + static_cast<float>(random(5)),
                                                                   WALKERFIRE,
                                                                   pAim);
                                    else {
                                        if (ShotCount % 12 == 0) {
                                            Projectiles.PushProjectile(xPos + 100.0f,
                                                                       yPos + 213.0f + static_cast<float>(random(5)),
                                                                       PFLANZESHOT,
                                                                       pAim);

                                            PartikelSystem.PushPartikel(xPos + 80.0f, yPos + 150.0f, EXPLOSIONFLARE);

                                            PartikelSystem.PushPartikel(xPos + 80.0f, yPos + 150.0f, EXPLOSIONFLARE);
                                        }
                                    }
                                }

                                ShotCount--;
                            }
                        }
                    } break;

                    case GEGNER::SCHLIESSEN: {
                        TurbineOff = static_cast<float>(sin(SinCount)) * 30.0f;

                        SinCount -= 0.2f SYNC;

                        if (SinCount <= 0) {
                            int j;

                            if (Energy < 2000.0f)
                                j = random(4);
                            else
                                j = random(3);

                            while (j == ShotArt) {
                                if (Energy < 2000.0f)
                                    j = random(4);
                                else
                                    j = random(3);
                            }

                            ShotArt = j;

                            if (ShotArt == 2) {
                                Akt = GEGNER::OEFFNEN;
                                MoveToNewPoint(static_cast<float>(Value1 + 10),
                                               static_cast<float>(Value2 + 0), 10.0f, GEGNER::SCHIESSEN);
                            } else if (ShotArt == 1) {
                                Akt = GEGNER::OEFFNEN;
                                MoveToNewPoint(static_cast<float>(Value1 + 130),
                                               static_cast<float>(Value2 + 0), 10.0f, GEGNER::SCHIESSEN);
                            } else if (ShotArt == 0) {
                                Akt = GEGNER::OEFFNEN;
                                MoveToNewPoint(static_cast<float>(Value1 + 70),
                                               static_cast<float>(Value2 + 80), 10.0f, GEGNER::SCHIESSEN);
                            } else {
                                ySpeed = 50.0f;
                                Handlung = GEGNER::CRUSHEN2;

                                SoundManager.PlayWave(100, 128, 11025, SOUND::STONEFALL);
                                yAcc = 10.0f;

                                Moving = false;

                                Akt = GEGNER::OEFFNEN;
                                SinCount = 0.0f;
                            }
                        }
                    } break;
                }
            }
                [[fallthrough]];

            // Head flies towards xto/yto
            case GEGNER::STEHEN: {
                // Open jaw when headbanging
                //
                // FIXME
                // Akt is initialized to SK_AUSGANG
                // and then only assigned values GEGNER::OEFFNEN, GEGNER::SCHLIESSEN and GEGNER::SCHIESSEN
                // Something's broken here...
                if (Akt == SK_BANGEN) {
                    if (KieferPos < 60.0f)
                        KieferPos += 5.0f SYNC;
                    else
                        KieferPos = 60.0f;
                }

                // Close again when pulling back into the starting position
                //
                if (Akt == SK_AUSGANG) {
                    if (KieferPos > 0.0f)
                        KieferPos -= 5.0f SYNC;
                    else
                        KieferPos = 0.0f;
                }

            } break;

            case GEGNER::EXPLODIEREN: {
                Energy = 100.0f;

                Eye_Alpha -= 0.3f SYNC;
                while (Eye_Alpha < 0)
                    Eye_Alpha += 2 * PI;

                AnimCount -= 1.0f SYNC;

                // Ein weiterer Halswirbel explodiert und fliegt weg
                //
                if (AnimCount < 0.0f) {
                    PartikelSystem.PushPartikel(Hals[AnimEnde].x + TileEngine.XOffset,
                                                Hals[AnimEnde].y, HALSWIRBEL);

                    PartikelSystem.PushPartikel(Hals[AnimEnde].x - 20.0f + TileEngine.XOffset,
                                                Hals[AnimEnde].y - Hals[AnimEnde].w / 3.0f, EXPLOSION_MEDIUM2);

                    for (int i = 0; i < 30; i++)
                        PartikelSystem.PushPartikel(
                            Hals[AnimEnde].x + static_cast<float>(random(20) - 20) + TileEngine.XOffset,
                            Hals[AnimEnde].y + static_cast<float>(random(20) - 20), FUNKE);

                    SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND::EXPLOSION1);
                    AnimCount = 2.5f;
                    AnimEnde--;

                    // Herumfliegender Kopf explodiert vollständig, nachdem 5 Halswirbel fehlen
                    //
                    if (AnimEnde == MAXWIRBEL - 5) {
                        for (int i = 0; i < 20; i++)
                            PartikelSystem.PushPartikel(xPos + 10.0f + static_cast<float>(random(160)),
                                                        yPos + 20.0f + static_cast<float>(random(80)),
                                                        EXPLOSION_MEDIUM2);

                        for (int i = 0; i < 5; i++)
                            PartikelSystem.PushPartikel(xPos + static_cast<float>(random(160)),
                                                        yPos + 40.0f + random(80), EXPLOSION_BIG);

                        for (int i = 0; i < 40; i++)
                            PartikelSystem.PushPartikel(xPos + static_cast<float>(random(180)),
                                                        yPos + static_cast<float>(random(140)), SCHROTT1);

                        for (int i = 0; i < 10; i++)
                            PartikelSystem.PushPartikel(xPos + 30.0f + static_cast<float>(random(160)),
                                                        yPos + 50.0f + static_cast<float>(random(80)), SPLITTER);

                        for (int i = 0; i < 100; i++)
                            PartikelSystem.PushPartikel(xPos + 30.0f + static_cast<float>(random(160)),
                                                        yPos + 50.0f + static_cast<float>(random(80)), SMOKE3);

                        SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION2);
                    }

                    if (AnimEnde <= 0)
                        Energy = 0.0f;
                }

            } break;

            default:
                break;
        }  // switch

    // Testen, ob der Spieler den Schädel berührt hat
    //
    RECT_struct r;
    r.top = 0;
    r.left = 0;
    r.bottom = 140;
    r.right = 110;

    for (int p = 0; p < NUMPLAYERS; p++)
        if (Handlung != GEGNER::EXPLODIEREN &&
            SpriteCollision(xPos + 50.0f, yPos + 130.0f, r,
                            Player[p].xpos, Player[p].ypos, Player[p].CollideRect) == true) {
            // Beim Ansaugen gleich viel abziehen und Ansaugen beenden
            //
            if (Handlung == GEGNER::SPECIAL2 &&
                    Player[p].Handlung != PlayerActionEnum::RADELN &&
                    Player[p].Handlung != PlayerActionEnum::RADELN_FALL) {
                    Player[p].DamagePlayer(500.0f);
                AnimPhase = 1600;
            } else
                Player[p].DamagePlayer(static_cast<float>(4.0 SYNC));
        }
}

// --------------------------------------------------------------------------------------
// MetalHead explodiert
// --------------------------------------------------------------------------------------

void GegnerMetalHead::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION2);

    Player[0].Score += 12500;

    ScrolltoPlayeAfterBoss();
}
