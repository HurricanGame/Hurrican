// --------------------------------------------------------------------------------------
// Der blaue Boulder
//
// fällt dem Hurri von oben auf die Birne
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_BlueBoulder.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// Achtung: Light gibt hier an false = fahrstuhl, true = golemwurf
// --------------------------------------------------------------------------------------

GegnerBlueBoulder::GegnerBlueBoulder(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_NOTVISIBLE;
	Energy			= 10;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= false;
	Destroyable		= false;
	AnimEnde		= 14;
	AnimStart		= 0;
	AnimSpeed		= 1.0f;

	// Im Fahrstuhl? Dann auf den Spieler runterfallen
	if (Light == false)
		ySpeed = g_Fahrstuhl_Speed + float (Value1);	
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerBlueBoulder::DoKI(void)
{
	SimpleAnimation();

	if (blocku & BLOCKWERT_WAND)
		Energy = 0.0f;

	switch (Handlung)
	{
		// Stein wird "aktiviert" (über den Hurri gesetzt damit er von dort runterfallen kann)
		case GEGNER_NOTVISIBLE:
		{
			Handlung = GEGNER_FALLEN;

			// Im Fahrstuhllevel runterfallen
			if (g_Fahrstuhl_yPos > -1.0f)
				yPos -= 480.0f;

			// andererseits, bei Golemwurf direkt auf den Spieler zufliegen
			else
			{				
				xSpeed		 = (float)Value1;
				ySpeed		 = (float)Value2;
				yAcc		 = 2.0f;
				AnimSpeed    = 0.4f;
			}
		} break;

		// Stein fällt runter
		case GEGNER_FALLEN:
		{
		} break;

		default : break;
	} // switch

	// Testen, ob der Spieler den Stein berührt hat
	TestDamagePlayers(20.0f, true);	

	// Testen, ob der Stein den Fahrstuhl getroffen hat
	if (g_Fahrstuhl_yPos > -1.0f &&
		yPos >= g_Fahrstuhl_yPos - GegnerRect[GegnerArt].bottom)
		Energy = 0.0f;

	// oder ob er an eine Wand gekommen ist
	if (blockl & BLOCKWERT_WAND ||
		blockr & BLOCKWERT_WAND ||
		blocku & BLOCKWERT_WAND ||
		blocko & BLOCKWERT_WAND)
		Energy = 0.0f;
}	

// --------------------------------------------------------------------------------------
// Boulder explodiert
// --------------------------------------------------------------------------------------

void GegnerBlueBoulder::GegnerExplode(void)
{
	pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_STONEEXPLODE);

	for (int i = 0; i < 16; i++)
		pPartikelSystem->PushPartikel(xPos - 10 + rand()%60, yPos + 5 + rand()%30, BOULDER_SMALL);

	for (int i = 0; i < 12; i++)
		pPartikelSystem->PushPartikel(xPos - 25 + rand()%60, yPos - 25 + rand()%50, SMOKEBIG);
}
