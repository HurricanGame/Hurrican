// --------------------------------------------------------------------------------------
// Der Manfred Trenz Shrine
//
// kniet man davor nieder, regnet es Extras
// schiesst man darauf, regent es Steine
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_Shrine.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerShrine::GegnerShrine(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	BlickRichtung	= RECHTS;
	Energy			= 100;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= false;
	Value1			= 1;
	AnimCount	    = 0.0f;

	pTileEngine->MaxOneUps += 2;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerShrine::DoKI(void)
{
	BlickRichtung	= LINKS;

	if (AnimCount > 0.0f)
		AnimCount -= 1.0f SYNC;
	else
		AnimCount = 0.0f;

/*
	// Drauf geschossen ? Dann Steine runterfallen lassen
	//
	if (Energy < 100.0f)
	{
		Energy = 100.0f;

		if (AnimCount == 0.0f)
		{
			pGegner->PushGegner (pPlayer[0]->xpos, 
								 pPlayer[0]->ypos - 50, 
								 FALLINGROCK, 0, 0, true);

			if (NUMPLAYERS == 2)
			pGegner->PushGegner (pPlayer[1]->xpos, 
								 pPlayer[1]->ypos - 50, 
								 FALLINGROCK, 0, 0, true);

			pSoundManager->PlayWave (100, 128, 11025, SOUND_PHARAODIE);

			AnimCount = 5.0f;
		}
	}
*/
	// Spieler kniet nieder
	//
	for (int p = 0; p < NUMPLAYERS; p++)
	if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt],
						pPlayer[p]->xpos, pPlayer[p]->ypos, pPlayer[p]->CollideRect) == true &&
						pPlayer[p]->Handlung == DUCKEN &&
						Value1 == 1)
	{
		Value1 = 0;
		for (int i = 0; i < 2; i++)
			pGegner->PushGegner (float (xPos - 20 + i * 110), yPos - 100, ONEUP, 0, 0, true);

		for (int i = 0; i < 8; i++)
			pGegner->PushGegner (xPos + rand()%100, yPos - 50 + rand()%100, EXTRAS, 3, 0, true);

		pSoundManager->PlayWave (100, 128, 11025, SOUND_COLLECT);
		pSoundManager->PlayWave (100, 128, 11025, SOUND_COLLECT);
		pSoundManager->PlayWave (100, 128, 11025, SOUND_COLLECT);
		pSoundManager->PlayWave (100, 128, 11025, SOUND_COLLECT);
		pSoundManager->PlayWave (100, 128, 11025, SOUND_COLLECT);
		pSoundManager->PlayWave (100, 128, 11025, SOUND_COLLECT);
	}
}

// --------------------------------------------------------------------------------------
// Shrine explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerShrine::GegnerExplode(void)
{
}
