// --------------------------------------------------------------------------------------
// Extra-Leben
//
// Bringt beim Einsammeln ein Extra Leben, was sonst
// --------------------------------------------------------------------------------------

#include "Gegner_OneUp.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerOneUp::GegnerOneUp(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::FALLEN;  // Zu Beginn fällt das Extraleben
    Energy = 1.0f;
    Value1 = Wert1;
    Value2 = Wert2;

    if (Value2 != 99)
        yAcc = 2.0f;

    AnimSpeed = 0.75f;
    ChangeLight = Light;
    Destroyable = false;
    AnimEnde = 20;
    AnimSpeed = 0.5f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerOneUp::DoKI() {
    SimpleAnimation();

    // auf dem Fliessband bewegen
    if (blocku & BLOCKWERT_FLIESSBANDL)
        xSpeed = -11.0f;

    if (blocku & BLOCKWERT_FLIESSBANDR)
        xSpeed = 11.0f;

    BlickRichtung = LINKS;

    switch (Handlung) {
        case GEGNER::FALLEN:  // Leben fällt runter
        {
            if (ySpeed > 50.0f)  // Fall-Limit nicht überschreiten
                ySpeed = 50.0f;
            // Am Boden abhopfen ?
            if (TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]) & BLOCKWERT_WAND ||
                TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]) &
                    BLOCKWERT_PLATTFORM) {
                yPos -= 1.0f;
                ySpeed = -ySpeed * 0.2f;
                if (ySpeed > -0.5f)  // Wieder abspringen oder liegenbleiben ?
                {
                    xSpeed = 0.0f;
                    ySpeed = 0.0f;
                    yAcc = 0.0f;
                    Handlung = GEGNER::SPECIAL;
                }
            }
        } break;

        case GEGNER::STEHEN: {
            // Wieder runterfallen
            //
            if (!(TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]) & BLOCKWERT_WAND) &&
                !(TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]) &
                  BLOCKWERT_PLATTFORM)) {
                Handlung = GEGNER::FALLEN;
                yAcc = 2.0f;
            }
        } break;

    }  // switch

    // Testen, ob der Spieler das Extraleben eingesammelt hat
    for (int i = 0; i < NUMPLAYERS; i++)
        if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], Player[i].xpos, Player[i].ypos, Player[i].CollideRect) ==
            true) {
            Player[0].Score += 1000;  // Punkte vergeben
            Player[i].Lives++;        // Anzahl der Leben erhöhen
            Player[0].LivesFullGame++;
            Player[0].LivesThisLevel++;
            Energy = 0.0f;  // Und Leben verschwinden lassen

            PartikelSystem.PushPartikel(xPos - 40.0f, yPos - 40.0f, EXPLOSIONFLARE);
            PartikelSystem.PushPartikel(xPos - 40.0f, yPos - 40.0f, EXPLOSIONFLARE);

            for (int j = 0; j < 200; j++)
                PartikelSystem.PushPartikel(xPos + 20.0f, yPos + 20.0f, KRINGELSECRET);
        }
}

// --------------------------------------------------------------------------------------
// Extra Leben verschwindet
// --------------------------------------------------------------------------------------

void GegnerOneUp::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 11025, SOUND::COLLECT);          // Sound ausgeben
    SoundManager.PlayWave(100, 128, 11025, SOUND::VOICE_EXTRALIFE);  // Sound ausgeben
}
