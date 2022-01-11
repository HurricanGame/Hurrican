// --------------------------------------------------------------------------------------
// Der Fahrstuhl Boss
//
// kommt von oben reingefahren und ballert mit zwei beweglichen Kanonen und einem
// riesen fetten Laser, der auf einer Schiene läuft
// --------------------------------------------------------------------------------------

#include "Boss_FahrstuhlBoss.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFahrstuhlBoss::GegnerFahrstuhlBoss(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::NOTVISIBLE;
    BlickRichtung = LINKS;
    Energy = 6000;
    ChangeLight = Light;
    Destroyable = true;
    ShotDelay = 5.0f;
    Activated = false;
    OwnDraw = true;

    dx1 = 0.0f;
    dx2 = 0.0f;

    x1 = 140;
    y1 = 160;
    x2 = 25;
    y2 = 80;
    x3 = 340;

    CanonAnim = 0.0f;

    TempY = 800.0f;
    yPos = 0.0f;

    // Zusätzliche Grafiken für die Kanone und den Kopf laden
    Head.LoadImage("fahrstuhlboss_head.png", 140, 186, 140, 186, 1, 1);
    Canon.LoadImage("fahrstuhlboss_canon.png", 177, 240, 59, 120, 3, 2);
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerFahrstuhlBoss::DoDraw() {
    // Gegner mit Kopf und Kanonen rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset, yPos - TileEngine.YOffset, 0,
                                           0xFFFFFFFF);

    Head.RenderSprite(xPos + x1 - TileEngine.XOffset,
                      yPos + y1 - TileEngine.YOffset, 0xFFFFFFFF);
    Canon.RenderSprite(xPos + x2 - TileEngine.XOffset,
                       yPos + y2 - TileEngine.YOffset, static_cast<int>(CanonAnim),
                       0xFFFFFFFF);
    Canon.RenderSprite(xPos + x3 - TileEngine.XOffset,
                       yPos + y2 - TileEngine.YOffset, 5 - static_cast<int>(CanonAnim),
                       0xFFFFFFFF);

    GegnerRect[FAHRSTUHLBOSS].left = static_cast<int>(x1 + 60.0f);
    GegnerRect[FAHRSTUHLBOSS].right = static_cast<int>(x1 + 60.0f + 25.0f);
    // GegnerRect[FAHRSTUHLBOSS].left   = int (140 + 76);
    // GegnerRect[FAHRSTUHLBOSS].right  = int (140 + 76 + 34);
    GegnerRect[FAHRSTUHLBOSS].top = static_cast<int>(y1 + 46.0f);
    GegnerRect[FAHRSTUHLBOSS].bottom = static_cast<int>(y1 + 46.0f + 34.0f);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFahrstuhlBoss::DoKI() {
    if (x1 < 0)
        x1 = 0;

    // Energie anzeigen
    if (Handlung != GEGNER::NOTVISIBLE && Handlung != GEGNER::EXPLODIEREN)
        HUD.ShowBossHUD(6000, Energy);

    // Boss aktivieren und Mucke laufen lassen
    if (Active == true && Activated == false) {
        SoundManager.FadeSong(MUSIC::STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
        Activated = true;
        Value1 = static_cast<int>(TileEngine.YOffset + 100.0f);
    }

    // Zwischenboss blinkt nicht so lange wie die restlichen Gegner
    if (DamageTaken > 0.0f)
        DamageTaken -= Timer.sync(50.0f);  // Rotwerden langsam ausfaden lassen
    else
        DamageTaken = 0.0f;  // oder ganz anhalten

    // Hat der Boss keine Energie mehr ? Dann explodiert er
    if (Energy <= 100.0f && Handlung != GEGNER::EXPLODIEREN) {
        Handlung = GEGNER::EXPLODIEREN;
        ShotDelay = 1.0f;
        g_Fahrstuhl_Speed = -1.0f;
        dx1 = 0.0f;
        dx2 = 0.0f;

        // Level wieder auf Spieler zentrieren
        //
        TileEngine.ScrollLevel(Player[0].xpos - 300, Player[0].ypos - 280, TileStateEnum::SCROLLTOPLAYER, 10.0f, 50.0f);

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC::BOSS, -2.0f, 0, false);
    }

    static float smokecount = 0.0f;

    // Füße an der Seite rauchen lassen
    //
    smokecount -= Timer.sync(1.0f);
    if (smokecount < 0.0f) {
        smokecount = 1.0f;

        PartikelSystem.PushPartikel(xPos- 15.0f + static_cast<float>(random(10)), yPos, SMOKEBIG);
        PartikelSystem.PushPartikel(xPos- 15.0f + static_cast<float>(random(10)), yPos + 210.0f, SMOKEBIG);

        PartikelSystem.PushPartikel(xPos + 375.0f + static_cast<float>(random(10)), yPos, SMOKEBIG);
        PartikelSystem.PushPartikel(xPos + 375.0f + static_cast<float>(random(10)), yPos + 210.0f, SMOKEBIG);

        // Wunden Punkt rauchen lassen bei wenig Energy
        if (Energy <= 1500.0f) {
            PartikelSystem.PushPartikel(xPos + x1 + 35.0f + static_cast<float>(random(20)),
                                        yPos + y1 + static_cast<float>(random(15)), SMOKEBIG);

            if (random(10) == 0) {
                PartikelSystem.PushPartikel(xPos + x1 + 50.0f + static_cast<float>(random(20)),
                                            yPos + y1 + 35.0f + static_cast<float>(random(15)), LASERFLAME);
            }
        }
    }

    // Kanonen bewegen
    x1 += Timer.sync(dx1);
    x2 += Timer.sync(dx2);
    x3 -= Timer.sync(dx2);

    CanonAnim += Timer.sync(dx2);
    if (CanonAnim >= 6.0f)
        CanonAnim = 0.0f;
    if (CanonAnim < 0.0f)
        CanonAnim = 6.0f;

    if (x2 > 340.0f) {
        x2 = 25.0f;
        x3 = 340.0f;
    }

    // Riesen Wumme begradigen
    if (Handlung == GEGNER::EINFLIEGEN || Handlung == GEGNER::ABSENKEN || Handlung == GEGNER::AUFRICHTEN) {
        if ((dx1 < 0.0f && x1 < 140.0f) || (dx1 > 0.0f && x1 > 140.0f)) {
            x1 = 140.0f;
            dx1 = 0.0f;
        }
    }

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::NOTVISIBLE:  // Boss einfliegen lassen
        {
            // Warten bis die Level Mucke ausgefadet ist
            // DKS - Overhauled SoundManagerClass:
            // if (SoundManager.its_Songs[MUSIC::STAGEMUSIC]->Volume ==
            // SoundManager.its_Songs[MUSIC::STAGEMUSIC]->FadingEnd)
            //{
            //    // Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
            //    if (MUSIC::IsPlaying(SoundManager.its_Songs[MUSIC::BOSS]->SongData) == false)
            //        SoundManager.PlaySong(MUSIC::BOSS, false);

            //    Handlung = GEGNER::EINFLIEGEN2;				// Und Boss erscheinen lassen
            //}
            if (SoundManager.SongIsPaused(MUSIC::STAGEMUSIC)) {
                // Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
                if (!SoundManager.SongIsPlaying(MUSIC::BOSS))
                    SoundManager.PlaySong(MUSIC::BOSS, false);

                Handlung = GEGNER::EINFLIEGEN2;  // Und Boss erscheinen lassen
            }
        } break;

        case GEGNER::EINFLIEGEN2:  // Gegner kommt in den Screen geflogen am Anfang (einmalig)
        {
            Energy = 6000;
            DamageTaken = 0.0f;

            TempY -= Timer.sync(5.0f);

            // Gegner an der richtigen Position ?
            if (TempY < 500.0f) {
                Handlung = GEGNER::ABSENKEN;
                TempY = 500.0f;
                dx2 = 5.0f;
            }

            // Levelausschnitt weiter nach unten scrollen damit man mehr vom Boss sieht
            if (g_Fahrstuhl_Offset < 150.0f)
                g_Fahrstuhl_Offset += Timer.sync(4.0f);
            else
                g_Fahrstuhl_Offset = 150.0f;
        } break;

        case GEGNER::EINFLIEGEN:  // Gegner kommt in den Screen geflogen
        {
            TempY -= Timer.sync(5.0f);

            // Gegner an der richtigen Position ?
            if (TempY < 500.0f) {
                int j = random(6);
                switch (j) {
                    case 0: {
                        Handlung = GEGNER::ABSENKEN;
                        TempY = 500.0f;
                        dx2 = 0.0f;
                    } break;
                    case 1:
                    case 2: {
                        Handlung = GEGNER::ABSENKEN;
                        TempY = 500.0f;
                        dx2 = 5.0f;
                    } break;
                    case 3: {
                        Handlung = GEGNER::AUSSPUCKEN;
                        TempY = 500.0f;
                        Shots = 1 + random(2);
                    } break;
                    case 4:
                    case 5: {
                        Handlung = GEGNER::CRUSHEN;
                        TempY = 500.0f;
                        Shots = 10 + random(20);
                        ShotDelay = 2.0f;
                    } break;
                }
            }

            // Levelausschnitt weiter nach unten scrollen damit man mehr vom Boss sieht
            if (g_Fahrstuhl_Offset < 150.0f)
                g_Fahrstuhl_Offset += Timer.sync(4.0f);
            else
                g_Fahrstuhl_Offset = 150.0f;
        } break;

        case GEGNER::LAUFEN:  // yPos über dem Fahrstuhl
        {
        } break;

        case GEGNER::ABSENKEN:  // Gegner kommt nach unten
        {
            TempY -= Timer.sync(5.0f);

            // Gegner an der richtigen Position ?
            if (TempY < 380.0f) {
                if (dx2 != 0.0f) {
                    Handlung = GEGNER::SCHIESSEN;
                    TempY = 380.0f;
                    ShotDelay = 1.0f;
                    Shots = random(10) + 10;
                } else {
                    Handlung = GEGNER::AUSSPUCKEN;
                    TempY = 380.0f;
                    ShotDelay = 5.0f;
                    Shots = 2 + random(2);
                }
            }
        } break;

        case GEGNER::AUFRICHTEN:  // Gegner fährt wieder hoch
        {
            TempY += Timer.sync(5.0f);

            // Gegner an der richtigen Position ?
            if (TempY > 600.0f) {
                int j = random(3);
                switch (j) {
                    case 0: {
                        Handlung = GEGNER::BOMBARDIEREN;
                        dx1 = 40.0f;
                        TempY = 600.0f;
                        ShotDelay = 1.0f;
                        Shots = random(8) + 5;
                    } break;
                    case 1: {
                        Handlung = GEGNER::SPECIAL;
                        dx1 = 0.0f;
                        TempY = 600.0f;
                        ShotDelay = 5.0f;
                        Shots = random(8) + 5;
                    } break;
                    case 2: {
                        Handlung = GEGNER::AUSSPUCKEN;
                        dx1 = 0.0f;
                        TempY = 600.0f;
                        ShotDelay = 5.0f;
                        Shots = random(4) + 2;
                    } break;
                }
            }
        } break;

        case GEGNER::SCHIESSEN:  // Mit zwei lasern ballern
        {
            ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay < 0.0f) {
                ShotDelay = 6.0f;

                SoundManager.PlayWave(100, 128, 44100, SOUND::LASERSHOT);

                Projectiles.PushProjectile(xPos + x2 + 28.0f,
                                           yPos + y2 + 95.0f, KRABBLERLASER1);
                PartikelSystem.PushPartikel(xPos + x2 + 28.0f - 25.0f,
                                            yPos + y2 + 95.0f - 4.0f, LASERFLAME);

                Projectiles.PushProjectile(xPos + x3 + 28.0f,
                                           yPos + y2 + 95.0f, KRABBLERLASER1);
                PartikelSystem.PushPartikel(xPos + x3 + 28.0f - 25.0f,
                                            yPos + y2 + 95.0f - 4.0f, LASERFLAME);

                y2 = 75.0f;

                // Genug geschossen ?
                Shots--;
                if (Shots <= 0) {
                    dx2 = 0.0f;
                    Handlung = GEGNER::AUFRICHTEN;
                }
            }
        } break;

        case GEGNER::BOMBARDIEREN:  // Steine bröckeln lassen
        {
            // An der Seite angekommen ? Dann ruckeln lassen und Steine werfen
            if ((x1 > 280 && dx1 > 0.0f) || (x1 < 0 && dx1 < 0.0f)) {
                ShakeScreen(3);
                dx1 *= -1.0f;

                Gegner.PushGegner(xPos + 10.0f + static_cast<float>(random(360)),
                                  yPos + 480.0f, BOULDER, 40 + random(20), 0, false);

                // aufhören damit ?
                Shots--;
                if (Shots <= 0) {
                    Handlung = GEGNER::EINFLIEGEN;

                    if (x1 < 140.0f)
                        dx1 = 10.0f;
                    else
                        dx1 = -10.0f;
                }
            }
        } break;

        case GEGNER::SPECIAL:  // Hurri anvisieren und Laser ballern
        {
            if (ShotDelay > 0.0f)
                ShotDelay -= Timer.sync(1.0f);

            // Bewegen
            if (pAim->xpos + 35.0f < xPos + x1 + 60.0f)
                x1 -= Timer.sync(8.0f);
            if (pAim->xpos + 35.0f > xPos + x1 + 80.0f)
                x1 += Timer.sync(8.0f);

            // Schiessen
            if (pAim->xpos + 35.0f < xPos + x1 + 100.0f && pAim->xpos + 35.0f > xPos + x1 + 40.0f && ShotDelay <= 0.0f) {
                ShotDelay = 8.0f;

                SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND::LASERSHOT);

                Projectiles.PushProjectile(xPos + x1 + 80.0f - 28.0f,
                                           yPos + y1 + 155.0f, UFOLASER);
                PartikelSystem.PushPartikel(xPos + x1 + 80.0f - 90.0f,
                                            yPos + y1 + 110.0f, UFOLASERFLARE);

                y1 = 155.0f;

                // Aufhören zu ballern
                Shots--;
                if (Shots <= 0) {
                    Handlung = GEGNER::EINFLIEGEN;

                    if (x1 < 140.0f)
                        dx1 = 10.0f;
                    else
                        dx1 = -10.0f;
                }
            }
        } break;

        case GEGNER::AUSSPUCKEN:  // Stelzis herbeirufen =)
        {
            ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay < 0.0f) {
                ShotDelay = 35.0f;

                Gegner.PushGegner(xPos + 10.0f + static_cast<float>(random(360)),
                                  yPos + 480.0f, STELZSACK, 80, 0, false);

                // Genug losgelassen ?
                Shots--;
                if (Shots <= 0) {
                    dx2 = 0.0f;
                    if (TempY != 380.0f)
                        Handlung = GEGNER::EINFLIEGEN;
                    else
                        Handlung = GEGNER::AUFRICHTEN;

                    if (x1 < 140.0f)
                        dx1 = 10.0f;
                    else
                        dx1 = -10.0f;
                }
            }

        } break;

        case GEGNER::CRUSHEN:  // Suchschüsse ballern
        {
            ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay < 0.0f) {
                ShotDelay = 5.0f;

                SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::CANON);

                Projectiles.PushProjectile(xPos + x2 + 28.0f,
                                           yPos + y2 + 95.0f + 5.0f, SUCHSCHUSS);
                PartikelSystem.PushPartikel(xPos + x2 + 28.0f - 10.0f,
                                            yPos + y2 + 95.0f, SMOKE);

                Projectiles.PushProjectile(xPos + x3 + 28.0f,
                                           yPos + y2 + 95.0f + 5.0f, SUCHSCHUSS);
                PartikelSystem.PushPartikel(xPos + x3 + 28.0f - 10.0f,
                                            yPos + y2 + 95.0f, SMOKE);

                y2 = 77.0f;

                // Genug losgelassen ?
                Shots--;
                if (Shots <= 0) {
                    dx2 = 5.0f;
                    Handlung = GEGNER::ABSENKEN;

                    if (x1 < 140.0f)
                        dx1 = 10.0f;
                    else
                        dx1 = -10.0f;
                }
            }

        } break;

        case GEGNER::FALLEN: {
        } break;

        // Gegner fliegt in die Luft
        //
        case GEGNER::EXPLODIEREN: {
            ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay < 0.0f) {
                ShotDelay = 0.5f;
                SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);

                PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(400)),
                                            yPos + static_cast<float>(random(280)), EXPLOSION_MEDIUM2);
                PartikelSystem.PushPartikel(xPos - 50.0f + static_cast<float>(random(400)),
                                            yPos + static_cast<float>(random(280)), EXPLOSION_BIG);

                if (random(2) == 0)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(400)),
                                                yPos + static_cast<float>(random(280)), SPLITTER);
            }

            Energy = 100.0f;

            // Levelausschnitt wieder weiter nach oben Scrollen
            if (g_Fahrstuhl_Offset > 20.0f)
                g_Fahrstuhl_Offset -= Timer.sync(4.0f);
            else
                g_Fahrstuhl_Offset = 20.0f;

            TempY -= Timer.sync(10.0f);

            // Gegner aus dem Screen, dann fetzen
            if (TempY < -400.0f)
                Energy = 0.0f;

        } break;

        default:
            break;
    }  // switch

    // yPosition über dem Fahrstuhl setzen
    yPos = g_Fahrstuhl_yPos - TempY;

    if (yPos < 0.0f)
        yPos = 0.0f;

    // Anhängsel am Fahrstuhl richtig positionieren
    if (y2 < 80.0f)
        y2 += Timer.sync(2.0f);
    else
        y2 = 80.0f;
    if (y1 < 160.0f)
        y1 += Timer.sync(2.0f);
    else
        y1 = 160.0f;

    // Schüsse und Partikel jeweils anpassen, damit sie gerade fliegen und nicht nach oben
    // abdriften wegen der FahrstuhlGeschwindigkeit

    for (int i = 0; i < NUMPLAYERS; i++)
        if (Player[i].AufPlattform == nullptr) {
            Player[i].ypos += Timer.sync(40.0f);
            Player[i].yposold += Timer.sync(40.0f);
            Player[i].JumpStart += Timer.sync(40.0f);
        }

    PartikelClass *pTemp = PartikelSystem.pStart;  // Zeiger auf den ersten Partikel
    while (pTemp != nullptr)                          // Ende der Liste erreicht ?
    {
        if (pTemp->PartikelArt != FUNKE && pTemp->PartikelArt != BULLET && pTemp->PartikelArt != SMOKE) {
            pTemp->yPos += Timer.sync(40.0f);  // Nach unten bewegen
            pTemp->yPosOld += Timer.sync(40.0f);
        }
        pTemp = pTemp->pNext;  // Zeiger auf das nächste Element
    }

    ProjectileClass *pTemp2 = Projectiles.pStart;  // Zeiger auf den ersten Schuss
    while (pTemp2 != nullptr)                         // Ende der Liste erreicht ?
    {
        if (pTemp2->ShotArt != STELZLASER) {
            pTemp2->yPos += Timer.sync(40.0f);  // Nach unten bewegen
            pTemp2->yPosOld += Timer.sync(40.0f);
        }
        pTemp2 = pTemp2->pNext;  // Zeiger auf das nächste Element
    }

    // Level-Hintergrund wiederholen und alle Objekte wieder nach oben setzen
    constexpr int A = 2 * 24 * 20;
    if (yPos > Value1 + A) {
        TileEngine.YOffset -= A;

        for (int p = 0; p < NUMPLAYERS; p++)
            Player[p].ypos -= A;

        GegnerClass *pTemp3 = Gegner.pStart;  // Zeiger auf den ersten Gegner
        while (pTemp3 != nullptr)                // Ende der Liste erreicht ?
        {
            if (pTemp3->GegnerArt >= SPITTER && pTemp3->GegnerArt <= FAHRSTUHL) {
                pTemp3->yPos -= A;  // Nach oben bewegen
                pTemp3->yPosOld -= A;
            }

            pTemp3 = pTemp3->pNext;  // Zeiger auf das nächste Element
        }

        ProjectileClass *pTemp4 = Projectiles.pStart;  // Zeiger auf den ersten Schuss
        while (pTemp4 != nullptr)                         // Ende der Liste erreicht ?
        {
            pTemp4->yPos -= A;  // Nach oben bewegen
            pTemp4->yPosOld -= A;
            pTemp4 = pTemp4->pNext;  // Zeiger auf das nächste Element
        }

        PartikelClass *pTemp5 = PartikelSystem.pStart;  // Zeiger auf den ersten Partikel
        while (pTemp5 != nullptr)                          // Ende der Liste erreicht ?
        {
            pTemp5->yPos -= A;  // Nach oben bewegen
            pTemp5->yPosOld -= A;
            pTemp5 = pTemp5->pNext;  // Zeiger auf das nächste Element
        }
    }
}

// --------------------------------------------------------------------------------------
// FahrstuhlBoss explodiert
// --------------------------------------------------------------------------------------

void GegnerFahrstuhlBoss::GegnerExplode() {
    ShakeScreen(10);
    SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION2);
    Player[0].Score += 9000;

    // Level Musik wieder einfaden lassen (aus Pause Zustand)
    SoundManager.FadeSong(MUSIC::STAGEMUSIC, 2.0f, 100, true);

    // TileEngine.YOffset = float (Value1);
}
