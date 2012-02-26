// --------------------------------------------------------------------------------------
// Die Rakete, die am Rand hängt
//
// sobald der Hurri in die Nähe kommt, läuft der Time ab. Steht er auf null,
// fliegt sie los
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_SideRocket.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSideRocket::GegnerSideRocket(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_INIT;
	Energy			= 50;
	ChangeLight		= Light;
	Destroyable		= true;
	Value1			= Wert1;

	if (Wert2 == 0)	Value2 = 10; else		// 10 = Default Wert für die Verzögerung
					Value2 = Wert2;	

}	

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSideRocket::DoKI(void)
{
	if (Value1 == 0)
		BlickRichtung = LINKS;
	else
		BlickRichtung = RECHTS;

	switch (Handlung)
	{
		// Jaeger wird "aktiviert", sprich, der Timer wird gestartet
		//
		case GEGNER_INIT:
		{
			AnimCount = float (Value2);

			if (PlayerAbstandVert()  < 100 &&
			    PlayerAbstandHoriz() < 400 &&
			   (Value1 == 0 && xPos > pAim->xpos ||
			    Value1 != 0 && xPos < pAim->xpos))
			Handlung  = GEGNER_STEHEN;
		} break;

		// Timer läuft ab
		//
		case GEGNER_STEHEN:
		{
			AnimCount -= 1.0f SYNC;

			// Timer zu ende? Dann Rakete starten
			//
			if (AnimCount <= 0.0f)
			{
				AnimCount = 0.1f;
				Handlung  = GEGNER_LAUFEN;
				pSoundManager->PlayWave(75, 128, 8000 + rand ()%4000, SOUND_ROCKET);

				if (Value1 == 0)
				{
					xSpeed = -20.0f;
					xAcc   = -5.0f;
				}
				else
				{
					xSpeed = 20.0f;
					xAcc   = 5.0f;
				}

			}

		} break;

		// Rakete fliegt
		//
		case GEGNER_LAUFEN:
		{
			if (xSpeed < -30.0f) xSpeed = -30.0f; else
			if (xSpeed >  30.0f) xSpeed =  30.0f;
			

			// Antriebspartikel erzeugen
			//
			AnimCount -= 1.0f SYNC;

			while (AnimCount <= 0.0f)
			{
				AnimCount += 0.05f;

				if (Value1 == 0)
					pPartikelSystem->PushPartikel (xPos + 35, yPos + 3, ROCKETSMOKE);
				else
					pPartikelSystem->PushPartikel (xPos - 5, yPos + 3, ROCKETSMOKE);
			}

			// Gegen die Wand geflogen? Dann explodieren lassen
			//
			if ((Value1 == 0 &&	 blockl & BLOCKWERT_WAND) ||
				(Value1 == 1 &&	 blockr & BLOCKWERT_WAND))

				Energy = 0.0f;

		} break;

		default : break;
	} // switch


	// Spieler getroffen?
	// Dann explodieren lassen und Energy abziehen
	//
	TestDamagePlayers(25.0f, true);
}

// --------------------------------------------------------------------------------------
// SideRocket explodiert
// --------------------------------------------------------------------------------------

void GegnerSideRocket::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, 128, 8000 + rand ()%4000, SOUND_EXPLOSION1);

	pPartikelSystem->PushPartikel (xPos - 10, yPos - 20, EXPLOSION_MEDIUM2);

	for (int i=0; i < 10; i++)	
	{
		pPartikelSystem->PushPartikel (xPos + rand ()%20, yPos - 10 + rand()%20, MINIFLARE);
		pPartikelSystem->PushPartikel (xPos + rand ()%20, yPos - 10 + rand()%20, SPIDERSPLITTER);
	}

	pPlayer[0]->Score += 50;
}
