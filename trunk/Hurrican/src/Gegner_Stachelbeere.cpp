// --------------------------------------------------------------------------------------
// Die Stachelbeere
//
// Rollt durch den Cave und klappt manchmal auf, um zu ballern =)
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Stachelbeere.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerStachelbeere::GegnerStachelbeere(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	Energy			= 120;
	BlickRichtung	= LINKS;
	Value1			= Wert1;
	Value2			= Wert2;
	AnimSpeed		= 0.3f;
	AnimStart		= 0;
	AnimEnde		= 5;
	ChangeLight		= Light;
	Destroyable		= false;
	OwnDraw			= true;
	RollCount		= 0.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Drawroutine
// --------------------------------------------------------------------------------------

void GegnerStachelbeere::DoDraw(void)
{
	bool mirrored;
	D3DCOLOR color = 0xFFFFFFFF;

	mirrored = (BlickRichtung == RECHTS);

	switch (Handlung)
	{
		case GEGNER_LAUFEN:
		case GEGNER_LAUFEN2:
		{
			pGegnerGrafix[GegnerArt]->SetRect(AnimPhase * 60, 0, (AnimPhase + 1) * 60, 60);
			pGegnerGrafix[GegnerArt]->RenderSprite(xPos - (float)pTileEngine->XOffset,
												   yPos - (float)pTileEngine->YOffset,
												   AnimPhase, color, mirrored);

		} break;

		case GEGNER_OEFFNEN:
		case GEGNER_SCHLIESSEN:
		case GEGNER_SCHIESSEN:
		{
			pGegnerGrafix[GegnerArt]->SetRect((AnimPhase % 3) * 120,	   (AnimPhase / 3 + 1) * 60, 
											  (AnimPhase % 3) * 120 + 120, (AnimPhase / 3 + 2) * 60);

			if (!mirrored)
				pGegnerGrafix[GegnerArt]->RenderMirroredSprite(xPos - (float)pTileEngine->XOffset - 60.0f,
															   yPos - (float)pTileEngine->YOffset,
															   color);
			else
				pGegnerGrafix[GegnerArt]->RenderSprite(xPos - (float)pTileEngine->XOffset,
													   yPos - (float)pTileEngine->YOffset,
													   color);
		} break;
	}	
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerStachelbeere::DoKI(void)
{		
	// Schrägen checken
	//
	if (ySpeed >= 0.0f)
		blocku = pTileEngine->BlockSlopes(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt], ySpeed);

	// Runterfallen?
	if (!(blocku & BLOCKWERT_SCHRAEGE_R) && 
		!(blocku & BLOCKWERT_SCHRAEGE_L))
	{
		blocku = pTileEngine->BlockUnten (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

		if (!(blocku & BLOCKWERT_WAND) && 
			!(blocku & BLOCKWERT_PLATTFORM))
			yAcc = 8.0f;
	}	

	// AnimSpeed passend zur Bewegugnsgeschwindigkeit setzen
	if (Handlung == GEGNER_LAUFEN ||
		Handlung == GEGNER_LAUFEN2)
	{
		float temp;

		temp = xSpeed;
		if (temp < 0.0f)
			temp *= -1;

		AnimSpeed = 2.8f - temp / 4.0f;
	}

	
	if (ySpeed > 0.0f)
	{
		if (ySpeed > 40.0f)
			ySpeed = 40.0f;

		// am Boden abhopsen?
		if (blocku & BLOCKWERT_WAND ||
			blocku & BLOCKWERT_PLATTFORM)
		{
			// Partikel erzeugen
			int i = 0;
			for (int i = 0; i < 10; i++)
				pPartikelSystem->PushPartikel(xPos + rand()%60, yPos + 50, FUNKE);

			for (int i = 0; i < 5; i++)
				pPartikelSystem->PushPartikel(xPos + rand()%40, yPos + 40, SMOKE);

			// Geschwindigkeit umdrehen
			ySpeed *= -0.5f;

			if (ySpeed > -5.0f)
			{
				ySpeed = 0.0f;
				yAcc = 0.0f;
			}
		}

	}

	// Je nach Handlung richtig verhalten
	//
	switch (Handlung)
	{
		case GEGNER_OEFFNEN:				// Kugel klappt auf
		{
			SimpleAnimation();

			if (AnimPhase >= AnimEnde - 1)
			{
				ShotCount = 4;
				RollCount = 4.0f;
				
				Handlung = GEGNER_SCHIESSEN;				
				AnimStart = -1;
				AnimPhase = AnimEnde - 2;
			}
		} break;

		case GEGNER_SCHIESSEN:				// Kugel ballert
		{
			RollCount -= 1.0f SYNC;

			if (RollCount < 0.0f)
			{
				int off = 25;

				if (BlickRichtung == LINKS)
					off = -60;

				pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_GRANATE);

				pGegner->PushGegner(xPos + ShotCount * 15.0f + off,
									yPos + 10.0f, 
									MINIROCKET, 0, 99, false, false);

				RollCount = 2.0f;
				ShotCount--;

				if (ShotCount <= 0)
				{
					Handlung = GEGNER_SCHLIESSEN;					
				}
			}
		} break;

		case GEGNER_SCHLIESSEN:				// Kugel klappt wieder zu
		{
			SimpleAnimation(true);

			if (AnimPhase <= AnimStart + 1)
			{
				AnimStart		= 0;
				AnimEnde		= 5;
				Handlung	    = GEGNER_LAUFEN;
				RollCount		= 50.0f;
				Destroyable = false;
			}
		} break;

		// abbremsen
		case GEGNER_LAUFEN2:
		{
			SimpleAnimation();
			bool stop = false;

			// abbremsen
			if (xSpeed > 0.0f)
			{		
				xSpeed -= 1.0f SYNC;

				if (xSpeed < 0.0f)
					stop = true;									
			} 
			else
			if (xSpeed < 0.0f)
			{		
				xSpeed += 1.0f SYNC;

				if (xSpeed > 0.0f)
					stop = true;									
			}			
			else
				stop = true;

			// angehalten?
			if (stop) 
			{
				xSpeed = 0.0f;
				Handlung = GEGNER_OEFFNEN;
				Destroyable = true;
				AnimPhase = 0;
				AnimEnde = 16;
				AnimSpeed = 1.0f;
				pSoundManager->PlayWave(50, 128, 12000 + rand()%1000, SOUND_STEAM);
			}
		} break;

		case GEGNER_LAUFEN:					// Kugel hüpft im Gang rum
		{
			if (RollCount > 0.0f)
				RollCount -= 1.0f SYNC;

			SimpleAnimation();

			if (BlickRichtung == LINKS)
				xAcc = -10.0f;
			else
				xAcc = 10.0f;

			if (xSpeed < -10.0)
				xSpeed = -10.0f;

			if (xSpeed > 10.0)
				xSpeed = 10.0f;

			// an der Wand umdrehen
			// oder am Screenrand, wenn der Screen gelockt ist
			if ((pTileEngine->Zustand == ZUSTAND_LOCKED &&
				 BlickRichtung == LINKS &&
				 xPos < pTileEngine->XOffset) ||

				(pTileEngine->Zustand == ZUSTAND_LOCKED &&
				 BlickRichtung == RECHTS &&
				 xPos > pTileEngine->XOffset + 640 - 50) ||

				(BlickRichtung == LINKS &&
				 (blockl & BLOCKWERT_WAND ||
				  blockl & BLOCKWERT_GEGNERWAND)) ||

				(BlickRichtung == RECHTS &&
				(blockr & BLOCKWERT_WAND ||
				 blockr & BLOCKWERT_GEGNERWAND)))
			{
				xSpeed *= -1;
				BlickRichtung *= -1;
			}

			if (ySpeed == 0.0f &&
				RollCount <= 0.0f &&
				(blocku & BLOCKWERT_WAND) &&
				PlayerAbstand() < 400)
			{
				Handlung = GEGNER_LAUFEN2;
				xAcc = 0.0f;
			}

		} break;

		default : break;
	} // switch

	// Berührt die Kugel den Spieler ?
	//
	Wegschieben(GegnerRect[GegnerArt], 4.0f);	
}

// --------------------------------------------------------------------------------------
// Stachelkugel explodiert
// --------------------------------------------------------------------------------------

void GegnerStachelbeere::GegnerExplode(void)
{
	for (int i = 0; i < 5; i++)
		pPartikelSystem->PushPartikel(float(xPos - 30 + rand()%60), 
									  float(yPos - 30 + rand()%60), EXPLOSION_MEDIUM2);

	// Explosion
	pPartikelSystem->PushPartikel(float(xPos - 15), 
								  float(yPos - 15), EXPLOSION_BIG);

	pSoundManager->PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION4);	// Sound ausgeben

	pPlayer[0]->Score += 300;

	// noch einm paar Raketen spawnen
	pGegner->PushGegner(xPos + 15.0f,
						yPos + 35.0f, 
						MINIROCKET, 0, 99, false, false);

	pGegner->PushGegner(xPos + 45.0f,
						yPos + 35.0f, 
						MINIROCKET, 0, 99, false, false);

	pGegner->PushGegner(xPos + 30.0f,
						yPos + 20, 
						MINIROCKET, 0, 99, false, false);
}
