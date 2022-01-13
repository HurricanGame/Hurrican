// --------------------------------------------------------------------------------------
// Die Extras
//
// Fliegen munter herum und warten darauf eingesammelt zu werden
// Value1 gibt den jeweiligen Typ des Extras an
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Die Extras
// Value1 gibt jeweils an, um welches Extra es sich handelt, wobei
// 0  : Spreadshot
// 1  : Laser
// 2  : Bounceshot
// 3  : Blitzstrahl
// 4  : Schutzschild
// 5  : Energie
// 6  : Rad-Power
// 7  : Powerline
// 8  : Grenade
// 9  : SmartBomb
// 10 : Autofire
// 11 : Fettboller
// 12 : Bonuspunkte
// --------------------------------------------------------------------------------------

#include "Gegner_Extras.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerExtras::GegnerExtras(int Wert1, int Wert2, bool Light) {
    BlickRichtung = DirectionEnum::LINKS;
    Energy = 1.0f;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimPhase = Value1;
    ChangeLight = false;

    // Rausfliegen?
    if (Value2 == 0) {
        Handlung = GEGNER::SPRINGEN;
        ySpeed = static_cast<float>(random(40)) / 10.0f - 20.0f;
        yAcc = 5.0f;

        if (Value1 == 0)
            xSpeed = 9.0f;
        else if (Value1 == 1)
            xSpeed = -9.0f;
        else if (Value1 == 2)
            xSpeed = 0.0f;
        else {
            xSpeed = static_cast<float>(random(80) + 20) / 8.0f;
            if (random(2) == 0)
                xSpeed *= -1;
        }

        xSpeed += static_cast<float>(random(20) - 10) / 5.0f;
    }

    // ansonsten liegenbleiben
    else
        Handlung = GEGNER::SPECIAL;

    Destroyable = false;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerExtras::DoKI() {
    BlickRichtung = DirectionEnum::LINKS;

    switch (Handlung) {
        case GEGNER::SPRINGEN:  // Extra fällt noch im Bogen raus
        {
            // an der Decke abprallen
            /*if (blocko & BLOCKWERT_WAND &&
                ySpeed < 0.0f)
                ySpeed = -ySpeed;*/

            // an der Wand abprallen
            if (blockl & BLOCKWERT_WAND || blockr & BLOCKWERT_WAND)
                xSpeed = -xSpeed;

            // Fall beendet oder auf den Boden gekommen
            if (ySpeed >= 25.0f) {
                ySpeed = -1.0f;
                xSpeed = 0;
                yAcc = 0;
                Handlung = GEGNER::STEHEN;

                // Im Tutorial fliegen die Granaten schneller nach oben =)
                if (RunningTutorial == true && Value1 == 8)
                    ySpeed = -3.0f;
            }

        } break;

        case GEGNER::STEHEN:  // Extra schwebt langsam nach oben
        {
            // An der Decke anhalten
            if (blocko & BLOCKWERT_WAND)
                ySpeed = 0;

            // Speed verlangsamen
            if (xSpeed > 0.0f) {
                xSpeed -= Timer.sync(0.5f);

                if (xSpeed < 0.0f)
                    xSpeed = 0.0f;
            }

            if (xSpeed < 0.0f) {
                xSpeed += Timer.sync(0.5f);

                if (xSpeed > 0.0f)
                    xSpeed = 0.0f;
            }
        } break;

    }  // switch

    // Testen, ob der Spieler das Extra eingesammelt hat
    PlayerClass *pCollector = nullptr;

    for (int i = 0; i < NUMPLAYERS; i++)
        if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], Player[i].xpos, Player[i].ypos, Player[i].CollideRect) ==
            true)
            pCollector = &Player[i];

    if (pCollector != nullptr)  // &&
                             // Handlung != GEGNER::SPRINGEN)
    {
        // Punkte fürs Extras sammeln gebe
        pCollector->Score += 10;

        // "Einsammel" Effekt anzeigen
        PartikelSystem.PushPartikel(xPos - 4.0f, yPos - 4.0f, EXTRACOLLECTED);

        // Waffen-Upgrade eingesammelt ?
        if (Value1 < 3) {
            // Upgraden überhaupt noch notwendig ?
            if (pCollector->CurrentWeaponLevel[Value1] < 8) {
                // Anzahl eingesammelter Powerups erhöhen
                // pCollector->CollectedPowerUps[Value1]++;

                // Maximum erreicht ? dann neuen Waffenlevel setzen
                // if (pCollector->CollectedPowerUps[Value1] >= pCollector->NextWeaponLevel[Value1])
                {
                    // pCollector->CollectedPowerUps [Value1] = 0;
                    pCollector->CurrentWeaponLevel[Value1]++;
                    // pCollector->CalcWeaponLevels();

                    // PowerUp Effekt erzeugen
                    //
                    PartikelSystem.ClearPowerUpEffects();

                    // DKS - Original code, entirely rewritten below
                    // for (int i = 0; i < 300; i++)
                    //{
                    //    int p = rand ()%360;
                    //    int r = rand ()%30+100;
                    //
                    //    //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
                    //    //DKS BUGFIX - why did the original code use radian sin/cos with random numbers within
                    //    //       huge ranges.. above values look like they were meant to be degrees - switched to
                    //    deg..
                    //    // ORIGINAL LINE:
                    //    //PartikelSystem.PushPartikel (float (pCollector->xpos + 40 - 6 + sin ((float)p)*r),
                    //    //	      float (pCollector->ypos + 40 - 6 + cos ((float)p)*r), KRINGELR + Value1,
                    //    pCollector);
                    //    // FIXED LINE:
                    //    PartikelSystem.PushPartikel (pCollector->xpos + 40.0f - 6.0f + sin_deg(p)*r,
                    //            pCollector->ypos + 40.0f - 6.0f + cos_deg(p)*r, KRINGELR + Value1, pCollector);
                    //}
                    // DKS-Rewrote the above to use radians directly and allow bitwise ops to replace mod operator.
                    for (int i = 0; i < 300; i++) {
                        float ang = TWO_PI * (1.0f / 512.0f) * static_cast<float>(random(512));
                        float r = static_cast<float>(random(32) + 98);
                        PartikelSystem.PushPartikel(pCollector->xpos + (40.0f - 6.0f) + sin_rad(ang) * r,
                                                    pCollector->ypos + (40.0f - 6.0f) + cos_rad(ang) * r,
                                                    KRINGELR + Value1, pCollector);
                    }

                    SoundManager.PlayWave(100, 128, 11025, SOUND::UPGRADE);
                    pCollector->BlinkCounter = 9.5f;
                    pCollector->BlinkColor = Value1 + 1;
                }
            }

            // Aktuelle Waffe auf zuletzt gesammeltes Extra setzen,
            // wenn die Waffe dadurch stärker wird
            //
            if (pCollector->CurrentWeaponLevel[Value1] > pCollector->CurrentWeaponLevel[pCollector->SelectedWeapon] ||
                RunningTutorial == true)
                pCollector->SelectedWeapon = Value1;
        }

        // Blitz-Upgrade eingesammelt ?
        if (Value1 == 3) {
            // Upgraden überhaupt noch notwendig ?
            if (pCollector->CurrentWeaponLevel[Value1] < 16) {
                // Anzahl eingesammelter Powerups erhöhen
                // pCollector->CollectedPowerUps[Value1]++;

                // Maximum erreicht ? dann neuen Waffenlevel setzen
                // if (pCollector->CollectedPowerUps[Value1] >= pCollector->NextWeaponLevel[Value1])
                {
                    // pCollector->CollectedPowerUps [Value1] = 0;
                    pCollector->CurrentWeaponLevel[Value1]++;
                    // pCollector->NextWeaponLevel   [Value1] = 3;

                    // PowerUp Effekt erzeugen
                    //
                    PartikelSystem.ClearPowerUpEffects();

                    for (int i = 0; i < 300; i++) {
                        float p = static_cast<float>(random(360));
                        float r = static_cast<float>(random(30) + 100);

                        // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
                        // DKS BUGFIX - why did the original code use radian sin/cos with random numbers within
                        //       huge ranges.. above values look like they were meant to be degrees - switched to deg..
                        // PartikelSystem.PushPartikel (float (pCollector->xpos + 40 - 6 + sin ((float)p)*r),
                        //                               float (pCollector->ypos + 40 - 6 + cos ((float)p)*r),
                        //                               KRINGELHB, pCollector);
                        PartikelSystem.PushPartikel(pCollector->xpos + 40.0f - 6.0f + sin_deg(p) * r,
                                                    pCollector->ypos + 40.0f - 6.0f + cos_deg(p) * r, KRINGELHB,
                                                    pCollector);
                    }

                    SoundManager.PlayWave(100, 128, 11025, SOUND::UPGRADE);
                    pCollector->BlinkCounter = 9.5f;
                    pCollector->BlinkColor = 4;
                }
            }
        }

        // Schutzschild
        if (Value1 == 4) {
            // Schutzuschild Spawner erzeugen
            if (pCollector->Shield <= 0.0f) {
                Projectiles.PushProjectile(pCollector->xpos, pCollector->ypos, SHIELDSPAWNER, pCollector);
                Projectiles.PushProjectile(pCollector->xpos, pCollector->ypos, SHIELDSPAWNER2, pCollector);
            }

            // Schild setzen
            pCollector->Shield = 100.0f;
        }

        // Energie auffrischen
        if (Value1 == 5)
            pCollector->Energy = MAX_ENERGY;

        // Rad-Energie auffrischen
        if (Value1 == 6)
            pCollector->Armour = MAX_ARMOUR;

        // Powerline
        if (Value1 == 7 && pCollector->PowerLines < PLAYER_MAXEXTRAS)
            pCollector->PowerLines++;

        // Granate
        if (Value1 == 8 && pCollector->Grenades < PLAYER_MAXEXTRAS)
            pCollector->Grenades++;

        // SmartBomb
        if (Value1 == 9 && pCollector->SmartBombs < 1)
            pCollector->SmartBombs++;

        // Autofire
        if (Value1 == 10)
            pCollector->AutoFireExtra += 120;

        // Supershot
        if (Value1 == 11)
            pCollector->RiesenShotExtra += 120;

        // Score
        if (Value1 == 12)
            Player[0].Score += 100;

        Energy = 0.0f;

        SoundManager.PlayWave(100, 128, 11025, SOUND::VOICE_SPREAD + Value1);  // Sound ausgeben

        switch (Value1) {
            case 0:
            case 1:
            case 2:
            case 4:
                SoundManager.PlayWave(100, 128, 11025, SOUND::COLLECT);  // Sound ausgeben
                break;

            case 8:
            case 9:
            case 10:
                SoundManager.PlayWave(100, 128, 11000 + random(1000), SOUND::AMMO);  // Sound ausgeben
                break;

            default:
                SoundManager.PlayWave(100, 128, 11025, SOUND::COLLECT);  // Sound ausgeben
                break;
        }

        SoundManager.PlayWave(100, 128, 11025, SOUND::COLLECT);  // Sound ausgeben
    }
}

// --------------------------------------------------------------------------------------
// Extra verschwindet
// --------------------------------------------------------------------------------------

void GegnerExtras::GegnerExplode() {}
