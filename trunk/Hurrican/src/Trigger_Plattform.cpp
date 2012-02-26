// --------------------------------------------------------------------------------------
// Die Plattform
//
// Fährt mit x und y Geschwindigkeit die den Values entsprechen in der Luft rum
// und dreht an einem WendePunkt um
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_Plattform.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPlattform::GegnerPlattform(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	BlickRichtung	= LINKS;
	Energy			= 100;
	Value1			= Wert1;
	Value2			= Wert2;
	xSpeed			= (float)Wert1;
	ySpeed			= (float)Wert2;	

	ChangeLight		= Light;
	Destroyable		= false;
	Active			= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerPlattform::DoKI(void)
{
	if (pTileEngine->DateiAppendix.UsedPowerblock == 2)
		AnimPhase = 1;
	else
		AnimPhase = 0;

	// An den Wendepunkten umdrehen

	if (blockl & BLOCKWERT_WENDEPUNKT)
		xSpeed = (float)Value1;

	if (blockr & BLOCKWERT_WENDEPUNKT)
		xSpeed = -(float)Value1;

	if (blocko & BLOCKWERT_WENDEPUNKT)
		ySpeed = (float)Value2;

	if (blocku & BLOCKWERT_WENDEPUNKT)
		ySpeed = -(float)Value2;

	// Spieler steht auf der Plattform
	//
	PlattformTest(GegnerRect[GegnerArt]);
}

// --------------------------------------------------------------------------------------
// Plattform explodiert
// --------------------------------------------------------------------------------------

void GegnerPlattform::GegnerExplode(void)
{
}
