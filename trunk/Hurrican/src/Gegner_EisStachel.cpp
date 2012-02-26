// --------------------------------------------------------------------------------------
// Der sich drehende Eisstachel
//
// Bewegt sich sinusförmig (auf der x-achse) hin und her und dreht sich dabei
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_EisStachel.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerEisStachel::GegnerEisStachel(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	Energy			= 100;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= false;
	xSpeed			= 0.0f;
	ySpeed			= 0.0f;
	xAcc			= 1.7f;
	AnimSpeed       = 1.0f;
	AnimEnde		= 19;
	Active			= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerEisStachel::DoKI(void)
{
	SimpleAnimation();

	// Je nach Handlung richtig verhalten
	switch (Handlung)
	{
		case GEGNER_LAUFEN:					// Umherdrehen
		{
			// Bei der Beschleunigungsgrenze umdrehen
			if (xSpeed >  Value1 && xAcc > 0.0f) xAcc = -xAcc;
			if (xSpeed < -Value1 && xAcc < 0.0f) xAcc = -xAcc;
		} break;

		default : break;
	} // switch

	Wegschieben(GegnerRect[GegnerArt], 10.0f);	
}

// --------------------------------------------------------------------------------------
// EisStachel explodiert
// --------------------------------------------------------------------------------------

void GegnerEisStachel::GegnerExplode(void)
{
}
