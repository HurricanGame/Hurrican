// --------------------------------------------------------------------------------------
// Die Roboter Raupe
//
// kriecht auf den Spieler zu und schiesst in regelmässigen Abständen 3 Feuerbälle
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_RoboRaupe.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerRoboRaupe::GegnerRoboRaupe(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	Energy			= 60;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= true;
	AnimEnde		= 9;
	AnimSpeed		= 1.0f;
	BlickRichtung   = LINKS;
}

// --------------------------------------------------------------------------------------
// Bewegungs KI
// --------------------------------------------------------------------------------------

void GegnerRoboRaupe::DoKI(void)
{
	// animieren
	AnimCount += SpeedFaktor;		// Animationscounter weiterzählen
	if (AnimCount > AnimSpeed)			// Grenze überschritten ?
	{
		AnimCount = 0;					// Dann wieder auf Null setzen
		AnimPhase++;					// Und nächste Animationsphase
		if (AnimPhase > AnimEnde)		// Animation von zu Ende	?
			AnimPhase = AnimStart;

		if (AnimPhase == 5)				// Bewegen
		{
			if (BlickRichtung == LINKS)
				xPos -= 18.0f;
			else
				xPos += 18.0f;

			// evtl schiessen wenn Spieler zu nahe
			if (PlayerAbstand(true) < 200 && rand()%2 == 0)
			{				
				if (BlickRichtung == RECHTS)
				{
					pProjectiles->PushProjectile(xPos + 31, yPos + 10, ARCSHOT);

					WinkelUebergabe = 1.5f;
					pProjectiles->PushProjectile(xPos + 15, yPos + 10, ARCSHOTLEFT);					
					pProjectiles->PushProjectile(xPos + 51, yPos + 10, ARCSHOTRIGHT);
				}
				else
				{							
					pProjectiles->PushProjectile(xPos + 50, yPos + 10, ARCSHOT);

					WinkelUebergabe = 1.5f;
					pProjectiles->PushProjectile(xPos + 32, yPos + 10, ARCSHOTLEFT);
					pProjectiles->PushProjectile(xPos + 70, yPos + 10, ARCSHOTRIGHT);
				}

				pSoundManager->PlayWave(100, 128, 11025, SOUND_GRANATE);
			}
		}

	}

	// runterfallen ?
	if (!(blocku & BLOCKWERT_WAND) &&
		!(blocku & BLOCKWERT_PLATTFORM))
	{
		yAcc = 3.0f;

		if (ySpeed > 20.0f)
			ySpeed = 20.0f;
	}
	else
	{
		ySpeed = 0.0f;
		yAcc   = 0.0f;
	}

// umdrehen ?
	if (blockl & BLOCKWERT_WAND ||	blockl & BLOCKWERT_GEGNERWAND) BlickRichtung = RECHTS;
	if (blockr & BLOCKWERT_WAND ||	blockr & BLOCKWERT_GEGNERWAND) BlickRichtung = LINKS;

	// Testen, ob der Spieler die Raupe berührt hat
	TestDamagePlayers(5.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Explodieren
// --------------------------------------------------------------------------------------

void GegnerRoboRaupe::GegnerExplode(void)
{
	// blaue Explosionen erzeugen
	for (int i=0; i<50; i++)
		pPartikelSystem->PushPartikel(xPos + rand()%90-10, yPos + rand()%40 - 10, BLUE_EXPLOSION);

	for (int i=0; i<300; i++)
		pPartikelSystem->PushPartikel(xPos + rand()%90-10, yPos + rand()%25 + 5, ROCKETSMOKEBLUE);
		
	pSoundManager->PlayWave(100, 128, 11025 + rand()%2000, SOUND_EXPLOSION3);	// Sound ausgeben

	pPlayer[0]->Score += 250;
}
