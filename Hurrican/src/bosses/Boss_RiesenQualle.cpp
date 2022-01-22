// --------------------------------------------------------------------------------------
// Die riesen Qualle
//
// Schwabbelt unter Wasser rum in Richtung Spieler
// --------------------------------------------------------------------------------------

#include "Boss_RiesenQualle.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerRiesenQualle::GegnerRiesenQualle(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_LAUFEN;
    AnimStart = 0;
    AnimEnde = 6;
    AnimSpeed = 2.0f;
    BlickRichtung = LINKS;
    xSpeed = 0.0f;
    ySpeed = -8.0f;
    yAcc = 0.4f;
    Energy = 3000;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerRiesenQualle::DoKI() {
    // animieren
    AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
    if (AnimCount > AnimSpeed)  // Grenze überschritten ?
    {
        AnimCount = 0;             // Dann wieder auf Null setzen
        AnimPhase++;               // Und nächste Animationsphase
        if (AnimPhase > AnimEnde)  // Animation von zu Ende	?
            AnimPhase = AnimStart;
    }

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER_SPRINGEN:  // Nach oben abgestossen zum Schwimmen
        {
            // Geschwindigkeits-Begrenzung
            if (ySpeed > 0.0f)
                yAcc = 0.25f;

            if (ySpeed > 8.0f)
                yAcc = 0.0f;

            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
            {
                // Dann absinken
                AnimPhase = 0;
                AnimEnde = 0;
                AnimStart = 0;
                Handlung = GEGNER_LAUFEN;
            }
        } break;

        case GEGNER_FALLEN: {
            if (PlayerAbstand() < 200)  // spieler verfolgen ?
            {
                AnimPhase = 0;
                Handlung = GEGNER_LAUFEN;
                yAcc = 0.0f;
            }
        } break;

        case GEGNER_LAUFEN:  // Qualle sinkt ab
        {
            // Nach oben abstossen
            if (yPos - 20 > pAim->ypos || blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_GEGNERWAND) {
                // Bläschen erzeugen
                PartikelSystem.PushPartikel(xPos + 20, yPos + 30, BUBBLE);

                // rechts / links bewegen
                if (xPos < pAim->xpos)
                    xSpeed = 1.0f;
                else
                    xSpeed = -1.0f;

                AnimPhase = 1;
                AnimEnde = 6;
                AnimCount = 0.0f;
                ySpeed = -8.0f;
                yAcc = 0.4f;
                Handlung = GEGNER_SPRINGEN;
            }
        } break;

        default:
            break;
    }  // switch

    // Testen, ob der Spieler die Qualle berührt hat
    TestDamagePlayers(5.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Riesen Qualle explodiert
// --------------------------------------------------------------------------------------

void GegnerRiesenQualle::GegnerExplode() {
    // Fetzen erzeugen
    for (int i = 0; i < 8; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(64) + 64),
                                    yPos + static_cast<float>(random(64) + 64), PIRANHATEILE2);

    // und noch n paar Luftbläschen dazu
    for (int i = 0; i < 100; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(256) - 10),
                                    yPos + static_cast<float>(random(210) + 10), BUBBLE);

    // Blutwolken dazu
    for (int i = 0; i < 70; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(256) - 10),
                                    yPos + static_cast<float>(random(210) + 10), PIRANHABLUT);

    // SoundManager.PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION1);	// Sound ausgeben

    Player[0].Score += 4000;
}
