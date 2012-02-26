// --------------------------------------------------------------------------------------
// Der neue Fisch
//
// Schwimmt wie der alte Fisch im Wasser, und geht auf den Hurri los
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_NeuFisch.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerNeuFisch::GegnerNeuFisch(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	HitSound		= 1;
	Energy			= 60;
	AnimEnde		= 9;
	AnimSpeed		= 0.5f;
	AnimCount		= 0.0f;
	ChangeLight		= Light;
	Destroyable		= true;
	pFest			= NULL;
	MoveSpeed		= 5.0f + (float)(rand()%50) / 10.0f;
}


// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerNeuFisch::DoKI(void)
{
	// Animieren
	//
	AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
	if (Handlung == GEGNER_LAUFEN ||
		Handlung == GEGNER_SPECIAL)
	{
		if (AnimCount > AnimSpeed)			// Grenze überschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen
			AnimPhase++;					// Und nächste Animationsphase
			if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
				AnimPhase = AnimStart;		// Dann wieder von vorne beginnen
		}
	} 
	else
	if (Handlung == GEGNER_DREHEN)
	{
		if (AnimCount > AnimSpeed)			// Grenze überschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen
			AnimPhase++;					// Und nächste Animationsphase
			if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
			{
				AnimCount = 0.0f;
				Handlung  = GEGNER_DREHEN2;
				BlickRichtung *= -1;
			}
		}
	} 
	else
	if (Handlung == GEGNER_DREHEN2)
	{
		if (AnimCount > AnimSpeed)			// Grenze überschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen
			AnimPhase--;					// Und nächste Animationsphase
			if (AnimPhase <= 9)				// Animation von zu Ende	?
			{
				AnimPhase = AnimStart;		// Dann wieder von vorne beginnen
				AnimEnde  = 9;
				Handlung  = GEGNER_LAUFEN;
				AnimSpeed = 0.5f;

				xSpeed = BlickRichtung * MoveSpeed;
			}
		}
	} 

	// Spieler verfolgen
	//
	ySpeed = 0.0f;

	if (Handlung != GEGNER_SPECIAL)
	{		
		// verfolgen
		xSpeed = BlickRichtung * MoveSpeed;

		if (pAim->InLiquid == true)
		{
			if (yPos < pAim->ypos - 8 && (blocku & BLOCKWERT_WASSER)) ySpeed =  float(rand()%50 + 50) / 10.0f;
			if (yPos > pAim->ypos - 8 && (blocko & BLOCKWERT_WASSER)) ySpeed = -float(rand()%50 + 50) / 10.0f;
		}
	}

	switch (Handlung)
	{
		// In der Suppe rumdümpeln
		//
		case GEGNER_LAUFEN:
		{		
			bool onWall = false;

			onWall = (BlickRichtung == LINKS &&
					 ((blockl & BLOCKWERT_WAND) ||
					  (blockl & BLOCKWERT_GEGNERWAND))) ||
					 (BlickRichtung == RECHTS &&
					 ((blockr & BLOCKWERT_WAND) ||
					  (blockr & BLOCKWERT_GEGNERWAND)));

			if (onWall ||
				(pAim->InLiquid == true &&
				(xPos + 30 < pAim->xpos + 35 && BlickRichtung == LINKS  ||
			 	 xPos + 30 > pAim->xpos + 35 && BlickRichtung == RECHTS)))
			{
				Handlung = GEGNER_DREHEN;
				AnimPhase = 9;
				AnimCount = 0.0f;
				AnimEnde  = 14;
				AnimSpeed = 0.3f;
				xSpeed = 0.0f;
				ySpeed = 0.0f;
			}

			blockl = blockr = 0;
			
			RECT rect;			
			rect.top    = 32;	
			rect.bottom = 40;

			if (BlickRichtung == LINKS)
			{
				rect.left   = 10;	
				rect.right  = 20;
			}
			else
			{
				rect.left   = 70;
				rect.right  = 80;
			}

			// festgebissen?
			for (int p = 0; p < NUMPLAYERS; p++)
			if (SpriteCollision(pPlayer[p]->xpos, pPlayer[p]->ypos, pPlayer[p]->CollideRect,
								xPos, yPos, rect) == true)
			{
				pFest = pPlayer[p];
				TestBlock = false;
				Handlung = GEGNER_SPECIAL;
				AnimSpeed = 0.3f;

				Value1 = (int)(pAim->xpos - xPos);
				Value2 = (int)(pAim->ypos - yPos);

				AnimPhase = 0;
				AnimEnde  = 9;
				AnimSpeed = 0.5f;
			}
		} break;

		// Fisch hängt am Spieler fest
		case GEGNER_SPECIAL:
		{
			pFest->GegnerDran = true;
			AnimPhase = 0;
			TestDamagePlayers(1.5f SYNC);

			xPos = pFest->xpos - Value1;
			yPos = pFest->ypos - Value2;

			if (pFest->Handlung == RADELN ||
				pFest->Handlung == RADELN_FALL)
			{
				Handlung  = GEGNER_LAUFEN;;
				AnimEnde  = 9;
				AnimSpeed = 0.5f;
				AnimCount = 0.0f;
				TestBlock = true;

				if (pFest->InLiquid == false)
					Energy = 0.0f;
			}
		} break;
	}	
}


// --------------------------------------------------------------------------------------
// NeuFisch explodiert
// --------------------------------------------------------------------------------------

void GegnerNeuFisch::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);

	for (int i = 0; i < 10; i++)
	{
		// Fetzen erzeugen
		pPartikelSystem->PushPartikel(float(xPos - 20 + rand()%90), 
									  float(yPos - 5  + rand()%70), PIRANHATEILE);

		// und noch n paar Luftblässchen dazu
		pPartikelSystem->PushPartikel(float(xPos - 10  + rand()%90), 
									  float(yPos + 10  + rand()%70), BUBBLE);
	}
	
	// Blutwolke dazu
	//
	for (int i = 0; i < 5; i++)
		pPartikelSystem->PushPartikel(float(xPos + rand()%60), 
									  float(yPos + rand()%40), PIRANHABLUT);

	pPlayer[0]->Score += 250;
}
