// --------------------------------------------------------------------------------------
// Der Walker
//
// Läuft bis er an eine Wand kommt und kehrt dann um und schiesst, sobald er einen
// bestimmten Abstand zum Spiel hat, geradeaus, läuft einen Schritt weiter, schiesst
// wieder usw
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Walker.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerWalker::GegnerWalker(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	AnimStart		= 0;
	AnimEnde		= 11;
	AnimSpeed		= 0.5f;
	xSpeed			= 10.0f;
	ySpeed			=  0.0f;
	xAcc			=  0.0f;
	yAcc			=  0.0f;
	Energy			= 10;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= true;
	ShotDelay		= 0.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerWalker::DoKI(void)
{	
	SimpleAnimation();

	// Nach links bzw rechts auf Kollision prüfen und dann ggf umkehren
	if (BlickRichtung == LINKS)
	  if (blockl & BLOCKWERT_WAND ||
		  blockl & BLOCKWERT_GEGNERWAND)
		{
			BlickRichtung =  RECHTS;
			xSpeed	      = 10.0f;

			if (Handlung == GEGNER_WATSCHELN)
				Energy = 0;
		}

	if (BlickRichtung == RECHTS)
	  if (blockr & BLOCKWERT_WAND ||
		  blockr & BLOCKWERT_GEGNERWAND)
		{
			BlickRichtung =  LINKS;
			xSpeed		  = -10.0f;

			if (Handlung == GEGNER_WATSCHELN)
				Energy = 0;
		}

	blocku = pTileEngine->BlockUnten (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

	// In Richtung Spieler laufen, wenn angeschossen
	//
	if (DamageTaken > 0				 &&
		Handlung != GEGNER_WATSCHELN &&
		Handlung != GEGNER_SPRINGEN)
	{
		if (pAim->xpos < xPos)
		{
			BlickRichtung = LINKS;
			xSpeed = -10.0f;
		}
		else
		{
			BlickRichtung = RECHTS;
			xSpeed = 10.0f;
		}
	}

	// Spieler kann Walker auf die Birne hopsen
	//
	if (Handlung != GEGNER_WATSCHELN &&
		Handlung != GEGNER_SPRINGEN)
		PlattformTest(GegnerRect[GegnerArt]);

// Je nach Handlung richtig verhalten
	switch (Handlung)
	{
		case GEGNER_LAUFEN:						// Normal laufen und dabei ab und zu schiessen
		{
			// Testen, ob der Walker runterfällt
			if (!(blocku & BLOCKWERT_WAND) &&
				!(blocku & BLOCKWERT_PLATTFORM))
			{ 
				Handlung = GEGNER_FALLEN;
				yAcc	  = 4.0f;
			}

			// Bei bestimmten Mindestabstand schiessen lassen
			if (PlayerAbstand() <= 220 &&
				((BlickRichtung == LINKS  && pAim->xpos+45 <= xPos) ||
				 (BlickRichtung == RECHTS && pAim->xpos-45 >= xPos)))
			{
				ShotDelay -= 1.0f SYNC;

				if (ShotDelay <= 0.0f)
				{
					ShotDelay = (float)(10 + rand()%5);
					AnimStart = 12;
					AnimPhase = 12;
					AnimEnde  = 20;
					xSpeed	  = 0.0f;
					Handlung  = GEGNER_SCHIESSEN;
				}				
			}
		} break;

		case GEGNER_SCHIESSEN:					// gegner schiesst auf den Spieler und läuft dann
		{
			xSpeed = 0.0f;

			if (AnimPhase == AnimStart &&		// Weiterlaufen
				AnimCount == 0.0f)
			{
				Handlung  = GEGNER_LAUFEN;
				AnimStart = 0;
				AnimPhase = 0;
				AnimEnde  = 11;
				xSpeed	  = float(10.0 * BlickRichtung);
			}

			// Schuss abgeben
			if (AnimPhase == 17	&&
				AnimCount == 0.0f)
			{
				pSoundManager->PlayWave(100, 128, 18000 + rand()%2000, SOUND_LASERSHOT);

				if (BlickRichtung == LINKS)
					pProjectiles->PushProjectile(xPos-18, yPos + 23, WALKER_LASER);
				else
					pProjectiles->PushProjectile(xPos+30, yPos + 23, WALKER_LASER2);
			}
		} break;

		case GEGNER_WATSCHELN:					// Walker ist getroffen und haut ab
		{
			// Testen, ob der Walker runterfällt
			if (!(blocku & BLOCKWERT_WAND) &&
				!(blocku & BLOCKWERT_PLATTFORM))
			{ 
				Handlung = GEGNER_SPRINGEN;
				yAcc	  = 3.0f;
			}
		} break;

		case GEGNER_FALLEN:						// Normal runterfallen
		{
			// Keine zu hohe Geschwindigkeit
			if (ySpeed > 25.0f)
				ySpeed = 25.0f;

			// Testen, ob der Walker auf den Boden kommt
			if (blocku & BLOCKWERT_WAND ||
				blocku & BLOCKWERT_PLATTFORM)
			{ 
				Handlung = GEGNER_LAUFEN;
				yAcc	  = 0.0f;
				ySpeed	  = 0.0f;
			}
		} break;

		case GEGNER_SPRINGEN:					// Getroffen fallen
		{
			// Keine zu hohe Geschwindigkeit
			if (ySpeed > 30.0f)
				ySpeed = 30.0f;

			// Testen, ob der Walker auf den Boden kommt
			if (ySpeed > 0.0f &&
			   (blocku & BLOCKWERT_WAND ||
			    blocku & BLOCKWERT_PLATTFORM))
			{ 
				Handlung  = GEGNER_WATSCHELN;
				yAcc	  =  0.0f;
				ySpeed	  =  0.0f;
				xSpeed	  = 25.0f * BlickRichtung;
			}
		} break;

		default : break;
	} // switch

	// Spieler kann dem Walker auf den Kopf springen
	for (int i = 0; i < NUMPLAYERS; i++)
	if (pPlayer[i]->Handlung != RADELN      &&
		pPlayer[i]->Handlung != RADELN_FALL &&
		pPlayer[i]->yspeed >= 0.0f)
	{
		if (pPlayer[i]->AufPlattform == this)
		{														// Spieler springen lassen
			pPlayer[i]->AufPlattform = NULL;
			pPlayer[i]->JumpPossible = false;
			pPlayer[i]->AnimPhase    = 2;
			pPlayer[i]->Handlung     = SPRINGEN;
			pPlayer[i]->JumpStart    = pPlayer[i]->ypos;
			pPlayer[i]->yspeed       = -PLAYER_MAXJUMPSPEED;
			pPlayer[i]->JumpAdd	  =  0.0f;

			AnimSpeed = 0.3f;
			AnimPhase = 20;
			AnimStart = 20;
			AnimEnde  = 31;
			Handlung  = GEGNER_SPRINGEN;

			xSpeed	  =   0.0f;
			ySpeed	  = -30.0f;
			yAcc	  =   5.0f;

			pSoundManager->PlayWave(100, 128, 11025, SOUND_WALKERGIGGLE);
			blocku = pTileEngine->BlockUnten (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
			yPos -= 5.0f;
		}
	}

	// Testen, ob der Spieler den Walker berührt hat
	if (Handlung != GEGNER_WATSCHELN &&
		Handlung != GEGNER_SPRINGEN)
		TestDamagePlayers(4.0f SYNC, false);
}

// --------------------------------------------------------------------------------------
// Walker explodiert
// --------------------------------------------------------------------------------------

void GegnerWalker::GegnerExplode(void)
{
	for (int i=0; i<5; i++)
		pPartikelSystem->PushPartikel(float(xPos - 20 + rand()%45), 
									  float(yPos - 20 + rand()%45), EXPLOSION_MEDIUM2);

	pSoundManager->PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION1);	// Sound ausgeben

	pPlayer[0]->Score += 100;
}
