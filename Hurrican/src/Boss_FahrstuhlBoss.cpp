// --------------------------------------------------------------------------------------
// Der Fahrstuhl Boss
//
// kommt von oben reingefahren und ballert mit zwei beweglichen Kanonen und einem
// riesen fetten Laser, der auf einer Schiene läuft
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_FahrstuhlBoss.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFahrstuhlBoss::GegnerFahrstuhlBoss(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_NOTVISIBLE;
    BlickRichtung	= LINKS;
    Energy			= 6000;
    ChangeLight		= Light;
    Destroyable		= true;
    ShotDelay		= 5.0f;
    Activated		= false;
    OwnDraw			= true;

    dx1				= 0.0f;
    dx2				= 0.0f;

    x1	= 140;
    y1	= 160;
    x2	=  25;
    y2	= 80;
    x3	= 340;

    CanonAnim = 0.0f;

    TempY			= 800.0f;
    yPos			= 0.0f;

    // Zusätzliche Grafiken für die Kanone und den Kopf laden
    Head.LoadImage ("fahrstuhlboss_head.png",    140, 186, 140, 186, 1, 1);
    Canon.LoadImage("fahrstuhlboss_canon.png",   177, 240,  59, 120, 3, 2);
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerFahrstuhlBoss::DoDraw(void)
{
    // Gegner mit Kopf und Kanonen rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSprite(float (xPos-TileEngine.XOffset),
                                           float (yPos-TileEngine.YOffset), 0, 0xFFFFFFFF);

    Head.RenderSprite  (float (xPos + x1 - TileEngine.XOffset), float (yPos + y1 - TileEngine.YOffset), 0xFFFFFFFF);
    Canon.RenderSprite (float (xPos + x2 - TileEngine.XOffset), float (yPos + y2 - TileEngine.YOffset), int (CanonAnim), 0xFFFFFFFF);
    Canon.RenderSprite (float (xPos + x3 - TileEngine.XOffset), float (yPos + y2 - TileEngine.YOffset), 5 - int (CanonAnim), 0xFFFFFFFF);

    GegnerRect[FAHRSTUHLBOSS].left   = int (x1 + 60);
    GegnerRect[FAHRSTUHLBOSS].right  = int (x1 + 60 + 25);
    //GegnerRect[FAHRSTUHLBOSS].left   = int (140 + 76);
    //GegnerRect[FAHRSTUHLBOSS].right  = int (140 + 76 + 34);
    GegnerRect[FAHRSTUHLBOSS].top    = int (y1 + 46);
    GegnerRect[FAHRSTUHLBOSS].bottom = int (y1 + 46 + 34);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFahrstuhlBoss::DoKI(void)
{
    if (x1 < 0)
        x1 = 0;

    // Energie anzeigen
    if (Handlung != GEGNER_NOTVISIBLE && Handlung != GEGNER_EXPLODIEREN)
        HUD.ShowBossHUD(6000, Energy);

    // Boss aktivieren und Mucke laufen lassen
    if (Active == true && Activated == false)
    {
        SoundManager.FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
        Activated = true;
        Value1 = int (TileEngine.YOffset + 100.0f);
    }

    // Zwischenboss blinkt nicht so lange wie die restlichen Gegner
    if (DamageTaken > 0.0f)
        DamageTaken -= 50 SYNC;					// Rotwerden langsam ausfaden lassen
    else
        DamageTaken = 0.0f;						// oder ganz anhalten

    // Hat der Boss keine Energie mehr ? Dann explodiert er
    if (Energy <= 100.0f && Handlung != GEGNER_EXPLODIEREN)
    {
        Handlung  = GEGNER_EXPLODIEREN;
        ShotDelay = 1.0f;
        g_Fahrstuhl_Speed = -1.0f;
        dx1		  = 0.0f;
        dx2		  = 0.0f;

        // Level wieder auf Spieler zentrieren
        //
        TileEngine.ScrollLevel(Player[0].xpos - 300,
                                 Player[0].ypos - 280, ZUSTAND_SCROLLTOPLAYER, 10.0f, 50.0f);

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC_BOSS, -2.0f, 0, false);
    }

    static float smokecount = 0.0f;

    // Füße an der Seite rauchen lassen
    //
    smokecount -= 1.0f SYNC;
    if (smokecount < 0.0f)
    {
        smokecount = 1.0f;

        PartikelSystem.PushPartikel(xPos + rand()%10 - 15, yPos,  SMOKEBIG);
        PartikelSystem.PushPartikel(xPos + rand()%10 - 15, yPos + 210, SMOKEBIG);

        PartikelSystem.PushPartikel(xPos + rand()%10 + 375, yPos,  SMOKEBIG);
        PartikelSystem.PushPartikel(xPos + rand()%10 + 375, yPos + 210, SMOKEBIG);

        // Wunden Punkt rauchen lassen bei wenig Energy
        if (Energy <= 1500.0f)
        {
            PartikelSystem.PushPartikel(xPos + x1 + 35 + rand()%20, yPos + y1 + rand()%15,  SMOKEBIG);

            if (rand()%10 == 0)
            {
                PartikelSystem.PushPartikel(xPos + x1 + 50 + rand()%20, yPos + y1 + 35 + rand()%15, LASERFLAME);
            }
        }
    }

    // Kanonen bewegen
    x1 += dx1 SYNC;
    x2 += dx2 SYNC;
    x3 -= dx2 SYNC;

    CanonAnim += dx2 SYNC;
    if (CanonAnim >= 6.0f) CanonAnim = 0.0f;
    if (CanonAnim <  0.0f) CanonAnim = 6.0f;

    if (x2 > 340)
    {
        x2 = 25;
        x3 = 340;
    }

    // Riesen Wumme begradigen
    if (Handlung == GEGNER_EINFLIEGEN ||
            Handlung == GEGNER_ABSENKEN   ||
            Handlung == GEGNER_AUFRICHTEN)
    {
        if ((dx1 < 0.0f && x1 < 140.0f) ||
                (dx1 > 0.0f && x1 > 140.0f))
        {
            x1 = 140.0f;
            dx1 = 0.0f;
        }
    }

// Je nach Handlung richtig verhalten
    switch (Handlung)
    {
    case GEGNER_NOTVISIBLE:			// Boss einfliegen lassen
    {
        // Warten bis die Level Mucke ausgefadet ist
        //DKS - Overhauled SoundManagerClass:
        //if (SoundManager.its_Songs[MUSIC_STAGEMUSIC]->Volume == SoundManager.its_Songs[MUSIC_STAGEMUSIC]->FadingEnd)
        //{
        //    // Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
        //    if (MUSIC_IsPlaying(SoundManager.its_Songs[MUSIC_BOSS]->SongData) == false)
        //        SoundManager.PlaySong(MUSIC_BOSS, false);

        //    Handlung = GEGNER_EINFLIEGEN2;				// Und Boss erscheinen lassen
        //}
        if (SoundManager.SongIsPaused(MUSIC_STAGEMUSIC))
        {
            // Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
            if (!SoundManager.SongIsPlaying(MUSIC_BOSS))
                SoundManager.PlaySong(MUSIC_BOSS, false);

            Handlung = GEGNER_EINFLIEGEN2;				// Und Boss erscheinen lassen
        }
    }
    break;

    case GEGNER_EINFLIEGEN2:	// Gegner kommt in den Screen geflogen am Anfang (einmalig)
    {
        Energy = 6000;
        DamageTaken = 0.0f;

        TempY -= 5.0f SYNC;

        // Gegner an der richtigen Position ?
        if (TempY < 500.0f)
        {
            Handlung = GEGNER_ABSENKEN;
            TempY    = 500.0f;
            dx2		 = 5.0f;
        }

        // Levelausschnitt weiter nach unten scrollen damit man mehr vom Boss sieht
        if (g_Fahrstuhl_Offset < 150.0f)
            g_Fahrstuhl_Offset += 4.0f SYNC;
        else
            g_Fahrstuhl_Offset = 150.0f;
    }
    break;

    case GEGNER_EINFLIEGEN:		// Gegner kommt in den Screen geflogen
    {
        TempY -= 5.0f SYNC;

        // Gegner an der richtigen Position ?
        if (TempY < 500.0f)
        {
            int j = rand ()%6;
            if (j == 0)
            {
                Handlung = GEGNER_ABSENKEN;
                TempY    = 500.0f;
                dx2		 = 0.0f;
            }
            else if (j == 1 || j == 2)
            {
                Handlung = GEGNER_ABSENKEN;
                TempY    = 500.0f;
                dx2		 = 5.0f;
            }
            else if (j == 3)
            {
                Handlung = GEGNER_AUSSPUCKEN;
                TempY    = 500.0f;
                Shots    = 1 + rand ()% 2;
            }
            else if (j == 4 || j == 5)
            {
                Handlung  = GEGNER_CRUSHEN;
                TempY     = 500.0f;
                Shots     = 10 + rand ()%20;
                ShotDelay = 2.0f;
            }
        }

        // Levelausschnitt weiter nach unten scrollen damit man mehr vom Boss sieht
        if (g_Fahrstuhl_Offset < 150.0f)
            g_Fahrstuhl_Offset += 4.0f SYNC;
        else
            g_Fahrstuhl_Offset = 150.0f;
    }
    break;

    case GEGNER_LAUFEN:			// yPos über dem Fahrstuhl
    {
    } break;

    case GEGNER_ABSENKEN:		// Gegner kommt nach unten
    {
        TempY -= 5.0f SYNC;

        // Gegner an der richtigen Position ?
        if (TempY < 380.0f)
        {
            if (dx2 != 0.0f)
            {
                Handlung  = GEGNER_SCHIESSEN;
                TempY     = 380.0f;
                ShotDelay = 1.0f;
                Shots     = rand()%10 + 10;
            }
            else
            {
                Handlung  = GEGNER_AUSSPUCKEN;
                TempY     = 380.0f;
                ShotDelay = 5.0f;
                Shots    = 2 + rand ()% 2;
            }
        }
    }
    break;

    case GEGNER_AUFRICHTEN:		// Gegner fährt wieder hoch
    {
        TempY += 5.0f SYNC;

        // Gegner an der richtigen Position ?
        if (TempY > 600.0f)
        {
            int j = rand()%3;
            if (j == 0)
            {
                Handlung  = GEGNER_BOMBARDIEREN;
                dx1	      = 40.0f;
                TempY     = 600.0f;
                ShotDelay = 1.0f;
                Shots     = rand()%8 + 5;
            }
            else if (j == 1)
            {
                Handlung  = GEGNER_SPECIAL;
                dx1	      = 0.0f;
                TempY     = 600.0f;
                ShotDelay = 5.0f;
                Shots     = rand()%8 + 5;
            }
            else if (j == 2)
            {
                Handlung  = GEGNER_AUSSPUCKEN;
                dx1	      = 0.0f;
                TempY     = 600.0f;
                ShotDelay = 5.0f;
                Shots     = rand()%4 + 2;
            }
        }
    }
    break;

    case GEGNER_SCHIESSEN:		// Mit zwei lasern ballern
    {
        ShotDelay -= 1.0f SYNC;

        if (ShotDelay < 0.0f)
        {
            ShotDelay = 6.0f;

            SoundManager.PlayWave(100, 128, 44100, SOUND_LASERSHOT);

            Projectiles.PushProjectile (xPos + x2 + 28,      yPos + y2 + 95,     KRABBLERLASER1);
            PartikelSystem.PushPartikel(xPos + x2 + 28 - 25, yPos + y2 + 95 - 4, LASERFLAME);

            Projectiles.PushProjectile (xPos + x3 + 28,      yPos + y2 + 95,     KRABBLERLASER1);
            PartikelSystem.PushPartikel(xPos + x3 + 28 - 25, yPos + y2 + 95 - 4, LASERFLAME);

            y2 = 75;

            // Genug geschossen ?
            Shots--;
            if (Shots <= 0)
            {
                dx2 = 0.0f;
                Handlung = GEGNER_AUFRICHTEN;
            }
        }
    }
    break;

    case GEGNER_BOMBARDIEREN:		// Steine bröckeln lassen
    {
        // An der Seite angekommen ? Dann ruckeln lassen und Steine werfen
        if ((x1 > 280 && dx1 > 0.0f) ||
                (x1 < 0   && dx1 < 0.0f))
        {
            ShakeScreen(3);
            dx1 *= -1.0f;

            Gegner.PushGegner (xPos + 10 + rand ()%360, yPos+480, BOULDER, 40+rand()%20, 0, false);

            // aufhören damit ?
            Shots--;
            if (Shots <= 0)
            {
                Handlung = GEGNER_EINFLIEGEN;

                if (x1 < 140.0f)
                    dx1 = 10.0f;
                else
                    dx1 = -10.0f;
            }
        }
    }
    break;

    case GEGNER_SPECIAL:			// Hurri anvisieren und Laser ballern
    {
        if (ShotDelay > 0.0f)
            ShotDelay -= 1.0f SYNC;

        // Bewegen
        if (pAim->xpos + 35 < xPos + x1 + 60) x1 -= 8.0f SYNC;
        if (pAim->xpos + 35 > xPos + x1 + 80) x1 += 8.0f SYNC;

        // Schiessen
        if (pAim->xpos + 35 < xPos + x1 + 100 &&
                pAim->xpos + 35 > xPos + x1 + 40 &&
                ShotDelay <= 0.0f)
        {
            ShotDelay = 8.0f;

            SoundManager.PlayWave (100, 128, 10000 + rand()%2000, SOUND_LASERSHOT);

            Projectiles.PushProjectile  (xPos + x1 + 80 - 28, yPos + y1 + 155, UFOLASER);
            PartikelSystem.PushPartikel (xPos + x1 + 80 - 90, yPos + y1 + 110, UFOLASERFLARE);

            y1 = 155;

            // Aufhören zu ballern
            Shots--;
            if (Shots <= 0)
            {
                Handlung = GEGNER_EINFLIEGEN;

                if (x1 < 140.0f)
                    dx1 = 10.0f;
                else
                    dx1 = -10.0f;
            }
        }
    }
    break;

    case GEGNER_AUSSPUCKEN:				// Stelzis herbeirufen =)
    {
        ShotDelay -= 1.0f SYNC;

        if (ShotDelay < 0.0f)
        {
            ShotDelay = 35.0f;

            Gegner.PushGegner (xPos + 10 + rand ()%360, yPos+480, STELZSACK, 80, 0, false);

            // Genug losgelassen ?
            Shots--;
            if (Shots <= 0)
            {
                dx2 = 0.0f;
                if (TempY != 380.0f)
                    Handlung = GEGNER_EINFLIEGEN;
                else
                    Handlung = GEGNER_AUFRICHTEN;

                if (x1 < 140.0f)
                    dx1 = 10.0f;
                else
                    dx1 = -10.0f;
            }
        }

    }
    break;

    case GEGNER_CRUSHEN:				// Suchschüsse ballern
    {
        ShotDelay -= 1.0f SYNC;

        if (ShotDelay < 0.0f)
        {
            ShotDelay = 5.0f;

            SoundManager.PlayWave(100, 128, 8000 + rand ()%4000, SOUND_CANON);

            Projectiles.PushProjectile (xPos + x2 + 28,      yPos + y2 + 95+5, SUCHSCHUSS);
            PartikelSystem.PushPartikel(xPos + x2 + 28 - 10, yPos + y2 + 95,   SMOKE);

            Projectiles.PushProjectile (xPos + x3 + 28,      yPos + y2 + 95+5, SUCHSCHUSS);
            PartikelSystem.PushPartikel(xPos + x3 + 28 - 10, yPos + y2 + 95,   SMOKE);

            y2 = 77;

            // Genug losgelassen ?
            Shots--;
            if (Shots <= 0)
            {
                dx2		 = 5.0f;
                Handlung = GEGNER_ABSENKEN;

                if (x1 < 140.0f)
                    dx1 = 10.0f;
                else
                    dx1 = -10.0f;
            }
        }

    }
    break;

    case GEGNER_FALLEN:
    {
    } break;

    // Gegner fliegt in die Luft
    //
    case GEGNER_EXPLODIEREN:
    {
        ShotDelay -= 1.0f SYNC;

        if (ShotDelay < 0.0f)
        {
            ShotDelay = 0.5f;
            SoundManager.PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);

            PartikelSystem.PushPartikel (xPos -30 + rand()%400, yPos + rand()%280, EXPLOSION_MEDIUM2);
            PartikelSystem.PushPartikel (xPos -50 + rand()%400, yPos + rand()%280, EXPLOSION_BIG);

            if (rand ()%2 == 0)
                PartikelSystem.PushPartikel (xPos + rand()%400, yPos + rand()%280, SPLITTER);
        }

        Energy = 100.0f;

        // Levelausschnitt wieder weiter nach oben Scrollen
        if (g_Fahrstuhl_Offset > 20.0f)
            g_Fahrstuhl_Offset -= 4.0f SYNC;
        else
            g_Fahrstuhl_Offset = 20.0f;

        TempY -= 10.0f SYNC;

        // Gegner aus dem Screen, dann fetzen
        if (TempY < -400.0f)
            Energy = 0.0f;

    }
    break;

    default :
        break;
    } // switch

    // yPosition über dem Fahrstuhl setzen
    yPos = g_Fahrstuhl_yPos - TempY;

    if (yPos < 0.0f)
        yPos = 0.0f;

    // Anhängsel am Fahrstuhl richtig positionieren
    if (y2 <  80.0f) y2 += 2.0f SYNC;
    else y2 =  80.0f;
    if (y1 < 160.0f) y1 += 2.0f SYNC;
    else y1 = 160.0f;

    // Schüsse und Partikel jeweils anpassen, damit sie gerade fliegen und nicht nach oben
    // abdriften wegen der FahrstuhlGeschwindigkeit

    for (int i = 0; i < NUMPLAYERS; i++)
        if (Player[i].AufPlattform == NULL)
        {
            Player[i].ypos	  += 40.0f SYNC;
            Player[i].yposold   += 40.0f SYNC;
            Player[i].JumpStart += 40.0f SYNC;
        }

    PartikelClass *pTemp = PartikelSystem.pStart; // Zeiger auf den ersten Partikel
    while (pTemp != NULL)						 	// Ende der Liste erreicht ?
    {
        if (pTemp->PartikelArt != FUNKE  &&
                pTemp->PartikelArt != BULLET &&
                pTemp->PartikelArt != SMOKE)
        {
            pTemp->yPos	   += 40.0f SYNC;			// Nach unten bewegen
            pTemp->yPosOld += 40.0f SYNC;
        }
        pTemp = pTemp->pNext;						// Zeiger auf das nächste Element
    }

    ProjectileClass *pTemp2 = Projectiles.pStart;	// Zeiger auf den ersten Schuss
    while (pTemp2 != NULL)							// Ende der Liste erreicht ?
    {
        if (pTemp2->ShotArt != STELZLASER)
        {
            pTemp2->yPos	+= 40.0f SYNC;			// Nach unten bewegen
            pTemp2->yPosOld += 40.0f SYNC;
        }
        pTemp2 = pTemp2->pNext;					// Zeiger auf das nächste Element
    }

    // Level-Hintergrund wiederholen und alle Objekte wieder nach oben setzen
    const int A = 2 * 24 * 20;
    if (yPos > Value1 + A)
    {
        TileEngine.YOffset -= A;

        for (int p = 0; p < NUMPLAYERS; p++)
            Player[p].ypos -= A;

        GegnerClass *pTemp    = Gegner.pStart;		// Zeiger auf den ersten Gegner
        while (pTemp != NULL)							// Ende der Liste erreicht ?
        {
            if (pTemp->GegnerArt >= SPITTER &&
                    pTemp->GegnerArt <= FAHRSTUHL)
            {
                pTemp->yPos	   -= A;				// Nach oben bewegen
                pTemp->yPosOld -= A;
            }

            pTemp = pTemp->pNext;						// Zeiger auf das nächste Element
        }

        ProjectileClass *pTemp2 = Projectiles.pStart;	// Zeiger auf den ersten Schuss
        while (pTemp2 != NULL)							// Ende der Liste erreicht ?
        {
            pTemp2->yPos	-= A;				// Nach oben bewegen
            pTemp2->yPosOld -= A;
            pTemp2 = pTemp2->pNext;						// Zeiger auf das nächste Element
        }

        PartikelClass *pTemp3 = PartikelSystem.pStart;// Zeiger auf den ersten Partikel
        while (pTemp3 != NULL)							// Ende der Liste erreicht ?
        {
            pTemp3->yPos	-= A;				// Nach oben bewegen
            pTemp3->yPosOld -= A;
            pTemp3 = pTemp3->pNext;						// Zeiger auf das nächste Element
        }
    }
}

// --------------------------------------------------------------------------------------
// FahrstuhlBoss explodiert
// --------------------------------------------------------------------------------------

void GegnerFahrstuhlBoss::GegnerExplode(void)
{
    ShakeScreen (10);
    SoundManager.PlayWave (100, 128, 11025, SOUND_EXPLOSION2);
    Player[0].Score += 9000;

    // Level Musik wieder einfaden lassen (aus Pause Zustand)
    SoundManager.FadeSong(MUSIC_STAGEMUSIC, 2.0f, 100, true);

    //TileEngine.YOffset = float (Value1);
}
