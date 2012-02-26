// --------------------------------------------------------------------------------------
// Der Spikelift
//
// Fährt dem Hurri hinterher, sobald dieser oberhalb ist
// Er kann zwar drauf stehen, verliert dabei aber enorm viel energie
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_Spikelift.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSpikelift::GegnerSpikelift(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_INIT;
	BlickRichtung	= RECHTS;
	Energy			= 100;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= false;
	AnimCount	    = 0.0f;
	TestBlock		= false;
	OwnDraw			= true;
}

// --------------------------------------------------------------------------------------
// Eigene Drawfuktion
// --------------------------------------------------------------------------------------

void GegnerSpikelift::DoDraw(void)
{
	// Unterteil rendern
	//
	pGegnerGrafix[GegnerArt]->RenderSprite ((float)(xPos-pTileEngine->XOffset), 
										    (float)(yPos-pTileEngine->YOffset) + 40.0f,
										    1, 0xFFFFFFFF);

	// Oberteil (Spikes) rendern
	if (Value1 == 0)
		pGegnerGrafix[GegnerArt]->RenderSprite ((float)(xPos-pTileEngine->XOffset), 
											    (float)(yPos-pTileEngine->YOffset), 
											    0, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Spikelift KI"
// --------------------------------------------------------------------------------------

void GegnerSpikelift::DoKI(void)
{
	/*
	// Je nach Handlung richtig verhalten
	//
	switch (Handlung)
	{
		// init
		case GEGNER_INIT:
		{
			// ypos merken
			oldypos = yPos;

			// Auf hurri warten?
			if (Value1 == 0)
				Handlung = GEGNER_STEHEN;

			// oder "runterdrückbar sein"
			else
			{
				yAcc = -1.0f;
				Handlung = GEGNER_LAUFEN;
			}
		} break;

		// warten, bis der hurri oben drüber ist
		case GEGNER_STEHEN:
		{
			if (PlayerAbstandHoriz() < 200 &&
				PlayerAbstandVert()  > 100 &&
				PlayerAbstandVert()  < 300 &&
				pPlayer->ypos < yPos)
			{
				Handlung = GEGNER_LAUFEN;
				yAcc = -1.0f;
			}
		} break;		
		
		case GEGNER_LAUFEN:
		{
			// nach oben fahren
			if (ySpeed < - 7.0f)
				ySpeed = -7.0f;

/*
			// Hurri eingeklemmt, dann anhalten
			if (pPlayer->AufPlattform == this &&
				pTileEngine->BlockOben(pPlayer->xpos,	 pPlayer->ypos, 
									   pPlayer->xposold, pPlayer->yposold, 
									   pPlayer->CollideRect) & BLOCKWERT_WAND)
				ySpeed = 0.0f;
*/
/*
			// Hurri draufgehopst, dann nach unten abstoßen
			if (pPlayer->AufPlattform != this)
			{
				PlattformTest(GegnerRect[GegnerArt]);

				// jetzt drauf?
				if (pPlayer->AufPlattform == this)
					ySpeed = 7.0f;
			}	
			
			// Nicht höher als Startpunkt fahren?
			if (Value1 != 0 &&
				yPos < oldypos)
				yPos = oldypos;

		} break;
	}

	// Als Plattform dienen
	PlattformTest(GegnerRect[GegnerArt]);

	// Kollision?
	GegnerRect[GegnerArt].top = 10;

	if (pPlayer->ypos < yPos)
		TestDamagePlayers(20.0f SYNC);

	// Wieder normale Collision Box
	GegnerRect[GegnerArt].top = 40;

	// von Unten dagegen gesprungen
	if (pPlayer->ypos      > yPos      &&
		pPlayer->yspeed    < 0.0f      &&
		pPlayer->ypos + 80 > yPos + 40 &&
		pPlayer->ypos + 10 < yPos + 80 &&
		pPlayer->xpos + 70 > xPos      &&
		pPlayer->xpos      < xPos + 460)
	{
		pPlayer->Handlung = SPRINGEN;
		pPlayer->yspeed  = 0.0f;
		pPlayer->JumpAdd = PLAYER_JUMPADDSPEED;
	}

	if (NUMPLAYERS == 2)
		if (pPlayer2->ypos      > yPos      &&
			pPlayer2->yspeed    < 0.0f      &&
			pPlayer2->ypos + 80 > yPos + 40 &&
			pPlayer2->ypos + 10 < yPos + 80 &&
			pPlayer2->xpos + 70 > xPos      &&
			pPlayer2->xpos      < xPos + 460)
			pPlayer2->yspeed = 0.0f;
		{
			pPlayer2->Handlung = SPRINGEN;
			pPlayer2->yspeed  = 0.0f;
			pPlayer2->JumpAdd = PLAYER_JUMPADDSPEED;
		}*/
}

// --------------------------------------------------------------------------------------
// Spikelift explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerSpikelift::GegnerExplode(void)
{
}
