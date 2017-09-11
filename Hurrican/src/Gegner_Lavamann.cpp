// --------------------------------------------------------------------------------------
// Der Lavamann
//
// Rennt auf den Hurri zu und explodiert dann, zieht eine Feuerspur hinter sich her
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Lavamann.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerLavamann::GegnerLavamann(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_INIT;
    AnimStart		= 0;
    AnimEnde		= 20;
    AnimSpeed		= 0.15f;
    xSpeed			= 24.0f;
    ySpeed			=  0.0f;
    Energy			= 20;
    Value1			= Wert1;
    Value2			= Wert2;
    ChangeLight		= false;
    Destroyable		= true;
    OwnDraw			= true;
    FlameCount		= 0.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerLavamann::DoDraw(void)
{
    bool mirror = false;

    if (BlickRichtung == LINKS)
        mirror = true;
    else
        mirror = false;

    // gegner leuchtend rendern
    //
    DirectGraphics.SetAdditiveMode();
    pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-TileEngine.XOffset),
                                           (float)(yPos-TileEngine.YOffset), AnimPhase, 0xFFFFFFFF, mirror);
    DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerLavamann::DoKI(void)
{
    SimpleAnimation();

    // Schrägen checken
    //
    if (ySpeed >= 0.0f)
        //DKS - Rewrote BlockSlopes function to only take the parameters it uses:
        //blocku = TileEngine.BlockSlopes(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt], ySpeed);
        blocku = TileEngine.BlockSlopes(xPos, yPos, GegnerRect[GegnerArt], ySpeed);

    // Flammenwand entstehen lassen
    //
    FlameCount -= 1.0f SYNC;
    if (FlameCount < 0.0f)
    {
        FlameCount = 0.5f;
        Projectiles.PushProjectile(xPos - 10 - xSpeed * 0.75f, yPos, FEUERFALLE_LAVAMANN);
    }

    blocku = TileEngine.BlockUnten (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

    // In Richtung Spieler laufen, wenn angeschossen
    //
    if (DamageTaken > 0)
    {
        if (pAim->xpos < xPos)
        {
            BlickRichtung = LINKS;
            xSpeed = -24.0f;
        }
        else
        {
            BlickRichtung = RECHTS;
            xSpeed = 24.0f;
        }
    }

    // runterfallen?
    //
    if (Handlung != GEGNER_STEHEN)
    {
        if (!(blocku & BLOCKWERT_SCHRAEGE_R) &&
                !(blocku & BLOCKWERT_SCHRAEGE_L))
        {
            blocku = TileEngine.BlockUnten (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

            if (!(blocku & BLOCKWERT_WAND) &&
                    !(blocku & BLOCKWERT_PLATTFORM))
            {
                if (ySpeed == 0.0f)
                    ySpeed = 5.0f;

                yAcc = 3.0f;
            }

            else
            {
                ySpeed = 0.0f;
                yAcc   = 0.0f;
            }

            // Nach links bzw rechts auf Kollision prüfen und ggf umdrehen
            if (BlickRichtung == LINKS)
                if (blockl & BLOCKWERT_WAND ||
                        blockl & BLOCKWERT_GEGNERWAND)
                {
                    BlickRichtung = RECHTS;
                    xSpeed *= -1.0f;
                }

            if (BlickRichtung == RECHTS)
                if (blockr & BLOCKWERT_WAND ||
                        blockr & BLOCKWERT_GEGNERWAND)
                {
                    BlickRichtung = LINKS;
                    xSpeed *= -1.0f;
                }
        }
    }


// Je nach Handlung richtig verhalten
    switch (Handlung)
    {
    // Zum ersten mal im Screen?
    // Dann Sound ausgeben
    //
    case GEGNER_INIT:
    {
        SoundManager.PlayWave(100, 128, 10000 + rand()%1000, SOUND_FEUERFALLE);
        Handlung = GEGNER_LAUFEN;
    }
    break;

    case GEGNER_FALLEN:						// Normal runterfallen
    {
        // Keine zu hohe Geschwindigkeit
        if (ySpeed > 25.0f)
            ySpeed = 25.0f;

        // Testen, ob der Lavamann auf den Boden kommt
        if (blocku & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_PLATTFORM)
        {
            Handlung = GEGNER_LAUFEN;
            yAcc	  = 0.0f;
            ySpeed	  = 0.0f;
        }
    }
    break;

    default :
        break;
    } // switch

    // Testen, ob der Spieler den Lavamann berührt hat
    if (Handlung != GEGNER_WATSCHELN &&
            Handlung != GEGNER_SPRINGEN)
        TestDamagePlayers(35.0f, true);
}

// --------------------------------------------------------------------------------------
// Lavamann explodiert
// --------------------------------------------------------------------------------------

void GegnerLavamann::GegnerExplode(void)
{
    for (int i=0; i<8; i++)
    {
        PartikelSystem.PushPartikel(float(xPos - 30 + rand()%40),
                                      float(yPos - 30 + rand()%50), SMOKEBIG);

        PartikelSystem.PushPartikel(float(xPos - 10 + rand()%40),
                                      float(yPos - 10 + rand()%50), SMOKE3);

        PartikelSystem.PushPartikel(float(xPos - 60 + rand()%80),
                                      float(yPos - 50 + rand()%60), EXPLOSIONFLARE);

        PartikelSystem.PushPartikel(float(xPos - 40 + rand()%40),
                                      float(yPos - 30 + rand()%50), EXPLOSION_MEDIUM2);

        PartikelSystem.PushPartikel(float(xPos - 20 + rand()%50),
                                      float(yPos - 20 + rand()%50), MINIFLARE);
    }

    SoundManager.PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION1);	// Sound ausgeben

    Player[0].Score += 350;
}
