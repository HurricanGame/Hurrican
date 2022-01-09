// --------------------------------------------------------------------------------------
// Die riesige Stahlfaust - Zwischenboss
//
// fliegt über dem Spieler und versucht, diesen zu zerquetschen
// fliegt ab und zu nach oben aus dem Screen und donnert auf den Hurri runter
// --------------------------------------------------------------------------------------

#include "Boss_Stahlfaust.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerStahlfaust::GegnerStahlfaust(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_NOTVISIBLE;
    BlickRichtung = LINKS;
    Energy = 4000;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimPhase = 0;
    ChangeLight = Light;
    AnimCount = 70.0f;  // Counter für Spezial Aktion
    Destroyable = true;
    TestBlock = false;
    SmokeDelay = 0.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerStahlfaust::DoKI() {
    // Energie anzeigen
    if (Handlung != GEGNER_INIT && Handlung != GEGNER_EXPLODIEREN)
        HUD.ShowBossHUD(4000, Energy);

    // Levelausschnitt auf die Faust zentrieren, sobald dieses sichtbar wird
    if (Active == true && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        TileEngine.ScrollLevel(static_cast<float>(Value1), static_cast<float>(Value2),
                               TileStateEnum::SCROLLTOLOCK);  // Level auf die Faust zentrieren
        yPos -= 300;                                   // und Faust aus dem Screen setzen
        Handlung = GEGNER_INIT;

        SoundManager.FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
    }

    // Zwischenboss blinkt nicht so lange wie die restlichen Gegner
    if (DamageTaken > 0.0f)
        DamageTaken -= 100 SYNC;  // Rotwerden langsam ausfaden lassen
    else
        DamageTaken = 0.0f;  // oder ganz anhalten

    // Schon schwer angeschlagen ? Dann raucht die Faust =)
    if (Energy < 2000 && random(2) == 0)
        PartikelSystem.PushPartikel(xPos + random(200) + 20, yPos + random(200) + 60, SMOKE);

    // Hat die Faust keine Energie mehr ? Dann explodiert sie
    if (Energy <= 100.0f && Handlung != GEGNER_EXPLODIEREN) {
        Handlung = GEGNER_EXPLODIEREN;
        xSpeed = 0.0f;
        ySpeed = 0.0f;
        xAcc = 0.0f;
        yAcc = 0.0f;
        AnimCount = 50.0f;
        SmokeDelay = 1.0f;

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC_BOSS, -2.0f, 0, false);
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
                Handlung = GEGNER_EINFLIEGEN2;
            }
        } break;

        case GEGNER_EINFLIEGEN2:  // Gegner kommt DAS ERSTE MAL in den Screen geflogen
        {
            Energy = 4000;
            DamageTaken = 0.0f;

            yPos += static_cast<float>(8.0 SYNC);           // Faust nach unten bewegen
            if (yPos >= TileEngine.ScrolltoY)  // Weit genug unten ?
            {
                Handlung = GEGNER_LAUFEN;
                xAcc = -8.0f;
            }
        } break;

        case GEGNER_EINFLIEGEN:  // Gegner kommt in den Screen geflogen
        {
            yPos += static_cast<float>(8.0 SYNC);           // Faust nach unten bewegen
            if (yPos >= TileEngine.ScrolltoY)  // Weit genug unten ?
            {
                Handlung = GEGNER_LAUFEN;
                xAcc = -8.0f;
            }
        } break;

        case GEGNER_LAUFEN:  // Über dem Spieler schweben und ggf runtersausen
        {
            AnimCount -= SpeedFaktor;

            // Rechts vom Spieler oder zu nahe am rechten Rand ?
            if (pAim->xpos + pAim->CollideRect.right < xPos || xPos > TileEngine.ScrolltoX + 480)
                xAcc = -8.0f;  // Dann nach Links fliegen

            // Links vom Spieler oder zu nahe am linken Rand ?
            if (pAim->xpos > xPos + GegnerRect[GegnerArt].right || xPos < TileEngine.ScrolltoX - 90)
                xAcc = 8.0f;  // Dann nach Rechts fliegen

            // Speed nicht zu hoch werde lassen
            xSpeed = std::clamp(xSpeed, -18.0f, 18.0f);

            // Spieler unter der Faust ? Dann crushen
            if (pAim->xpos < xPos + GegnerRect[GegnerArt].right - 115 &&
                pAim->xpos + pAim->CollideRect.right > xPos + 155 && random(2) == 0) {
                Handlung = GEGNER_CRUSHEN;
                ySpeed = 0.0f;
                yAcc = 10.0f;
                xSpeed = 0.0f;
                xAcc = 0.0f;
            }

            // Faust Spezial Aktion und oben rausfliegen ?
            if (AnimCount <= 0.0f) {
                AnimCount = 70.0f;  // Nächste Spezial Aktion planen
                Handlung = GEGNER_SPRINGEN;
                xAcc = 0.0f;
                xSpeed = 0.0f;
                ySpeed = -5.0f;
                yAcc = -5.0f;
            }

        } break;

        // Faust zerquetscht den Hurri
        case GEGNER_CRUSHEN: {
            blocku = TileEngine.BlockUnten(xPos, yPos, xPos, yPos, GegnerRect[GegnerArt]);

            // Auf den Boden gecrashed ?
            if (blocku & BLOCKWERT_WAND) {
                // Sound ausgeben
                SoundManager.PlayWave(100, 128, 6000, SOUND_EXPLOSION1);
                SoundManager.PlayWave(100, 128, 6000, SOUND_LANDEN);

                // Rauch am Boden erzeugen
                for (int i = 0; i < 25; i++)
                    PartikelSystem.PushPartikel(xPos + random(200), yPos + GegnerRect[GegnerArt].bottom - 20, SMOKE);

                // Beschleunigung und Geschwindigkeit wieder richtig setzen um hochzufliegen
                yAcc = -1.5f;
                ySpeed = 0.0f;

                // Screen Wackeln lassen
                ShakeScreen(2.5);

                Handlung = GEGNER_CRUSHENERHOLEN;
            }
        } break;

        // Faust fliegt nach dem Crushen wieder nach oben
        case GEGNER_CRUSHENERHOLEN: {
            // Nach dem nach oben fliegen wieder ganz oben ?
            if (ySpeed < 0.0f && yPos <= TileEngine.ScrolltoY) {
                Handlung = GEGNER_LAUFEN;
                ySpeed = 0.0f;
                yAcc = 0.0f;
                yPos = TileEngine.ScrolltoY;
                xAcc = -8.0f;
            }
        } break;

        // Faust fliegt oben raus
        case GEGNER_SPRINGEN: {
            // Oben umkehren ?
            if (yPos <= TileEngine.ScrolltoY - 280.0f) {
                Handlung = GEGNER_FALLEN;
                AnimPhase = 1;
                yAcc = 0;
                ySpeed = 50.0f;
                xPos = pAim->xpos - 110;
            }
        } break;

        // Faust fällt auf den Hurri drauf und fliegt dann wieder oben raus
        case GEGNER_FALLEN: {
            blocku = TileEngine.BlockUnten(xPos, yPos, xPos, yPos, GegnerRect[GegnerArt]);

            // Auf den Boden gecrashed ?
            if (blocku & BLOCKWERT_WAND) {
                // Sound ausgeben
                SoundManager.PlayWave(100, 128, 6000, SOUND_EXPLOSION1);
                SoundManager.PlayWave(100, 128, 6000, SOUND_LANDEN);

                // Rauch am Boden erzeugen
                for (int i = 0; i < 25; i++)
                    PartikelSystem.PushPartikel(xPos + 30 + random(180), yPos + GegnerRect[GegnerArt].bottom - 20,
                                                SMOKE);

                // Beschleunigung und Geschwindigkeit wieder richtig setzen um hochzufliegen
                yAcc = -0.5f;
                ySpeed = 0.0f;

                // Screen Wackeln lassen
                ShakeScreen(5);

                Handlung = GEGNER_STEHEN;

                // Spieler auch als Rad verwunden
                for (int i = 0; i < NUMPLAYERS; i++)
                    if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], Player[i].xpos, Player[i].ypos,
                                        Player[i].CollideRect))
                        Player[i].DamagePlayer(100.0f, true);
            }
        } break;

        // Faust fliegt nach dem Crushen wieder nach oben raus
        case GEGNER_STEHEN: {
            // Nach dem nach oben fliegen wieder ganz oben ?
            if (yPos <= TileEngine.ScrolltoY - 280.0f) {
                AnimPhase = 0;  // Wieder Faust von der Seite
                Handlung = GEGNER_EINFLIEGEN;
                ySpeed = 0.0f;
                yAcc = 0.0f;
                yPos = TileEngine.ScrolltoY - 250.0f;
            }
        } break;

        // Faust fliegt in die Luft
        case GEGNER_EXPLODIEREN: {
            AnimCount -= 1.0f SYNC;
            SmokeDelay -= 1.0f SYNC;

            Energy = 100.0f;

            if (SmokeDelay < 0.0f) {
                SmokeDelay = 1.0f;

                PartikelSystem.PushPartikel(xPos + random(200), yPos + random(200) + 20, EXPLOSION_MEDIUM2);
                SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND_EXPLOSION1);

                if (random(8) == 0)
                    PartikelSystem.PushPartikel(xPos + random(200), yPos + random(200) + 20, EXPLOSION_BIG);

                if (random(20) == 0)
                    PartikelSystem.PushPartikel(xPos + random(100) + 60, yPos + random(100) + 60, SPLITTER);
            }

            // Fertig explodiert ? Dann wird sie ganz zerlegt
            if (AnimCount <= 0.0f)
                Energy = 0.0f;
        } break;

        default:
            break;
    }  // switch

    // Hat die Faust den Hurri getroffen ?
    if (Handlung != GEGNER_EXPLODIEREN)
        TestDamagePlayers(5.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Stahlfaust explodiert
// --------------------------------------------------------------------------------------

void GegnerStahlfaust::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 11025, SOUND_EXPLOSION2);

    // Splitter
    for (int i = 0; i < 20; i++)
        PartikelSystem.PushPartikel(xPos + 60 + random(60), yPos + 80 + random(40), SPLITTER);

    // DKS - In the course of optimizing PartikelsystemClass, I discovered that
    //      SPIDERSPLITTER2 was not handled in CreatePartikel(), and that
    //      the SPIDERSPLITTER2 particles here were never being seen in the
    //      original code. When this was corrected and they were visible, they
    //      remained stationary in position since their acceleration variables
    //      were never being initialized. Since here and Boss_EisFaust.cpp are
    //      the only places SPIDERSPLITTER2 particles were ever being created,
    //      and since 50% of SPIDERSPLITTER particles become SPIDERSPLITTER2
    //      artwork, I've replaced SPIDERSPLITTER2 here with SPIDERSPLITTER:

    for (int i = 0; i < 60; i++)
        // PartikelSystem.PushPartikel(xPos + 20 + rand()%100,
        //                              yPos + 40 + rand()%100, SPIDERSPLITTER2);
        PartikelSystem.PushPartikel(xPos + 20 + random(100), yPos + 40 + random(100), SPIDERSPLITTER);

    for (int p = 0; p < NUMPLAYERS; p++)
        DirectInput.Joysticks[Player[p].JoystickIndex].ForceFeedbackEffect(FFE_BIGRUMBLE);

    ShakeScreen(4);

    Player[0].Score += 5000;

    ScrolltoPlayeAfterBoss();
}
