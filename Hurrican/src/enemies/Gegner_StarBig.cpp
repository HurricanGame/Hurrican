// --------------------------------------------------------------------------------------
// Der große Stern im Flugsack level
//
// Prallt von der Wand ab und schiesst ab und zu
// --------------------------------------------------------------------------------------

#include "Gegner_StarBig.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerStarBig::GegnerStarBig(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 100;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimStart = 0;
    AnimEnde = 4;
    AnimSpeed = 0.75f;
    AnimCount = 0.0f;
    ChangeLight = Light;
    Destroyable = true;
    ShotCount = 0;

    ySpeed = 12.0f;
    yAcc = 0.0f;
    xAcc = 0.0f;

    // in Richtung Spieler fliegen
    //
    if (pAim->xpos + 80 < xPos)
        xSpeed = -12.0f;
    else
        xSpeed = 12.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerStarBig::DoKI() {
    if (xSpeed > 0.0f)
        BlickRichtung = DirectionEnum::RECHTS;
    else
        BlickRichtung = DirectionEnum::LINKS;

    // Animieren
    AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
    if (AnimCount > AnimSpeed)  // Grenze überschritten ?
    {
        AnimCount = 0;              // Dann wieder auf Null setzen
        AnimPhase++;                // Und nächste Animationsphase
        if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
        {
            AnimPhase = AnimStart;  // Dann wieder von vorne beginnen
            ShotCount++;

            // Schiessen
            //
            if (ShotCount > Skill + 2) {
                ShotCount = 0;
                SoundManager.PlayWave(100, 128, 15000 + random(4000), SOUND::CANON);
                Projectiles.PushProjectile(xPos + 36.0f, yPos + 36.0f, SUCHSCHUSS);
            }
        }
    }  // animieren

    // Je nach Handlung richtig verhalten
    //
    switch (Handlung) {
        case GEGNER::LAUFEN: {
            // An den Wänden umdrehen
            //
            if ((xSpeed < 0.0f && blockl & BLOCKWERT_WAND) || (xSpeed > 0.0f && blockr & BLOCKWERT_WAND))
                xSpeed *= -1.0f;

            if ((ySpeed < 0.0f && blocko & BLOCKWERT_WAND) || (ySpeed > 0.0f && blocku & BLOCKWERT_WAND) ||
                (ySpeed < 0.0f && yPos - TileEngine.YOffset < 0.0f) ||
                (ySpeed > 0.0f && yPos - TileEngine.YOffset > 400.0f))
                ySpeed *= -1.0f;
        } break;

        default:
            break;

    }  // switch

    // Testen, ob der Spieler den Sack berührt hat
    TestDamagePlayers(Timer.sync(2.0f));
}

// --------------------------------------------------------------------------------------
// StarBig explodiert
// --------------------------------------------------------------------------------------

void GegnerStarBig::GegnerExplode() {
    PartikelSystem.PushPartikel(xPos - 20.0f, yPos - 20.0f, EXPLOSION_GIANT);

    for (int i = 0; i < 10; i++)
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(80)),
                                    yPos - 30.0f + static_cast<float>(random(80)), EXPLOSION_MEDIUM2);

    for (int i = 0; i < 15; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(80)),
                                    yPos + static_cast<float>(random(80)), LONGFUNKE);

    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION3);

    Player[0].Score += 150;  // Punkte geben
}
