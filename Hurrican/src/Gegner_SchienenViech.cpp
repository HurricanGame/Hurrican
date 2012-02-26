// --------------------------------------------------------------------------------------
// Das SchienenViech
//
// Läuft auf Schienen entlang auf den Hurri zu
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_SchienenViech.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchienenViech::GegnerSchienenViech(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	Energy			= 80;
	AnimSpeed		= 1.0f;
	AnimEnde		= 4;
	ChangeLight		= Light;
	Destroyable		= true;
	TestBlock		= false;
	OwnDraw			= true;
	xAcc			= 4.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Drawfunktion
// --------------------------------------------------------------------------------------

void GegnerSchienenViech::DoDraw(void)
{	
	D3DCOLOR col;

	col = 0xFFCCCCCC;

	// Draw kommt jetzt erst hier, damit der Gegner über allen Leveltiles gerendert wird
	pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset), 
										   (float)(yPos-pTileEngine->YOffset), 
										   AnimPhase, col);	

	AlreadyDrawn = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSchienenViech::DoKI(void)
{
	// temporäre position für Gegnerwand Check (Wandcheck komplett rausgenmommen, damit er nicht in der 
	// Schiene stecken bleibt ;)
	//
	float x = xPos;
	float y = yPos;
	blockl = pTileEngine->BlockLinks  (x, y, x, y, GegnerRect[GegnerArt]);
	blockr = pTileEngine->BlockRechts (x, y, x, y, GegnerRect[GegnerArt]);

	if ((((blockl & BLOCKWERT_GEGNERWAND) || (blockl & BLOCKWERT_WAND)) && xSpeed < 0.0f) ||
		(((blockr & BLOCKWERT_GEGNERWAND) || (blockl & BLOCKWERT_WAND)) && xSpeed > 0.0f))
	{
		Handlung = GEGNER_LAUFEN;
		xSpeed *= -1.0f;
		xAcc   *= -1.0f;		
	}

	AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
	if (AnimCount > AnimSpeed)			// Grenze überschritten ?
	{
		AnimCount = 0;					// Dann wieder auf Null setzen

		// Vorwärts oder rückwärts animieren, je nachdem, in welche Richtung das Viech gerade fährt
		//
		if (xSpeed < 0.0f)
		{
			AnimPhase++;					// Und nächste Animationsphase
			if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
			{
				AnimPhase = AnimStart;		// Dann wieder von vorne beginnen

				// Schiessen
				//
				if (pAim->ypos < yPos)
					pProjectiles->PushProjectile (xPos+18, yPos - 8, STRAIGHTSCHUSS);
				else
					pProjectiles->PushProjectile (xPos+18, yPos + 48,STRAIGHTSCHUSS2);
			}
		}
		else
		{
			AnimPhase--;					// Und vorherige Animationsphase
			if (AnimPhase < AnimStart)		// Animation von zu Ende	?
			{
				AnimPhase = AnimEnde;		// Dann wieder von hinten her beginnen

				// Schiessen
				//
				if (pAim->ypos < yPos)
					pProjectiles->PushProjectile (xPos+18, yPos - 8, STRAIGHTSCHUSS);
				else
					pProjectiles->PushProjectile (xPos+18, yPos + 48,STRAIGHTSCHUSS2);
			}
		}
	} // animieren

	switch (Handlung)
	{
		// Spieler nicht verfolgen
		case GEGNER_LAUFEN:
		{
			// Spieler in Fahrtrichtung gesehen? Dann wieder verfolgen
			if ((xSpeed > 0.0f && pAim->xpos + 35 > xPos + 24) ||
				(xSpeed < 0.0f && pAim->xpos + 35 < xPos + 24))
				Handlung = GEGNER_VERFOLGEN;

		} break;

		// Spieler verfolgen
		case GEGNER_VERFOLGEN:
		{
			
			if (PlayerAbstandHoriz() < 250)
			{
				if (pAim->xpos + 35 > xPos + 24)
					xAcc = 4.0f;
				else
					xAcc = -4.0f;
			}
		} break;
	}	

	// Verfolgungsspeed begrenzen wenn er auf den Spieler zufährt
	//
	if (xSpeed > 20.0f)	xSpeed = 20.0f;
	if (xSpeed <-20.0f)	xSpeed =-20.0f;

	// ansonten Verfolgungsspeed begrenzen wenn er am Spieler dran ist
	//
	if (PlayerAbstandHoriz() < 60)
	{
		if (xSpeed > 15.0f)	{xAcc = 0.0f;	xSpeed = 15.0f;	}
		if (xSpeed <-15.0f)	{xAcc = 0.0f;	xSpeed =-15.0f;	}
	}			
}

// --------------------------------------------------------------------------------------
// SchienenViech explodiert
// --------------------------------------------------------------------------------------

void GegnerSchienenViech::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION3);

	for (int i = 0; i < 5; i++)
		pPartikelSystem->PushPartikel(xPos - 30 + rand()%30, yPos - 30 + rand()%40, EXPLOSION_MEDIUM2);

	pPlayer[0]->Score += 500;
}
