// --------------------------------------------------------------------------------------
// T.R.Schmidts Gegnerorgie im Cave Level
//
// Gegnermassen hopsen von der Seite rein
// --------------------------------------------------------------------------------------

#include "Boss_SchmidtOrgie.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchmidtOrgie::GegnerSchmidtOrgie(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::VERFOLGEN;
    BlickRichtung = DirectionEnum::LINKS;
    Energy = 4500;
    ChangeLight = Light;
    Destroyable = false;
    Value1 = Wert1;
    Value2 = Wert2;
    ActionCounter = 1.0f;
    DontMove = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSchmidtOrgie::DoKI() {
    // Linseneffekt ausfaden
    if (Handlung != GEGNER::EXPLODIEREN) {
        if (ShadowAlpha > 0.0f)
            ShadowAlpha -= Timer.sync(5.0f);
        else
            ShadowAlpha = 0.0f;
    } else {
        if (ShadowAlpha < 255.0f)
            ShadowAlpha += Timer.sync(5.0f);
        else
            ShadowAlpha = 255.0f;
    }

    // Energie anzeigen
    if (Handlung != GEGNER::INIT && Handlung != GEGNER::VERFOLGEN && Handlung != GEGNER::EXPLODIEREN)
        HUD.ShowBossHUD(4500, Energy);

    // Boss aktivieren und Mucke laufen lassen
    //
    if (Active == true && Handlung != GEGNER::VERFOLGEN && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        TileEngine.ScrollLevel(static_cast<float>(Value1), static_cast<float>(Value2),
                               TileStateEnum::SCROLLTOLOCK);  // Level auf den Boss zentrieren

        SoundManager.FadeSong(MUSIC::STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
        Gegner.PushGegner(xPos, yPos - 40, EXTRAS, 10, 0, false);
    }

    // Hat der Boss keine Energie mehr ? Dann explodiert er
    if (Energy <= 100.0f && Handlung != GEGNER::EXPLODIEREN) {
        Handlung = GEGNER::EXPLODIEREN;

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC::BOSS, -2.0f, 0, false);
    }

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::INIT:  // Warten bis der Screen zentriert wurde
        {
            if (TileEngine.Zustand == TileStateEnum::LOCKED) {
                // Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
                //
                // DKS - Added function SongIsPlaying() to SoundManagerClass:
                if (!SoundManager.SongIsPlaying(MUSIC::BOSS)) {
                    SoundManager.PlaySong(MUSIC::BOSS, false);

                    // Und Boss erscheinen lassen
                    //
                    Handlung = GEGNER::STEHEN;
                }
            }
        } break;

        case GEGNER::STEHEN: {
            // ActionCounter runterzählen
            // bei null Gegner spawnen
            ActionCounter -= Timer.sync(1.5f);

            float x;
            if (random(2) == 0)
                x = -40.0f;
            else
                x = 620.0f;

            if (ActionCounter <= 0.0f) {
                ActionCounter = 1.2f + Energy / 600.0f;

                // Gegner per Zufall wählen und spawnen
                //
                int spawn = random(2);

                if (spawn == 0)
                    Gegner.PushGegner(static_cast<float>(Value1) + x, static_cast<float>(Value2 + 50 + random(200)), FLEDERMAUS, 0, 1, false);

                if (spawn == 1)
                    Gegner.PushGegner(static_cast<float>(Value1) + x, static_cast<float>(Value2 + 200 + random(50)), SCHLEIMBOLLER, 24, 0,
                                      false);

                Energy -= 10;

                // ab und zu ein Paar Augen einstreuen
                //
                if (static_cast<int>(Energy) % 1000 == 0) {
                    Gegner.PushGegner(static_cast<float>(Value1) - 20, static_cast<float>(Value2 + 130), AUGE, random(10), 0, false);
                    Gegner.PushGegner(static_cast<float>(Value1) + 620, static_cast<float>(Value2 + 200), AUGE, random(10), 0, false);
                }

                // oder eine Stachelkugel, einen Firewalker oder ein großer Schleimboller;)
                //
                if (static_cast<int>(Energy) % 500 == 0) {
                    int j = random(3);

                    switch (j) {
                        case 0:
                            Gegner.PushGegner(static_cast<float>(Value1) + x,
                                              static_cast<float>(Value2) + 100.0f, STACHELBEERE, 0, 0, false);
                            break;

                        case 1:
                            Gegner.PushGegner(static_cast<float>(Value1) + x,
                                              static_cast<float>(Value2) + 100.0f, SCHLEIMBOLLER, 46, 0, false);
                            break;

                        case 2:
                            Gegner.PushGegner(static_cast<float>(Value1) + x,
                                              static_cast<float>(Value2) + 250.0f, FIESERWALKER2, 0, 0, false);
                            break;
                    }
                }
            }
        } break;

        // warten, bis der Spieler den Gegner berührt hat, und dann aktivieren
        //
        case GEGNER::VERFOLGEN: {
            if (PlayerAbstand() < 600 && pAim->xpos + 80 > xPos)

                Handlung = GEGNER::INIT;

        } break;

        case GEGNER::EXPLODIEREN: {
            Energy = 0.0f;
        } break;

        default:
            break;
    }  // switch
}

// --------------------------------------------------------------------------------------
// SchmidtOrgie explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerSchmidtOrgie::GegnerExplode() {
    // Extra Leben
    Gegner.PushGegner(xPos, yPos - 250, ONEUP, 10, 0, false);

    ScrolltoPlayeAfterBoss();
}
