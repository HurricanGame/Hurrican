// --------------------------------------------------------------------------------------
// Das Augen Extra
//
// fliegt Sinus förmig durchs Level und hinterlässt nach Zerstörung ein Extra
// welches Extra, das steht in Value1
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Auge.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerAuge::GegnerAuge(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	Energy			= 1;
	Value1			= Wert1;
	Value2			= Wert2;

	// Powerline ist Standard
	//
	if (Wert1 < 0 || Wert1 > 11)
		Wert1 = 7;


	smokedelay = 0.2f;
	ChangeLight		= Light;
	Destroyable		= true;
	xSpeed = 12.0f;

	ySpeed = 0.0f;
	yAcc   = 4.0f;
	AnimSpeed = 0.5f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerAuge::DoKI(void)
{
	// rauchen lassen
	//
	smokedelay -= 0.25f SYNC;

	if (smokedelay < 0.0f)
	{
		smokedelay = 0.1f;
		if (BlickRichtung == RECHTS)
			pPartikelSystem->PushPartikel (xPos +  8, yPos + 18, SMOKE2);
		else
			pPartikelSystem->PushPartikel (xPos + 42, yPos + 18, SMOKE2);
	}
	// Animieren (nur drehen)
	//
	if (Handlung != GEGNER_STEHEN)
	{
	
		AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
		if (Handlung == GEGNER_DREHEN)
		{
			if (AnimCount > AnimSpeed)			// Grenze überschritten ?
			{
				AnimCount = 0;					// Dann wieder auf Null setzen
				AnimPhase++;					// Und nächste Animationsphase
				if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
				{
					AnimCount = 0.0f;
					Handlung  = GEGNER_DREHEN2;
					BlickRichtung *= -1;
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
				if (AnimPhase <= 0)				// Animation von zu Ende	?
				{
					AnimPhase = 0;				// Dann wieder von vorne beginnen
					AnimEnde  = 0;
					Handlung  = GEGNER_LAUFEN;
				}
			}
		} 
	}

	if (ySpeed > 12.0f)
		yAcc = -4.0f;
	else
	if (ySpeed < -12.0f)
		yAcc = 4.0f;

	// An der Decke bzw dem Boden
	// abprallen
	//
	if (blocko & BLOCKWERT_WAND	   ||
		blocko & BLOCKWERT_GEGNERWAND)
	{
		yAcc   = -yAcc;
		ySpeed = -ySpeed;
	}

	if (blocku & BLOCKWERT_WAND	   ||
		blocku & BLOCKWERT_GEGNERWAND)
	{
		yAcc   = -yAcc;
		ySpeed = -ySpeed;
	}

	// Links oder rechts an der Wand abgeprallt ?
	//
	if ((xSpeed < 0.0f &&
		(blockl & BLOCKWERT_WAND	   ||
		 blockl & BLOCKWERT_GEGNERWAND)) ||
		(xSpeed > 0.0f &&
		(blockr & BLOCKWERT_WAND	   ||
		 blockr & BLOCKWERT_GEGNERWAND)))
	{
		Handlung = GEGNER_DREHEN;
		AnimPhase = 0;
		AnimCount = 0.0f;
		AnimEnde  = 5;
		xSpeed *= -1.0f;
	}

	// Spieler verletzen ?
	//
	/*if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt],
						pPlayer->xpos, pPlayer->ypos, pPlayer->CollideRect) == true)
		pPlayer->DamagePlayer(2.0f SYNC);*/
}

// --------------------------------------------------------------------------------------
// Auge explodiert
// --------------------------------------------------------------------------------------

void GegnerAuge::GegnerExplode(void)
{
	// Explosion
	for (int i=0; i<5; i++)
	pPartikelSystem->PushPartikel(float(xPos - 15) + rand()%20, 
								  float(yPos - 15) + rand()%40, EXPLOSION_MEDIUM2);

	pSoundManager->PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION1);	// Sound ausgeben

	pPlayer[0]->Score += 100;

	// Extra spawnen
	pGegner->PushGegner(xPos + 12, yPos + 12, EXTRAS, Value1, 0, true);
}
