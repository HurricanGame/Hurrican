// --------------------------------------------------------------------------------------
// Die große schwebende Plattform, die sich dreht, wenn man draufhopst
//
// Geht ein Stück nach unten, wenn der Hurri draufhopst
// Je schneller die Fallgeschwindigkeit beim aufkommen, desto weiter geht sie nach unten
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_Floating3.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFloating3::GegnerFloating3(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	BlickRichtung	= RECHTS;
	Energy			= 100;
	Value1			= Wert1;
	Value2			= Wert2;
	if (Value1 == 0)
		Value1 = 16;
	ChangeLight		= Light;
	Destroyable		= false;
	AnimCount	    = 0.0f;
	playeron		= false;
	winkel			= 0.0f;
	turncount		= 0.0f;
	drehdir			= 0.0f;

	OwnDraw	= true;
}


// --------------------------------------------------------------------------------------
// Eigene Drawfunktion
// --------------------------------------------------------------------------------------

void GegnerFloating3::DoDraw(void)
{
	// Standfläche rendern
	//	
	pGegnerGrafix[GegnerArt]->itsRect = pGegnerGrafix[GegnerArt]->itsPreCalcedRects[0];
	pGegnerGrafix[GegnerArt]->RenderSpriteRotatedOffset((float)(xPos-pTileEngine->XOffset), 
													    (float)(yPos-pTileEngine->YOffset), winkel,
														0.0f, -6.0f, 0xFFFFFFFF);	

	// Mittelteil rendern
	//
	pGegnerGrafix[GegnerArt]->itsRect = pGegnerGrafix[GegnerArt]->itsPreCalcedRects[1];
	pGegnerGrafix[GegnerArt]->RenderSprite ((float)(xPos-pTileEngine->XOffset), 
										    (float)(yPos-pTileEngine->YOffset),
										    1, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// Floating3 KI
// --------------------------------------------------------------------------------------

void GegnerFloating3::DoKI(void)
{
	playeron  = false;
	
	for (int p = 0; p < NUMPLAYERS; p++)
		if (pPlayer[p]->AufPlattform == this)
			playeron = true;

	switch (Handlung)
	{
		case GEGNER_STEHEN:
		{
			PlattformTest(GegnerRect[GegnerArt]);

			// Spieler in diesem Frame auf die Plattform gehopst ?
			//
			for (int p = 0; p < NUMPLAYERS; p++)
				if (pPlayer[p]->AufPlattform == this && playeron == false)
			{
				playeron = true;
				turncount = 0.01f;

				// Drehrichtung rausfinden
				if (pPlayer[p]->xpos + 35 < xPos + 50)
					drehdir = 1.0f;
				else
					drehdir = -1.0f;

				// Plattform dopst kurz runter
				//
				if (ySpeed == 0.0f)
				{
					Value2 = int (yPos);
					ySpeed = float (pPlayer[p]->yspeed) / 5;
					yAcc   = -3.0f;
				}
			}			

			// Spieler war drauf? Dann Counter hochzählen, dass die Plattform sich bald dreht
			if (turncount > 0.0f) 
				turncount += 1.0f SYNC;

			// drehen? -> Spieler fällt runter
			if (turncount > 8.0f)
			{
				// Noch drauf? Dann nochmal Drehrichtung rausfinden
				for (int p = 0; p < NUMPLAYERS; p++)
				{
					if (pPlayer[p]->AufPlattform == this)
					if (pPlayer[p]->xpos + 35 < xPos + 50)
						drehdir = 1.0f;
					else
						drehdir = -1.0f;

					pPlayer[p]->AufPlattform = NULL;

					turncount = 0.0f;
					Handlung = GEGNER_DREHEN;				
				}				
			}
		} break;		

		// plattform dreht sich
		case GEGNER_DREHEN:
		{
			winkel += 25.0f * drehdir SYNC;

			if (winkel >  180.0f ||
				winkel < -180.0f)
			{
				winkel = 0.0f;
				Handlung = GEGNER_STEHEN;
			}
		} break;
	}

	// Plattform wieder an Ausgangspos ?
	//
	if (ySpeed < 0.0f && int (yPos) <= Value2)
	{
		ySpeed = 0.0f;
		yAcc   = 0.0f;
		yPos   = float (Value2);
	}
	
	// Counter für Partikel runterzählen
	//
	AnimCount -= 1.0f SYNC;

	// Partikel erzeugen und counter wieder hochsetzen
	//
	if (AnimCount <= 0.0f)
	{
		AnimCount = 0.15f;
		pPartikelSystem->PushPartikel (xPos + 44, yPos + 21, FLOATSMOKE);
		pPartikelSystem->PushPartikel (xPos + 27, yPos + 10, FLUGSACKSMOKE);
		pPartikelSystem->PushPartikel (xPos + 57, yPos + 10, FLUGSACKSMOKE2);
	}

}

// --------------------------------------------------------------------------------------
// Floating3 explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerFloating3::GegnerExplode(void)
{
}
