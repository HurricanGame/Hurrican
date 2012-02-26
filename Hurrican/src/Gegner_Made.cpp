// --------------------------------------------------------------------------------------
// Die Made
//
// Wird vom Bratklops gekotzt und explodiert am Boden/Hurri oder bei Beschuss
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Made.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerMade::GegnerMade(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_INIT;
	HitSound		= 1;
	Energy			= 10;
	Value1			= Wert1;
	Value2			= Wert2;
	AnimStart		= 0;
	AnimEnde		= 20;
	AnimSpeed		= (rand()%10 + 5) / 20.0f;
	AnimCount		= 0.0f;
	ChangeLight		= Light;
	Destroyable		= true;
	AnimPhase		= 16;
	OwnDraw			= true;
	movesin			= 0.0f;

	// Nur nach rechts fliegen? (Bratklops)
	//
	if (Value1 == 98)
	{
		AnimPhase = rand()%20;
		yAcc   = 4.0f;
		xSpeed =  (float)  (rand()%120) / 3.0f;
		ySpeed = - float (((rand()%40) / 3.0f) + 8.0f);
		Handlung = GEGNER_FALLEN;
	}

	// oder in alle Richtungen (Schwabbel)
	else
	if (Value1 == 99)
	{
		AnimPhase = rand()%20;
		yAcc   = 4.0f;
		xSpeed =  (float)  (rand()%200-100)/12;
		ySpeed = - float (((rand()%40) / 2.0f) + 12.0f);
		Handlung = GEGNER_FALLEN;
	}	
}

// --------------------------------------------------------------------------------------
// Draw funktion
// --------------------------------------------------------------------------------------

void GegnerMade::DoDraw(void)
{
	movesin += 0.8f SYNC;

	if (movesin > 2*PI)
		movesin = 0.0f;
	
	switch (Handlung)
	{
		case GEGNER_LAUFEN:
		{
			pGegnerGrafix[GegnerArt]->RenderSpriteScaled((float)(xPos - pTileEngine->XOffset) - (int)(sin(movesin) * 2.5f),
														 (float)(yPos - pTileEngine->YOffset),
														 20 + (int)(sin(movesin) * 5.0f), 
														 20, 15, 0xFFFFFFFF);

		} break;

		default:
		{
			pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos - pTileEngine->XOffset),
												   (float)(yPos - pTileEngine->YOffset), AnimPhase,
													0xFFFFFFFF);
		} break;
	}
}


// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerMade::DoKI(void)
{
	// langsame zugrunde gehen :P
	Energy -= 0.1f SYNC;

	BlickRichtung = LINKS;

	if (Handlung == GEGNER_INIT)
	{
		Handlung = GEGNER_LAUFEN;

		if (xPos + 10 < pAim->xpos + 35) 
			xSpeed = 0.8f;
		else
			xSpeed = -0.8f;
	}

	if (Handlung == GEGNER_FALLEN)
	{
		SimpleAnimation();

		if ((blocku & BLOCKWERT_WAND) ||
			(blocku & BLOCKWERT_GEGNERWAND) ||
			(blocku & BLOCKWERT_PLATTFORM))
		{
			Handlung = GEGNER_LAUFEN;
		}
	}
	else
	{
		AnimPhase = 15;
		yAcc = 4.0f;
		ySpeed = 0.0f;

		if (!(blocku & BLOCKWERT_WAND) &&
			!(blocku & BLOCKWERT_GEGNERWAND) &&
			!(blocku & BLOCKWERT_PLATTFORM))
		{
			Handlung = GEGNER_FALLEN;
			yAcc = 4.0f;
		}
	}

	// an der Wand umdrehen
	if ((xSpeed < 0.0f &&
		(blockl & BLOCKWERT_WAND ||
		 blockl & BLOCKWERT_GEGNERWAND)) ||

		(xSpeed > 0.0f &&
		(blockr & BLOCKWERT_WAND ||
		 blockr & BLOCKWERT_GEGNERWAND)))
	{
		xSpeed *= -1;
	}

	// Testen, ob der Spieler die Made berührt hat
	//
	TestDamagePlayers(2.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Made explodiert
// --------------------------------------------------------------------------------------

void GegnerMade::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, rand ()%200 + 20, 8000 + rand()%4000, SOUND_MADE);

	for (int i = 0; i < 10; i++)
		pPartikelSystem->PushPartikel(float(xPos - 10 + rand()%24), 
									  float(yPos - 12 + rand()%22), MADEBLUT);
}
