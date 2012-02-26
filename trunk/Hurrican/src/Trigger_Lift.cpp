// --------------------------------------------------------------------------------------
// Der Lift
//
// Fährt nach oben, sobald der Hurri draufsteht, und explodiert, wenn der Countdown
// abgelaufen ist, oder wenn der Hurri runterspringt
// Value1 gibt an, wie schnell der Countdown runterzählt
// Value2 speichert die Ursprungs y-Position
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_Lift.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerLift::GegnerLift(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	BlickRichtung	= LINKS;
	Energy			= 100;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= false;
	TestBlock		= false;
	SmokeDelay	    = 0.0f;
	AnimSpeed	    = Value1 / 10.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerLift::DoKI(void)
{
	BlickRichtung = LINKS;

	// Animieren
	if (AnimEnde > 0)						// Soll überhaupt animiert werden ?
	{
		AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
		if (AnimCount > AnimSpeed)			// Grenze überschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen
			AnimPhase++;					// Und nächste Animationsphase
			if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
			{
				for (int i = 0; i < 10; i++)
					pPartikelSystem->PushPartikel (xPos - 30 + rand ()% 80, yPos - 20 + rand()%20, EXPLOSION_MEDIUM2);

				Handlung  = GEGNER_STEHEN;
				AnimPhase = 0;
				AnimEnde  = 0;
				yPos      = float (Value2);
				ySpeed    = 0.0f;
				yAcc      = 0.0f;

				for (int p = 0; p < NUMPLAYERS; p++)
					pPlayer[p]->AufPlattform = NULL;

				pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);
			}
		}
	} // animieren

	PlattformTest(GegnerRect[GegnerArt]);

	switch (Handlung)
	{
		// Lift wartet auf den Hurri
		case GEGNER_STEHEN:
		{
			Value2 = int (yPos);

			for (int p = 0; p < NUMPLAYERS; p++)
			if (pPlayer[p]->AufPlattform == this)
			{
				Handlung  = GEGNER_LAUFEN;
				ySpeed    = -3.0f;
				yAcc	  = -3.0f;				
				AnimPhase = 0;
				AnimEnde  = 10;
				AnimStart = 0;
			}	
		} break;

		// Lift fährt nach oben
		case GEGNER_LAUFEN:
		{		
			// Max Speed
			if (ySpeed < -30.0f)
			{
				yAcc   =   0.0f;
				ySpeed = -30.0f;
			}
		} break;
	}
}

// --------------------------------------------------------------------------------------
// Lift explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerLift::GegnerExplode(void)
{	
}
