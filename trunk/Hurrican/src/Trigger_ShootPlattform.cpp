// --------------------------------------------------------------------------------------
// Die Shootplattform
//
// ich staune immer wieder über meine geile Namensgebung...
//
// Eine Plattform mit einem Button oben drauf
// schiesst man mit einem Blitz auf den Button (seperater Trigger) so fliegt die Plattform ein Stück
// nach oben
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_ShootPlattform.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerShootPlattform::GegnerShootPlattform(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	BlickRichtung	= RECHTS;
	Energy			= 100;
	Value1			= Wert2;			// yPos der Plattform
	Value2			= 0;
	ChangeLight		= Light;
	Destroyable		= false;
	OwnDraw			= true;
	TestBlock		= false;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerShootPlattform::DoDraw(void)
{
	// Button rendern
	pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset), 
										   (float)(yPos-pTileEngine->YOffset), 
										   AnimPhase, 0xFFFFFFFF);		

	if (Value2 == 1)
	{
		// Lighflare rendern, wenn angeschossen
		DirectGraphics.SetAdditiveMode ();
		LavaFlare.RenderSprite (float (xPos - 9 - pTileEngine->XOffset), 
								float (yPos - 5 - pTileEngine->YOffset), 0, 0xAAFFFFFF);

		LavaFlare.RenderSprite (float (xPos - 9 - pTileEngine->XOffset), 
								float (yPos - 5 - pTileEngine->YOffset), 0, 0x88AAAA00);
		LavaFlare.RenderSprite (float (xPos - 9 - pTileEngine->XOffset), 
								float (yPos - 5 - pTileEngine->YOffset), 0, 0x88AA0000);
		DirectGraphics.SetColorKeyMode ();

		Value2 = 0;
	}
}

// --------------------------------------------------------------------------------------
// "ShootPlattform KI"
// --------------------------------------------------------------------------------------

void GegnerShootPlattform::DoKI(void)
{
	BlickRichtung	= LINKS;	

	// Testen, ob der Spieler auf der Plattform steht
	//
	PlattformTest(GegnerRect[GegnerArt]);

	// MaxSpeed
	//
	if (ySpeed < -30.0f) ySpeed = -30.0f;
	if (ySpeed >  30.0f) ySpeed =  30.0f;

	// Wieder am Ausgangspunkt gelandet ?
	//
	if (ySpeed > 0.0f &&
		int (yPos) > Value1)
	{
		yPos = float (Value1);
		ySpeed = 0.0f;
		yAcc   = 0.0f;
	}

	// An der Decke abprallen
	for (int p = 0; p < NUMPLAYERS; p++)
	if (pPlayer[p]->AufPlattform == this)
	{
		int bo = pTileEngine->BlockOben(pPlayer[p]->xpos, pPlayer[p]->ypos, 
										pPlayer[p]->xposold,  pPlayer[p]->yposold, pPlayer[p]->CollideRect);

		if (ySpeed < 0.0f &&
		    bo & BLOCKWERT_WAND)
			ySpeed *= -1.0f;
	}
	
}

// --------------------------------------------------------------------------------------
// ShootPlattform explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerShootPlattform::GegnerExplode(void)
{
}
