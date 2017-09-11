// --------------------------------------------------------------------------------------
// Der Fiese Walker mit dem Flammenwerfer
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_FieserFireWalker.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFieserFireWalker::GegnerFieserFireWalker (int  Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_LAUFEN;
    Energy			= 120;
    Value1			= Wert1;
    Value2			= Wert2;
    ChangeLight		= Light;
    Destroyable		= true;
    AnimSpeed       = 0.5f;
    AnimEnde		= 14;
    xSpeed			= 5.0f;
    ShotDelay		= 30.0f;
    ShotDelay2		= 0.0f;
    OwnDraw			= true;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerFieserFireWalker::DoDraw(void)
{
    // Gegner rendern
    int  Wert = 255 - (int)(DamageTaken);
    bool mirror;

    if (BlickRichtung == LINKS)
        mirror = false;
    else
        mirror = true;

    D3DCOLOR Color = D3DCOLOR_RGBA(255, Wert, Wert, 255);
    pGegnerGrafix[GegnerArt]->RenderSprite ((float)(xPos-TileEngine.XOffset),
                                            (float)(yPos-TileEngine.YOffset),
                                            AnimPhase, Color, mirror);

    if (AlreadyDrawn)
        return;

    if (Handlung == GEGNER_STEHEN)
    {
        // Leuchten beim Schiessen rendern
        //
        DirectGraphics.SetAdditiveMode ();
        Projectiles.LavaFlare.RenderSprite (float (xPos - TileEngine.XOffset - 30) + 6 + BlickRichtung * 36,
                                float (yPos - TileEngine.YOffset - 30), 0,
                                0xFFFF8822);
        DirectGraphics.SetColorKeyMode ();
    }

    AlreadyDrawn = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFieserFireWalker::DoKI(void)
{
    SimpleAnimation ();

    // Je nach Handlung anders verhalten
    //
    switch (Handlung)
    {
    // rumhopsen
    //
    case GEGNER_LAUFEN:
    {
        if (!(blocku & BLOCKWERT_WAND) &&
                !(blocku & BLOCKWERT_PLATTFORM))
        {
            Handlung = GEGNER_FALLEN;
            ySpeed   = 2.0f;
            yAcc	 = 3.0f;
        }

        // nahe genug zum schiessen?
        //
        if (ShotDelay <= 0.0f &&
                AnimPhase == 3 &&
                PlayerAbstand() < 400 &&
                PlayerAbstandVert () < 150 &&
                ((BlickRichtung == RECHTS && xPos+GegnerRect[GegnerArt].right < pAim->xpos + pAim->CollideRect.left) ||
                 (BlickRichtung == LINKS  && xPos+GegnerRect[GegnerArt].left  > pAim->xpos + pAim->CollideRect.right))
           )
        {
            ShotDelay  = 40.0f;
            ShotDelay2 = 0.0f;
            Handlung  = GEGNER_STEHEN;
            xSpeed    = 0.0f;
            AnimEnde  = 0;

            SoundManager.PlayWave (100, 128, 11025, SOUND_FEUERFALLE);
        }

        if (ShotDelay > 0.0f)
            ShotDelay -= 1.0f SYNC;

        if (TurnonShot())
            ShotDelay = -1.0f;

    }
    break;

    // Flammenwerfer schiessen
    //
    case GEGNER_STEHEN:
    {
        if (ShotDelay2 > 0.0f)
            ShotDelay2 -= 1.0f SYNC;

        if (ShotDelay2 <= 0.0f)
        {
            ShotDelay2 = 0.4f;
            ShotDelay -= 1.0f;

            Projectiles.PushProjectile  (xPos + 5 + BlickRichtung * 38, yPos - 7, WALKERFIRE, pAim);
        }

        // Spieler nicht mehr vor dem Walker? Dann auch nicht mehr schiessen
        if ((BlickRichtung == RECHTS && xPos+60 >= pAim->xpos) ||
                (BlickRichtung == LINKS  && xPos    <= pAim->xpos + 80))
            ShotDelay = -1.0f;

        if (ShotDelay < 0.0f)
        {
            ShotDelay = 20.0f;
            Handlung = GEGNER_LAUFEN;
            AnimEnde = 14;
            xSpeed = 5.0f * BlickRichtung;
        }

    }
    break;

    case GEGNER_FALLEN:
    {
        if (ySpeed > 35.0f)
        {
            ySpeed = 35.0f;
            yAcc   = 0.0f;
        }

        if (blocku & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_PLATTFORM)
        {
            ySpeed = 0.0f;
            yAcc   = 0.0f;
            Handlung = GEGNER_LAUFEN;
        }

    }
    break;

    }

    TurnonWall();

    // Spieler berührt den Gegner?
    //
    TestDamagePlayers(4.0f SYNC);
}

// --------------------------------------------------------------------------------------
// FieserFireWalker explodiert
// --------------------------------------------------------------------------------------

void GegnerFieserFireWalker::GegnerExplode(void)
{
    PartikelSystem.PushPartikel(xPos - 30, yPos - 30, EXPLOSION_BIG);

    for (int i = 0; i < 8; i++)
        PartikelSystem.PushPartikel(xPos - 30 + rand ()% 60,
                                      yPos - 30 + rand ()% 60, EXPLOSION_MEDIUM2);
    for (int i = 0; i < 12; i++)
        PartikelSystem.PushPartikel(xPos + rand ()% 50,
                                      yPos + rand ()% 50, SPIDERSPLITTER);


    SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION4);	// Sound ausgeben

    Player[0].Score += 80;
}
