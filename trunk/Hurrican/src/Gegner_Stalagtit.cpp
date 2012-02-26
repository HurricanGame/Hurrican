// --------------------------------------------------------------------------------------
// Der Stalagtit
//
// Wartet wie der fallende Stein, bis der Spieler drunter vorbei läuft, und fällt dann
// runter
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Stalagtit.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerStalagtit::GegnerStalagtit(int Wert1, int Wert2, bool Light)
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

void GegnerStalagtit::DoKI(void)
{
// Je nach Handlung richtig verhalten
	switch (Handlung)
	{
		case GEGNER_FALLEN:						// Stein fällt runter
		{
			// Maximale Geschwindigkeitsbegrenzung
			if (ySpeed > 50.0f)
				yAcc = 0.0f;

			TestDamagePlayers(50.0f, true);

			// Stein ist am Boden gelandet
			if (blocku & BLOCKWERT_WAND)
			{
				Energy = 0;
			}
		} break;

		case GEGNER_LAUFEN:						// Stein wartet bis der Spieler vorbeikommt
		{
			if (pAim->xpos + pAim->CollideRect.right > xPos &&
				pAim->xpos < xPos + 80 &&
				pAim->ypos > yPos &&
				PlayerAbstand() < 640)
			{
				// Partikel erzeugen wo der Stein die Decke verlässt
				for (int i=0; i<3; i++)
				{
					pPartikelSystem->PushPartikel(xPos+i*10, yPos + 30, ROCKSPLITTERSMALLBLUE);
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
// Stalagtit explodiert
// --------------------------------------------------------------------------------------

void GegnerStalagtit::GegnerExplode(void)
{
// und Splitter erzeugen Rauch
	for (int i=0; i<20; i++)
	{
		pPartikelSystem->PushPartikel(xPos + rand()%40-8, yPos + rand()%80 - 8, SMOKE);
		pPartikelSystem->PushPartikel(xPos + rand()%40-8, yPos + rand()%80 - 8, ROCKSPLITTERSMALLBLUE);
	}
	pSoundManager->PlayWave(100, 128, 11025 + rand()%2000, SOUND_STONEEXPLODE);	// Sound ausgeben

	pPlayer[0]->Score += 100;
}
