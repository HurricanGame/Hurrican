// --------------------------------------------------------------------------------------
// Das Ufo
//
// fliegt von links nach rechts und andersrum über dem Spieler und ballert dabei
// --------------------------------------------------------------------------------------

#include "Boss_Ufo.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerUfo::GegnerUfo(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_NOTVISIBLE;
    BlickRichtung = LINKS;
    Energy = 4000;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimEnde = 21;
    AnimSpeed = 1.0f;
    ChangeLight = Light;
    Destroyable = true;
    ActionDelay = 40.0f;
    SmokeDelay = 0.0f;
    OwnDraw = true;
    LastAction = -1;

    // DKS - Added initializer, after valgrind reported GEGNER_LAUFEN switch case in
    //      DoKI() was using this uninitialized:
    ShotDelay = 0.0f;

    TileEngine.MaxOneUps++;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerUfo::DoDraw() {
    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - static_cast<float>(TileEngine.XOffset),
                                           yPos - static_cast<float>(TileEngine.YOffset), AnimPhase, 0xFFFFFFFF);

    if (AlreadyDrawn == false) {
        DirectGraphics.SetAdditiveMode();
        Projectiles.LavaFlare.RenderSpriteRotated(xPos + 40.0f - static_cast<float>(TileEngine.XOffset),
                                                  yPos + 20.0f - static_cast<float>(TileEngine.YOffset), xPos * 0.5f,
                                                  0xFFFF2288);

        // DKS - This was commented out in original source code:
        //		Projectiles.LavaFlare.RenderSpriteRotated(xPos + 40.0f - static_cast<float>(TileEngine.XOffset),
        //									  yPos + 30.0f - static_cast<float>(TileEngine.YOffset),
        //									  360.0f - xPos * 0.5f, 0x88FFFF88);

        DirectGraphics.SetColorKeyMode();
    }

    AlreadyDrawn = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerUfo::DoKI() {
    SmokeDelay -= 1.0f SYNC;

    // Energie anzeigen
    if (Handlung != GEGNER_INIT && Handlung != GEGNER_EXPLODIEREN)
        HUD.ShowBossHUD(4000, Energy);

    // Animieren
    if (AnimEnde > 0)  // Soll überhaupt anmiert werden ?
    {
        AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;              // Dann wieder auf Null setzen
            AnimPhase++;                // Und nächste Animationsphase
            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
                AnimPhase = AnimStart;  // Dann wieder von vorne beginnen
        }
    }  // animieren

    // Levelausschnitt auf den Boss zentrieren, sobald dieser sichtbar wird
    if (Active == true && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        TileEngine.ScrollLevel(static_cast<float>(Value1), static_cast<float>(Value2),
                               TileStateEnum::SCROLLTOLOCK);  // Level auf den Boss zentrieren
        yPos -= 300;                                   // und Boss aus dem Screen setzen
        Handlung = GEGNER_INIT;

        SoundManager.FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
    }

    // Zwischenboss blinkt nicht so lange wie die restlichen Gegner
    if (DamageTaken > 0.0f)
        DamageTaken -= 100 SYNC;  // Rotwerden langsam ausfaden lassen
    else
        DamageTaken = 0.0f;  // oder ganz anhalten

    // Schon schwer angeschlagen ? Dann raucht das Ufo =)
    if (Energy < 700 && random(5) == 0)
        PartikelSystem.PushPartikel(xPos + random(190), yPos + random(60) + 25, SMOKE);

    // Hat der Boss keine Energie mehr ? Dann explodiert er
    if (Energy <= 100.0f && Handlung != GEGNER_EXPLODIEREN) {
        Handlung = GEGNER_EXPLODIEREN;
        xSpeed = 0.0f;
        ySpeed = 0.0f;
        xAcc = 0.0f;
        yAcc = 0.5f;
        AnimEnde = 0;
        AnimCount = 1.0f;

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC_BOSS, -2.0f, 0, false);
    }

    // Über dem Spieler schweben
    //
    if (Handlung != GEGNER_INIT && Handlung != GEGNER_EINFLIEGEN && Handlung != GEGNER_EXPLODIEREN) {
        if (xPos + 100 < Value1 + 320)
            xAcc = 3.0f;
        else
            xAcc = -3.0f;
    }

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

        case GEGNER_EINFLIEGEN:  // Gegner kommt in den Screen geflogen
        {
            Energy = 4000;
            DamageTaken = 0.0f;

            yPos += static_cast<float>(8.0 SYNC);                 // Ufo nach unten bewegen
            if (yPos >= TileEngine.ScrolltoY + 100)  // Weit genug unten ?
            {
                yPos = static_cast<float>(TileEngine.ScrolltoY + 100);
                Handlung = GEGNER_LAUFEN;
                xSpeed = -35.0f;
            }
        } break;

        case GEGNER_LAUFEN:  // Über dem Spieler schweben
        {
            // Nach unten fliegen oder oben
            if (yPos < Value2 + 100)
                yPos += 5.0f SYNC;
            if (yPos > Value2 + 100)
                yPos -= 5.0f SYNC;

            // Wenig Energie? Dann Gegner spawnen
            // if (Energy <= 2000.0f)
            {
                ShotDelay -= 1.0f SYNC;

                if (ShotDelay < 0.0f) {
                    Gegner.PushGegner(xPos + 45.0f, yPos + 40.0f, FETTERAKETE, 360, 99, false);
                    Gegner.PushGegner(xPos + 135.0f, yPos + 40.0f, FETTERAKETE, 0, 99, false);
                    ShotDelay = 18.0f;
                    SoundManager.PlayWave(100, 128, 8000 + random(1000), SOUND_GRANATE);
                }
            }

            // Neue Action ausführen ?
            //
            ActionDelay -= 1.0f SYNC;

            if (ActionDelay < 0.0f) {
                int j = random(3);

                while (j == LastAction)
                    j = random(3);

                LastAction = j;

                if (j == 0)
                    Handlung = GEGNER_SCHIESSEN;
                if (j == 1)
                    Handlung = GEGNER_BOMBARDIEREN;
                if (j == 2) {
                    Handlung = GEGNER_FALLEN;
                    ySpeed = 28.0f;
                    yAcc = -2.0f;
                    ShotDelay = 1.0f;
                }

                ShotDelay = 10.0f;
                ActionDelay = 60.0f;
            }
        } break;

        case GEGNER_SCHIESSEN:  // Mit Suchschuss auf den Spieler schiessen
        {
            // Nach unten fliegen
            if (yPos > Value2 + 50)
                yPos -= 5.0f SYNC;
            else
                yPos = Value2 + 50.0f;

            // Wieder nur rumschweben ?
            ActionDelay -= 1.0f SYNC;
            if (ActionDelay < 0.0f) {
                ActionDelay = 80.0f;
                Handlung = GEGNER_LAUFEN;
            }

            if (ShotDelay > 0.0f)
                ShotDelay -= 1.0f SYNC;

            if (ShotDelay <= 0.0f) {
                SoundManager.PlayWave(50, 128, 14000 + random(2000), SOUND_GOLEMSHOT);
                Projectiles.PushProjectile(xPos + 20.0f, yPos + 40.0f, SUCHSCHUSS2);
                Projectiles.PushProjectile(xPos + 165.0f, yPos + 40.0f, SUCHSCHUSS2);
                ShotDelay = 5.0f;
            }
        } break;

        case GEGNER_BOMBARDIEREN:  // Mit Laser auf den Spieler schiessen
        {
            // Nach oben fliegen
            if (yPos > Value2 + 50)
                yPos -= 5.0f SYNC;
            else
                yPos = Value2 + 50.0f;

            // Wieder nur rumschweben ?
            ActionDelay -= 1.0f SYNC;
            if (ActionDelay < 0.0f) {
                ActionDelay = 60.0f;
                Handlung = GEGNER_LAUFEN;
            }

            if (ShotDelay > 0.0f)
                ShotDelay -= 1.0f SYNC;

            if (ShotDelay <= 0.0f) {
                ShotDelay = 4.0f;

                SoundManager.PlayWave(100, 128, 10000 + random(500), SOUND_LASERSHOT);

                Projectiles.PushProjectile(xPos + 100 - 21, yPos + 60, UFOLASER);
                PartikelSystem.PushPartikel(xPos + 100 - 80, yPos + 30, UFOLASERFLARE);
            }
        } break;

        case GEGNER_FALLEN: {
            // Fertig mit abtauchen ?
            if (ySpeed < -28.0f) {
                yAcc = 0.0f;
                ySpeed = 0.0f;
                ActionDelay = 30.0f;
                Handlung = GEGNER_LAUFEN;
            }
        } break;

        // Ufo fliegt in die Luft
        case GEGNER_EXPLODIEREN: {
            Energy = 100.0f;
            AnimCount -= 1.0f SYNC;

            if (AnimCount < 0.0f) {
                PartikelSystem.PushPartikel(xPos + random(180), yPos + random(70) + 20, EXPLOSION_MEDIUM2);
                SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND_EXPLOSION1);

                if (random(4) == 0)
                    PartikelSystem.PushPartikel(xPos + random(180) - 40, yPos + random(40), EXPLOSION_BIG);

                AnimCount = 2.0f;
            }

            if (ySpeed > 20.0f)
                ySpeed = 20.0f;

            // Auf den Boden gecrasht ? Dann wird das Ufo ganz zerlegt
            if (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM) {
                yAcc = 0.0f;
                ySpeed = 0.0f;
                Energy = 0.0f;
            }
        } break;

        default:
            break;
    }  // switch

    // Hat das Ufo den Hurri berührt ?
    TestDamagePlayers(20.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Ufo explodiert
// --------------------------------------------------------------------------------------

void GegnerUfo::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 11025, SOUND_EXPLOSION2);
    ShakeScreen(5.0f);

    // Splitter
    for (int i = 0; i < 20; i++)
        PartikelSystem.PushPartikel(xPos + random(190), yPos + random(60) + 30, SPLITTER);

    Player[0].Score += 4000;
    Gegner.PushGegner(xPos + 80.0f, yPos + 20.0f, ONEUP, 0, 0, false);

    ScrolltoPlayeAfterBoss();
}
