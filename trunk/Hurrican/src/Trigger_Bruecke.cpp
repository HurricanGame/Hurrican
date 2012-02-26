// --------------------------------------------------------------------------------------
// Die Hängebrücke
//
// Ein Teil der Brücke
// Hängt je nach Abstand zum Spieler oder einem Objekt unterschiedlich weit runter
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_Bruecke.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerBruecke::GegnerBruecke(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	BlickRichtung	= RECHTS;
	Energy			= 100;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= false;
	AnimCount	    = 0.0f;
	TestBlock		= false;
}

// --------------------------------------------------------------------------------------
// "Bruecke KI"
// --------------------------------------------------------------------------------------

void GegnerBruecke::DoKI(void)
{
	// Brücke nicht im Bild ?
	//
	if (xPos + 10 < pTileEngine->XOffset		||
		xPos	  > pTileEngine->XOffset + 640	||
		yPos + 10 < pTileEngine->YOffset		||
		yPos	  > pTileEngine->YOffset + 480)
	{
		return;
	}

	// Ausgangsposition
	//

	yPos = float (Value1);

	if (options_Detail == DETAIL_MAXIMUM)
	{

		// Irgendein Gegner steht auf der Brücke
		//
		GegnerClass *pTemp = pGegner->pStart;			// Anfang der Liste

		while (pTemp != NULL)							// noch nicht alle durch ?
		{
			if (pTemp->Active == true &&
				pTemp->GegnerArt != BRUECKE &&
				pTemp->GegnerArt != BRUECKE &&
				pTemp->GegnerArt != STAHLMUECKE &&
				pTemp->ySpeed == 0.0f	&&
				SpriteCollision (xPos, yPos, GegnerRect[GegnerArt],
								 pTemp->xPos, pTemp->yPos, GegnerRect[pTemp->GegnerArt]) == true)
			{			
				float dx, dy;
				float   w, h;

				w = (GegnerRect[pTemp->GegnerArt].right  - GegnerRect[pTemp->GegnerArt].left) / 2.0f;
				h = (GegnerRect[pTemp->GegnerArt].bottom - GegnerRect[pTemp->GegnerArt].top)  / 2.0f;

				if (pTemp->GegnerArt == DIAMANT)
					h = 2;

				dx = pTemp->xPos + w - (xPos + 5);
				dy = pTemp->yPos + h - (yPos + 5);
		
				double a = sqrt ((dx * dx) + (dy * dy));

				if (a > 40.0f)
					a = 40.0f;

				if (pTemp->GegnerArt == SPITTER)
					a += 13.0f;

				yPos = float (pTemp->yPos + GegnerRect[pTemp->GegnerArt].bottom - a / 2.5f + 13.0f);
			}

			pTemp = pTemp->pNext;				// Nächsten Gegner durchgehen
		}

		// Spieler steht auf der Brücke
		// dann das Brückenteil je nach Entfernung zum Spieler nach unten verschieben
		//
		for (int p = 0; p < NUMPLAYERS; p++)
		if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt],
							pPlayer[p]->xpos, 
							pPlayer[p]->ypos, 
							pPlayer[p]->CollideRect) == true &&
							pPlayer[p]->yspeed >= 0.0f)
		{
			float dx, dy;

			dx = (pPlayer[p]->xpos + 35) - (xPos + 5);
			dy = (pPlayer[p]->ypos + 40) - (yPos + 5);

			double a = sqrt ((dx * dx) + (dy * dy));

			yPos = float (Value1 + (32 - a / 1.5f));
		}
	}
	else
		yPos = (float)(Value1 + 3);

	
}

// --------------------------------------------------------------------------------------
// Bruecke explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerBruecke::GegnerExplode(void)
{
}
