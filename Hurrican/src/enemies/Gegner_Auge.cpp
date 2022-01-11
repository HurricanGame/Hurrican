// --------------------------------------------------------------------------------------
// Das Augen Extra
//
// fliegt Sinus förmig durchs Level und hinterlässt nach Zerstörung ein Extra
// welches Extra, das steht in Value1
// --------------------------------------------------------------------------------------

#include "Gegner_Auge.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerAuge::GegnerAuge(int Wert1, int Wert2, bool Light) {
    // DKS - Moved the Wert1 check check to the top, it was having no effect in its original
    //      position here:
    //      I discovered that GegnerExtras were being pushed with Value1 parameters that were
    //      outside their valid range, causing AnimPhase to be set to 40, when its only has
    //      13 animation frames.
    // Powerline ist Standard
    //
    if (Wert1 < 0 || Wert1 > 11)
        Wert1 = 7;

    Handlung = GEGNER::LAUFEN;
    Energy = 1;
    Value1 = Wert1;
    Value2 = Wert2;

    smokedelay = 0.2f;
    ChangeLight = Light;
    Destroyable = true;
    xSpeed = 12.0f;

    ySpeed = 0.0f;
    yAcc = 4.0f;
    AnimSpeed = 0.5f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerAuge::DoKI() {
    // rauchen lassen
    //
    smokedelay -= Timer.sync(0.25f);

    if (smokedelay < 0.0f) {
        smokedelay = 0.1f;
        if (BlickRichtung == RECHTS)
            PartikelSystem.PushPartikel(xPos + 8.0f, yPos + 18.0f, SMOKE2);
        else
            PartikelSystem.PushPartikel(xPos + 42.0f, yPos + 18.0f, SMOKE2);
    }
    // Animieren (nur drehen)
    //
    if (Handlung != GEGNER::STEHEN) {
        AnimCount += SpeedFaktor;  // Animationscounter weiterzählen
        if (Handlung == GEGNER::DREHEN) {
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
                if (AnimPhase <= 0)  // Animation von zu Ende	?
                {
                    AnimPhase = 0;  // Dann wieder von vorne beginnen
                    AnimEnde = 0;
                    Handlung = GEGNER::LAUFEN;
                }
            }
        }
    }

    if (ySpeed > 12.0f)
        yAcc = -4.0f;
    else if (ySpeed < -12.0f)
        yAcc = 4.0f;

    // An der Decke bzw dem Boden
    // abprallen
    //
    if (blocko & BLOCKWERT_WAND || blocko & BLOCKWERT_GEGNERWAND) {
        yAcc = -yAcc;
        ySpeed = -ySpeed;
    }

    if (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_GEGNERWAND) {
        yAcc = -yAcc;
        ySpeed = -ySpeed;
    }

    // Links oder rechts an der Wand abgeprallt ?
    //
    if ((xSpeed < 0.0f && (blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND)) ||
        (xSpeed > 0.0f && (blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND))) {
        Handlung = GEGNER::DREHEN;
        AnimPhase = 0;
        AnimCount = 0.0f;
        AnimEnde = 5;
        xSpeed *= -1.0f;
    }

    // Spieler verletzen ?
    //
    /*if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt],
                        pPlayer->xpos, pPlayer->ypos, pPlayer->CollideRect) == true)
        pPlayer->DamagePlayer(Timer.sync(2.0f));*/
}

// --------------------------------------------------------------------------------------
// Auge explodiert
// --------------------------------------------------------------------------------------

void GegnerAuge::GegnerExplode() {
    // Explosion
    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(xPos - 15.0f + static_cast<float>(random(20)),
                                    yPos - 15.0f + static_cast<float>(random(40)), EXPLOSION_MEDIUM2);

    SoundManager.PlayWave(100, 128, -random(2000) + 11025, SOUND::EXPLOSION1);  // Sound ausgeben

    Player[0].Score += 100;

    // Extra spawnen
    Gegner.PushGegner(xPos + 12.0f, yPos + 12.0f, EXTRAS, Value1, 0, true);
}
