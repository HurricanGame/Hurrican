// --------------------------------------------------------------------------------------
// Das SchienenViech
//
// Läuft auf Schienen entlang auf den Hurri zu
// --------------------------------------------------------------------------------------

#include "Gegner_SchienenViech.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchienenViech::GegnerSchienenViech(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_LAUFEN;
    Energy = 80;
    AnimSpeed = 1.0f;
    AnimEnde = 4;
    ChangeLight = Light;
    Destroyable = true;
    TestBlock = false;
    OwnDraw = true;
    xAcc = 4.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Drawfunktion
// --------------------------------------------------------------------------------------

void GegnerSchienenViech::DoDraw() {

    D3DCOLOR col = 0xFFCCCCCC;

    // Draw kommt jetzt erst hier, damit der Gegner über allen Leveltiles gerendert wird
    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                           yPos - TileEngine.YOffset, AnimPhase, col);

    AlreadyDrawn = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSchienenViech::DoKI() {
    // temporäre position für Gegnerwand Check (Wandcheck komplett rausgenmommen, damit er nicht in der
    // Schiene stecken bleibt ;)
    //
    float x = xPos;
    float y = yPos;
    blockl = TileEngine.BlockLinks(x, y, x, y, GegnerRect[GegnerArt]);
    blockr = TileEngine.BlockRechts(x, y, x, y, GegnerRect[GegnerArt]);

    if ((((blockl & BLOCKWERT_GEGNERWAND) || (blockl & BLOCKWERT_WAND)) && xSpeed < 0.0f) ||
        (((blockr & BLOCKWERT_GEGNERWAND) || (blockl & BLOCKWERT_WAND)) && xSpeed > 0.0f)) {
        Handlung = GEGNER_LAUFEN;
        xSpeed *= -1.0f;
        xAcc *= -1.0f;
    }

    AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
    if (AnimCount > AnimSpeed)  // Grenze überschritten ?
    {
        AnimCount = 0;  // Dann wieder auf Null setzen

        // Vorwärts oder rückwärts animieren, je nachdem, in welche Richtung das Viech gerade fährt
        //
        if (xSpeed < 0.0f) {
            AnimPhase++;                // Und nächste Animationsphase
            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
            {
                AnimPhase = AnimStart;  // Dann wieder von vorne beginnen

                // Schiessen
                //
                if (pAim->ypos < yPos)
                    Projectiles.PushProjectile(xPos + 18.0f, yPos - 8.0f, STRAIGHTSCHUSS);
                else
                    Projectiles.PushProjectile(xPos + 18.0f, yPos + 48.0f, STRAIGHTSCHUSS2);
            }
        } else {
            AnimPhase--;                // Und vorherige Animationsphase
            if (AnimPhase < AnimStart)  // Animation von zu Ende	?
            {
                AnimPhase = AnimEnde;  // Dann wieder von hinten her beginnen

                // Schiessen
                //
                if (pAim->ypos < yPos)
                    Projectiles.PushProjectile(xPos + 18.0f, yPos - 8.0f, STRAIGHTSCHUSS);
                else
                    Projectiles.PushProjectile(xPos + 18.0f, yPos + 48.0f, STRAIGHTSCHUSS2);
            }
        }
    }  // animieren

    switch (Handlung) {
        // Spieler nicht verfolgen
        case GEGNER_LAUFEN: {
            // Spieler in Fahrtrichtung gesehen? Dann wieder verfolgen
            if ((xSpeed > 0.0f && pAim->xpos + 35 > xPos + 24) || (xSpeed < 0.0f && pAim->xpos + 35 < xPos + 24))
                Handlung = GEGNER_VERFOLGEN;

        } break;

        // Spieler verfolgen
        case GEGNER_VERFOLGEN: {
            if (PlayerAbstandHoriz() < 250) {
                if (pAim->xpos + 35 > xPos + 24)
                    xAcc = 4.0f;
                else
                    xAcc = -4.0f;
            }
        } break;
    }

    // Verfolgungsspeed begrenzen wenn er auf den Spieler zufährt
    //
    if (xSpeed > 20.0f)
        xSpeed = 20.0f;
    if (xSpeed < -20.0f)
        xSpeed = -20.0f;

    // ansonten Verfolgungsspeed begrenzen wenn er am Spieler dran ist
    //
    if (PlayerAbstandHoriz() < 60) {
        if (xSpeed > 15.0f) {
            xAcc = 0.0f;
            xSpeed = 15.0f;
        }
        if (xSpeed < -15.0f) {
            xAcc = 0.0f;
            xSpeed = -15.0f;
        }
    }
}

// --------------------------------------------------------------------------------------
// SchienenViech explodiert
// --------------------------------------------------------------------------------------

void GegnerSchienenViech::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND_EXPLOSION3);

    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(30)),
                                    yPos - 30.0f + static_cast<float>(random(40)), EXPLOSION_MEDIUM2);

    Player[0].Score += 500;
}
