// --------------------------------------------------------------------------------------
// Die riesige Stahlfaust (im Eislevel) - Zwischenboss
//
// fliegt über dem Spieler und versucht, diesen zu zerquetschen
// fliegt ab und zu nach oben aus dem Screen und donnert auf den Hurri runter
// saust von links nach rechts über den screen und lässt Eiszapfen fallen
// --------------------------------------------------------------------------------------

#include "Boss_EisFaust.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerEisFaust::GegnerEisFaust(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::NOTVISIBLE;
    BlickRichtung = DirectionEnum::LINKS;
    Energy = 7000;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimPhase = 0;
    ChangeLight = Light;
    AnimCount = 50.0f;  // Counter für Spezial Aktion
    Destroyable = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerEisFaust::DoKI() {
    // Energie anzeigen
    if (Handlung != GEGNER::NOTVISIBLE && Handlung != GEGNER::EXPLODIEREN)
        HUD.ShowBossHUD(7000, Energy);

    // Levelausschnitt auf die Faust zentrieren, sobald dieses sichtbar wird
    if (Active == true && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        TileEngine.ScrollLevel(static_cast<float>(Value1),
                               static_cast<float>(Value2),
                               TileStateEnum::SCROLLTOLOCK);  // Level auf die Faust zentrieren
        yPos -= 300;                                   // und Faust aus dem Screen setzen

        SoundManager.FadeSong(MUSIC::STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
    }

    // Zwischenboss blinkt nicht so lange wie die restlichen Gegner
    if (DamageTaken > 0.0f)
        DamageTaken -= Timer.sync(100.0f);  // Rotwerden langsam ausfaden lassen
    else
        DamageTaken = 0.0f;  // oder ganz anhalten

    // Schon schwer angeschlagen ? Dann raucht die Faust =)
    if (Energy < 2000 && random(2) == 0)
        PartikelSystem.PushPartikel(xPos + 20.0f + static_cast<float>(random(200)),
                                    yPos + 60.0f + static_cast<float>(random(200)), SMOKE);

    // Hat die Faust keine Energie mehr ? Dann explodiert sie
    if (Energy <= 100.0f && Handlung != GEGNER::EXPLODIEREN) {
        Handlung = GEGNER::EXPLODIEREN;
        xSpeed = 0.0f;
        ySpeed = 0.0f;
        xAcc = 0.0f;
        yAcc = 0.0f;
        AnimCount = 50.0f;

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC::BOSS, -2.0f, 0, false);
    }

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::NOTVISIBLE:  // Warten bis der Screen zentriert wurde
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

        case GEGNER::EINFLIEGEN2:  // Gegner kommt DAS ERSTE MAL in den Screen geflogen
        {
            Energy = 7000;
            DamageTaken = 0.0f;

            yPos += Timer.sync(8.0f);          // Faust nach unten bewegen
            if (yPos >= TileEngine.ScrolltoY)  // Weit genug unten ?
            {
                Handlung = GEGNER::LAUFEN;
                xAcc = -8.0f;
            }
        } break;

        case GEGNER::EINFLIEGEN:  // Gegner kommt in den Screen geflogen
        {
            yPos += Timer.sync(8.0f);          // Faust nach unten bewegen
            if (yPos >= TileEngine.ScrolltoY)  // Weit genug unten ?
            {
                Handlung = GEGNER::LAUFEN;
                xAcc = -8.0f;
            }
        } break;

        case GEGNER::LAUFEN:  // Über dem Spieler schweben und ggf runtersausen
        {
            AnimCount -= SpeedFaktor;

            // Rechts vom Spieler oder zu nahe am rechten Rand ?
            if (pAim->xpos + pAim->CollideRect.right < xPos || xPos > TileEngine.ScrolltoX + 480)
                xAcc = -8.0f;  // Dann nach Links fliegen

            // Links vom Spieler oder zu nahe am linken Rand ?
            if (pAim->xpos > xPos + GegnerRect[GegnerArt].right || xPos < TileEngine.ScrolltoX - 90)
                xAcc = 8.0f;  // Dann nach Rechts fliegen

            // Speed nicht zu hoch werde lassen
            if (xSpeed < -18.0f)
                xSpeed = -18.0f;
            if (xSpeed > 18.0f)
                xSpeed = 18.0f;

            // Spieler unter der Faust ? Dann crushen
            if (pAim->xpos < xPos + GegnerRect[GegnerArt].right - 115 &&
                pAim->xpos + pAim->CollideRect.right > xPos + 155 && random(2) == 0) {
                Handlung = GEGNER::CRUSHEN;
                ySpeed = 0.0f;
                yAcc = 10.0f;
                xSpeed = 0.0f;
                xAcc = 0.0f;
            }

            // Faust Spezial Aktion und oben rausfliegen ?
            if (AnimCount <= 0.0f) {
                AnimCount = 50.0f;  // Nächste Spezial Aktion planen
                Handlung = GEGNER::SPRINGEN;
                xAcc = 0.0f;
                xSpeed = 0.0f;
                ySpeed = -5.0f;
                yAcc = -5.0f;
            }

        } break;

        // Faust zerquetscht den Hurri
        case GEGNER::CRUSHEN: {
            // Auf den Boden gecrashed ?
            if (blocku & BLOCKWERT_WAND) {
                // Sound ausgeben
                SoundManager.PlayWave(100, 128, 6000, SOUND::EXPLOSION1);
                SoundManager.PlayWave(100, 128, 6000, SOUND::LANDEN);

                // Schnee am Boden erzeugen
                for (int i = 0; i < 80; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(200)),
                                                yPos + static_cast<float>(GegnerRect[GegnerArt].bottom - 40 + random(20)),
                                                WATERFLUSH2);

                // Beschleunigung und Geschwindigkeit wieder richtig setzen um hochzufliegen
                yAcc = -1.5f;
                ySpeed = 0.0f;

                // Screen Wackeln lassen
                ShakeScreen(2.5);

                Handlung = GEGNER::CRUSHENERHOLEN;
            }
        } break;

        // Faust fliegt nach dem Crushen wieder nach oben
        case GEGNER::CRUSHENERHOLEN: {
            // Nach dem nach oben fliegen wieder ganz oben ?
            if (ySpeed < 0.0f && yPos <= TileEngine.ScrolltoY) {
                Handlung = GEGNER::LAUFEN;
                ySpeed = 0.0f;
                yAcc = 0.0f;
                yPos = TileEngine.ScrolltoY;
                xAcc = -8.0f;
            }
        } break;

        // Faust fliegt oben raus
        case GEGNER::SPRINGEN: {
            // Oben umkehren neue Aktion machen?
            if (yPos <= TileEngine.ScrolltoY - 280.0f) {
                if (random(2) == 0) {
                    Handlung = GEGNER::FALLEN;
                    AnimPhase = 1;
                    yAcc = 0;
                    ySpeed = 50.0f;
                    xPos = pAim->xpos - 110;
                } else {
                    Handlung = GEGNER::BOMBARDIEREN;

                    xPos = static_cast<float>(Value1) - 250.0f;
                    yPos = static_cast<float>(Value2) - 200.0f;
                    yAcc = 0;
                    ySpeed = 10.0f;
                    yAcc = -0.4f;
                    xSpeed = 20.0f;
                    AnimCount = 8.0f;
                }
            }
        } break;

        // Faust fliegt von rechts nach links und wirft dabei Eiszapfen
        case GEGNER::BOMBARDIEREN: {
            if (xSpeed > 0.0f)
                BlickRichtung = DirectionEnum::RECHTS;
            else
                BlickRichtung = DirectionEnum::LINKS;

            AnimCount -= Timer.sync(1.0f);

            while (AnimCount <= 0.0f) {
                AnimCount += 4.0f;

                if (xPos + 100.0f > TileEngine.XOffset && xPos + 100.0f < TileEngine.XOffset + 620.0f &&
                    yPos + 220.0f > TileEngine.XOffset && yPos + 220.0f < TileEngine.XOffset + 440.0f)
                    Gegner.PushGegner(xPos + 100.0f, yPos + 220.0f, EISZAPFEN, 1, 0, false);
            }

            if (xSpeed > 0.0f && xPos > static_cast<float>(Value1 + RENDERWIDTH) + 30.0f) {
                xSpeed = -20.0f;
                ySpeed = 10.0f;
                yPos = static_cast<float>(Value2) - 100.0f;
            }

            if (xSpeed < 0.0f && xPos < static_cast<float>(Value1) - 300.0f) {
                xSpeed = 0.0f;
                AnimCount = 50.0f;

                AnimPhase = 0;  // Wieder Faust von der Seite
                Handlung = GEGNER::EINFLIEGEN;
                xSpeed = 0.0f;
                xAcc = 0.0f;
                ySpeed = 0.0f;
                yAcc = 0.0f;
                yPos = TileEngine.ScrolltoY - 400.0f;
                xPos = static_cast<float>(Value1) + 200.0f;
            }

        } break;

        // Faust fällt auf den Hurri drauf und fliegt dann wieder oben raus
        case GEGNER::FALLEN: {
            // Auf den Boden gecrashed ?
            if (blocku & BLOCKWERT_WAND) {
                // Sound ausgeben
                SoundManager.PlayWave(100, 128, 6000, SOUND::EXPLOSION1);
                SoundManager.PlayWave(100, 128, 6000, SOUND::LANDEN);

                // Schnee am Boden erzeugen
                for (int i = 0; i < 80; i++)
                    PartikelSystem.PushPartikel(xPos + 30.0f + static_cast<float>(random(180)),
                                                yPos + static_cast<float>(GegnerRect[GegnerArt].bottom - 40 + random(20)),
                                                                          WATERFLUSH2);

                // Beschleunigung und Geschwindigkeit wieder richtig setzen um hochzufliegen
                yAcc = -0.5f;
                ySpeed = 0.0f;

                // Screen Wackeln lassen
                ShakeScreen(5);

                Handlung = GEGNER::STEHEN;
            }
        } break;

        // Faust fliegt nach dem Crushen wieder nach oben raus
        case GEGNER::STEHEN: {
            // Nach dem nach oben fliegen wieder ganz oben ?
            if (yPos <= TileEngine.ScrolltoY - 280.0f) {
                AnimPhase = 0;  // Wieder Faust von der Seite
                Handlung = GEGNER::EINFLIEGEN;
                ySpeed = 0.0f;
                yAcc = 0.0f;
                yPos = TileEngine.ScrolltoY - 250.0f;
            }
        } break;

        // Faust fliegt in die Luft
        case GEGNER::EXPLODIEREN: {
            AnimCount -= SpeedFaktor;

            Energy = 100.0f;

            if (random(5) == 0) {
                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(200)),
                                            yPos + 20.0f + static_cast<float>(random(200)), EXPLOSION_MEDIUM2);
                SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);
            }

            if (random(8) == 0)
                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(200)),
                                            yPos + 20.0f + static_cast<float>(random(200)), EXPLOSION_BIG);

            if (random(20) == 0)
                PartikelSystem.PushPartikel(xPos + 60.0f + static_cast<float>(random(100)),
                                            yPos + 60.0f + static_cast<float>(random(100)), SPLITTER);

            // Fertig explodiert ? Dann wird sie ganz zerlegt
            if (AnimCount <= 0.0f)
                Energy = 0.0f;
        } break;

        default:
            break;
    }  // switch

    // Hat die Faust den Hurri getroffen ?
    if (Handlung != GEGNER::EXPLODIEREN)
        TestDamagePlayers(Timer.sync(5.0f));
}

// --------------------------------------------------------------------------------------
// EisFaust explodiert
// --------------------------------------------------------------------------------------

void GegnerEisFaust::GegnerExplode() {
    // Splitter
    for (int i = 0; i < 20; i++)
        PartikelSystem.PushPartikel(xPos + 60.0f + static_cast<float>(random(60)),
                                    yPos + 80.0f + static_cast<float>(random(40)), SPLITTER);

    // DKS - In the course of optimizing PartikelsystemClass, I discovered that
    //      SPIDERSPLITTER2 was not handled in CreatePartikel(), and that
    //      the SPIDERSPLITTER2 particles here were never being seen in the
    //      original code. When this was corrected and they were visible, they
    //      remained stationary in position since their acceleration variables
    //      were never being initialized. Since here and Boss_EisFaust.cpp are
    //      the only places SPIDERSPLITTER2 particles were ever being created,
    //      and since 50% of SPIDERSPLITTER particles become SPIDERSPLITTER2
    //      artwork, I've replaced SPIDERSPLITTER2 here with SPIDERSPLITTER:
    for (int i = 0; i < 60; i++) {
        // PartikelSystem.PushPartikel(xPos + 20 + rand()%100,
        //                              yPos + 40 + rand()%100, SPIDERSPLITTER2);
        PartikelSystem.PushPartikel(xPos + 20.0f + static_cast<float>(random(100)),
                                    yPos + 40.0f + static_cast<float>(random(100)), SPIDERSPLITTER);

        PartikelSystem.PushPartikel(xPos + 60.0f + static_cast<float>(random(60)),
                                    yPos + 80.0f + static_cast<float>(random(40)), WATERFLUSH2);
    }

    Player[0].Score += 5000;

    ScrolltoPlayeAfterBoss();
}
