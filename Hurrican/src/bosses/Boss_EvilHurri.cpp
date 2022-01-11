// --------------------------------------------------------------------------------------
// Der Böse Hurrican
//
// --------------------------------------------------------------------------------------

#include "Boss_EvilHurri.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerEvilHurri::GegnerEvilHurri(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::INIT;
    BlickRichtung = RECHTS;

    Energy = 6000;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimPhase = 3;
    AnimStart = 3;
    AnimEnde = 21;
    AnimSpeed = 0.4f;
    ChangeLight = Light;
    Destroyable = true;
    OwnDraw = true;
    BlinkDirection = 1.0f;
    BlinkWert = 0.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerEvilHurri::DoDraw() {
    for (int i = 0; i < 4; i++) {
        DirectGraphics.SetAdditiveMode();
        if (BlickRichtung == LINKS) {
            Player[0].PlayerRun.RenderSprite(xPos - TileEngine.XOffset,
                                             yPos - TileEngine.YOffset, AnimPhase, 0xAA444444,
                                             true);
            Player[0].PlayerRun.RenderSprite(xPos - TileEngine.XOffset,
                                             yPos - TileEngine.YOffset, AnimPhase, 0xFF0022FF,
                                             true);
        } else {
            Player[0].PlayerRun.RenderSprite(xPos - TileEngine.XOffset,
                                             yPos - TileEngine.YOffset, AnimPhase, 0xAA444444);

            Player[0].PlayerRun.RenderSprite(xPos - TileEngine.XOffset,
                                             yPos - TileEngine.YOffset, AnimPhase, 0xFF0022FF);
        }
    }
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerEvilHurri::DoKI() {
    // Animieren
    SimpleAnimation();

    // Energie anzeigen
    if (Handlung != GEGNER::INIT && Handlung != GEGNER::EXPLODIEREN)
        HUD.ShowBossHUD(6000, Energy);

    // Levelausschnitt auf den Boss zentrieren, sobald dieser sichtbar wird
    if (Active == true && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        TileEngine.ScrollLevel(static_cast<float>(Value1),
                               static_cast<float>(Value2),
                               TileStateEnum::SCROLLTOLOCK);  // Level auf die Faust zentrieren

        SoundManager.FadeSong(MUSIC::STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
    }

    // Zwischenboss blinkt nicht so lange wie die restlichen Gegner
    if (DamageTaken > 0.0f)
        DamageTaken -= Timer.sync(100.0f);  // Rotwerden langsam ausfaden lassen
    else
        DamageTaken = 0.0f;  // oder ganz anhalten

    // Testen, ob der Spieler den Boss berührt hat
    for (int p = 0; p < NUMPLAYERS; p++)
        if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], Player[p].xpos, Player[p].ypos, Player[p].CollideRect) ==
            true) {
            // Spieler als Rad ? Dann abprallen
            if (Player[p].Handlung == PlayerActionEnum::RADELN ||
                    Player[p].Handlung == PlayerActionEnum::RADELN_FALL) {
                if (Player[p].xpos < xPos)
                    Player[p].Blickrichtung = LINKS;
                if (Player[p].xpos > xPos)
                    Player[p].Blickrichtung = RECHTS;
            }

            // Sonst Energie abziehen
            else {
                Player[p].DamagePlayer(Timer.sync(10.0f));

                // Spieler wegschieben
                if (Player[p].xpos < xPos)
                    Player[p].xpos = xPos + GegnerRect[GegnerArt].left - Player[p].CollideRect.right;
                if (Player[p].xpos > xPos)
                    Player[p].xpos = xPos + GegnerRect[GegnerArt].right - Player[p].CollideRect.left;
            }
        }

    // Hat der Boss keine Energie mehr ? Dann fliegt er in die Luft
    if (Energy <= 100.0f && Handlung != GEGNER::EXPLODIEREN) {
        ShakeScreen(5);

        AnimPhase = 40;
        AnimEnde = 0;
        AnimCount = 0.0f;
        ySpeed = 0.0f;
        xSpeed = 0.0f;
        yAcc = 0.0f;
        xAcc = 0.0f;

        SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION2);

        Handlung = GEGNER::EXPLODIEREN;
        ActionDelay = 0.0f;

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC::BOSS, -2.0f, 0, false);
    }

    // EvilHurri blinken lassen
    BlinkWert += Timer.sync(BlinkDirection * 30.0f);

    if (BlinkWert <= 0)
        BlinkDirection = 1.0f;
    if (BlinkWert >= 128)
        BlinkDirection = -1.0f;

    // Rand checken
    if (xPos < static_cast<float>(Value1 + 10) && Handlung != GEGNER::EINFLIEGEN && Handlung != GEGNER::INIT)
        xPos = static_cast<float>(Value1 + 10);
    if (xPos > static_cast<float>(Value1 + 550) && Handlung != GEGNER::EINFLIEGEN && Handlung != GEGNER::INIT)
        xPos = static_cast<float>(Value1 + 550);

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::INIT:  // Warten bis der Screen zentriert wurde
        {
            if (TileEngine.Zustand == TileStateEnum::LOCKED) {
                // Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
                // DKS - Added function SongIsPlaying() to SoundManagerClass:
                if (!SoundManager.SongIsPlaying(MUSIC::BOSS))
                    SoundManager.PlaySong(MUSIC::BOSS, false);

                // Und Boss erscheinen lassen
                Handlung = GEGNER::EINFLIEGEN;
            }
        } break;

        case GEGNER::STEHEN:  // Counter runterzählen und nächste Aktion entscheiden
        {
            AnimPhase = 0;
            AnimStart = 0;
            AnimEnde = 0;
            xSpeed = 0.0f;
            ySpeed = 0.0f;

            if (xPos + 70.0f < pAim->xpos)
                BlickRichtung = RECHTS;
            if (xPos > pAim->xpos + 70.0f)
                BlickRichtung = LINKS;

            ActionDelay -= Timer.sync(1.0f);
            if (ActionDelay < 0.0f) {
                ActionDelay = 0.0f;

                // Bei großem Abstand spieler zerquetschen
                if (PlayerAbstand() > 300) {
                    if (random(2) == 0) {
                        Handlung = GEGNER::CRUSHEN;
                        ActionDelay = 1.0f;
                        AnimPhase = 48;

                        if (BlickRichtung == RECHTS)
                            xSpeed = 40.0f;
                        else
                            xSpeed = -40.0f;
                    } else {
                        Handlung = GEGNER::CRUSHEN2;
                        ActionDelay = 1.0f;
                        AnimPhase = 48;

                        if (BlickRichtung == RECHTS)
                            xSpeed = 40.0f;
                        else
                            xSpeed = -40.0f;
                    }

                }

                // Ansonsten im Kreis rum ballern
                else {
                    if (random(2) == 0 && xPos > Value1 + 200 && xPos < Value1 + 440) {
                        Handlung = GEGNER::BOMBARDIEREN;
                        ActionDelay = 3.0f;
                        AnimEnde = 0;
                        AnimStart = 0;
                        AnimPhase = 35;
                        BlickRichtung = RECHTS;
                    }

                    // oder ballernd auf den Spieler zurennen
                    else {
                        if (random(2) == 0) {
                            Handlung = GEGNER::LAUFEN;
                            ActionDelay = 1.0f;
                            AnimPhase = 3;
                            AnimStart = 3;
                            AnimEnde = FRAMES_RUN;
                            AnimSpeed = 0.4f;

                            if (BlickRichtung == RECHTS)
                                xSpeed = 20.0f;
                            else
                                xSpeed = -20.0f;
                        }

                        else {
                            Handlung = GEGNER::LAUFEN2;
                            ActionDelay = 1.0f;
                            AnimPhase = 3;
                            AnimStart = 3;
                            AnimEnde = 21;
                            AnimSpeed = 0.4f;

                            if (BlickRichtung == RECHTS)
                                xSpeed = 20.0f;
                            else
                                xSpeed = -20.0f;
                        }
                    }
                }
            }
        } break;

        case GEGNER::EINFLIEGEN:  // EvilHurri läuft ins Level
        {
            Energy = 6000;
            DamageTaken = 0.0f;

            xSpeed = +15.0f;

            if (xPos >= Value1 + 30)  // Weit genug eingelaufen ?
            {
                Handlung = GEGNER::STEHEN;
                ActionDelay = 10.0f;
            }
        } break;

        case GEGNER::CRUSHEN: {
            ActionDelay -= Timer.sync(1.0f);

            if (ActionDelay < 0.0f) {
                ActionDelay = 1.0f;

                if (BlickRichtung == RECHTS)
                    PartikelSystem.PushPartikel(xPos, yPos, EVILSMOKE2);
                else
                    PartikelSystem.PushPartikel(xPos, yPos, EVILSMOKE);
            }

            if ((BlickRichtung == RECHTS && xPos > pAim->xpos + 140) ||
                (BlickRichtung == LINKS && xPos + 140 < pAim->xpos)) {
                Handlung = GEGNER::STEHEN;
                ActionDelay = 10.0f;
            }

        } break;

        case GEGNER::CRUSHEN2:  // Nur bis zur Mitte des Screens und dann blitzen
        {
            ActionDelay -= Timer.sync(1.0f);

            if (ActionDelay < 0.0f) {
                ActionDelay = 1.0f;

                if (BlickRichtung == RECHTS)
                    PartikelSystem.PushPartikel(xPos, yPos, EVILSMOKE2);
                else
                    PartikelSystem.PushPartikel(xPos, yPos, EVILSMOKE);
            }

            if ((BlickRichtung == RECHTS && xPos + 35 > Value1 + 320) ||
                (BlickRichtung == LINKS && xPos + 35 < Value1 + 320)) {
                Handlung = GEGNER::SPECIAL;
                AnimPhase = 0;
                AnimEnde = 0;
                ActionDelay = 0.0f;
                xSpeed = 0.0f;

                SoundManager.PlayWave(100, 128, 11025, SOUND::BLITZSTART);
            }
        } break;

        case GEGNER::LAUFEN:  // Auf Spieler zulaufen
        {
            // Ballern
            ActionDelay -= Timer.sync(1.0f);
            if (ActionDelay < 0.0f) {
                ActionDelay = 2.5f;

                // PartikelSystem.PushPartikel(xPos+30, yPos+28, BULLET);

                SoundManager.PlayWave(100, random(255), 8000 + random(4000), SOUND::CANON);

                if (BlickRichtung == RECHTS) {
                    PartikelSystem.PushPartikel(xPos + 50.0f, yPos + 20.0f, SMOKE);
                    Projectiles.PushProjectile(xPos + 55.0f, yPos + 30.0f, EVILSHOT);
                } else {
                    PartikelSystem.PushPartikel(xPos + 10.0f, yPos + 20.0f, SMOKE);
                    Projectiles.PushProjectile(xPos, yPos + 30.0f, EVILSHOT2);
                }
            }

            // Spingen ?
            if (pAim->ypos + 10 < yPos && ySpeed == 0.0f && random(10) == 0) {
                ySpeed = -43.0f;
                yAcc = 8.0f;
                yPos -= 2.0f;
                AnimPhase = 48;
                AnimEnde = 55;
                AnimSpeed = 2.0f;
            }

            if (ySpeed != 0.0f) {
                // Gelandet ?
                if (blocku & BLOCKWERT_WAND) {
                    ySpeed = 0.0f;
                    yAcc = 0.0f;
                    AnimPhase = 3;
                    AnimStart = 3;
                    AnimEnde = 21;
                    AnimSpeed = 0.4f;

                    TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
                }
            }

            if (((BlickRichtung == RECHTS && xPos > pAim->xpos + 70) ||
                 (BlickRichtung == LINKS && xPos + 70 < pAim->xpos)) &&
                ySpeed == 0.0f) {
                Handlung = GEGNER::STEHEN;
                ActionDelay = 10.0f;
            }
        } break;

        case GEGNER::LAUFEN2:  // In die Mitte des Screens laufen und da Schweinereien machen
        {
            // Ballern
            ActionDelay -= Timer.sync(1.0f);
            if (ActionDelay < 0.0f) {
                ActionDelay = 2.5f;

                //				PartikelSystem.PushPartikel(xPos+30, yPos+28, BULLET);

                SoundManager.PlayWave(100, random(255), 8000 + random(4000), SOUND::CANON);

                if (BlickRichtung == RECHTS) {
                    PartikelSystem.PushPartikel(xPos + 50.0f, yPos + 20.0f, SMOKE);
                    Projectiles.PushProjectile(xPos + 55.0f, yPos + 30.0f, EVILSHOT);
                } else {
                    PartikelSystem.PushPartikel(xPos + 10.0f, yPos + 20.0f, SMOKE);
                    Projectiles.PushProjectile(xPos, yPos + 30.0f, EVILSHOT2);
                }
            }

            if ((BlickRichtung == RECHTS && xPos + 35 > Value1 + 320) ||
                (BlickRichtung == LINKS && xPos + 35 < Value1 + 320)) {
                Handlung = GEGNER::SPECIAL;
                AnimPhase = 0;
                AnimEnde = 0;
                ActionDelay = 0.0f;
                xSpeed = 0.0f;

                SoundManager.PlayWave(100, 128, 11025, SOUND::BLITZSTART);
            }
        } break;

        // Boss ballert im Kreis um sich herum
        case GEGNER::BOMBARDIEREN: {
            ActionDelay -= Timer.sync(1.0f);

            if (ActionDelay < 0.0f) {
                SoundManager.PlayWave(100, random(255), 8000 + random(4000), SOUND::CANON);
                PartikelSystem.PushPartikel(xPos + static_cast<float>((AnimPhase - 36) * 5),
                                            yPos - 23.0f + static_cast<float>(abs(AnimPhase - 41) * 8), SMOKE);
                //				PartikelSystem.PushPartikel(xPos+30, yPos+28, BULLET);

                Projectiles.PushProjectile(xPos + 5.0f + static_cast<float>((AnimPhase - 36) * 5),
                                           yPos - 10.0f + static_cast<float>(abs(AnimPhase - 41) * 8),
                                           EVILROUND1 + (AnimPhase - 35));

                ActionDelay = 3.0f;
                AnimPhase++;

                if (AnimPhase > 46) {
                    Handlung = GEGNER::STEHEN;
                    AnimPhase = 0;
                    ActionDelay = 10.0f;
                }
            }
        } break;

        // Gegner Blitzt in den Himmel und Schüsse fallen von oben
        case GEGNER::SPECIAL: {
            ActionDelay += Timer.sync(1.0f);

            if (ActionDelay > 5.0f && ActionDelay < 20.0f && AnimPhase == 0)
                AnimPhase = 40;

            if (ActionDelay > 25.0f && AnimPhase != 0) {
                AnimPhase = 0;

                for (int i = 0; i < 30; i++)
                    PartikelSystem.PushPartikel(xPos + 30.0f + static_cast<float>(random(10)),
                                                yPos + static_cast<float>(random(10)), LASERFUNKE2);

                Projectiles.PushProjectile(xPos, yPos - 20, EVILBLITZ);
                SoundManager.PlayWave(100, 128, 11025, SOUND::SPIDERGRENADE);
            }

            // Screen blinken
            if (ActionDelay > 25.0f && ActionDelay < 35.0f) {
                int w = 128 - static_cast<int>(ActionDelay - 25.0f) * 12;

                RenderRect(0, 0, 640, 480, D3DCOLOR_RGBA(255, 255, 128, w));
            }

            if (ActionDelay > 70.0f) {
                if (random(2) == 0) {
                    Handlung = GEGNER::AUFRICHTEN;
                    BlickRichtung = LINKS;
                    AnimPhase = 3;
                    AnimStart = 3;
                    AnimEnde = 21;
                    xSpeed = -20;
                } else {
                    Handlung = GEGNER::AUFRICHTENZWEI;
                    BlickRichtung = RECHTS;
                    AnimPhase = 3;
                    AnimStart = 3;
                    AnimEnde = 21;
                    xSpeed = 20;
                }
            }
        } break;

        case GEGNER::AUFRICHTEN: {
            if (xPos < Value1 + 20) {
                BlickRichtung = RECHTS;
                xSpeed = 15.0f;
                ActionDelay = 4.0f;
                Handlung = GEGNER::AUSSPUCKEN;
            }
        } break;

        case GEGNER::AUFRICHTENZWEI: {
            if (xPos >= Value1 + 550) {
                BlickRichtung = LINKS;
                xSpeed = -15.0f;
                ActionDelay = 4.0f;
                Handlung = GEGNER::AUSSPUCKENZWEI;
            }
        } break;

        // Über den Screen rennen und dabei nach oben ballern
        case GEGNER::AUSSPUCKEN: {
            // Am Rand angekommen ?
            if (xPos >= Value1 + 550) {
                xSpeed = 0;
                AnimPhase = 0;
                ActionDelay = 10.0f;
                Handlung = GEGNER::STEHEN;
            }

            ActionDelay -= Timer.sync(1.0f);

            if (ActionDelay < 0.0f) {
                // schiessen ?
                if (AnimPhase != 40) {
                    xSpeed = 0.0f;
                    AnimEnde = 0;
                    AnimPhase = 40;
                    Projectiles.PushProjectile(xPos + 20.0f, yPos + 10.0f, ARCSHOT);
                    Projectiles.PushProjectile(xPos + 20.0f, yPos + 10.0f, ARCSHOTLEFT);
                    Projectiles.PushProjectile(xPos + 20.0f, yPos + 10.0f, ARCSHOTRIGHT);
                    SoundManager.PlayWave(100, 128, 11025, SOUND::SPIDERGRENADE);

                    ActionDelay = 3.0f;
                }
                // oder weiterlaufen
                else {
                    xSpeed = 15.0f;
                    AnimPhase = 3;
                    AnimStart = 3;
                    AnimEnde = 21;
                    ActionDelay = 7.0f;
                }
            }
        } break;

        // Über den Screen rennen und dabei nach oben ballern
        case GEGNER::AUSSPUCKENZWEI: {
            // Am Rand angekommen ?
            if (xPos <= Value1 + 20) {
                xSpeed = 0;
                AnimPhase = 0;
                ActionDelay = 10.0f;
                Handlung = GEGNER::STEHEN;
            }

            ActionDelay -= Timer.sync(1.0f);

            if (ActionDelay < 0.0f) {
                // schiessen ?
                if (AnimPhase != 40) {
                    xSpeed = 0.0f;
                    AnimEnde = 0;
                    AnimPhase = 40;
                    Projectiles.PushProjectile(xPos, yPos + 10.0f, ROCKETSPIDER);
                    SoundManager.PlayWave(100, 128, 11025, SOUND::ROCKET);

                    ActionDelay = 4.0f;
                }
                // oder weiterlaufen
                else {
                    xSpeed = -15.0f;
                    AnimPhase = 3;
                    AnimStart = 3;
                    AnimEnde = 21;
                    ActionDelay = 10.0f;
                }
            }
        } break;

        case GEGNER::EXPLODIEREN: {
            Energy = 80.0f;

            ActionDelay += Timer.sync(1.0f);
            if (ActionDelay >= 50.0f)
                Energy = 0.0f;

            AnimCount -= Timer.sync(1.0f);
            if (AnimCount < 0.0f) {
                AnimCount = 2.0f;

                SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);
                PartikelSystem.PushPartikel(xPos - 20.0f + static_cast<float>(random(70)),
                                            yPos + static_cast<float>(random(80)), EXPLOSION_MEDIUM);
                PartikelSystem.PushPartikel(xPos - 20.0f + static_cast<float>(random(70)),
                                            yPos + static_cast<float>(random(80)), EXPLOSION_MEDIUM2);
                PartikelSystem.PushPartikel(xPos - 50.0f + static_cast<float>(random(70)),
                                            yPos - 30.0f + static_cast<float>(random(80)), EXPLOSION_BIG);
            }
        } break;

        default:
            break;
    }  // switch
}

// --------------------------------------------------------------------------------------
// EvilHurri explodiert
// --------------------------------------------------------------------------------------

void GegnerEvilHurri::GegnerExplode() {
    Player[0].Score += 9000;

    for (int i = 0; i < 10; i++)
        PartikelSystem.PushPartikel(xPos - 20.0f + static_cast<float>(random(70)),
                                    yPos + static_cast<float>(random(80)), SPLITTER);

    for (int i = 0; i < 10; i++)
        PartikelSystem.PushPartikel(xPos - 20.0f + static_cast<float>(random(70)),
                                    yPos + static_cast<float>(random(80)), EXPLOSION_MEDIUM2);

    SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION2);

    ScrolltoPlayeAfterBoss();

    HUD.BossHUDActive = false;
}
