// --------------------------------------------------------------------------------------
// Secret Area
//
// Wird sie vom Spieler berührt, so wird der "Secret per Level" Counter erhöht
// wird für die "Level-Endabrechnung/Summary Screen" benötigt
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_Secret.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSecret::GegnerSecret(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;	
	Energy			= 100;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= false;
	TestBlock		= false;
	DontMove		= true;
}

// --------------------------------------------------------------------------------------
// "Secret KI"
// --------------------------------------------------------------------------------------

void GegnerSecret::DoKI(void)
{
	// Secret nicht im Bild ?
	//
	if (xPos + 10 < pTileEngine->XOffset		||
		xPos	  > pTileEngine->XOffset + 640	||
		yPos + 10 < pTileEngine->YOffset		||
		yPos	  > pTileEngine->YOffset + 480)
	{
		return;
	}

	switch (Handlung)
	{
		case GEGNER_STEHEN:
		{
			// Spieler berührt das Secret? Dann Counter erhöhen und deaktivieren
			//
			GegnerRect[GegnerArt].left   = -Value1 / 2 + 20;
			GegnerRect[GegnerArt].right  =  Value1 / 2 + 20;
			GegnerRect[GegnerArt].top    = -Value2 / 2 + 20;
			GegnerRect[GegnerArt].bottom =  Value2 / 2 + 20;

			for (int p = 0; p < NUMPLAYERS; p++)
			if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt],
								pPlayer[p]->xpos, 
								pPlayer[p]->ypos, 
								pPlayer[p]->CollideRect) == true)
			{
				pPlayer[0]->SecretThisLevel++;
				pPlayer[0]->SecretFullGame++;
				Handlung = GEGNER_FALLEN;

				pSoundManager->PlayWave(100, 128, 11025, SOUND_SECRET);

				for (int i = 0; i < 200; i++)
					pPartikelSystem->PushPartikel(xPos + 20, yPos + 20, KRINGELSECRET);

				WinkelUebergabe = -1.0f;
				pPartikelSystem->PushPartikel(xPos + 20, yPos + 12, TEXTSECRET);
				pPartikelSystem->PushPartikel(xPos - 100, yPos - 100, GRENADEFLARE);
				pPartikelSystem->PushPartikel (xPos + 5, yPos + 5, EXPLOSION_KRINGEL);

			}
		} break;
	}
}

// --------------------------------------------------------------------------------------
// Secret explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerSecret::GegnerExplode(void)
{
}