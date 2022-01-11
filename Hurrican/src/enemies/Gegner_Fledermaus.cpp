// --------------------------------------------------------------------------------------
// Die Fledermaus
//
// Hängt an der Decke und fliegt auf den Hurri zu, wenn er in die Nähe kommt,
// oder wenn sie angeschossen wird
// --------------------------------------------------------------------------------------

#include "Gegner_Fledermaus.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFledermaus::GegnerFledermaus(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    Energy = 35;
    AnimSpeed = 0.75f;
    ChangeLight = Light;
    Destroyable = true;
    AnimPhase = 15;
    AnimStart = 15;
    AnimEnde = 21;
    Value1 = Wert1;
    Value2 = Wert2;

    // gleich losfliegen ? (Beim Cave "Boss" T.R.Schmidt Orgien Quark)
    //
    if (Value2 == 1)
        Handlung = GEGNER::LAUFEN;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFledermaus::DoKI() {
    SimpleAnimation();

    if ((blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM) && ySpeed > 0.0f)
        ySpeed = -ySpeed;
    if (blocko & BLOCKWERT_WAND && ySpeed < 0.0f)
        ySpeed = -ySpeed;
    if (blockr & BLOCKWERT_WAND && xSpeed > 0.0f) {
        xSpeed = -xSpeed;
        BlickRichtung = LINKS;
    }
    if (blockl & BLOCKWERT_WAND && xSpeed < 0.0f) {
        xSpeed = -xSpeed;
        BlickRichtung = RECHTS;
    }

    // Je nach Handlung richtig
    switch (Handlung) {
        case GEGNER::STEHEN: {
            // Hurri in der Nähe oder von einem Schuss getroffen ?
            // Dann losfliegen
            //
            if ((PlayerAbstandHoriz() < 240 && pAim->ypos > yPos) || DamageTaken > 0)
                Handlung = GEGNER::LAUFEN;
        } break;

        case GEGNER::LAUFEN:  // Warten bis der Spieler nahe genug rankommt
        {
            AnimSpeed = 0.5f;
            AnimEnde = 14;
            AnimStart = 0;
            Value1 = static_cast<int>(pAim->xpos) + random(80) - 30;   // Flugziel zufällig in Richtung Spieler
            Value2 = static_cast<int>(pAim->ypos) - 50 + random(100);  // setzen mit etwas Variation
            Handlung = GEGNER::VERFOLGEN;
        } break;

        case GEGNER::VERFOLGEN:  // Mücke verfolgt den Spieler
        {
            // Punkt links
            if (Value1 < xPos) {
                if (!(blockl & BLOCKWERT_WAND)) {
                    xAcc = -3.0;
                    BlickRichtung = LINKS;
                }
            }

            // Punkt rechts
            if (Value1 > xPos) {
                if (!(blockr & BLOCKWERT_WAND)) {
                    xAcc = 3.0;
                    BlickRichtung = RECHTS;
                }
            }

            // Punkt oben
            if (Value2 < yPos) {
                if (!(blocko & BLOCKWERT_WAND))
                    yAcc = -3.0;
                else
                    ySpeed = 20;
            }

            // Punkt unterhalb
            if (Value2 > yPos) {
                if (!(blocku & BLOCKWERT_WAND))
                    yAcc = 3.0;
                else {
                    ySpeed = -10.0f;
                    yAcc = 0.0f;
                }
            }

            xSpeed = std::clamp(xSpeed, -10.0f, 10.0f);
            ySpeed = std::clamp(ySpeed, -10.0f, 10.0f);

            int dx = abs(static_cast<int>(xPos - Value1));
            int dy = abs(static_cast<int>(yPos - Value2));

            // Punkt erreicht oder Abstand zu groß ? Dann neues Ziel setzen
            //
            if (PlayerAbstand() > 300 || (dx * dx + dy * dy) < 20 * 20)
                Handlung = GEGNER::LAUFEN;

            // An die Wand gekommen ? Dann auch neues Ziel setzen
            //
            if (blockl & BLOCKWERT_WAND || blockr & BLOCKWERT_WAND || blocko & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM) {
                Handlung = GEGNER::LAUFEN;
                AnimPhase = 0;
            }

            // Wieder an der Decke?
            //
            static float DeckenCount = 0.0f;

            if (DeckenCount > 0.0f)
                DeckenCount -= Timer.sync(1.0f);

            if (DeckenCount <= 0.0f && blocko & BLOCKWERT_WAND) {
                DeckenCount = 10.0f;
                Handlung = GEGNER::STEHEN;
                AnimPhase = 15;
                AnimStart = 15;
                AnimEnde = 21;
                AnimSpeed = 0.75f;
                xSpeed = 0.0f;
                ySpeed = 0.0f;
                xAcc = 0.0f;
                yAcc = 0.0f;
            }
        } break;
    }

    // Spieler berührt ?
    TestDamagePlayers(Timer.sync(4.0f));
}

// --------------------------------------------------------------------------------------
// Fledermaus explodiert
// --------------------------------------------------------------------------------------

void GegnerFledermaus::GegnerExplode() {
    for (int i = 0; i < 100; i++)
        PartikelSystem.PushPartikel(xPos + 10.0f + static_cast<float>(random(35)),
                                    yPos + 10.0f + static_cast<float>(random(25)), FUNKE);

    PartikelSystem.PushPartikel(xPos - 5.0f, yPos - 5.0f, EXPLOSION_MEDIUM2);

    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);

    Player[0].Score += 100;
}
