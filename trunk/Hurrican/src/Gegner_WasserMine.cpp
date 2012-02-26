// --------------------------------------------------------------------------------------
// Die WasserMine
//
// Dümpelt im Wasser rum und wartet auf eine Kollision ;)
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_WasserMine.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerWasserMine::GegnerWasserMine(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	Energy			= 20;
	AnimSpeed		= 3.0f;
	ChangeLight		= Light;
	Destroyable		= false;
	AnimSpeed		= 4.0f;
	AnimEnde		= 10;
	Value1			= int (Wert1);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerWasserMine::DoKI(void)
{
	SimpleAnimation();

	// Spieler berührt ?
	TestDamagePlayers(50.0f, true);
}

// --------------------------------------------------------------------------------------
// WasserMine explodiert
// --------------------------------------------------------------------------------------

void GegnerWasserMine::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, 128, 8000, SOUND_EXPLOSION3);
	pSoundManager->PlayWave (100, 128,14000, SOUND_EXPLOSION4);

	ShakeScreen(5.0f);

	for (int i = 0; i < 50; i++)
		pPartikelSystem->PushPartikel(xPos - 5 + rand()%40, yPos - 5 + rand()%60, WATERFLUSH_HIGH);

	for (int i = 0; i < 25; i++)
		pPartikelSystem->PushPartikel(xPos - 5 + rand()%40, yPos - 50 + rand()%110, SPIDERSPLITTER);

	pPartikelSystem->PushPartikel(xPos - 40, yPos - 55, EXPLOSION_GIANT);
}
