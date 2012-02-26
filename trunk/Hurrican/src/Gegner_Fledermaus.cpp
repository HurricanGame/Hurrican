// --------------------------------------------------------------------------------------
// Die Fledermaus
//
// Hängt an der Decke und fliegt auf den Hurri zu, wenn er in die Nähe kommt,
// oder wenn sie angeschossen wird
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Fledermaus.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFledermaus::GegnerFledermaus(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	Energy			= 35;
	AnimSpeed		= 0.75f;
	ChangeLight		= Light;
	Destroyable		= true;
	AnimPhase		= 15;
	AnimStart		= 15;
	AnimEnde		= 21;
	Value1			= Wert1;
	Value2			= Wert2;

	// gleich losfliegen ? (Beim Cave "Boss" T.R.Schmidt Orgien Quark)
	//
	if (Value2 == 1)
		Handlung  = GEGNER_LAUFEN;		
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFledermaus::DoKI(void)
{
	SimpleAnimation();

	if ((blocku & BLOCKWERT_WAND ||	 blocku & BLOCKWERT_PLATTFORM) && ySpeed > 0.0f) ySpeed = -ySpeed;
	if (blocko & BLOCKWERT_WAND &&	ySpeed < 0.0f)	ySpeed = -ySpeed;
	if (blockr & BLOCKWERT_WAND &&	xSpeed > 0.0f){ xSpeed = -xSpeed; BlickRichtung = LINKS; }
	if (blockl & BLOCKWERT_WAND &&	xSpeed < 0.0f){	xSpeed = -xSpeed; BlickRichtung = RECHTS;}	

	// Je nach Handlung richtig
	switch (Handlung)
	{
		case GEGNER_STEHEN:
		{
			// Hurri in der Nähe oder von einem Schuss getroffen ?
			// Dann losfliegen
			//
			if ((PlayerAbstandHoriz() < 240 && pAim->ypos > yPos) ||
				DamageTaken > 0)				
				Handlung  = GEGNER_LAUFEN;
		} break;

		case GEGNER_LAUFEN:					// Warten bis der Spieler nahe genug rankommt
		{			
			AnimSpeed = 0.5f;			
			AnimEnde  = 14;
			AnimStart = 0;
			Value1	= (int)pAim->xpos+rand()%80 - 30;			// Flugziel zufällig in Richtung Spieler
			Value2	= (int)pAim->ypos - 50 +rand()%100;	// setzen mit etwas Variation
			Handlung = GEGNER_VERFOLGEN;
		} break;

		case GEGNER_VERFOLGEN:					// Mücke verfolgt den Spieler
		{			
			// Punkt links
			if (Value1 < xPos)
			{
				if(!(blockl & BLOCKWERT_WAND))
				{
					xAcc =- 3.0;
					BlickRichtung = LINKS;
				}
			}

			// Punkt rechts
			if (Value1 > xPos)
			{
				if(!(blockr & BLOCKWERT_WAND))
				{
					xAcc = 3.0;
					BlickRichtung = RECHTS;
				}
			}

			// Punkt oben
			if (Value2 < yPos)
			{
				if (!(blocko & BLOCKWERT_WAND))
					yAcc =- 3.0;
				else
					ySpeed = 20;
			}

			// Punkt unterhalb
			if (Value2 > yPos)
			{
				if (!(blocku & BLOCKWERT_WAND))
					yAcc = 3.0;
				else
				{
					ySpeed = -10.0f;
					yAcc   = 0.0f;
				}
			}

			if (xSpeed >  10.0f) xSpeed = 10.0f;
			if (xSpeed < -10.0f) xSpeed =-10.0f;
			if (ySpeed >  10.0f) ySpeed = 10.0f;
			if (ySpeed < -10.0f) ySpeed =-10.0f;

			int dx = abs ((int) (xPos - Value1));
			int dy = abs ((int) (yPos - Value2));

			// Punkt erreicht oder Abstand zu groß ? Dann neues Ziel setzen
			//
			if (PlayerAbstand() > 300 ||
				(dx*dx + dy*dy) < 20*20)
				Handlung = GEGNER_LAUFEN;

			// An die Wand gekommen ? Dann auch neues Ziel setzen
			//
			if (blockl & BLOCKWERT_WAND ||
			    blockr & BLOCKWERT_WAND ||
				blocko & BLOCKWERT_WAND ||
				blocku & BLOCKWERT_WAND ||
				blocku & BLOCKWERT_PLATTFORM)
			{
				Handlung = GEGNER_LAUFEN;
				AnimPhase = 0;
			}

			// Wieder an der Decke?
			//
			static float DeckenCount = 0.0f;

			if (DeckenCount > 0.0f)
				DeckenCount -= 1.0f SYNC;

			if (DeckenCount <= 0.0f &&
				blocko & BLOCKWERT_WAND)
			{
				DeckenCount = 10.0f;
				Handlung  = GEGNER_STEHEN;
				AnimPhase = 15;
				AnimStart = 15;
				AnimEnde  = 21;
				AnimSpeed = 0.75f;
				xSpeed	  = 0.0f;
				ySpeed    = 0.0f;
				xAcc	  = 0.0f;
				yAcc      = 0.0f;
			}
		} break;

	}

	// Spieler berührt ?
	TestDamagePlayers(4.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Fledermaus explodiert
// --------------------------------------------------------------------------------------

void GegnerFledermaus::GegnerExplode(void)
{
	for (int i = 0; i < 100; i++)
		pPartikelSystem->PushPartikel(xPos + 10 + rand()%35, yPos + 10 + rand()%25, FUNKE);

	pPartikelSystem->PushPartikel(xPos - 5, yPos - 5, EXPLOSION_MEDIUM2);

	pSoundManager->PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);

	pPlayer[0]->Score += 100;
}
