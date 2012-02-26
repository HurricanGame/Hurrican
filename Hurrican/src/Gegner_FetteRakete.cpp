// --------------------------------------------------------------------------------------
// Die Rakete, die die Riesenspinnen spuckt
//
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_FetteRakete.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFetteRakete::GegnerFetteRakete(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_INIT;
	Energy			= 50;
	ChangeLight		= Light;
	Destroyable		= true;
	Value1			= Wert1;
	Value2			= Wert2;
	SmokeDelay		= 0.0f;
	AnimEnde		= 10;
	AnimSpeed		= 0.3f;
	OwnDraw			= true;
	rot				= (float)(Wert1);
	BlickRichtung   = LINKS;

	// Rakete kommt vom Drachen? Dann verhält sie sich anders
	if (Wert2 == 99)
	{
		xSpeed    =  0.0f;
		ySpeed    = -10.0f;
		yAcc      =  2.0f;
		Handlung  = GEGNER_DREHEN;
		AnimCount = 12.0f;
	}
}
	
// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerFetteRakete::DoDraw(void)
{
	// Rakete rendern
	//
	pGegnerGrafix[GegnerArt]->RenderSpriteRotated ((float)(xPos-pTileEngine->XOffset), 
										  		   (float)(yPos-pTileEngine->YOffset), 
												   (float)rot, AnimPhase, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFetteRakete::DoKI(void)
{
	BlickRichtung = LINKS;

	switch (Handlung)
	{
		case GEGNER_INIT:
		{
			// Rakete kam nicht vom Drachen? Dann
			// enthält Wert1 den Winkel, in den die Raketen fliegen sollen
			xSpeed =  float (sin (Value1 * PI / 180.0f) * 35);
			ySpeed = -float (cos (Value1 * PI / 180.0f) * 35);
			Handlung = GEGNER_LAUFEN;
		} break;

		// Rakete dreht sich in Richtung Hurrican
		case GEGNER_DREHEN:
		{
			AnimCount -= 1.0f SYNC;

			// Gegner auf Spieler ausrichten
			//
			float dx, dy;

			// Abstände berechnen
			dx = (xPos + 12) - (pAim->xpos + 35);
			dy = (yPos + 17) - (pAim->ypos + 40);

			// Division durch Null verhinden
			if (dy == 0.0f)
				dy = 0.01f;

			float w, winkel;

			w = float(atan(dx / dy) * 360.0f / (D3DX_PI * 2));

			if (dx >= 0 && dy >= 0) winkel = w;		  else
			if (dx > 0  && dy < 0 ) winkel = 180 + w;  else
			if (dx < 0  && dy > 0 ) winkel = 360 + w;  else
			if (dx < 0  && dy < 0 ) winkel = 180 + w;

			winkel = 360.0f - winkel;

			if (rot < winkel) rot += 25.0f SYNC;
			if (rot > winkel) rot -= 25.0f SYNC;

			// zuende gedreht? Dann losfliegen
			if (AnimCount < 0.0f)
			{
				Handlung = GEGNER_LAUFEN;
				pSoundManager->PlayWave(100, 128, 9000 + rand()%2000, SOUND_ROCKET);

				xAcc = 0;
				yAcc = 0;
				xSpeed =  float (sin (rot * PI / 180.0f) * 35);
				ySpeed = -float (cos (rot * PI / 180.0f) * 35);
			}
		} break;

		// Rakete fliegt
		//
		case GEGNER_LAUFEN:
		{
			SimpleAnimation();

			SmokeDelay -= 1.0f SYNC;

			if (SmokeDelay < 0.0f)
			{
				SmokeDelay = 0.3f;

				pPartikelSystem->PushPartikel(xPos + 2, yPos + 5, SMOKE);
				//pPartikelSystem->PushPartikel(xPos + 8, yPos + 12, ROCKETSMOKE);
			}			

			// Gegen die Wand geflogen? Dann explodieren lassen
			//
			if (blockl & BLOCKWERT_WAND ||
				blockr & BLOCKWERT_WAND ||
				blocko & BLOCKWERT_WAND ||
				blocku & BLOCKWERT_WAND)				
				Energy = 0.0f;

		} break;

		default : break;
	} // switch


	// Spieler getroffen?
	// Dann explodieren lassen und Energy abziehen
	//
	TestDamagePlayers(25.0f, true);
}

// --------------------------------------------------------------------------------------
// FetteRakete explodiert
// --------------------------------------------------------------------------------------

void GegnerFetteRakete::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, 128, 8000 + rand ()%4000, SOUND_EXPLOSION1);

	pPartikelSystem->PushPartikel (xPos - 10, yPos - 20, EXPLOSION_MEDIUM2);

	for (int i=0; i < 5; i++)	
	{
		pPartikelSystem->PushPartikel (xPos + rand ()%20, yPos - 10 + rand()%20, MINIFLARE);
		pPartikelSystem->PushPartikel (xPos + rand ()%20, yPos - 10 + rand()%20, SPIDERSPLITTER);
	}

	pPlayer[0]->Score += 50;
}
