// --------------------------------------------------------------------------------------
// Die kleine Wespe
//
// fliegt ein wenig verwirrter durch die Gegend als die Wespe
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_SmallWespe.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSmallWespe::GegnerSmallWespe(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	AnimStart		= 0;
	AnimEnde		= 4;
	AnimSpeed		= 0.5f;
	xSpeed			= 0.0f;
	ySpeed			= 0.0f;
	xAcc			= 0.0f;
	yAcc			= 0.0f;
	Energy			= 50;
	Value1			= (int)pAim->xpos-100+rand()%200;	// Flugziel zufällig in Richtung Spieler
	Value2			= (int)pAim->ypos-100+rand()%50;		// setzen mit etwas Variation
	ChangeLight		= Light;
	Destroyable		= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSmallWespe::DoKI(void)
{
	static float MAXSPEED = 15.0f;

	SimpleAnimation();

// Je nach Handlung richtig verhalten
	switch (Handlung)
	{
		case GEGNER_LAUFEN:					// Warten bis der Spieler nahe genug rankommt
		{
			if (PlayerAbstand() < 500)
			{
				Value1	= (int)pAim->xpos+35-50+rand()%100;	// Flugziel zufällig in Richtung Spieler
				Value2	= (int)pAim->ypos+40-50+rand()%30;	// setzen mit etwas Variation
				Handlung = GEGNER_VERFOLGEN;
			}
		} break;

		case GEGNER_VERFOLGEN:					// Wespe verfolgt den Spieler
		{
			// Punkt links von der Wespe
			if (Value1 < xPos)
			{
				if(!(blockl & BLOCKWERT_WAND))
				{
					xAcc =- 3.0;
					BlickRichtung = LINKS;
				}
			}

			// Punkt rechts von der Wespe
			if (Value1 > xPos)
			{
				if(!(blockr & BLOCKWERT_WAND))
				{
					xAcc = 3.0;
					BlickRichtung = RECHTS;
				}
			}

			// Punkt oben von der Wespe
			if (Value2 < yPos)
			{
				if (!(blocko & BLOCKWERT_WAND))
					yAcc =- 3.0;
				else
				{
					Handlung = GEGNER_VERFOLGEN;
					ySpeed = 20;
				}
			}

			// Punkt unterhalb von der Wespe
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

			if (xSpeed >  MAXSPEED) xSpeed =  MAXSPEED;
			if (xSpeed < -MAXSPEED) xSpeed = -MAXSPEED;
			if (ySpeed >  MAXSPEED) ySpeed =  MAXSPEED;
			if (ySpeed < -MAXSPEED) ySpeed = -MAXSPEED;

			int dx = abs ((int) (xPos - Value1));
			int dy = abs ((int) (yPos - Value2));

			// nah am Spieler "zuschlagen" ?
			if (PlayerAbstand() < 200)
				MAXSPEED = 20.0f;
			else
				MAXSPEED = 8.0f;

			// Punkt erreicht oder Abstand zu groß ? Dann neues Ziel setzen
			if (PlayerAbstand() > 400 ||
				(dx*dx + dy*dy) < 20*20)

			{
				Value1	= (int)pAim->xpos-50+rand()%100;	// Flugziel zufällig in Richtung Spieler
				Value2	= (int)pAim->ypos-50+rand()%100;	// setzen mit etwas Variation
			}

			// An die Wand gekommen ? Dann auch neues Ziel setzen
			if (blockl & BLOCKWERT_WAND ||
			    blockr & BLOCKWERT_WAND ||
				blocko & BLOCKWERT_WAND ||
				blocku & BLOCKWERT_WAND ||
				blocku & BLOCKWERT_PLATTFORM)
			{
				Value1	= (int)pAim->xpos+35-50+rand()%100;	// Flugziel zufällig in Richtung Spieler
				Value2	= (int)pAim->ypos+40-50+rand()%30;	// setzen mit etwas Variation
			}
		} break;

		// Wespe stürzt ab
		case GEGNER_FALLEN:
		{
			// An die Wand gekracht ?
			if (blockl & BLOCKWERT_WAND ||
			    blockr & BLOCKWERT_WAND ||
				blocko & BLOCKWERT_WAND ||
				blocku & BLOCKWERT_WAND ||
				blocku & BLOCKWERT_PLATTFORM)
				Energy = 0.0f;

			// Grenze der Fallgeschwindigkeit
			if (ySpeed > 35.0f)
				yAcc = 0.0f;

			// Wespe rauchen lassen
			if (AnimCount == 0.0f && AnimPhase%2 == 0)
				pPartikelSystem->PushPartikel(xPos+12, yPos+5, SMOKE);
		} break;

		default : break;
	} // switch

	// Testen, ob die Wespe einen Spieler berührt hat. Wenn ja, dann prallt sie ab und wirft den Spieler etwas zurück
	if (Handlung != GEGNER_FALLEN)
	for (int p = 0; p < NUMPLAYERS; p++)
	if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt],
						pPlayer[p]->xpos,
						pPlayer[p]->ypos,
						pPlayer[p]->CollideRect) == true)
	{
		//Wegschieben(GegnerRect[GegnerArt], 0.0f);
		if (xPos + 25 > pPlayer[p]->xpos + 35 &&
			BlickRichtung == LINKS &&
			xSpeed < -4.0f)
		{
			xSpeed = 16.0f;			
			pPlayer[p]->DamagePlayer(8.0f);
		}
		else		
		if (xPos + 25 < pPlayer[p]->xpos + 35 &&
			BlickRichtung == RECHTS &&
			xSpeed > 4.0f)
		{			
			xSpeed = -16.0f;
			pPlayer[p]->DamagePlayer(8.0f);
		}
	}
		

	// beim Abstürzen rot
	//
	if (Handlung == GEGNER_FALLEN)
		DamageTaken = 192.0f;


	// Soviel Energie verloren, dass die Wespe abstürzt ?
	if (Energy <= 0.0f && Handlung != GEGNER_FALLEN)
	{
		Handlung = GEGNER_FALLEN;
		Energy	 = 30.0f;
		ySpeed   = 3.0f;
		xAcc	 = 0.0f;
		yAcc	 = 2.0f;

		pPartikelSystem->PushPartikel(float(xPos + 2), float(yPos - 10), EXPLOSION_MEDIUM);

		for (int i = 0; i < 5; i++)
			pPartikelSystem->PushPartikel(float(xPos + 2) + rand()%20, float(yPos - 10) + rand()%10, SMOKE2);

		if (BlickRichtung == LINKS)
			xSpeed = -5.0f;
		else
			xSpeed = 5.0f;
	}
}

// --------------------------------------------------------------------------------------
// Explodieren
// --------------------------------------------------------------------------------------

void GegnerSmallWespe::GegnerExplode(void)
{
	// Explosion
	pPartikelSystem->PushPartikel(float(xPos + 2), 
								  float(yPos - 10), EXPLOSION_MEDIUM);

	// Rauchende Splitter
	for (int i=0; i<3; i++)
		pPartikelSystem->PushPartikel(float(xPos + 15), 
									  float(yPos + 20), SPLITTER);


	pSoundManager->PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION1);	// Sound ausgeben

	pPlayer[0]->Score += 200;
}
