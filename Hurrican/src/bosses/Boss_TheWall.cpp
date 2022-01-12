// --------------------------------------------------------------------------------------
// Die Endbosswand
//
// Der allerletzte State des Endgegners
// --------------------------------------------------------------------------------------

#include "Boss_TheWall.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerTheWall::GegnerTheWall(int Wert1, int Wert2, bool Light) {
    Active = true;
    Handlung = GEGNER::INIT;
    BlickRichtung = LINKS;
    Energy = 4000;
    OldEnergy = Energy;
    ChangeLight = Light;
    Destroyable = false;
    Value1 = Wert1;
    Value2 = Wert2;
    TestBlock = false;
    OwnDraw = true;
    KringelWinkel = 0.0f;
    HeartBeat = 0.0f;
    HeartAnim = 0;
    DoorOffset = 0.0f;
    CountOpen = 3;
    SkullEnergy = 250;
    Value1 = -1;
    VorneX = 0.0f;
    VorneY = 0.0f;
    AnimCount = 10.0f;
    Laughing = 0.0f;
    dummy = 0.0f;
    SkullShotDelay = 0.0f;

    // DKS - Added initializers after valgrind reported use of uninitialized data:
    SmokeDelay = 0.0f;
    ShotDelay = 0.0f;

    // Zusätzliche Grafiken laden
    //
    Tuer.LoadImage("endbosstuer.png", 288, 185, 288, 185, 1, 1);
    Kringel[0].LoadImage("endbosskringel.png", 192, 191, 192, 191, 1, 1);
    Kringel[1].LoadImage("endbosskringel2.png", 570, 382, 190, 191, 3, 2);
    Brain.LoadImage("endbosshirni.png", 129, 157, 38, 42, 2, 1);
    Rahmen.LoadImage("endbossrahmen.png", 285, 217, 285, 217, 1, 1);
    Vorne.LoadImage("endbossmaschine_vorne.png", 136, 223, 136, 223, 1, 1);
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

const int TunnelOffx = -320;
const int TunnelOffY = +140;

void GegnerTheWall::DoDraw() {
    if (PlayerAbstand() > 800)
        return;

    // DKS - Added initialization to 0 to silence compiler warning:
    D3DCOLOR Color = 0, col = 0;

    // DKS - Added initialization to 0 to silence compiler warning:
    int toff = 0;

    if (AlreadyDrawn == false) {
        toff = static_cast<int>(sin(DoorOffset) * 100);
        int const comp = toff * 2 + 50;
        col = D3DCOLOR_RGBA(comp, comp, comp, 255);

        int Wert = 255 - (static_cast<int>(DamageTaken));

        if (DirectGraphics.GetBlendMode() == BlendModeEnum::ADDITIV)
            Color = D3DCOLOR_RGBA(Wert, 0, 0, 255);
        else
            Color = 0xFFFFFFFF;

        // Maschine rechts
        pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                               yPos - TileEngine.YOffset, 0xFFFFFFFF);

        // Vorderteil
        Vorne.RenderSprite(xPos - TileEngine.XOffset + VorneX + 3.0f,
                           yPos - TileEngine.YOffset + VorneY + 148.0f, 0xFFFFFFFF);
    }

    // Skeletorkopf rendern
    if (Value1 == 1) {
        Brain.SetRect(0, 123, 27, 157);
        if (DirectGraphics.GetBlendMode() == BlendModeEnum::ADDITIV)
            DirectGraphics.SetAdditiveMode();
        Brain.RenderSprite(xPos - TileEngine.XOffset + 3.0f + VorneX,
                           yPos - TileEngine.YOffset + 228.0f + VorneY, Color);
    }

    if (AlreadyDrawn == true)
        return;

    // Kringel im Hintergrund
    Kringel[1].RenderSprite(xPos - TileEngine.XOffset + static_cast<float>(TunnelOffx + 38),
                            yPos - TileEngine.YOffset + static_cast<float>(TunnelOffY - 14),
                            static_cast<int>(KringelWinkel / 3.0f) % 5, col);

    // Herz rendern
    if (Handlung != GEGNER::EXPLODIEREN) {
        Brain.RenderSprite(xPos - TileEngine.XOffset + static_cast<float>(TunnelOffx + 116),
                           yPos - TileEngine.YOffset + static_cast<float>(TunnelOffY + 59), HeartAnim, col);

        // Suppe rendern
        DirectGraphics.SetAdditiveMode();
        int const alpha = toff + toff/2;
        D3DCOLOR supcol1 = D3DCOLOR_RGBA(150, 0, 0, alpha);
        D3DCOLOR supcol2 = D3DCOLOR_RGBA(150, 0, 0, alpha);
        int start = static_cast<int>((4000.0f - Energy * 0.75f) / 4000.0f * 42.0f);

        RenderRect4(xPos - TileEngine.XOffset + static_cast<float>(TunnelOffx + 117),
                    yPos - TileEngine.YOffset + static_cast<float>(TunnelOffY + 59 + start), 37,
                    static_cast<float>(42 - start), supcol1, supcol1, supcol2, supcol2);

        DirectGraphics.SetColorKeyMode();
    }

    // Röhrenaufhängung rendern
    Brain.SetRect(76, 0, 129, 157);
    Brain.RenderSprite(xPos - TileEngine.XOffset + static_cast<float>(TunnelOffx + 110),
                       yPos - TileEngine.YOffset + static_cast<float>(TunnelOffY + 2), col);

    // abdeckenden Kringel rendern
    // DKS - Changed to use simple version of RenderSpriteRotated (original code was
    //      specifying anim parameter than never changed from zero)
    // Kringel[0].RenderSpriteRotated(xPos - TileEngine.XOffset + TunnelOffx + 37,
    //                               yPos - TileEngine.YOffset + TunnelOffY - 16, 360.0f -
    //                               KringelWinkel, 0, col);
    Kringel[0].RenderSpriteRotated(xPos - TileEngine.XOffset + static_cast<float>(TunnelOffx + 37),
                                   yPos - TileEngine.YOffset + static_cast<float>(TunnelOffY - 16),
                                   360.0f - KringelWinkel, col);

    // Türen

    // Tür links
    Tuer.SetRect(toff, 0, 138, 185);

    // Schatten
    Tuer.RenderSprite(xPos - TileEngine.XOffset + static_cast<float>(TunnelOffx + 20),
                      yPos - TileEngine.YOffset + static_cast<float>(TunnelOffY - 10), 0x80000000);

    Tuer.RenderSprite(xPos - TileEngine.XOffset + static_cast<float>(TunnelOffx + 10),
                      yPos - TileEngine.YOffset + static_cast<float>(TunnelOffY - 10), -1, 0xFF333333, col, 0xFF333333,
                      col);

    // Tür rechts
    Tuer.SetRect(138, 0, 266 - toff, 185);

    Tuer.RenderSprite(xPos - TileEngine.XOffset + static_cast<float>(TunnelOffx + 128 + toff),
                      yPos - TileEngine.YOffset + static_cast<float>(TunnelOffY - 10), -1, col, 0xFF333333, col,
                      0xFF333333);

    // rahmen aussenrum
    Rahmen.RenderSprite(xPos - TileEngine.XOffset + static_cast<float>(TunnelOffx - 8),
                        yPos - TileEngine.YOffset + static_cast<float>(TunnelOffY - 26), 0xFFA0A0A0);

    AlreadyDrawn = true;
}

// --------------------------------------------------------------------------------------
// Neue Aktion
// --------------------------------------------------------------------------------------

void GegnerTheWall::NeueAktion() {
    // Wieder aufmachen?
    if (CountOpen <= 0) {
        CountOpen = 1;
        AnimCount = 50.0f;
        Handlung = GEGNER::OEFFNEN;
        SoundManager.PlayWave(100, 128, 10000 + random(1000), SOUND::DOOR);

        // Laser beenden
        Projectiles.ClearType(SPIDERLASER);
        SoundManager.StopWave(SOUND::SPIDERLASER);
        SoundManager.StopWave(SOUND::BEAMLOAD2);

        // Schädel rumfliegen lassen
        Value1 = -1;
        Gegner.PushGegner(xPos + 3.0f, yPos + 228.0f, SKULL, 99, 0, false);
    } else {
        int i = random(3);

        while (i == LastAction)
            i = random(3);

        // Steht ein Spieler auf den Kisten? Dann Laser. Sonst Boden Elektro-Shock
        bool DoLaser = false;

        for (int p = 0; p < NUMPLAYERS; p++)
            if (Player[p].ypos < TileEngine.YOffset + 480.0f - 20.0f * 8)
                DoLaser = true;

        if (DoLaser == true)
            i = 2;

        LastAction = i;

        switch (i) {
            case 0: {
                ShotCount = 20;
                ShotDelay = 5.0f;
                Handlung = GEGNER::SPECIAL;
                dummy = static_cast<float>(random(10)) + 15;
            } break;

            case 1: {
                ShotCount = 16;
                ShotDelay = 5.0f;
                Handlung = GEGNER::SPECIAL2;
            } break;

            case 2: {
                ShotCount = 2;
                ShotDelay = 20.0f;

                if (DoLaser)
                    Handlung = GEGNER::BOMBARDIEREN;
                else
                    Handlung = GEGNER::SPECIAL3;

            } break;
        }
    }

    Laughing = 10.0f;
    SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND::LAUGH);
}

// --------------------------------------------------------------------------------------
// Den Schädel wieder zurückpfeiffen
// --------------------------------------------------------------------------------------

void GegnerTheWall::ReturnSkull() {
    GegnerClass *pTemp = Gegner.pStart;

    while (pTemp != nullptr) {
        if (pTemp->GegnerArt == SKULL)
            pTemp->Handlung = GEGNER::SPECIAL2;

        pTemp = pTemp->pNext;
    }
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerTheWall::DoKI() {
    // Auf den Spieler schiessen?
    if (Energy < 1500.0f && Value1 == 1) {
        SkullShotDelay -= Timer.sync(1.0f);

        if (SkullShotDelay < 0.0f) {
            SkullShotDelay = 15.0f;
            Projectiles.PushProjectile(xPos, yPos + 240.0f, SUCHSCHUSS2, pAim);
            SoundManager.PlayWave(50, 128, 14000 + random(2000), SOUND::GOLEMSHOT);
        }
    }

    if (Laughing > 0.0f) {
        Laughing -= Timer.sync(1.0f);
        return;
    }

    if (Handlung != GEGNER::EXPLODIEREN) {
        // Vorderteil zurückfahren?
        if (VorneX > 0.0f)
            VorneX -= Timer.sync(1.0f);
        else
            VorneX = 0.0f;

        // getroffen? Dann dem Schädel die Energie abziehen
        if (Energy < OldEnergy) {
            if (SkullEnergy > 0.0f) {
                SkullEnergy -= (OldEnergy - Energy);
                Energy = OldEnergy;
            } else {
                if (Handlung != GEGNER::OEFFNEN && Handlung != GEGNER::SCHLIESSEN) {
                    CountOpen = 0;
                    NeueAktion();
                }
            }
        }

        Destroyable = false;

        // Schädel kann man treffen?
        if (Handlung != GEGNER::OEFFNEN && Handlung != GEGNER::SCHLIESSEN && Value1 == 1) {
            GegnerRect[GegnerArt].left = 5;
            GegnerRect[GegnerArt].top = 232;
            GegnerRect[GegnerArt].right = 5 + 20;
            GegnerRect[GegnerArt].bottom = 232 + 28;
            Destroyable = true;
        }

        // Herz kann man treffen
        if (Handlung == GEGNER::OEFFNEN || Handlung == GEGNER::SCHLIESSEN) {
            GegnerRect[GegnerArt].left = TunnelOffx + 116;
            GegnerRect[GegnerArt].top = TunnelOffY + 59;
            GegnerRect[GegnerArt].right = TunnelOffx + 116 + 38;
            GegnerRect[GegnerArt].bottom = TunnelOffY + 59 + 42;
            Destroyable = true;
        }

        // Kringel drehen lassen
        KringelWinkel += Timer.sync(10.0f);

        clampAngle(KringelWinkel);

        // Herz klopfen lassen
        float e = 1.0f;

        if (Energy < 3500.0f)
            e = 1.0f;
        if (Energy < 3000.0f)
            e = 2.0f;
        if (Energy < 2500.0f)
            e = 3.0f;
        if (Energy < 2000.0f)
            e = 4.0f;
        if (Energy < 1500.0f)
            e = 5.0f;
        if (Energy < 1000.0f)
            e = 6.0f;
        if (Energy < 500.0f)
            e = 7.0f;
        if (Energy < 250.0f)
            e = 8.0f;
        if (Energy < 150.0f)
            e = 9.0f;

        HeartBeat += Timer.sync(e);

        if (HeartBeat > 10.0f) {
            if (HeartAnim == 0) {
                HeartAnim = 1;
                HeartBeat = 5.0f;

                if (Handlung == GEGNER::OEFFNEN || Handlung == GEGNER::SCHLIESSEN)
                    SoundManager.PlayWave(100, 128, 11025, SOUND::HEART2);
            } else {
                HeartAnim = 0;
                HeartBeat = 0.0f;

                if (Handlung == GEGNER::OEFFNEN || Handlung == GEGNER::SCHLIESSEN)
                    SoundManager.PlayWave(100, 128, 11025, SOUND::HEART1);
            }
        }

        // Rauch
        SmokeDelay -= Timer.sync(1.0f);

        if (SmokeDelay < 0.0f) {
            SmokeDelay = 0.2f;

            if (Energy < 3500)
                PartikelSystem.PushPartikel(xPos + 82.0f, yPos + 260.0f, SMOKE3);

            if (Energy < 3000)
                PartikelSystem.PushPartikel(xPos + 180.0f, yPos + 250.0f, SMOKE3);

            if (Energy < 2500)
                PartikelSystem.PushPartikel(xPos + 110.0f, yPos + 320.0f, SMOKE3);

            if (Energy < 2000)
                PartikelSystem.PushPartikel(xPos + 255.0f, yPos + 130.0f, SMOKE3);

            if (Energy < 1500)
                if (random(20) == 0) {
                    PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(10)),
                                                yPos + 220.0f + static_cast<float>(random(10)), LASERFLAME);

                    for (int i = 0; i < 10; i++)
                        PartikelSystem.PushPartikel(xPos + 10.0f + static_cast<float>(random(10)),
                                                    yPos + 240.0f + static_cast<float>(random(10)), FUNKE);

                    for (int i = 0; i < 10; i++)
                        PartikelSystem.PushPartikel(xPos + 20.0f + static_cast<float>(random(2)),
                                                    yPos + 240.0f + static_cast<float>(random(10)), LASERFUNKE2);
                }
        }

        // Energie anzeigen
        if (Handlung != GEGNER::INIT)
            HUD.ShowBossHUD(4000, Energy);

        // Boss aktivieren und Mucke laufen lassen
        //
        if (Active == true && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
            if (PlayerAbstand() < 800 && PlayerAbstandHoriz() < 400.0f) {
                TileEngine.ScrollLevel(xPos - 355.0f, yPos, TileStateEnum::SCROLLTOLOCK);  // Level auf den Boss zentrieren
                SoundManager.FadeSong(MUSIC::STAGEMUSIC, -2.0f, 0, true);         // Ausfaden und pausieren
            }
        }

        // Keine Energie mehr? Dann explodieren lassen
        if (Energy < 100.0f) {
            Energy = 100.0f;
            Destroyable = false;

            // Level wieder zum Spieler scrollen und dann weiterscrollen lassen
            TileEngine.ScrollLevel(Player[0].xpos - 300.0f, Player[0].ypos - 280.0f, TileStateEnum::SCROLLTOPLAYER);

            // Rad checken
            for (int p = 0; p < NUMPLAYERS; p++) {
                if (Player[p].Handlung == PlayerActionEnum::RADELN ||
                        Player[p].Handlung == PlayerActionEnum::RADELN_FALL)
                    Player[p].Handlung = PlayerActionEnum::LAUFEN;
            }

            ShotCount = 1;
            Handlung = GEGNER::EXPLODIEREN;
            Laughing = 0.0f;

            // Alle Schüsse löschen
            Projectiles.ClearAll();

            // Zeitlupe
            Timer.SetMoveSpeed(1.0f);

            // Musiken anhalten
            // DKS - SoundManagerClass has new functions for this:
            // SoundManager.StopAllSongs(true);
            // SoundManager.StopAllSounds();
            SoundManager.PauseSongs();
            SoundManager.StopSounds();

            // Glassplitter
            SoundManager.PlayWave(100, 128, 11025, SOUND::GLASSBREAK);

            for (int i = 0; i < 20; i++)
                PartikelSystem.PushPartikel(xPos + static_cast<float>(TunnelOffx + 114 + random(30)),
                                            yPos + static_cast<float>(TunnelOffY + 59 + random(45)),
                                            SCHLEIM);

            for (int i = 0; i < 80; i++)
                PartikelSystem.PushPartikel(xPos + static_cast<float>(TunnelOffx + 114 + random(30)),
                                            yPos + static_cast<float>(TunnelOffY + 59 + random(45)),
                                            GLASSPLITTER);

            // Wie lange warten?
            AnimCount = 100.0f;

            // Den Schädel verschwinden lassen
            GegnerClass *pTemp;
            pTemp = Gegner.pStart;

            while (pTemp != nullptr) {
                if (pTemp->GegnerArt == SKULL) {
                    pTemp->Value1 = 0;
                    pTemp->AnimCount = 0.5f;
                    pTemp->Handlung = GEGNER::SPECIAL3;
                }

                pTemp = pTemp->pNext;
            }
        }
    } else {
        // int x = 0;    // PICKLE not used
    }

    // je nach handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::INIT: {
            if (PlayerAbstandHoriz() < 400.0f && PlayerAbstandVert() < 400.0f && PlayerAbstand(true) < 600.0f) {
                // Den Schädel wieder zurückpfeiffen
                if (Value1 == -1) {
                    Value1 = -2;
                    ReturnSkull();
                    SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND::LAUGH);
                }

                // und loslegen (nach kurzer Pause)
                if (Value1 == 1) {
                    AnimCount -= Timer.sync(1.0f);
                    if (AnimCount <= 0.0f)
                        NeueAktion();
                }
            }
        } break;

        case GEGNER::SPECIAL: {
            ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay < 0.0f) {
                WinkelUebergabe = dummy;

                SoundManager.PlayWave(50, 128, 8000 + random(2000), SOUND::GOLEMSHOT);

                Projectiles.PushProjectile(xPos, yPos + 332.0f, GOLEMSCHUSS);
                PartikelSystem.PushPartikel(xPos - 30.0f, yPos + 295.0f, EXPLOSIONFLARE);
                PartikelSystem.PushPartikel(xPos, yPos + 320.0f, SMOKEBIG);
                ShotDelay = 1.2f;
                VorneX = 5.0f;
                ShotCount--;

                if (ShotCount <= 0)
                    NeueAktion();
            }
        } break;

        // lila boller schiessen
        case GEGNER::SPECIAL2: {
            ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay < 0.0f) {
                SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND::LILA);
                Projectiles.PushProjectile(xPos, yPos + 332.0f, SPIDERSHOT2);
                PartikelSystem.PushPartikel(xPos - 30.0f, yPos + 295.0f, EXPLOSIONFLARE);
                PartikelSystem.PushPartikel(xPos, yPos + 320.0f, SMOKEBIG);
                ShotDelay = 5.0f;
                VorneX = 5.0f;
                ShotCount--;

                if (ShotCount <= 0)
                    NeueAktion();
            }
        } break;

        // Elektropampe schiessen
        case GEGNER::SPECIAL3: {
            ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay < 0.0f) {
                if (ShotCount > 1) {
                    SoundManager.PlayWave(128, 128, 11025, SOUND::LILA);
                    SoundManager.PlayWave(128, 128, 25050, SOUND::GOLEMLOAD);
                    Projectiles.PushProjectile(xPos + 220.0f, yPos + 210.0f, ELEKTROSCHUSS);
                }

                ShotDelay = 30.0f;
                ShotCount--;

                if (ShotCount <= 0)
                    NeueAktion();
            }
        } break;

        // Fetten Laser schiessen
        case GEGNER::BOMBARDIEREN: {
            ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay < 0.0f) {
                if (ShotCount > 1) {
                    Projectiles.PushProjectile(TileEngine.XOffset - 40.0f, yPos + 240.0f,
                                               SPIDERLASER);
                    SoundManager.PlayWave(100, 128, 11025, SOUND::BEAMLOAD2);
                }

                ShotDelay = 60.0f;
                ShotCount--;

                if (ShotCount <= 0)
                    NeueAktion();
            }
        } break;

        case GEGNER::STEHEN: {
            if (Value1 == 1)
                NeueAktion();
        } break;

        case GEGNER::OEFFNEN: {
            // Tür bewegen
            if (DoorOffset < HALF_PI)
                DoorOffset += Timer.sync(0.075f);

            // Tür offen? Dann countdown, bis sie wieder zugeht
            else {
                DoorOffset = HALF_PI;

                // Tür öffnen Sound anhalten
                // DKS - Added function WaveIsPlaying() to SoundManagerClass:
                if (SoundManager.WaveIsPlaying(SOUND::DOOR)) {
                    SoundManager.StopWave(SOUND::DOOR);
                    SoundManager.PlayWave(100, 128, 22000, SOUND::STEAM2);
                }

                AnimCount -= Timer.sync(1.0f);
                if (AnimCount < 0.0f) {
                    SoundManager.PlayWave(100, 128, 10000 + random(1000), SOUND::DOOR);
                    Handlung = GEGNER::SCHLIESSEN;
                }
            }

        } break;

        case GEGNER::SCHLIESSEN: {
            // Tür
            DoorOffset += Timer.sync(0.125f);

            if (DoorOffset > PI) {
                DoorOffset = 0.0f;
                ShakeScreen(2.0f);
                SoundManager.PlayWave(100, 128, 11000 + random(1500), SOUND::DOORSTOP);

                // Tür öffnen Sound anhalten
                SoundManager.StopWave(SOUND::DOOR);

                for (int i = 0; i < 36; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(TunnelOffx + 110 + random(30)),
                                                yPos + static_cast<float>(TunnelOffY + random(10) + i * 5 - 25),
                                                SMOKE);

                for (int i = 0; i < 15; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(TunnelOffx + 95 + random(30)),
                                                yPos + static_cast<float>(TunnelOffY + random(10) + i * 10 - 25),
                                                SMOKEBIG);

                Handlung = GEGNER::STEHEN;
                AnimCount = 20.0f;
                OldEnergy = Energy;
                SkullEnergy = 250.0f;

                // Den Schädel wieder zurückpfeiffen
                ReturnSkull();
            }
        } break;

        case GEGNER::EXPLODIEREN: {
            AnimCount -= Timer.sync(1.0f);

            if (AnimCount < 0.0f) {
                if (ShotCount > 0) {
                    ShotCount = 0;
                    ScrolltoPlayeAfterBoss();
                }
            }
        } break;
    }
}

// --------------------------------------------------------------------------------------
// Explodieren
// --------------------------------------------------------------------------------------

void GegnerTheWall::GegnerExplode() {}
