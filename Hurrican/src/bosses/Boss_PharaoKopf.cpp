// --------------------------------------------------------------------------------------
// Die Pharao-Kopf Endgegner
//
// rammt seitlich gegen die Wände und versucht den Hurri zu zerquetschen
// lässt ggf Steine von der Decke rieseln
// --------------------------------------------------------------------------------------

#include "Boss_PharaoKopf.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPharaoKopf::GegnerPharaoKopf(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::NOTVISIBLE;
    BlickRichtung = LINKS;

    Energy = 6000;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimPhase = 0;
    ChangeLight = Light;
    AnimCount = 100.0f;  // Counter für Spezial Aktion
    Destroyable = true;
    TestBlock = false;
}

bool GegnerPharaoKopf::Links() {
    int const limit = Value1 + BORDER;
    if (static_cast<int>(xPos) < limit) {
        xPos = static_cast<float>(limit);
        return true;
    }

    return false;
}

bool GegnerPharaoKopf::Rechts() {
    int const limit = Value1 + 652 - BORDER;
    if (static_cast<int>(xPos) + GegnerRect[GegnerArt].right > limit) {
        xPos = static_cast<float>(limit - GegnerRect[GegnerArt].right);
        return true;
    }

    return false;
}

bool GegnerPharaoKopf::Unten() {
    int const limit = Value2 + 480 - BORDER2;
    if (static_cast<int>(yPos) + GegnerRect[GegnerArt].bottom > limit) {
        yPos = static_cast<float>(limit - GegnerRect[GegnerArt].bottom);
        return true;
    }

    return false;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerPharaoKopf::DoKI() {
    // Energie anzeigen
    if (Handlung != GEGNER::NOTVISIBLE && Handlung != GEGNER::EXPLODIEREN)
        HUD.ShowBossHUD(6000, Energy);

    //----- Pharao dient auch als Plattform
    PlattformTest(GegnerRect[GegnerArt]);

    //----- Pharao Bewegungs Code

    // Levelausschnitt auf den PharaoKopf zentrieren, sobald dieser sichtbar wird
    if (Active == true && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        TileEngine.ScrollLevel(static_cast<float>(Value1), static_cast<float>(Value2),
                               TileStateEnum::SCROLLTOLOCK);  // Level auf die Faust zentrieren

        SoundManager.FadeSong(MUSIC::STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
    }

    // Zwischenboss blinkt nicht so lange wie die restlichen Gegner
    if (DamageTaken > 0.0f)
        DamageTaken -= Timer.sync(100.0f);  // Rotwerden langsam ausfaden lassen
    else
        DamageTaken = 0.0f;  // oder ganz anhalten

    // Hat der PharaoKopf keine Energie mehr ? Dann explodiert er
    if (Energy <= 100.0f && Handlung != GEGNER::EXPLODIEREN) {
        Handlung = GEGNER::EXPLODIEREN;
        xSpeed = 0.0f;
        ySpeed = 0.0f;
        xAcc = 0.0f;
        yAcc = 0.0f;
        AnimCount = 20.0f;

        SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION1);
        SoundManager.PlayWave(100, 128, 11025, SOUND::PHARAODIE);

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC::BOSS, -2.0f, 0, false);
    }

    // Kopf zieht dem Spieler Energie an wenn er ihn berührt
    TestDamagePlayers(Timer.sync(2.0f));

    if (Energy >= 3000 && AnimPhase > 1)
        AnimPhase = 0;

    // Hälfte der Energie verloren ? Dann Risse Zeigen und kurz Brocken fliegen lassen
    if (Energy < 3000 && AnimPhase < 2) {
        AnimPhase = 2;

        // Brocken
        for (int i = 0; i < 100; i++) {
            PartikelSystem.PushPartikel(xPos + static_cast<float>(random(224)),
                                        yPos + static_cast<float>(random(224)), ROCKSPLITTER);
            PartikelSystem.PushPartikel(xPos + static_cast<float>(random(224)),
                                        yPos + static_cast<float>(random(224)), ROCKSPLITTERSMALL);
        }

        SoundManager.PlayWave(100, 128, 11025, SOUND::PHARAORAMM);
        SoundManager.PlayWave(100, 128, 11025, SOUND::PHARAODIE);
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

                AnimCount = 1.0f;
            }
        } break;

        case GEGNER::EINFLIEGEN:  // Gegner kommt aus dem Boden raus
        {
            Energy = 6000;
            DamageTaken = 0.0f;

            yPos -= Timer.sync(4.5f);  // Pharao erscheint langsam

            AnimCount -= Timer.sync(1.0f);

            // Boden brodeln lassen wo er auftaucht
            if (AnimCount <= 0.0f) {
                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(230) - 30),
                                            static_cast<float>(Value2 + 400 + random(12)),
                                            SMOKEBIG);
                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(230) - 12),
                                            static_cast<float>(Value2 + 405 + random(12)),
                                            ROCKSPLITTER);
                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(230) - 12),
                                            static_cast<float>(Value2 + 410 + random(12)),
                                            ROCKSPLITTERSMALL);

                AnimCount += 0.2f;
            }

            float const limit = static_cast<float>(Value2 + 480 - BORDER2 - GegnerRect[GegnerArt].bottom);
            if (yPos <= limit)  // Weit genug unten ?
            {
                for (int p = 0; p < NUMPLAYERS; p++)
                    if (Player[p].AufPlattform == this) {
                        ySpeed = -10.0f;
                        yAcc = -3.0f;
                        goto _weiter;
                    }

                if (pAim->xpos < xPos) {
                    xSpeed = -1.0f;
                    xAcc = -3.0f;
                } else {
                    xSpeed = 1.0f;
                    xAcc = 3.0f;
                }
            _weiter:
                yPos = limit;
                SoundManager.PlayWave(100, 128, 8000, SOUND::STONEFALL);

                Handlung = GEGNER::LAUFEN;
            }
        } break;

        case GEGNER::LAUFEN:  // In Richtung Spieler rutschen
        {
            xSpeed = 0.0f;
            ySpeed = 0.0f;

            // Spieler steht auf dem Pharao Kopf ?
            for (int p = 0; p < NUMPLAYERS; p++)
                if (Player[p].AufPlattform == this) {
                    ySpeed = -10.0f;
                    yAcc = -3.0f;
                    goto _weiter2;
                }

                // Spieler ist am Boden
                else {
                    if (pAim->xpos < xPos) {
                        xSpeed = -1.0f;
                        xAcc = -3.0f;
                    } else {
                        xSpeed = 1.0f;
                        xAcc = 3.0f;
                    }

                    for (p = 0; p < NUMPLAYERS; p++)
                        if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], Player[p].xpos, Player[p].ypos,
                                            Player[p].CollideRect) == true) {
                            xSpeed = -xSpeed;
                            xAcc = -xAcc;
                        }
                }
        _weiter2:

            Handlung = GEGNER::CRUSHEN;
        } break;

        case GEGNER::CRUSHEN:  // An die Wand klatschen und an der Wand anhalten =)
        {
            // Partikel am Boden
            if (ySpeed == 0)
                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(224)),
                                            yPos + 210.0f, ROCKSPLITTERSMALL);

            // Links an die Wand rutschen ?
            if (xSpeed < 0) {
                // Hurri verschieben
                for (int p = 0; p < NUMPLAYERS; p++)
                    if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], Player[p].xpos, Player[p].ypos,
                                        Player[p].CollideRect) == true &&
                        !(TileEngine.BlockLinks(Player[p].xpos, Player[p].ypos, Player[p].xposold, Player[p].yposold,
                                                Player[p].CollideRect) &
                          BLOCKWERT_WAND))
                        Player[p].xpos += Timer.sync(xSpeed);

                // an die Wand gekommen?
                if (Links()) {
                    xSpeed = 0.0f;
                    xAcc = 0.0f;

                    for (int p = 0; p < NUMPLAYERS; p++)
                        DirectInput.Joysticks[Player[p].JoystickIndex].ForceFeedbackEffect(FFE_MEDIUMRUMBLE);

                    for (int i = 0; i < 20; i++) {
                        PartikelSystem.PushPartikel(xPos - 10.0f,
                                                    yPos - 30.0f + static_cast<float>(random(224)), SMOKEBIG);
                        PartikelSystem.PushPartikel(xPos - 10.0f,
                                                    yPos + static_cast<float>(random(224)), ROCKSPLITTER);
                        PartikelSystem.PushPartikel(xPos - 10.0f,
                                                    yPos + static_cast<float>(random(224)), ROCKSPLITTERSMALL);
                    }

                    SoundManager.PlayWave(100, 128, 11025, SOUND::PHARAORAMM);
                    Handlung = GEGNER::CRUSHENERHOLEN;
                    AnimCount = 10.0f;

                    // Hurri dabei erwischt ?
                    TestDamagePlayers(1000.0f);

                    // Screen Wackeln lassen
                    ShakeScreen(3);
                }
            }

            // Rechts an die Wand rutschen ?
            if (xSpeed > 0) {
                // Hurri verschieben
                for (int p = 0; p < NUMPLAYERS; p++)
                    if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], Player[p].xpos, Player[p].ypos,
                                        Player[p].CollideRect) == true &&
                        !(blockr & BLOCKWERT_WAND))
                        Player[p].xpos += Timer.sync(xSpeed);

                // an die Wand gekommen?
                if (Rechts()) {
                    for (int p = 0; p < NUMPLAYERS; p++)
                        DirectInput.Joysticks[Player[p].JoystickIndex].ForceFeedbackEffect(FFE_MEDIUMRUMBLE);

                    xSpeed = 0.0f;
                    xAcc = 0.0f;
                    for (int i = 0; i < 20; i++) {
                        PartikelSystem.PushPartikel(xPos + 175.0f,
                                                    yPos - 30.0f + static_cast<float>(random(224)), SMOKEBIG);
                        PartikelSystem.PushPartikel(xPos + 212.0f,
                                                    yPos + static_cast<float>(random(224)), ROCKSPLITTER);
                        PartikelSystem.PushPartikel(xPos + 212.0f,
                                                    yPos + static_cast<float>(random(224)), ROCKSPLITTERSMALL);
                    }

                    SoundManager.PlayWave(100, 128, 11025, SOUND::PHARAORAMM);
                    Handlung = GEGNER::CRUSHENERHOLEN;
                    AnimCount = 10.0f;

                    // Hurri dabei erwischt ?
                    TestDamagePlayers(1000.0f);

                    // Screen Wackeln lassen
                    ShakeScreen(3);
                }
            }

            // An die Decke gekommen ?
            if (ySpeed < 0) {
                if (yPos < static_cast<float>(Value2)) {
                    for (int i = 0; i < 20; i++) {
                        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(224)),
                                                    yPos, SMOKEBIG);
                        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(224)),
                                                    yPos + 10.0f, ROCKSPLITTER);
                        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(224)),
                                                    yPos + 10.0f, ROCKSPLITTERSMALL);
                    }

                    SoundManager.PlayWave(100, 128, 11025, SOUND::PHARAORAMM);
                    yPos = static_cast<float>(Value2);
                    ySpeed = 5.0f;
                    yAcc = 10.0f;

                    for (int p = 0; p < NUMPLAYERS; p++) {
                        if (Player[p].AufPlattform == this)
                            Player[p].DamagePlayer(200.0f);

                        Player[p].AufPlattform = nullptr;
                    }

                    // Screen Wackeln lassen
                    ShakeScreen(3);
                }
            } else

                // An den Boden gekommen ?
                if (ySpeed > 0.0f) {
                // Hurri dabei erwischt ?
                for (int p = 0; p < NUMPLAYERS; p++)
                    if (Player[p].Energy > 0 && Player[p].AufPlattform == nullptr &&
                        SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], Player[p].xpos, Player[p].ypos,
                                        Player[p].CollideRect) == true) {
                        Player[p].DamagePlayer(25.0f);

                        // Wieder hoch fliegen
                        ySpeed = -10.0f;
                        yAcc = -3.0f;
                    }

                if (Unten()) {
                    ySpeed = 0.0f;
                    yAcc = 0.0f;
                    for (int i = 0; i < 20; i++) {
                        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(224)),
                                                    yPos + 170.0f, SMOKEBIG);
                        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(224)),
                                                    yPos + 220.0f, ROCKSPLITTER);
                        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(224)),
                                                    yPos + 220.0f, ROCKSPLITTERSMALL);
                    }

                    SoundManager.PlayWave(100, 128, 11025, SOUND::PHARAORAMM);
                    TileEngine.BlockUnten(xPos, yPos, xPos, yPos, GegnerRect[GegnerArt]);
                    AnimCount = 20.0f;
                    Handlung = GEGNER::CRUSHENERHOLEN;
                }
                // Screen Wackeln lassen
                ShakeScreen(3);
            }
        } break;

        case GEGNER::CRUSHENERHOLEN: {
            // ErholungsCounter runterzählen
            AnimCount -= Timer.sync(1.0f);

            if (AnimCount <= 0.0f) {
                int j = random(3);

                // Schiessen ?
                if (j == 0 && AnimPhase > 1) {
                    Handlung = GEGNER::SCHIESSEN;
                    AnimPhase = 3;
                    AnimCount = 8.0f;
                } else
                    j = random(2) + 1;

                // Steine von der Decke rieseln lassen ?
                if (j == 1) {
                    // Springen
                    ySpeed = -10.0f;
                    yAcc = 2.0f;

                    // böse kucken
                    if (AnimPhase == 0)
                        AnimPhase = 1;
                    else
                        AnimPhase = 3;

                    Handlung = GEGNER::SPECIAL;
                    TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
                }

                // Wieder zerquetschen
                bool PlayerOn = false;
                for (int p = 0; p < NUMPLAYERS; p++)
                    if (Player[p].AufPlattform == this)
                        PlayerOn = true;

                if (j == 2 || PlayerOn) {
                    Handlung = GEGNER::LAUFEN;

                    if (j == 2)
                        SoundManager.PlayWave(100, 128, 8000, SOUND::STONEFALL);
                }
            }
        } break;

        case GEGNER::SPECIAL:  // Boss hopft auf und ab und lässt Steine von der Decke rieseln
        {
            if (Unten() && ySpeed > 0.0f) {
                // Stein erscheinen lassen
                Gegner.PushGegner(pAim->xpos, static_cast<float>(Value2), FALLINGROCK, 0, 0, false);

                // Partikel unter Boss
                for (int i = 0; i < 20; i++) {
                    PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(224)),
                                                yPos + 170.0f, SMOKEBIG);
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(224)),
                                                yPos + 220.0f, ROCKSPLITTER);
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(224)),
                                                yPos + 220.0f, ROCKSPLITTERSMALL);
                }

                SoundManager.PlayWave(100, 128, 11025, SOUND::PHARAORAMM);

                ySpeed = 0.0f;
                yAcc = 0.0f;
                AnimCount = 10.0f;
                Handlung = GEGNER::CRUSHENERHOLEN;

                // Normal kucken
                if (AnimPhase == 1)
                    AnimPhase = 0;
                else
                    AnimPhase = 2;
            }
        } break;

        case GEGNER::SCHIESSEN:  // Gegner schiesst aus den Augen
        {
            AnimCount -= Timer.sync(1.0f);

            if (AnimCount <= 0) {
                // Augen leuchten lassen
                PartikelSystem.PushPartikel(xPos + 68.0f, yPos + 77.0f, LASERFLAMEPHARAO);
                PartikelSystem.PushPartikel(xPos + 68.0f, yPos + 77.0f, LASERFLAMEPHARAO);
                PartikelSystem.PushPartikel(xPos + 112.0f, yPos + 77.0f, LASERFLAMEPHARAO);
                PartikelSystem.PushPartikel(xPos + 112.0f, yPos + 77.0f, LASERFLAMEPHARAO);

                // Schüsse erzeugen
                Projectiles.PushProjectile(xPos + 68.0f, yPos + 77.0f, PHARAOLASER, pAim);
                Projectiles.PushProjectile(xPos + 112.0f, yPos + 77.0f, PHARAOLASER, pAim);

                // Sound ausgeben
                SoundManager.PlayWave(50, 128, 22050, SOUND::PHARAODIE);
                SoundManager.PlayWave(70, 128, 11025, SOUND::LASERSHOT);

                // Je nach SchwierigkeitsGrad die Schuss Frequenz setzen
                switch (Skill) {
                    case SKILL_EASY:     AnimCount = 10.0f; break;
                    case SKILL_MEDIUM:   AnimCount =  8.5f; break;
                    case SKILL_HARD:     AnimCount =  7.0f; break;
                    case SKILL_HURRICAN: AnimCount =  6.0f; break;
                }

                if (random(2) == 0) {
                    AnimPhase = 2;
                    Handlung = GEGNER::CRUSHENERHOLEN;
                }
            }
        } break;

        // Pharao Kopf explodiert
        case GEGNER::EXPLODIEREN: {
            AnimCount -= SpeedFaktor;
            Energy = 100.0f;

            // brodeln lassen
            PartikelSystem.PushPartikel(xPos + static_cast<float>(random(224)),
                                        yPos + static_cast<float>(random(224)), ROCKSPLITTER);
            PartikelSystem.PushPartikel(xPos + static_cast<float>(random(224)),
                                        yPos + static_cast<float>(random(224)), ROCKSPLITTERSMALL);

            // Fertig explodiert ? Dann wird er ganz zerlegt
            if (AnimCount <= 0.0f)
                Energy = 0.0f;
        } break;

        default:
            break;
    }  // switch
}

// --------------------------------------------------------------------------------------
// PharaoKopf explodiert
// --------------------------------------------------------------------------------------

void GegnerPharaoKopf::GegnerExplode() {
    for (int p = 0; p < NUMPLAYERS; p++) {
        if (Player[p].AufPlattform == this)
            Player[p].AufPlattform = nullptr;

        DirectInput.Joysticks[Player[p].JoystickIndex].ForceFeedbackEffect(FFE_BIGRUMBLE);
    }

    // Splitter
    for (int i = 0; i < 50; i++) {
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(224)),
                                    yPos + static_cast<float>(random(224)), ROCKSPLITTER);
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(224)),
                                    yPos + static_cast<float>(random(224)), ROCKSPLITTERSMALL);
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(224)),
                                    yPos + static_cast<float>(random(224)), SMOKEBIG);
    }

    Player[0].Score += 8000;

    SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION2);
    SoundManager.PlayWave(100, 128, 11025, SOUND::PHARAORAMM);

    ShakeScreen(4);

    HUD.deactivateBossHUD();

    ScrolltoPlayeAfterBoss();
}
