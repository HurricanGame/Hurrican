// --------------------------------------------------------------------------------------
// Der Button für die Shootplattform
//
// Ein Button, der bei Beschuss eine ShootPlattform nach oben befördert
// je länger man schiesst, um so mehr beschleunigt die Plattform
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_ShootButton.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerShootButton::GegnerShootButton  (GegnerClass *Plattform)
{
	Handlung		= GEGNER_STEHEN;
	BlickRichtung	= RECHTS;
	Energy			= 100;
	Value1			= 0;
	Value2			= 0;
	ChangeLight		= false;
	Destroyable		= true;
	AnimEnde		= 2;
	AnimSpeed       = 5.0f;
	pPlattForm		= Plattform;
	SmokeCount		= 0.1f;
	OwnDraw			= true;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerShootButton::DoDraw(void)
{
	// Button rendern
	pGegnerGrafix[GegnerArt]->RenderSprite((float)(pPlattForm->xPos-pTileEngine->XOffset + 41), 
										   (float)(pPlattForm->yPos-pTileEngine->YOffset - 8), 
										   AnimPhase, 0xFFFFFFFF);		
}

// --------------------------------------------------------------------------------------
// ShootButton KI
// --------------------------------------------------------------------------------------

void GegnerShootButton::DoKI(void)
{
	SimpleAnimation();

	yPos = pPlattForm->yPos - 9 + pPlattForm->ySpeed SYNC;

	// beschossen ?
	//
	if (Energy < 100.0f)
	{
		Energy = 100.0f;
		pPlattForm->yAcc = -3.0f;

		pPlattForm->Value2 = 1;		

		// Flamme ausgeben
		//
		SmokeCount -= 1.0f SYNC;
		if (SmokeCount <= 0.0f)
		{
			SmokeCount = 0.2f;
			pProjectiles->PushProjectile  (pPlattForm->xPos + 23 + rand()%4, pPlattForm->yPos - 15, FEUERFALLE3); 
			pPartikelSystem->PushPartikel (pPlattForm->xPos + 33 + rand()%2, pPlattForm->yPos + 40, FLUGSACKSMOKE); 
			pPartikelSystem->PushPartikel (pPlattForm->xPos + 51 + rand()%2, pPlattForm->yPos + 40, FLUGSACKSMOKE2); 
			pPartikelSystem->PushPartikel (pPlattForm->xPos + 48 + rand()%2, pPlattForm->yPos + 40, FUNKE); 
		}		
	}
	else
		if (int (pPlattForm->yPos) < pPlattForm->Value1)
			pPlattForm->yAcc = 3.0f;
}

// --------------------------------------------------------------------------------------
// ShootButton explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerShootButton::GegnerExplode(void)
{
}
