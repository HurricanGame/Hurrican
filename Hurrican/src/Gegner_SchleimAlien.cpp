// --------------------------------------------------------------------------------------
// Der Alien Schleimboller
//
// Hängt nur in der Gegend rum als KanonenFutter
// Besitzt evtl einen Pointer auf seinen Spawner, damit der
// Spawner weiss, wieviele children er gespawnt hat
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_SchleimAlien.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchleimAlien::GegnerSchleimAlien(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_STEHEN;
    Energy			= 30;
    HitSound		= 1;

    Value1			= Wert1;
    Value2			= Wert2;
    ChangeLight		= Light;
    Destroyable		= true;
    AnimSpeed       = ((float)(rand()%4 + 2)) / 5.0f;
    AnimEnde		= 15;
    OwnDraw			= true;

    // Hängt nur im Level rum? Dann schon Maximal Größe
    if (Wert1 == 0)
        Size = 60.0f;

    // ansonsten langsam wachsen
    else
        Size = (float)(Wert1);
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerSchleimAlien::DoDraw(void)
{
    // Je nach Größe anders gestrecht rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSpriteScaled ((float)(xPos-TileEngine.XOffset) + 30 - Size/2.0f,
            (float)(yPos-TileEngine.YOffset) + 30 - Size/2.0f,
            (int)(Size), (int)(Size), AnimPhase, 0xAAFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSchleimAlien::DoKI(void)
{
    SimpleAnimation ();

    // Wachsen lassen
    if (Size < 60.0f)
    {
        DamageTaken = 0.0f;
        Energy		= 2.0f;
        Destroyable = false;
        Size += 2.5f SYNC;
    }
    else
    {
        Destroyable = true;
        Size = 60.0f;

        PlattformTest(GegnerRect[GegnerArt]);

        for (int i = 0; i < NUMPLAYERS; i++)
            if (Player[i].AufPlattform == this)
            {
                int off;

                if (AnimPhase < 8)
                    off = - AnimPhase;
                else
                    off = AnimPhase - 15;

                Player[i].ypos = yPos - Player[i].CollideRect.bottom + GegnerRect[GegnerArt].top - off;
            }
            else
                Wegschieben(GegnerRect[GegnerArt], 0.0f);
    }

    // Energie anziehen
    TestDamagePlayers(4.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Alien SchleimBoller explodiert
// --------------------------------------------------------------------------------------

void GegnerSchleimAlien::GegnerExplode(void)
{
    for (int i = 0; i < NUMPLAYERS; i++)
        if (Player[i].AufPlattform == this)
            Player[i].AufPlattform = NULL;

    PartikelSystem.PushPartikel(xPos, yPos, EXPLOSION_ALIEN);

    for (int i = 0; i < 16; i++)
        PartikelSystem.PushPartikel(xPos + 15 + rand ()% 20,
                                      yPos + 15 + rand ()% 20, SCHLEIM2);

    SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_SCHLEIM);	// Sound ausgeben

    Player[0].Score += 120;
}
