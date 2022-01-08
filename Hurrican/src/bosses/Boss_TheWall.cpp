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
    Handlung = GEGNER_INIT;
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
        col = D3DCOLOR_RGBA(toff * 2 + 50, toff * 2 + 50, toff * 2 + 50, 255);

        int Wert = 255 - (static_cast<int>(DamageTaken));

        if (DirectGraphics.BlendMode == BlendModeEnum::ADDITIV)
            Color = D3DCOLOR_RGBA(Wert, 0, 0, 255);
        else
            Color = 0xFFFFFFFF;

        // Maschine rechts
        pGegnerGrafix[GegnerArt]->RenderSprite(xPos - static_cast<float>(TileEngine.XOffset),
                                               yPos - static_cast<float>(TileEngine.YOffset), 0xFFFFFFFF);

        // Vorderteil
        Vorne.RenderSprite(xPos - static_cast<float>(TileEngine.XOffset) + VorneX + 3.0f,
                           yPos - static_cast<float>(TileEngine.YOffset) + VorneY + 148.0f, 0xFFFFFFFF);
    }

    // Skeletorkopf rendern
    if (Value1 == 1) {
        Brain.SetRect(0, 123, 27, 157);
        if (DirectGraphics.BlendMode == BlendModeEnum::ADDITIV)
            DirectGraphics.SetAdditiveMode();
        Brain.RenderSprite(xPos - static_cast<float>(TileEngine.XOffset) + 3 + VorneX,
                           yPos - static_cast<float>(TileEngine.YOffset) + 228 + VorneY, Color);
    }

    if (AlreadyDrawn == true)
        return;

    // Kringel im Hintergrund
    Kringel[1].RenderSprite(xPos - static_cast<float>(TileEngine.XOffset) + TunnelOffx + 38,
                            yPos - static_cast<float>(TileEngine.YOffset) + TunnelOffY - 14,
                            static_cast<int>(KringelWinkel / 3.0f) % 5, col);

    // Herz rendern
    if (Handlung != GEGNER_EXPLODIEREN) {
        Brain.RenderSprite(xPos - static_cast<float>(TileEngine.XOffset) + TunnelOffx + 116,
                           yPos - static_cast<float>(TileEngine.YOffset) + TunnelOffY + 59, HeartAnim, col);

        // Suppe rendern
        DirectGraphics.SetAdditiveMode();
        D3DCOLOR supcol1 = D3DCOLOR_RGBA(150, 0, 0, static_cast<int>(toff * 1.5f));
        D3DCOLOR supcol2 = D3DCOLOR_RGBA(150, 0, 0, static_cast<int>(toff * 1.5f));
        int start = static_cast<int>((4000 - Energy * 0.75f) / 4000.0f * 42);

        RenderRect4(xPos - static_cast<float>(TileEngine.XOffset) + TunnelOffx + 117,
                    yPos - static_cast<float>(TileEngine.YOffset) + TunnelOffY + 59 + start, 37,
                    static_cast<float>(42 - start), supcol1, supcol1, supcol2, supcol2);

        DirectGraphics.SetColorKeyMode();
    }

    // Röhrenaufhängung rendern
    Brain.SetRect(76, 0, 129, 157);
    Brain.RenderSprite(xPos - static_cast<float>(TileEngine.XOffset) + TunnelOffx + 110,
                       yPos - static_cast<float>(TileEngine.YOffset) + TunnelOffY + 2, col);

    // abdeckenden Kringel rendern
    // DKS - Changed to use simple version of RenderSpriteRotated (original code was
    //      specifying anim parameter than never changed from zero)
    // Kringel[0].RenderSpriteRotated(xPos - static_cast<float>(TileEngine.XOffset) + TunnelOffx + 37,
    //                               yPos - static_cast<float>(TileEngine.YOffset) + TunnelOffY - 16, 360.0f -
    //                               KringelWinkel, 0, col);
    Kringel[0].RenderSpriteRotated(xPos - static_cast<float>(TileEngine.XOffset) + TunnelOffx + 37,
                                   yPos - static_cast<float>(TileEngine.YOffset) + TunnelOffY - 16,
                                   360.0f - KringelWinkel, col);

    // Türen

    // Tür links
    Tuer.SetRect(toff, 0, 138, 185);

    // Schatten
    Tuer.RenderSprite(xPos - static_cast<float>(TileEngine.XOffset) + TunnelOffx + 20,
                      yPos - static_cast<float>(TileEngine.YOffset) + TunnelOffY - 10, 0x80000000);

    Tuer.RenderSprite(xPos - static_cast<float>(TileEngine.XOffset) + TunnelOffx + 10,
                      yPos - static_cast<float>(TileEngine.YOffset) + TunnelOffY - 10, -1, 0xFF333333, col, 0xFF333333,
                      col);

    // Tür rechts
    Tuer.SetRect(138, 0, 266 - toff, 185);

    Tuer.RenderSprite(xPos - static_cast<float>(TileEngine.XOffset) + TunnelOffx + 128 + toff,
                      yPos - static_cast<float>(TileEngine.YOffset) + TunnelOffY - 10, -1, col, 0xFF333333, col,
                      0xFF333333);

    // rahmen aussenrum
    Rahmen.RenderSprite(xPos - static_cast<float>(TileEngine.XOffset) + TunnelOffx - 8,
                        yPos - static_cast<float>(TileEngine.YOffset) + TunnelOffY - 26, 0xFFA0A0A0);

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
        Handlung = GEGNER_OEFFNEN;
        SoundManager.PlayWave(100, 128, 10000 + random(1000), SOUND_DOOR);

        // Laser beenden
        Projectiles.ClearType(SPIDERLASER);
        SoundManager.StopWave(SOUND_SPIDERLASER);
        SoundManager.StopWave(SOUND_BEAMLOAD2);

        // Schädel rumfliegen lassen
        Value1 = -1;
        Gegner.PushGegner(xPos + 3, yPos + 228, SKULL, 99, 0, false);
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
                Handlung = GEGNER_SPECIAL;
                dummy = static_cast<float>(random(10)) + 15;
            } break;

            case 1: {
                ShotCount = 16;
                ShotDelay = 5.0f;
                Handlung = GEGNER_SPECIAL2;
            } break;

            case 2: {
                ShotCount = 2;
                ShotDelay = 20.0f;

                if (DoLaser)
                    Handlung = GEGNER_BOMBARDIEREN;
                else
                    Handlung = GEGNER_SPECIAL3;

            } break;
        }
    }

    Laughing = 10.0f;
    SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND_LAUGH);
}

// --------------------------------------------------------------------------------------
// Den Schädel wieder zurückpfeiffen
// --------------------------------------------------------------------------------------

void GegnerTheWall::ReturnSkull() {
    GegnerClass *pTemp;
    pTemp = Gegner.pStart;

    while (pTemp != nullptr) {
        if (pTemp->GegnerArt == SKULL)
            pTemp->Handlung = GEGNER_SPECIAL2;

        pTemp = pTemp->pNext;
    }
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerTheWall::DoKI() {
    // Auf den Spieler schiessen?
    if (Energy < 1500.0f && Value1 == 1) {
        SkullShotDelay -= 1.0f SYNC;

        if (SkullShotDelay < 0.0f) {
            SkullShotDelay = 15.0f;
            Projectiles.PushProjectile(xPos, yPos + 240.0f, SUCHSCHUSS2, pAim);
            SoundManager.PlayWave(50, 128, 14000 + random(2000), SOUND_GOLEMSHOT);
        }
    }

    if (Laughing > 0.0f) {
        Laughing -= 1.0f SYNC;
        return;
    }

    if (Handlung != GEGNER_EXPLODIEREN) {
        // Vorderteil zurückfahren?
        if (VorneX > 0.0f)
            VorneX -= 1.0f SYNC;
        else
            VorneX = 0.0f;

        // getroffen? Dann dem Schädel die Energie abziehen
        if (Energy < OldEnergy) {
            if (SkullEnergy > 0.0f) {
                SkullEnergy -= (OldEnergy - Energy);
                Energy = OldEnergy;
            } else {
                if (Handlung != GEGNER_OEFFNEN && Handlung != GEGNER_SCHLIESSEN) {
                    CountOpen = 0;
                    NeueAktion();
                }
            }
        }

        Destroyable = false;

        // Schädel kann man treffen?
        if (Handlung != GEGNER_OEFFNEN && Handlung != GEGNER_SCHLIESSEN && Value1 == 1) {
            GegnerRect[GegnerArt].left = 5;
            GegnerRect[GegnerArt].top = 232;
            GegnerRect[GegnerArt].right = 5 + 20;
            GegnerRect[GegnerArt].bottom = 232 + 28;
            Destroyable = true;
        }

        // Herz kann man treffen
        if (Handlung == GEGNER_OEFFNEN || Handlung == GEGNER_SCHLIESSEN) {
            GegnerRect[GegnerArt].left = TunnelOffx + 116;
            GegnerRect[GegnerArt].top = TunnelOffY + 59;
            GegnerRect[GegnerArt].right = TunnelOffx + 116 + 38;
            GegnerRect[GegnerArt].bottom = TunnelOffY + 59 + 42;
            Destroyable = true;
        }

        // Kringel drehen lassen
        KringelWinkel += 10.0f SYNC;

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

        HeartBeat += e SYNC;

        if (HeartBeat > 10.0f) {
            if (HeartAnim == 0) {
                HeartAnim = 1;
                HeartBeat = 5.0f;

                if (Handlung == GEGNER_OEFFNEN || Handlung == GEGNER_SCHLIESSEN)
                    SoundManager.PlayWave(100, 128, 11025, SOUND_HEART2);
            } else {
                HeartAnim = 0;
                HeartBeat = 0.0f;

                if (Handlung == GEGNER_OEFFNEN || Handlung == GEGNER_SCHLIESSEN)
                    SoundManager.PlayWave(100, 128, 11025, SOUND_HEART1);
            }
        }

        // Rauch
        SmokeDelay -= 1.0f SYNC;

        if (SmokeDelay < 0.0f) {
            SmokeDelay = 0.2f;

            if (Energy < 3500)
                PartikelSystem.PushPartikel(xPos + 82, yPos + 260, SMOKE3);

            if (Energy < 3000)
                PartikelSystem.PushPartikel(xPos + 180, yPos + 250, SMOKE3);

            if (Energy < 2500)
                PartikelSystem.PushPartikel(xPos + 110, yPos + 320, SMOKE3);

            if (Energy < 2000)
                PartikelSystem.PushPartikel(xPos + 255, yPos + 130, SMOKE3);

            if (Energy < 1500)
                if (random(20) == 0) {
                    PartikelSystem.PushPartikel(xPos - 10 + random(10), yPos + 220 + random(10), LASERFLAME);

                    for (int i = 0; i < 10; i++)
                        PartikelSystem.PushPartikel(xPos + 10 + random(10), yPos + 240 + random(10), FUNKE);

                    for (int i = 0; i < 10; i++)
                        PartikelSystem.PushPartikel(xPos + 20 + random(2), yPos + 240 + random(10), LASERFUNKE2);
                }
        }

        // Energie anzeigen
        if (Handlung != GEGNER_INIT)
            HUD.ShowBossHUD(4000, Energy);

        // Boss aktivieren und Mucke laufen lassen
        //
        if (Active == true && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
            if (PlayerAbstand() < 800 && PlayerAbstandHoriz() < 400.0f) {
                TileEngine.ScrollLevel(xPos - 355, yPos, TileStateEnum::SCROLLTOLOCK);  // Level auf den Boss zentrieren
                SoundManager.FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);         // Ausfaden und pausieren
            }
        }

        // Keine Energie mehr? Dann explodieren lassen
        if (Energy < 100.0f) {
            Energy = 100.0f;
            Destroyable = false;

            // Level wieder zum Spieler scrollen und dann weiterscrollen lassen
            TileEngine.ScrollLevel(Player[0].xpos - 300, Player[0].ypos - 280, TileStateEnum::SCROLLTOPLAYER);

            // Rad checken
            for (int p = 0; p < NUMPLAYERS; p++) {
                if (Player[p].Handlung == PlayerActionEnum::RADELN ||
                        Player[p].Handlung == PlayerActionEnum::RADELN_FALL)
                    Player[p].Handlung = PlayerActionEnum::LAUFEN;
            }

            ShotCount = 1;
            Handlung = GEGNER_EXPLODIEREN;
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
            SoundManager.PlayWave(100, 128, 11025, SOUND_GLASSBREAK);

            int i;
            for (i = 0; i < 20; i++)
                PartikelSystem.PushPartikel(xPos + TunnelOffx + 114 + random(30), yPos + TunnelOffY + 59 + random(45),
                                            SCHLEIM);

            for (i = 0; i < 80; i++)
                PartikelSystem.PushPartikel(xPos + TunnelOffx + 114 + random(30), yPos + TunnelOffY + 59 + random(45),
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
                    pTemp->Handlung = GEGNER_SPECIAL3;
                }

                pTemp = pTemp->pNext;
            }
        }
    } else {
        // int x = 0;    // PICKLE not used
    }

    // je nach handlung richtig verhalten
    switch (Handlung) {
        case GEGNER_INIT: {
            if (PlayerAbstandHoriz() < 400.0f && PlayerAbstandVert() < 400.0f && PlayerAbstand(true) < 600.0f) {
                // Den Schädel wieder zurückpfeiffen
                if (Value1 == -1) {
                    Value1 = -2;
                    ReturnSkull();
                    SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND_LAUGH);
                }

                // und loslegen (nach kurzer Pause)
                if (Value1 == 1) {
                    AnimCount -= 1.0f SYNC;
                    if (AnimCount <= 0.0f)
                        NeueAktion();
                }
            }
        } break;

        case GEGNER_SPECIAL: {
            ShotDelay -= 1.0f SYNC;

            if (ShotDelay < 0.0f) {
                WinkelUebergabe = dummy;

                SoundManager.PlayWave(50, 128, 8000 + random(2000), SOUND_GOLEMSHOT);

                Projectiles.PushProjectile(xPos, yPos + 332, GOLEMSCHUSS);
                PartikelSystem.PushPartikel(xPos - 30, yPos + 295, EXPLOSIONFLARE);
                PartikelSystem.PushPartikel(xPos, yPos + 320, SMOKEBIG);
                ShotDelay = 1.2f;
                VorneX = 5.0f;
                ShotCount--;

                if (ShotCount <= 0)
                    NeueAktion();
            }
        } break;

        // lila boller schiessen
        case GEGNER_SPECIAL2: {
            ShotDelay -= 1.0f SYNC;

            if (ShotDelay < 0.0f) {
                SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND_LILA);
                Projectiles.PushProjectile(xPos, yPos + 332, SPIDERSHOT2);
                PartikelSystem.PushPartikel(xPos - 30, yPos + 295, EXPLOSIONFLARE);
                PartikelSystem.PushPartikel(xPos, yPos + 320, SMOKEBIG);
                ShotDelay = 5.0f;
                VorneX = 5.0f;
                ShotCount--;

                if (ShotCount <= 0)
                    NeueAktion();
            }
        } break;

        // Elektropampe schiessen
        case GEGNER_SPECIAL3: {
            ShotDelay -= 1.0f SYNC;

            if (ShotDelay < 0.0f) {
                if (ShotCount > 1) {
                    SoundManager.PlayWave(128, 128, 11025, SOUND_LILA);
                    SoundManager.PlayWave(128, 128, 25050, SOUND_GOLEMLOAD);
                    Projectiles.PushProjectile(xPos + 220.0f, yPos + 210.0f, ELEKTROSCHUSS);
                }

                ShotDelay = 30.0f;
                ShotCount--;

                if (ShotCount <= 0)
                    NeueAktion();
            }
        } break;

        // Fetten Laser schiessen
        case GEGNER_BOMBARDIEREN: {
            ShotDelay -= 1.0f SYNC;

            if (ShotDelay < 0.0f) {
                if (ShotCount > 1) {
                    Projectiles.PushProjectile(static_cast<float>(TileEngine.XOffset) - 40.0f, yPos + 240.0f,
                                               SPIDERLASER);
                    SoundManager.PlayWave(100, 128, 11025, SOUND_BEAMLOAD2);
                }

                ShotDelay = 60.0f;
                ShotCount--;

                if (ShotCount <= 0)
                    NeueAktion();
            }
        } break;

        case GEGNER_STEHEN: {
            if (Value1 == 1)
                NeueAktion();
        } break;

        case GEGNER_OEFFNEN: {
            // Tür bewegen
            if (DoorOffset < PI / 2.0f)
                DoorOffset += 0.075f SYNC;

            // Tür offen? Dann countdown, bis sie wieder zugeht
            else {
                DoorOffset = PI / 2.0f;

                // Tür öffnen Sound anhalten
                // DKS - Added function WaveIsPlaying() to SoundManagerClass:
                if (SoundManager.WaveIsPlaying(SOUND_DOOR)) {
                    SoundManager.StopWave(SOUND_DOOR);
                    SoundManager.PlayWave(100, 128, 22000, SOUND_STEAM2);
                }

                AnimCount -= 1.0f SYNC;
                if (AnimCount < 0.0f) {
                    SoundManager.PlayWave(100, 128, 10000 + random(1000), SOUND_DOOR);
                    Handlung = GEGNER_SCHLIESSEN;
                }
            }

        } break;

        case GEGNER_SCHLIESSEN: {
            // Tür
            DoorOffset += 0.125f SYNC;

            if (DoorOffset > PI) {
                DoorOffset = 0.0f;
                ShakeScreen(2.0f);
                SoundManager.PlayWave(100, 128, 11000 + random(1500), SOUND_DOORSTOP);

                // Tür öffnen Sound anhalten
                SoundManager.StopWave(SOUND_DOOR);

                for (int i = 0; i < 36; i++)
                    PartikelSystem.PushPartikel(xPos + TunnelOffx + 110 + random(30),
                                                yPos + TunnelOffY + random(10) + i * 5 - 25, SMOKE);

                for (int i = 0; i < 15; i++)
                    PartikelSystem.PushPartikel(xPos + TunnelOffx + 95 + random(30),
                                                yPos + TunnelOffY + random(10) + i * 10 - 25, SMOKEBIG);

                Handlung = GEGNER_STEHEN;
                AnimCount = 20.0f;
                OldEnergy = Energy;
                SkullEnergy = 250.0f;

                // Den Schädel wieder zurückpfeiffen
                ReturnSkull();
            }
        } break;

        case GEGNER_EXPLODIEREN: {
            AnimCount -= 1.0f SYNC;

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
