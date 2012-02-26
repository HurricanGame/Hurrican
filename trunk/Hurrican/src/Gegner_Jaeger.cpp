// --------------------------------------------------------------------------------------
// Der Abfangjaeger
//
// flieget erst im Hintergrund vorbei und saust dann von oben runter
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Jaeger.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerJaeger::GegnerJaeger(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_INIT;
	ySpeed			= -20.0f;
	Energy			= 120;
	ChangeLight		= Light;
	Destroyable		= false;
	Value1			= Wert1;
	Value2			= Wert2;
	OwnDraw			= true;
	AnimEnde		= 15;
	AnimStart		= 0;
	AnimSpeed		= 0.5f;
	smokeDelay		= 0.1f;
}	

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerJaeger::DoDraw(void)
{
	// beim Hoch fliegen
	//
	if (Handlung == GEGNER_DREHEN)
		pGegnerGrafix[GegnerArt]->RenderSpriteScaled((float)(xPos-pTileEngine->XOffset), 
													 (float)(yPos-pTileEngine->YOffset), 
													  35, 26, AnimPhase, 0xFFFFFFFF);
	// und beim runterfliegen
	//
	else
	if (Handlung == GEGNER_DREHEN2)
		pGegnerGrafix[GegnerArt]->RenderSpriteRotated((float)(xPos-pTileEngine->XOffset), 
												      (float)(yPos-pTileEngine->YOffset), 
													  180, AnimPhase, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerJaeger::DoKI(void)
{
	SimpleAnimation();

	switch (Handlung)
	{
		// Jaeger wird "aktiviert" (unter den Hurri gesetzt, damit er von dort hochfliegen kann)
		//
		case GEGNER_INIT:
		{
			Handlung = GEGNER_DREHEN;
			yPos += 512.0f;
		} break;

		// Gegner fliegt von unten nach oben
		//
		case GEGNER_DREHEN:
		{
			Destroyable		= false;

			// Gegner oben aus dem Screeb raus? Dann kurz in Pausemodus setzen
			//
			if (yPos - pTileEngine->YOffset < - 53)
			{
				Handlung    = GEGNER_STEHEN;				
				smokeDelay	= 50.0f; 
			}

		} break;

		// Gegner fliegt von oben nach unten
		//
		case GEGNER_DREHEN2:
		{
			// Gegner unten aus dem Screen raus? Dann explodieren lassen
			//
			if (yPos - pTileEngine->YOffset > 480)
			{
				Energy			= 0.0f;
			}

			// Antriebspartikel erzeugen

			smokeDelay -= 1.0f SYNC;

			while (smokeDelay <= 0.0f)
			{
				smokeDelay += 0.1f;

				pPartikelSystem->PushPartikel (xPos + 18, yPos - 8, ROCKETSMOKE);
				pPartikelSystem->PushPartikel (xPos + 40, yPos - 8, ROCKETSMOKE);
			}

		} break;

		// warten, bis Counter abgelaufen ist, und dann nach unten fliegen
		//
		case GEGNER_STEHEN:
		{
			smokeDelay -= 10.0f SYNC;

			if (smokeDelay <= 0.0f)
			{
				smokeDelay  = 0.1f;				
				Destroyable = true;
				ySpeed		= 22.0f;
				Handlung    = GEGNER_DREHEN2;
			}

		} break;			

		default : break;
	} // switch

	// Spieler getroffen beim runterfliegen?
	// Dann explodieren lassen und Energy abziehen
	//
	if (Handlung == GEGNER_DREHEN2)
		TestDamagePlayers(50.0f, true);
}

// --------------------------------------------------------------------------------------
// Jaeger explodiert
// --------------------------------------------------------------------------------------

void GegnerJaeger::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, 128, 11025, SOUND_EXPLOSION1);	

	for (int i=0; i < 6; i++)	
		pPartikelSystem->PushPartikel (xPos - 10 + rand ()%30, yPos - 8 + rand()%30, EXPLOSION_MEDIUM2);

	for (int i=0; i < 20; i++)	
	{
		pPartikelSystem->PushPartikel (xPos + rand ()%30, yPos + rand()%64, MINIFLARE);
		pPartikelSystem->PushPartikel (xPos + rand ()%25, yPos + rand()%50, SPIDERSPLITTER);
	}

	pPlayer[0]->Score += 100;
}
