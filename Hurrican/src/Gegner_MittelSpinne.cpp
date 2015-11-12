// --------------------------------------------------------------------------------------
// Die mittelgro�e Spinne
//
// Klettert beim Wuxe Spinnenansturm von der Decke
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_MittelSpinne.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerMittelSpinne::GegnerMittelSpinne(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_LAUFEN;
    Energy			= 50;
    AnimSpeed		= (float)(rand()%8 + 5) / 8.0f;
    ChangeLight		= Light;
    Destroyable		= true;
    AnimEnde		= 8;
    Value1			= Wert1;
    Value2			= Wert2;
    rot				= (float)(rand()%80 + 140);
    OwnDraw			= true;
    TestBlock		= false;
    shotdelay		= 0.0f;
    Active			= true;
    yStart			= (float)(TileEngine.YOffset);
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerMittelSpinne::DoDraw(void)
{
    // Helligkeit abh�ngig vom der Entfernung zum oberen Rand berechnen, damit
    // es aussieht, als k�men die Spinnen aus der Dunkelheit
    // Ich bin schon ein Fux ;)
    //
    float light, lightred;

    //DKS - Removed superfluous "/1.0f"
    //light = (yPos - 20) - yStart / 1.0f;
    light = (yPos - 20) - yStart;
    if (light < 0.0f)	light = 0.0;
    if (light > 255.0f)	light = 255.0;

    lightred = 255 - DamageTaken;

    if (lightred > light)
        lightred = light;

    D3DCOLOR Color = D3DCOLOR_RGBA((int)(light), (int)(lightred), (int)(lightred), 255);

    // Schatten rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSpriteRotated ((float)(xPos-TileEngine.XOffset) + 6,
            (float)(yPos-TileEngine.YOffset) + 6,
            rot, AnimPhase, 0x55000000);

    // Spinne rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSpriteRotated ((float)(xPos-TileEngine.XOffset),
            (float)(yPos-TileEngine.YOffset),
            rot, AnimPhase, Color);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerMittelSpinne::DoKI(void)
{
    SimpleAnimation();

    // Je nach Handlung richtig verhalten
    switch (Handlung)
    {
    case GEGNER_LAUFEN:
    {
        // Gegner auf Spieler ausrichten
        //
        float dx, dy;

        // Abst�nde berechnen
        dx = (xPos + 25) - (pAim->xpos + 35);
        dy = (yPos + 18) - (pAim->ypos + 40);

        // Division durch Null verhinden
        if (dy == 0.0f)
            dy = 0.01f;

        //DKS - fixed uninitialized var warning:
        //float w, winkel;

        //DKS - converted to float, optimized:
        //w = float(atan(dx / dy) * 360.0f / (D3DX_PI * 2));
        float w = RadToDeg(atanf(dx / dy));
        float winkel = w;

        if (dx >= 0 && dy >= 0) winkel = w;
        else if (dx > 0  && dy < 0 ) winkel = 180 + w;
        else if (dx < 0  && dy > 0 ) winkel = 360 + w;
        else if (dx < 0  && dy < 0 ) winkel = 180 + w;

        winkel = 360.0f - winkel;

        // Spinne rotieren
        //
        float inc = 0.3f;

        if (yPos > pAim->ypos)
            inc = 0.0f;

        if (rot < winkel) rot += inc SYNC;
        if (rot > winkel) rot -= inc SYNC;

        // Bewegen
        //
        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed = float ( sin(rot * D3DX_PI / 180.0f) * 15.0f * (5.0f - AnimSpeed) SYNC);
        //ySpeed = float (-cos(rot * D3DX_PI / 180.0f) * 15.0f * (5.0f - AnimSpeed) SYNC);
        xSpeed =  sin_deg(rot) * 15.0f * (5.0f - AnimSpeed) SYNC;
        ySpeed = -cos_deg(rot) * 15.0f * (5.0f - AnimSpeed) SYNC;			

    }
    break;

    case GEGNER_FALLEN:
    {
        rot += Value2 SYNC;

        while (rot > 360.0f)
            rot -= 360.0f;

        // An Wand gestossen ?
        //
        if (blocko & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_WAND ||
                blockl & BLOCKWERT_WAND ||
                blockr & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_PLATTFORM)
            Energy = 0.0f;

        shotdelay -= 1.0f SYNC;

        if (shotdelay < 0.0f)
        {
            shotdelay = 0.2f SYNC;
            PartikelSystem.PushPartikel (xPos + 20 + rand()%5, yPos + 15 + rand ()%5, ROCKETSMOKE);
        }
    }
    break;
    }

    // Spieler ber�hrt ?
    //
    if (Handlung != GEGNER_FALLEN)
        TestDamagePlayers(4.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Explodieren
// --------------------------------------------------------------------------------------

void GegnerMittelSpinne::GegnerExplode(void)
{
    SoundManager.PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);
    PartikelSystem.PushPartikel (xPos + 5, yPos, EXPLOSION_MEDIUM3);

    for (int i=0; i < 10; i++)
        PartikelSystem.PushPartikel (xPos + rand ()%80, yPos + rand()%50, SPIDERSPLITTER);

    for (int i=0; i < 30; i++)
        PartikelSystem.PushPartikel (xPos + rand ()%80, yPos + rand()%50, FUNKE);

    for (int i=0; i < 7; i++)
        PartikelSystem.PushPartikel (xPos  - 10 + rand ()%60, yPos - 10 + rand()%60, EXPLOSION_MEDIUM2);

    Player[0].Score += 250;
}
