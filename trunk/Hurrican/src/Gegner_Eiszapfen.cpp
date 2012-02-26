// --------------------------------------------------------------------------------------
// Der Eiszapfen
//
// hängt faul an der Decke und fällt runter, wenn der Hurri drunter vorbeikommt
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Eiszapfen.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerEiszapfen::GegnerEiszapfen(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	Energy			= 100;
	ChangeLight		= Light;
	Destroyable		= false;
	Value1			= Wert1;
	Value2			= Wert2;	

	// schon vorher runterfallen? (bei Eisfaust Boss)
	//
	if (Value1 == 1.0f)
	{
		Handlung = GEGNER_FALLEN;
		ySpeed = 30.0f;
		yAcc   = 5.0f;

		pSoundManager->PlayWave(100, 128, 11025 + rand()%2000, SOUND_STONEFALL);
	}
}	

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerEiszapfen::DoKI(void)
{
	SimpleAnimation();

	switch (Handlung)
	{
		case GEGNER_STEHEN:
		{
			if (pAim->ypos > yPos &&
				pAim->xpos + 35 > xPos + 10 - 60 &&
				pAim->xpos + 35 < xPos + 10 + 60)
			{
				Handlung = GEGNER_FALLEN;
				ySpeed = 30.0f;
				yAcc   = 5.0f;

				pSoundManager->PlayWave(100, 128, 11025 + rand()%2000, SOUND_STONEFALL);

				
				for (int i=0; i < 15; i++)
				{
					pPartikelSystem->PushPartikel (xPos - 20 + rand ()%35, yPos - 10 + rand()%20, WATERFLUSH2);
					pPartikelSystem->PushPartikel (xPos - 20 + rand ()%35, yPos - 10 + rand()%20, SMOKE);
				}
			}
		} break;

		case GEGNER_FALLEN:
		{
			if (ySpeed > 40.0f)
			{
				ySpeed = 40.0f;
				yAcc   = 0.0f;
			}

			if (blocku & BLOCKWERT_WAND)
				Energy = 0.0f;

		} break;

		default : break;
	} // switch

	// Testen, ob der Spieler den Zapfen berührt hat
	TestDamagePlayers(30.0f, true);
}

// --------------------------------------------------------------------------------------
// Eiszapfen explodiert
// --------------------------------------------------------------------------------------

void GegnerEiszapfen::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, 128, 11025, SOUND_EXPLOSION1);

	for (int i=0; i < 30; i++)	
	{
		pPartikelSystem->PushPartikel (xPos - 20 + rand ()%35, yPos - 10 + rand()%60, WATERFLUSH2);
		pPartikelSystem->PushPartikel (xPos - 20 + rand ()%35, yPos - 10 + rand()%60, SMOKE);
	}

	pPlayer[0]->Score += 100;
}
