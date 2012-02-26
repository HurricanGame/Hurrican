// --------------------------------------------------------------------------------------
// Die Spitter Bombe
//
// Bombe, aus der der Spitter rauskommt.
// geile Erklärung, oder ? ;)
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Spitterbombe.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSpitterbombe::GegnerSpitterbombe(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	Energy			= 20;
	AnimEnde		= 6;
	AnimSpeed		= 0.5;
	ChangeLight		= Light;
	Destroyable		= true;
	ySpeed			= 15.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSpitterbombe::DoKI(void)
{
	// Animieren
	//
	AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
	if (AnimCount > AnimSpeed)			// Grenze überschritten ?
	{
		AnimCount = 0;					// Dann wieder auf Null setzen
		AnimPhase++;					// Und nächste Animationsphase

		pPartikelSystem->PushPartikel (xPos - 5 + rand()%10, yPos, SMOKE);

		if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
			AnimPhase = AnimStart;
	}

	// Testen, ob der Spieler die Bombe berührt hat
	//
	TestDamagePlayers(1.0f SYNC);

	// Auf den Boden aufgekommen ? Dann Spitter rauslassen
	if ((blocku & BLOCKWERT_WAND) ||
		(blocku & BLOCKWERT_PLATTFORM))
	{
		Energy = 0.0f;
		pGegner->PushGegner (xPos + 4, yPos + 20, SPITTER, 0, 0, false);
	}
}


// --------------------------------------------------------------------------------------
// Bombe explodiert
// --------------------------------------------------------------------------------------

void GegnerSpitterbombe::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);

	for (int i = 0; i < 5; i++)
		pPartikelSystem->PushPartikel(xPos - 25 + rand()%20, yPos - 30 + rand()%50, EXPLOSION_MEDIUM2);

	for (int i = 0; i < 5; i++)
		pPartikelSystem->PushPartikel(xPos + rand()%20, yPos + rand()%40, SPIDERSPLITTER);

	// Nicht am Boden explodiert ? Dann wurde er abgeschossen
	if (!(blocku & BLOCKWERT_WAND) &&
		!(blocku & BLOCKWERT_PLATTFORM))
	{
		pProjectiles->PushProjectile (xPos + 10, yPos + 30, SPITTERBOMBESHOTLO);
		pProjectiles->PushProjectile (xPos + 10, yPos + 30, SPITTERBOMBESHOTLM);
		pProjectiles->PushProjectile (xPos + 10, yPos + 30, SPITTERBOMBESHOTLU);
		pProjectiles->PushProjectile (xPos + 10, yPos + 30, SPITTERBOMBESHOTRO);
		pProjectiles->PushProjectile (xPos + 10, yPos + 30, SPITTERBOMBESHOTRM);
		pProjectiles->PushProjectile (xPos + 10, yPos + 30, SPITTERBOMBESHOTRU);
		pPlayer[0]->Score += 200;
	}
}
