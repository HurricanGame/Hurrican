// --------------------------------------------------------------------------------------
// Der End Level Trigger
//
// Berührt ihn der Spieler so kommt er in
// die Stage, die bei Value1 angegeben ist
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_EndLevel.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerEndLevel::GegnerEndLevel(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= false;
	Energy			= 100;
	DontMove		= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerEndLevel::DoKI(void)
{
	// Ist der Trigger im Bild ?
	if (Energy > 0.0f &&
		(NewStage != Value1 ||
		RunningTutorial) &&
		xPos + 120 > pTileEngine->XOffset	    &&
		xPos	   < pTileEngine->XOffset + 640 &&
		yPos + 120 > pTileEngine->YOffset	    &&
		yPos	   < pTileEngine->YOffset + 480)
	{
		// Ausgang berührt ? Dann Nächsten Level laden
		for (int p = 0; p < NUMPLAYERS; p++)
		if (SpriteCollision(xPos,		   yPos,		  GegnerRect[GegnerArt],
						    pPlayer[p]->xpos, 
							pPlayer[p]->ypos, 
							pPlayer[p]->CollideRect) == true)							
							
		{
			// Spieler zentrieren
			if (NUMPLAYERS == 0)
				pTileEngine->ScrollLevel(pPlayer[0]->xpos - 300, 
										 pPlayer[0]->ypos - 280, ZUSTAND_SCROLLTOLOCK);
			else
				pTileEngine->ScrollLevel(pTileEngine->XOffset,
										 pTileEngine->YOffset, ZUSTAND_SCROLLTOLOCK);

			Energy = 0.0f;

			for (int p2 = 0; p2 < NUMPLAYERS; p2++)
			{				
				pPlayer[p2]->FesteAktion   = Value2;		// Art wie der Spieler den Screen verlässt
				pPlayer[p2]->DoFesteAktion = true;
			}

			NewStage = Value1;

			if (RunningTutorial && Value1 <= 9)
			{
				pSoundManager->PlayWave(100, 128, 11025, SOUND_MARIO);
				StageClear(false);
			}
			else			
				StageClear(true);
		}
	}
}

// --------------------------------------------------------------------------------------
// EndLevel explodiert (nicht *g*)
// --------------------------------------------------------------------------------------

void GegnerEndLevel::GegnerExplode(void)
{
}
