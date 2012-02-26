// --------------------------------------------------------------------------------------
// Die kleine Drone
//
// Umkreist den Spieler und Ballert auf ihn
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_FieseDrone.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFieseDrone::GegnerFieseDrone(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	Energy			= 70;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= true;
	xSpeed			= 0.0f;
	ySpeed			= 0.0f;
	yAcc			= 0.0f;
	AnimSpeed       = 1.5f;
	AnimEnde		= 12;
	ShotDelay = (float)(rand()%12 + 12);
	SmokeDelay		= 0.0f;	
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFieseDrone::DoKI(void)
{
	static int flamex = 0;

	// Animieren
	//
	AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
	if (Handlung == GEGNER_LAUFEN)
	{
		if (AnimCount > AnimSpeed)			// Grenze überschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen
			AnimPhase++;					// Und nächste Animationsphase
			if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
				AnimPhase = AnimStart;		// Dann wieder von vorne beginnen
		}
	} 
	else
	if (Handlung == GEGNER_DREHEN)
	{
		if (AnimCount > AnimSpeed)			// Grenze überschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen
			AnimPhase++;					// Und nächste Animationsphase
			if (AnimPhase > AnimEnde)		// Animation von zu Ende	?
			{
				AnimCount = 0.0f;
				Handlung  = GEGNER_DREHEN2;
				BlickRichtung *= -1;
				xPos -= BlickRichtung * 5.0f;
				AnimPhase = AnimEnde;
			}
		}
	} 
	else
	if (Handlung == GEGNER_DREHEN2)
	{
		if (AnimCount > AnimSpeed)			// Grenze überschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen
			AnimPhase--;					// Und nächste Animationsphase
			if (AnimPhase <= 13)				// Animation von zu Ende	?
			{
				AnimPhase = 0;				// Dann wieder von vorne beginnen
				AnimStart = 0;
				AnimEnde  = 12;
				Handlung  = GEGNER_LAUFEN;
				AnimSpeed = 1.5f;
			}
		}
	}

	//in Richtung Spieler fliegen
	//
	if (xPos + 30 < pAim->xpos + 35) xAcc =  1.0f;
	if (xPos + 30 > pAim->xpos + 35) xAcc = -1.0f;

	if (yPos < pAim->ypos - 100) yAcc =  1.0f;
	if (yPos > pAim->ypos - 100) yAcc = -1.0f;

	if (xSpeed >  10.0f) xSpeed =  10.0f;
	if (xSpeed < -10.0f) xSpeed = -10.0f;
	if (ySpeed >  8.0f) ySpeed =  8.0f;
	if (ySpeed < -8.0f) ySpeed = -8.0f;

	if (Handlung == GEGNER_LAUFEN &&
		PlayerAbstand() < 600)
	{
		if (ShotDelay > 0.0f) 
			ShotDelay -= 1.0f SYNC;
		else
		{
			ShotDelay = (float)(rand()%12 + 12);

			float xdiv, ydiv, newwinkel;

			ydiv = (pAim->ypos + 40) - (yPos + 20);

			if (ydiv == 0.0f)
				ydiv  = 0.00001f;

			xdiv = (pAim->xpos + 35) - (xPos + 20);	

			newwinkel = (float)atan(xdiv / ydiv) * 180.0f / D3DX_PI + 180.0f;

			if (xdiv >= 0 && ydiv >= 0) newwinkel = newwinkel;		  else
			if (xdiv > 0  && ydiv < 0 ) newwinkel = 180 + newwinkel;  else
			if (xdiv < 0  && ydiv > 0 ) newwinkel = 360 + newwinkel;  else
			if (xdiv < 0  && ydiv < 0 ) newwinkel = 180 + newwinkel;

			WinkelUebergabe = 360.0f - newwinkel;
			
			pProjectiles->PushProjectile(xPos + 20, yPos + 20, FLUGLASER);
			pPartikelSystem->PushPartikel(xPos + 20, yPos + 20, SMOKE);
			pSoundManager->PlayWave(50, 128, 25000 + rand()%5000, SOUND_LASERSHOT);
		}
	}

	if ((blocko & BLOCKWERT_WAND ||blocko & BLOCKWERT_GEGNERWAND) && ySpeed < 0.0f) ySpeed *= 1.0f;
	if ((blocku & BLOCKWERT_WAND ||blocku & BLOCKWERT_GEGNERWAND) && ySpeed > 0.0f) ySpeed *= 1.0f;
	if ((blockl & BLOCKWERT_WAND ||blockl & BLOCKWERT_GEGNERWAND) && xSpeed < 0.0f) xSpeed *= 1.0f;
	if ((blockr & BLOCKWERT_WAND ||blockr & BLOCKWERT_GEGNERWAND) && xSpeed > 0.0f) xSpeed *= 1.0f;

	switch (Handlung)
	{
		// In der Luft rumdümpeln
		//
		case GEGNER_LAUFEN:
		{
			if ((xPos + 30 < pAim->xpos + 35 && BlickRichtung == LINKS) ||
				(xPos + 30 > pAim->xpos + 35 && BlickRichtung == RECHTS))
			{
				Handlung = GEGNER_DREHEN;
				AnimPhase = 13;
				AnimStart = 13;
				AnimCount = 0.0f;
				AnimEnde  = 17;
				AnimSpeed = 0.5f;
			}

		}	break;

	}

	// Rauch
	//
	if (Handlung == GEGNER_LAUFEN)
	{
		if (BlickRichtung == LINKS)
			flamex = 31; 
		else
			flamex = -12; 
	}
	else
	{
		if (BlickRichtung == LINKS)
			flamex = 31 - (AnimPhase - 13) * 5;
		else
			flamex = -12 + (AnimPhase - 13) * 5;

	}

	SmokeDelay -= 1.0f SYNC;
	
	while (SmokeDelay < 0.0f)
	{
		SmokeDelay += 0.7f;

		pPartikelSystem->PushPartikel (xPos + flamex, yPos + 28, SMOKE);
	}

	// Testen, ob der Spieler die Drone berührt hat
	TestDamagePlayers(1.0f SYNC);
}

// --------------------------------------------------------------------------------------
// FieseDrone explodiert
// --------------------------------------------------------------------------------------

void GegnerFieseDrone::GegnerExplode(void)
{
	for (int i=0; i<5; i++)
		pPartikelSystem->PushPartikel(float(xPos - 25 + rand()%40), 
									  float(yPos - 25 + rand()%40), EXPLOSION_MEDIUM2);
	for (int i=0; i<20; i++)
		pPartikelSystem->PushPartikel(float(xPos + 10 + rand()%30), 
									  float(yPos + 10 + rand()%30), LASERFUNKE2);

	pSoundManager->PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION1);	// Sound ausgeben

	pPlayer[0]->Score += 80;
}
