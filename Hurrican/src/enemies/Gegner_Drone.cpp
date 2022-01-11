// --------------------------------------------------------------------------------------
// Die Drone
//
// Schwirrt um Hurri rum und ballert dabei
// --------------------------------------------------------------------------------------

#include "Gegner_Drone.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerDrone::GegnerDrone(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 125;
    AnimEnde = 7;
    AnimSpeed = 0.75f;
    AnimCount = 0.0f;
    ChangeLight = Light;
    Destroyable = true;

    ShotCount = 0;
    ShotDelay = 0.0f;
    ActionDelay = 5.0f;

    Value1 = Wert1;

    // Aus der Spinnenmaschine ausgespuckt?
    if (Wert1 == 99)
        ySpeed = -22.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerDrone::DoKI() {
    // Animieren
    //
    AnimCount += SpeedFaktor;  // Animationscounter weiterzählen
    if (Handlung == GEGNER::LAUFEN) {
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;              // Dann wieder auf Null setzen
            AnimPhase++;                // Und nächste Animationsphase
            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
                AnimPhase = AnimStart;  // Dann wieder von vorne beginnen
        }
    } else if (Handlung == GEGNER::DREHEN) {
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;              // Dann wieder auf Null setzen
            AnimPhase++;                // Und nächste Animationsphase
            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
            {
                AnimCount = 0.0f;
                Handlung = GEGNER::DREHEN2;
                BlickRichtung *= -1;
            }
        }
    } else if (Handlung == GEGNER::DREHEN2) {
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;       // Dann wieder auf Null setzen
            AnimPhase--;         // Und nächste Animationsphase
            if (AnimPhase <= 6)  // Animation von zu Ende	?
            {
                AnimPhase = AnimStart;  // Dann wieder von vorne beginnen
                AnimEnde = 7;
                Handlung = GEGNER::LAUFEN;
                ActionDelay = 1.0f;
            }
        }
    }

    // in Richtung Spieler fliegen
    //
    if (Handlung != GEGNER::FALLEN) {
        if (xPos + 30.0f < pAim->xpos + 35.0f)
            xAcc = 1.5f;
        if (xPos + 30.0f > pAim->xpos + 35.0f)
            xAcc = -1.5f;

        if (yPos < pAim->ypos - 60.0f)
            yAcc = 1.5f;
        if (yPos > pAim->ypos - 60.0f)
            yAcc = -1.5f;

        xSpeed = std::clamp(xSpeed, -20.0f, 20.0f);
        if (ySpeed > 10.0f)
            ySpeed = 10.0f;
        if (ySpeed < -10.0f && Value1 != 99)
            ySpeed = -10.0f;

        // An der Wand umdrehen
        //
        if ((blocko & BLOCKWERT_WAND || blocko & BLOCKWERT_GEGNERWAND) && ySpeed < 0.0f)
            ySpeed *= 1.0f;
        if ((blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_GEGNERWAND) && ySpeed > 0.0f)
            ySpeed *= 1.0f;
        if ((blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND) && xSpeed < 0.0f)
            xSpeed *= 1.0f;
        if ((blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND) && xSpeed > 0.0f)
            xSpeed *= 1.0f;
    }

    switch (Handlung) {
        // In der Luft rumdümpeln
        //
        case GEGNER::LAUFEN:
            if ((xPos + 30.0f < pAim->xpos + 35.0f && BlickRichtung == LINKS) ||
                (xPos + 30.0f > pAim->xpos + 35.0f && BlickRichtung == RECHTS)) {
                Handlung = GEGNER::DREHEN;
                AnimPhase = 6;
                AnimCount = 0.0f;
                AnimEnde = 11;
            }

            ActionDelay -= 1.0f SYNC;

            if (ActionDelay <= 0.0f && PlayerAbstand() < 600) {
                Handlung = GEGNER::SCHIESSEN;

                switch (Skill) {
                    case SKILL_EASY:     ShotCount =  4; break;
                    case SKILL_MEDIUM:   ShotCount =  6; break;
                    case SKILL_HARD:     ShotCount =  8; break;
                    case SKILL_HURRICAN: ShotCount = 10; break;
                }

                ShotDelay = 20.0f;

                SoundManager.PlayWave3D(static_cast<int>(xPos), static_cast<int>(yPos), 12000, SOUND::DRONE);

                PartikelSystem.PushPartikel(xPos + 33.0f, yPos + 60.0f, BULLET, &Player[0]);

                if (BlickRichtung == RECHTS)
                    Projectiles.PushProjectile(xPos + 58.0f, yPos + 68.0f, DRONEBULLET);
                else
                    Projectiles.PushProjectile(xPos - 10.0f, yPos + 68.0f, DRONEBULLET2);
            }
            break;

        // Auf den Spieler ballern
        //
        case GEGNER::SCHIESSEN:
            if ((xPos + 30.0f < pAim->xpos + 35.0f && BlickRichtung == LINKS) ||
                (xPos + 30.0f > pAim->xpos + 35.0f && BlickRichtung == RECHTS)) {
                Handlung = GEGNER::DREHEN;
                AnimPhase = 6;
                AnimCount = 0.0f;
                AnimEnde = 11;
            }

            ShotDelay -= 12.0f SYNC;

            // Salve zuende und weiterfliegen ?
            //
            if (ShotCount <= 0 && ShotDelay < 12.0f) {
                ActionDelay = 10.0f;
                Handlung = GEGNER::LAUFEN;
            }

            // Schuss abgeben
            //
            if (ShotDelay <= 0.0f && PlayerAbstand() < 600) {
                SoundManager.PlayWave3D(static_cast<int>(xPos), static_cast<int>(yPos), 12000, SOUND::DRONE);
                ShotDelay = 20.0f;
                ShotCount--;

                PartikelSystem.PushPartikel(xPos + 36.0f, yPos + 57.0f, BULLET, &Player[0]);

                if (BlickRichtung == RECHTS)
                    Projectiles.PushProjectile(xPos + 58.0f, yPos + 68.0f, DRONEBULLET);
                else
                    Projectiles.PushProjectile(xPos - 10.0f, yPos + 68.0f, DRONEBULLET2);
            }

            // Flamme anzeigen
            //
            if (static_cast<int>(ShotDelay) >= 12) {
                DirectGraphics.SetAdditiveMode();

                int anim;
                if (BlickRichtung == -1)
                    anim = 0;
                else
                    anim = 1;

                Gegner.DroneFlame.RenderSprite(xPos - TileEngine.XOffset - 5.0f + static_cast<float>(BlickRichtung * 56),
                                               yPos - TileEngine.YOffset + 60.0f, anim, 0xFFFFFFFF);
                DirectGraphics.SetColorKeyMode();
            }
            break;

        // Drone stürzt ab
        //
        case GEGNER::FALLEN: {
            // An die Wand gekracht ?
            if (blockl & BLOCKWERT_WAND || blockr & BLOCKWERT_WAND || blocko & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM)
                Energy = 0.0f;

            // Grenze der Fallgeschwindigkeit
            if (ySpeed > 35.0f)
                yAcc = 0.0f;

            // Drone rauchen lassen
            if (AnimCount >= 0.5f) {
                AnimCount = 0.0f;
                PartikelSystem.PushPartikel(xPos + 20 + static_cast<float>(random(30)),
                                            yPos + 10 + static_cast<float>(random(40)), SMOKE);
                PartikelSystem.PushPartikel(xPos + 20 + static_cast<float>(random(30)),
                                            yPos + 10 + static_cast<float>(random(40)), SMOKE3);
            }
        } break;
    }

    // Drone stürzt ab ?
    //
    if (Energy <= 0.0f && Handlung != GEGNER::FALLEN) {
        Handlung = GEGNER::FALLEN;
        AnimCount = 0.0f;
        Energy = 50.0f;
        ySpeed = 1.0f;
        yAcc = 2.0f;
        xAcc = 0.0f;
    }
}

// --------------------------------------------------------------------------------------
// Drone explodiert
// --------------------------------------------------------------------------------------

void GegnerDrone::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION3);

    for (int i = 0; i < 5; i++) {
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(70)),
                                    yPos - 30.0f + static_cast<float>(random(80)), EXPLOSION_MEDIUM2);
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(70)),
                                    yPos - 30.0f + static_cast<float>(random(80)), SPIDERSPLITTER);
    }

    for (int i = 0; i < 10; i++)
        PartikelSystem.PushPartikel(xPos - 30 + static_cast<float>(random(70)),
                                    yPos - 30 + static_cast<float>(random(80)), SPLITTER);

    PartikelSystem.PushPartikel(xPos - 24.0f, yPos - 16.0f, EXPLOSION_GIANT);

    Player[0].Score += 300;
}
