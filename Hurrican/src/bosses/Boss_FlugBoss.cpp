// --------------------------------------------------------------------------------------
// Der Flugboss
//
// Muss noch erfunden werden ;)
// --------------------------------------------------------------------------------------

#include "Boss_FlugBoss.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFlugBoss::GegnerFlugBoss(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::NOTVISIBLE;
    Energy = 6000;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimPhase = 0;
    AnimEnde = 0;
    AnimSpeed = 0.0f;
    ChangeLight = Light;
    Destroyable = true;
    TestBlock = false;
    OwnDraw = true;
    SchienePos = 320.0f;
    tempSpeed = 0.0f;

    Winkel[0] = Winkel[1] = 0.0f;

    alphaAuge = 0.0f;
    alphaSpeed = 10.0f;
    alphaDir = 1.0f;

    ShotDelay = 0.0f;
    ShotCount = 0.0f;

    yKanone[0] = yKanone[1] = yKanone[2] = yKanone[3] = 0.0f;

    xKanone = 0.0f;
    xHit = random(440) + 100.0f;
    yHit = 100.0f;
    dHit = -1.0f;

    // Zusätzliche Grafiken laden
    //
    SchieneL.LoadImage("flugboss_schiene_l.png", 320, 73, 320, 73, 1, 1);
    SchieneR.LoadImage("flugboss_schiene_r.png", 320, 73, 320, 73, 1, 1);

    Kanone_big.LoadImage("flugboss_kanone_fett.png", 21, 85, 21, 85, 1, 1);
    Kanone_medium.LoadImage("flugboss_kanone_klein.png", 11, 48, 11, 48, 1, 1);
    Kanone_small.LoadImage("flugboss_kanone_mini.png", 8, 63, 8, 63, 1, 1);
    Geschuetz.LoadImage("flugboss_geschuetz.png", 62, 97, 62, 97, 1, 1);

    Auge.LoadImage("flugboss_auge.png", 29, 30, 29, 30, 1, 1);

    Hitzone[0].LoadImage("flugboss_hitzone.png", 52, 100, 52, 100, 1, 1);
    Hitzone[1].LoadImage("flugboss_hitzone2.png", 52, 31, 52, 31, 1, 1);
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerFlugBoss::DoDraw() {
    // Schienen
    SchieneL.RenderSprite(-SchienePos, yPos - TileEngine.YOffset + 161.0f, 0xFFFFFFFF);

    SchieneR.RenderSprite(320 + SchienePos, yPos - TileEngine.YOffset + 161.0f, 0xFFFFFFFF);

    // Fette Kanone
    Kanone_big.RenderSprite(-TileEngine.XOffset + xPos + 121.0f,
                            -TileEngine.YOffset + yPos + 249.0f - yKanone[0], 0xFFFFFFFF);

    // Medium Kanone
    Kanone_medium.RenderSprite(-TileEngine.XOffset + xPos + 148.0f,
                               -TileEngine.YOffset + yPos + 254.0f - yKanone[1], 0xFFFFFFFF);

    // Mini Kanone 1
    Kanone_small.RenderSprite(-TileEngine.XOffset + xPos + 68.0f,
                              -TileEngine.YOffset + yPos + 261.0f - yKanone[2], 0xFFFFFFFF);

    // Mini Kanone 2
    Kanone_small.RenderSprite(-TileEngine.XOffset + xPos + 78.0f,
                              -TileEngine.YOffset + yPos + 251.0f - yKanone[3], 0xFFFFFFFF);

    // Schatten
    RenderRect(xPos - TileEngine.XOffset + 255.0f,
               yPos - TileEngine.YOffset + 161.0f,
               18, 80, 0x88000000);

    // Main
    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                           yPos - TileEngine.YOffset, AnimPhase, 0xFFFFFFFF);

    // int a = int (alphaAuge); // PICKLE not used

    // Hit Zone
    Hitzone[0].RenderSprite(xHit, 380.0f + yHit, 0, 0xFFFFFFFF);
    Hitzone[1].RenderSprite(xHit, 401.0f + yHit, 0, 0xFFFFFFFF);

    GegnerRect[GegnerArt].left = static_cast<int>(xHit + TileEngine.XOffset - xPos);
    GegnerRect[GegnerArt].right = GegnerRect[GegnerArt].left + 52;
    GegnerRect[GegnerArt].top = static_cast<int>(380.0f + TileEngine.YOffset - yPos + yHit);
    GegnerRect[GegnerArt].bottom = GegnerRect[GegnerArt].top + 100;

    // Geschuetz
    Geschuetz.RenderSpriteRotated(xKanone - 62, -TileEngine.YOffset + yPos + 160.0f, Winkel[0], 0,
                                  0xFFFFFFFF);

    // Geschuetz
    Geschuetz.RenderSpriteRotated(RENDERWIDTH - xKanone, -TileEngine.YOffset + yPos + 160.0f, Winkel[1], 0,
                                  0xFFFFFFFF);

    // Auge
    alphaAuge += Timer.sync(alphaSpeed * alphaDir);

    // Grenzen für Alpha checken
    if (alphaAuge > 255.0f) {
        alphaAuge = 255.0f;
        alphaDir = -1.0f;
    }

    if (alphaAuge < 0.0f) {
        alphaAuge = 0.0f;
        alphaDir = 1.0f;
    }

    Auge.RenderSprite(-TileEngine.XOffset + xPos + 175.0f,
                      -TileEngine.YOffset + yPos + 182.0f, 0, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFlugBoss::DoKI() {
    /*
        BlickRichtung	= LINKS;

        // Energie anzeigen
        if (Handlung != GEGNER::INIT && Handlung != GEGNER::EXPLODIEREN)
            HUD.ShowBossHUD(6000, Energy);

        // Animieren
        if (AnimEnde > 0)						// Soll überhaupt anmiert werden ?
        {
            AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
            if (AnimCount > AnimSpeed)			// Grenze überschritten ?
            {
                AnimCount = 0;					// Dann wieder auf Null setzen
                AnimPhase++;					// Und nächste Animationsphase
                if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
                    AnimPhase = AnimStart;		// Dann wieder von vorne beginnen
            }
        } // animieren

        // Levelausschnitt auf den Boss zentrieren, sobald dieser sichtbar wird
        if (Active == true && TileEngine.Zustand == ZUSTAND_SCROLLBAR)
        {
            TileEngine.ScrollLevel(static_cast<float>(Value1),
                                     static_cast<float>(Value2), ZUSTAND_SCROLLTOLOCK);		// Level auf den Boss
       zentrieren

            SoundManager.FadeSong(MUSIC::STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren

            yPos -= 480;
        }

        // Zwischenboss blinkt nicht so lange wie die restlichen Gegner
        if (DamageTaken > 0.0f)
            DamageTaken -= Timer.sync(100-0f);				// Rotwerden langsam ausfaden lassen
        else
            DamageTaken = 0.0f;						// oder ganz anhalten

        // Hat der Boss keine Energie mehr ? Dann explodiert er
        //
        if (Energy <= 100.0f && Handlung != GEGNER::EXPLODIEREN)
        {
            Handlung  = GEGNER::EXPLODIEREN;

            // Endboss-Musik ausfaden und abschalten
            SoundManager.FadeSong(MUSIC::BOSS, -2.0f, 0, false);
        }

        // Je nach Handlung richtig verhalten
        //
        switch (Handlung)
        {
            case GEGNER::INIT:			// Warten bis der Screen zentriert wurde
            {
                if (TileEngine.Zustand == ZUSTAND_LOCKED)
                {
                    tempSpeed = 35.0f;
                    Handlung  = GEGNER::INIT2;

                    SoundManager.PlayWave (100, 128, 3500, SOUND::ROCKET);
                    SoundManager.PlayWave (100, 128, 4500, SOUND::ROCKET);
                }
            } break;


            // von oben einfliegen
            //
            case GEGNER::INIT2 :
            {
                tempSpeed -= Timer.sync(1.35f);

                if (tempSpeed < 3.0f)
                    tempSpeed = 3.0f;

                yPos += Timer.sync(tempSpeed);

                // Endposition erreicht? Dann Schienen einfahren
                //
                if (yPos >= float (Value2 - 60))
                {
                    yPos	  = float (Value2 - 60);
                    tempSpeed = 15.0f;
                    Handlung  = GEGNER::INIT3;

                    SoundManager.StopWave (SOUND::ROCKET);
                    SoundManager.PlayWave (100, 128, 11025, SOUND::DOORSTOP);
                    SoundManager.PlayWave (100, 128, 11025, SOUND::DOOR);
                }

            } break;

            // Schienen einfahren
            //
            case GEGNER::INIT3:
            {
                tempSpeed -= Timer.sync(0.2f);

                if (tempSpeed < 2.0f)
                    tempSpeed = 2.0f;

                SchienePos -= Timer.sync(tempSpeed);

                if (SchienePos <= 0.0f)
                {
                    xKanone    = 0.0f;
                    AnimCount  = 10.0f;
                    Handlung   = GEGNER::LAUFEN;
                    SchienePos = 0.0f;
                    SoundManager.StopWave (SOUND::DOOR);
                    SoundManager.PlayWave (100, 128, 13000, SOUND::DOORSTOP);
                    SoundManager.PlayWave (100, 128, 11000, SOUND::ROCKET);
                    tempSpeed = 12.0f;

                    // Boss-Musik abspielen, sofern diese noch nicht gespielt wird
                    //
                    if (MUSIC::IsPlaying(SoundManager.its_Songs[MUSIC::BOSS]->SongData) == false)
                        SoundManager.PlaySong(MUSIC::BOSS, false);
                }

            } break;

            // Geschütze reinfahren
            //
            case GEGNER::EINFAHREN:
            {
                tempSpeed -= Timer.sync(0.5f);

                if (tempSpeed < 2.0f)
                    tempSpeed = 2.0f;

                xKanone += Timer.sync(tempSpeed);

                if (xKanone >= 110.0f)
                {
                    AnimCount  = 10.0f;
                    Handlung   = GEGNER::SPECIAL;
                    ShotDelay  = 1.0f;
                    ShotCount  = 20.0f;
                    xKanone	   = 110.0f;
                    SoundManager.PlayWave (100, 128, 15000, SOUND::DOORSTOP);
                }

            } break;

            // Geschütze rausfahren
            //
            case GEGNER::AUSFAHREN:
            {
                tempSpeed += Timer.sync(0.5f);

                if (tempSpeed > 10.0f)
                    tempSpeed = 10.0f;

                xKanone -= Timer.sync(tempSpeed);

                if (xKanone <= 0.0f)
                {
                    AnimCount  = 10.0f;
                    Handlung   = GEGNER::EINFAHREN;
                    xKanone	   = 0.0f;
                    tempSpeed  = 12.0f;
                    SoundManager.StopWave (SOUND::DOOR);
                    SoundManager.PlayWave (100, 128, 15000, SOUND::DOORSTOP);
                    SoundManager.PlayWave (100, 128, 11000, SOUND::ROCKET);
                }

            } break;

            // Mit Geschützen ballern
            //
            case GEGNER::SPECIAL:
            {
                // Kanonen ausrichten
                //
                float dx, dy;
                float w, winkel;

                // Kanone Links

                // Abstände berechnen
                dx = (xPos - 120) - (pPlayer->xpos + 35);
                dy = (yPos + 194) - (pPlayer->ypos + 40);

                // Division durch Null verhinden
                if (dy == 0.0f)
                    dy = 0.01f;

                w = static_cast<float>(atan(dx / dy) * 360.0f / (D3DX_PI * 2));

                if (dx >= 0 && dy >= 0) winkel = w;		  else
                if (dx > 0  && dy < 0 ) winkel = 180 + w;  else
                if (dx < 0  && dy > 0 ) winkel = 360 + w;  else
                if (dx < 0  && dy < 0 ) winkel = 180 + w;

                winkel = 180.0f - winkel;

                if (Winkel[0] < winkel)
                    Winkel[0] += Timer.sync(5.0f);
                if (Winkel[0] > winkel)	Winkel[0] -= Timer.sync(5.0f);

                // Kanone rechts

                // Abstände berechnen
                dx = (xPos + 370) - (pPlayer->xpos + 35);
                dy = (yPos + 194) - (pPlayer->ypos + 40);

                // Division durch Null verhinden
                if (dy == 0.0f)
                    dy = 0.01f;

                w = static_cast<float>(atan(dx / dy) * 360.0f / (D3DX_PI * 2));

                if (dx >= 0 && dy >= 0) winkel = w;		  else
                if (dx > 0  && dy < 0 ) winkel = 180 + w;  else
                if (dx < 0  && dy > 0 ) winkel = 360 + w;  else
                if (dx < 0  && dy < 0 ) winkel = 180 + w;

                winkel = 180.0f - winkel;

                if (Winkel[1] < winkel)
                    Winkel[1] += Timer.sync(5.0f);
                if (Winkel[1] > winkel)	Winkel[1] -= Timer.sync(5.0f);

                // Schiessen lassen
                //
                ShotDelay -= Timer.sync(1.0f);

                while (ShotDelay <= 0.0f)
                {
                    ShotDelay += 0.2f;
                    ShotCount -= 1.0f;

                    SoundManager.PlayWave (100, 128, 8000 + rand()%1000, SOUND::CANON);

                    if (int (ShotCount) % 2 == 0)
                    {
                        float xo, yo;

                        xo = xPos - 116;
                        yo = yPos + 210;

                        xo += float (60 * sin (PI * winkel / 180.0f));
                        yo += float (60 * cos (PI * winkel / 180.0f));

                        Projectiles.PushProjectile (xo, yo, SUCHSCHUSS);
                    }
                    else
                        Projectiles.PushProjectile (xPos + 370, yPos + 210, SUCHSCHUSS);
                }


            } break;

            case GEGNER::EXPLODIEREN:
            {
                //Energy = 100.0f;
                AnimCount -= Timer.sync(1.0f);

                if (AnimCount < 0.0f)
                {
                    PartikelSystem.PushPartikel(xPos + rand()%500, yPos + rand()%300, EXPLOSION_MEDIUM2);
                    SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND::EXPLOSION1);

                    AnimCount = 1.0f;
                }
            }

            // Wait Counter runterzählen und neue Aktion bestimmen
            //
            case GEGNER::LAUFEN:
            {
                AnimCount -= Timer.sync(1.0f);

                if (AnimCount < 0.0f)
                {
                    AnimCount = 0.0f;

                    // neue Aktion festlegen
                    //
                    int j = rand ()% 3;

                    j = 1;

                    switch (j)
                    {
                        // aus den zwei kleinen Geschützen ballern?
                        //
                        case 0 :
                        {
                            Handlung = GEGNER::SCHIESSEN;

                            ShotDelay = 1.0f;
                            ShotCount = 50;
                            xSpeed    = -10.0f;
                        } break;

                        // Geschütze einfahren und ballern
                        //
                        case 1 :
                        {
                            xKanone    = 0.0f;
                            AnimCount  = 10.0f;
                            Handlung   = GEGNER::EINFAHREN;
                            SchienePos = 0.0f;
                            SoundManager.PlayWave (100, 128, 11000, SOUND::ROCKET);
                            tempSpeed = 12.0f;
                        } break;

                        case 2 :
                        {
                        } break;

                    } // switch

                } break;

                // Boss schiesst mit den zwei kleinen Kanonen
                //
                case GEGNER::SCHIESSEN:
                {
                    ShotDelay -= Timer.sync(1.0f);

                    while (ShotDelay <= 0.0f)
                    {
                        ShotDelay += 3.0f;

                        SoundManager.PlayWave (100, 128, 15000, SOUND::LASERSHOT);
                        ShotCount -= 1.0f;

                        if (int (ShotCount) % 2 == 0)
                        {
                            yKanone[2] = 20;

                            Projectiles.PushProjectile (xPos + 68,
                                                          yPos + 261 + 30, FLUGLASER);
                        }
                        else
                        {
                            yKanone[3] = 20;

                            Projectiles.PushProjectile (xPos + 78,
                                                          yPos + 251 + 30, FLUGLASER);
                        }

                        if (ShotCount <= 0.0f)
                        {
                            Handlung = GEGNER::LAUFEN;
                        }
                    }

                    if ((xSpeed < 0.0f && xPos - TileEngine.XOffset < 100) ||
                        (xSpeed > 0.0f && xPos - TileEngine.XOffset > 640 - 280 - 100))
                        xSpeed *= -1.0f;

                } break;
            }

            default : break;
        } // switch

        // Kanonen wieder zurückziehen, wenn sie geschossen haben
        //
        for (int i = 0; i < 4; i++)
        {
            if (yKanone[i] > 0.0f)
                yKanone[i] -= Timer.sync(10.0f);
            else
                yKanone[i] = 0.0f;
        }*/

    // Kanone reindrehen
    //
    if (xKanone < 150.0f) {
        /*Winkel[0] = xKanone + 180.0f * 1.4f;


        if (Winkel[0] > 360.0f)
            Winkel[0] = 0.0f;

        Winkel[1] = 360.0f - Winkel[0];*/
    }

    // Hitzone erscheinen lassen
    //
    yHit += Timer.sync(dHit);

    if (yHit < 0.0f) {
        yHit = 0.0f;
        dHit = 0.75f;
    }

    if (yHit > 100.0f) {
        yHit = 100.0f;
        dHit = -3.0f;
        xHit = random(440) + 100.0f;
    }
}

// --------------------------------------------------------------------------------------
// FlugBoss explodiert
// --------------------------------------------------------------------------------------

void GegnerFlugBoss::GegnerExplode() {
    // Splitter
    for (int i = 0; i < 20; i++)
        PartikelSystem.PushPartikel(xPos + 30.0f + static_cast<float>(random(500)),
                                    yPos + 30.0f + static_cast<float>(random(2000)), SPLITTER);

    Player[0].Score += 6000;

    /*
        // Level wieder zum Spieler scrollen und dann weiterscrollen lassen
        // dabei aufpassen, dass der Spieler nicht zu weit links fliegt und der Scrollbereich im
        // negativen liegt, da sonst das Spiel hängenbleibt =)
        //
        if (pPlayer->xpos - 300 <= 0)
            TileEngine.ScrollLevel(0,
                                     pPlayer->ypos - 280, ZUSTAND_SCROLLTOPLAYER);
        else

            TileEngine.ScrollLevel(pPlayer->xpos - 300,
                                     pPlayer->ypos - 280, ZUSTAND_SCROLLTOPLAYER);
                                     */

    // Level Musik wieder einfaden lassen (aus Pause Zustand)
    SoundManager.FadeSong(MUSIC::STAGEMUSIC, 2.0f, 100, true);
}
