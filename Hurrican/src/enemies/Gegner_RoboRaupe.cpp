// --------------------------------------------------------------------------------------
// Die Roboter Raupe
//
// kriecht auf den Spieler zu und schiesst in regelmässigen Abständen 3 Feuerbälle
// --------------------------------------------------------------------------------------

#include "Gegner_RoboRaupe.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerRoboRaupe::GegnerRoboRaupe(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 60;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    AnimEnde = 9;
    AnimSpeed = 1.0f;
    BlickRichtung = LINKS;
}

// --------------------------------------------------------------------------------------
// Bewegungs KI
// --------------------------------------------------------------------------------------

void GegnerRoboRaupe::DoKI() {
    // animieren
    AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
    if (AnimCount > AnimSpeed)  // Grenze überschritten ?
    {
        AnimCount = 0;             // Dann wieder auf Null setzen
        AnimPhase++;               // Und nächste Animationsphase
        if (AnimPhase > AnimEnde)  // Animation von zu Ende	?
            AnimPhase = AnimStart;

        if (AnimPhase == 5)  // Bewegen
        {
            if (BlickRichtung == LINKS)
                xPos -= 18.0f;
            else
                xPos += 18.0f;

            // evtl schiessen wenn Spieler zu nahe
            if (PlayerAbstand(true) < 200 && random(2) == 0) {
                if (BlickRichtung == RECHTS) {
                    Projectiles.PushProjectile(xPos + 31.0f, yPos + 10.0f, ARCSHOT);

                    WinkelUebergabe = 1.5f;
                    Projectiles.PushProjectile(xPos + 15.0f, yPos + 10.0f, ARCSHOTLEFT);
                    Projectiles.PushProjectile(xPos + 51.0f, yPos + 10.0f, ARCSHOTRIGHT);
                } else {
                    Projectiles.PushProjectile(xPos + 50.0f, yPos + 10.0f, ARCSHOT);

                    WinkelUebergabe = 1.5f;
                    Projectiles.PushProjectile(xPos + 32.0f, yPos + 10.0f, ARCSHOTLEFT);
                    Projectiles.PushProjectile(xPos + 70.0f, yPos + 10.0f, ARCSHOTRIGHT);
                }

                SoundManager.PlayWave(100, 128, 11025, SOUND::GRANATE);
            }
        }
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
    if (blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND)
        BlickRichtung = RECHTS;
    if (blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND)
        BlickRichtung = LINKS;

    // Testen, ob der Spieler die Raupe berührt hat
    TestDamagePlayers(Timer.sync(5.0f));
}

// --------------------------------------------------------------------------------------
// Explodieren
// --------------------------------------------------------------------------------------

void GegnerRoboRaupe::GegnerExplode() {
    // blaue Explosionen erzeugen
    for (int i = 0; i < 50; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(90)),
                                    yPos - 10.0f + static_cast<float>(random(40)), BLUE_EXPLOSION);

    for (int i = 0; i < 300; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(90)),
                                    yPos + 5.0f + static_cast<float>(random(25)), ROCKETSMOKEBLUE);

    SoundManager.PlayWave(100, 128, 11025 + random(2000), SOUND::EXPLOSION3);  // Sound ausgeben

    Player[0].Score += 250;
}
