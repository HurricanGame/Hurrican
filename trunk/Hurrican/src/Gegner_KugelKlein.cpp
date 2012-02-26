// --------------------------------------------------------------------------------------
// Die kleine Stachelkugel
//
// H�pft nur im Gang entlang
// Je nachdem wie hoch sie im Editor �ber dem Boden gesetzt wurde springt sie auch
// sp�ter umher. Springt zu Beginn in eine zuf�llige Richtung links/rechts
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_KugelKlein.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerKugelKlein::GegnerKugelKlein(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	Energy			= 20;
	BlickRichtung	= LINKS;
	(rand()%2 == 0) ? xSpeed = -8.0f : xSpeed = 8.0f;	// x-Richtung zu Beginn Zufall
	yAcc			= 5.0f;					// Fallen lassen
	Value1			= Wert1;
	Value2			= Wert2;
	if (Value1 != 0)						// ggf vorgegebene Speed benutzen
		xSpeed = float(Value1);
	AnimSpeed		= 0.3f;
	AnimStart		= 0;
	AnimEnde		= 19;
	ChangeLight		= Light;
	Destroyable		= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerKugelKlein::DoKI(void)
{
	BlickRichtung = LINKS;
	AnimCount += SpeedFaktor;			// Animationscounter weiterz�hlen
	if (AnimCount > AnimSpeed)			// Grenze �berschritten ?
	{
		AnimCount = 0;					// Dann wieder auf Null setzen

		// Vorw�rts oder r�ckw�rts animieren, je nachdem, in welche Richtung die Kugel gerade fliegt
		//
		if (xSpeed > 0.0f)
		{
			AnimPhase++;					// Und n�chste Animationsphase
			if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
				AnimPhase = AnimStart;		// Dann wieder von vorne beginnen
		}
		else
		{
			AnimPhase--;					// Und vorherige Animationsphase
			if (AnimPhase < AnimStart)		// Animation von zu Ende	?
				AnimPhase = AnimEnde;		// Dann wieder von hinten her beginnen
		}
	} // animieren

	// Grenze f�r y-Bewegung setzen
	//
	if (ySpeed > 20.0f) ySpeed = 20.0f;

	// Je nach Handlung richtig verhalten
	//
	switch (Handlung)
	{
		case GEGNER_LAUFEN:					// Kugel h�pft im Gang rum
		{
			// Vertikal abh�pfen
			//
			if ((ySpeed > 0.0f &&						// Boden
				 (blocku & BLOCKWERT_WAND		||
				  blocku & BLOCKWERT_GEGNERWAND ||
				  blocku & BLOCKWERT_PLATTFORM))
				||										// Decke
				 (ySpeed < 0.0f &&		
				(blocko & BLOCKWERT_WAND		||
				 blocko & BLOCKWERT_GEGNERWAND)))
			{
				ySpeed *= -1.0f;

				// Mindestsprungh�he setzen
				//
				if (ySpeed < 0.0f &&
					ySpeed > -10.0f)
					ySpeed = -10.0f;
			}

			// Horizontal abh�pfen
			//
			if ((xSpeed < 0.0f &&						// Links
				 (blockl & BLOCKWERT_WAND		||
				  blockl & BLOCKWERT_GEGNERWAND))
				||										// Rechts
				 (xSpeed > 0.0f &&		
				(blockr & BLOCKWERT_WAND		||
				 blockr & BLOCKWERT_GEGNERWAND)))
				 xSpeed *= -1.0f;

		} break;

		default : break;
	} // switch

	// Ber�hrt die Kugel den Spieler ?
	//
	TestDamagePlayers(2.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Stachelkugel explodiert
// --------------------------------------------------------------------------------------

void GegnerKugelKlein::GegnerExplode(void)
{
	// Explosion
	pPartikelSystem->PushPartikel(float(xPos - 15), 
								  float(yPos - 15), EXPLOSION_MEDIUM2);

	pSoundManager->PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION4);	// Sound ausgeben

	pPlayer[0]->Score += 300;
}
