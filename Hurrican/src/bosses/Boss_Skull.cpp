// --------------------------------------------------------------------------------------
// Der Schädel vom Skeletor
//
// Der Counter, wann sich der Schädel wieder auf die Maschine setzt, darf erst abgezogen werden,
// wenn der Schädel in der Nähe der Maschine ist. Er kann ja auch vom Skeletor gespawnt werden.
// --------------------------------------------------------------------------------------

#include "Boss_Skull.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSkull::GegnerSkull(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::INIT;
    Value1 = Wert1;
    Value2 = Wert2;
    Energy = 100000;
    ChangeLight = Light;
    Destroyable = true;
    TestBlock = false;
    SmokeDelay = 0.0f;
    ShotDelay = 10.0f;
    ShotCount = 0;
    Disappear = 20.0f;
    pMachine = nullptr;

    AnimCount = 0.0f;
    if (Value2 == 99)
        AnimCount = 20.0f;

    Handlung = GEGNER::SPECIAL;

    // Position der Endboss Wand rausfinden
    GegnerClass *pTemp = Gegner.pStart;
    while (pTemp != nullptr) {
        if (pTemp->GegnerArt == THEWALL) {
            mxPos = pTemp->xPos + 5;
            myPos = pTemp->yPos + 229;
            pMachine = pTemp;

            pTemp = nullptr;
        } else
            pTemp = pTemp->pNext;
    }
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSkull::DoKI() {
    Energy = 100000;
    BlickRichtung = LINKS;

    // Animphase abhängig von Position zum Spieler setzen
    //
    float dx = static_cast<float>(pAim->xpos + 35) - (xPos + 16);
    AnimPhase = static_cast<int>(dx / 30.0f);
    AnimPhase = std::clamp(AnimPhase, -5, 9);
    AnimPhase += 5;
    if (xSpeed < 0.0f)
        AnimPhase = 20 - AnimPhase;
    AnimPhase = std::clamp(AnimPhase, 0, 19);

    // Antrieb
    //
    SmokeDelay -= 1.0f SYNC;
    if (SmokeDelay <= 0.0f) {
        SmokeDelay = 5.0f;

        //		PartikelSystem.PushPartikel(xPos, yPos + 20, SMOKE);

        if (random(20) == 0)
            for (int i = 0; i < 10; i++)
                PartikelSystem.PushPartikel(xPos + 10.0f, yPos + 40.0f, FUNKE);
    }

    // Je nach Handlung richtig verhalten
    //
    switch (Handlung) {
        // In die Mitte fliegen und explodieren
        case GEGNER::SPECIAL3: {
            Disappear -= 1.0f SYNC;

            float endwert = 10.0f - (Disappear / 20.0f * 10.0f);

            if (endwert < 1.0f)
                endwert = 1.0f;

            Timer.SetMoveSpeed(endwert);

            // Explosionen
            if (AnimCount > 0.0f)
                AnimCount -= 1.0f SYNC;
            else {
                AnimCount = 0.8f;

                SoundManager.StopWave(SOUND::EXPLOSION2);
                SoundManager.PlayWave(75, 128, 8000 + static_cast<int>(endwert * 800.0f), SOUND::EXPLOSION2);

                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(30) - 30),
                                            yPos + static_cast<float>(random(30) - 30), EXPLOSION_MEDIUM2);

                for (int i = 0; i < 3; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(30) - 10),
                                                yPos + static_cast<float>(random(20) - 10), SPIDERSPLITTER);
            }

            if (xPos + 16 > TileEngine.XOffset + 320.0f)
                xAcc = -4.0f;
            else
                xAcc = 4.0f;

            if (yPos + 20 > TileEngine.YOffset + 200.0f)
                yAcc = -4.0f;
            else
                yAcc = 4.0f;

            xSpeed = std::clamp(xSpeed, -25.0f, 25.0f);
            ySpeed = std::clamp(ySpeed, -15.0f, 15.0f);

            if (Disappear < 0.0f) {
                // explodieren lassen
                SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION2);
                SoundManager.PlayWave(100, 128, 9000, SOUND::EXPLOSION3);
                SoundManager.PlayWave(100, 128, 7000, SOUND::EXPLOSION1);
                ShakeScreen(10.0f);
                Energy = 0.0f;
                SoundManager.PlayWave(100, 128, 7000, SOUND::MUTANT);

                PartikelSystem.PushPartikel(xPos + 20.0f, yPos + 20.0f, SHOCKEXPLOSION);
                PartikelSystem.PushPartikel(xPos + 20.0f, yPos + 20.0f, SHOCKEXPLOSION);
                PartikelSystem.PushPartikel(xPos + 20.0f, yPos + 20.0f, SHOCKEXPLOSION);

                for (int i = 0; i < 20; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(30)),
                                                yPos + static_cast<float>(random(30)), SPIDERSPLITTER);

                for (int i = 0; i < 20; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(100) - 50),
                                                yPos + static_cast<float>(random(100) - 50), EXPLOSION_BIG);

                for (int i = 0; i < 20; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(100) - 50),
                                                yPos + static_cast<float>(random(100) - 50), SPLITTER);

                for (int i = 0; i < 20; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(100) - 50),
                                                yPos + static_cast<float>(random(100) - 50), SCHROTT1);

                PartikelSystem.PushPartikel(xPos - 80.0f, yPos - 80.0f, EXPLOSION_GIGA);

                Timer.SetMoveSpeed(10.0f);

                // Flugsäcke spawnen
                Gegner.PushGegner(TileEngine.XOffset + 200.0f,
                                  TileEngine.YOffset - 40.0f, REITFLUGSACK, 1, 0, false);

                if (NUMPLAYERS == 2)
                    Gegner.PushGegner(TileEngine.XOffset + 300.0f,
                                      TileEngine.YOffset - 50.0f, REITFLUGSACK, 1, 0, false);
            }
        } break;

        // Der Schädel fliegt um den Spieler herum. Darf nicht aus dem Screen gedrängt werden
        // So lange, bis der Counter abgelaufen ist
        // dann setzt er sich wieder auf die Endboss Maschine
        case GEGNER::SPECIAL: {
            if (AnimCount > 0.0f)
                AnimCount -= 1.0f SYNC;
            else {
                if (xPos + 16 > pAim->xpos + 35)
                    xAcc = -2.0f;
                else
                    xAcc = 2.0f;

                if (yPos + 20 > pAim->ypos + 10)
                    yAcc = -2.0f;
                else
                    yAcc = 2.0f;

                xSpeed = std::clamp(xSpeed, -25.0f, 25.0f);
                ySpeed = std::clamp(ySpeed, -15.0f, 15.0f);

                if ((xSpeed < 0.0f && xPos < TileEngine.XOffset) ||
                    (xSpeed > 0.0f && xPos > TileEngine.XOffset + 640.0f))
                    xSpeed *= -1.0f;

                if ((ySpeed < 0.0f && yPos < TileEngine.YOffset) ||
                    (ySpeed > 0.0f && yPos > TileEngine.YOffset + 640.0f))
                    ySpeed *= -1.0f;
            }
        } break;

        // Auf die Maschine zufliegen?
        case GEGNER::SPECIAL2: {
            xAcc = 0.0f;
            yAcc = 0.0f;

            if (xSpeed < 0.0f)
                xSpeed += 1.0f SYNC;
            if (xSpeed > 0.0f)
                xSpeed -= 1.0f SYNC;
            if (ySpeed < 0.0f)
                ySpeed += 1.0f SYNC;
            if (ySpeed > 0.0f)
                ySpeed -= 1.0f SYNC;

            if (xPos < mxPos) {
                xPos += 10.0f SYNC;

                if (xPos > mxPos)
                    xPos = mxPos;
            }

            if (xPos > mxPos) {
                xPos -= 10.0f SYNC;

                if (xPos < mxPos)
                    xPos = mxPos;
            }

            if (yPos < myPos) {
                yPos += 10.0f SYNC;

                if (yPos > myPos)
                    yPos = myPos;
            }

            if (yPos > myPos) {
                yPos -= 10.0f SYNC;

                if (yPos < myPos)
                    yPos = myPos;
            }

            // angekommen?
            if (xPos == mxPos && yPos == myPos) {
                Energy = 0.0f;
                pMachine->Value1 = 1;

                SoundManager.PlayWave(50, 128, 15000, SOUND::KLONG);

                // DKS - Added function SongIsPlaying() to SoundManagerClass:
                if (!SoundManager.SongIsPlaying(MUSIC::BOSS))
                    SoundManager.PlaySong(MUSIC::BOSS, false);
            }
        } break;

        default:
            break;
    }  // switch

    // Auf den Spieler schiessen?
    if (Value1 == 99) {
        if (pMachine->Energy < 1500.0f)
            ShotDelay -= 1.0f SYNC;

        if (ShotDelay < 0.0f) {
            ShotDelay = 15.0f;
            Projectiles.PushProjectile(xPos - 10.0f, yPos, SUCHSCHUSS2, pAim);
            SoundManager.PlayWave(50, 128, 14000 + random(2000), SOUND::GOLEMSHOT);
        }
    }

    if (Handlung != GEGNER::SPECIAL3)
        TestDamagePlayers(1.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Schädel explodiert
// --------------------------------------------------------------------------------------

void GegnerSkull::GegnerExplode() {}
