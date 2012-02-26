// --------------------------------------------------------------------------------------
// Die riesen Qualle
//
// Schwabbelt unter Wasser rum in Richtung Spieler
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_RiesenQualle.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerRiesenQualle::GegnerRiesenQualle(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	AnimStart		= 0;
	AnimEnde		= 6;
	AnimSpeed		= 2.0f;
	BlickRichtung	= LINKS;
	xSpeed			=  0.0f;
	ySpeed			= -8.0f;
	yAcc			=  0.4f;
	Energy			= 3000;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerRiesenQualle::DoKI(void)
{
	// animieren
	AnimCount += SpeedFaktor;		// Animationscounter weiterzählen
	if (AnimCount > AnimSpeed)			// Grenze überschritten ?
	{
		AnimCount = 0;					// Dann wieder auf Null setzen
		AnimPhase++;					// Und nächste Animationsphase
		if (AnimPhase > AnimEnde)		// Animation von zu Ende	?
			AnimPhase = AnimStart;
	}

// Je nach Handlung richtig verhalten
	switch (Handlung)
	{
		case GEGNER_SPRINGEN:				// Nach oben abgestossen zum Schwimmen
		{
			// Geschwindigkeits-Begrenzung
			if (ySpeed > 0.0f)
				yAcc = 0.25f;

			if (ySpeed > 8.0f)
				yAcc = 0.0f;

			if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
			{								// Dann absinken
				AnimPhase = 0;
				AnimEnde  = 0;
				AnimStart = 0;
				Handlung  = GEGNER_LAUFEN;
			}
		} break;

		case GEGNER_FALLEN:
		{
			if (PlayerAbstand() < 200)	// spieler verfolgen ?
			{
				AnimPhase = 0;
				Handlung = GEGNER_LAUFEN;
				yAcc	 = 0.0f;
			}
		} break;

		case GEGNER_LAUFEN:						// Qualle sinkt ab
		{

			// Nach oben abstossen
			if (yPos-20 > pAim->ypos ||
				blocku & BLOCKWERT_WAND ||
				blocku & BLOCKWERT_GEGNERWAND)
			{
				// Bläschen erzeugen
				pPartikelSystem->PushPartikel(xPos + 20, yPos + 30, BUBBLE);

				// rechts / links bewegen
				if (xPos < pAim->xpos)
					xSpeed = 1.0f;
				else
					xSpeed = -1.0f;

				AnimPhase = 1;
				AnimEnde  = 6;
				AnimCount = 0.0f;
				ySpeed    = -8.0f;
				yAcc      =  0.4f;
				Handlung  = GEGNER_SPRINGEN;
			}
		} break;
		
		default : break;
	} // switch

	// Testen, ob der Spieler die Qualle berührt hat
	TestDamagePlayers(5.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Riesen Qualle explodiert
// --------------------------------------------------------------------------------------

void GegnerRiesenQualle::GegnerExplode(void)
{
	// Fetzen erzeugen
	for (int i=0; i<8; i++)
		pPartikelSystem->PushPartikel(float(xPos + 64 + rand()%64), 
									  float(yPos + 64 + rand()%64), PIRANHATEILE2);

	// und noch n paar Luftbläschen dazu
	for (int i=0; i<100; i++)
		pPartikelSystem->PushPartikel(float(xPos - 10  + rand()%256), 
									  float(yPos + 10  + rand()%210), BUBBLE);

	// Blutwolken dazu
	for (int i=0; i<70; i++)
		pPartikelSystem->PushPartikel(float(xPos - 10  + rand()%256), 
									  float(yPos + 10  + rand()%210), PIRANHABLUT);

	//pSoundManager->PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION1);	// Sound ausgeben

	pPlayer[0]->Score += 4000;
}
