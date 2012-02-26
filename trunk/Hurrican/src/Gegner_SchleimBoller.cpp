// --------------------------------------------------------------------------------------
// Der Schleimboller
//
// Springt auf den Spieler zu
// Wird er zerstört, so teilt er sich in zwei kleinere Bollen auf
// Value1 gibt die größe an (0 - 60)
// Value2 gibt die neue xSpeed an
// Ist ein Bollen kleiner als 30, so teilt er sich nicht mehr.
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_SchleimBoller.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchleimBoller::GegnerSchleimBoller(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_FALLEN;	
	HitSound		= 1;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= true;

	if (Value1 != 0)
		Size			= Value1;
	else
		Size			= 60;

	Energy = (float)Size;

	xSpeed			= (float)(Value2);
	OwnDraw			= true;

	if (Value2 == 0)
		ySpeed			= 0.0f;
	else
		ySpeed			= -40.0f;

	AnimSpeed       = 2.0f;
	AnimEnde		= 2;

	yAcc = 8.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerSchleimBoller::DoDraw(void)
{
	// Je nach Größe anders gestrecht rendern
	//
	pGegnerGrafix[GegnerArt]->RenderSpriteScaled ((float)(xPos-pTileEngine->XOffset) + (30 - Size/2), 
											      (float)(yPos-pTileEngine->YOffset) + (60 - Size),
												  Size, Size, AnimPhase, 0xFFFFFFFF);

	// Leuchten noch dazurendern?
	//
	if (AlreadyDrawn == false)
	{
		if (options_Detail >= DETAIL_HIGH)
		{
			DirectGraphics.SetAdditiveMode ();
			LavaFlare.RenderSpriteScaled ((float)(xPos-pTileEngine->XOffset) + 30 - Size,
										  (float)(yPos-pTileEngine->YOffset) + 40 - Size,
										  Size * 2, Size * 2, 0x8888FF88);
			DirectGraphics.SetColorKeyMode ();
		}		
	}

	AlreadyDrawn = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSchleimBoller::DoKI(void)
{
	BlickRichtung = LINKS;

	if (Handlung == GEGNER_FALLEN)
		SimpleAnimation ();

	// nach Animation am Boden wieder abspringen
	//
	else
	{
		AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
		if (AnimCount > AnimSpeed)			// Grenze überschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen
			AnimPhase++;					// Und nächste Animationsphase
			if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
			{
				Handlung  = GEGNER_FALLEN;
				ySpeed	  = -(float)(rand()%10 + 30);
				yAcc	  = 8.0f;
				AnimPhase = 0;
				AnimEnde  = 2;
				AnimSpeed = 2.0f;

				if (pAim->xpos + 35 > xPos + 30)
					xSpeed = (float)(rand ()%8 + 6);
				else
					xSpeed = -(float)(rand ()%8 + 6);
			}
		} 
	}
	
	switch (Handlung)
	{
		// rumhopsen
		//
		case GEGNER_FALLEN:
		{			
			if (ySpeed > 50.0f)
			{
				ySpeed = 50.0f;
				yAcc = 0.0f;
			}

			// am Boden abhopfen?
			//
			if (ySpeed > 0.0f &&
				(
				 blocku & BLOCKWERT_WAND ||
				 blocku & BLOCKWERT_PLATTFORM))
			{
				Handlung  = GEGNER_STEHEN;
				AnimEnde  = 12;
				AnimSpeed = 0.5f;
				ySpeed = 0.0f;
				yAcc   = 0.0f;
				xSpeed = 0.0f;
			}

			// an der Decke abprallen
			if (ySpeed < 0.0f &&
			   (blocko & BLOCKWERT_WAND ||
			    blocko & BLOCKWERT_GEGNERWAND))
				 ySpeed *= -1;

		} break;

		case GEGNER_STEHEN:
		{
		} break;

	}

	// Testen, ob der Spieler den SchleimBoller berührt hat
	TestDamagePlayers(2.0f SYNC);
	Wegschieben(GegnerRect[GegnerArt], 2.0f);	
}

// --------------------------------------------------------------------------------------
// SchleimBoller explodiert
// --------------------------------------------------------------------------------------

void GegnerSchleimBoller::GegnerExplode(void)
{
	//pPartikelSystem->PushPartikel(xPos, yPos, EXPLOSION_GREEN);

	for (int i = 0; i < 3 + int (Size/8); i++)
		pPartikelSystem->PushPartikel(xPos + 15 + rand ()% 20, 
									  yPos + 15 + rand ()% 20, SCHLEIM);

	if (pSoundManager->its_Sounds[SOUND_SCHLEIM]->isPlaying == false)
		pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_SCHLEIM);	// Sound ausgeben

	pPlayer[0]->Score += 150;

	// Zwei kleinere Bobbel spawnen, wenn noch groß genug
	//
	if (Size >= 30)
	{
		int mul = 1;

		if (xPos + 30 - Size / 2 > pAim->xpos + 35)
			mul = -1;
		
		pGegner->PushGegner (xPos + 20 * mul, yPos, SCHLEIMBOLLER, int (Size - 8),  5, ChangeLight);
		pGegner->PushGegner (xPos - 20 * mul, yPos, SCHLEIMBOLLER, int (Size - 8), -5, ChangeLight);		
	}
}
