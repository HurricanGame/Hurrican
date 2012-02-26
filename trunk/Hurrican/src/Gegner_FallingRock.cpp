// --------------------------------------------------------------------------------------
// Der fallende Felsblock
//
// ruht in der Decke und wartet bis der Spieler drunter durchläuft, umd dann runter
// zu fallen
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_FallingRock.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFallingRock::GegnerFallingRock(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	Energy			= 10;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= false;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFallingRock::DoKI(void)
{
// Je nach Handlung richtig verhalten
	switch (Handlung)
	{
		case GEGNER_FALLEN:						// Stein fällt runter
		{
			// Maximale Geschwindigkeitsbegrenzung
			if (ySpeed > 50.0f)
				yAcc = 0.0f;

			// Spieler berührt Stein
			TestDamagePlayers(40.0f, true);

			// Stein ist am Boden gelandet
			if (ySpeed > 10.0f &&
				(blocku & BLOCKWERT_WAND ||
				 blocku & BLOCKWERT_PLATTFORM))
			{
				Energy = 0;
			}
		} break;

		case GEGNER_LAUFEN:						// Stein wartet bis der Spieler vorbeikommt
		{
			for (int p = 0; p < NUMPLAYERS; p++)
				if (pPlayer[p]->xpos + pPlayer[p]->CollideRect.right > xPos &&
					pPlayer[p]->xpos < xPos + 80 &&
					pPlayer[p]->ypos > yPos &&
					PlayerAbstand() < 480)
			{
				Active = true;

				// Rauch erzeugen wo der Stein die Decke verlässt
				for (int i=0; i<3; i++)
				{
					pPartikelSystem->PushPartikel(xPos-10, yPos + i*10, SMOKE);
					pPartikelSystem->PushPartikel(xPos+63, yPos + i*10, SMOKE);
				}

				// Sound ausgeben
				pSoundManager->PlayWave(100, 128, 11025 + rand()%2000, SOUND_STONEFALL);

				Handlung = GEGNER_FALLEN;
				ySpeed =  0.0f;
				yAcc   =  12.0f;
			}
		} break;
		
		default : break;
	} // switch
}

// --------------------------------------------------------------------------------------
// FallingRock explodiert
// --------------------------------------------------------------------------------------

void GegnerFallingRock::GegnerExplode(void)
{
	// und Splitter erzeugen Rauch
	for (int i=0; i<10; i++)
	{
		pPartikelSystem->PushPartikel(xPos + rand()%80-12, yPos + rand()%20 + 20, SMOKE);
		pPartikelSystem->PushPartikel(xPos + rand()%80-12, yPos + rand()%40, ROCKSPLITTER);
		pPartikelSystem->PushPartikel(xPos + rand()%80-12, yPos + rand()%40, ROCKSPLITTERSMALL);
	}
	pSoundManager->PlayWave(100, 128, 11025 + rand()%2000, SOUND_STONEEXPLODE);	// Sound ausgeben

	pPlayer[0]->Score += 100;
}
