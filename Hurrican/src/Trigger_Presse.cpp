// --------------------------------------------------------------------------------------
// Die Stahlpresse
//
// Bewegt sich mit Value1 als y-Speed hinunter
// und wartet dann solange wie es Value2 angibt
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_Presse.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPresse::GegnerPresse(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	BlickRichtung	= LINKS;
	Energy			= 100;
	Value1			= Wert1;
	Value2			= Wert2;
	ySpeed			= (float)Wert2;
	AnimPhase		= 0;
	ChangeLight		= Light;
	Destroyable		= false;
}

// --------------------------------------------------------------------------------------
// Bewegungs KI
// --------------------------------------------------------------------------------------

void GegnerPresse::DoKI(void)
{
	// Ja nach Handlung richtig verhalten
	switch (Handlung)
	{
		// Pressen (während der Wehen ...)
		case GEGNER_LAUFEN :
		{
			// Am Boden ? Dann aufstampfen und wieder nach oben fahren
			if (ySpeed > 0.0f &&
			   (blocku & BLOCKWERT_WAND		  ||
				blocku & BLOCKWERT_GEGNERWAND ||
				blocku & BLOCKWERT_WAND))
			{				
				ySpeed = -(float)Value2 / 2;

				// Staub erzeugen
				for (int i=0; i < 8; i++)
					pPartikelSystem->PushPartikel (xPos + i * 10, yPos + 244, SMOKE);

				if (PlayerAbstand() < 600)
					pSoundManager->PlayWave(100, 128, 11025, SOUND_PRESSE);
			}

			// An der Decke angekommen ? Dann warten
			if (blocko & BLOCKWERT_WAND)
			{
				Handlung  = GEGNER_STEHEN;
				AnimCount = float(Value2);
				ySpeed = 0.0f;
			}

		} break;

		// Warten
		case GEGNER_STEHEN :
		{
			// Wieder nach unten fahren
			AnimCount -= 1.0f SYNC;

			if (AnimCount <= 0.0f)
			{
				AnimCount = 0.0f;
				Handlung  = GEGNER_LAUFEN;
				ySpeed    = float(Value2);

				if (PlayerAbstand() < 600)
					pSoundManager->PlayWave(100, 128, 11025, SOUND_PRESSE);

				pPartikelSystem->PushPartikel (xPos + 14, yPos + 230, SMOKE);
				pPartikelSystem->PushPartikel (xPos + 54, yPos + 230, SMOKE);
			}
		} break;
	} // switch

	
	Wegschieben(GegnerRect[GegnerArt], 10.0f);	
}

// --------------------------------------------------------------------------------------
// Presse explodiert (geht ja garnich *g*)
// --------------------------------------------------------------------------------------

void GegnerPresse::GegnerExplode(void)
{
}