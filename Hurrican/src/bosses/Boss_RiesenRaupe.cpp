// --------------------------------------------------------------------------------------
// Die Riesen Raupe
//
// kriecht auf den Spieler zu und schiesst 2 Suchschüsse bei richtigem Abstand
// --------------------------------------------------------------------------------------

#include "Boss_RiesenRaupe.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerRiesenRaupe::GegnerRiesenRaupe(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 800;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    AnimEnde = 10;
    AnimSpeed = 1.5f;
    BlickRichtung = LINKS;
    ShotCount = 0;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerRiesenRaupe::DoKI() {
    // animieren
    AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
    if (AnimCount > AnimSpeed)  // Grenze überschritten ?
    {
        AnimCount = 0;  // Dann wieder auf Null setzen
        AnimPhase++;    // Und nächste Animationsphase

        if (AnimPhase > AnimEnde)  // Animation zu Ende	?
        {
            AnimPhase = AnimStart;
            xSpeed = 0.0f;

            if (PlayerAbstand() > 400 || ShotCount == 0) {
                ShotCount = random(5);
                AnimEnde = 10;
                AnimSpeed = 1.5f;
            }

            // evtl schiessen?
            //
            else {
                // evtl schiessen wenn Spieler zu nahe
                ShotCount--;

                AnimPhase = 10;
                AnimEnde = 13;
                AnimSpeed = 1.5f;

                PartikelSystem.PushPartikel(xPos + 30.0f, yPos - 2.0f, SMOKE);
                PartikelSystem.PushPartikel(xPos + 120.0f, yPos - 2.0f, SMOKE);

                Projectiles.PushProjectile(xPos + 30.0f, yPos - 2.0f, PFLANZESHOT, pAim);
                Projectiles.PushProjectile(xPos + 120.0f, yPos - 2.0f, PFLANZESHOT, pAim);

                SoundManager.PlayWave(100, 128, 11025, SOUND::ROCKET);
            }
        }
    }

    if (AnimPhase < 10) {
        if (BlickRichtung == LINKS)
            xSpeed = -1.0f;
        else
            xSpeed = 1.0f;
    }

    // runterfallen ?
    if (!(blocku & BLOCKWERT_WAND) && !(blocku & BLOCKWERT_PLATTFORM)) {
        yAcc = 3.0f;

        if (ySpeed > 20.0f)
            ySpeed = 20.0f;
    } else {
        ySpeed = 0.0f;
        yAcc = 0.0f;
    }

    // umdrehen ?
    if ((blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND) && xSpeed < 0.0f) {
        BlickRichtung = RECHTS;
        xSpeed *= -1;
    }

    if ((blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND) && xSpeed > 0.0f) {
        BlickRichtung = LINKS;
        xSpeed *= -1;
    }

    // Testen, ob der Spieler die Raupe berührt hat
    TestDamagePlayers(Timer.sync(5.0f));
}

// --------------------------------------------------------------------------------------
// RiesenRaupe explodiert
// --------------------------------------------------------------------------------------

void GegnerRiesenRaupe::GegnerExplode() {
    // Explosionen erzeugen
    for (int i = 0; i < 10; i++) {
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(180)),
                                    yPos - 30.0f + static_cast<float>(random(60)), EXPLOSION_BIG);
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(180)),
                                    yPos - 30.0f + static_cast<float>(random(60)), SPIDERSPLITTER);
    }

    for (int i = 0; i < 20; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(180)),
                                    yPos - 10.0f + static_cast<float>(random(60)), EXPLOSION_MEDIUM2);

    for (int i = 0; i < 300; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(180)),
                                    yPos + static_cast<float>(random(50)), ROCKETSMOKE);

    SoundManager.PlayWave(100, 128, 11025 + random(2000), SOUND::EXPLOSION3);  // Sound ausgeben

    Player[0].Score += 250;
}
