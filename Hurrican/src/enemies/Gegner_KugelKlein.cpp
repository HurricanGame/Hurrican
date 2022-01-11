// --------------------------------------------------------------------------------------
// Die kleine Stachelkugel
//
// Hüpft nur im Gang entlang
// Je nachdem wie hoch sie im Editor über dem Boden gesetzt wurde springt sie auch
// später umher. Springt zu Beginn in eine zufällige Richtung links/rechts
// --------------------------------------------------------------------------------------

#include "Gegner_KugelKlein.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerKugelKlein::GegnerKugelKlein(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 20;
    BlickRichtung = LINKS;
    yAcc = 5.0f;                                         // Fallen lassen
    Value1 = Wert1;
    Value2 = Wert2;
    if (Value1 != 0)  // ggf vorgegebene Speed benutzen
        xSpeed = static_cast<float>(Value1);
    else
        xSpeed = (random(2) == 0) ? -8.0f : 8.0f;  // x-Richtung zu Beginn Zufall
    AnimSpeed = 0.3f;
    AnimStart = 0;
    AnimEnde = 19;
    ChangeLight = Light;
    Destroyable = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerKugelKlein::DoKI() {
    BlickRichtung = LINKS;
    AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
    if (AnimCount > AnimSpeed)  // Grenze überschritten ?
    {
        AnimCount = 0;  // Dann wieder auf Null setzen

        // Vorwärts oder rückwärts animieren, je nachdem, in welche Richtung die Kugel gerade fliegt
        //
        if (xSpeed > 0.0f) {
            AnimPhase++;                // Und nächste Animationsphase
            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
                AnimPhase = AnimStart;  // Dann wieder von vorne beginnen
        } else {
            AnimPhase--;                // Und vorherige Animationsphase
            if (AnimPhase < AnimStart)  // Animation von zu Ende	?
                AnimPhase = AnimEnde;   // Dann wieder von hinten her beginnen
        }
    }  // animieren

    // Grenze für y-Bewegung setzen
    //
    if (ySpeed > 20.0f)
        ySpeed = 20.0f;

    // Je nach Handlung richtig verhalten
    //
    switch (Handlung) {
        case GEGNER::LAUFEN:  // Kugel hüpft im Gang rum
        {
            // Vertikal abhüpfen
            //
            if ((ySpeed > 0.0f &&  // Boden
                 (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_GEGNERWAND ||
                  blocku & BLOCKWERT_PLATTFORM)) ||  // Decke
                (ySpeed < 0.0f && (blocko & BLOCKWERT_WAND || blocko & BLOCKWERT_GEGNERWAND))) {
                ySpeed *= -1.0f;

                // Mindestsprunghöhe setzen
                //
                if (ySpeed < 0.0f && ySpeed > -10.0f)
                    ySpeed = -10.0f;
            }

            // Horizontal abhüpfen
            //
            if ((xSpeed < 0.0f &&                                                // Links
                 (blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND)) ||  // Rechts
                (xSpeed > 0.0f && (blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND)))
                xSpeed *= -1.0f;

        } break;

        default:
            break;
    }  // switch

    // Berührt die Kugel den Spieler ?
    //
    TestDamagePlayers(2.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Stachelkugel explodiert
// --------------------------------------------------------------------------------------

void GegnerKugelKlein::GegnerExplode() {
    // Explosion
    PartikelSystem.PushPartikel(xPos - 15.0f, yPos - 15.0f, EXPLOSION_MEDIUM2);

    SoundManager.PlayWave(100, 128, -random(2000) + 11025, SOUND::EXPLOSION4);  // Sound ausgeben

    Player[0].Score += 300;
}
