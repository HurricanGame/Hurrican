// --------------------------------------------------------------------------------------
// Der Geschützturm an der Decke
//
// Hängt geschlossen an der Decke, und öffnet sich im Loop, wenn der Spieler in der nähe ist
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Geschuetz.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerGeschuetz::GegnerGeschuetz(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	BlickRichtung	= LINKS;
	Energy			= 50;
	Value1			= Wert1;
	Value2			= Wert2;
	AnimSpeed		= 0.75f;
	ChangeLight		= Light;
	Destroyable		= true;
	TestBlock		= false;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerGeschuetz::DoKI(void)
{
	BlickRichtung = LINKS;

	SimpleAnimation();

	// Je nach Handlung richtig verhalten
	switch (Handlung)
	{
		// Kanone hängt an der Decke und wartet bis der Spieler in seine Reichweite kommt
		case GEGNER_STEHEN:			
		{			
			// Spieler in Sichtweite ? Dann aufmachen
			if (pAim->ypos + 70 >= yPos &&
				PlayerAbstandHoriz() < 400 &&
				PlayerAbstandVert()  < 400)
			{				
				Handlung  = GEGNER_SCHIESSEN;
				AnimPhase = 1;
				AnimStart = 11;
				AnimEnde  = 11;
				AnimCount = 0.0f;
			}
		} break;

		case GEGNER_SCHIESSEN:
		{
			// Schuss abgeben?
			if (AnimPhase == AnimStart)
			{
				pProjectiles->PushProjectile(xPos + 27, yPos + 36, KRABBLERLASER1);
				pSoundManager->PlayWave3D((int)(xPos + 30), (int)(yPos + 30), 44000, SOUND_LASERSHOT);
				Handlung  = GEGNER_SCHLIESSEN;				
				AnimEnde  = 25;
				AnimStart = 0;
			}
		} break;			

		case GEGNER_SCHLIESSEN:
		{
			// ganz geschlossen?
			if (AnimPhase == AnimStart)
			{				
				Handlung  = GEGNER_STEHEN;
				AnimEnde  = 0;
				AnimStart = 0;
				AnimPhase = 0;
			}
		} break;			
				
		default : break;
	} // switch

	TestDamagePlayers(5.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Geschuetz explodiert
// --------------------------------------------------------------------------------------

void GegnerGeschuetz::GegnerExplode(void)
{
	// Explosion
	pPartikelSystem->PushPartikel(float(xPos - 10), 
								  float(yPos - 10), EXPLOSION_MEDIUM2);

	pPartikelSystem->PushPartikel(float(xPos - 10), 
								  float(yPos - 10), SMOKEBIG);
	
	for (int i = 0; i < 10; i++)
		pPartikelSystem->PushPartikel(float(xPos + 10 + rand()%40), 
									  float(yPos + rand()%40), LONGFUNKE);



	pSoundManager->PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION1);	// Sound ausgeben

	pPlayer[0]->Score += 200;
}
