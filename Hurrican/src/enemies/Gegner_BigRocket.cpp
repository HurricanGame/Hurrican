// --------------------------------------------------------------------------------------
// Die fette Rakete
//
// fliegt im Flugsack Level von unten nach oben rauf
// --------------------------------------------------------------------------------------

#include "Gegner_BigRocket.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerBigRocket::GegnerBigRocket(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_STEHEN;
    Energy = 1000;
    ChangeLight = Light;
    Destroyable = true;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimSpeed = 1.5f;
    AnimEnde = 15;
    TestBlock = false;
    OwnDraw = true;
    smokedelay = 0.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerBigRocket::DoDraw() {
    pGegnerGrafix[GegnerArt]->RenderSprite(static_cast<float>(xPos - TileEngine.XOffset),
                                           static_cast<float>(yPos - TileEngine.YOffset), AnimPhase, 0xFFFFFFFF, false);

    // Noch warten?
    //
    if (Handlung == GEGNER_STEHEN)
        return;

    // Im Flug noch den Schweif anzeigen
    //
    if (options_Detail >= DETAIL_HIGH) {
        DirectGraphics.SetAdditiveMode();
        Projectiles.LavaFlare.RenderSprite(static_cast<float>(xPos - 27 - TileEngine.XOffset),
                                           static_cast<float>(yPos + 237 - TileEngine.YOffset), 0, 0xCCFFFFFF);
        Projectiles.LavaFlare.RenderSprite(static_cast<float>(xPos - 27 - TileEngine.XOffset),
                                           static_cast<float>(yPos + 237 - TileEngine.YOffset), 0, 0xCCAAAA00);
        Projectiles.LavaFlare.RenderSprite(static_cast<float>(xPos - 27 - TileEngine.XOffset),
                                           static_cast<float>(yPos + 237 - TileEngine.YOffset), 0, 0xCCAA0000);
        DirectGraphics.SetColorKeyMode();
    }
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerBigRocket::DoKI() {
    SimpleAnimation();

    switch (Handlung) {
        // Rakete wird "aktiviert" (unter den Hurri gesetzt, damit sie von unten nach oben fliegen kann)
        case GEGNER_STEHEN: {
            Handlung = GEGNER_FALLEN;
            yPos += 480.0f + 300.0f;
            ySpeed = -static_cast<float>(Value1 + 7);

            Value2 = static_cast<int>(xPos);
        } break;

        // Rakete fliegt
        case GEGNER_FALLEN: {
            xPos = static_cast<float>(Value2) + ((static_cast<int>(yPos) % 14) / 7);

            smokedelay -= 1.0f SYNC;

            if (smokedelay <= 0.0f) {
                smokedelay += 0.2f;

                PartikelSystem.PushPartikel(xPos + 20 + random(10), yPos + 290 + random(5), SMOKE3_U);
                PartikelSystem.PushPartikel(xPos + 20 + random(13), yPos + 290 + random(5), ROCKETSMOKE);
            }

            blocko = TileEngine.BlockOben(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

            // ausserhalb des Screens? Dann verschwinden lassen
            if (yPos + 350.0f < TileEngine.YOffset || blocko & BLOCKWERT_WAND)
                Energy = 0.0f;

        } break;

        default:
            break;
    }  // switch

    // Testen, ob der Spieler die Rakete berührt hat
    TestDamagePlayers(30.0f, true);
}

// --------------------------------------------------------------------------------------
// BigRocket explodiert
// --------------------------------------------------------------------------------------

void GegnerBigRocket::GegnerExplode() {
    // Nur explodieren lassen, wenn Rakete zu sehen ist
    //
    if (!(yPos + 350.0f < TileEngine.YOffset)) {
        ShakeScreen(5.0f);

        SoundManager.PlayWave(100, 128, 11025, SOUND_EXPLOSION1);
        SoundManager.PlayWave(100, 128, 11025, SOUND_EXPLOSION3);

        for (int i = 0; i < 30; i++) {
            PartikelSystem.PushPartikel(xPos + random(60), yPos + 30 + random(250), EXPLOSION_MEDIUM2);
            PartikelSystem.PushPartikel(xPos + 10 + random(50), yPos + 30 + random(250), SPIDERSPLITTER);
        }

        for (int i = 0; i < 80; i++) {
            PartikelSystem.PushPartikel(xPos + 10 + random(50), yPos + 30 + random(250), MINIFLARE);
            PartikelSystem.PushPartikel(xPos + 10 + random(50), yPos + 30 + random(250), LONGFUNKE);
        }

        Player[0].Score += 2000;
    }
}
