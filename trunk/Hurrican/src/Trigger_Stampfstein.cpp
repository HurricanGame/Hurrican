// --------------------------------------------------------------------------------------
// Die Stampfstein
//
// Fällt runter, sobald der Hurri in Reichweite steht
// wird danach wieder hochgezogen
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_Stampfstein.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerStampfstein::GegnerStampfstein(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_NOTVISIBLE;
	BlickRichtung	= LINKS;
	Energy			= 5000.0f;
	Value1			= Wert1;
	Value2			= Wert2;	
	ChangeLight		= Light;
	Destroyable		= true;
	OwnDraw			= true;
	oldy			= 0.0f;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerStampfstein::DoDraw(void)
{
	// rendern
	//
	DirectGraphics.SetColorKeyMode();
	int size = (int)(oldy - yPos) + 165;
	pGegnerGrafix[GegnerArt]->SetRect(0, size, 80, 256);
	pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset), 
										   (float)(yPos-pTileEngine->YOffset) + size, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// Bewegungs KI
// --------------------------------------------------------------------------------------

void GegnerStampfstein::DoKI(void)
{
	Energy = 5000.0f;

	// Spieler kann seitlich nicht durchlaufen
	//
	Wegschieben(GegnerRect[GegnerArt], 0.0f);

	// Spieler kann von unten nicht durchspringen
	//
	for (int p = 0; p < NUMPLAYERS; p++)
	if (SpriteCollision(pPlayer[p]->xpos, 
						pPlayer[p]->ypos, 
						pPlayer[p]->CollideRect,
						xPos, yPos, GegnerRect[GegnerArt]) == true &&
						yPos < pPlayer[p]->ypos					   && 
						pPlayer[p]->yspeed < 0.0f)
						pPlayer[p]->yspeed *= -0.25f;

	// Ja nach Handlung richtig verhalten
	switch (Handlung)
	{
		// warten, bis der Hurri vorbeikommt ;)
		//
		case GEGNER_NOTVISIBLE:
		{
			oldy = yPos;
			Handlung = STEHEN;
		} break;

		case STEHEN:
		{
			oldy = yPos;

			// Spieler in der Nähe? Dann runterfallen lassen
			//
			for (int p = 0; p < NUMPLAYERS; p++)
			{
				pAim = pPlayer[p];

				if (PlayerAbstandHoriz() < 50 &&
					PlayerAbstandVert()  < 600 &&
					pPlayer[p]->ypos > yPos)
				{				
					Handlung = GEGNER_FALLEN;
					ySpeed   = 20.0f;
					yAcc     = 15.0f;

					pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_STONEFALL);				
				}
			}
		} break;

		
		// Stein fällt gerade runter
		//
		case GEGNER_FALLEN :
		{			
			// Fallgeschwindigkeit begrenzen
			//
			if (ySpeed > 40.0f)
				ySpeed = 40.0f;			

			// Spieler beim Runterfallen berührt? Dann stirbt er leider ;)
			//
			for (int p = 0; p < NUMPLAYERS; p++)
			{
				if (pPlayer[p]->AufPlattform != this &&
					SpriteCollision(pPlayer[p]->xpos, pPlayer[p]->ypos, pPlayer[p]->CollideRect,
									xPos, yPos, GegnerRect[GegnerArt]) == true &&
					pPlayer[p]->ypos + pPlayer[p]->CollideRect.bottom >= yPos)
				TestDamagePlayers(500.0f);
			}

			// Am Boden ? Dann aufstampfen und wieder nach oben fahren
			if (blocku & BLOCKWERT_WAND		 ||
				blocku & BLOCKWERT_PLATTFORM ||
				blocku & BLOCKWERT_WAND)
			{									
				// Staub erzeugen
				for (int i=0; i < 20; i++)
					pPartikelSystem->PushPartikel (xPos + i * 5 - 30, yPos + 220, SMOKEBIG);

				// Trümmer erzeugen
				for (int i=0; i < 20; i++)
					pPartikelSystem->PushPartikel (xPos + i * 5 - 10, yPos + 236, ROCKSPLITTER + rand()%2);

				pSoundManager->PlayWave(175, 128, 8000 + rand()%4000, SOUND_PHARAORAMM);				

				ySpeed = 0.0f;
				yAcc   = 0.0f;
				AnimCount = 20.0f;
				Handlung = GEGNER_INIT;												

				// Checken, ob Gegner darunter waren
				GegnerClass *pTemp;

				pTemp = pGegner->pStart;

				while (pTemp != NULL)
				{
					if (pTemp->Destroyable == true &&
						pTemp != this &&
						SpriteCollision(xPos, yPos, GegnerRect[GegnerArt],
										pTemp->xPos, pTemp->yPos, GegnerRect[pTemp->GegnerArt]) == true)
						pTemp->Energy = 0.0f;

					pTemp = pTemp->pNext;
				}
			}

		} break;

		// Stein wartet kurz und wird dann wieder hochgezogen
		case GEGNER_INIT:
		{
			PlattformTest(GegnerRect[GegnerArt]);

			AnimCount -= 1.0f SYNC;
			if (AnimCount < 0.0f)
			{
				ySpeed   = -4.0f;
				Handlung = GEGNER_SPRINGEN;

				// Hochzieh Sound
//				if (pSoundManager->its_Sounds[SOUND_CHAIN]->isPlaying == false)
//					pSoundManager->PlayWave(100, 128, 11000 + rand()%50, SOUND_CHAIN);
			}
		}
		break;

		// Stein wird hochgezogen
		//
		case GEGNER_SPRINGEN:
		{
			PlattformTest(GegnerRect[GegnerArt]);			

//			pSoundManager->Update3D((int)(xPos + 40), (int)(yPos + 20), SOUND_CHAIN);

			if (blocko & BLOCKWERT_WAND)
			{
				Handlung = GEGNER_STEHEN;
				ySpeed   = 0.0f;

//				pSoundManager->StopWave(SOUND_CHAIN);
			}

			// Spieler zwischen Decke und Stein eingeklemmt?
			//
			for (int p = 0; p < NUMPLAYERS; p++)
			if (pPlayer[p]->AufPlattform == this &&
				pTileEngine->BlockOben(pPlayer[p]->xpos, pPlayer[p]->ypos, 
									   pPlayer[p]->xposold, pPlayer[p]->yposold, 
									   pPlayer[p]->CollideRect) & BLOCKWERT_WAND)
				{
					pPlayer[p]->DamagePlayer(500.0f, true);
					pPlayer[p]->AufPlattform = NULL;
					pPlayer[p]->ypos += 10.0f;
					pPlayer[p]->yspeed = 10.0f;
				}	
		}
		break;

	} // switch

	// Spieler hat Stein von unten berührt (an den Stacheln) ?
	//
	for (int p = 0; p < NUMPLAYERS; p++)
	if (SpriteCollision(pPlayer[p]->xpos, 
						pPlayer[p]->ypos, 
						pPlayer[p]->CollideRect,
						xPos, yPos, GegnerRect[GegnerArt]) == true &&
						pPlayer[p]->ypos > yPos)
						pPlayer[p]->DamagePlayer(20.0f SYNC);		
}

// --------------------------------------------------------------------------------------
// Stampfstein explodiert (geht ja garnich *g*)
// --------------------------------------------------------------------------------------

void GegnerStampfstein::GegnerExplode(void)
{
}
