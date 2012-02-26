// --------------------------------------------------------------------------------------
// Das Wespennest
//
// Bringt in unregelmässigen Abständen Wespen hervor
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Nest.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerNest::GegnerNest(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	Energy			= 120;
	AnimSpeed		= 1.0f;
	ChangeLight		= Light;
	Destroyable		= true;
	AnimCount		= 50.0f;
	Value1			= Wert1;
	Value2			= Wert2;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerNest::DoKI(void)
{
	// Wespe spawnen?
	//

	if (PlayerAbstand() < 800 &&
		Value2 != 99)				// Im Tutorial keine Wespen spawnen
	{
		AnimCount -= 1.0f SYNC;

		if (AnimCount <= 0.0f)
		{
			pGegner->PushGegner (xPos + 20, yPos - 10, STAHLMUECKE, 0, 0, false);
			AnimCount = float (rand()%30 + 30);
		}
	}
	

	// Testen, ob der Spieler das Nest berührt
	//
	TestDamagePlayers(0.5f SYNC);
}


// --------------------------------------------------------------------------------------
// Nest explodiert
// --------------------------------------------------------------------------------------

void GegnerNest::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION4);

	for (int i = 0; i < 5; i++)
		pPartikelSystem->PushPartikel(xPos - 25 + rand()%20, yPos - 30 + rand()%50, EXPLOSION_MEDIUM2);

	pPartikelSystem->PushPartikel(xPos - 5, yPos - 5, NESTLUFT);

	pPlayer[0]->Score += 500;
}
