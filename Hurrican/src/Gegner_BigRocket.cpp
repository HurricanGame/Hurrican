// --------------------------------------------------------------------------------------
// Die fette Rakete
//
// fliegt im Flugsack Level von unten nach oben rauf
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_BigRocket.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerBigRocket::GegnerBigRocket(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_STEHEN;
    Energy			= 1000;
    ChangeLight		= Light;
    Destroyable		= true;
    Value1			= Wert1;
    Value2			= Wert2;
    AnimSpeed		= 1.5f;
    AnimEnde		= 15;
    TestBlock		= false;
    OwnDraw		    = true;
    smokedelay		= 0.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerBigRocket::DoDraw(void)
{
    pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-TileEngine.XOffset),
                                           (float)(yPos-TileEngine.YOffset),
                                           AnimPhase, 0xFFFFFFFF, false);

    // Noch warten?
    //
    if (Handlung == GEGNER_STEHEN)
        return;

    // Im Flug noch den Schweif anzeigen
    //
    if (options_Detail >= DETAIL_HIGH)
    {
        DirectGraphics.SetAdditiveMode ();
        pProjectiles->LavaFlare.RenderSprite (float (xPos - 27 - TileEngine.XOffset),
                                float (yPos + 237 - TileEngine.YOffset), 0, 0xCCFFFFFF);
        pProjectiles->LavaFlare.RenderSprite (float (xPos - 27 - TileEngine.XOffset),
                                float (yPos + 237 - TileEngine.YOffset), 0, 0xCCAAAA00);
        pProjectiles->LavaFlare.RenderSprite (float (xPos - 27 - TileEngine.XOffset),
                                float (yPos + 237 - TileEngine.YOffset), 0, 0xCCAA0000);
        DirectGraphics.SetColorKeyMode ();
    }
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerBigRocket::DoKI(void)
{
    SimpleAnimation();

    switch (Handlung)
    {
    // Rakete wird "aktiviert" (unter den Hurri gesetzt, damit sie von unten nach oben fliegen kann)
    case GEGNER_STEHEN:
    {
        Handlung = GEGNER_FALLEN;
        yPos  += 480.0f + 300.0f;
        ySpeed = -float (Value1 + 7);

        Value2 = int (xPos);
    }
    break;

    // Rakete fliegt
    case GEGNER_FALLEN:
    {
        xPos = float (Value2) + ((int (yPos) % 14) / 7);

        smokedelay -= 1.0f SYNC;

        if (smokedelay <= 0.0f)
        {
            smokedelay += 0.2f;

            pPartikelSystem->PushPartikel (xPos + 20 + rand ()%10, yPos + 290 + rand()%5, SMOKE3_U);
            pPartikelSystem->PushPartikel (xPos + 20 + rand ()%13, yPos + 290 + rand()%5, ROCKETSMOKE);
        }

        blocko = TileEngine.BlockOben (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

        // ausserhalb des Screens? Dann verschwinden lassen
        if (yPos + 350.0f < TileEngine.YOffset ||
                blocko & BLOCKWERT_WAND)
            Energy = 0.0f;

    }
    break;

    default :
        break;
    } // switch

    // Testen, ob der Spieler die Rakete berührt hat
    TestDamagePlayers(30.0f, true);
}

// --------------------------------------------------------------------------------------
// BigRocket explodiert
// --------------------------------------------------------------------------------------

void GegnerBigRocket::GegnerExplode(void)
{
    // Nur explodieren lassen, wenn Rakete zu sehen ist
    //
    if (!(yPos + 350.0f < TileEngine.YOffset))
    {
        ShakeScreen (5.0f);

        SoundManager.PlayWave (100, 128, 11025, SOUND_EXPLOSION1);
        SoundManager.PlayWave (100, 128, 11025, SOUND_EXPLOSION3);

        for (int i=0; i < 30; i++)
        {
            pPartikelSystem->PushPartikel (xPos + rand ()%60, yPos + 30 + rand()%250, EXPLOSION_MEDIUM2);
            pPartikelSystem->PushPartikel (xPos + 10 + rand ()%50, yPos + 30 + rand()%250, SPIDERSPLITTER);
        }

        for (int i=0; i < 80; i++)
        {
            pPartikelSystem->PushPartikel (xPos + 10 + rand ()%50, yPos + 30 + rand()%250, MINIFLARE);
            pPartikelSystem->PushPartikel (xPos + 10 + rand ()%50, yPos + 30 + rand()%250, LONGFUNKE);
        }

        Player[0].Score += 2000;
    }
}
