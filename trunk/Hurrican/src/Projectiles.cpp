// Datei : Projectiles.cpp

// --------------------------------------------------------------------------------------
//
// Projektile (Schüsse) für Hurrican
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "Projectiles.h"
#include "Console.h"
#include "DX8Font.h"
#include "DX8Sound.h"
#include "Gameplay.h"
#include "Gegner_Helper.h"
#include "Globals.h"
#include "Logdatei.h"
#include "Player.h"
#include "Partikelsystem.h"
#include "Tileengine.h"
#include "Timer.h"

// --------------------------------------------------------------------------------------
// Variablen
// --------------------------------------------------------------------------------------

DirectGraphicsSprite	ProjectileGrafix[MAX_SHOTGFX];	    // Grafiken der Schüsse
RECT					ShotRect[MAX_SHOTGFX];				// Rechtecke für Level Kollision
int						CurrentShotTexture;					// Aktuelle Textur der Schüsse
int						bo, bu, bl, br;						// Blockwerte um den aktuellen Schuss herum

float					WinkelUebergabe;					// Extra "Parameter" für PushProjectile


// --------------------------------------------------------------------------------------
// ProjectileKlasse Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

//DKS - Moved constructor assignments to CreateShot(), since there's no need to have 
//      a constructor when CreateShot was always called immediately after allocating a shot.
//      Constructor is now empty inline in header. The original constructor was performing
//      redundant or unnecessary assignments.
//      Note that PushBlitzBeam() is an an exception and has been modified to account
//      for this change.
#if 0
ProjectileClass::ProjectileClass(void)
{
    xPos	= 0.0f;
    yPos	= 0.0f;
    xSpeed	= 0.0f;
    ySpeed	= 0.0f;
    xAcc	= 0.0f;
    yAcc	= 0.0f;
    Counter = 0.0f;
    Winkel  = -10000.0f;					// wird von den wenigsten Schüssen verwendet
}
#endif //0

// --------------------------------------------------------------------------------------
// Schuss vom Typ "Art" erzeugen
// --------------------------------------------------------------------------------------

void ProjectileClass::CreateShot(float x, float y, int Art, PlayerClass *pTemp)
{
    xPos = x;
    yPos = y;
    xPosOld = x;
    yPosOld = y;
    ShotArt = Art;
    AnimPhase = 0;
    AnimCount = 0.0f;
    AnimEnde  = 0;		// Standardmässig von einem nicht animierten Schuss ausgehen
    AnimSpeed = 0.0f;
    BounceWalls		= false;
    CheckBlock		= true;
    HasGlow			= false;
    ExplodeOnImpact = true;
    OwnDraw			= false;

    //DKS - Moved the class's constructor assignments here, since there's no need to
    //      have a constructor when CreateShot() is always called immediately after
    //      allocating a new shot. PushBlitzBeam() is an exception and has been
    //      modified accordingly.
	xSpeed	= 0.0f;	ySpeed	= 0.0f;
	xAcc	= 0.0f;	yAcc	= 0.0f;
	Counter = 0.0f;
	Winkel  = -10000.0f;					// wird von den wenigsten Schüssen verwendet

    // Winkel zwischen 0 und 360
    while (WinkelUebergabe > 360.0f) WinkelUebergabe -= 360.0f;
    while (WinkelUebergabe < 0.0f)	 WinkelUebergabe += 360.0f;

    pParent = pTemp;

    switch (Art)
    {
    case DIAMONDSHOT:		// Diamant, der auf den Punisher fliegt
    {
        //DKS - Fixed uninitialized variable:
        //GegnerClass *pAim, *pTemp;
        GegnerClass *pAim = NULL;
        GegnerClass *pTemp = Gegner.pStart;

        while (pTemp != NULL)
        {
            if (pTemp->GegnerArt == PUNISHER)
            {
                pAim = pTemp;
                pTemp = NULL;
            }
            else
                pTemp = pTemp->pNext;
        }

        if (pAim == NULL)
            return;

        //DKS - Converted to float:
        float	absx, absy, speed;				// Variablen für die Geschwindigkeits-
        // berechnung
        absx = pAim->xPos+85-xPos;			// Differenz der x
        absy = pAim->yPos+40-yPos;			// und y Strecke

        //DKS - Converted to float:
        speed = 1.0f/sqrtf(absx*absx + absy*absy);	// Länge der Strecke berechnen
        speed = speed*85;						// Geschwindigkeit ist 4 fach

        absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
        absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

        xSpeed = float(absx);
        ySpeed = float(absy);
        //DKS - off-by-one error:
        //AnimEnde = 9;
        AnimEnde = 8;
        AnimSpeed = 0.75f;

        Damage       = 1000;
        DamagePlayer = false;
    }
    break;

    case SKELETOR_SHOT :	// Schuss vom Skeletor
    {
        Damage		 = 4;
        DamagePlayer = true;
        Winkel = WinkelUebergabe;

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 100);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 100);
        xSpeed	  = cos_deg(w) * 100.0f;
        ySpeed	  = sin_deg(w) * 100.0f;

    }
    break;

    case SPREADSHOT :	// SpreadShot des Spielers geradeaus
    {
        Damage		 = 6 - ((int)(pParent->CurrentWeaponLevel[pParent->SelectedWeapon] * 0.75f));
        DamagePlayer = false;
        Winkel = WinkelUebergabe;

        if (pParent->Blickrichtung == LINKS)
            Winkel = float ((int (Winkel) + 180) % 360);

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 50);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 50);
        xSpeed	  = cos_deg(w) * 50.0f;
        ySpeed	  = sin_deg(w) * 50.0f;

        if (w == -90)
            xSpeed = 0.0f;

    }
    break;

    case SPREADSHOT2 :	// SpreadShot des Spielers geradeaus
    {
        xPos -= 2.0f;
        yPos -= 2.0f;
        Damage		 = 6;
        DamagePlayer = false;
        Winkel = WinkelUebergabe;

        if (pParent->Blickrichtung == LINKS)
            Winkel = float ((int (Winkel) + 180) % 360);

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 50);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 50);
        xSpeed	  = cos_deg(w) * 50.0f;
        ySpeed	  = sin_deg(w) * 50.0f;

        if (w == -90)
            xSpeed = 0.0f;

    }
    break;

    case SPREADSHOTBIG :	// SpreadShot des Spielers geradeaus
    {
        Damage		 = 12;
        DamagePlayer = false;
        Winkel = WinkelUebergabe;
        HasGlow = true;

        if (pParent->Blickrichtung == LINKS)
            Winkel = float ((int (Winkel) + 180) % 360);

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 50);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 50);
        xSpeed	  = cos_deg(w) * 50.0f;
        ySpeed	  = sin_deg(w) * 50.0f;

        if (w == -90)
            xSpeed = 0.0f;

    }
    break;

    case SPREADSHOTBIG2 :	// SpreadShot des Spielers geradeaus
    {
        xPos -= 4;
        yPos -= 4;
        Damage		 = 16;
        DamagePlayer = false;
        Winkel = WinkelUebergabe;
        HasGlow = true;

        if (pParent->Blickrichtung == LINKS)
            Winkel = float ((int (Winkel) + 180) % 360);

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 50);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 50);
        xSpeed	  = cos_deg(w) * 50.0f;
        ySpeed	  = sin_deg(w) * 50.0f;

        if (w == -90)
            xSpeed = 0.0f;

    }
    break;

    case LASERSHOT :	// Laser des Spielers
    {
        Damage		 = 8;
        DamagePlayer = false;
        Winkel = WinkelUebergabe;
        HasGlow = true;

        if (pParent->Blickrichtung == LINKS)
            Winkel = float ((int (Winkel) + 180) % 360);

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 60);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 60);
        xSpeed	  = cos_deg(w) * 60.0f;
        ySpeed	  = sin_deg(w) * 60.0f;

        if (w == -90)
            xSpeed = 0.0f;

        ExplodeOnImpact = false;
    }
    break;

    case LASERSHOT2 :	// Laser des Spielers
    {
        Damage		 = 10;
        DamagePlayer = false;
        Winkel = WinkelUebergabe;
        HasGlow = true;

        if (pParent->Blickrichtung == LINKS)
            Winkel = float ((int (Winkel) + 180) % 360);

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 60);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 60);
        xSpeed	  = cos_deg(w) * 60.0f;
        ySpeed	  = sin_deg(w) * 60.0f;

        if (w == -90)
            xSpeed = 0.0f;

        //DKS - off-by-one error:
        //AnimEnde = 10;
        AnimEnde = 9;
        AnimSpeed = 0.25f;

        ExplodeOnImpact = false;
    }
    break;

    case LASERSHOTBIG :	// Riesen Laser des Spielers
    {
        Damage		 = 12;
        DamagePlayer = false;
        Winkel = WinkelUebergabe;
        HasGlow = true;

        if (pParent->Blickrichtung == LINKS)
            Winkel = float ((int (Winkel) + 180) % 360);

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 60);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 60);
        xSpeed	  = cos_deg(w) * 60.0f;
        ySpeed	  = sin_deg(w) * 60.0f;

        if (w == -90)
            xSpeed = 0.0f;

        ExplodeOnImpact = false;
    }
    break;

    case LASERSHOTBIG2 :	// Riesen Laser des Spielers
    {
        Damage		 = 16;
        DamagePlayer = false;
        Winkel = WinkelUebergabe;
        HasGlow = true;

        if (pParent->Blickrichtung == LINKS)
            Winkel = float ((int (Winkel) + 180) % 360);

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 60);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 60);
        xSpeed	  = cos_deg(w) * 60.0f;
        ySpeed	  = sin_deg(w) * 60.0f;

        if (w == -90)
            xSpeed = 0.0f;

        //DKS - off-by-one error:
        //AnimEnde = 10;
        AnimEnde = 9;
        AnimSpeed = 0.25f;

        ExplodeOnImpact = false;
    }
    break;

    case BOUNCESHOT1 :	// BounceShot in gross (teilt sich in 2 mittlere Schüsse auf)
    {
        Winkel = WinkelUebergabe;

        if (pParent->Blickrichtung == LINKS)
            Winkel = float ((int (Winkel) + 180) % 360);

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 65);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 65);
        xSpeed	  = cos_deg(w) * 65.0f;
        ySpeed	  = sin_deg(w) * 65.0f;

        if (w == -90)
            xSpeed = 0.01f;

        DamagePlayer = false;
        Damage		 =  8;

        if (pParent->CurrentWeaponLevel[2] == 2)
            BounceWalls = true;

        OwnDraw = true;
    }
    break;


    case BOUNCESHOT2:		// BounceShot Mittel
    {
        Winkel = WinkelUebergabe;

        // je nach Waffenlevel öfters abspringen
        Damage = pParent->CurrentWeaponLevel[pParent->SelectedWeapon] * 2 - 3;

        if (Damage > 8)
            Damage = 8;

        if (pParent != NULL)
        {
            if (pParent->Blickrichtung == LINKS)
                Winkel = float ((int (Winkel) + 180) % 360);
        }

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 65);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 65);
        xSpeed	  = cos_deg(w) * 65.0f;
        ySpeed	  = sin_deg(w) * 65.0f;

        if (w == -90)
            xSpeed = 0.01f;

        DamagePlayer = false;

        if (pParent->CurrentWeaponLevel[pParent->SelectedWeapon] < 5)
            BounceWalls = true;
    }
    break;

    case BOUNCESHOT3 :		// BounceShot Klein
    {
        Winkel = WinkelUebergabe;

        if (pParent != NULL &&
                pParent->Blickrichtung == LINKS)
            Winkel = float ((int (Winkel) + 180) % 360);

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 65);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 65);
        xSpeed	  = cos_deg(w) * 65.0f;
        ySpeed	  = sin_deg(w) * 65.0f;

        if (w == -90)
            xSpeed = 0.01f;

        DamagePlayer = false;

        // je nach Waffenlevel öfters abspringen
        Damage = pParent->CurrentWeaponLevel[pParent->SelectedWeapon] - 3;

        if (Damage > 6)
            Damage = 6;

        BounceWalls  = true;
    }
    break;

    case BOUNCESHOTBIG1 :	// Riesen BounceShot in Gross
    {
        Winkel = WinkelUebergabe;

        if (pParent->Blickrichtung == LINKS)
            Winkel = float ((int (Winkel) + 180) % 360);

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 65);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 65);
        xSpeed	  = cos_deg(w) * 65.0f;
        ySpeed	  = sin_deg(w) * 65.0f;

        if (w == -90)
            xSpeed = 0.01f;

        DamagePlayer = false;
        Damage		 =  20;

        if (pParent->CurrentWeaponLevel[2] == 2)
            BounceWalls = true;
    }
    break;

    case BOUNCESHOTBIG2 : // Riesen BounceShot Mittel
    {
        Winkel = WinkelUebergabe;

        if (pParent != NULL &&
                pParent->Blickrichtung == LINKS)
            Winkel = float ((int (Winkel) + 180) % 360);

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 65);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 65);
        xSpeed	  = cos_deg(w) * 65.0f;
        ySpeed	  = sin_deg(w) * 65.0f;

        if (w == -90)
            xSpeed = 0.01f;

        DamagePlayer = false;
        Damage		 =  14;

        if (pParent->CurrentWeaponLevel[pParent->SelectedWeapon] < 5)
            BounceWalls = true;
    }
    break;

    case BOUNCESHOTBIG3 :// Riesen BounceShot Klein
    {
        Winkel = WinkelUebergabe;

        if (pParent != NULL &&
                pParent->Blickrichtung == LINKS)
            Winkel = float ((int (Winkel) + 180) % 360);

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 65);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 65);
        xSpeed	  = cos_deg(w) * 65.0f;
        ySpeed	  = sin_deg(w) * 65.0f;

        if (w == -90)
            xSpeed = 0.01f;

        DamagePlayer = false;
        Damage		 =  12;
        BounceWalls  = true;
    }
    break;

    case WALKER_LASER :		// Laser des Walkers
    {
        xSpeed	  =-30.0f;
        ySpeed	  = 0.0f;
        Damage    = 2;
        DamagePlayer = true;
        HasGlow = true;
        ExplodeOnImpact = false;
    }
    break;

    case WALKER_LASER2 :	// Laser des Walkers
    {
        xSpeed	  = 30.0f;
        ySpeed	  =  0.0f;
        Damage    = 2;
        DamagePlayer = true;
        HasGlow = true;
        ExplodeOnImpact = false;
        ShotArt	  = WALKER_LASER;
    }
    break;

    case SPIDER_LASER :		// Laser der Spinne
    {
        xSpeed	  =-60.0f;
        ySpeed	  =  0.0f;
        Damage       = 10;
        DamagePlayer = true;
        ShotArt   = WALKER_LASER;
    }
    break;


    case CANONBALL :		// Gechützturm Kugel links
    {
        xSpeed	  =-30.0f;
        ySpeed	  =  0.0f;
        Damage       = 8;
        DamagePlayer = true;
    }
    break;

    case CANONBALL2 :		// Gechützturm Kugel rechts
    {
        ShotArt	  = CANONBALL;
        xSpeed	  = 30.0f;
        ySpeed	  =  0.0f;
        Damage       = 8;
        DamagePlayer = true;
    }
    break;

    case BOMBE :			// Spieler Bombe
    {
        //DKS - off-by-one error:
        //AnimEnde	 = 16;
        AnimEnde	 = 15;
        AnimSpeed    = 0.5f;
        Damage       = 30;
        DamagePlayer = false;
    }
    break;

    case BOMBEBIG :		// Fette Spieler Bombe
    {
        //DKS - off-by-one error:
        //AnimEnde	 = 16;
        AnimEnde	 = 15;
        AnimSpeed    = 0.5f;
        Damage       = 90;
        DamagePlayer = false;
    }
    break;

    case SUCHSCHUSS :		// Kugel des Deckenturms, der direkt auf den Spieler schiesst
    {
        PlayerClass *pAim;

        if (pParent == NULL)
            pAim = ChooseAim();
        else
            pAim = pParent;

        //DKS - Converted to float:
        float	absx, absy, speed;				// Variablen für die Geschwindigkeits-
        // berechnung
        absx = pAim->xpos+35-xPos;			// Differenz der x
        absy = pAim->ypos+40-yPos;			// und y Strecke

        //DKS - Converted to float:
        speed = 1.0f/sqrtf(absx*absx + absy*absy);	// Länge der Strecke berechnen
        speed = speed*20;						// Geschwindigkeit ist 4 fach

        absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
        absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

        xSpeed = float(absx);
        ySpeed = float(absy);

        Damage       = 8;
        DamagePlayer = true;
    }
    break;

    case STRAIGHTSCHUSS :		// Schuss direkt nach oben (Spitter und Schienenviech)
    {
        ySpeed = -20.0f;
        Damage       = 8;
        DamagePlayer = true;
        ShotArt = SUCHSCHUSS;
    }
    break;

    case STRAIGHTSCHUSS2 :		// Schuss direkt nach unten (Schienenviech)
    {
        ySpeed = 20.0f;
        Damage       = 8;
        DamagePlayer = true;
        ShotArt = SUCHSCHUSS;
    }
    break;

    case SPITTERBOMBESHOTLO:
    {
        ySpeed = -8.0f;
        xSpeed = -20.0f;
        Damage = 8;
        DamagePlayer = true;
        ShotArt = SUCHSCHUSS;
    }
    break;

    case SPITTERBOMBESHOTLM:
    {
        xSpeed = -25.0f;
        Damage = 8;
        DamagePlayer = true;
        ShotArt = SUCHSCHUSS;
    }
    break;

    case SPITTERBOMBESHOTLU:
    {
        ySpeed =  8.0f;
        xSpeed = -20.0f;
        Damage = 8;
        DamagePlayer = true;
        ShotArt = SUCHSCHUSS;
    }
    break;

    case SPITTERBOMBESHOTRO:
    {
        ySpeed = -8.0f;
        xSpeed =  20.0f;
        Damage = 8;
        DamagePlayer = true;
        ShotArt = SUCHSCHUSS;
    }
    break;

    case SPITTERBOMBESHOTRM:
    {
        xSpeed = 25.0f;
        Damage = 8;
        DamagePlayer = true;
        ShotArt = SUCHSCHUSS;
    }
    break;

    case SPITTERBOMBESHOTRU:
    {
        ySpeed =  8.0f;
        xSpeed =  20.0f;
        Damage = 8;
        DamagePlayer = true;
        ShotArt = SUCHSCHUSS;
    }
    break;

    case SHIELDSPAWNER :		// Schutzschild Emitter
    {
        Damage = 10;
        DamagePlayer = false;
        ySpeed = 0.0f;
        xSpeed = 1.0f;
        //DKS - off-by-one error:
        //AnimEnde = 10;
        AnimEnde = 9;
        AnimSpeed = 1.0f;
        ExplodeOnImpact = false;
    }
    break;

    case SHIELDSPAWNER2 :		// Schutzschild Emitter
    {
        Damage = 10;
        DamagePlayer = false;
        ySpeed = D3DX_PI;
        xSpeed = 1.0f;
        ShotArt = SHIELDSPAWNER;
        //DKS - off-by-one error:
        //AnimEnde = 10;
        AnimEnde = 9;
        AnimSpeed = 1.0f;
        ExplodeOnImpact = false;
    }
    break;

    case SUCHSCHUSS2 :		// Blauer Schuss der Riesenwepse, genau auf den Spieler
    {
        PlayerClass *pAim = ChooseAim();

        //DKS - Converted to float:
        float	absx, absy, speed;				// Variablen für die Geschwindigkeits-
        // berechnung
        absx = pAim->xpos+35-xPos-20;		// Differenz der x
        absy = pAim->ypos+40-yPos-20;		// und y Strecke

        //DKS - Converted to float:
        speed = 1.0f/sqrtf(absx*absx + absy*absy);	// Länge der Strecke berechnen
        speed = speed*20;						// Geschwindigkeit setzen

        absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
        absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

        xSpeed = float(absx);
        ySpeed = float(absy);

        Damage       = 12;
        DamagePlayer = true;

        //DKS - off-by-one error:
        //AnimEnde	 = 4;
        AnimEnde	 = 3;
        AnimSpeed	 = 0.5f;
        HasGlow = true;
    }
    break;

    case TURRIEXTRAWURST:
    {
        Counter = WinkelUebergabe;

        ySpeed = -16.0f;
        yAcc   = 1.0f;

        Damage       = 50;
        DamagePlayer = true;

        //DKS - off-by-one error:
        //AnimEnde    = 25;
        AnimEnde    = 24;
        AnimSpeed   = 0.5f;
    }
    break;

    case GOLEMSCHUSS :							// Golem Suchschuss
    {
        Winkel = WinkelUebergabe;

        int w = int (Winkel) - 90;
        w = w % 360;

        xSpeed  = -WinkelUebergabe;
        ySpeed  = -40;
        yAcc	= 10.0f;
        Counter = -ySpeed;

        if ((int)xSpeed % 2 == 0)
        {
            ySpeed  *= -1.0f;
            yAcc	*= -1.0f;
        }

        Damage       = 50;
        DamagePlayer = true;

        //DKS - off-by-one error:
        //AnimEnde    = 25;
        AnimEnde    = 24;
        AnimSpeed   = 0.5f;
    }
    break;

    case SPIDERSLOW :							// langsamer Spinnenschuss
    {
        PlayerClass *pAim = ChooseAim();

        //DKS - Converted to float:
        float	absx, absy, speed;				// Variablen für die Geschwindigkeits-
        // berechnung
        absx = pAim->xpos+35-xPos-20;		// Differenz der x
        absy = pAim->ypos+40-yPos-20;		// und y Strecke

        //DKS - Converted to float:
        speed = 1.0f/sqrtf(absx*absx + absy*absy);	// Länge der Strecke berechnen
        speed = speed*20;						// Geschwindigkeit setzen

        absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
        absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

        xSpeed = float(absx);
        ySpeed = float(absy);

        Damage       = 30;
        DamagePlayer = true;

        //DKS - off-by-one error:
        //AnimEnde    = 25;
        AnimEnde    = 24;
        AnimSpeed   = 0.5f;
    }
    break;

    case SPIDERLASER :							// Feter Spinnenlaser
    {
        Damage       = 10;
        DamagePlayer = true;

        AnimCount = 0.0f;
        AnimSpeed = 0.0f;
        AnimPhase = 1;
        ExplodeOnImpact = false;
        OwnDraw		 = true;

        ShotRect[SPIDERLASER].left = 0;
        ShotRect[SPIDERLASER].right  = 0;
        ShotRect[SPIDERLASER].top  = 0;
        ShotRect[SPIDERLASER].bottom = 0;
    }
    break;

    case SPIDERSHOT :							// Schuss der Spinne
    {
        Winkel = WinkelUebergabe;

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = -float (sin (w * PI / 180.0f) * 50);
        //ySpeed	  = float (cos (w * PI / 180.0f) * 50);
		xSpeed	  = -sin_deg(w) * 50.0f;
		ySpeed	  =  cos_deg(w) * 50.0f;

        Damage    =   10;
        DamagePlayer = true;
        ExplodeOnImpact = false;
    }
    break;

    case SPIDERSHOTLANGSAM :							// Schuss der Spinne
    {
        Winkel = WinkelUebergabe;

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = -float (sin (w * PI / 180.0f) * 25);
        //ySpeed	  = float (cos (w * PI / 180.0f) * 25);
		xSpeed	  = -sin_deg(w) * 25.0f;
		ySpeed	  =  cos_deg(w) * 25.0f;

        Damage    =   10;
        DamagePlayer = true;

        ShotArt = SPIDERSHOT;
    }
    break;

    case SPIDERSHOT2 :							// Schuss der Spinne
    {
        xSpeed	  = -20.0f;
        ySpeed	  = -(float)(rand()%12 + 5);
        yAcc	  =  (float)(rand()%10 + 5) / 5.0f;

        if (rand()%2 == 0)
        {
            ySpeed *= -1.0f;
            yAcc   *= -1.0f;
        }

        Damage    =   10;
        DamagePlayer = true;
        BounceWalls = true;
        AnimCount = 1.0f;
    }
    break;

    case SPIDERSHOT3 :							// Schuss der Spinne
    {
        ySpeed	  = -40.0f;
        Damage    =   10;
        DamagePlayer = true;
        AnimCount = 1.0f;
        ShotArt = SPIDERSHOT2;
    }
    break;


    case PHARAOLASER :		// Laser des Pharao Kopfes, der direkt auf den Spieler schiesst
    {
        PlayerClass *pAim = ChooseAim();

        //DKS - Converted to float:
        float	absx, absy, speed;				// Variablen für die Geschwindigkeits-
        // berechnung
        absx = pAim->xpos+35-(xPos+24);		// Differenz der x
        absy = pAim->ypos+40-(yPos+24);		// und y Strecke

        //DKS - Converted to float:
        speed = 1.0f/sqrtf(absx*absx + absy*absy);	// Länge der Strecke berechnen
        speed = speed*40;

        absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
        absy = speed*absy;

        xSpeed = float(absx);
        ySpeed = float(absy);

        Damage       = 40;
        DamagePlayer = true;

        //DKS - off-by-one error:
        //AnimEnde  = 4;
        AnimEnde  = 3;
        AnimSpeed = 0.5f;
    }
    break;

    //DKS - NOTE: this projectile was never actually used in the game (commented out in Partikelsystem.cpp)
    case FLAMEWALL :		// Aufsteigende Feuerwand die aus der Spinne Granate entsteht
    {
        xSpeed =  40.0f;
        ySpeed = -20.0f;
        yAcc   =   2.0f;

        Damage       = 40;
        DamagePlayer = true;

        AnimEnde  = 6;
        AnimSpeed = 1.0f;

        SoundManager.PlayWave(100, 128, 11025, SOUND_EXPLOSION1);
        CheckBlock   = false;
        ExplodeOnImpact = false;
    }
    break;


    case POWERLINE :		// Powerline links
    {
        xSpeed	  =- 80.0f;
        ySpeed	  =   0.0f;
        Damage    =   20;
        DamagePlayer = false;
        HasGlow	= true;
        ExplodeOnImpact = false;
    }
    break;

    case POWERLINE2 :		// Powerline rechts
    {
        ShotArt	  = POWERLINE;
        xSpeed	  =  80.0f;
        ySpeed	  =   0.0f;
        AnimPhase =   1;
        Damage    =   12;
        DamagePlayer = false;
        HasGlow	= true;
        ExplodeOnImpact = false;
    }
    break;

    case FLAMME :			// Splitter-Flamme aus dem Fass
    {
        ShotArt	  = FLAMME;
        xSpeed	  = ((float)(rand()%120-60)/3);
        ySpeed	  = -(float)(rand()% 60+30)/2;
        yAcc	  = 4.0f;
        Damage    =   50;
        //DKS - off-by-one error:
        //AnimEnde  = 8;
        AnimEnde  = 7;
        AnimSpeed = (float)(rand()%20+20)/100;
        DamagePlayer = false;
    }
    break;

    case FEUERFALLE_LAVAMANN :		// Flamme des Lavamannes
    {
        //DKS - off-by-one error:
        //AnimEnde	 = 30;
        AnimEnde	 = 29;
        AnimPhase    = 6 + rand()%3;
        AnimSpeed    = float((rand()%3+10)/20.0f);
        Damage       = 1;
        ySpeed		 = 1.0f;
        DamagePlayer = true;
        xSpeed		 = ((float)(rand()%20-10)/8);
        CheckBlock   = false;
        ExplodeOnImpact = false;
        OwnDraw		 = true;

    }
    break;

    case FEUERFALLE :		// Flamme des Wandflammenwerfers oben
    {
        //DKS - off-by-one error:
        //AnimEnde	 = 30;
        AnimEnde	 = 29;
        AnimSpeed    = float((rand()%3+10)/20.0f);
        Damage       = 1;
        DamagePlayer = true;
        ySpeed		 = -20.0f;
        xSpeed		= ((float)(rand()%20-10)/8);
        yPos		-= 40;
        xPos		+= 5;
        ExplodeOnImpact = false;
        Winkel = (float)(rand()%360);
        OwnDraw = true;

    }
    break;

    case FIREBALL :		// Feuerball des Minidrachens
    {
        Damage       = 15;
        DamagePlayer = true;
        AnimCount = 0.1f;

        Winkel = WinkelUebergabe;
        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 50);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 50);
        xSpeed	  = cos_deg(w) * 50.0f;
        ySpeed	  = sin_deg(w) * 50.0f;
    }
    break;

    case FIREBALL_BIG :		// Feuerball des Minidrachens
    {
        AnimCount = 0.1f;
        Damage       = 30;
        DamagePlayer = true;

        Winkel = WinkelUebergabe;
        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 50);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 50);
        xSpeed	  = cos_deg(w) * 50.0f;
        ySpeed	  = sin_deg(w) * 50.0f;
    }
    break;

    case FIREBALL_BOMB :		// Feuerbombe des Metallkopfs
    {
        Damage       = 30;
        DamagePlayer = true;

        float absx = pParent->xpos+35-(xPos + 50);
        float absy = pParent->ypos+40-(yPos + 180);

        xSpeed		 = absx / 10.0f;
        Damage		 = 10;
        ySpeed		 = absy / 10.0f;
        yAcc		 = 3.0f;

        Winkel    = (float)(rand()%360);
    }
    break;

    case ROTZSHOT :		// Shuss des Rotzpotts
    {
        Damage       = 12;
        DamagePlayer = true;

        Winkel = WinkelUebergabe;
        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 40);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 30);
		xSpeed	  = cos_deg(w) * 40.0f;
		ySpeed	  = sin_deg(w) * 30.0f;

        yAcc = 5.0f;
        //DKS - off-by-one error:
        //AnimEnde = 25;
        AnimEnde = 24;
        AnimSpeed = 0.2f;

        Counter = 0.2f;
    }
    break;


    case GOLEMSAEULE :		// Flammensäule des Golems
    {
        Damage       = 1;
        DamagePlayer = true;
        xSpeed		 = -(float)(rand()%10 + 5);
        CheckBlock   = false;
        AnimCount    = 0.0f;

    }
    break;

    case FEUERFALLE2 :		// Flamme des Wandflammenwerfers rechts
    {
        ShotArt		 = FEUERFALLE;
        //DKS - off-by-one error:
        //AnimEnde	 = 30;
        AnimEnde	 = 29;
        AnimSpeed    = float((rand()%3+10)/20.0f);
        Damage       = 1;
        DamagePlayer = true;
        ySpeed		 = ((float)(rand()%20-10)/8) - 1.0f;
        xSpeed		 = 25.0f;
        yPos		-= 7 + ((float)(rand()%20)/10.0f);
        xPos		+= 33;
        ExplodeOnImpact = false;
        Winkel = (float)(rand()%360);
        OwnDraw = true;

    }
    break;

    case FEUERFALLE3 :		// Flamme des Wandflammenwerfers unten
    {
        ShotArt		 = FEUERFALLE;
        //DKS - off-by-one error:
        //AnimEnde	 = 30;
        AnimEnde	 = 29;
        AnimSpeed    = float((rand()%3+10)/20.0f);
        Damage       = 1;
        DamagePlayer = true;
        ySpeed		 = 20.0f;
        xSpeed		= ((float)(rand()%20-10)/8);
        yPos		+= 30;
        ExplodeOnImpact = false;
        Winkel = (float)(rand()%360);
        OwnDraw = true;

    }
    break;

    case FEUERFALLE4 :		// Flamme des Wandflammenwerfers links
    {
        ShotArt		 = FEUERFALLE;
        //DKS - off-by-one error:
        //AnimEnde	 = 30;
        AnimEnde	 = 29;
        AnimSpeed    = float((rand()%3+10)/20.0f);
        Damage       = 1;
        DamagePlayer = true;
        ySpeed		 = ((float)(rand()%20-10)/8) - 1.0f;
        xSpeed		 = -25.0f;
        yPos		-= 15 + ((float)(rand()%20)/10.0f);
        xPos		-= 28;
        ExplodeOnImpact = false;
        Winkel = (float)(rand()%360);
        OwnDraw = true;

    }
    break;

    case SPIDERFIRE :		// Flamme der Feuer Spinne (wie Suchschuss)
    {
        //DKS - off-by-one error:
        //AnimEnde	 = 30;
        AnimEnde	 = 29;
        AnimSpeed    = float((rand()%3+10)/30.0f);
        Damage       = 1;
        DamagePlayer = true;

        //DKS - Converted to float:
        float	absx, absy, speed;				// Variablen für die Geschwindigkeits-
        // berechnung
        absx = pParent->xpos+35-xPos;			// Differenz der x
        absy = pParent->ypos+40-yPos;			// und y Strecke

        //DKS - Converted to float:
        speed = 1.0f/sqrtf(absx*absx + absy*absy);	// Länge der Strecke berechnen
        speed = speed*20;						// Geschwindigkeit

        absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
        absy = speed*absy;
        xSpeed = float(absx);
        ySpeed = float(absy);

        //DKS - BUG here, two settings of CheckBlock, disabling first of two:
        //CheckBlock	 = true;
        Damage       = 4;
        CheckBlock   = false;
        ExplodeOnImpact = false;
        Winkel = (float)(rand()%360);
        OwnDraw		 = true;
    }
    break;

    case WALKERFIRE :		// Flamme des Feuer Walkers
    {
        //DKS - off-by-one error:
        //AnimEnde	 = 30;
        AnimEnde	 = 29;
        AnimSpeed    = float((rand()%3+10)/30.0f);
        Damage       = 1;
        DamagePlayer = true;

        //DKS - Converted to float:
        float	absx, absy, speed;				// Variablen für die Geschwindigkeits-
        // berechnung
        absx = pParent->xpos+35-xPos;			// Differenz der x
        absy = pParent->ypos+5-yPos;			// und y Strecke

        //DKS - Converted to float:
        speed = 1.0f/sqrtf(absx*absx + absy*absy);	// Länge der Strecke berechnen
        speed = speed*22;						// Geschwindigkeit ist 4 fach

        absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
        absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

        xSpeed = float(absx);
        ySpeed = float(absy);

        if (ySpeed < -5.0f)	ySpeed = -5.0f;
        if (ySpeed >  5.0f)	ySpeed =  5.0f;

        if (xSpeed > 0.0f)
            if (xSpeed < 5.0f)
                xSpeed = 5.0f;

        if (xSpeed < 0.0f)
            if (xSpeed > -5.0f)
                xSpeed = -5.0f;

        Damage       = 4;
        CheckBlock   = false;

        Winkel = (float)(rand()%360);
        OwnDraw		 = true;
        ExplodeOnImpact = false;

        ShotArt = SPIDERFIRE;
    }
    break;

    case ELEKTROSCHUSS:
    {
        Counter   = 1000;
        DamagePlayer = true;
        Damage       = 1000;
        ExplodeOnImpact = false;
        ySpeed = 25.0f;
        xSpeed = -3.0f;
        HasGlow = true;
    }
    break;

    case ELEKTROPAMPE:
    {
        Counter = 1500;
        //DKS - off-by-one error:
        //AnimEnde	 = 5;
        AnimEnde	 = 4;
        AnimSpeed    = 0.5f;
        DamagePlayer = true;

        Damage       = 20;
        ExplodeOnImpact = false;
    }
    break;

    case PLAYERFIRE :		// Flamme des Spielers
    {
        Counter = 255;
        //DKS - off-by-one error:
        //AnimEnde	 = 30;
        AnimEnde	 = 29;
        AnimSpeed    = float((rand()%3+10)/40.0f);
        DamagePlayer = false;

        Damage       = 20;
        CheckBlock   = true;

        Winkel = WinkelUebergabe - 5 + rand()%10;
        if (pParent->Blickrichtung == LINKS)
            Winkel = float ((int (Winkel) + 180) % 360);

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 35);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 35);
        xSpeed	  = cos_deg(w) * 35.0f;
        ySpeed	  = sin_deg(w) * 35.0f;

        Winkel = (float)(rand()%360);

        if (w == -90)
            xSpeed = 0.0f;

        ExplodeOnImpact = false;
        OwnDraw		 = true;
    }
    break;

    case KRABBLERLASER1 :		// Laser des Deckenkrabblers
    {
        Damage          = 8;
        DamagePlayer    = true;
        ySpeed		    = 30.0f;
        ExplodeOnImpact = false;
    }
    break;

    case KRABBLERLASER2 :		// Laser des linken WandKrabblers
    {
        Damage       = 8;
        DamagePlayer = true;
        xSpeed		 = 30.0f;
        ExplodeOnImpact = false;
    }
    break;

    case KRABBLERLASER3 :		// Laser des rechten WandKrabblers
    {
        Damage       = 8;
        DamagePlayer = true;
        xSpeed		 = -30.0f;
        ExplodeOnImpact = false;
    }
    break;

    case GRENADE :				// Granate
    {
        Winkel = WinkelUebergabe;

        if (pParent->Blickrichtung == LINKS)
            Winkel = float ((int (Winkel) + 180) % 360);

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 60);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 60) - 6.0f;
        xSpeed	  = cos_deg(w) * 60.0f;
        ySpeed	  = sin_deg(w) * 60.0f - 6.0f;

        yPos   -= 2;
        yAcc   = 5.0f;
        BounceWalls = false;
        Damage    = 300;
        DamagePlayer = false;
    }
    break;

    case ROCKET :				// Normale Rakete
    {
        xSpeed = -50.0f;
        Damage    = 100;
        DamagePlayer = true;
    }
    break;

    case ROCKETSPIDER :			// Rakete der Riesenspinne
    {
        AnimPhase = 0;
        AnimCount = 5.0f;
        ySpeed = -60.0f;
        Damage = 40;
        DamagePlayer = true;
    }
    break;

    case ROCKETWERFER :				// Rakete des Raketenwerfers
    {
        AnimPhase = 0;

        if (pParent->xpos > x)
            AnimPhase = 1;

        if (pParent->ypos + 60 < y)
            ySpeed = -20.0f;
        else
            ySpeed = 20.0f;

        xSpeed = 0.0f;
        Damage = 40;
        DamagePlayer = true;

    }
    break;

    case ROBOROCKET :			// Rakete des RoboMans
    {
        AnimPhase = 0;
        xSpeed    = -30.0f;

        if (pParent->xpos > x)
        {
            AnimPhase = 1;
            xSpeed    = 30.0f;
        }

        Damage = 40;
        DamagePlayer = true;

        ShotArt = ROCKETWERFER;

    }
    break;

    case ARCSHOT :				// Schuss der im Bogen fliegt
    {
        ySpeed = -30.0f;
        yAcc   =   4.0f;
        Damage =   10;
        DamagePlayer = true;
    }
    break;

    case EVILROUND1:			// Rundum Schuss des Evil Hurri
    {
        xSpeed = -50.0f;
        yAcc   =   5.0f;
        Damage =   20;
        DamagePlayer = true;
    }
    break;

    case EVILROUND2:			// Rundum Schuss des Evil Hurri
    {
        ShotArt = EVILROUND1;
        xSpeed = -45.0f;
        ySpeed =  -8.0f;
        yAcc   =   5.0f;
        Damage =   20;
        DamagePlayer = true;
    }
    break;

    case EVILROUND3:			// Rundum Schuss des Evil Hurri
    {
        ShotArt = EVILROUND1;
        xSpeed = -40.0f;
        ySpeed =  -16.0f;
        yAcc   =   5.0f;
        Damage =   20;
        DamagePlayer = true;
    }
    break;

    case EVILROUND4:			// Rundum Schuss des Evil Hurri
    {
        ShotArt = EVILROUND1;
        xSpeed = -30.0f;
        ySpeed =  -24.0f;
        yAcc   =   5.0f;
        Damage =   20;
        DamagePlayer = true;
    }
    break;

    case EVILROUND5:			// Rundum Schuss des Evil Hurri
    {
        ShotArt = EVILROUND1;
        xSpeed = -20.0f;
        ySpeed =  -32.0f;
        yAcc   =   5.0f;
        Damage =   20;
        DamagePlayer = true;
    }
    break;

    case EVILROUND6:			// Rundum Schuss des Evil Hurri
    {
        ShotArt = EVILROUND1;
        xSpeed =  -10.0f;
        ySpeed =  -40.0f;
        yAcc   =    5.0f;
        Damage =    20;
        DamagePlayer = true;
    }
    break;

    case EVILROUND7:			// Rundum Schuss des Evil Hurri
    {
        ShotArt = EVILROUND1;
        xSpeed =  10.0f;
        ySpeed =  -40.0f;
        yAcc   =    5.0f;
        Damage =    20;
        DamagePlayer = true;
    }
    break;

    case EVILROUND8:			// Rundum Schuss des Evil Hurri
    {
        ShotArt = EVILROUND1;
        xSpeed = 20.0f;
        ySpeed =  -32.0f;
        yAcc   =    5.0f;
        Damage =    20;
        DamagePlayer = true;
    }
    break;

    case EVILROUND9:			// Rundum Schuss des Evil Hurri
    {
        ShotArt = EVILROUND1;
        xSpeed = 30.0f;
        ySpeed =  -24.0f;
        yAcc   =    5.0f;
        Damage =    20;
        DamagePlayer = true;
    }
    break;

    case EVILROUNDA:			// Rundum Schuss des Evil Hurri
    {
        ShotArt = EVILROUND1;
        xSpeed = 40.0f;
        ySpeed =  -16.0f;
        yAcc   =    5.0f;
        Damage =    20;
        DamagePlayer = true;
    }
    break;

    case EVILROUNDB:			// Rundum Schuss des Evil Hurri
    {
        ShotArt = EVILROUND1;
        xSpeed = 45.0f;
        yAcc   =  8.0f;
        Damage =    20;
        DamagePlayer = true;
    }
    break;

    case EVILROUNDC:			// Rundum Schuss des Evil Hurri
    {
        ShotArt = EVILROUND1;
        xSpeed = 50.0f;
        yAcc   =    5.0f;
        Damage =    20;
        DamagePlayer = true;
    }
    break;

    case SNOWBOMB:				// SchneeBombe
    {
        xSpeed = - 8.0f;
        ySpeed =   26.0f;
        Damage =   40;
        DamagePlayer = true;
        //DKS - off-by-one error:
        //AnimEnde  = 20;
        AnimEnde  = 19;
        AnimSpeed = 0.7f;
    }
    break;

    case FETTESPINNESHOT:		// Schuss der fetten Spinne rechts
    {
        xSpeed =  30.0f;
        Damage =   20;
        DamagePlayer = true;
        //DKS - off-by-one error:
        //AnimEnde  = 4;
        AnimEnde  = 3;
        AnimSpeed = 0.5f;
        HasGlow = true;
    }
    break;

    case FETTESPINNESHOT2:		// Schuss der fetten Spinne links
    {
        xSpeed =  -30.0f;
        Damage =   20;
        DamagePlayer = true;
        //DKS - off-by-one error:
        //AnimEnde  = 4;
        AnimEnde  = 3;
        AnimSpeed = 0.5f;
        HasGlow = true;
    }
    break;

    case EIERBOMBE:				// Eierbome, die der Eiermann legt, und die auf den Hurri zurollt
    {
        if (pParent->xpos + 40 < xPos + 20)
            xSpeed =  -18.0f;
        else
            xSpeed =   18.0f;

        Damage =   40;
        DamagePlayer = true;
        AnimSpeed = 0.5f;

        // Linksrum oder rechtsrum rollen lassen
        if (xSpeed < 0.0f)
        {
            //DKS - off-by-one error:
            //AnimPhase = 19;
            //AnimEnde  = 20;
            AnimPhase = 18;
            AnimEnde  = 19;
        }
        else
        {
            AnimPhase = 0;
            AnimEnde  = 19;
        }

    }
    break;

    case SNOWBOMBSMALL:			// SchneeBombe
    {
        xSpeed =  (rand()%80-40) / 5.0f;
        ySpeed = -(rand()%40+80) / 5.0f;
        yAcc   =   5.0f;
        Damage =   15;
        DamagePlayer = true;
        //DKS - off-by-one error:
        //AnimEnde  = 20;
        AnimEnde  = 19;
        AnimSpeed = 0.7f;
    }
    break;

    case TORPEDO :				// Torpedo Schuss
    {
        if (pParent->xpos > x)
        {
            AnimPhase = 1;
            xSpeed    = 20.0f;
        }
        else
        {
            AnimPhase = 0;
            xSpeed    = -20.0f;
        }

        AnimCount = 2.0f;

        Damage = 20;
        DamagePlayer = true;
    }
    break;

    case EVILSHOT:				// Schuss des Evil Hurri rechts
    {
        xSpeed = 65.0f;
        Damage = 10;
        DamagePlayer = true;
    }
    break;

    case EVILSHOT2:				// Schuss des Evil Hurri links
    {
        xSpeed = -65.0f;
        Damage =  10;
        DamagePlayer = true;
    }
    break;

    case EVILBLITZ:				// Blitz des bösen Hurris hoch
    {
        //DKS - off-by-one error:
        //AnimEnde  = 2;
        AnimEnde  = 1;
        AnimSpeed = 0.7f;
        Damage =   50;
        ySpeed = -20.0f;
        yAcc   = -1.0f;
        DamagePlayer = true;
        ExplodeOnImpact = false;
    }
    break;

    case EVILBLITZ2:			// Blitz des bösen Hurris runter
    {
        Damage = 100;
        AnimSpeed = 0.5f;
        //DKS - off-by-one error:
        //AnimEnde  = 2;
        AnimEnde  = 1;
        ySpeed = 30;
        DamagePlayer = true;
        ExplodeOnImpact = false;
    }
    break;

    case UFOLASER:				// Laser des Ufos
    {
        HasGlow      = true;
        //DKS - off-by-one error:
        //AnimEnde     = 10;
        AnimEnde     = 9;
        AnimSpeed	 = 0.25f;
        Damage       =  20;
        ySpeed       = 20.0f;
        yAcc	     = 10.0f;
        DamagePlayer = true;
        ExplodeOnImpact = false;
    }
    break;

    case STELZLASER:			// Laser des Stelzsacks links
    {
        AnimPhase = 1;
        xSpeed    = -30.0f;
        Damage       =  8;
        ySpeed       = 15.0f + g_Fahrstuhl_Speed;
        DamagePlayer = true;
        ExplodeOnImpact = false;
    }
    break;

    case STELZLASER2:			// Laser des Stelzsacks rechts
    {
        ShotArt		 = STELZLASER;

        AnimPhase    = 0;
        xSpeed       = 30.0f;
        Damage       =  8;
        ySpeed       = 15.0f + g_Fahrstuhl_Speed;
        DamagePlayer = true;
    }
    break;

    case PFLANZESHOT:			// Schuss der Pflanze
    {
        float absx = pParent->xpos+35-(xPos + 20);
        float absy = pParent->ypos+40-(yPos + 20) - 160;

        xSpeed		 = absx / 10.0f;
        Damage		 = 10;
        ySpeed		 = absy / 10.0f;
        yAcc		 = 3.25f;
        DamagePlayer = true;
        //DKS - off-by-one error:
        //AnimEnde	 = 4;
        AnimEnde	 = 3;
        AnimSpeed	 = 0.25f;
        HasGlow = true;
    }
    break;

    case BRATKLOPSSHOT:			// Suchschuss des Bratklopses
    {
        //DKS - Converted to float:
        float	absx, absy, speed;				// Variablen für die Geschwindigkeits-
        // berechnung
        absx = pParent->xpos+35-(xPos+30);			// Differenz der x
        absy = pParent->ypos+40-(yPos+30);			// und y Strecke

        //DKS - Converted to float:
        speed = 1.0f/sqrtf(absx*absx + absy*absy);	// Länge der Strecke berechnen
        speed = speed*20;						// Geschwindigkeit ist 4 fach

        absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
        absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

        xSpeed = float(absx);
        ySpeed = float(absy);

        Damage    =  40;
        DamagePlayer = true;
        OwnDraw		 = true;
    }
    break;

    case DRONEBULLET:							// Schuss der Drone
    {
        xSpeed =  40.0f;
        ySpeed =  9.0f;
        Damage =   6;
        DamagePlayer = true;
        AnimEnde  = 3;
        AnimSpeed = 0.2f;
    }
    break;

    case DRONEBULLET2:							// Schuss der Drone
    {
        xSpeed = -40.0f;
        ySpeed =  9.0f;
        Damage =   6;
        DamagePlayer = true;
        AnimEnde  = 3;
        AnimSpeed = 0.2f;
        ShotArt = DRONEBULLET;
    }
    break;

    case SCHLEIMSHOT:							// Schuss der Drone
    {
        //DKS - Converted to float:
        float	absx, absy, speed;				// Variablen für die Geschwindigkeits-
        // berechnung
        absx = pParent->xpos+35-xPos;			// Differenz der x
        absy = pParent->ypos+40-yPos;			// und y Strecke

        //DKS - Converted to float:
        speed = 1.0f/sqrtf(absx*absx + absy*absy);	// Länge der Strecke berechnen
        speed = speed*20;						// Geschwindigkeit ist 4 fach

        absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
        absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

        xSpeed = float(absx);
        ySpeed = float(absy);

        Damage       = 8;
        DamagePlayer = true;
    }
    break;

    case LAFASSSHOT:								// Das runterfallende Fass
    {
        xSpeed = WinkelUebergabe;;
        ySpeed = 0.0f;
        yAcc   = 5.0f;

        //DKS - This was way off, there's only 15 frames of animation in the spritesheet image.
        //AnimEnde = 20;
        AnimEnde = 14;
        AnimSpeed = 1.0f;

        Damage = 8000;
        DamagePlayer = false;
    }
    break;

    case FLUGLASER :			// Laser der FlugKanone
    {
        Damage = 15;
        DamagePlayer = true;
        Winkel = WinkelUebergabe;

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 40);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 40);
        xSpeed	  = cos_deg(w) * 40.0f;
        ySpeed	  = sin_deg(w) * 40.0f;
    }
    break;

    case EISZAPFENSHOT :			// Eiszapfengeschoss
    {
        Damage = 12;
        DamagePlayer = true;
        Winkel = WinkelUebergabe;

        int w = int (Winkel) - 90;
        w = w % 360;

        //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        //xSpeed	  = float (cos (w * PI / 180.0f) * 40);
        //ySpeed	  = float (sin (w * PI / 180.0f) * 40);
        xSpeed	  = cos_deg(w) * 40.0f;
        ySpeed	  = sin_deg(w) * 40.0f;
    }
    break;


    case ARCSHOTLEFT :			// Schuss der im Bogen fliegt links
    {
        ShotArt = ARCSHOT;
        ySpeed  = -30.0f;
        xSpeed  =  -8.0f * WinkelUebergabe;
        yAcc    =   4.0f;
        Damage  =   10;
        DamagePlayer = true;
    }
    break;

    case ARCSHOTRIGHT :			// Schuss der im Bogen fliegt rechts
    {
        ShotArt = ARCSHOT;
        ySpeed  = -30.0f;
        xSpeed  =   8.0f * WinkelUebergabe;
        yAcc    =   4.0f;
        Damage  =   10;
        DamagePlayer = true;
    }
    break;

    case SMARTBOMB :			// Smartbomb
    {
        AnimCount = 32.0f;
        DamagePlayer = false;
        CheckBlock   = false;
        ExplodeOnImpact = false;
        Damage = int (255-AnimCount)*2;
        OwnDraw		 = true;
    }
    break;

    case BLUEBOMB:
    {
        AnimSpeed = 0.5f;
        DamagePlayer = true;
        Damage = 20;
        //DKS - off-by-one error:
        //AnimEnde = 20;
        AnimEnde = 19;
        ySpeed = 10.0f;
        yAcc = 10.0f;
    }
    break;

    case SKELETORGRANATE:
    {
        //DKS - off-by-one error:
        //AnimEnde	 = 10;
        AnimEnde	 = 9;
        AnimSpeed    = 0.5f;
        Damage		 = 40;
        Counter		 = 1.0f;
        DamagePlayer = false;

        if (WinkelUebergabe > 180.0f)
            WinkelUebergabe = -(360.0f - WinkelUebergabe);

        xSpeed	  = WinkelUebergabe * 1.8f;
        ySpeed	  = -18.0f;

        yAcc = 6.2f;
    }
    break;

    default :
        break;
    }
}

// --------------------------------------------------------------------------------------
// Schüsse auf Kollision mit den Gegnern oder Spielern testen
// --------------------------------------------------------------------------------------

void ProjectileClass::CheckCollision(void)
{
    GegnerClass *pEnemy = Gegner.pStart;			// Anfang der Gegnerliste

    while (pEnemy != NULL)				// Noch nicht alle durch ?
    {
        if (pEnemy->Active		== true &&	// Ist der Gegner überhaupt aktiv ?
                pEnemy->Destroyable	== true &&	// und kann man ihn zerstören
                pEnemy->Energy      > 0.0f  &&
                ShotArt		!= BOMBE && // Und der Schuss keine Bombe ?
                !(pEnemy->GegnerArt == POWERBLOCK && ShotArt == SHIELDSPAWNER)) // und kein powerblock + shield
        {
            // Beim BlitzBeam die größe vom Rect dynamisch anpassen
            if (ShotArt == BLITZBEAM)
            {
                //DKS - Fixed ending commas on next two lines, changed to semicolons. No idea why they were commas.
                ShotRect[BLITZBEAM].left   = int(Damage / 4.0f);
                ShotRect[BLITZBEAM].top    = int(Damage / 4.0f);
                ShotRect[BLITZBEAM].right  = ShotRect[BLITZBEAM].left + int(Damage / 2.0f);
                ShotRect[BLITZBEAM].bottom = ShotRect[BLITZBEAM].top  + int(Damage / 2.0f);
            }

            // Überschneiden sich die Rechtecke ?
            if (SpriteCollision( xPos,  yPos,
                                 ShotRect  [ShotArt],
                                 pEnemy->xPos, pEnemy->yPos,
                                 GegnerRect[pEnemy->GegnerArt]) == true &&

                    (DamagePlayer == false	    ||

                     (ShotArt	  == FEUERFALLE &&
                      pEnemy->GegnerArt != LAVAMANN)    ||

                     pEnemy->GegnerArt   == SCHLEUSEH  ||
                     pEnemy->GegnerArt   == SCHLEUSEV))
            {
                // blinken lassen, wenn noch nicht blinkt
                if (pEnemy->DamageTaken <= 0.0f)
                    pEnemy->DamageTaken = 255;

                float BossZiehtWenigerAb = 1;

                if ((ShotArt == GRENADE ||
                        ShotArt == POWERLINE ||
                        ShotArt == SMARTBOMB) &&
                        pEnemy->GegnerArt >= STAHLFAUST &&
                        pEnemy->GegnerArt <  PARTIKELSPAWN)
                    BossZiehtWenigerAb = 0.7f;

                // Im 2 Spieler Modus sind die Gegner fieser
                if (NUMPLAYERS == 2)
                    BossZiehtWenigerAb *= 0.75;

                // Schuss explodiert beim Auftreffen?
                if (ExplodeOnImpact == true)
                {

                    // Dann Energy abziehen
                    pEnemy->Energy -= Damage * BossZiehtWenigerAb;

                    // Powerline? Dann alles mit Energy < 40 eliminieren :)
                    if(ShotArt == POWERLINE &&
                            pEnemy->Energy < 40)
                        pEnemy->Energy = 0;

                    //DKS - Added function WaveIsPlaying() to SoundManagerClass:
                    if (!SoundManager.WaveIsPlaying(SOUND_HIT + pEnemy->HitSound))
                        SoundManager.PlayWave(50, 128, 21000 + rand()%1000, SOUND_HIT + pEnemy->HitSound);

                    Damage = 0;				// und verschwinden lassen
                    ExplodeShot();			// aber explodieren tut er auch =)
                }
                else
                {
                    // War es ein Laser oder ne Powerline oder ähnliches ?
                    pEnemy->Energy -= BossZiehtWenigerAb * Damage SYNC;	// Dann fliegt er

                    // Hit-Sound
                    //DKS - Added function WaveIsPlaying() to SoundManagerClass:
                    if (!SoundManager.WaveIsPlaying(SOUND_HIT + pEnemy->HitSound))
                        SoundManager.PlayWave(50, 128, 21000, SOUND_HIT + pEnemy->HitSound);
                }											// nach dem Energy abziehen weiter

                if (pEnemy->GegnerArt == SCHNEEKOPPE)
                {
                    if (ShotArt != SMARTBOMB)
                        Damage = 0;

                    if (pEnemy->LastAction == -1 &&
                            (ShotArt == SMARTBOMB ||
                             fabs(xSpeed) >= fabs(ySpeed)))
                    {
                        pEnemy->xSpeed *= -0.75f;
                        pEnemy->LastAction = 0;

                        if (ySpeed > 0.0f) pEnemy->ySpeed += 8.0f;
                        if (ySpeed < 0.0f) pEnemy->ySpeed -= 8.0f;
                    }
                }

                if (ShotArt != SMARTBOMB &&
                        (pEnemy->GegnerArt == SKULL ||
                         pEnemy->GegnerArt == PUNISHER))
                {
                    float mul = 1.0f;

                    if (ShotArt != DIAMONDSHOT)
                        mul = 0.05f;
                    else
                        mul = 0.2f;

                    pEnemy->xSpeed += xSpeed * mul;
                    pEnemy->ySpeed += ySpeed * mul;

                    if (ShotArt != PLAYERFIRE)
                    {
                        Damage = 0;
                        ExplodeShot();
                    }
                }

                // evtl pAim wechseln
                if (pEnemy->TimeToChange <= 0.0f)
                {
                    pEnemy->TimeToChange = 30.0f;

                    if (rand()%2 == 0 &&
                            pParent != NULL)
                        pEnemy->pAim = pParent;
                }
            }
        }
        pEnemy = pEnemy->pNext;			// Nächsten Gegner testen
    }
}

// --------------------------------------------------------------------------------------
// Projektil rendern
// --------------------------------------------------------------------------------------

void ProjectileClass::Render(void)
{
    D3DCOLOR Color;

    // Schuss rendern
    //
    if (Damage <= 0)
        return;

    if (OwnDraw == false)
    {
        float l,  r,  o,  u;					// Vertice Koordinaten
        float tl, tr, to, tu;					// Textur Koordinaten

        D3DXMATRIX	matRot, matTrans, matTrans2;

        float xts = ProjectileGrafix[ShotArt].itsXTexScale;
        float yts = ProjectileGrafix[ShotArt].itsYTexScale;

        //DKS - There is no need to compute this, it's already in the sprite's itsPreCalcedRects array:
        //int xfs, yfs, xfc
        //xfs = ProjectileGrafix[ShotArt].itsXFrameSize;
        //yfs = ProjectileGrafix[ShotArt].itsYFrameSize;
        //xfc = ProjectileGrafix[ShotArt].itsXFrameCount;
        //RECT Rect;
        //// Ausschnitt berechnen
        //Rect.top	= (AnimPhase/xfc) * yfs;
        //Rect.left	= (AnimPhase%xfc) * xfs;
        //Rect.right  = Rect.left + xfs;
        //Rect.bottom = Rect.top  + yfs;

        RECT &Rect = ProjectileGrafix[ShotArt].itsPreCalcedRects[AnimPhase];

        l = float(-TileEngine.XOffset+xPos-0.5f);								// Links
        r = float(-TileEngine.XOffset+xPos+(Rect.right -Rect.left-1)+0.5f);	// Rechts
        o = float(-TileEngine.YOffset+yPos-0.5f);								// Oben
        u = float(-TileEngine.YOffset+yPos+(Rect.bottom-Rect.top-1) +0.5f);	// Unten

        tl = Rect.left   * xts;	// Links
        tr = Rect.right  * xts;	// Rechts
        to = Rect.top    * yts;	// Oben
        tu = Rect.bottom * yts;	// Unten

        Color = 0xFFFFFFFF;

        QUAD2D TriangleStrip; //DKS - Added local declaration
        
        //DKS - Altered this code to assign to TriangleStrip directly:
        TriangleStrip.v1.color = TriangleStrip.v2.color = TriangleStrip.v3.color = TriangleStrip.v4.color = Color;

        TriangleStrip.v1.x		= l;		// Links oben
        TriangleStrip.v1.y		= o;
        TriangleStrip.v1.tu		= tl;
        TriangleStrip.v1.tv		= to;

        TriangleStrip.v2.x		= r;		// Rechts oben
        TriangleStrip.v2.y		= o;
        TriangleStrip.v2.tu		= tr;
        TriangleStrip.v2.tv		= to;

        TriangleStrip.v3.x		= l;		// Links unten
        TriangleStrip.v3.y		= u;
        TriangleStrip.v3.tu		= tl;
        TriangleStrip.v3.tv		= tu;

        TriangleStrip.v4.x		= r;		// Rechts unten
        TriangleStrip.v4.y		= u;
        TriangleStrip.v4.tu		= tr;
        TriangleStrip.v4.tv		= tu;

        // Schuss gedreht rendern?
        //
        if (Winkel > -10000.0f)
        {
            Winkel = float (int (Winkel)%360);

            // Rotationsmatrix
            D3DXMatrixRotationZ  (&matRot, DegreetoRad[int (Winkel)]);

            float x = float (-TileEngine.XOffset + xPos);
            float y = float (-TileEngine.YOffset + yPos);
            float mx = ProjectileGrafix[ShotArt].itsXFrameSize / 2.0f;
            float my = ProjectileGrafix[ShotArt].itsYFrameSize / 2.0f;

            // Transformation zum Ursprung
            D3DXMatrixTranslation(&matTrans,-x-mx,
                                  -y-my, 0.0f);

            // Transformation wieder zurück
            D3DXMatrixTranslation(&matTrans2,x+mx,
                                  y+my, 0.0f);

            D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans);		// Verschieben
            D3DXMatrixMultiply	 (&matWorld, &matWorld, &matRot);		// rotieren
            D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans2);	// und wieder zurück verschieben
#if defined(PLATFORM_DIRECTX)
            lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
#elif defined(PLATFORM_SDL)
            g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
            load_matrix( GL_MODELVIEW, g_matModelView.data() );
#endif
#endif

            DirectGraphics.SetFilterMode (true);
        }

        if (ShotArt != CurrentShotTexture)
        {
            DirectGraphics.SetTexture(  ProjectileGrafix[ShotArt].itsTexIdx );
            CurrentShotTexture = ShotArt;
        }

        // Sprite zeichnen
        //DKS - Altered to match new QUAD2D strips:
        DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2, &TriangleStrip);

        if (Winkel > -10000.0f)
        {
            // Normale Projektions-Matrix wieder herstellen
            D3DXMatrixRotationZ (&matWorld, 0.0f);
#if defined(PLATFORM_DIRECTX)
            lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
#elif defined(PLATFORM_SDL)
            g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
            load_matrix( GL_MODELVIEW, g_matModelView.data() );
#endif
#endif

            DirectGraphics.SetFilterMode (false);
        }

#ifdef _DEBUG
        if (DebugMode == true)
            RenderRect (float (xPos - TileEngine.XOffset + ShotRect[ShotArt].left),
                        float (yPos - TileEngine.YOffset + ShotRect[ShotArt].top),
                        float (ShotRect[ShotArt].right  - ShotRect[ShotArt].left),
                        float (ShotRect[ShotArt].bottom - ShotRect[ShotArt].top), 0x88FFFFFF);
#endif //_DEBUG

    }

    // Elektropampe rendern
    else if (ShotArt == BOUNCESHOT1)
    {
        int size = (pParent->CurrentWeaponLevel[2] * 2 + 12);
        if (size < 20)
            size = 20;
        ProjectileGrafix[ShotArt].RenderSpriteScaled((float)(xPos-TileEngine.XOffset) + 12 - size / 2.0f,
                (float)(yPos-TileEngine.YOffset) + 12 - size / 2.0f, size, size, 0xFFFFFFFF);
    }
    else if (ShotArt == ELEKTROPAMPE)
    {
        DirectGraphics.SetAdditiveMode();

        if (Counter > 255.0f)
            Color = D3DCOLOR_RGBA(255, 255, 255, 255);
        else
            Color = D3DCOLOR_RGBA(255, 255, 255, (int)(Counter));

        ProjectileGrafix[ShotArt].itsRect = ProjectileGrafix[ShotArt].itsPreCalcedRects[AnimPhase];
        ProjectileGrafix[ShotArt].RenderSprite((float)(xPos-TileEngine.XOffset),
                (float)(yPos-TileEngine.YOffset), Color);

        DirectGraphics.SetColorKeyMode();
    }
    else if (ShotArt == FEUERFALLE ||
             ShotArt == SPIDERFIRE ||
             ShotArt == PLAYERFIRE)
    {
        DirectGraphics.SetAdditiveMode();

        if (ShotArt == PLAYERFIRE)
            Color = D3DCOLOR_RGBA(255, 128, 80, (int)(Counter));
        else
            Color = D3DCOLOR_RGBA(255, 192, 128, 192 - (AnimPhase * 6));

        ProjectileGrafix[ShotArt].itsRect = ProjectileGrafix[ShotArt].itsPreCalcedRects[AnimPhase];

        ProjectileGrafix[ShotArt].RenderSpriteScaledRotated (46+(float)(xPos-TileEngine.XOffset)-(AnimPhase+30),
                54+(float)(yPos-TileEngine.YOffset)-(AnimPhase+30),
                AnimPhase*2.2f+20, AnimPhase*2.2f+20, Winkel, Color);
        DirectGraphics.SetColorKeyMode();
    }

    // Feuer des Lavamanns
    //
    else if (ShotArt == FEUERFALLE_LAVAMANN)
    {
        DirectGraphics.SetAdditiveMode();
        Color = D3DCOLOR_RGBA(255, 255, 255, 192 - (AnimPhase * 6));

        int a = 30 - AnimPhase;

        ProjectileGrafix[ShotArt].RenderSpriteScaled (46+(float)(xPos-TileEngine.XOffset)-(a+30),
                54+(float)(yPos-TileEngine.YOffset)-(a+30),
                int (a*2.2f+20), int (a*2.2f+20), AnimPhase, Color);
        DirectGraphics.SetColorKeyMode();
    }

    // Druckwellen Smartbomb anzeigen
    else if (ShotArt == SMARTBOMB)
    {
        DirectGraphics.SetAdditiveMode();
        ShotRect[ShotArt].bottom = (255-int(Damage/2))*2;
        ShotRect[ShotArt].right  = (255-int(Damage/2))*2;

        Color = D3DCOLOR_RGBA(255, 255, 255, int(Damage/2));
        ProjectileGrafix[ShotArt].RenderSpriteScaled ((float)(xPos-TileEngine.XOffset),
                (float)(yPos-TileEngine.YOffset),
                (255-int(Damage/2))*2, (255-int(Damage/2))*2, Color);

        Color = D3DCOLOR_RGBA(255, 255, 255, int(Damage/4));
        ProjectileGrafix[ShotArt].RenderSpriteScaled ((float)(xPos-TileEngine.XOffset),
                (float)(yPos-TileEngine.YOffset),
                (255-int(Damage/2))*2, (255-int(Damage/2))*2, Color);

        DirectGraphics.SetColorKeyMode();
    }
    else if (ShotArt == BLITZBEAM)
    {
        DirectGraphics.SetAdditiveMode();
        Color = 0xFFFFFFFF;

        ProjectileGrafix[ShotArt].RenderSpriteScaled ((float)(xPos-TileEngine.XOffset),
                (float)(yPos-TileEngine.YOffset),
                Damage, Damage, AnimPhase, Color);

        DirectGraphics.SetColorKeyMode();
    }

    // Fetter Spinnenlaser
    else if (ShotArt == SPIDERLASER)
    {
        D3DCOLOR col;
        static float off = 0.0f;

        DirectGraphics.SetAdditiveMode();
        ShotRect[ShotArt].bottom = 0;
        ShotRect[ShotArt].right  = 0;

        float size = 0.0f;
        int length = 0;

        // Größe festlegen
        if (AnimCount < 45.0f)
        {
            length = 480;
            size = 10.0f;
            col = D3DCOLOR_RGBA(255, 255, 255, (int)(AnimCount / 45.0f * 255.0f));

            ShotRect[SPIDERLASER].left = 0;
            ShotRect[SPIDERLASER].right  = 0;
            ShotRect[SPIDERLASER].top  = 0;
            ShotRect[SPIDERLASER].bottom = 0;
        }
        else
        {
            length = 560;
            size = (7.0f) * 30.0f + (float)sin(off) * 10.0f;
            off += 5.0f SYNC;
            col = 0xFFFFFFFF;

            ShotRect[SPIDERLASER].left = 0;
            ShotRect[SPIDERLASER].right  = 500;
            ShotRect[SPIDERLASER].top  = -100;
            ShotRect[SPIDERLASER].bottom = 100;
            Damage = 500;
        }

        ProjectileGrafix[ShotArt].RenderSpriteScaled ((float)(xPos-TileEngine.XOffset) - 10.0f,
                (float)(yPos-TileEngine.YOffset) - size / 2.0f,
                length, int(size), 0, col);
        DirectGraphics.SetColorKeyMode();
    }
    else if (ShotArt == BRATKLOPSSHOT)
    {
        DirectGraphics.SetAdditiveMode();

        ProjectileGrafix[ShotArt].RenderSpriteRotated((float)(xPos-TileEngine.XOffset),
                (float)(yPos-TileEngine.YOffset),
                (xPos + yPos) / 2, 0x88FFFFFF);

        ProjectileGrafix[ShotArt].RenderSpriteRotated((float)(xPos-TileEngine.XOffset),
                (float)(yPos-TileEngine.YOffset),
                (xPos + yPos) / 2, 0x88FFFFFF);

        DirectGraphics.SetColorKeyMode();
    }

    CurrentShotTexture = ShotArt;


    // Leuchten bei bestimmten Schussarten anzeigen ?
    if (HasGlow == true &&
            options_Detail >= DETAIL_HIGH)
    {
        DirectGraphics.SetAdditiveMode();
        CurrentShotTexture = -1;				// Erst mal einstellen dass wir jetzt die falsche Textur
        // gesetzt haben =)

        if (ShotArt == UFOLASER)
        {
            PartikelGrafix[UFOLASERFLARE].RenderSpriteRotated(xPos-float(TileEngine.XOffset) - 63,
                    yPos-float(TileEngine.YOffset) - 30, yPos * 0.5f, 0xFFFF22EE);
        }
        else if (ShotArt == ELEKTROSCHUSS)
        {
            Projectiles.LavaFlare.RenderSpriteScaled(xPos-28-float(TileEngine.XOffset),
                                         yPos-28-float(TileEngine.YOffset), 100, 100, 0xFFFF22BB);
        }
        else if (ShotArt == LASERSHOT ||
                 ShotArt == LASERSHOT2)
        {
            switch (int (Winkel))
            {
            case 0 :
                Projectiles.LaserSmoke.RenderSpriteRotated(xPos-float(TileEngine.XOffset) - 60 + (ShotArt - LASERSHOT) * 5,
                                               yPos-float(TileEngine.YOffset) - 40, Winkel, 0xBB1188FF);
                break;

            case 45 :
                Projectiles.LaserSmoke.RenderSpriteRotated(xPos-float(TileEngine.XOffset) - 60,
                                               yPos-float(TileEngine.YOffset) - 40, Winkel, 0xBB1188FF);
                break;

            case 90 :
                Projectiles.LaserSmoke.RenderSpriteRotated(xPos-float(TileEngine.XOffset) - 53,
                                               yPos-float(TileEngine.YOffset) - 40, Winkel, 0xBB1188FF);
                break;

            case 270 :
                Projectiles.LaserSmoke.RenderSpriteRotated(xPos-float(TileEngine.XOffset) - 52,
                                               yPos-float(TileEngine.YOffset) - 40, Winkel, 0xBB1188FF);
                break;

            case 315 :
                Projectiles.LaserSmoke.RenderSpriteRotated(xPos-float(TileEngine.XOffset) - 60,
                                               yPos-float(TileEngine.YOffset) - 40, Winkel, 0xBB1188FF);
                break;

            default:
                break;
            }
        }

        else if (ShotArt == LASERSHOTBIG ||
                 ShotArt == LASERSHOTBIG2)
        {
            switch (int (Winkel))
            {
            case 0 :
                Projectiles.LaserSmokeBig.RenderSpriteRotated(xPos-float(TileEngine.XOffset) - 120,
                                                  yPos-float(TileEngine.YOffset) - 80, Winkel, 0xBB1188FF);
                break;

            case 45 :
                Projectiles.LaserSmokeBig.RenderSpriteRotated(xPos-float(TileEngine.XOffset) - 120,
                                                  yPos-float(TileEngine.YOffset) - 85, Winkel, 0xBB1188FF);
                break;

            case 90 :
                Projectiles.LaserSmokeBig.RenderSpriteRotated(xPos-float(TileEngine.XOffset) - 120,
                                                  yPos-float(TileEngine.YOffset) - 85, Winkel, 0xBB1188FF);
                break;

            case 270 :
                Projectiles.LaserSmokeBig.RenderSpriteRotated(xPos-float(TileEngine.XOffset) - 105,
                                                  yPos-float(TileEngine.YOffset) - 85, Winkel, 0xBB1188FF);
                break;

            case 315 :
                Projectiles.LaserSmokeBig.RenderSpriteRotated(xPos-float(TileEngine.XOffset) - 120,
                                                  yPos-float(TileEngine.YOffset) - 85, Winkel, 0xBB1188FF);
                break;

            default:
                break;
            }
        }

        else if (ShotArt == SPREADSHOTBIG ||
                 ShotArt == SPREADSHOTBIG2)
        {
            Projectiles.SpreadShotSmoke.RenderSprite(xPos-16-float(TileEngine.XOffset),
                                         yPos-18-float(TileEngine.YOffset), 0, 0xAAFFFFFF);
        }

        else if (ShotArt == PFLANZESHOT)
        {
            Projectiles.SpreadShotSmoke.RenderSprite(xPos-24-float(TileEngine.XOffset),
                                         yPos-24-float(TileEngine.YOffset), 0, 0xAAFFFFFF);
        }

        else if (ShotArt == SUCHSCHUSS2)
        {
            Projectiles.LavaFlare.RenderSpriteScaled(xPos-24-float(TileEngine.XOffset),
                                         yPos-24-float(TileEngine.YOffset), 64, 64, 0xCC0088FF);
        }

        else if (ShotArt == WALKER_LASER)
            Projectiles.LavaFlare.RenderSprite(xPos-45-float(TileEngine.XOffset),
                                   yPos-60-float(TileEngine.YOffset), 0, 0xAAFF3300);

        else if (ShotArt == FETTESPINNESHOT ||
                 ShotArt == FETTESPINNESHOT2)		// Laser der fetten Spinne
        {
            Projectiles.SpreadShotSmoke.RenderSprite(xPos-12-float(TileEngine.XOffset),
                                         yPos- 7-float(TileEngine.YOffset), 0, 0xFFFF0000);
        }

        if (ShotArt == POWERLINE)				// Powerline leuchten lassen ?
        {
            if (xSpeed < 0.0f)
                Projectiles.PowerlineSmoke.RenderSprite(xPos-10-float(TileEngine.XOffset),
                                            yPos   -float(TileEngine.YOffset), 0, 0xFFFFFFFF);
            else
                Projectiles.PowerlineSmoke.RenderSprite(xPos-28-float(TileEngine.XOffset),
                                            yPos   -float(TileEngine.YOffset), 1, 0xFFFFFFFF);
        }

        DirectGraphics.SetColorKeyMode();
    } // Leuchten bei bestimmten Schussarten anzeigen

    // Animieren
    if (ShotArt != EIERBOMBE && AnimEnde > 0)						// Soll überhaupt animiert werden ?
    {
        if (AnimCount > AnimSpeed)			// Grenze überschritten ?
        {
            AnimCount = AnimCount - AnimSpeed;	// Dann wieder auf Null setzen
            AnimPhase++;						// Und nächste Animationsphase
            //DKS - off-by-one error:
            //if (AnimPhase == AnimEnde)			// Animation von zu Ende	?
            if (AnimPhase > AnimEnde)			// Animation von zu Ende	?
                AnimPhase = 0;					// Dann wieder von vorne beginnen
        }
    }

}

// --------------------------------------------------------------------------------------
// Projektil animieren und bewegen
// --------------------------------------------------------------------------------------

void ProjectileClass::Run(void)
{
    bo = bu = bl = br = 0;

    if (AnimEnde > 0)						// Soll überhaupt anmiert werden ?
        AnimCount += 1.0f SYNC;				// Animationscounter weiterzählen

    // Bewegen
    xSpeed += xAcc   SYNC;
    ySpeed += yAcc   SYNC;
    xPos   += xSpeed SYNC;
    yPos   += ySpeed SYNC;

    // Screen verlassen ?
    if (ShotArt != FLAMME		&&
            ShotArt != ROCKETSPIDER &&
            ShotArt != EVILBLITZ    &&
            ShotArt != EVILBLITZ2   &&
            ShotArt != SNOWBOMB     &&
            ShotArt != EIERBOMBE    &&
            (yPos-TileEngine.YOffset						    > 480 + 100 ||
             yPos-TileEngine.YOffset+ShotRect[ShotArt].bottom < 0   - 100 ||
             xPos-TileEngine.XOffset						    > 640 + 100 ||
             xPos-TileEngine.XOffset+ShotRect[ShotArt].right  < 0   - 100))
        Damage = 0;

    // Level verlassen ?
    if ((ShotArt == FLAMME	  ||
            ShotArt == SNOWBOMB  ||
            ShotArt == EIERBOMBE ||
            ShotArt == EVILBLITZ ||
            ShotArt == EVILBLITZ2) &&
            (yPos > TileEngine.LEVELPIXELSIZE_Y||
             xPos > TileEngine.LEVELPIXELSIZE_X||
             yPos < 0							 ||
             xPos < 0))
        Damage = 0;

    if (Damage == 0)
        return;

    // Testen ob ein zerstörbares Leveltile von einem Schuss des Spielers
    // getroffen wurde und wenn ja, dann dieses und den Schuss zerstören
    //
    if (DamagePlayer == false &&
            ShotArt != BOMBE &&
            ShotArt != BOMBEBIG &&
            ShotArt != SHIELDSPAWNER)
        if (TileEngine.BlockDestroyRechts(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) ||
                TileEngine.BlockDestroyLinks (xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) ||
                TileEngine.BlockDestroyOben  (xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) ||
                TileEngine.BlockDestroyUnten (xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]))
        {
            // Alle Schüsse ausser dem Beam (und dem Schutzschild, welches auch ein Schuss ist) exlodieren
            // an der zerschiessbaren Wand
            //
            if (ShotArt != BLITZBEAM	 &&
                    ShotArt != SHIELDSPAWNER)
            {
                ExplodeShot ();
                Damage = 0;
            }
            else
                TileEngine.CheckDestroyableWalls(xPos, yPos, xSpeed, ySpeed, ShotRect[ShotArt]);
        }

    if (CheckBlock == true)
    {
        bo = TileEngine.BlockOben			(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);
        bu = TileEngine.BlockUntenNormal  (xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);
        bl = TileEngine.BlockLinks		(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);
        br = TileEngine.BlockRechts		(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);
    }

    // Auf Kollision mit dem Level Testen (nur bei Bounce-fähigen Schüssen)
    if(BounceWalls == true)
    {
        int DoDamage = 0;

        if ((ySpeed < 0.0f &&
                bo & BLOCKWERT_WAND) ||
                (ySpeed > 0.0f &&
                 bu & BLOCKWERT_WAND))
        {
            ySpeed *= -1.0f;
            DoDamage = 1;
        }

        if ((xSpeed > 0.0f &&
                br & BLOCKWERT_WAND) ||
                (xSpeed < 0.0f &&
                 bl & BLOCKWERT_WAND))
        {
            xSpeed *= -1.0f;
            DoDamage = 1;
        }

        if (ShotArt == BOUNCESHOT2 ||
                ShotArt == BOUNCESHOT3 ||
                ShotArt == BOUNCESHOTBIG2 ||
                ShotArt == BOUNCESHOTBIG3)
        {
            Damage -= DoDamage;

            if (Damage <= 0)
            {
                SoundManager.StopWave(SOUND_BOUNCESHOT);
                SoundManager.PlayWave(100, 128, 11025, SOUND_BOUNCESHOT);

                ExplodeShot();
            }
        }
    }

    // Verschiedene Schüsse unterschiedlich behandeln
    switch(ShotArt)
    {
    case SKELETOR_SHOT:
    case SPREADSHOT:					// Der Streuschuss (explodiert an der Wand)
    case SPREADSHOT2:					// Der Streuschuss (explodiert an der Wand)
    case SPREADSHOTBIG:
    case SPREADSHOTBIG2:
    {
        if ((TileEngine.BlockUntenNormal  (xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) & BLOCKWERT_WAND &&
                ySpeed > 0.0f) ||
                bo & BLOCKWERT_WAND ||
                bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND)
        {
            Damage = 0;

            SoundManager.StopWave(SOUND_SPREADHIT);
            SoundManager.PlayWave(50, 128, rand()%2000+11025, SOUND_SPREADHIT);
            ExplodeShot();
        }
    }
    break;

    case LASERSHOT:						// Der Laser (explodiert nur an der Wand, nicht am Gegner)
    case LASERSHOT2:					// Der Laser (explodiert nur an der Wand, nicht am Gegner)
    case LASERSHOTBIG:
    case LASERSHOTBIG2:
    {
        //DKS - off-by-one error:
        //if (AnimPhase >= AnimEnde)
        if (AnimPhase > AnimEnde)
            AnimPhase = 0;

        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case BOUNCESHOT1:					// Der Bounce explodiert an der Wand
    case BOUNCESHOTBIG1:
    {
        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND)
        {
            if (BounceWalls == true)
            {
                Damage-=3;

                if (xSpeed == 0.0f ||
                        ySpeed == 0.0f)
                    ySpeed = xSpeed;

                if (Damage <= 0)
                    ExplodeShot();
            }
            else
            {
                ExplodeShot();
                Damage = 0;
            }
        }
    }
    break;

    case BOUNCESHOT2:					// Der mittlere Bounce explodiert nur, wenn er sich nochmal aufteilen würde
    case BOUNCESHOTBIG2:
    {
        if (pParent->CurrentWeaponLevel[pParent->SelectedWeapon] >= 3 &&
                (bl & BLOCKWERT_WAND ||
                 br & BLOCKWERT_WAND ||
                 bu & BLOCKWERT_WAND ||
                 bo & BLOCKWERT_WAND))
        {
            if (pParent->CurrentWeaponLevel[pParent->SelectedWeapon] >= 5)
            {
                ExplodeShot();
                Damage = 0;
            }
        }

    }
    break;

    case POWERLINE:					// Die Powerline (explodiert an der Wand)
    {
        if ((xSpeed < 0.0f && bl & BLOCKWERT_WAND) ||
                (xSpeed > 0.0f && br & BLOCKWERT_WAND))
        {
            Damage		 = 0;
            ExplodeShot();
        }
    }
    break;

    case FLAMME:					// Flammen-Splitter explodiert auch an der Wand
    {
        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                TileEngine.BlockUntenNormal  (xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) & BLOCKWERT_WAND ||
                TileEngine.BlockUntenNormal  (xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) & BLOCKWERT_PLATTFORM)
        {
            Damage		 = 0;
            ExplodeShot();
        }
    }
    break;

    case CANONBALL:					// Kannonenkugel des Geschützturms
    case SUCHSCHUSS:				// Kannonenkugel des Deckenturms
    case SUCHSCHUSS2:				// Blauer Schuss der Riesenwespe
    case WALKER_LASER:				// Walkerlaser
    case SPIDER_LASER:				// Spinnenlaser
    case SPIDERSLOW:
    {
        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                TileEngine.BlockUntenNormal  (xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) & BLOCKWERT_WAND)
        {
            Damage		 = 0;
            ExplodeShot();
        }
    }
    break;

    case FIREBALL:
    {
        AnimCount -= 1.0f SYNC;

        if (AnimCount < 0.0f)
        {
            AnimCount = 0.1f;
            PartikelSystem.PushPartikel(xPos, yPos + 10 + rand()%5, FIREBALL_SMOKE);
        }

        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND)
        {
            ExplodeShot();
            Damage = 0;
        }
    }
    break;

    case FIREBALL_BIG:
    {
        AnimCount -= 1.0f SYNC;

        if (AnimCount < 0.0f)
        {
            AnimCount = 0.1f;

            for (int i = 0; i < 4; i++)
                PartikelSystem.PushPartikel(xPos + 10 + rand()%20, yPos + 10 + rand()%20, FIREBALL_SMOKE);
        }

        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND)
        {
            ExplodeShot();
            Damage = 0;
        }
    }
    break;

    case FIREBALL_BOMB:
    {
        Winkel += 100.0f SYNC;
        AnimCount -= 1.0f SYNC;

        if (AnimCount < 0.0f)
        {
            AnimCount = 1.0f;

            PartikelSystem.PushPartikel(xPos, yPos, SMOKEBIG);

            for (int i = 0; i < 5; i++)
                PartikelSystem.PushPartikel(xPos + rand()%50, yPos + rand()%50, FUNKE);
        }

        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND)
        {
            ExplodeShot();
            Damage = 0;
        }
    }
    break;

    case SPIDERLASER:
    {
        AnimCount += 2.5f SYNC;

        // Partikel fliegen
        if (AnimCount > 45.0f)
            AnimSpeed -= 1.0f SYNC;

        if (AnimSpeed < 0.0f)
        {
            AnimSpeed = 0.25f;
            PartikelSystem.PushPartikel(xPos + 460, yPos + (float)(rand()%170 - 90), LILA3);

            if (AnimPhase > 0)
            {
                AnimPhase = 0;
                SoundManager.PlayWave(100, 128, 12000, SOUND_SPIDERLASER);
                SoundManager.PlayWave(100, 128, 8000, SOUND_LILA);
                SoundManager.StopWave(SOUND_BEAMLOAD2);
            }
        }
        else
        {
            // Frequenz setzen
            //DKS - Added check for NULLness and DirectX, since SDL port doesn't support SetFrequency:
#if defined(PLATFORM_DIRECTX)
            int Freq = 11025 + (int)(AnimCount * 500.0f);
            // DKS - added function SetWaveFrequency()
            SetWaveFrequency(SOUND_BEAMLOAD2, Freq);
#endif
        }

        if (AnimCount > 140.0f)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case SPIDERSHOT:
    {
        //DKS - Lightmaps have been disabled (never worked originally, see Tileengine.cpp's
        //      comments for DrawLightmap()):
        //TileEngine.DrawLightmap(LIGHTMAP_LILA, xPos + 65.0f, yPos + 40.0f, 255);

        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                TileEngine.BlockUntenNormal  (xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) & BLOCKWERT_WAND)
        {
            Damage		 = 0;
            ExplodeShot();
        }

    }
    break;

    case SPIDERSHOT2:
    {
        AnimCount -= 1.0f SYNC;
        if (AnimCount <= 0.0f)
        {
            AnimCount = 0.3f;
            PartikelSystem.PushPartikel(xPos + 12, yPos + 12, LILA);
        }

        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                TileEngine.BlockUntenNormal  (xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) & BLOCKWERT_WAND)
        {
            Damage		 = 0;
            ExplodeShot();
        }
    }
    break;

    case TURRIEXTRAWURST:
    case GOLEMSCHUSS:				// Suchschuss des Golems
    {
        DirectGraphics.SetAdditiveMode();
        Projectiles.LavaFlare.RenderSpriteScaled((float)(xPos - TileEngine.XOffset) - 21,
                                     (float)(yPos - TileEngine.YOffset) - 21, 62, 62,
                                     0xAA88FF00);
        DirectGraphics.SetColorKeyMode();
        CurrentShotTexture = -1;

        if (ShotArt == GOLEMSCHUSS)
        {
            if ((yAcc > 0.0f &&
                    ySpeed > Counter) ||
                    (yAcc < 0.0f &&
                     ySpeed < -Counter))
                yAcc *= -1.0f;
        }
        else
        {
            if (Counter > 0.0f)
            {
                Counter -= 1.0f SYNC;

                if (ySpeed >= 0.0f)
                    ySpeed = 0.0f;

            }
            else
            {
                if (yAcc != 0.0f)
                {
                    PlayerClass *pAim;
                    yAcc = 0.0f;

                    if (pParent == NULL)
                        pAim = ChooseAim();
                    else
                        pAim = pParent;

                    //DKS - Converted to float:
                    float	absx, absy, speed;				// Variablen für die Geschwindigkeits-
                    // berechnung
                    absx = pAim->xpos+35-xPos;			// Differenz der x
                    absy = pAim->ypos+40-yPos;			// und y Strecke

                    //DKS - Converted to float:
                    speed = 1.0f/sqrtf(absx*absx + absy*absy);	// Länge der Strecke berechnen
                    speed = speed*35;						// Geschwindigkeit ist 4 fach

                    absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
                    absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

                    xSpeed = float(absx);
                    ySpeed = float(absy);
                }
            }
        }

        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case ROTZSHOT:
    {
        Counter -= 1.0f SYNC;

        if (Counter <= 0.0f)
        {
            Counter = 0.1f;
            PartikelSystem.PushPartikel(xPos+ 4.0f, yPos, PHARAOSMOKE);
        }

        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND)// ||
            //bu & BLOCKWERT_PLATTFORM)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case BOMBE:
    case BOMBEBIG:
    {
        bool fahrstuhlunten = false;

        bu = TileEngine.BlockUntenNormal (xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);

        if (TileEngine.IsElevatorLevel &&
                yPos + ShotRect[ShotArt].bottom + 4 > g_Fahrstuhl_yPos)
            fahrstuhlunten = true;

        // Kein Boden? Dann runterfallen
        //
        if (!(bu & BLOCKWERT_WAND) &&
                !(bu & BLOCKWERT_PLATTFORM) &&
                !fahrstuhlunten)
        {
            yAcc = 8.0f;
        }

        // Geschwindigkeit begrenzen
        //
        if (ySpeed > 40.0f)
            ySpeed = 40.0f;

        // an der Wand abprallen
        //
        if ((xSpeed < 0.0f && bl & BLOCKWERT_WAND) ||
                (xSpeed > 0.0f && br & BLOCKWERT_WAND))
            xSpeed *= -1.0f;

        // fällt runter?
        if (ySpeed > 0.0f)
        {
            // Am Boden abprallen
            //
            if (bu & BLOCKWERT_WAND ||
                    bu & BLOCKWERT_PLATTFORM ||
                    fahrstuhlunten)
            {
                ySpeed *= -0.75f;

                // an der Schräge seitlich abprallen
                //
                if (bl & BLOCKWERT_SCHRAEGE_L) xSpeed =  10.0f;
                if (br & BLOCKWERT_SCHRAEGE_R) xSpeed = -10.0f;
            }
        }

        //DKS - off-by-one error:
        //if (AnimPhase == AnimEnde-1)
        if (AnimPhase == AnimEnde)
        {
            //DKS - These two lines were already commented out in original source:
            //AnimEnde--;
            //if (AnimEnde <= 0)
            {
                Damage = 0;
                ExplodeShot();
            }
        }

    }
    break;

    case FEUERFALLE:
    case FEUERFALLE_LAVAMANN:
    {
        Winkel += 10.0f SYNC;
        if (AnimPhase >= 29)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case GOLEMSAEULE:
    {
        // Counter runterzählen
        AnimCount -= 1.0f SYNC;

        if (AnimCount <= 0.0f)
        {
            AnimCount = 13.0f;

            SoundManager.PlayWave(100, 128, 11025, SOUND_FEUERFALLE);

            for (int i = 0; i < 25; i++)
                Projectiles.PushProjectile(xPos, yPos + i * 5.0f, FEUERFALLE);
        }

    }

    case SPIDERFIRE:
    {
        Winkel += 10.0f SYNC;
        if (AnimPhase >= 29)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case ELEKTROSCHUSS:
    {
        Counter -= 100.0f SYNC;

        if (Counter < 750.0f)
        {
            xAcc = -6.0f;
            yAcc = -5.0f;

            if (xSpeed < -25.0f)
                xSpeed = -25.0f;

            if (ySpeed < 4.0f)
                ySpeed = 4.0f;
        }

        if (Counter < 350.0f)
        {
            yAcc = 7.0f;
            xAcc = 1.0f;
        }

        if (Counter <= -150.0f)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case ELEKTROPAMPE:
    {
        //DKS - off-by-one error:
        //if (AnimPhase >= AnimEnde)
        if (AnimPhase > AnimEnde)
            AnimPhase = 0;

        Counter -= 70.0f SYNC;

        if (Counter <= 0.0f)
            Damage = 0;

        OwnDraw = true;
    }
    break;

    case PLAYERFIRE:
    {
        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND)
        {
            xSpeed = ySpeed = xAcc = yAcc = 0.0f;
        }


        if (AnimPhase > 10)
            Counter -= 50.0f SYNC;

        Winkel += 10.0f SYNC;
        if (AnimPhase >= 29 ||
                Counter <= 0.0f)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case KRABBLERLASER1:
    {
        if (bu & BLOCKWERT_WAND)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case KRABBLERLASER2:
    {
        if (br & BLOCKWERT_WAND)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case KRABBLERLASER3:
    {
        if (bl & BLOCKWERT_WAND)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case PHARAOLASER:
    {
        if (ySpeed >  20.0f) ySpeed =  20.0f;
        if (ySpeed < -20.0f) ySpeed = -20.0f;

        // Spieler verfolgen
        if (pParent->ypos + 40 < yPos + 24)	yAcc = -5.0f;
        else if (pParent->ypos + 40 > yPos + 24)	yAcc =  5.0f;

        for (int i=0; i < int(200 * Timer.getElapsed())+1; i++)
            PartikelSystem.PushPartikel(xPos + 18, yPos + 18, PHARAOSMOKE);

        if (bu & BLOCKWERT_WAND && ySpeed > 0.0f)
            ySpeed *= -1;

        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case ROCKETSPIDER:
    {
        // Flamme erzeugen
        for (int i=0; i < int(200 * Timer.getElapsed())+1; i++)
        {
            switch (AnimPhase)
            {
            case 0 :
            {
                PartikelSystem.PushPartikel(xPos + 33, yPos + 70, ROCKETSMOKE);
            }
            break;

            case 4 :
            {
                PartikelSystem.PushPartikel(xPos + 33, yPos - 2, ROCKETSMOKE);
            }
            break;
            }
        }

        // Counter runzerzählen wann die Rakete von oben auf den Spieler runtersaust
        AnimCount -= 1.0f SYNC;

        if (AnimCount <= 0.0f && AnimPhase == 0)
        {
            AnimPhase = 4;
            ySpeed = -ySpeed;
            xPos = pParent->xpos;
        }

        if (bu & BLOCKWERT_WAND)
        {
            Damage = 0;
            ExplodeShot();
        }

    }
    break;

    case ROCKETWERFER:
    {
        // Rakete fliegt noch runter bzw hoch
        if (xSpeed == 0.0f)
        {
            // Auf richtiger Höhe mit dem Spieler ? Dann seitlich losfliegen
            if (yPos > pParent->ypos + pParent->CollideRect.top &&
                    yPos < pParent->ypos + pParent->CollideRect.bottom)
            {
                if (ySpeed < 0) yAcc =  5.0f;
                else
                    yAcc = -5.0f;

                if (AnimPhase == 0)
                {
                    xSpeed = -10.0f;
                    xAcc = -5.0f;
                }
                else
                {
                    xSpeed =  10.0f;
                    xAcc =  5.0f;
                }

                SoundManager.PlayWave(100, 128, 11025, SOUND_ROCKET);
            }
        }

        else

        {
            // Flamme erzeugen
            for (int i=0; i < int(200 * Timer.getElapsed())+1; i++)
            {
                switch (AnimPhase)
                {
                case 0 :
                {
                    PartikelSystem.PushPartikel(xPos + 40, yPos, ROCKETSMOKE);
                }
                break;

                case 1 :
                {
                    PartikelSystem.PushPartikel(xPos, yPos, ROCKETSMOKE);
                }
                break;
                }
            }

            // gerade fliegen
            if (yAcc < 0)	if (ySpeed < 0) ySpeed = yAcc = 0.0f;
            if (yAcc > 0)	if (ySpeed > 0) ySpeed = yAcc = 0.0f;
        }

        // Wand getroffen
        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND)
        {
            Damage = 0;
            ExplodeShot();
        }

    }
    break;

    case TORPEDO:
    {
        // Spieler verfolgen
        if (yPos + 5 < pParent->ypos + 45) yAcc =  2.0f;
        if (yPos + 5 > pParent->ypos + 45) yAcc = -2.0f;

        if (ySpeed < -10.0f) ySpeed = -10.0f;
        if (ySpeed >  10.0f) ySpeed =  10.0f;

        // Blubbern
        AnimCount -= 1.0f SYNC;
        if (AnimCount < 0)
        {
            AnimCount += 0.5f;

            switch (AnimPhase)
            {
            case 0 :
            {
                PartikelSystem.PushPartikel(xPos + 30, yPos, BUBBLE);
            }
            break;

            case 1 :
            {
                PartikelSystem.PushPartikel(xPos, yPos, BUBBLE);
            }
            break;
            }
        }

        // Wand getroffen
        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND)
        {
            Damage = 0;
            ExplodeShot();
        }

    }
    break;

    case EVILBLITZ:
    {
        for (int i=0; i < int(200 * Timer.getElapsed())+1; i++)
            PartikelSystem.PushPartikel(xPos + 10 + rand()%50, yPos + 60 + rand()%20, EVILFUNKE);

        if (ySpeed < -45.0f)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case EVILBLITZ2:
    {
        if (bu & BLOCKWERT_WAND && ySpeed > 0.0f)
        {
            ySpeed = 0.0f;
            AnimEnde = 0;
            AnimPhase = 0;
            AnimCount = 255.0f;

            SoundManager.PlayWave(100, 128, 11025, SOUND_BLITZHIT);
        }

        if (ySpeed == 0.0f)
        {
            AnimCount -= 40.0f SYNC;
            if (AnimCount < 0.0f)
            {
                Damage = 0;
                ExplodeShot();
            }
        }
    }
    break;

    case BLITZBEAM:
    {
        if (Damage > 0)
            Counter -= 1.0f SYNC;

        if (Counter <= 0.0f)
        {
            Counter = 0.1f;

            PartikelSystem.PushPartikel(xPos + int(Damage / 2) + 1 + rand()%3,
                                          yPos + int(Damage / 2) + 1 + rand()%3,  BEAMSMOKE);

            PartikelSystem.PushPartikel(xPos + int(Damage / 2) - 2 + rand()%3,
                                          yPos + int(Damage / 2) - 6 + rand()%3,  BEAMSMOKE2);
        }

        // Wand getroffen?
        //
        // DKS - Fixed indentation / replaced commas with semicolons on first two lines here:
        ShotRect[BLITZBEAM].left   = int(Damage / 4.0f);
        ShotRect[BLITZBEAM].top    = int(Damage / 4.0f);
        ShotRect[BLITZBEAM].right  = ShotRect[BLITZBEAM].left + int(Damage / 2.0f);
        ShotRect[BLITZBEAM].bottom = ShotRect[BLITZBEAM].top  + int(Damage / 2.0f);

        bo = TileEngine.BlockOben			(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);
        bu = TileEngine.BlockUntenNormal  (xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);
        bl = TileEngine.BlockLinks		(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);
        br = TileEngine.BlockRechts		(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);

        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND)
        {
            //if (Damage > 200.0f)
            //	ShakeScreen((float)Damage / 20.0f);

            ExplodeShot();
        }
    }
    break;

    case EISZAPFENSHOT:
    case UFOLASER:
    case STELZLASER:
    case PFLANZESHOT:
    case BRATKLOPSSHOT:
    case DRONEBULLET:
    case SCHLEIMSHOT:
    case LAFASSSHOT:
    case BLUEBOMB:
    {
        // Wand getroffen
        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case SKELETORGRANATE:
    {
        // Drehwinkel aus der Geschwindigkeit errechnen
        //DKS - Converted to float, new Rad/Deg macros:
        //float w = 90 + float(atan(ySpeed / xSpeed) * 360.0f / (D3DX_PI * 2));
        float w = 90.0f + RadToDeg(atanf(ySpeed / xSpeed));
        Winkel = w;

        if (xSpeed < 0.0f)
            Winkel = Winkel + 180.0f;

        // Boden getroffen? Dann stoppen
        if (bu & BLOCKWERT_WAND)
        {
            xSpeed = 0.0f;
            ySpeed = 0.0f;
            xAcc = 0.0f;
            yAcc = 0.0f;
            Winkel = 180.0f;
        }

        Counter -= 2.0f SYNC;

        if (Counter <= 0.0f)
        {
            if (!(bu & BLOCKWERT_WAND))
            {
                if (xSpeed > 0.0f)
                    PartikelSystem.PushPartikel(xPos-5, yPos - 5, SMOKE);
                else
                    PartikelSystem.PushPartikel(xPos-5, yPos, SMOKE);
            }

            Counter = 1.0f;
            Damage--;

            if (Damage <= 0)
                ExplodeShot();
        }
    }
    break;

    case FLUGLASER:
    {
        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND ||
                bu & BLOCKWERT_PLATTFORM)
        {
            Damage = 0;
            ExplodeShot();
        }

    }
    break;

    case ARCSHOT:
    {
        // Rauch erzeugen
        AnimCount -= 1.0f SYNC;
        if (AnimCount <= 0.0f)
        {
            PartikelSystem.PushPartikel(xPos - 2, yPos - 2, ROCKETSMOKEBLUE);
            AnimCount = 0.1f;
        }

        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND ||
                bu & BLOCKWERT_PLATTFORM)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case SNOWBOMB:
    {
        // Rauch erzeugen
//			for (int i=0; i < int(50 * Timer.getElapsed())+1; i++)
//				PartikelSystem.PushPartikel(xPos - 2, yPos - 2, ROCKETSMOKEBLUE);

        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND ||
                bu & BLOCKWERT_PLATTFORM)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case SNOWBOMBSMALL:
    {
        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND ||
                bu & BLOCKWERT_PLATTFORM)
        {
            // Abhopsen
            //
            if (ySpeed > 0.0f)
                ySpeed = -ySpeed / 1.5f;

            // Bis nich mehr geht ;)
            //
            if (ySpeed > -0.5f)
            {
                Damage = 0;
                ExplodeShot();
            }
        }
    }
    break;

    case FETTESPINNESHOT:
    case FETTESPINNESHOT2:
    {
        //DKS - off-by-one error:
        //if (AnimPhase >= AnimEnde)
        if (AnimPhase > AnimEnde)
            AnimPhase = 0;

        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case EIERBOMBE:
    {
        // Links oder rechts rum kullern ?

        if (AnimCount > AnimSpeed)			// Grenze überschritten ?
        {
            AnimCount = AnimCount - AnimSpeed;	// Dann wieder auf Null setzen

            // linksrum
            if (xSpeed < 0.0f)
            {
                AnimPhase--;						// Und nächste Animationsphase
                if (AnimPhase < 0)					// Animation von zu Ende	?
                {
                    //DKS - off-by-one error:
                    //DKS - TODO double check I got this right
                    //AnimPhase = 19;
                    //AnimEnde  = 20;
                    AnimPhase = 18;
                    AnimEnde  = 19;
                }
            }

            // rechtsrum
            else
            {
                AnimPhase++;						// Und nächste Animationsphase
                //DKS - off-by-one error:
                //if (AnimPhase > 19)					// Animation von zu Ende	?
                //{
                //    AnimPhase = 0;
                //    AnimEnde  = 19;
                //}
                if (AnimPhase > AnimEnde)					// Animation von zu Ende	?
                {
                    AnimPhase = 0;
                    AnimEnde  = 19;
                }
            }
        }

        // Kugel fällt runter ?
        //
        if (ySpeed > 30.0f)							// Maximale Fallgeschwindigkeit
            ySpeed = 30.0f;

        if (!(bu & BLOCKWERT_WAND) && !(bu & BLOCKWERT_PLATTFORM) && ySpeed == 0.0f)
        {
            ySpeed = 0.0f;
            yAcc   = 4.0f;
        }

        // Kugel hopft am Boden ab ?
        //
        if (ySpeed > 0.0f && (bu & BLOCKWERT_WAND || bu & BLOCKWERT_PLATTFORM))
        {
            ySpeed *= -0.75f;

            if (ySpeed > -1.0f)
            {
                ySpeed	 = 0.0f;
                yAcc	 = 0.0f;

                TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);
            }
        }

        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND)
        {
            Damage = 0;
            ExplodeShot();
        }

    }
    break;


    case EVILROUND1:
    {
        AnimCount -= 1.0f SYNC;

        if (AnimCount < 0.0f)
        {
            // Rauch erzeugen
            for (int i=0; i < 2; i++)
                PartikelSystem.PushPartikel(xPos - 2, yPos - 2, EVILROUNDSMOKE);

            AnimCount += 0.1f;
        }

        if (bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                bu & BLOCKWERT_WAND ||
                bu & BLOCKWERT_PLATTFORM)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;

    case SMARTBOMB:
    {
        AnimCount += 32.0f SYNC;
        xPos -= 32.0f SYNC;
        yPos -= 32.0f SYNC;
        if (AnimCount > 256.0f)
            AnimCount = 256.0f;

        Damage = int (255-AnimCount)*2;
    }
    break;

    case GRENADE:
    {
        // rauchen lassen
        //
        AnimCount -= 1.0f SYNC;

        while (AnimCount <= 0.0f)
        {
            AnimCount += 0.1f;
            PartikelSystem.PushPartikel (xPos - 4, yPos - 4 + float (rand ()% 3), SMOKE2);
        }

        if (TileEngine.BlockUntenNormal  (xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND ||
                bl & BLOCKWERT_WAND ||
                br & BLOCKWERT_WAND)
        {
            Damage = 0;
            ExplodeShot();
        }
    }
    break;


    // Der Satellit, der das Schutzschild erzeugt, wird immer im Kreis um den Spieler bewegt. Da die Position
    // immer neu berechnet wird, werden xspeed und yspeed nicht benötigt. Daher können sie zweckentfremdet werden,
    // um die aktuelle Rotation zu speichern (xspeed) und den counter für die shield-partikel (yspeed)
    //
    case SHIELDSPAWNER:
    {
        // Schild um Spieler kreisen lassen
        if (pParent->Blickrichtung == LINKS)
            ySpeed += 0.5f SYNC;
        else
            ySpeed -= 0.5f SYNC;

        // Grenzen checken
        if (ySpeed > D3DX_PI * 2)	ySpeed -= D3DX_PI * 2;
        if (ySpeed < 0.0f)		 	ySpeed += D3DX_PI * 2;


        if (pParent->Handlung != RADELN &&
                pParent->Handlung != RADELN_FALL)
        {
            //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            //xPos = float (pParent->xpos + 28 + sin (ySpeed) * 40);
            //yPos = float (pParent->ypos + 38 + cos (ySpeed) * 40);
            xPos = float(pParent->xpos) + 28.0f + sin_rad(ySpeed) * 40.0f;
            yPos = float(pParent->ypos) + 38.0f + cos_rad(ySpeed) * 40.0f;
        }
        else
        {
            //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            //xPos = float (pParent->xpos + 28 + sin (ySpeed) * 30);
            //yPos = float (pParent->ypos + 55 + cos (ySpeed) * 30);
            xPos = float(pParent->xpos) + 28.0f + sin_rad(ySpeed) * 30.0f;
            yPos = float(pParent->ypos) + 55.0f + cos_rad(ySpeed) * 30.0f;
        }

        // Schild Partikel spawnen
        xSpeed -= 1.0f SYNC;

        if (xSpeed < 0.0f)
        {
            xSpeed = 0.1f;
            PartikelSystem.PushPartikel (xPos, yPos, SHIELD, pParent);
        }

        // Spawner verschwinden lassen ?
        if (pParent->Shield <= 0.0f)
        {
            ExplodeShot();
            Damage = 0;
        }

    }
    break;


    default :
        break;
    }

    // Hat ein Gegnerschuss den Spieler getroffen ?
    //bool coll1 = false;
    //bool coll2 = false;

    for (int p = 0; p < NUMPLAYERS; p++)
        if (DamagePlayer == true &&
                SpriteCollision(xPos, yPos, ShotRect[ShotArt],
                                Player[p].xpos, Player[p].ypos, Player[p].CollideRect) == true)
        {
            // Schüsse, die durch den Spieler durchgehen
            if (ExplodeOnImpact == false)
                Player[p].DamagePlayer(float (Damage SYNC));

            // Schüsse, die bei Berührung verschwinden
            else
            {
                Player[p].DamagePlayer(float(Damage));

                ExplodeShot();							// Schuss Explodieren lassen
                Damage = 0;								// und verschwinden lassen
            }

            // Spieler wurde gerendert -> Textur ging flöten =)
            CurrentShotTexture = -1;
        }

    // Schüsse Additiv rendern ?
    if 	(ShotArt >= FLAMME)
        DirectGraphics.SetAdditiveMode();
    else
        DirectGraphics.SetColorKeyMode();

    //D3DCOLOR Color;

    //Color = 0xFFFFFFFF;

    // Evil Blitz fadet auch aus
    //if (ShotArt == EVILBLITZ2 && ySpeed == 0.0f)
    //{
    //int a = int (AnimCount);
    //Color = D3DCOLOR_RGBA(255, 255, 255, a);
    //}

    // Alte Position sichern
    xPosOld = xPos;
    yPosOld = yPos;
}

// --------------------------------------------------------------------------------------
// Schuss "explodieren" lassen und Partikel erzeugen
// Bei Kollision mit Level oder Gegner
// --------------------------------------------------------------------------------------

void ProjectileClass::ExplodeShot(void)
{
    CurrentShotTexture = -1;

    // Jeder Schuss erzeugt beim explodieren andere Partikel
    switch (ShotArt)
    {
    case FIREBALL:
    {
        SoundManager.PlayWave(80, 128, 15000 + rand()%2000, SOUND_EXPLOSION1);
        PartikelSystem.PushPartikel(xPos, yPos, SHOTFLARE);

        for(int i=0; i<4; i++)
        {
            PartikelSystem.PushPartikel(xPos+5 + rand()%20, yPos+5+rand()%10, FUNKE);
            PartikelSystem.PushPartikel(xPos + rand()%20, yPos-5+rand()%10, SMOKE);
            PartikelSystem.PushPartikel(xPos+5 + rand()%20, yPos+5+rand()%10, LASERFUNKE2);
        }

        PartikelSystem.PushPartikel(xPos-10 , yPos, EXPLOSION_MEDIUM2);
    }
    break;

    case FIREBALL_BIG:
    {
        SoundManager.PlayWave(80, 128, 10000 + rand()%2000, SOUND_EXPLOSION1);
        PartikelSystem.PushPartikel(xPos, yPos, SHOTFLARE);

        for(int i=0; i<4; i++)
        {
            PartikelSystem.PushPartikel(xPos+5 + rand()%20, yPos+5+rand()%10, FUNKE);
            PartikelSystem.PushPartikel(xPos + rand()%20, yPos-5+rand()%10, SMOKEBIG);
            PartikelSystem.PushPartikel(xPos+5 + rand()%20, yPos+5+rand()%10, LASERFUNKE2);
        }

        PartikelSystem.PushPartikel(xPos-10 , yPos-20, EXPLOSION_BIG);
    }
    break;

    case FIREBALL_BOMB:
    {
        int i;

        SoundManager.PlayWave(80, 128, 10000 + rand()%2000, SOUND_EXPLOSION1);

        for(i=0; i<2; i++)
            PartikelSystem.PushPartikel(xPos - 30, yPos - 20, EXPLOSIONFLARE);

        for(i=0; i<4; i++)
            PartikelSystem.PushPartikel(xPos + rand()%20 - 10, yPos-5+rand()%10, SMOKEBIG);

        for (int i = 0; i < 8; i++)
            Projectiles.PushProjectile(xPos - 20 + rand()%30, yPos + 20 + rand()%30, FEUERFALLE);


    }
    break;

    case ROTZSHOT:
    {
        SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_SPREADHIT);
        PartikelSystem.PushPartikel(xPos, yPos, SHOTFLARE);

        for(int i=0; i<4; i++)
        {
            PartikelSystem.PushPartikel(xPos - 5 + rand()%10, yPos-5+rand()%10, FUNKE);
            PartikelSystem.PushPartikel(xPos - 5 + rand()%10, yPos-5+rand()%10, LASERFUNKE2);
        }
    }
    break;

    case DIAMONDSHOT:
    {
        PartikelSystem.PushPartikel(xPos, yPos, DIAMANTCOLLECTED);
        SoundManager.PlayWave(50, 128, 11025, SOUND_EXPLOSION1);
        SoundManager.PlayWave(100, 128, 11025, SOUND_COLLECT);
    }
    break;

    case SKELETOR_SHOT:
    {
        PartikelSystem.PushPartikel(xPos+5, yPos+5, SHOTFLARE);

        for(int i=0; i<4; i++)
            PartikelSystem.PushPartikel(xPos+10, yPos+10, FUNKE);


        if (rand()%3 == 0)
            for(int i=0; i<3; i++)
                if (rand()%2 == 0)
                    PartikelSystem.PushPartikel(xPos+10, yPos+5, LONGFUNKE);
    }
    break;

    case SPREADSHOT:
    case SPREADSHOT2:
    {
        PartikelSystem.PushPartikel(xPos, yPos, SHOTFLARE);

        for(int i=0; i<4; i++)
            PartikelSystem.PushPartikel(xPos+5, yPos+5, FUNKE);


        if (rand()%3 == 0)
            for(int i=0; i<3; i++)
                if (rand()%2 == 0)
                    PartikelSystem.PushPartikel(xPos+5, yPos-15, LONGFUNKE);
    }
    break;

    case SPREADSHOTBIG:
    case SPREADSHOTBIG2:
    {
        SoundManager.PlayWave(50, 128, rand()%2000+11025, SOUND_SPREADHIT);
        for(int i=0; i<8; i++)
            PartikelSystem.PushPartikel(xPos+5, yPos+5, FUNKE);

        for(int i=0; i<3; i++)
            PartikelSystem.PushPartikel(xPos+5, yPos+5, LONGFUNKE);
    }
    break;


    case LASERSHOT:
    case LASERSHOT2:
    {
        PartikelSystem.PushPartikel(xPos - 50, yPos - 30, LASERFLARE);

        for(int i=0; i<3; i++)
            PartikelSystem.PushPartikel(xPos+10, yPos+5, LASERFUNKE);

        if (xSpeed > 0.0f)
            PartikelSystem.PushPartikel(xPos+25, yPos+8, BLUE_EXPLOSION);
        else
            PartikelSystem.PushPartikel(xPos-5, yPos+8, BLUE_EXPLOSION);
    }
    break;

    case LASERSHOTBIG:
    case LASERSHOTBIG2:
    {
        for(int i=0; i<8; i++)
            PartikelSystem.PushPartikel(xPos+10 + rand()%70, yPos+5, LASERFUNKE);

        if (xSpeed > 0.0f)
            for(int i=0; i<5; i++)
                PartikelSystem.PushPartikel(xPos+10 + rand ()%70, yPos-10 + rand()%20, BLUE_EXPLOSION);
        else
            for(int i=0; i<5; i++)
                PartikelSystem.PushPartikel(xPos-20 + rand ()%20, yPos-10 + rand()%20, BLUE_EXPLOSION);
    }
    break;

    case BOUNCESHOT1:
    {
        PartikelSystem.PushPartikel(xPos+4, yPos+4, SHOTFLARE2);

        for(int i=0; i<4; i++)
            PartikelSystem.PushPartikel(xPos+8, yPos+8, FUNKE2);

        // Aufsplitten ?
        if (pParent->CurrentWeaponLevel[pParent->SelectedWeapon] > 2)
        {
            if (bl & BLOCKWERT_WAND)
            {
                WinkelUebergabe = 45;
                Projectiles.PushProjectile(xPos + 4 - xSpeed SYNC, yPos + 4 - ySpeed SYNC, BOUNCESHOT2, pParent);
                WinkelUebergabe = 135;
                Projectiles.PushProjectile(xPos + 4 - xSpeed SYNC, yPos + 4 - ySpeed SYNC, BOUNCESHOT2, pParent);
            }
            else if (br & BLOCKWERT_WAND)
            {
                WinkelUebergabe = 315;
                Projectiles.PushProjectile(xPos + 4 - xSpeed SYNC, yPos + 4 - ySpeed SYNC, BOUNCESHOT2, pParent);
                WinkelUebergabe = 225;
                Projectiles.PushProjectile(xPos + 4 - xSpeed SYNC, yPos + 4 - ySpeed SYNC, BOUNCESHOT2, pParent);
            }
            else if (bo & BLOCKWERT_WAND)
            {
                WinkelUebergabe = 225;
                Projectiles.PushProjectile(xPos + 4 - xSpeed SYNC, yPos + 4 - ySpeed SYNC, BOUNCESHOT2, pParent);
                WinkelUebergabe = 135;
                Projectiles.PushProjectile(xPos + 4 - xSpeed SYNC, yPos + 4 - ySpeed SYNC, BOUNCESHOT2, pParent);
            }
            else
            {
                WinkelUebergabe = 315;
                Projectiles.PushProjectile(xPos + 4 - xSpeed SYNC, yPos + 4 - ySpeed SYNC, BOUNCESHOT2, pParent);
                WinkelUebergabe = 45;
                Projectiles.PushProjectile(xPos + 4 - xSpeed SYNC, yPos + 4 - ySpeed SYNC, BOUNCESHOT2, pParent);
            }
        }

    }
    break;

    case BOUNCESHOT2:
    {
        SoundManager.StopWave(SOUND_BOUNCESHOT);
        SoundManager.PlayWave(100, 128, 11025, SOUND_BOUNCESHOT);

        PartikelSystem.PushPartikel(xPos, yPos, SHOTFLARE2);

        for(int i=0; i<2; i++)
            PartikelSystem.PushPartikel(xPos+8, yPos+8, FUNKE2);

        // Aufsplitten ?
        if (pParent->CurrentWeaponLevel[pParent->SelectedWeapon] >= 5)
        {
            if (bl & BLOCKWERT_WAND)
            {
                WinkelUebergabe = 45;
                Projectiles.PushProjectile(xPos + 3 - xSpeed SYNC, yPos + 3 - ySpeed SYNC, BOUNCESHOT3, pParent);
                WinkelUebergabe = 135;
                Projectiles.PushProjectile(xPos + 3 - xSpeed SYNC, yPos + 3 - ySpeed SYNC, BOUNCESHOT3, pParent);
            }
            else if (br & BLOCKWERT_WAND)
            {
                WinkelUebergabe = 315;
                Projectiles.PushProjectile(xPos + 3 - xSpeed SYNC, yPos + 3 - ySpeed SYNC, BOUNCESHOT3, pParent);
                WinkelUebergabe = 225;
                Projectiles.PushProjectile(xPos + 3 - xSpeed SYNC, yPos + 3 - ySpeed SYNC, BOUNCESHOT3, pParent);
            }
            else if (bo & BLOCKWERT_WAND)
            {
                WinkelUebergabe = 225;
                Projectiles.PushProjectile(xPos + 3 - xSpeed SYNC, yPos + 3 - ySpeed SYNC, BOUNCESHOT3, pParent);
                WinkelUebergabe = 135;
                Projectiles.PushProjectile(xPos + 3 - xSpeed SYNC, yPos + 3 - ySpeed SYNC, BOUNCESHOT3, pParent);
            }
            else
            {
                WinkelUebergabe = 315;
                Projectiles.PushProjectile(xPos + 3 - xSpeed SYNC, yPos + 3 - ySpeed SYNC, BOUNCESHOT3, pParent);
                WinkelUebergabe = 45;
                Projectiles.PushProjectile(xPos + 3 - xSpeed SYNC, yPos + 3 - ySpeed SYNC, BOUNCESHOT3, pParent);
            }
        }

    }
    break;

    case BOUNCESHOT3:
    {
        SoundManager.StopWave(SOUND_BOUNCESHOT);
        SoundManager.PlayWave(100, 128, 11025, SOUND_BOUNCESHOT);

        for(int i=0; i<3; i++)
            PartikelSystem.PushPartikel(xPos+4, yPos+4, FUNKE2);

    }
    break;

    case BOUNCESHOTBIG1:
    {
        SoundManager.StopWave(SOUND_BOUNCESHOT);
        SoundManager.PlayWave(100, 128, 11025, SOUND_BOUNCESHOT);

        PartikelSystem.PushPartikel(xPos-32, yPos-32, EXPLOSIONFLARE);

        for(int i=0; i<4; i++)
            PartikelSystem.PushPartikel(xPos+8, yPos+8, FUNKE2);

        // Aufsplitten ?
        if (pParent->CurrentWeaponLevel[pParent->SelectedWeapon] > 2)
        {
            if (bl & BLOCKWERT_WAND)
            {
                WinkelUebergabe = 45;
                Projectiles.PushProjectile(xPos + 8 - xSpeed SYNC, yPos + 8 - ySpeed SYNC, BOUNCESHOTBIG2, pParent);
                WinkelUebergabe = 135;
                Projectiles.PushProjectile(xPos + 8 - xSpeed SYNC, yPos + 8 - ySpeed SYNC, BOUNCESHOTBIG2, pParent);
            }
            else if (br & BLOCKWERT_WAND)
            {
                WinkelUebergabe = 315;
                Projectiles.PushProjectile(xPos + 8 - xSpeed SYNC, yPos + 8 - ySpeed SYNC, BOUNCESHOTBIG2, pParent);
                WinkelUebergabe = 225;
                Projectiles.PushProjectile(xPos + 8 - xSpeed SYNC, yPos + 8 - ySpeed SYNC, BOUNCESHOTBIG2, pParent);
            }
            else if (bo & BLOCKWERT_WAND)
            {
                WinkelUebergabe = 225;
                Projectiles.PushProjectile(xPos + 8 - xSpeed SYNC, yPos + 8 - ySpeed SYNC, BOUNCESHOTBIG2, pParent);
                WinkelUebergabe = 135;
                Projectiles.PushProjectile(xPos + 8 - xSpeed SYNC, yPos + 8 - ySpeed SYNC, BOUNCESHOTBIG2, pParent);
            }
            else
            {
                WinkelUebergabe = 315;
                Projectiles.PushProjectile(xPos + 8 - xSpeed SYNC, yPos + 8 - ySpeed SYNC, BOUNCESHOTBIG2, pParent);
                WinkelUebergabe = 45;
                Projectiles.PushProjectile(xPos + 8 - xSpeed SYNC, yPos + 8 - ySpeed SYNC, BOUNCESHOTBIG2, pParent);
            }
        }

    }
    break;

    case BOUNCESHOTBIG2:
    {
        SoundManager.StopWave(SOUND_BOUNCESHOT);
        SoundManager.PlayWave(100, 128, 11025, SOUND_BOUNCESHOT);

        PartikelSystem.PushPartikel(xPos-32, yPos-32, EXPLOSIONFLARE);

        for(int i=0; i<4; i++)
            PartikelSystem.PushPartikel(xPos+8, yPos+8, FUNKE2);

        // Aufsplitten ?
        if (pParent->CurrentWeaponLevel[pParent->SelectedWeapon] >= 5)
        {
            if (bl & BLOCKWERT_WAND)
            {
                WinkelUebergabe = 45;
                Projectiles.PushProjectile(xPos + 8 - xSpeed SYNC, yPos + 8 - ySpeed SYNC, BOUNCESHOTBIG3, pParent);
                WinkelUebergabe = 135;
                Projectiles.PushProjectile(xPos + 8 - xSpeed SYNC, yPos + 8 - ySpeed SYNC, BOUNCESHOTBIG3, pParent);
            }
            else if (br & BLOCKWERT_WAND)
            {
                WinkelUebergabe = 315;
                Projectiles.PushProjectile(xPos + 8 - xSpeed SYNC, yPos + 8 - ySpeed SYNC, BOUNCESHOTBIG3, pParent);
                WinkelUebergabe = 225;
                Projectiles.PushProjectile(xPos + 8 - xSpeed SYNC, yPos + 8 - ySpeed SYNC, BOUNCESHOTBIG3, pParent);
            }
            else if (bo & BLOCKWERT_WAND)
            {
                WinkelUebergabe = 225;
                Projectiles.PushProjectile(xPos + 8 - xSpeed SYNC, yPos + 8 - ySpeed SYNC, BOUNCESHOTBIG3, pParent);
                WinkelUebergabe = 135;
                Projectiles.PushProjectile(xPos + 8 - xSpeed SYNC, yPos + 8 - ySpeed SYNC, BOUNCESHOTBIG3, pParent);
            }
            else
            {
                WinkelUebergabe = 315;
                Projectiles.PushProjectile(xPos + 8 - xSpeed SYNC, yPos + 8 - ySpeed SYNC, BOUNCESHOTBIG3, pParent);
                WinkelUebergabe = 45;
                Projectiles.PushProjectile(xPos + 8 - xSpeed SYNC, yPos + 8 - ySpeed SYNC, BOUNCESHOTBIG3, pParent);
            }
        }

    }
    break;

    case CANONBALL :
    case SUCHSCHUSS :
    {
        SoundManager.PlayWave(25, 128, 11025, SOUND_SPREADHIT);
        for(int i=0; i<4; i++)
            PartikelSystem.PushPartikel(xPos+5, yPos+5, FUNKE);

        PartikelSystem.PushPartikel(xPos-10, yPos-10, SMOKE);
    }
    break;

    case SPIDERSHOT :
    case SPIDERSHOT2 :
    case SPIDERSLOW:
    {
        for (int i = 0; i < 10; i++)
            PartikelSystem.PushPartikel(xPos - 10.0f + rand()%40, yPos - 10.0f + rand()%40, LILA2);

        PartikelSystem.PushPartikel(xPos - 40.0f, yPos - 40.0f, EXPLOSIONFLARE2);
    }
    break;

    case ELEKTROSCHUSS:
    {
        SoundManager.PlayWave(100, 128, 6000, SOUND_BLITZENDE);
        SoundManager.PlayWave(100, 128, 6000, SOUND_BLITZENDE);
        SoundManager.PlayWave(100, 128, 9000, SOUND_BLITZENDE);
        SoundManager.PlayWave(100, 128, 9000, SOUND_BLITZENDE);

        Projectiles.PushProjectile((float)TileEngine.XOffset - 60,
                                     (float)TileEngine.YOffset + 480.0f - 128.0f,
                                     ELEKTROPAMPE);
    }
    break;

    /*
    		case SPIDERSHOT :
    		{
    			//Projectiles.PushProjectile(xPos, yPos, SPIDERSHOT3);
    		} break;
    */

    case SUCHSCHUSS2 :
    {
        SoundManager.PlayWave(25, 128, 11025, SOUND_FUNKE2);
        SoundManager.PlayWave(25, 128, 11025, SOUND_FUNKE3);

        PartikelSystem.PushPartikel(xPos - 50, yPos - 50, LASERFLARE);

        for(int i=0; i<10; i++)
            PartikelSystem.PushPartikel(xPos + rand ()% 12, yPos + rand ()% 12, LASERFUNKE);

        for (int i = 0; i < 4; i++)
            PartikelSystem.PushPartikel(xPos - 4 + rand ()% 8, yPos - 4 + rand ()% 8, SMOKE3);
    }
    break;

    case TURRIEXTRAWURST :
    case GOLEMSCHUSS :
    {
        SoundManager.PlayWave(100, 128, 11025, SOUND_EXPLOSION3);

        for (int i = 0; i < 40; i++)
            PartikelSystem.PushPartikel(xPos + rand()%35, yPos + rand()%35, FUNKE2);
    }
    break;

    case WALKER_LASER :
    case SPIDER_LASER :
    {
        SoundManager.PlayWave(25, 128, 11025, SOUND_SPREADHIT);
        for(int i=0; i<8; i++)
            PartikelSystem.PushPartikel(xPos+10, yPos, FUNKE);
    }
    break;

    case PHARAOLASER:
    {
        for (int i=0; i < 50; i++)
            PartikelSystem.PushPartikel(xPos + rand()%48-6, yPos + rand()%48-6, PHARAOSMOKE);
    }
    break;

    case ROCKET:
    case ROCKETSPIDER:
    {
        PartikelSystem.PushPartikel(xPos + 8, yPos + 8, EXPLOSION_MEDIUM2);

        for (int i=0; i < 100; i++)
        {
            PartikelSystem.PushPartikel(xPos + rand()%55+10 - 6, yPos + rand()%55+10 - 6, ROCKETSMOKE);
            PartikelSystem.PushPartikel(xPos + rand()%35+20 - 6, yPos + rand()%35+20 - 6, ROCKETSMOKE);
        }

        SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);
    }
    break;

    case ROCKETWERFER:
    {
        PartikelSystem.PushPartikel(xPos + 8, yPos - 20, EXPLOSION_MEDIUM2);

        for (int i=0; i < 75; i++)
        {
            PartikelSystem.PushPartikel(xPos + rand()%40 + 18, yPos + rand()%40 - 20, ROCKETSMOKE);
            PartikelSystem.PushPartikel(xPos + rand()%20 + 26, yPos + rand()%20 - 10, ROCKETSMOKE);
        }

        SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);
    }
    break;

    case TORPEDO:
    {
        PartikelSystem.PushPartikel(xPos + 8, yPos - 20, EXPLOSION_MEDIUM2);

        for (int i=0; i < 20; i++)
            PartikelSystem.PushPartikel(xPos + rand()%33 , yPos + rand()%10, BUBBLE);

        SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);
    }
    break;

    case EVILSHOT:
    case EVILSHOT2:
    {
        PartikelSystem.PushPartikel(xPos, yPos - 20, EXPLOSION_MEDIUM2);

        SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);
    }
    break;

    case FLUGLASER:
    {
        for (int i=0; i < 12; i++)
            PartikelSystem.PushPartikel(xPos + rand()%4, yPos + 10 + rand()%14, FUNKE);

        for (int i=0; i < 6; i++)
            PartikelSystem.PushPartikel(xPos + rand()%4, yPos + 10 + rand()%14, LASERFUNKE2);

        SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_SPREADHIT);
    }
    break;

    case EISZAPFENSHOT:
    {
        PartikelSystem.PushPartikel(xPos - 10, yPos + 8, SNOWFLUSH);

        SoundManager.PlayWave(25, 128, 8000 + rand()%4000, SOUND_SPREADHIT);
    }
    break;

    case EVILBLITZ:
    {
        Projectiles.PushProjectile(xPos-290, yPos-300, EVILBLITZ2);
        Projectiles.PushProjectile(xPos-100, yPos-200, EVILBLITZ2);
        Projectiles.PushProjectile(xPos+100, yPos-100, EVILBLITZ2);
        Projectiles.PushProjectile(xPos+290, yPos,     EVILBLITZ2);

        SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);
    }
    break;

    case BLITZBEAM:
    {
        // Wirkung ähnlich einer schwachen Granate (Damage = 0 - 100)
        //
        Gegner.DamageEnemiesonScreen(xPos, yPos, Damage);

        // Partikel
        //
        for (int i=0; i < 300; i++)	PartikelSystem.PushPartikel(xPos + Damage / 2.0f, yPos + Damage / 2.0f, BEAMSMOKE3);
        for (int i=0; i < 100; i++)	PartikelSystem.PushPartikel(xPos + Damage / 2.0f - 12, yPos + Damage / 2.0f - 12, BEAMSMOKE4);

        // 2 Druckwellen (xPos um 1 Pixel verschoben, dadurch breitet sich die eine mit 30, die andere mit 10 Pixeln aus)
        //

        PartikelSystem.PushPartikel(xPos - 32 + Damage / 2.0f, yPos - 32 + Damage / 2.0f, BEAMSMOKE5);
        PartikelSystem.PushPartikel(xPos - 31 + Damage / 2.0f, yPos - 32 + Damage / 2.0f, BEAMSMOKE5);

        // Screen aufblitzen
        PartikelSystem.ThunderAlpha = Damage * 2.0f;

        if (PartikelSystem.ThunderAlpha > 255.0f)
            PartikelSystem.ThunderAlpha = 255.0f;

        PartikelSystem.ThunderColor [0] = 96;
        PartikelSystem.ThunderColor [1] = 128;
        PartikelSystem.ThunderColor [2] = 255;

        Damage = 0;

        SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_BLITZENDE);
        SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);

    }
    break;

    /*
    		case UFOLASER:
    		{
    			for (int i=0; i < 50; i++)
    				PartikelSystem.PushPartikel(xPos + rand()%42,      yPos + rand()%100 , LASERFUNKE);

    			for (i=0; i < 20; i++)
    				PartikelSystem.PushPartikel(xPos - 10 + rand()%42, yPos + 50 + rand()%20 , BLUE_EXPLOSION);

    			SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);
    		} break;
    */

    case STELZLASER:
    {
        for (int i=0; i < 50; i++)
            PartikelSystem.PushPartikel(xPos + rand()%60-6, yPos + rand()%35+5, PHARAOSMOKE);

        SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_SPREADHIT);
    }
    break;

    case PFLANZESHOT:
    {
        for (int i=0; i < 20; i++)
            PartikelSystem.PushPartikel(xPos + rand()%16, yPos + rand()%16, FUNKE);

        SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_SPREADHIT);
    }
    break;

    case BRATKLOPSSHOT:
    {
        for (int i=0; i < 5; i++)
            PartikelSystem.PushPartikel(xPos  - 20 + rand()%40, yPos  - 20 + rand()%40, EXPLOSION_GREEN);

        for (int i=0; i < 100; i++)
            PartikelSystem.PushPartikel(xPos + 10 + rand()%40, yPos + 10 + rand()%40, FUNKE2);

        SoundManager.PlayWave(100, 128, 10000 + rand()%4000, SOUND_EXPLOSION1);
    }
    break;


    case DRONEBULLET:
    {
        SoundManager.PlayWave(50, 128, rand()%2000+11025, SOUND_SPREADHIT);
        for(int i=0; i<12; i++)
            PartikelSystem.PushPartikel(xPos+2, yPos+2, FUNKE);

        PartikelSystem.PushPartikel(xPos, yPos, SMOKE3);

        if (rand()%3 == 0)
            for(int i=0; i<3; i++)
                if (rand()%2 == 0)
                    PartikelSystem.PushPartikel(xPos+2, yPos-12, LONGFUNKE);
    }
    break;

    case SCHLEIMSHOT:
    {
        SoundManager.PlayWave(50, 128, rand()%2000+11025, SOUND_MADE);
        PartikelSystem.PushPartikel(xPos, yPos, MADEBLUT);
    }
    break;

    case SKELETORGRANATE:
    {
        SoundManager.PlayWave(50, 128, rand()%4000+8000, SOUND_EXPLOSION3);

        PartikelSystem.PushPartikel(xPos - 20,
                                      yPos - 20, EXPLOSION_MEDIUM2);

        for (int i = 0; i < 2; i++)
            PartikelSystem.PushPartikel(xPos - 10 + rand()%5,
                                          yPos - 10, SPIDERSPLITTER);


        for (int i = 0; i < 15; i++)
            Projectiles.PushProjectile(xPos - 20, yPos + i * 9, FEUERFALLE);
    }
    break;

    case BLUEBOMB:
    {
        SoundManager.PlayWave(50, 128, rand()%4000+8000, SOUND_EXPLOSION3);

        for (int i = 0; i < 10; i++)
            PartikelSystem.PushPartikel(xPos - 10 + rand ()%20,
                                          yPos - 10 + rand ()%20, BLUE_EXPLOSION);

        for (int i = 0; i < 15; i++)
            PartikelSystem.PushPartikel(xPos - 10 + rand ()%20,
                                          yPos - 10 + rand ()%20, WATERFLUSH_HIGH2);

    }
    break;

    case LAFASSSHOT:
    {
        SoundManager.PlayWave(100, 128, rand()%2000+11025, SOUND_EXPLOSION3);
        PartikelSystem.PushPartikel(xPos, yPos, EXPLOSION_TRACE);

        for (int i = 0; i < 20; i++)
            PartikelSystem.PushPartikel(xPos + 10 + rand()%40, yPos + 10 + rand()%60, SPIDERSPLITTER);

        for (int i = 0; i < 5; i++)
            PartikelSystem.PushPartikel(xPos - 30 + rand()%66, yPos - 30 + rand()%83, EXPLOSION_MEDIUM2);
    }
    break;

    case ARCSHOT:
    {
        //PartikelSystem.PushPartikel(xPos + 8, yPos + 8, EXPLOSION_MEDIUM2);

        for (int i=0; i < 50; i++)
        {
            PartikelSystem.PushPartikel(xPos + rand()%20-10 , yPos + rand()%20-10 , ROCKETSMOKEBLUE);
            PartikelSystem.PushPartikel(xPos + rand()%20-10 , yPos + rand()%20-10 , ROCKETSMOKEBLUE);
        }

        SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);
    }
    break;

    case SNOWBOMB:
    {
        for (int i=0; i < 8; i++)
            Projectiles.PushProjectile(xPos + rand()%60, yPos + rand()%50 , SNOWBOMBSMALL);

        for (int i=0; i < 20; i++)
            PartikelSystem.PushPartikel(xPos + rand()%70 - 10, yPos + rand()%70 - 10, SNOWFLUSH);

        SoundManager.PlayWave(100, 128, 6000 + rand()%2000, SOUND_LANDEN);
    }
    break;

    case SNOWBOMBSMALL:
    {
        for (int i=0; i < 4; i++)
            PartikelSystem.PushPartikel(xPos - 25 + rand()%20, yPos - 25 + rand()%20, SNOWFLUSH);

        SoundManager.PlayWave(100, 128, 6000 + rand()%2000, SOUND_LANDEN);
    }
    break;

    case FETTESPINNESHOT:
    case FETTESPINNESHOT2:
    {
        SoundManager.PlayWave(100, 128, 16000, SOUND_SPREADHIT);
        SoundManager.PlayWave(100, 128,  8000, SOUND_SPREADHIT);

        for (int i=0; i < 100; i++)
            PartikelSystem.PushPartikel(xPos - 5 + rand()%40, yPos - 5 + rand()%50, SMOKE2);

        for(int i=0; i<30; i++)
            PartikelSystem.PushPartikel(xPos+rand()%40,   yPos+rand()%50,   FUNKE);

        for(int i=0; i<40; i++)
            PartikelSystem.PushPartikel(xPos+rand()%20+10, yPos+rand()%30+10, PHARAOSMOKE);

    }
    break;

    case EIERBOMBE:
    {
        SoundManager.PlayWave(100, 128, 11025, SOUND_EXPLOSION1);

        for (int i=0; i < 30; i++)
            PartikelSystem.PushPartikel(xPos - 20 + rand()%60, yPos - 20 + rand()%60, SMOKE3);

        PartikelSystem.PushPartikel(xPos - 40, yPos - 40, EXPLOSION_GIANT);

        for (int i=0; i < 4; i++)
            PartikelSystem.PushPartikel(xPos - 30 + rand()%40, yPos - 30 + rand()%40, EXPLOSION_MEDIUM2);

    }
    break;

    case EVILROUND1:
    {
        for (int i=0; i < 50; i++)
        {
            PartikelSystem.PushPartikel(xPos + rand()%20-10 , yPos + rand()%20-10 , EVILROUNDSMOKE);
        }

        SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);
    }
    break;


    case BOMBE:
    case BOMBEBIG:
    {
        float xdiff, ydiff, Abstand;

        // Prüfen, ob die Bombe eine zerstörbare Wand getroffen hat
        //
        TileEngine.ExplodeWalls((int)xPos / 20, (int)yPos / 20);

        // Explodieren lassen
        SoundManager.PlayWave(80, 128, 11025, SOUND_EXPLOSION1);
        SoundManager.PlayWave(50, 128, 8000 + rand()%2000, SOUND_EXPLOSION3);

        int g = 2;

        if (ShotArt == BOMBEBIG)
            g = 5;

        for (int i = 0; i < g; i++)
        {
            PartikelSystem.PushPartikel(xPos-26, yPos-26, SMOKEBIG);
            PartikelSystem.PushPartikel(xPos+rand()%4, yPos+rand()%4, SPIDERSPLITTER);
        }

        PartikelSystem.PushPartikel(xPos-24, yPos-24, EXPLOSION_MEDIUM2);

        if (ShotArt == BOMBEBIG)
            PartikelSystem.PushPartikel(xPos-55, yPos-55, EXPLOSION_GIANT);

        // Prüfen, ob ein Gegner in der Nähe war und dann je nach Abstand
        // Energie abziehen
        GegnerClass	    *pEnemy;				// Gegner zum Kollision testen

        // je nach bombentyp mehr schaden
        int schaden = 80;

        if (ShotArt == BOMBEBIG)
            schaden = 250;

        pEnemy = Gegner.pStart;			// Anfang der Gegnerliste
        while (pEnemy != NULL)				// Noch nicht alle durch ?
        {
            xdiff = (xPos + ShotRect[ShotArt].left + (ShotRect[ShotArt].right - ShotRect[ShotArt].left)/2)
                    - (pEnemy->xPos + GegnerRect[pEnemy->GegnerArt].right/2);
            ydiff = (yPos + ShotRect[ShotArt].top + (ShotRect[ShotArt].bottom - ShotRect[ShotArt].top)/2)
                    - (pEnemy->yPos + GegnerRect[pEnemy->GegnerArt].bottom/2);

            //DKS - Converted to float:
            Abstand = sqrtf((xdiff * xdiff) + (ydiff * ydiff));

            if (pEnemy->Active == true		 &&	// Ist der Gegner überhaupt aktiv ?
                    pEnemy->Destroyable == true  &&	// und zerstörbar ?
                    Abstand <= schaden	         && // Und der Abstand klein genug ?
                    pEnemy->GegnerArt != EXTRAS  &&	// Und kein Extra ?
                    pEnemy->GegnerArt != ONEUP   &&	// Und kein OneUp ?
                    pEnemy->GegnerArt != DIAMANT)	// Und kein Diamant ?
            {
                // blinken lassen, wenn noch nicht blinkt
                if (pEnemy->DamageTaken <= 0.0f)
                    pEnemy->DamageTaken = 255;

                pEnemy->Energy -=(schaden-Abstand)*1.5f;// Dann Energy abziehen je nach Abstand
            }
            pEnemy = pEnemy->pNext;				// Nächsten Gegner testen
        }

        // Prüfen, ob der Spieler in der Nähe war, und falls ja, dann diesen
        // nach oben katapultieren
        for (int p = 0; p < NUMPLAYERS; p++)
        {
            xdiff = (xPos + ShotRect[ShotArt].left + (ShotRect[ShotArt].right - ShotRect[ShotArt].left)/2)
                    - (Player[p].xpos + Player[p].CollideRect.right/2);
            ydiff = (yPos + ShotRect[ShotArt].top + (ShotRect[ShotArt].bottom - ShotRect[ShotArt].top)/2)
                    - (Player[p].ypos + Player[p].CollideRect.bottom/2);

            //DKS - Converted to float:
            Abstand = sqrtf((xdiff * xdiff) + (ydiff * ydiff));

            if (((ShotArt == BOMBE    && Abstand <= 60) ||
                    (ShotArt == BOMBEBIG && Abstand <= 80)) &&
                    (Player[p].Handlung == RADELN ||
                     Player[p].Handlung == RADELN_FALL))
            {
                if (ShotArt == BOMBE)
                    Player[p].yspeed  -= 45.0f;					// Bombe
                else
                    Player[p].yspeed  -= 75.0f;					// SuperBombe


                Player[p].JumpAdd  = PLAYER_JUMPADDSPEED;	// lassen
                Player[p].Handlung = RADELN_FALL;			// als Rad
            }
        }

    }
    break;

    case FLAMME :								// Flammen-Splitter
    {
        SoundManager.PlayWave(25, 128, 11025, SOUND_EXPLOSION1);
        PartikelSystem.PushPartikel(xPos-16, yPos-16, EXPLOSION_MEDIUM2);

        for(int i=0; i<8; i++)
            PartikelSystem.PushPartikel(xPos+4, yPos+4, FUNKE);

    }
    break;

    case KRABBLERLASER1:
    {
        SoundManager.PlayWave(100, 128, 10000 + rand()%2000, SOUND_SPREADHIT);

        for(int i=0; i<24; i++)
            PartikelSystem.PushPartikel(xPos-1 + rand()%2, yPos+i*2+rand()%2, LASERFUNKE2);
    }
    break;

    case KRABBLERLASER2:
    {
        SoundManager.PlayWave(100, 128, 10000 + rand()%2000, SOUND_SPREADHIT);

        for(int i=0; i<8; i++)
            PartikelSystem.PushPartikel(xPos+36, yPos, LASERFUNKE2);
    }
    break;

    case KRABBLERLASER3:
    {
        SoundManager.PlayWave(100, 128, 10000 + rand()%2000, SOUND_SPREADHIT);

        for(int i=0; i<8; i++)
            PartikelSystem.PushPartikel(xPos, yPos, LASERFUNKE2);
    }
    break;

    case GRENADE:
    {
        SoundManager.PlayWave(100, 128, 10000 + rand()%2000, SOUND_EXPLOSION1);

        PartikelSystem.PushPartikel(xPos-100,
                                      yPos-100, EXPLOSION_GIGA);

        PartikelSystem.PushPartikel(xPos-20,
                                      yPos-20, EXPLOSION_MEDIUM2);

        for(int i=0; i<100; i++)
        {
            PartikelSystem.PushPartikel(xPos-20 + rand()%40,
                                          yPos-20 + rand()%40, ROCKETSMOKE);

            PartikelSystem.PushPartikel(xPos-20 + rand()%40,
                                          yPos-20 + rand()%40, FUNKE);
        }

        for(int i=0; i<50; i++)
            PartikelSystem.PushPartikel(xPos-20 + rand()%40,
                                          yPos-20 + rand()%40, LONGFUNKE);

        PartikelSystem.PushPartikel(xPos - 128, yPos - 128, GRENADEFLARE);


        ShakeScreen(5);
        Gegner.DamageEnemiesonScreen(xPos, yPos, 400);
        TileEngine.ExplodeWalls((int)xPos / 20, (int)yPos / 20);

        for (int p = 0; p < NUMPLAYERS; p++)
            DirectInput.Joysticks[Player[p].JoystickIndex].ForceFeedbackEffect(FFE_MEDIUMRUMBLE);

    }
    break;

    case SHIELDSPAWNER:
    {
        SoundManager.PlayWave (75, 128, 15000 + rand ()% 2000, SOUND_EXPLOSION1);

        for (int i = 0; i < 10; i++)
            PartikelSystem.PushPartikel (xPos - 5 + rand ()%10,
                                           yPos - 5 + rand ()%10, SMOKE);
        for (int i = 0; i < 30; i++)
            PartikelSystem.PushPartikel (xPos - 5 + rand ()%10,
                                           yPos - 5 + rand ()%10, FUNKE);
    }
    break;

    default:
        break;
    }

    Damage = 0;
}

// --------------------------------------------------------------------------------------
// ProjectileListClass Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor : laden der Projektil Grafiken
// --------------------------------------------------------------------------------------
ProjectileListClass::ProjectileListClass(void)
{
    pStart			= NULL;
    pEnd			= NULL;
    NumProjectiles	= 0;
}

void ProjectileListClass::LoadSprites(void)
{
    //DKS - All of these 5 sprites are no longer globals, I moved them here cleaning up big messes
    //      and fixing ambiguous orders of calls to destructors.
    // Grafiken für das Leuchten diverser Schüsse laden
    LaserSmoke.LoadImage     ("lasersmoke.png",	    128, 128, 128, 128, 1, 1);
    LaserSmokeBig.LoadImage  ("lasersmoke_big.png",	256, 256, 256, 256, 1, 1);
    SpreadShotSmoke.LoadImage("spreadshotsmoke.png",   64, 64, 64, 64, 1, 1);
    PowerlineSmoke.LoadImage ("powerlinesmoke.png",	 160, 20, 80, 20, 2, 1);
    LavaFlare.LoadImage      ("lavaflare.png", 120, 120, 120, 120, 1, 1);

    //DKS - Moved these three here from being globals in Player.cpp (same reasoning as above note)
    Blitzstrahl[0].LoadImage("blitzstrahl1.png",32,  32, 32, 32, 1, 1);
    Blitzstrahl[1].LoadImage("blitzstrahl2.png",32,  32, 32, 32, 1, 1);
    Blitzstrahl[2].LoadImage("blitzstrahl3.png",32,  32, 32, 32, 1, 1);
    Blitzstrahl[3].LoadImage("blitzstrahl4.png",32,  32, 32, 32, 1, 1);
    Blitzflash[0].LoadImage ("blitzflash1.png",	66,  64, 66, 64, 1, 1);
    Blitzflash[1].LoadImage ("blitzflash2.png",	66,  64, 66, 64, 1, 1);
    Blitzflash[2].LoadImage ("blitzflash3.png",	66,  64, 66, 64, 1, 1);
    Blitzflash[3].LoadImage ("blitzflash4.png",	66,  64, 66, 64, 1, 1);
    BlitzTexture.LoadImage  ("blitztexture.png", 64, 64, 64, 64, 1, 1);
    // Spieler grafiken laden
    SchussFlamme[0].LoadImage("schussflamme.png",  76,  72,  38, 24, 2, 3);
    SchussFlamme[1].LoadImage("schussflamme2.png", 80,  108,  40, 36, 2, 3);
    SchussFlamme[2].LoadImage("schussflamme3.png", 48,  114,  24, 38, 2, 3);
    SchussFlammeFlare.LoadImage("schussflammeflare.png", 140,  140,  140, 140, 1, 1);

    // Diamant schuss
    ProjectileGrafix[DIAMONDSHOT].LoadImage("diamant.png", 261, 29, 29, 29, 9, 1);
    ShotRect[DIAMONDSHOT].left = 5;
    ShotRect[DIAMONDSHOT].right  = 24;
    ShotRect[DIAMONDSHOT].top  = 5;
    ShotRect[DIAMONDSHOT].bottom = 24;

    // Skeletor Schuss
    ProjectileGrafix[SKELETOR_SHOT].LoadImage("skeletor_shot.png", 5, 40, 5, 40, 1, 1);
    ShotRect[SKELETOR_SHOT].left = 10;
    ShotRect[SKELETOR_SHOT].right  = 30;
    ShotRect[SKELETOR_SHOT].top  = 10;
    ShotRect[SKELETOR_SHOT].bottom = 30;

    // Spreadshot
    ProjectileGrafix[SPREADSHOT].LoadImage("spreadshot.png", 16, 16, 16, 16, 1, 1);
    ShotRect[SPREADSHOT].left = 4;
    ShotRect[SPREADSHOT].right  = 12;
    ShotRect[SPREADSHOT].top  = 4;
    ShotRect[SPREADSHOT].bottom = 12;

    // Spreadshot2
    ProjectileGrafix[SPREADSHOT2].LoadImage("spreadshot2.png", 20, 20, 20, 20, 1, 1);
    ShotRect[SPREADSHOT2].left = 2;
    ShotRect[SPREADSHOT2].right  = 16;
    ShotRect[SPREADSHOT2].top  = 2;
    ShotRect[SPREADSHOT2].bottom = 16;

    // Riesen Spreadshot
    ProjectileGrafix[SPREADSHOTBIG].LoadImage("spreadshot_big.png", 32, 32, 32, 32, 1, 1);
    ShotRect[SPREADSHOTBIG].left = 4;
    ShotRect[SPREADSHOTBIG].right  = 28;
    ShotRect[SPREADSHOTBIG].top  = 4;
    ShotRect[SPREADSHOTBIG].bottom = 28;

    // Riesen Spreadshot doppelt
    ProjectileGrafix[SPREADSHOTBIG2].LoadImage("spreadshot_big2.png", 40, 40, 40, 40, 1, 1);
    ShotRect[SPREADSHOTBIG2].left = 6;
    ShotRect[SPREADSHOTBIG2].right  = 34;
    ShotRect[SPREADSHOTBIG2].top  = 6;
    ShotRect[SPREADSHOTBIG2].bottom = 34;

    // Bounceshot
    ProjectileGrafix[BOUNCESHOT1].LoadImage("bounce1.png", 24, 24, 24, 24, 1, 1);
    ShotRect[BOUNCESHOT1].left = 4;
    ShotRect[BOUNCESHOT1].right  = 20;
    ShotRect[BOUNCESHOT1].top  = 4;
    ShotRect[BOUNCESHOT1].bottom = 20;

    ProjectileGrafix[BOUNCESHOT2].LoadImage("bounce2.png", 16, 16, 16, 16, 1, 1);
    ShotRect[BOUNCESHOT2].left = 4;
    ShotRect[BOUNCESHOT2].right  = 12;
    ShotRect[BOUNCESHOT2].top  = 4;
    ShotRect[BOUNCESHOT2].bottom = 12;

    ProjectileGrafix[BOUNCESHOT3].LoadImage("bounce3.png", 10, 10, 10, 10, 1, 1);
    ShotRect[BOUNCESHOT3].left = 0;
    ShotRect[BOUNCESHOT3].right  = 10;
    ShotRect[BOUNCESHOT3].top  = 0;
    ShotRect[BOUNCESHOT3].bottom = 10;

    // Riesen Bounceshot
    ProjectileGrafix[BOUNCESHOTBIG1].LoadImage("bounce_big1.png", 48, 48, 48, 48, 1, 1);
    ShotRect[BOUNCESHOTBIG1].left = 8;
    ShotRect[BOUNCESHOTBIG1].right  = 40;
    ShotRect[BOUNCESHOTBIG1].top  = 8;
    ShotRect[BOUNCESHOTBIG1].bottom = 40;

    ProjectileGrafix[BOUNCESHOTBIG2].LoadImage("bounce_big2.png", 32, 32, 32, 32, 1, 1);
    ShotRect[BOUNCESHOTBIG2].left = 4;
    ShotRect[BOUNCESHOTBIG2].right  = 28;
    ShotRect[BOUNCESHOTBIG2].top  = 4;
    ShotRect[BOUNCESHOTBIG2].bottom = 28;

    ProjectileGrafix[BOUNCESHOTBIG3].LoadImage("bounce_big3.png", 20, 20, 20, 20, 1, 1);
    ShotRect[BOUNCESHOTBIG3].left = 2;
    ShotRect[BOUNCESHOTBIG3].right  = 18;
    ShotRect[BOUNCESHOTBIG3].top  = 2;
    ShotRect[BOUNCESHOTBIG3].bottom = 18;

    // Lasershot
    ProjectileGrafix[LASERSHOT].LoadImage("laser.png", 9, 48, 9, 48, 1, 1);
    ShotRect[LASERSHOT].left = -4;
    ShotRect[LASERSHOT].right  = 15;
    ShotRect[LASERSHOT].top  = 18;
    ShotRect[LASERSHOT].bottom = 30;

    // Lasershot2
    ProjectileGrafix[LASERSHOT2].LoadImage("laser2.png", 160, 48, 16, 48, 10, 1);
    ShotRect[LASERSHOT2].left = 0;
    ShotRect[LASERSHOT2].right  = 20;
    ShotRect[LASERSHOT2].top  = 12;
    ShotRect[LASERSHOT2].bottom = 38;

    // Riesen Lasershot
    ProjectileGrafix[LASERSHOTBIG].LoadImage("laserbig.png", 16, 80, 16, 80, 1, 1);
    ShotRect[LASERSHOTBIG].left = 0;
    ShotRect[LASERSHOTBIG].right  = 30;
    ShotRect[LASERSHOTBIG].top  = 30;
    ShotRect[LASERSHOTBIG].bottom = 50;

    // Riesen Lasershot
    ProjectileGrafix[LASERSHOTBIG2].LoadImage("laserbig2.png", 280, 80, 28, 80, 10, 1);
    ShotRect[LASERSHOTBIG2].left = 0;
    ShotRect[LASERSHOTBIG2].right  = 30;
    ShotRect[LASERSHOTBIG2].top  = 30;
    ShotRect[LASERSHOTBIG2].bottom = 50;

    // Walker Laser
    ProjectileGrafix[WALKER_LASER].LoadImage("walker-laser.png", 32, 3, 32, 3, 1, 1);
    ShotRect[WALKER_LASER].left = 0;
    ShotRect[WALKER_LASER].right  = 32;
    ShotRect[WALKER_LASER].top  = 0;
    ShotRect[WALKER_LASER].bottom = 3;

    // Geschützturm Kugel
    ProjectileGrafix[CANONBALL].LoadImage("canonball.png", 8, 8, 8, 8, 1, 1);
    ShotRect[CANONBALL].left = 0;
    ShotRect[CANONBALL].right  = 8;
    ShotRect[CANONBALL].top  = 0;
    ShotRect[CANONBALL].bottom = 8;

    // Powerline
    ProjectileGrafix[POWERLINE].LoadImage("powerline.png", 80, 20, 40, 20, 2, 1);
    ShotRect[POWERLINE].left = 0;
    ShotRect[POWERLINE].right  = 20;
    ShotRect[POWERLINE].top  = 0;
    ShotRect[POWERLINE].bottom = 20;

    // Deckenturm Kugel
    ProjectileGrafix[SUCHSCHUSS].LoadImage("suchschuss.png", 8, 8, 8, 8, 1, 1);
    ShotRect[SUCHSCHUSS].left = 0;
    ShotRect[SUCHSCHUSS].right  = 8;
    ShotRect[SUCHSCHUSS].top  = 0;
    ShotRect[SUCHSCHUSS].bottom = 8;

    // Blauer Schuss der Riesenwepse
    ProjectileGrafix[SUCHSCHUSS2].LoadImage("suchschuss2.png", 64, 16, 16, 16, 4, 1);
    ShotRect[SUCHSCHUSS2].left = 1;
    ShotRect[SUCHSCHUSS2].right  = 15;
    ShotRect[SUCHSCHUSS2].top  = 1;
    ShotRect[SUCHSCHUSS2].bottom = 15;

    // Grüner Golemschuss
    ProjectileGrafix[GOLEMSCHUSS].LoadImage("golemschuss.png", 100, 100, 20, 20, 5, 5);
    ShotRect[GOLEMSCHUSS].left = 2;
    ShotRect[GOLEMSCHUSS].right  = 18;
    ShotRect[GOLEMSCHUSS].top  = 2;
    ShotRect[GOLEMSCHUSS].bottom = 18;

    // Grüner Skeletor Schuss
    ProjectileGrafix[TURRIEXTRAWURST].LoadImage("golemschuss.png", 100, 100, 20, 20, 5, 5);
    ShotRect[TURRIEXTRAWURST].left = 2;
    ShotRect[TURRIEXTRAWURST].right  = 18;
    ShotRect[TURRIEXTRAWURST].top  = 2;
    ShotRect[TURRIEXTRAWURST].bottom = 18;

    // Lilaner langsamer Spinnenschuss
    ProjectileGrafix[SPIDERSLOW].LoadImage("spiderslow.png", 200, 200, 40, 40, 5, 5);
    ShotRect[SPIDERSLOW].left = 5;
    ShotRect[SPIDERSLOW].right  = 35;
    ShotRect[SPIDERSLOW].top  = 5;
    ShotRect[SPIDERSLOW].bottom = 35;

    // Lilaner Spinnenlaser
    ProjectileGrafix[SPIDERLASER].LoadImage("fetterspinnenlaser.png", 64, 128, 64, 128, 1, 1);
    ShotRect[SPIDERLASER].left = 0;
    ShotRect[SPIDERLASER].right  = 500;
    ShotRect[SPIDERLASER].top  = 0;
    ShotRect[SPIDERLASER].bottom = 10;

    // Spinnenschuss
    ProjectileGrafix[SPIDERSHOT].LoadImage("spidershot.png", 96, 48, 96, 48, 1, 1);
    ShotRect[SPIDERSHOT].left = 24;
    ShotRect[SPIDERSHOT].right  = 72;
    ShotRect[SPIDERSHOT].top  = 12;
    ShotRect[SPIDERSHOT].bottom = 36;

    // Spinnenschuss2
    ProjectileGrafix[SPIDERSHOT2].LoadImage("spidershot2.png", 48, 48, 48, 48, 1, 1);
    ShotRect[SPIDERSHOT2].left = 16;
    ShotRect[SPIDERSHOT2].right  = 32;
    ShotRect[SPIDERSHOT2].top  = 16;
    ShotRect[SPIDERSHOT2].bottom = 32;

    // Bombe des Spieler-Rads
    ProjectileGrafix[BOMBE].LoadImage("bombe_small.png", 128, 32, 16, 16, 8, 2);
    ShotRect[BOMBE].left = 0;
    ShotRect[BOMBE].right  = 16;
    ShotRect[BOMBE].top  = 0;
    ShotRect[BOMBE].bottom = 12;

    // Bombe des Spieler-Rads
    ProjectileGrafix[BOMBEBIG].LoadImage("bombe_big.png", 192, 48, 24, 24, 8, 2);
    ShotRect[BOMBEBIG].left = 0;
    ShotRect[BOMBEBIG].right  = 24;
    ShotRect[BOMBEBIG].top  = 0;
    ShotRect[BOMBEBIG].bottom = 20;

    // Flammen-Splitter
    ProjectileGrafix[FLAMME].LoadImage("flamme.png", 192, 96, 48, 48, 4, 2);
    ShotRect[FLAMME].left = 16;
    ShotRect[FLAMME].right  = 32;
    ShotRect[FLAMME].top  = 16;
    ShotRect[FLAMME].bottom = 32;

    // Flamme des Wand-Flammenwerfers
    ProjectileGrafix[FEUERFALLE].LoadImage("flame.png", 504, 500, 84, 100, 6, 5);
    ShotRect[FEUERFALLE].left = 30;
    ShotRect[FEUERFALLE].right  = 54;
    ShotRect[FEUERFALLE].top  = 30;
    ShotRect[FEUERFALLE].bottom = 70;

    // Feuerball des Minidrachen
    ProjectileGrafix[FIREBALL].LoadImage("fireball.png", 24, 48, 24, 48, 1, 1);
    ShotRect[FIREBALL].left = 0;
    ShotRect[FIREBALL].right  = 24;
    ShotRect[FIREBALL].top  = 12;
    ShotRect[FIREBALL].bottom = 36;

    // Feuerball des Riesendrachen
    ProjectileGrafix[FIREBALL_BIG].LoadImage("fireball_big.png", 60, 54, 60, 54, 1, 1);
    ShotRect[FIREBALL_BIG].left = 8;
    ShotRect[FIREBALL_BIG].right  = 52;
    ShotRect[FIREBALL_BIG].top  = 8;
    ShotRect[FIREBALL_BIG].bottom = 46;

    // Feuerball des Riesendrachen
    ProjectileGrafix[FIREBALL_BOMB].LoadImage("fireball_big.png", 60, 54, 60, 54, 1, 1);
    ShotRect[FIREBALL_BOMB].left = 8;
    ShotRect[FIREBALL_BOMB].right  = 52;
    ShotRect[FIREBALL_BOMB].top  = 8;
    ShotRect[FIREBALL_BOMB].bottom = 46;

    // Rotzschuss des Rotzpotts
    ProjectileGrafix[ROTZSHOT].LoadImage("rotzshot.png", 80, 80, 16, 16, 5, 5);
    ShotRect[ROTZSHOT].left = 2;
    ShotRect[ROTZSHOT].right  = 14;
    ShotRect[ROTZSHOT].top  = 2;
    ShotRect[ROTZSHOT].bottom = 14;

    // Flamme des Wand-Flammenwerfers
    ProjectileGrafix[GOLEMSAEULE].LoadImage("flame.png", 504, 500, 84, 100, 6, 5);
    ShotRect[GOLEMSAEULE].left = 30;
    ShotRect[GOLEMSAEULE].right  = 54;
    ShotRect[GOLEMSAEULE].top  = 30;
    ShotRect[GOLEMSAEULE].bottom = 70;

    // Flamme des Lavamanns
    ProjectileGrafix[FEUERFALLE_LAVAMANN].LoadImage("flame.png", 504, 500, 84, 100, 6, 5);
    ShotRect[FEUERFALLE_LAVAMANN].left = 30;
    ShotRect[FEUERFALLE_LAVAMANN].right  = 54;
    ShotRect[FEUERFALLE_LAVAMANN].top  = 30;
    ShotRect[FEUERFALLE_LAVAMANN].bottom = 70;

    // Flamme der Feuer Spinne
    ProjectileGrafix[PLAYERFIRE].LoadImage("flame.png", 504, 500, 84, 100, 6, 5);
    ShotRect[PLAYERFIRE].left = 30;
    ShotRect[PLAYERFIRE].right  = 54;
    ShotRect[PLAYERFIRE].top  = 30;
    ShotRect[PLAYERFIRE].bottom = 70;

    // Flamme der Feuer Spinne
    ProjectileGrafix[SPIDERFIRE].LoadImage("flame.png", 504, 500, 84, 100, 6, 5);
    ShotRect[SPIDERFIRE].left = 30;
    ShotRect[SPIDERFIRE].right  = 54;
    ShotRect[SPIDERFIRE].top  = 30;
    ShotRect[SPIDERFIRE].bottom = 70;

    // Laser des Deckenkrabblers
    ProjectileGrafix[KRABBLERLASER1].LoadImage("krabblerlaser1.png", 4, 48, 4, 48, 1, 1);
    ShotRect[KRABBLERLASER1].left = 0;
    ShotRect[KRABBLERLASER1].right  = 4;
    ShotRect[KRABBLERLASER1].top  = 0;
    ShotRect[KRABBLERLASER1].bottom = 48;

    // Laser des Wandkrabblers links
    ProjectileGrafix[KRABBLERLASER2].LoadImage("krabblerlaser2.png", 48, 4, 48, 4, 1, 1);
    ShotRect[KRABBLERLASER2].left = 0;
    ShotRect[KRABBLERLASER2].right  = 48;
    ShotRect[KRABBLERLASER2].top  = 0;
    ShotRect[KRABBLERLASER2].bottom = 4;

    // Laser des Wandkrabblers rechts
    ProjectileGrafix[KRABBLERLASER3].LoadImage("krabblerlaser2.png", 48, 4, 48, 4, 1, 1);
    ShotRect[KRABBLERLASER3].left = 0;
    ShotRect[KRABBLERLASER3].right  = 48;
    ShotRect[KRABBLERLASER3].top  = 0;
    ShotRect[KRABBLERLASER3].bottom = 4;

    // Schuss des Pharao Kopfes
    ProjectileGrafix[PHARAOLASER].LoadImage("pharaolaser.png", 96, 96, 48, 48, 2, 2);
    ShotRect[PHARAOLASER].left = 0;
    ShotRect[PHARAOLASER].right  = 48;
    ShotRect[PHARAOLASER].top  = 0;
    ShotRect[PHARAOLASER].bottom = 48;

    // Granate
    ProjectileGrafix[GRENADE].LoadImage("grenade.png", 11, 14, 11, 14, 1, 1);
    ShotRect[GRENADE].left = 0;
    ShotRect[GRENADE].right  = 11;
    ShotRect[GRENADE].top  = 0;
    ShotRect[GRENADE].bottom = 14;

    // Rakete
    ProjectileGrafix[ROCKET].LoadImage("rocket.png", 225, 225, 75, 75, 3, 3);
    ShotRect[ROCKET].left = 20;
    ShotRect[ROCKET].right  = 55;
    ShotRect[ROCKET].top  = 20;
    ShotRect[ROCKET].bottom = 55;

    ProjectileGrafix[ROCKETSPIDER].LoadImage("rocket.png", 225, 225, 75, 75, 3, 3);
    ShotRect[ROCKETSPIDER].left = 20;
    ShotRect[ROCKETSPIDER].right  = 55;
    ShotRect[ROCKETSPIDER].top  = 20;
    ShotRect[ROCKETSPIDER].bottom = 55;

    // Rakete
    ProjectileGrafix[ROCKETWERFER].LoadImage("rocketwerfer.png", 45, 24, 45, 12, 1, 2);
    ShotRect[ROCKETWERFER].left = 0;
    ShotRect[ROCKETWERFER].right  = 45;
    ShotRect[ROCKETWERFER].top  = 0;
    ShotRect[ROCKETWERFER].bottom = 12;

    // Torpedo
    ProjectileGrafix[TORPEDO].LoadImage("torpedo.png", 33, 20, 33, 10, 1, 2);
    ShotRect[TORPEDO].left = 0;
    ShotRect[TORPEDO].right  = 33;
    ShotRect[TORPEDO].top  = 0;
    ShotRect[TORPEDO].bottom = 10;

    // EvilShot
    ProjectileGrafix[EVILSHOT].LoadImage("evilshot.png", 32, 16, 32, 16, 1, 1);
    ShotRect[EVILSHOT].left = 0;
    ShotRect[EVILSHOT].right  = 32;
    ShotRect[EVILSHOT].top  = 0;
    ShotRect[EVILSHOT].bottom = 16;

    // EvilShot2
    ProjectileGrafix[EVILSHOT2].LoadImage("evilshot2.png", 32, 16, 32, 16, 1, 1);
    ShotRect[EVILSHOT2].left = 0;
    ShotRect[EVILSHOT2].right  = 32;
    ShotRect[EVILSHOT2].top  = 0;
    ShotRect[EVILSHOT2].bottom = 16;

    // EvilBlitz
    ProjectileGrafix[EVILBLITZ].LoadImage("evilblitz.png", 140, 100, 70, 100, 2, 1);
    ShotRect[EVILBLITZ].left = 0;
    ShotRect[EVILBLITZ].right  = 70;
    ShotRect[EVILBLITZ].top  = 0;
    ShotRect[EVILBLITZ].bottom = 100;

    // BeamBlitz
    ProjectileGrafix[BLITZBEAM].LoadImage("blitzbeam.png", 320, 320, 160, 160, 2, 2);
    ShotRect[BLITZBEAM].left = 40;
    ShotRect[BLITZBEAM].right  = 120;
    ShotRect[BLITZBEAM].top  = 40;
    ShotRect[BLITZBEAM].bottom = 120;

    // Ufolaser
    ProjectileGrafix[UFOLASER].LoadImage("ufolaser.png", 340, 100, 34, 100, 10, 1);
    ShotRect[UFOLASER].left = 5;
    ShotRect[UFOLASER].right  = 29;
    ShotRect[UFOLASER].top  = 10;
    ShotRect[UFOLASER].bottom = 90;

    // Stelzlaser
    ProjectileGrafix[STELZLASER].LoadImage("stelzlaser.png", 120, 43, 60, 43, 2, 1);
    ShotRect[STELZLASER].left = 5;
    ShotRect[STELZLASER].right  = 55;
    ShotRect[STELZLASER].top  = 5;
    ShotRect[STELZLASER].bottom = 38;

    // Pflanzenschuss
    ProjectileGrafix[PFLANZESHOT].LoadImage("pflanzeschuss.png", 64, 16, 16, 16, 4, 1);
    ShotRect[PFLANZESHOT].left = 1;
    ShotRect[PFLANZESHOT].right  = 15;
    ShotRect[PFLANZESHOT].top  = 1;
    ShotRect[PFLANZESHOT].bottom = 15;

    // Bratklopsshot
    //DKS - There was only one frame in this image file, but four are specified here:
    //      I've changed it from 2,2 to 1,1. Also see my notes in Boss_Bratklops.cpp.
    //      I've also fixed the right and bottom coordinates of ShotRect:
    //ProjectileGrafix[BRATKLOPSSHOT].LoadImage("bratklopsshot.png", 60, 60, 60, 60, 2, 2);
    ProjectileGrafix[BRATKLOPSSHOT].LoadImage("bratklopsshot.png", 60, 60, 60, 60, 1, 1);
    ShotRect[BRATKLOPSSHOT].left = 10;
    ShotRect[BRATKLOPSSHOT].right  = 50;
    ShotRect[BRATKLOPSSHOT].top  = 10;
    ShotRect[BRATKLOPSSHOT].bottom = 50;

    // Dronebullet
    ProjectileGrafix[DRONEBULLET].LoadImage("dronebullet.png", 48, 12, 12, 12, 4, 1);
    ShotRect[DRONEBULLET].left = 2;
    ShotRect[DRONEBULLET].right  = 10;
    ShotRect[DRONEBULLET].top  = 2;
    ShotRect[DRONEBULLET].bottom = 10;

    // SchleimShot
    ProjectileGrafix[SCHLEIMSHOT].LoadImage("schleimshot.png", 12, 12, 12, 12, 1, 1);
    ShotRect[SCHLEIMSHOT].left = 2;
    ShotRect[SCHLEIMSHOT].right  = 10;
    ShotRect[SCHLEIMSHOT].top  = 2;
    ShotRect[SCHLEIMSHOT].bottom = 10;

    // LaFass
    //DKS - Corrected dimensions from 264x334 to 264x332, to match actual image file:
    ProjectileGrafix[LAFASSSHOT].LoadImage("lafass_fall.png", 264, 332, 66, 83, 4, 4);
    ShotRect[LAFASSSHOT].left = 10;
    ShotRect[LAFASSSHOT].right  = 56;
    ShotRect[LAFASSSHOT].top  = 10;
    ShotRect[LAFASSSHOT].bottom = 73;

    // EvilBlitz2
    ProjectileGrafix[EVILBLITZ2].LoadImage("evilblitz2.png", 140, 400, 70, 400, 2, 1);
    ShotRect[EVILBLITZ2].left = 0;
    ShotRect[EVILBLITZ2].right  = 70;
    ShotRect[EVILBLITZ2].top  = 0;
    ShotRect[EVILBLITZ2].bottom = 400;

    // FlugLaser
    ProjectileGrafix[FLUGLASER].LoadImage("flugkanonelaser.png", 8, 32, 8, 32, 1, 1);
    ShotRect[FLUGLASER].left = 0;
    ShotRect[FLUGLASER].right  = 8;
    ShotRect[FLUGLASER].top  = 0;
    ShotRect[FLUGLASER].bottom = 32;

    // Eiszapfengeschoss
    ProjectileGrafix[EISZAPFENSHOT].LoadImage("eiszapfenshot.png", 12, 48, 12, 48, 1, 1);
    ShotRect[EISZAPFENSHOT].left = 2;
    ShotRect[EISZAPFENSHOT].right  = 10;
    ShotRect[EISZAPFENSHOT].top  = 0;
    ShotRect[EISZAPFENSHOT].bottom = 48;

    // Schuss der im Bogen fliegt
    ProjectileGrafix[ARCSHOT].LoadImage("arcshot.png", 8, 8, 8, 8, 1, 1);
    ShotRect[ARCSHOT].left = 0;
    ShotRect[ARCSHOT].right  = 8;
    ShotRect[ARCSHOT].top  = 0;
    ShotRect[ARCSHOT].bottom = 8;

    // SchneeBombe
    ProjectileGrafix[SNOWBOMB].LoadImage("snowbomb.png", 480, 320, 80, 80, 6, 4);
    ShotRect[SNOWBOMB].left = 10;
    ShotRect[SNOWBOMB].right  = 70;
    ShotRect[SNOWBOMB].top  = 10;
    ShotRect[SNOWBOMB].bottom = 70;

    // SchneeBombe klein
    ProjectileGrafix[SNOWBOMBSMALL].LoadImage("snowbombsmall.png", 120, 80, 20, 20, 6, 4);
    ShotRect[SNOWBOMBSMALL].left = 5;
    ShotRect[SNOWBOMBSMALL].right  = 15;
    ShotRect[SNOWBOMBSMALL].top  = 5;
    ShotRect[SNOWBOMBSMALL].bottom = 15;

    // Schuss der fetten Spinne
    ProjectileGrafix[FETTESPINNESHOT].LoadImage("fettespinneshot.png", 160, 50, 40, 50, 4, 1);
    ShotRect[FETTESPINNESHOT].left = 5;
    ShotRect[FETTESPINNESHOT].right  = 35;
    ShotRect[FETTESPINNESHOT].top  = 5;
    ShotRect[FETTESPINNESHOT].bottom = 45;

    // Schuss der fetten Spinne
    ProjectileGrafix[FETTESPINNESHOT2].LoadImage("fettespinneshot2.png", 160, 50, 40, 50, 4, 1);
    ShotRect[FETTESPINNESHOT2].left = 5;
    ShotRect[FETTESPINNESHOT2].right  = 35;
    ShotRect[FETTESPINNESHOT2].top  = 5;
    ShotRect[FETTESPINNESHOT2].bottom = 45;

    // Eierbombe des Eiermanns
    ProjectileGrafix[EIERBOMBE].LoadImage("eierbombe.png", 125, 100, 25, 25, 5, 4);
    ShotRect[EIERBOMBE].left = 5;
    ShotRect[EIERBOMBE].right  = 20;
    ShotRect[EIERBOMBE].top  = 5;
    ShotRect[EIERBOMBE].bottom = 20;

    // Rundumschuss des evil hurri
    ProjectileGrafix[EVILROUND1].LoadImage("evilround.png", 10, 10, 10, 10, 1, 1);
    ShotRect[EVILROUND1].left = 0;
    ShotRect[EVILROUND1].right  = 10;
    ShotRect[EVILROUND1].top  = 0;
    ShotRect[EVILROUND1].bottom = 10;

    // SmartBomb laden
    ProjectileGrafix[SMARTBOMB].LoadImage("druckwelle.png", 256, 256, 256, 256, 1, 1);
    ShotRect[SMARTBOMB].left = 0;
    ShotRect[SMARTBOMB].right  = 256;
    ShotRect[SMARTBOMB].top  = 0;
    ShotRect[SMARTBOMB].bottom = 256;

    // Schutzschild Generator laden
    ProjectileGrafix[SHIELDSPAWNER].LoadImage("shieldspawner.png", 120, 12, 12, 12, 10, 1);
    ShotRect[SHIELDSPAWNER].left = 0;
    ShotRect[SHIELDSPAWNER].right  = 12;
    ShotRect[SHIELDSPAWNER].top  = 0;
    ShotRect[SHIELDSPAWNER].bottom = 12;

    // Blaue Bombe laden
    ProjectileGrafix[BLUEBOMB].LoadImage("blauebombe.png", 150, 120, 30, 30, 5, 4);
    ShotRect[BLUEBOMB].left = 4;
    ShotRect[BLUEBOMB].right  = 26;
    ShotRect[BLUEBOMB].top  = 4;
    ShotRect[BLUEBOMB].bottom = 26;

    // Blaue Bombe laden
    ProjectileGrafix[SKELETORGRANATE].LoadImage("skeletor_granate.png", 100, 60, 20, 30, 5, 2);
    ShotRect[SKELETORGRANATE].left = 4;
    ShotRect[SKELETORGRANATE].right  = 16;
    ShotRect[SKELETORGRANATE].top  = 4;
    ShotRect[SKELETORGRANATE].bottom = 15;

    // Blaue Bombe laden
    ProjectileGrafix[ELEKTROPAMPE].LoadImage("elektropampe.png", 640, 640, 640, 128, 1, 5);
    ShotRect[ELEKTROPAMPE].left = 0;
    ShotRect[ELEKTROPAMPE].right  = 640;
    ShotRect[ELEKTROPAMPE].top  = 0;
    ShotRect[ELEKTROPAMPE].bottom = 128;

    // Blaue Bombe laden
    ProjectileGrafix[ELEKTROSCHUSS].LoadImage("spidershot2.png", 48, 48, 48, 48, 1, 1);
    ShotRect[ELEKTROSCHUSS].left = 16;
    ShotRect[ELEKTROSCHUSS].right  = 32;
    ShotRect[ELEKTROSCHUSS].top  = 16;
    ShotRect[ELEKTROSCHUSS].bottom = 32;
}

// --------------------------------------------------------------------------------------
// Destruktor : Löschen der ganzen Liste und Freigabe der Projektil-Grafiken
// --------------------------------------------------------------------------------------

ProjectileListClass::~ProjectileListClass(void)
{
    // Schuss-Liste komplett leeren
    ClearAll();
}

// --------------------------------------------------------------------------------------
// Schuss "Art" hinzufügen
// --------------------------------------------------------------------------------------

//DKS - Modified to support new pooled memory manager as well as the fact that
//      projectile list is now singly-linked
#if 0
bool ProjectileListClass::PushProjectile(float x, float y, int Art, PlayerClass* pTemp)
{
    if(NumProjectiles >= MAX_SHOTS)					// Grenze überschritten ?
        return false;

    ProjectileClass *pNew = new ProjectileClass;	// Neues zu erstellendes Projectile

    pNew->CreateShot(x, y, Art, pTemp);					// neues Projektil erzeugen

    if(pStart==NULL)						// Liste leer ?
    {
        pStart = pNew;						// Ja, dann neues projektil gleich das erste
        pEnd   = pNew;						// und letzte Projektil

        pStart->pNext=NULL;					// Next/Previous gibts nich, da wir
        pStart->pPrev=NULL;					// nur 1 Projektil haben
    }
    else									// Liste ist NICHT leer
    {
        pEnd->pNext = pNew;					// Letztes Projektil zeigt auf das neue
        pNew->pPrev = pEnd;					// Letztes Projektil ist nicht mehr das letzte

        pNew->pNext = NULL;					// Nach dem neuen Projektil kommt keines mehr
        pEnd		= pNew;					// da es jetzt das letzte in der Liste ist
    }

    NumProjectiles++;							// Projektilanzahl erhöhen
    return true;
}
#endif //0
bool ProjectileListClass::PushProjectile(float x, float y, int Art, PlayerClass* pTemp)
{
    if(NumProjectiles >= MAX_SHOTS)
        return false;

    //DKS - added support for new, fast pooled mem-manager:
#if USE_MEMPOOL
    ProjectileClass *pNew = projectile_pool.alloc();
#else
    ProjectileClass *pNew = new ProjectileClass;
#endif

    pNew->CreateShot(x, y, Art, pTemp);
    pNew->pNext = NULL;

    if (pEnd)
        pEnd->pNext = pNew;     // If list isn't empty, insert this projectile on the end.
    else
        pStart = pNew;          // Or, if list is empty, make this the new head of the list.

    pEnd = pNew;                // Update end-of-list pointer

    NumProjectiles++;
    return true;
}

// --------------------------------------------------------------------------------------
// BlitzBeam hinzufügen
// --------------------------------------------------------------------------------------

bool ProjectileListClass::PushBlitzBeam (int Size, float Richtung, PlayerClass* pSource)
{
    if(NumProjectiles >= MAX_SHOTS)					// Grenze überschritten ?
        return false;

    //DKS - added support for new, fast pooled mem-manager
#if USE_MEMPOOL
    ProjectileClass *pNew = projectile_pool.alloc();
#else
    ProjectileClass *pNew = new ProjectileClass;	// Neues zu erstellendes Projectile
#endif

    pNew->ShotArt = BLITZBEAM;
    pNew->xPos = pSource->xpos - Size / 2 + 20;
    pNew->yPos = pSource->ypos - Size / 2 + 32;
    pNew->xPosOld	   = pNew->xPos;
    pNew->yPosOld	   = pNew->yPos;
    pNew->AnimPhase    = 0;
    pNew->AnimCount    = 0.0f;
    //DKS - off-by-one error:
    //pNew->AnimEnde     = 4;
    pNew->AnimEnde     = 3;
    pNew->AnimSpeed    = 0.5f;
    pNew->Damage	   = Size;
    pNew->DamagePlayer = false;
    pNew->BounceWalls  = false;
    pNew->pParent	   = pSource;
    pNew->ExplodeOnImpact = false;
    pNew->OwnDraw	   = true;

    Richtung += 180;
    if (Richtung > 360.0f)
        Richtung -= 360.0f;

    //DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
    // In Rad umwandeln
    //Richtung = PI * Richtung / 180.0f;
    //pNew->xSpeed	= float(-40.0f * sin(Richtung));
    //pNew->ySpeed	= float( 40.0f * cos(Richtung));
	pNew->xSpeed	= -40.0f * sin_deg(Richtung);
	pNew->ySpeed	=  40.0f * cos_deg(Richtung);

    //DKS - Since our ProjectileClass constructor is now empty, and the BLITZBEAM projectile
    //      does not use CreateShot(), we must insert the missing four pieces of the old
    //      constructor here:
    pNew->xAcc	= 0.0f;
    pNew->yAcc	= 0.0f;
    pNew->Counter = 0.0f;
    pNew->Winkel  = -10000.0f;					// wird von den wenigsten Schüssen verwendet

    //DKS - Dividing an int by a float and then casting to an int again makes no sense at all:
    //ShotRect[BLITZBEAM].left   = int(Size / 4.0f),
    //                    ShotRect[BLITZBEAM].top    = int(Size / 4.0f),
    //                                        ShotRect[BLITZBEAM].right  = ShotRect[BLITZBEAM].left + int(Size / 2.0f);
    //ShotRect[BLITZBEAM].bottom = ShotRect[BLITZBEAM].top  + int(Size / 2.0f);
	ShotRect[BLITZBEAM].left   = Size / 4;
	ShotRect[BLITZBEAM].top    = Size / 4;
	ShotRect[BLITZBEAM].right  = ShotRect[BLITZBEAM].left + Size / 2;
	ShotRect[BLITZBEAM].bottom = ShotRect[BLITZBEAM].top  + Size / 2;

    //DKS - Projectile list is now singly-linked:
#if 0
    if(pStart==NULL)						// Liste leer ?
    {
        pStart = pNew;						// Ja, dann neues projektil gleich das erste
        pEnd   = pNew;						// und letzte Projektil

        pStart->pNext=NULL;					// Next/Previous gibts nich, da wir
        pStart->pPrev=NULL;					// nur 1 Projektil haben
    }
    else									// Liste ist NICHT leer
    {
        pEnd->pNext = pNew;					// Letztes Projektil zeigt auf das neue
        pNew->pPrev = pEnd;					// Letztes Projektil ist nicht mehr das letzte

        pNew->pNext = NULL;					// Nach dem neuen Projektil kommt keines mehr
        pEnd		= pNew;					// da es jetzt das letzte in der Liste ist
    }
#endif //0
    //DKS - New version of above supporting singly-linked list:
    pNew->pNext = NULL;

    if (pEnd)
        pEnd->pNext = pNew;     // If list isn't empty, insert this projectile on the end.
    else
        pStart = pNew;          // Or, if list is empty, make this the new head of the list.

    pEnd = pNew;                // Update end-of-list pointer


    NumProjectiles++;							// Projektilanzahl erhöhen
    return true;
}

// --------------------------------------------------------------------------------------
// Bestimmtes Projektil der Liste löschen
// --------------------------------------------------------------------------------------

//DKS - Replaced with new DelNode() function that supports a singly-linked list and
//      new pooled memory manager. (see next function below this )
#if 0
void ProjectileListClass::DelSel(ProjectileClass *pTemp)
{
    ProjectileClass  *pN;
    ProjectileClass  *pP;

    if(pTemp!=NULL)						// zu löschendes Projectile existiert
    {
        pN = pTemp->pNext;
        pP = pTemp->pPrev;

        if(pP == NULL)					// Wird das erste Projectile gelöscht ?
            pStart = pN;				// Dann wird dessen Nächstes zum Ersten
        else
            pP->pNext = pN;	   	        // ansonsten normal eins aufrücken

        if(pN == NULL)					// Wird das letzte Projectile gelöscht ?
            pEnd = pP;					// Dann wir das letzte Projectile zum ersten
        else
            pN->pPrev = pP;

        delete (pTemp);					// Speicher freigeben
        pTemp = NULL;

        NumProjectiles--;				// Projectilezahl verringern
    }
}
#endif //0

//DKS - Replaced DelSel() with DelNode(), which supports the now-singly-linked-list. It operates
//      a bit differently:
//      It is now up to the caller to splice the list, this blindly deletes what is passed to it
//      and returns the pointer that was in pPtr->pNext, or NULL if pPtr was NULL.
ProjectileClass* ProjectileListClass::DelNode(ProjectileClass *pPtr)
{
    ProjectileClass  *pNext = NULL;
    if (pPtr!=NULL)	
    {
        pNext = pPtr->pNext;

        if (pStart == pPtr)             // Are we deleting the first node in the list?
            pStart = pNext;

        //DKS - added support for new, fast pooled mem-manager:
#if USE_MEMPOOL
        projectile_pool.free(pPtr);
#else
        delete (pPtr);
#endif

        NumProjectiles--;
    }
    return pNext;
}

// --------------------------------------------------------------------------------------
// Alle Projectile der Liste löschen
// --------------------------------------------------------------------------------------
//DKS - Converted ProjectileListClass to a singly-linked list (depends on DelNode() now).
//      and added support for new pooled memory manager.
#if 0
void ProjectileListClass::ClearAll(void)
{
    ProjectileClass *pTemp    = pStart;				// Zeiger auf das erste    Proectile
    ProjectileClass *pNaechst;						// Zeiger auf das nächste  Proectile (falls
    // das eine gelöscht wird)
    while (pTemp != NULL)							// Ende der Liste erreicht ?
    {
        pNaechst = pTemp->pNext;					// Zeiger auf das nächste Element
        DelSel(pTemp);								// Das aktuelle löschen
        pTemp = pNaechst;							// und das nächste bearbeiten
    }

    pStart = NULL;
    pEnd   = NULL;
}
#endif //0
void ProjectileListClass::ClearAll(void)
{
    if (pStart) {
        ProjectileClass *pNext = pStart->pNext;

        while (pNext)           // Delete everything but the head of the list
            pNext = DelNode(pNext); 

        DelNode(pStart);        // Finally, delete the head of the list
    }
    pStart = pEnd = NULL;

#ifdef _DEBUG
    if (NumProjectiles != 0) 
        Protokoll.WriteText( false, "ERROR: poss. mem leak / corruption in linked list of projectiles\n" );
#endif

    //DKS - added support for new, fast pooled mem-manager:
#ifdef USE_MEMPOOL
    projectile_pool.reinit();
#endif

    // Just to be safe:
    NumProjectiles = 0;
}


// --------------------------------------------------------------------------------------
// Alle Projectiles eines Typs löschen
// --------------------------------------------------------------------------------------

//DKS - Adapted after converting ProjectileListClass to a singly-linked list:
#if 0
void ProjectileListClass::ClearType(int Type)
{
    ProjectileClass *pTemp    = pStart;				// Zeiger auf das erste    Proectile
    ProjectileClass *pNaechst;						// Zeiger auf das nächste  Proectile (falls
    // das eine gelöscht wird)
    while (pTemp != NULL)							// Ende der Liste erreicht ?
    {
        pNaechst = pTemp->pNext;					// Zeiger auf das nächste Element

        if (pTemp->ShotArt == Type)
            DelSel(pTemp);								// Das aktuelle löschen

        pTemp = pNaechst;							// und das nächste bearbeiten
    }
}
#endif //0
void ProjectileListClass::ClearType(int Type)
{
    ProjectileClass *pPrev = NULL;
    ProjectileClass *pCurr = pStart;

    while (pCurr != NULL) 
    {
        if (pCurr->ShotArt == Type) {
            // If this is the last node in the list, update the main class's pEnd pointer
            if (pEnd == pCurr)
                pEnd = pPrev;

            pCurr = DelNode(pCurr); //pCurr now points to the node after the one deleted

            if (pPrev) {
                // This is not the first node in the list, so splice this node onto the previous one
                pPrev->pNext = pCurr;
            }
        } else {
            pPrev = pCurr;
            pCurr = pCurr->pNext;
        }
    }
}

// --------------------------------------------------------------------------------------
// Zahl der Projectile zurückliefern
// --------------------------------------------------------------------------------------

int ProjectileListClass::GetNumProjectiles(void)
{
    return NumProjectiles;
}

// --------------------------------------------------------------------------------------
// Alle Proectile der Liste animieren und bewegen
// --------------------------------------------------------------------------------------

//DKS - Adapted after converting projectile list to singly-linked one
#if 0
void ProjectileListClass::DoProjectiles(void)
{
    ProjectileClass *pTemp = pStart;		// Anfang der Liste
    ProjectileClass *pNext = NULL;			// Nächstes projectile in der Liste

    CurrentShotTexture = -1;				// Aktuelle Textur gibt es noch keine

    while (pTemp != NULL)					// Noch nicht alle durch ?
    {
        if (Console.Showing == false)
            pTemp->Run();

        if (pTemp->Damage > 0)
            pTemp->Render();

        pNext = pTemp->pNext;				// Nächsten sichern

        if (Console.Showing == false)
        {
            if (pTemp->Damage <= 0)			 	// ggf Schuss löschen (bei Damage <= 0)
                DelSel(pTemp);
            else
                pTemp->CheckCollision();
        }

        pTemp = pNext;						// Und Schuss Partikel anhandeln
    }
}
#endif //0
void ProjectileListClass::DoProjectiles(void)
{
    ProjectileClass *pPrev = NULL;
    ProjectileClass *pCurr = pStart;

    CurrentShotTexture = -1;				// Aktuelle Textur gibt es noch keine

    while (pCurr != NULL)
    {
        if (Console.Showing == false)
            pCurr->Run();

        if (pCurr->Damage > 0)
            pCurr->Render();

        if (Console.Showing == false)
        {
            if (pCurr->Damage <= 0)			 	// ggf Schuss löschen (bei Damage <= 0)
            {
                // Projectile's time to die..
                // If this is the last node in the list, update the class's pEnd pointer
                if (pEnd == pCurr)
                    pEnd = pPrev;

                pCurr = DelNode(pCurr); //pCurr now points to the node after the one deleted

                // If this is not the first node in the list, splice this node onto the previous one:
                if (pPrev)
                    pPrev->pNext = pCurr;
            } else
            {
                pCurr->CheckCollision();
                pPrev = pCurr;
                pCurr = pCurr->pNext;
            }
        } else {
            pPrev = pCurr;
            pCurr = pCurr->pNext;
        }
    }
}
