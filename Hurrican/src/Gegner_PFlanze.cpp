// --------------------------------------------------------------------------------------
// Die Super-Mario Pflanze
//
// Spuckt Feuerbälle, wenn der Spieler in der Nähe ist
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Pflanze.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPflanze::GegnerPflanze(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	HitSound		= 1;
	Energy			= 80;
	AnimPhase		= rand()%8;
	AnimEnde		= 9;
	AnimSpeed = 2.2f - Skill * 0.4f;
	AnimCount		= 0.0f;
	ChangeLight		= Light;
	Destroyable		= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerPflanze::DoKI(void)
{
	// Animieren
	//
	AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
	if (AnimCount > AnimSpeed)			// Grenze überschritten ?
	{
		AnimCount = 0;					// Dann wieder auf Null setzen
		AnimPhase++;					// Und nächste Animationsphase

		if (AnimPhase == 14)			// Schuss abgeben
		{
			if (BlickRichtung == RECHTS)
				pProjectiles->PushProjectile (xPos + 84, yPos + 16, PFLANZESHOT, pAim);
			else
				pProjectiles->PushProjectile (xPos + 12, yPos + 16, PFLANZESHOT, pAim);
		}

		if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
		{
			// Schiessen ?
			//
			if (RunningTutorial == false &&
				(PlayerAbstand() < 300 || rand()%2 == 0))
			{
				Handlung  = GEGNER_SCHIESSEN;
				AnimEnde  = 17;
				AnimSpeed = 2.2f - Skill * 0.4f;
				AnimPhase = 9;
			}

			// oder normal wippen
			//
			else 
			{
				Handlung  = GEGNER_LAUFEN;
				AnimEnde  = 8;
				AnimSpeed = 1.8f;
				AnimPhase = 0;
			}
		}
	}

	// In Richtung Spieler kucken
	//
	if (pAim->xpos + 35 < xPos + 55)
		BlickRichtung = LINKS;
	else
		BlickRichtung = RECHTS;


	switch (Handlung)
	{
		// Nur rumwackeln
		//
		case GEGNER_LAUFEN:
		break;
	}

	// Testen, ob der Spieler die Pflanze berührt hat
	//
	TestDamagePlayers(1.5f SYNC);
}


// --------------------------------------------------------------------------------------
// Pflanze explodiert
// --------------------------------------------------------------------------------------

void GegnerPflanze::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);

	for (int i = 0; i < 5; i++)
		pPartikelSystem->PushPartikel(xPos + rand()%50, yPos + rand()%40, EXPLOSION_GREEN);

	for (int i = 0; i < 30; i++)
		// Blätter erzeugen
		pPartikelSystem->PushPartikel(float(xPos + rand()%100), 
									  float(yPos + 10 +  rand()%50), BLATT);


	pPlayer[0]->Score += 300;
}
