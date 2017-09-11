// --------------------------------------------------------------------------------------
// Die Kletter Spinne
//
// Klettert hinten an der Wand rum und dreht sich in Richtung Hurri
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Climber.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerClimber::GegnerClimber(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_INIT;
    Energy			= 20;
    AnimSpeed		= 0.5f;
    ChangeLight		= Light;
    Destroyable		= true;
    AnimPhase		= 3;
    AnimStart		= 3;
    AnimEnde		= 13;
    Value1			= Wert1;
    Value2			= Wert2;
    rot				= 0.0f;
    shotdelay		= 10.0f;
    OwnDraw			= true;

    // Aus der Spinnenmaschine? Dann anfangs per Zufall drehen
    if (Wert1 == 99)
    {
        rot = (float)((int)(360.0f + rand()%50 - 50) % 360);
    }

    rotspeed = (float)(rand()%8 + 3) / 2.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerClimber::DoDraw(void)
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

void GegnerClimber::DoKI(void)
{
    blocku = TileEngine.BlockUntenNormal (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

    // Animationscounter weiterzählen
    AnimCount += SpeedFaktor;

    if (AnimCount > AnimSpeed)			// Grenze überschritten ?
    {
        AnimCount = 0;					// Dann wieder auf Null setzen

        if (Handlung == GEGNER_LAUFEN)
        {
            AnimPhase--;					// Und nächste Animationsphase
            if (AnimPhase <= AnimStart)		// Animation von zu Ende	?
                AnimPhase = AnimEnde;		// Dann wieder von vorne beginnen
        }
        else if (Handlung == GEGNER_SCHIESSEN)
        {
            AnimPhase++;					// Und nächste Animationsphase
            if (AnimPhase >= AnimEnde)		// Animation von zu Ende? Dann wieder laufen
            {
                Handlung = GEGNER_LAUFEN;
                AnimPhase		= 3;
                AnimStart		= 3;
                AnimEnde		= 13;
            }
        }
    } // animieren

    // Je nach Handlung richtig verhalten
    switch (Handlung)
    {
    // Gegner zu Beginn einmalig auf Spieler ausrichten, falls nicht Value1 auf 99 steht
    //
    case GEGNER_INIT:
    {
        if (Value1 != 99)
        {
            float dx, dy;

            // Abstände berechnen
            dx = (xPos + 25) - (pAim->xpos + 35);
            dy = (yPos + 18) - (pAim->ypos + 20);

            // Division durch Null verhinden
            if (dy == 0.0f)
                dy = 0.01f;

            //DKS - fixed uninitialized var warning:
            //float w, winkel;

            //DKS - converted to float, macro:
            //w = float(atan(dx / dy) * 360.0f / (D3DX_PI * 2));
            float w = RadToDeg(atanf(dx / dy));
            float winkel = w;

            if (dx >= 0 && dy >= 0) winkel = w;
            else if (dx > 0  && dy < 0 ) winkel = 180 + w;
            else if (dx < 0  && dy > 0 ) winkel = 360 + w;
            else if (dx < 0  && dy < 0 ) winkel = 180 + w;

            winkel = 360.0f - winkel;
            rot = winkel;
        }
        else
            rot = 0;

        Handlung = GEGNER_LAUFEN;

    }
    break;

    case GEGNER_LAUFEN:
    {
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

        //DKS - converted to float:
        //w = float(atan(dx / dy) * 360.0f / (D3DX_PI * 2));
        float w = RadToDeg(atanf(dx / dy));
        float winkel = w;

        if (dx >= 0 && dy >= 0) winkel = w;
        else if (dx > 0  && dy < 0 ) winkel = 180 + w;
        else if (dx < 0  && dy > 0 ) winkel = 360 + w;
        else if (dx < 0  && dy < 0 ) winkel = 180 + w;

        winkel = 360.0f - winkel;

        // Spinne aus der Spinnenmaschine erst ein stück nach rechts laufen lassen
        if (Value1 == 99)
        {
            if (xPos - TileEngine.XOffset < 250 &&
                    winkel > 75)
                winkel = 75;
        }

        // Spinne rotieren
        //
        float inc = rotspeed;
        if (PlayerAbstand() > 150) inc = rotspeed * 2.0f;
        if (PlayerAbstand() > 250) inc = rotspeed * 3.0f;

        // An der Wand schneller rotieren
        if (blocko & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_WAND ||
                blockl & BLOCKWERT_WAND ||
                blockr & BLOCKWERT_WAND)
            inc *= 5.0f;

        if (rot < winkel) rot += inc SYNC;
        if (rot > winkel) rot -= inc SYNC;

        // Direkt in Richtung Spieler kucken?
        if (PlayerAbstand() > 300 &&
                Value1 != 99)
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

        // An der Wand anhalten rotieren
        if (xSpeed < 0.0f &&
                (blockl & BLOCKWERT_WAND ||
                 blockl & BLOCKWERT_GEGNERWAND))
            xSpeed = 0.0f;

        if (xSpeed > 0.0f &&
                (blockr & BLOCKWERT_WAND ||
                 blockr & BLOCKWERT_GEGNERWAND))
            xSpeed = 0.0f;

        if (ySpeed < 0.0f &&
                (blocko & BLOCKWERT_WAND ||
                 blocko & BLOCKWERT_GEGNERWAND))
            ySpeed = 0.0f;

        if (ySpeed > 0.0f &&
                (blocku & BLOCKWERT_WAND ||
                 blocku & BLOCKWERT_GEGNERWAND))
            ySpeed = 0.0f;

        shotdelay -= 1.0f SYNC;

        // Schusscounter unten und Blickwinkel in Richtung Spieler ?
        // Dann schiessen
        if (shotdelay < 0.0f			  &&
                Value1 != 99					  &&
                abs (int (winkel - rot)) < 10 &&
                PlayerAbstand() < 350)
        {
            shotdelay = float (rand()%10 + 10);
            Handlung  = GEGNER_SCHIESSEN;
            AnimPhase = 0;
            AnimEnde  = 3;
            AnimStart = 0;
            xSpeed	  = 0.0f;
            ySpeed	  = 0.0f;

            Projectiles.PushProjectile (xPos + 21, yPos + 13, SUCHSCHUSS);
            SoundManager.PlayWave(100, 128, 15000+rand()%2000, SOUND_CANON);
        }
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
                blockr & BLOCKWERT_WAND)
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
// Climber explodiert
// --------------------------------------------------------------------------------------

void GegnerClimber::GegnerExplode(void)
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
