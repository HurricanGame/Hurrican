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
    Handlung = GEGNER_INIT;
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
            Player[0].PlayerRun.RenderSprite(static_cast<float>(xPos - TileEngine.XOffset),
                                             static_cast<float>(yPos - TileEngine.YOffset), AnimPhase, 0xAA444444,
                                             true);
            Player[0].PlayerRun.RenderSprite(static_cast<float>(xPos - TileEngine.XOffset),
                                             static_cast<float>(yPos - TileEngine.YOffset), AnimPhase, 0xFF0022FF,
                                             true);
        } else {
            Player[0].PlayerRun.RenderSprite(static_cast<float>(xPos - TileEngine.XOffset),
                                             static_cast<float>(yPos - TileEngine.YOffset), AnimPhase, 0xAA444444);

            Player[0].PlayerRun.RenderSprite(static_cast<float>(xPos - TileEngine.XOffset),
                                             static_cast<float>(yPos - TileEngine.YOffset), AnimPhase, 0xFF0022FF);
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
    if (Handlung != GEGNER_INIT && Handlung != GEGNER_EXPLODIEREN)
        HUD.ShowBossHUD(6000, Energy);

    // Levelausschnitt auf den Boss zentrieren, sobald dieser sichtbar wird
    if (Active == true && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        TileEngine.ScrollLevel(static_cast<float>(Value1), static_cast<float>(Value2),
                               TileStateEnum::SCROLLTOLOCK);  // Level auf die Faust zentrieren

        SoundManager.FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
    }

    // Zwischenboss blinkt nicht so lange wie die restlichen Gegner
    if (DamageTaken > 0.0f)
        DamageTaken -= 100 SYNC;  // Rotwerden langsam ausfaden lassen
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
                Player[p].DamagePlayer(static_cast<float>(10.0 SYNC));

                // Spieler wegschieben
                if (Player[p].xpos < xPos)
                    Player[p].xpos = xPos + GegnerRect[GegnerArt].left - Player[p].CollideRect.right;
                if (Player[p].xpos > xPos)
                    Player[p].xpos = xPos + GegnerRect[GegnerArt].right - Player[p].CollideRect.left;
            }
        }

    // Hat der Boss keine Energie mehr ? Dann fliegt er in die Luft
    if (Energy <= 100.0f && Handlung != GEGNER_EXPLODIEREN) {
        ShakeScreen(5);

        AnimPhase = 40;
        AnimEnde = 0;
        AnimCount = 0.0f;
        ySpeed = 0.0f;
        xSpeed = 0.0f;
        yAcc = 0.0f;
        xAcc = 0.0f;

        SoundManager.PlayWave(100, 128, 11025, SOUND_EXPLOSION2);

        Handlung = GEGNER_EXPLODIEREN;
        ActionDelay = 0.0f;

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC_BOSS, -2.0f, 0, false);
    }

    // EvilHurri blinken lassen
    BlinkWert += BlinkDirection * 30.0f SYNC;

    if (BlinkWert <= 0)
        BlinkDirection = 1.0f;
    if (BlinkWert >= 128)
        BlinkDirection = -1.0f;

    // Rand checken
    if (xPos < static_cast<float>(Value1 + 10) && Handlung != GEGNER_EINFLIEGEN && Handlung != GEGNER_INIT)
        xPos = static_cast<float>(Value1 + 10);
    if (xPos > static_cast<float>(Value1 + 550) && Handlung != GEGNER_EINFLIEGEN && Handlung != GEGNER_INIT)
        xPos = static_cast<float>(Value1 + 550);

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER_INIT:  // Warten bis der Screen zentriert wurde
        {
            if (TileEngine.Zustand == TileStateEnum::LOCKED) {
                // Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
                // DKS - Added function SongIsPlaying() to SoundManagerClass:
                if (!SoundManager.SongIsPlaying(MUSIC_BOSS))
                    SoundManager.PlaySong(MUSIC_BOSS, false);

                // Und Boss erscheinen lassen
                Handlung = GEGNER_EINFLIEGEN;
            }
        } break;

        case GEGNER_STEHEN:  // Counter runterzählen und nächste Aktion entscheiden
        {
            AnimPhase = 0;
            AnimStart = 0;
            AnimEnde = 0;
            xSpeed = 0.0f;
            ySpeed = 0.0f;

            if (xPos + 70 < pAim->xpos)
                BlickRichtung = RECHTS;
            if (xPos > pAim->xpos + 70)
                BlickRichtung = LINKS;

            ActionDelay -= 1.0f SYNC;
            if (ActionDelay < 0.0f) {
                ActionDelay = 0.0f;

                // Bei großem Abstand spieler zerquetschen
                if (PlayerAbstand() > 300) {
                    if (random(2) == 0) {
                        Handlung = GEGNER_CRUSHEN;
                        ActionDelay = 1.0f;
                        AnimPhase = 48;

                        if (BlickRichtung == RECHTS)
                            xSpeed = 40.0f;
                        else
                            xSpeed = -40.0f;
                    } else {
                        Handlung = GEGNER_CRUSHEN2;
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
                        Handlung = GEGNER_BOMBARDIEREN;
                        ActionDelay = 3.0f;
                        AnimEnde = 0;
                        AnimStart = 0;
                        AnimPhase = 35;
                        BlickRichtung = RECHTS;
                    }

                    // oder ballernd auf den Spieler zurennen
                    else {
                        if (random(2) == 0) {
                            Handlung = GEGNER_LAUFEN;
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
                            Handlung = GEGNER_LAUFEN2;
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

        case GEGNER_EINFLIEGEN:  // EvilHurri läuft ins Level
        {
            Energy = 6000;
            DamageTaken = 0.0f;

            xSpeed = +15.0f;

            if (xPos >= Value1 + 30)  // Weit genug eingelaufen ?
            {
                Handlung = GEGNER_STEHEN;
                ActionDelay = 10.0f;
            }
        } break;

        case GEGNER_CRUSHEN: {
            ActionDelay -= 1.0f SYNC;

            if (ActionDelay < 0.0f) {
                ActionDelay = 1.0f;

                if (BlickRichtung == RECHTS)
                    PartikelSystem.PushPartikel(xPos, yPos, EVILSMOKE2);
                else
                    PartikelSystem.PushPartikel(xPos, yPos, EVILSMOKE);
            }

            if ((BlickRichtung == RECHTS && xPos > pAim->xpos + 140) ||
                (BlickRichtung == LINKS && xPos + 140 < pAim->xpos)) {
                Handlung = GEGNER_STEHEN;
                ActionDelay = 10.0f;
            }

        } break;

        case GEGNER_CRUSHEN2:  // Nur bis zur Mitte des Screens und dann blitzen
        {
            ActionDelay -= 1.0f SYNC;

            if (ActionDelay < 0.0f) {
                ActionDelay = 1.0f;

                if (BlickRichtung == RECHTS)
                    PartikelSystem.PushPartikel(xPos, yPos, EVILSMOKE2);
                else
                    PartikelSystem.PushPartikel(xPos, yPos, EVILSMOKE);
            }

            if ((BlickRichtung == RECHTS && xPos + 35 > Value1 + 320) ||
                (BlickRichtung == LINKS && xPos + 35 < Value1 + 320)) {
                Handlung = GEGNER_SPECIAL;
                AnimPhase = 0;
                AnimEnde = 0;
                ActionDelay = 0.0f;
                xSpeed = 0.0f;

                SoundManager.PlayWave(100, 128, 11025, SOUND_BLITZSTART);
            }
        } break;

        case GEGNER_LAUFEN:  // Auf Spieler zulaufen
        {
            // Ballern
            ActionDelay -= 1.0f SYNC;
            if (ActionDelay < 0.0f) {
                ActionDelay = 2.5f;

                // PartikelSystem.PushPartikel(xPos+30, yPos+28, BULLET);

                SoundManager.PlayWave(100, random(255), 8000 + random(4000), SOUND_CANON);

                if (BlickRichtung == RECHTS) {
                    PartikelSystem.PushPartikel(xPos + 50, yPos + 20, SMOKE);
                    Projectiles.PushProjectile(xPos + 55, yPos + 30, EVILSHOT);
                } else {
                    PartikelSystem.PushPartikel(xPos + 10, yPos + 20, SMOKE);
                    Projectiles.PushProjectile(xPos, yPos + 30, EVILSHOT2);
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
                Handlung = GEGNER_STEHEN;
                ActionDelay = 10.0f;
            }
        } break;

        case GEGNER_LAUFEN2:  // In die Mitte des Screens laufen und da Schweinereien machen
        {
            // Ballern
            ActionDelay -= 1.0f SYNC;
            if (ActionDelay < 0.0f) {
                ActionDelay = 2.5f;

                //				PartikelSystem.PushPartikel(xPos+30, yPos+28, BULLET);

                SoundManager.PlayWave(100, random(255), 8000 + random(4000), SOUND_CANON);

                if (BlickRichtung == RECHTS) {
                    PartikelSystem.PushPartikel(xPos + 50, yPos + 20, SMOKE);
                    Projectiles.PushProjectile(xPos + 55, yPos + 30, EVILSHOT);
                } else {
                    PartikelSystem.PushPartikel(xPos + 10, yPos + 20, SMOKE);
                    Projectiles.PushProjectile(xPos, yPos + 30, EVILSHOT2);
                }
            }

            if ((BlickRichtung == RECHTS && xPos + 35 > Value1 + 320) ||
                (BlickRichtung == LINKS && xPos + 35 < Value1 + 320)) {
                Handlung = GEGNER_SPECIAL;
                AnimPhase = 0;
                AnimEnde = 0;
                ActionDelay = 0.0f;
                xSpeed = 0.0f;

                SoundManager.PlayWave(100, 128, 11025, SOUND_BLITZSTART);
            }
        } break;

        // Boss ballert im Kreis um sich herum
        case GEGNER_BOMBARDIEREN: {
            ActionDelay -= 1.0f SYNC;

            if (ActionDelay < 0.0f) {
                SoundManager.PlayWave(100, random(255), 8000 + random(4000), SOUND_CANON);
                PartikelSystem.PushPartikel(xPos + (AnimPhase - 36) * 5, yPos - 23 + abs(AnimPhase - 41) * 8, SMOKE);
                //				PartikelSystem.PushPartikel(xPos+30, yPos+28, BULLET);

                Projectiles.PushProjectile(xPos + 5 + (AnimPhase - 36) * 5, yPos - 10 + abs(AnimPhase - 41) * 8,
                                           EVILROUND1 + (AnimPhase - 35));

                ActionDelay = 3.0f;
                AnimPhase++;

                if (AnimPhase > 46) {
                    Handlung = GEGNER_STEHEN;
                    AnimPhase = 0;
                    ActionDelay = 10.0f;
                }
            }
        } break;

        // Gegner Blitzt in den Himmel und Schüsse fallen von oben
        case GEGNER_SPECIAL: {
            ActionDelay += 1.0f SYNC;

            if (ActionDelay > 5.0f && ActionDelay < 20.0f && AnimPhase == 0)
                AnimPhase = 40;

            if (ActionDelay > 25.0f && AnimPhase != 0) {
                AnimPhase = 0;

                for (int i = 0; i < 30; i++)
                    PartikelSystem.PushPartikel(xPos + 30 + random(10), yPos + random(10), LASERFUNKE2);

                Projectiles.PushProjectile(xPos, yPos - 20, EVILBLITZ);
                SoundManager.PlayWave(100, 128, 11025, SOUND_SPIDERGRENADE);
            }

            // Screen blinken
            if (ActionDelay > 25.0f && ActionDelay < 35.0f) {
                int w = 128 - static_cast<int>(ActionDelay - 25.0f) * 12;

                RenderRect(0, 0, 640, 480, D3DCOLOR_RGBA(255, 255, 128, w));
            }

            if (ActionDelay > 70.0f) {
                if (random(2) == 0) {
                    Handlung = GEGNER_AUFRICHTEN;
                    BlickRichtung = LINKS;
                    AnimPhase = 3;
                    AnimStart = 3;
                    AnimEnde = 21;
                    xSpeed = -20;
                } else {
                    Handlung = GEGNER_AUFRICHTENZWEI;
                    BlickRichtung = RECHTS;
                    AnimPhase = 3;
                    AnimStart = 3;
                    AnimEnde = 21;
                    xSpeed = 20;
                }
            }
        } break;

        case GEGNER_AUFRICHTEN: {
            if (xPos < Value1 + 20) {
                BlickRichtung = RECHTS;
                xSpeed = 15.0f;
                ActionDelay = 4.0f;
                Handlung = GEGNER_AUSSPUCKEN;
            }
        } break;

        case GEGNER_AUFRICHTENZWEI: {
            if (xPos >= Value1 + 550) {
                BlickRichtung = LINKS;
                xSpeed = -15.0f;
                ActionDelay = 4.0f;
                Handlung = GEGNER_AUSSPUCKENZWEI;
            }
        } break;

        // Über den Screen rennen und dabei nach oben ballern
        case GEGNER_AUSSPUCKEN: {
            // Am Rand angekommen ?
            if (xPos >= Value1 + 550) {
                xSpeed = 0;
                AnimPhase = 0;
                ActionDelay = 10.0f;
                Handlung = GEGNER_STEHEN;
            }

            ActionDelay -= 1.0f SYNC;

            if (ActionDelay < 0.0f) {
                // schiessen ?
                if (AnimPhase != 40) {
                    xSpeed = 0.0f;
                    AnimEnde = 0;
                    AnimPhase = 40;
                    Projectiles.PushProjectile(xPos + 20, yPos + 10, ARCSHOT);
                    Projectiles.PushProjectile(xPos + 20, yPos + 10, ARCSHOTLEFT);
                    Projectiles.PushProjectile(xPos + 20, yPos + 10, ARCSHOTRIGHT);
                    SoundManager.PlayWave(100, 128, 11025, SOUND_SPIDERGRENADE);

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
        case GEGNER_AUSSPUCKENZWEI: {
            // Am Rand angekommen ?
            if (xPos <= Value1 + 20) {
                xSpeed = 0;
                AnimPhase = 0;
                ActionDelay = 10.0f;
                Handlung = GEGNER_STEHEN;
            }

            ActionDelay -= 1.0f SYNC;

            if (ActionDelay < 0.0f) {
                // schiessen ?
                if (AnimPhase != 40) {
                    xSpeed = 0.0f;
                    AnimEnde = 0;
                    AnimPhase = 40;
                    Projectiles.PushProjectile(xPos, yPos + 10, ROCKETSPIDER);
                    SoundManager.PlayWave(100, 128, 11025, SOUND_ROCKET);

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

        case GEGNER_EXPLODIEREN: {
            Energy = 80.0f;

            ActionDelay += 1.0f SYNC;
            if (ActionDelay >= 50.0f)
                Energy = 0.0f;

            AnimCount -= 1.0f SYNC;
            if (AnimCount < 0.0f) {
                AnimCount = 2.0f;

                SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND_EXPLOSION1);
                PartikelSystem.PushPartikel(xPos - 20 + random(70), yPos + random(80), EXPLOSION_MEDIUM);
                PartikelSystem.PushPartikel(xPos - 20 + random(70), yPos + random(80), EXPLOSION_MEDIUM2);
                PartikelSystem.PushPartikel(xPos - 50 + random(70), yPos - 30 + random(80), EXPLOSION_BIG);
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
        PartikelSystem.PushPartikel(xPos - 20 + random(70), yPos + random(80), SPLITTER);

    for (int i = 0; i < 10; i++)
        PartikelSystem.PushPartikel(xPos - 20 + random(70), yPos + random(80), EXPLOSION_MEDIUM2);

    SoundManager.PlayWave(100, 128, 11025, SOUND_EXPLOSION2);

    ScrolltoPlayeAfterBoss();

    HUD.BossHUDActive = false;
}
