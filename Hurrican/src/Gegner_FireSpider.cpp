// --------------------------------------------------------------------------------------
// Die Feuerspuckende Kletter Spinne
//
// Klettert hinten an der Wand rum und dreht sich in Richtung Hurri
// schiesst mit nem fiesen Flammenwerfer
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_FireSpider.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFireSpider::GegnerFireSpider(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_LAUFEN;
    Energy			= 75;
    AnimSpeed		= 0.75f;
    ChangeLight		= Light;
    Destroyable		= true;
    OwnDraw			= true;
    AnimPhase		= 0;
    AnimStart		= 0;
    AnimEnde		= 10;
    Value1			= Wert1;
    Value2			= Wert2;
    rot				= 0.0f;
    shotdelay		= 10.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerFireSpider::DoDraw(void)
{
    // Spinne rendern
    //
    pGegnerGrafix[GegnerArt]->RenderSpriteRotated ((float)(xPos-TileEngine.XOffset),
            (float)(yPos-TileEngine.YOffset),
            rot, AnimPhase, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFireSpider::DoKI(void)
{
    blocku = TileEngine.BlockUntenNormal (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
    SimpleAnimation();

    // Gegner auf Spieler ausrichten
    //
    float dx, dy;

    // Abstände berechnen
    dx = (xPos + 25) - (pAim->xpos + 35);
    dy = (yPos + 18) - (pAim->ypos + 40);

    // Division durch Null verhinden
    if (dy == 0.0f)
        dy = 0.01f;

    //DKS - fixed uninitialized var warning:
    //float w, winkel;

    //DKS - converted to float, used new macros:
    //w = float(atan(dx / dy) * 360.0f / (D3DX_PI * 2));
    float w = RadToDeg(atanf(dx / dy));
    float winkel = w;

    if (dx >= 0 && dy >= 0) winkel = w;
    else if (dx > 0  && dy < 0 ) winkel = 180 + w;
    else if (dx < 0  && dy > 0 ) winkel = 360 + w;
    else if (dx < 0  && dy < 0 ) winkel = 180 + w;

    winkel = 360.0f - winkel;

    // Je nach Handlung richtig verhalten
    switch (Handlung)
    {
    case GEGNER_LAUFEN:
    {
        // Spinne rotieren
        //
        float inc = 5.0f;
        if (PlayerAbstand() > 150) inc = 8.0f;
        if (PlayerAbstand() > 250) inc = 12.0f;
        if (rot < winkel) rot += inc SYNC;
        if (rot > winkel) rot -= inc SYNC;

        if (PlayerAbstand() > 300)
            rot = winkel;

        // Spieler entsprechend verfolgen
        //
        if (PlayerAbstand() < 1000)
        {
            //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            //xSpeed = float ( sin(rot * D3DX_PI / 180.0f) * 7.5f);
            //ySpeed = float (-cos(rot * D3DX_PI / 180.0f) * 7.5f);
            xSpeed =  sin_deg(rot) * 7.5f;
            ySpeed = -cos_deg(rot) * 7.5f;
        }
        else
        {
            xSpeed = 0;
            ySpeed = 0;
        }

        // Schusscounter unten und Blickwinkel in Richtung Spieler ?
        // Dann schiessen
        shotdelay -= 1.0f SYNC;

        if (shotdelay < 0.0f			  &&
                abs (int (winkel - rot)) < 10 &&
                PlayerAbstand() < 200)
        {
            shotdelay = 0.3f;
            Handlung  = GEGNER_SCHIESSEN;
            AnimPhase = 11;
            AnimEnde  = 11;
            AnimStart = 11;
            AnimCount = 0.0f;
            xSpeed	  = 0.0f;
            ySpeed	  = 0.0f;
        }
    }
    break;

    // ballern ?
    // solange, bis Hurri aus dem Schussfeld raus ist, oder der Counter abgelaufen ist
    //
    case GEGNER_SCHIESSEN:
    {
        shotdelay -= 1.0f SYNC;
        AnimCount += 1.0f SYNC;

        if (shotdelay <= 0.0f)
        {
            float xs, ys, r;

            xs = xPos + 8;
            ys = yPos - 13;
            r = 180.0f - rot;
            if (r < 0.0f)
                r += 360.0f;

            //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            //r  = r * D3DX_PI / 180.0f;
            //xs = float (xs + sin (r) * 30.0f);
            //ys = float (ys + cos (r) * 30.0f);
            xs = xs + sin_deg(r) * 30.0f;
            ys = ys + cos_deg(r) * 30.0f;

            Projectiles.PushProjectile  (xs, ys, SPIDERFIRE, pAim);
            shotdelay = 0.5f;
        }

        // Spieler nicht mehr im Schussfeld oder Schuss dauert schon eine Weile ?
        //
        if (abs (int (winkel - rot)) > 10 ||
                PlayerAbstand() > 350		  ||
                AnimCount > 20.0f)
        {
            Handlung = GEGNER_LAUFEN;
            AnimPhase		= 0;
            AnimStart		= 0;
            AnimEnde		= 10;
            shotdelay		= float (rand()%20) + 20.0f;
        }
    }
    break;

    // rumfliegen, weil abgeschossen ?
    //
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
                blockr & BLOCKWERT_WAND)
            Energy = 0.0f;

        shotdelay -= 1.0f SYNC;

        if (shotdelay < 0.0f)
        {
            shotdelay = 8.0f SYNC;
            PartikelSystem.PushPartikel (xPos + 35 + rand()%5, yPos + 20 + rand ()%5, ROCKETSMOKE);
            PartikelSystem.PushPartikel (xPos + 30 + rand()%5, yPos + 20 + rand ()%5, SMOKE3);
            PartikelSystem.PushPartikel (xPos + 30 + rand()%5, yPos + 20 + rand ()%5, FUNKE);
        }
    }
    break;
    }

    // Spieler berührt ?
    //
    if (Handlung != GEGNER_FALLEN)
        TestDamagePlayers(4.0f SYNC);

    // Spinne abgeknallt ?
    // Dann Explosion erzeugen und Spinne lossegeln lassen ;)
    //
    if (Energy <= 0.0f && Handlung != GEGNER_FALLEN)
    {
        Energy   = 100.0f;
        Handlung = GEGNER_FALLEN;
        xSpeed	 =  rand()%15  - 7.0f,
         ySpeed   = -(rand()%8) - 8.0f;
        yAcc     = 3.0f;

        // Drehspeed beim Runterfallen setzen
        //
        Value2	 = rand()%20 + 20;

        // evtl negativ (andere Richtung drehen)
        //
        if (rand()%2 == 0)
            Value2 *= -1;

        SoundManager.PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);
        PartikelSystem.PushPartikel (xPos + 5, yPos, EXPLOSION_MEDIUM2);

        shotdelay = 1.0f;
    }

}

// --------------------------------------------------------------------------------------
// FireSpider explodiert
// --------------------------------------------------------------------------------------

void GegnerFireSpider::GegnerExplode(void)
{
    SoundManager.PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);
    PartikelSystem.PushPartikel (xPos + 5, yPos, EXPLOSION_MEDIUM2);

    for (int i=0; i < 10; i++)
    {
        PartikelSystem.PushPartikel (xPos + rand ()%40, yPos + rand()%30, SPIDERSPLITTER);
        PartikelSystem.PushPartikel (xPos + rand ()%40, yPos + rand()%30, FUNKE);
    }

    Player[0].Score += 250;
}
