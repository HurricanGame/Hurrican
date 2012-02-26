// --------------------------------------------------------------------------------------
// Die große Schnee Kugel
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_SnowBomb.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSnowBomb::GegnerSnowBomb(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_INIT;
	Energy			= 30;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= false;
	AnimSpeed		= 0.5f;
	AnimEnde		= 19;
	OwnDraw			= true;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerSnowBomb::DoDraw(void)
{
	int anim;

	if (xSpeed < 0.0f)	
		anim = AnimPhase;
	else
		anim = AnimEnde - AnimPhase;

	// Nur rendern, wenn die Kugel auch rollt
	//
	if (Handlung != GEGNER_INIT)
	pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset), 
									  	   (float)(yPos-pTileEngine->YOffset), 
										   anim, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSnowBomb::DoKI(void)
{	
	PlattformTest(GegnerRect[GegnerArt]);
	SimpleAnimation();

	// Je nach Handlung richtig verhalten
	//
	switch (Handlung)
	{
		// Kommt zum ersten mal ins Bild? Dann an die gewünschte Position setzen (Value1/Value2)
		//
		case GEGNER_INIT:
		{			
			// Schneebombe losrollen lassen, wenn Spieler in der Nähe ist
			//
			if (pAim->xpos > xPos)
				xSpeed =  15.0f;
			else
				xSpeed = -15.0f;

			Handlung = GEGNER_LAUFEN;
		} break;

		// Zur Seite kugeln
		//
		case GEGNER_LAUFEN:
		{
			if (!(blocku & BLOCKWERT_WAND))
			{
				ySpeed   = 10.0f;
				yAcc     = 5.0f;
				Handlung = GEGNER_FALLEN;
			}
		} break;

		// Fallen und ggf am Boden abspringen
		//
		case GEGNER_FALLEN:
		{
			if (ySpeed > 0.0f &&
				(blocku & BLOCKWERT_WAND ||
				 blocku & BLOCKWERT_PLATTFORM))
			{
				ySpeed   *= -0.7f;

				if (ySpeed > -1.0f)
				{
					ySpeed = 0.0f;
					yAcc   = 0.0f;
					Handlung = GEGNER_LAUFEN;

					// Position "begradigen"
					//
					blocku = pTileEngine->BlockUnten (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
				}

				for (int i = 0; i < 10; i++)
					pPartikelSystem->PushPartikel(xPos + rand()%50, yPos + rand()%20 + 50, WATERFLUSH2);

				pSoundManager->PlayWave(100, 128, 6000 + rand()%2000, SOUND_LANDEN);

			}

			if (ySpeed > 30.0f)
				ySpeed = 30.0f;
		} break;

		default : break;
	} // switch

	// Testen, ob der Spieler die Kugel berührt hat (nur, wenn er nicht drauf steht)
	//
	//TestDamagePlayers(4.0f SYNC);

	// an der Wand abprallen (und dabei Energie verlieren)
	//
	if ((xSpeed < 0.0f && blockl & BLOCKWERT_WAND) ||
		(xSpeed > 0.0f && blockr & BLOCKWERT_WAND))
	{
		Energy -=  1.0f;
		xSpeed *= -1.0f;
	}

	// Spieler seitlich verschieben, wenn auf Kugel drauf
	//
	//for (int i = 0; i < NUMPLAYERS; i++)
	//	if (pPlayer[i]->AufPlattform == this)	
	//		pPlayer[i]->xpos += xSpeed / 2.0f SYNC;
}

// --------------------------------------------------------------------------------------
// SnowBomb explodiert
// --------------------------------------------------------------------------------------

void GegnerSnowBomb::GegnerExplode(void)
{
	for (int i=0; i < 8; i++)
		pProjectiles->PushProjectile(xPos + rand()%60, yPos + rand()%50 , SNOWBOMBSMALL);

	for (int i=0; i < 20; i++)
		pPartikelSystem->PushPartikel(xPos + rand()%70 - 10, yPos + rand()%70 - 10, SNOWFLUSH);

	pSoundManager->PlayWave(100, 128, 6000 + rand()%2000, SOUND_LANDEN);

	for (int i = 0; i < NUMPLAYERS; i++)
	if (pPlayer[i]->AufPlattform == this)
		pPlayer[i]->AufPlattform = NULL;
}
