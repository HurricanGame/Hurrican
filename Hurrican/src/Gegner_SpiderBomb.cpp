// --------------------------------------------------------------------------------------
// Die Spinnenbombe
//
// Läuft in Richtung Spieler und explodiert an ihm
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_SpiderBomb.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSpiderBomb::GegnerSpiderBomb(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	AnimStart		= 0;
	AnimEnde		= 5;
	AnimSpeed		= 0.75f;
	BlickRichtung	= LINKS;
	xSpeed			=  0.0;
	ySpeed			=  0.0f;
	xAcc			=  0.0f;
	yAcc			=  0.0f;
	Energy			= 30;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= true;

	// von der Spinnenmaschine ausgespuckt? Dann erstmal rausfliegen
	if (Wert1 == 99)
	{
		Handlung  = GEGNER_FALLEN;
		AnimPhase = 6;
		xSpeed =  (float)(rand()%10) + 20.0f;
		ySpeed = -(float)(rand()%10 + 30.0f);
		yAcc   = 5.0f;
	}
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSpiderBomb::DoKI(void)
{
	if (Handlung == GEGNER_LAUFEN)
		SimpleAnimation();

// Nach links bzw rechts auf Kollision prüfen und dann ggf umkehren
	if (BlickRichtung == LINKS)
	  if (blockl & BLOCKWERT_WAND)// || blockl & BLOCKWERT_GEGNERWAND)
		{
			BlickRichtung =  RECHTS;
			xSpeed	      = -xSpeed;
		}

	if (BlickRichtung == RECHTS)
	  if (blockr & BLOCKWERT_WAND)// || blockr & BLOCKWERT_GEGNERWAND)
		{
			BlickRichtung =  LINKS;
			xSpeed		  = -xSpeed;
		}

	if (ySpeed >= 0.0f)
		blocku = pTileEngine->BlockSlopes(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt], ySpeed, true);

	if (!(blocku & BLOCKWERT_SCHRAEGE_R) && 
	    !(blocku & BLOCKWERT_SCHRAEGE_L))
		blocku = pTileEngine->BlockUnten (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);


// Je nach Handlung richtig verhalten
	switch (Handlung)
	{
		case GEGNER_LAUFEN:						// Spinne läuft rum
		{
			// Kein Boden mehr unter den Füssen ? Dann runterfallen lassen
			if (!(blocku & BLOCKWERT_WAND) &&
				!(blocku & BLOCKWERT_PLATTFORM))
			{
				Handlung = GEGNER_FALLEN;
				yAcc   = 5.0f;
				ySpeed = 1.0f;
			}

			// Spieler links von der Bombe ?
			if (pAim->xpos + 35 < xPos + 25)	
			{
				xAcc		  = -4.0f;
				BlickRichtung = LINKS;
			}
			else
			{
				xAcc		  = 4.0f;
				BlickRichtung = RECHTS;
			}

			// Speed begrenzen
			if (xSpeed > 10.0f)
			{
				xSpeed = 10.0f;
				xAcc = 0.0f;
			}

			if (xSpeed < -10.0f)
			{
				xSpeed = -10.0f;
				xAcc = 0.0f;
			}

			// Spieler in Reichweite? Dann in Lauerstellung gehen
			if (PlayerAbstand() < 150)
			{
				Handlung = GEGNER_SPECIAL;
				AnimPhase = 5;
				AnimCount = 5.0f;
				xSpeed = 0.0f;
				xAcc   = 0.0f;
				ySpeed = 0.0f;
			}

		} break;

		// kurz geduckt am Boden bleiben
		case GEGNER_SPECIAL:
		{
			AnimCount -= 1.0f SYNC;

			if (AnimCount <= 0.0f)
			{
				Handlung = GEGNER_FALLEN;
				AnimPhase = 6;
				ySpeed = -22.0f;
				yPos   -= 5.0f;
				yAcc   = 5.0f;

				if (pAim->xpos + 35 > xPos + 15)
					xSpeed = 25.0f;
				else
					xSpeed = -25.0f;				
			}
		} break;

		case GEGNER_SPRINGEN:
		{
		} break;

		case GEGNER_FALLEN:						// Spinne fällt runter
		{
			// Wieder Boden unter den Füssen ? Dann wieder normal laufen
			if (blocku & BLOCKWERT_WAND ||
				blocku & BLOCKWERT_PLATTFORM)
			{
				Handlung  = GEGNER_LAUFEN;
				ySpeed    = 0.0f;
				yAcc      = 0.0f;
				AnimPhase = 0;
			}
		} break;

		default : break;
	} // switch
	
	// Testen, ob der Spieler die Bombe berührt hat
	TestDamagePlayers(20.0f, true);
}

// --------------------------------------------------------------------------------------
// Explodieren
// --------------------------------------------------------------------------------------

void GegnerSpiderBomb::GegnerExplode(void)
{
	ShakeScreen(5);

	for (int i=0; i<6; i++)
		pPartikelSystem->PushPartikel(float(xPos - 60 + rand()%80), 
									  float(yPos - 40 + rand()%30), EXPLOSION_MEDIUM2);

	pPartikelSystem->PushPartikel(float(xPos - 20), 
								  float(yPos - 40), EXPLOSION_BIG);

	for (int i=0; i<4; i++)
		pPartikelSystem->PushPartikel(float(xPos + 10), 
									  float(yPos + 10), SPLITTER);

	pSoundManager->PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION3);	// Sound ausgeben

	pPlayer[0]->Score += 250;
}