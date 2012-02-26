// --------------------------------------------------------------------------------------
// Der bröckelnde Felsblock
//
// fängt an zu bröckeln sobald der Spieler darüber läuft und fällt dann runter
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_BrockelRock.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerBrockelRock::GegnerBrockelRock(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	Energy			= 10;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= false;
	OwnDraw			= true;
	AnimCount		= 255.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerBrockelRock::DoDraw(void)
{
	pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset), 
										   (float)(yPos-pTileEngine->YOffset), 
										   0, D3DCOLOR_RGBA(255, 255, 255, int(AnimCount)));

	pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset), 
										   (float)(yPos-pTileEngine->YOffset), 
										   1, D3DCOLOR_RGBA(255, 255, 255, 255 - int(AnimCount)));
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerBrockelRock::DoKI(void)
{
	PlattformTest(GegnerRect[GegnerArt]);

	// Je nach Handlung richtig verhalten
	//
	switch (Handlung)
	{
		// Stein wartet bis der Spieler darüber läuft
		case GEGNER_STEHEN:						
		{
			for (int i = 0; i < NUMPLAYERS; i++)
				if (pPlayer[i]->AufPlattform == this)
					Handlung = GEGNER_SPRINGEN;
		} break;

		// Counter zählt runter bis Stein fällt
		//
		case GEGNER_SPRINGEN:					
		{
			AnimCount -= 50.0f SYNC;

			if (AnimCount < 0.0f)				// Langsam zerbröckeln
			{
				AnimCount = 0.0f;

				// Partikel erzeugen
				//
				for (int i = 0; i < 5; i++)
				{
					pPartikelSystem->PushPartikel(xPos + rand()%60, yPos + 20, ROCKSPLITTERSMALL);
					pPartikelSystem->PushPartikel(xPos + i * 10 - 10, yPos, SMOKEBIG);
				}

				// Sound ausgeben
				//
				pSoundManager->PlayWave(100, 128, 11025 + rand()%2000, SOUND_STONEFALL);
				Handlung = GEGNER_FALLEN;
				yAcc     =  4.0f;
			}

		} break;

		// Stein fällt runter
		//
		case GEGNER_FALLEN:
		{
			// Maximale Geschwindigkeitsbegrenzung
			if (ySpeed > 50.0f)
				yAcc = 0.0f;

			// Stein ist am Boden gelandet
			if (blocku & BLOCKWERT_WAND)
			{
				Energy = 0;
			}
		} break;		
		
		default : break;
	} // switch
}

// --------------------------------------------------------------------------------------
// Explodieren
// --------------------------------------------------------------------------------------

void GegnerBrockelRock::GegnerExplode(void)
{
	for (int i = 0; i < NUMPLAYERS; i++)
	if (pPlayer[i]->AufPlattform == this)	
		pPlayer[i]->AufPlattform = NULL;

	// Splitter erzeugen Rauch
	for (int i = 0; i<10; i++)
	{
		pPartikelSystem->PushPartikel(xPos + rand()%80-12, yPos + rand()%20 + 20, SMOKE);
		pPartikelSystem->PushPartikel(xPos + rand()%80-12, yPos + rand()%40, ROCKSPLITTER);
		pPartikelSystem->PushPartikel(xPos + rand()%80-12, yPos + rand()%40, ROCKSPLITTERSMALL);
	}

	pSoundManager->PlayWave(100, 128, 11025 + rand()%2000, SOUND_STONEEXPLODE);	// Sound ausgeben
}
