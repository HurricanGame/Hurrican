// --------------------------------------------------------------------------------------
// Der Spinnen Eiermann
//
// Legt Eier die auf Hurri zurollen und dann explodieren
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_EierMann.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerEierMann::GegnerEierMann(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	Energy			= 50;
	Value1			= Wert1;
	Value2			= Wert2;
	AnimStart		= 0;
	AnimEnde		= 10;

	if (Skill == 0) AnimSpeed = 1.9f;
	if (Skill == 1) AnimSpeed = 1.6f;
	if (Skill == 2) AnimSpeed = 1.3f;
	if (Skill == 3) AnimSpeed = 1.0f;

	ChangeLight		= Light;
	Destroyable		= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerEierMann::DoKI(void)
{
	// Animieren
	if (AnimEnde > 0)						// Soll überhaupt anmiert werden ?
	{
		AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
		if (AnimCount > AnimSpeed)			// Grenze überschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen
			AnimPhase++;					// Und nächste Animationsphase
			if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
			{
				AnimPhase = AnimStart;		// Dann wieder von vorne beginnen

				if (PlayerAbstand () <= 700)
					pProjectiles->PushProjectile (xPos + 16, yPos + 40, EIERBOMBE, pAim);
			}
		}
	} // animieren

	// Je nach Handlung richtig verhalten
	//
	switch (Handlung)
	{
		case GEGNER_LAUFEN :
		{
		} break;

		default :
		break;

	} // switch
}

// --------------------------------------------------------------------------------------
// EierMann explodiert
// --------------------------------------------------------------------------------------

void GegnerEierMann::GegnerExplode(void)
{
	pPartikelSystem->PushPartikel (xPos, yPos, EXPLOSION_GIANT);

	for (int i = 0; i < 10; i++)
	{
		pPartikelSystem->PushPartikel (xPos-30+rand()%100, yPos-30+rand()%80, EXPLOSION_MEDIUM2);
		pPartikelSystem->PushPartikel (xPos+10+rand()%40, yPos+10+rand()%40,  SPIDERSPLITTER);
	}

	for (int i = 0; i < 4; i++)
		pPartikelSystem->PushPartikel (xPos+rand()%80, yPos+rand()%80, SPLITTER);

	pSoundManager->PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION4);

	pPlayer[0]->Score += 1500;		// Punkte geben
}
