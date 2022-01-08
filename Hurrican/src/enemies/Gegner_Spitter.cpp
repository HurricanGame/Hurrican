// --------------------------------------------------------------------------------------
// Der Spitter
//
// Bewegt sich nur von links nach rechts und dreht um, wenn er an eine Wand gelangt
// und schiesst in regelmässigen Abständen senkrecht nach oben
// --------------------------------------------------------------------------------------

#include "Gegner_Spitter.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSpitter::GegnerSpitter(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_LAUFEN;
    AnimEnde = 4;
    AnimSpeed = 1.25f;
    Energy = 20;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    TestBlock = false;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSpitter::DoKI() {
    blockl = TileEngine.BlockLinks(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
    blockr = TileEngine.BlockRechts(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
    blocko = TileEngine.BlockOben(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

    // Animieren
    if (AnimEnde > 0)  // Soll überhaupt anmiert werden ?
    {
        AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;  // Dann wieder auf Null setzen
            AnimPhase++;    // Und nächste Animationsphase

            if (AnimPhase == 2) {
                Projectiles.PushProjectile(xPos + 8, yPos, STRAIGHTSCHUSS);
                PartikelSystem.PushPartikel(xPos, yPos - 8, SMOKE);
            }

            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
                AnimPhase = 0;          // Dann wieder von vorne beginnen
        }
    }  // animieren

    // Spieler verfolgen
    //

    if (PlayerAbstand() > 100) {
        if (pAim->xpos < xPos) {
            BlickRichtung = LINKS;
            xAcc = -2.0f;
        } else {
            BlickRichtung = RECHTS;
            xAcc = 2.0f;
        }
    }

    if (xSpeed < -7.0f) {
        xSpeed = -7.0f;
        xAcc = 0.0f;
    }

    if (xSpeed > 7.0f) {
        xSpeed = 7.0f;
        xAcc = 0.0f;
    }

    if (ySpeed >= 0.0f)
        // DKS - Rewrote BlockSlopes function to only take the parameters it uses:
        // blocku = TileEngine.BlockSlopes(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt], ySpeed);
        blocku = TileEngine.BlockSlopes(xPos, yPos, GegnerRect[GegnerArt], ySpeed);

    // runterfallen?
    //

    if (!(blocku & BLOCKWERT_SCHRAEGE_R) && !(blocku & BLOCKWERT_SCHRAEGE_L)) {
        blocku = TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

        if (!(blocku & BLOCKWERT_WAND) && !(blocku & BLOCKWERT_PLATTFORM)) {
            if (ySpeed == 0.0f)
                ySpeed = 5.0f;

            yAcc = 3.0f;
        }

        else {
            ySpeed = 0.0f;
            yAcc = 0.0f;
        }
    }

    // An der Wand umdrehen
    //

    if (xSpeed < 0.0f) {
        if (!(blocku & BLOCKWERT_SCHRAEGE_L) && (blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND))
            xSpeed *= -1.0f;
    }

    if (xSpeed > 0.0f) {
        if (!(blocku & BLOCKWERT_SCHRAEGE_R) && (blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND))
            xSpeed *= -1.0f;
    }

    // Testen, ob der Spieler den Spitter berührt hat
    //
    TestDamagePlayers(1.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Spitter explodiert
// --------------------------------------------------------------------------------------

void GegnerSpitter::GegnerExplode() {
    Player[0].Score += 50;

    PartikelSystem.PushPartikel(static_cast<float>(xPos - 10), static_cast<float>(yPos - 10), EXPLOSION_MEDIUM);

    SoundManager.PlayWave(100, 128, -random(2000) + 11025, SOUND_EXPLOSION1);  // Sound ausgeben
}
