// --------------------------------------------------------------------------------------
// Der letzte Endboss ... Robo-Skeletor :)
//
// --------------------------------------------------------------------------------------

#include "Boss_Skeletor.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSkeletor::GegnerSkeletor(int Wert1, int Wert2, bool Light) {
    AnimPhase = 10;
    Handlung = GEGNER::NOTVISIBLE;
    BlickRichtung = DirectionEnum::RECHTS;
    Value1 = Wert1;
    Value2 = Wert2;
    Energy = 7000;
    ChangeLight = Light;
    Destroyable = false;
    TestBlock = false;
    OwnDraw = true;
    SmokeDelay = 0.0f;
    ShotDelay = 0.0f;
    ShotCount = 0;
    GunWinkel = 0.0f;
    HasLaughed = false;
    LastHandlung = -1;
    DrawNow = false;

    // Zusätzliche Grafiken laden
    //
    Flamme.LoadImage("skeletor_flame.png", 160, 140, 160, 70, 1, 2);
}

// --------------------------------------------------------------------------------------
// Lachen ;)
// --------------------------------------------------------------------------------------

void GegnerSkeletor::Laugh() {
    AnimPhase = 2;
    SoundManager.PlayWave(100, 128, 11025, SOUND::LAUGH);

    xSpeed = 0.0f;
    ySpeed = 0.0f;
    xAcc = 0.0f;
    yAcc = 0.0f;
    Handlung = GEGNER::SPECIAL;
    AnimCount = 20.0f;
}

// --------------------------------------------------------------------------------------
// Winkel zum Spieler berechnen
// --------------------------------------------------------------------------------------

void GegnerSkeletor::CalcGunWinkel() {

    float ydiv = (pAim->ypos + 40.0f) - (yPos + 100.0f);
    if (ydiv == 0.0f)
        ydiv = 0.00001f;

    float xdiv = (pAim->xpos + 35.0f) - (xPos + 60.0f);
    // DKS - converted to float:
    // GunWinkel = 90.0f + (float)atan(ydiv / xdiv) * 180.0f / PI;
    GunWinkel = 90.0f + RadToDeg(atanf(ydiv / xdiv));

    GunWinkel = std::clamp(GunWinkel, 50.0f, 120.0f);
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerSkeletor::DoDraw() {
    if (DrawNow == false)
        return;

    if (Handlung == GEGNER::EXPLODIEREN)
        return;

    float yoff = 0.0f;

    int Wert = 255 - (static_cast<int>(DamageTaken));
    D3DCOLOR Color = D3DCOLOR_RGBA(255, Wert, Wert, 255);
    bool mirror = BlickRichtung == DirectionEnum::RECHTS;

    if (AnimPhase >= 10)
        yoff = 10.0f;

    /*
    if (Handlung == GEGNER::SPRINGEN)
    {
        DirectGraphics.SetAdditiveMode();
        LavaFlare.RenderSpriteScaled(xPos - static_cast<float>(TileEngine.XOffset) - 90,
                                     yPos - static_cast<float>(TileEngine.YOffset) - 50,
                                     300, 300, 0, 0x88FF8822);

        LavaFlare.RenderSpriteScaled(xPos - static_cast<float>(TileEngine.XOffset) - 40,
                                     yPos - static_cast<float>(TileEngine.YOffset),
                                     200, 200, 0, 0x88FFCC66);

        DirectGraphics.SetColorKeyMode();
    }
    */

    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                           yPos - TileEngine.YOffset + yoff, AnimPhase, Color,
                                           mirror);

    if (Handlung == GEGNER::SPECIAL2) {
        for (int i = 0; i < 4; i++) {
            D3DCOLOR col = D3DCOLOR_RGBA(255, 255, 255, 200 - i * 30);
            pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset - xSpeed * static_cast<float>(i * 2),
                                                   yPos - TileEngine.YOffset + yoff, AnimPhase, col,
                                                   mirror);
        }
    }

    // Flamme rendern
    if (Handlung == GEGNER::SCHIESSEN && ShotDelay > 0.5f) {
        DirectGraphics.SetAdditiveMode();

        int foff = -120;
        if (mirror)
            foff = 87;

        Flamme.itsRect = Flamme.itsPreCalcedRects[ShotCount % 2];

        if (BlickRichtung == DirectionEnum::RECHTS)
            Flamme.RenderSpriteRotatedOffset(xPos - TileEngine.XOffset + static_cast<float>(foff),
                                             yPos - TileEngine.YOffset + 60.0f, 90 - GunWinkel, -50, 0,
                                             0xFFFFFFFF, !mirror);
        else
            Flamme.RenderSpriteRotatedOffset(xPos - TileEngine.XOffset + static_cast<float>(foff),
                                             yPos - TileEngine.YOffset + 60.0f, GunWinkel - 90, -50, 0,
                                             0xFFFFFFFF, !mirror);

        DirectGraphics.SetColorKeyMode();
    }
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSkeletor::DoKI() {
    CalcGunWinkel();

    // Energie anzeigen
    if (Handlung != GEGNER::NOTVISIBLE && Handlung != GEGNER::EXPLODIEREN)
        HUD.ShowBossHUD(7000, Energy);

    // Boss aktivieren und Stagemusik ausfaden
    //
    if (Active == true && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        // Ausserhalb des Screens setzen
        xPos = TileEngine.XOffset + 100.0f;
        yPos = TileEngine.YOffset - 250.0f;
        DrawNow = true;

        TileEngine.ScrollLevel(static_cast<float>(Value1), static_cast<float>(Value2),
                               TileStateEnum::SCROLLTOLOCK);  // Level auf den Boss zentrieren

        SoundManager.FadeSong(MUSIC::STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
    }

    // Je nach Handlung richtig verhalten
    //
    switch (Handlung) {
        // Abwarten nach dem Lachen
        case GEGNER::SPECIAL: {
            AnimCount -= Timer.sync(1.0f);
            if (AnimCount < 0.0f) {
                Handlung = GEGNER::STEHEN;
            }
        } break;

        case GEGNER::CRUSHENERHOLEN: {
            AnimCount += Timer.sync(1.0f);

            if (AnimCount > 10.0f && AnimPhase == 0) {
                // SoundManager.PlayWave(100, 128, 11025, SOUND::STANDUP);
                AnimPhase = 1;
            }

            if (AnimCount > 20.0f && AnimPhase == 1) {
                AnimPhase = 2;

                if (HasLaughed == false)
                    SoundManager.PlayWave(100, 128, 11025, SOUND::LAUGH);
            }

            if (AnimCount > 25.0f) {
                Handlung = GEGNER::STEHEN;
                if (HasLaughed == false) {
                    HasLaughed = true;
                    SoundManager.PlaySong(MUSIC::BOSS, false);
                }
            }

        } break;

        case GEGNER::STEHEN: {
            int j = random(4);

            while (j == LastHandlung)
                j = random(4);

            LastHandlung = j;

            switch (j) {
                case 0: {
                    ShotDelay = 0.0f;
                    ShotCount = 40;

                    Handlung = GEGNER::SCHIESSEN;
                    AnimOffset = 0;
                } break;

                case 1: {
                    ShotDelay = 0.0f;
                    ShotCount = 20;

                    Handlung = GEGNER::BOMBARDIEREN;
                    AnimOffset = 0;
                } break;

                case 2: {
                    ShotDelay = 3.0f;
                    ShotCount = 1;
                    Handlung = GEGNER::SPRINGEN;
                    xSpeed = 25.0f;
                    ySpeed = -60.0f;
                    yAcc = 6.0f;
                } break;

                case 3: {
                    Handlung = GEGNER::SPECIAL2;
                    xSpeed = 0.0f;
                    xAcc = 2.0f;
                    ShotDelay = 9.0f;
                    ShotCount = 5;
                }
            }

            if (xPos > Value1 + 320) {
                xSpeed *= -1;
                xAcc *= -1;
            }
        } break;

        case GEGNER::SPECIAL2: {
            // Rakete schiessen
            if (ShotCount > 0)
                ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay < 0.0f) {
                WinkelUebergabe = 2.0f * ShotCount + 8.0f;
                SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND::ROCKET);
                Projectiles.PushProjectile(xPos + 50.0f, yPos + 80.0f, TURRIEXTRAWURST);

                ShotDelay = 3.0f;
                ShotCount--;
            }

            // Animationsphase setzen
            if ((xSpeed > 31.0f && xAcc > 0.0f) || (xSpeed < -31.0f && xAcc < 0.0f))
                xAcc *= -1.0f;

            if ((xAcc > 0.0f && xSpeed < 0.0f && xSpeed > -2.0f) || (xAcc < 0.0f && xSpeed > 0.0f && xSpeed < 2.0f))
                xAcc = 0.0f;

            if (xPos + 60 < Value1 + 320) {
                int a = static_cast<int>(xPos - TileEngine.XOffset) / 56;
                BlickRichtung = DirectionEnum::RECHTS;
                AnimPhase = 10 + a;
            } else {
                int a = static_cast<int>(xPos - TileEngine.XOffset - 320.0f + 60.0f) / 56;
                BlickRichtung = DirectionEnum::LINKS;
                AnimPhase = 15 - a;
            }

            // Angekommen?
            if (xPos < Value1 + 20.0f || xPos > Value1 + 500) {
                if (xPos < Value1 + 50)
                    xPos = Value1 + 20.0f;
                else
                    xPos = Value1 + 500.0f;

                xSpeed = 0.0f;
                ySpeed = 0.0f;
                xAcc = 0.0f;
                yAcc = 0.0f;

                Laugh();
            }
        } break;

        case GEGNER::SPRINGEN: {
            // Animationsphase setzen

            if (xPos + 60 < Value1 + 320) {
                int a = static_cast<int>(xPos - TileEngine.XOffset) / 56;
                BlickRichtung = DirectionEnum::RECHTS;
                AnimPhase = 10 + a;
            } else {
                int a = static_cast<int>(xPos - TileEngine.XOffset - 320.0f + 60.0f) / 56;
                BlickRichtung = DirectionEnum::LINKS;
                AnimPhase = 15 - a;
            }

            // 5er Schuss abgeben?
            if (ShotCount == 1) {
                if (xPos + 62.0f > Value1 + 300 && xPos + 62.0f < Value1 + 340) {
                    SoundManager.PlayWave(100, 128, 8000 + random(2000), SOUND::FIREBALL);
                    ShotCount = 0;

                    for (int i = 0; i < 5; i++) {
                        WinkelUebergabe = 180.0f - 50 + i * 25;

                        Projectiles.PushProjectile(xPos + 50.0f, yPos + 70.0f, FIREBALL_BIG);
                    }
                }
            }

            // Aufgekommen?
            if (yPos > Value2 + 250) {
                yPos = static_cast<float>(Value2) + 250;
                xSpeed = 0.0f;
                ySpeed = 0.0f;
                xAcc = 0.0f;
                yAcc = 0.0f;

                AnimPhase = 0;
                ySpeed = 0.0f;
                Handlung = GEGNER::CRUSHENERHOLEN;
                AnimCount = 0.0f;

                for (int i = 0; i < 10; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(100) - 20),
                                                yPos + static_cast<float>(random(20) + 130), SMOKEBIG);

                for (int i = 0; i < 10; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(100)),
                                                yPos + static_cast<float>(random(10) + 180), SPIDERSPLITTER);

                ShakeScreen(5.0f);
                SoundManager.PlayWave(100, 128, 11025, SOUND::DOORSTOP);
            }
        } break;

        // Granaten
        case GEGNER::BOMBARDIEREN: {
            ShotDelay -= Timer.sync(0.1f);

            AnimCount -= Timer.sync(1.0f);

            if (AnimCount <= 0.0f) {
                AnimOffset++;

                if (AnimOffset % 2 == 0)
                    AnimCount = 2.0f;
                else {
                    // Granate abfeuern
                    int off = 0;

                    if (BlickRichtung == DirectionEnum::LINKS)
                        off = -95;

                    AnimCount = 1.5f;
                    SoundManager.PlayWave(100, 128, 6000 + random(2000), SOUND::GRANATE);
                    PartikelSystem.PushPartikel(xPos + 50.0f + off, yPos + 35.0f, EXPLOSIONFLARE);
                    PartikelSystem.PushPartikel(xPos + 50.0f + off, yPos + 35.0f, EXPLOSIONFLARE);

                    WinkelUebergabe = 40.0f - static_cast<float>(AnimPhase) * 4 - random(8);

                    if (BlickRichtung == DirectionEnum::LINKS)
                        WinkelUebergabe += 1;

                    if (WinkelUebergabe < 0.0f)
                        WinkelUebergabe = 0.0f;

                    WinkelUebergabe *= Direction::asInt(BlickRichtung);

                    Projectiles.PushProjectile(xPos + 100.0f + off, yPos + 80.0f, SKELETORGRANATE);

                    ShotCount--;
                }

                int a = AnimOffset % 14;

                if (a < 8)
                    AnimPhase = a + 2;
                else
                    AnimPhase = 16 - a;

                if (AnimOffset > 42)
                    AnimOffset = 0;

                // fertig?
                if (ShotCount < 0)
                    Laugh();
            }
        } break;

        // Gatling
        case GEGNER::SCHIESSEN: {
            ShotDelay -= Timer.sync(0.5f);

            // Animationsphase setzen
            if (ShotDelay > 0.5f)
                AnimPhase = 3;
            else
                AnimPhase = 2;

            // schuss abgeben
            if (ShotDelay <= 0.0f) {
                ShotCount--;

                ShotDelay = 1.0f;

                // Hülse
                int off = 70;
                if (BlickRichtung == DirectionEnum::LINKS)
                    off = 30;

                PartikelSystem.PushPartikel(xPos + static_cast<float>(off), yPos + 75.0f, BULLET_SKELETOR);

                // Sound
                SoundManager.PlayWave(100, 128, 10000 + random(400), SOUND::GATLING);

                // Schuss
                WinkelUebergabe = GunWinkel + random(4) - 2;

                if (BlickRichtung == DirectionEnum::LINKS)
                    WinkelUebergabe += 180;

                off = 100;

                if (BlickRichtung == DirectionEnum::LINKS)
                    off = 20;

                Projectiles.PushProjectile(xPos + static_cast<float>(off), yPos + 75.0f, SKELETOR_SHOT);

                // fertig?
                if (ShotCount < 0)
                    Laugh();
            }
        } break;

        case GEGNER::NOTVISIBLE:  // Warten bis der Screen zentriert wurde
        {
            if (TileEngine.Zustand == TileStateEnum::LOCKED) {
                // Boss erscheinen lassen
                //
                SoundManager.PlayWave(100, 128, 11025, SOUND::STONEFALL);
                Handlung = GEGNER::EINFLIEGEN;
                xSpeed = 0.0f;
                ySpeed = 80.0f;
                Destroyable = true;
            }
        } break;

        case GEGNER::EINFLIEGEN:  // Kopf erhebt sich aus dem Schrotthaufen
        {
            BlickRichtung = DirectionEnum::RECHTS;

            if (yPos > Value2 + 250) {
                yPos = static_cast<float>(Value2) + 250;
                AnimPhase = 0;
                ySpeed = 0.0f;
                Handlung = GEGNER::CRUSHENERHOLEN;
                AnimCount = 0.0f;

                for (int i = 0; i < 50; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(100)),
                                                yPos + static_cast<float>(random(150) + 30), SMOKEBIG);

                for (int i = 0; i < 20; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(100)),
                                                yPos + static_cast<float>(random(10) + 180), SPIDERSPLITTER);

                ShakeScreen(5.0f);
                SoundManager.PlayWave(100, 128, 11025, SOUND::DOORSTOP);
            }
        } break;

        case GEGNER::EXPLODIEREN: {
            ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay < 0.0f)
                Energy = 0.0f;

        } break;

        default:
            break;
    }  // switch

    if (Energy < 0.0f && Handlung != GEGNER::EXPLODIEREN) {
        Energy = 1.0f;
        Destroyable = false;
        Handlung = GEGNER::EXPLODIEREN;
        ShotDelay = 20.0f;

        // Schädel spawnen
        Gegner.PushGegner(xPos + 50.0f, yPos + 50.0f, SKULL, 0, 99, false);

        // Explosionen
        // int i = 0;	// PICKLE not used

        for (int i = 0; i < 15; i++)
            PartikelSystem.PushPartikel(xPos + static_cast<float>(random(100)),
                                        yPos + static_cast<float>(random(200)), SPLITTER);

        for (int i = 0; i < 40; i++)
            PartikelSystem.PushPartikel(xPos + static_cast<float>(random(200) - 50),
                                        yPos + static_cast<float>(random(250) - 20), SPIDERSPLITTER);

        for (int i = 0; i < 40; i++)
            PartikelSystem.PushPartikel(xPos + static_cast<float>(random(200) - 50),
                                        yPos + static_cast<float>(random(250) - 20), SCHROTT1);

        for (int i = 0; i < 60; i++)
            PartikelSystem.PushPartikel(xPos + static_cast<float>(random(200) - 50),
                                        yPos + static_cast<float>(random(200) - 50), EXPLOSION_MEDIUM2);

        PartikelSystem.PushPartikel(xPos + 62.0f, yPos + 100.0f, SHOCKEXPLOSION);
        SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION2);
        Player[0].Score += 12500;
        ShakeScreen(5.0f);

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC::BOSS, -2.0f, 0, false);
    }

    TestDamagePlayers(Timer.sync(10.0f));
}

// --------------------------------------------------------------------------------------
// Skeletor explodiert
// --------------------------------------------------------------------------------------

void GegnerSkeletor::GegnerExplode() {
    ScrolltoPlayeAfterBoss();
}
