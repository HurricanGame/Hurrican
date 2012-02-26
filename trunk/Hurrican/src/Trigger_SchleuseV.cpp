// --------------------------------------------------------------------------------------
// Vertikale Schleuse
//
// Blockiert das Level
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_SchleuseV.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchleuseV::GegnerSchleuseV(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	BlickRichtung	= LINKS;
	Energy			= 100;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= true;
	TestBlock		= false;
}

// --------------------------------------------------------------------------------------
// "SchleuseV KI"
// --------------------------------------------------------------------------------------

void GegnerSchleuseV::DoKI(void)
{
	Energy = 100;
	DamageTaken = 0.0f;

	// Testen, ob der Spieler die Schleuse berührt hat
	//
	for (int p = 0; p < NUMPLAYERS; p++)
	if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt],
						pPlayer[p]->xpos, 
						pPlayer[p]->ypos, 
						pPlayer[p]->CollideRect) == true)
	{
		// Spieler wegschieben
		if (pPlayer[p]->xpos < xPos) pPlayer[p]->xpos = xPos + GegnerRect[GegnerArt].left  - pPlayer[p]->CollideRect.right;
		if (pPlayer[p]->xpos > xPos) pPlayer[p]->xpos = xPos + GegnerRect[GegnerArt].right - pPlayer[p]->CollideRect.left;
	}

	// Testen, ob ein Gegner die Schleuse berührt hat
	//
	GegnerClass *pTemp = pGegner->pStart;			// Anfang der Liste

	while (pTemp != NULL)							// noch nicht alle durch ?
	{
		if (pTemp->Active == true &&
		    SpriteCollision (xPos, yPos, GegnerRect[GegnerArt],
							 pTemp->xPos, pTemp->yPos, GegnerRect[pTemp->GegnerArt]) == true)
		{			
			if (pTemp->xPos < xPos) pTemp->xPos = xPos + GegnerRect[GegnerArt].left  - GegnerRect[pTemp->GegnerArt].right;	
			if (pTemp->xPos > xPos) pTemp->xPos = xPos + GegnerRect[GegnerArt].right - GegnerRect[pTemp->GegnerArt].left;
		}

		pTemp = pTemp->pNext;				// Nächsten Gegner durchgehen
	}

	switch (Handlung)
	{
		// Schleuse wird geöffnet
		//
		case GEGNER_OEFFNEN:
		{
			Handlung = GEGNER_LAUFEN;
			Value1   = int (yPos);
			yAcc     = -0.5f;
			if (pSoundManager->its_Sounds [SOUND_DOOR]->isPlaying == false)
				pSoundManager->PlayWave (100, 128, 11025, SOUND_DOOR);
		} break;

		// Schleuse öffnet sich
		//
		case GEGNER_LAUFEN:
		{
			if (ySpeed < -5.0f)
			{
				ySpeed = -5.0f;
				yAcc   =  0.0f;				
			}

			// Ganz geöffnet?
			if (yPos < float (Value1) - 201.0f)
			{
				ySpeed   = 0.0f;
				yAcc	 = 0.0f;
				Handlung = GEGNER_STEHEN;
				pSoundManager->PlayWave (100, 128, 11025, SOUND_DOORSTOP);
				pSoundManager->StopWave (SOUND_DOOR);
			}
		} break;
	} // switch
}

// --------------------------------------------------------------------------------------
// SchleuseV explodiert
// --------------------------------------------------------------------------------------

void GegnerSchleuseV::GegnerExplode(void)
{
}
