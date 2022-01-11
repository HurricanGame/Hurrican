// --------------------------------------------------------------------------------------
// Die Stahlmuecke
//
// Fliegt auf den Spieler zu, sobald sie ins Bild kommt
// Ist sie getroffen, stürzt sie rauchend ab und explodiert an der nächsten Wand/Boden
// --------------------------------------------------------------------------------------

#include "Gegner_Stahlmuecke.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerStahlmuecke::GegnerStahlmuecke(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    AnimStart = 0;
    AnimEnde = 8;
    AnimSpeed = 0.5f;
    xSpeed = 0.0f;
    ySpeed = 0.0f;
    xAcc = 0.0f;
    yAcc = 0.0f;
    Energy = 10;
    Value1 = static_cast<int>(pAim->xpos) - 100 + random(200);  // Flugziel zufällig in Richtung Spieler
    Value2 = static_cast<int>(pAim->ypos) - 100 + random(200);  // setzen mit etwas Variation
    ChangeLight = Light;
    Destroyable = true;
    OwnDraw = true;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerStahlmuecke::DoDraw() {
    bool mirrored = BlickRichtung == RECHTS;

    if (Handlung != GEGNER::FALLEN)
        pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                               yPos - TileEngine.YOffset, AnimPhase, 0xFFFFFFFF,
                                               mirrored);
    else
        pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                               yPos - TileEngine.YOffset, AnimPhase, 0xFFFF0000,
                                               mirrored);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerStahlmuecke::DoKI() {
    SimpleAnimation();

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::LAUFEN:  // Warten bis der Spieler nahe genug rankommt
        {
            if ((xSpeed < 0.0f && blockl & BLOCKWERT_WAND) || (xSpeed > 0.0f && blockr & BLOCKWERT_WAND))
                xSpeed *= -1.0f;

            if (PlayerAbstand() < 500) {
                Value1 =
                    static_cast<int>(pAim->xpos) + 35 - 50 + random(100);  // Flugziel zufällig in Richtung Spieler
                Value2 = static_cast<int>(pAim->ypos) + 40 - 50 + random(100);  // setzen mit etwas Variation
                Handlung = GEGNER::VERFOLGEN;
            }
        } break;

        case GEGNER::VERFOLGEN:  // Mücke verfolgt den Spieler
        {
            // Punkt links von der Mücke
            if (Value1 < xPos) {
                if (!(blockl & BLOCKWERT_WAND)) {
                    xAcc = -3.0;
                    BlickRichtung = LINKS;
                }
            }

            // Punkt rechts von der Mücke
            if (Value1 > xPos) {
                if (!(blockr & BLOCKWERT_WAND)) {
                    xAcc = 3.0;
                    BlickRichtung = RECHTS;
                }
            }

            // Punkt oben von der Mücke
            if (Value2 < yPos) {
                if (!(blocko & BLOCKWERT_WAND))
                    yAcc = -3.0;
                else {
                    Handlung = GEGNER::VERFOLGEN;
                    ySpeed = 20;
                }
            }

            // Punkt unterhalb von der Mücke
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

            int dx = abs(static_cast<int>(xPos) - Value1);
            int dy = abs(static_cast<int>(yPos) - Value2);

            // Punkt erreicht oder Abstand zu groß ? Dann neues Ziel setzen
            if (PlayerAbstand() > 400 || (dx * dx + dy * dy) < 20 * 20)

            {
                Value1 = static_cast<int>(pAim->xpos) - 50 + random(100);  // Flugziel zufällig in Richtung Spieler
                Value2 = static_cast<int>(pAim->ypos) - 50 + random(100);  // setzen mit etwas Variation
            }

            // An die Wand gekommen ? Dann auch neues Ziel setzen
            if (blockl & BLOCKWERT_WAND || blockr & BLOCKWERT_WAND || blocko & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM) {
                Value1 =
                    static_cast<int>(pAim->xpos) + 35 - 50 + random(100);  // Flugziel zufällig in Richtung Spieler
                Value2 = static_cast<int>(pAim->ypos) + 40 - 50 + random(100);  // setzen mit etwas Variation
            }

            // Nicht ins Wasser fliegen
            if (blockl & BLOCKWERT_WASSER || blockr & BLOCKWERT_WASSER || blocko & BLOCKWERT_WASSER ||
                blocku & BLOCKWERT_WASSER) {
                Handlung = GEGNER::LAUFEN;

                if (ySpeed > 0.0f)
                    ySpeed *= -1.0f;

                xAcc = 0.0f;
                yAcc = 0.0f;
            }
        } break;

        // Mücke stürzt ab
        case GEGNER::FALLEN: {
            // An die Wand gekracht ?
            if (blockl & BLOCKWERT_WAND || blockr & BLOCKWERT_WAND || blocko & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM)
                Energy = 0.0f;

            // Grenze der Fallgeschwindigkeit
            if (ySpeed > 35.0f)
                yAcc = 0.0f;

            // Mücke rauchen lassen
            if (AnimCount == 0.0f && AnimPhase % 2 == 0)
                PartikelSystem.PushPartikel(xPos + 12, yPos + 5, SMOKE);
        } break;

        default:
            break;
    }  // switch

    // Testen, ob der Spieler die Mücke berührt hat
    if (Handlung != GEGNER::FALLEN)
        TestDamagePlayers(1.5f SYNC);

    // Soviel Energie verloren, dass die Mücke abstürzt ?
    if (Energy <= 0.0f && Handlung != GEGNER::FALLEN) {
        Handlung = GEGNER::FALLEN;
        Energy = 30.0f;
        ySpeed = 3.0f;
        xAcc = 0.0f;
        yAcc = 2.0f;

        PartikelSystem.PushPartikel(xPos + 2.0f, yPos - 10.0f, EXPLOSION_MEDIUM3);

        for (int i = 0; i < 5; i++)
            PartikelSystem.PushPartikel(xPos + 2.0f + static_cast<float>(random(20)),
                                        yPos - 10.0f + static_cast<float>(random(10)), SMOKE2);

        if (BlickRichtung == LINKS)
            xSpeed = -5.0f;
        else
            xSpeed = 5.0f;
    }
}

// --------------------------------------------------------------------------------------
// Explodieren
// --------------------------------------------------------------------------------------

void GegnerStahlmuecke::GegnerExplode() {
    // Explosion
    PartikelSystem.PushPartikel(xPos + 2.0f, yPos - 10.0f, EXPLOSION_MEDIUM3);

    // Rauchende Splitter
    for (int i = 0; i < 3; i++)
        PartikelSystem.PushPartikel(xPos + 15.0f, yPos + 20.0f, SPLITTER);

    SoundManager.PlayWave(100, 128, -random(2000) + 11025, SOUND::EXPLOSION1);  // Sound ausgeben

    Player[0].Score += 200;
}
