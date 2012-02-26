// --------------------------------------------------------------------------------------
// Der Schleimboller mit Maul
//
// Hängt in der Gegend rum und spawnt maximal 6 Baby Alien-Schleimboller
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_SchleimMaul.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchleimMaul::GegnerSchleimMaul (float x, float y, int  Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_INIT;
	HitSound		= 1;
	Energy			= 200;	
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= true;
	AnimSpeed       = ((float)(rand()%4 + 2)) / 10.0f;
	AnimEnde		= 15;
	ShotDelay		= 0.0f;
	SpawnCount		= 0.0f;
	OwnDraw			= true;

	// 8 Baby Schleimbollen spawnen ;)
	//
	for (int i = 0; i < 8; i++)
	{
		pChildren[i] = NULL;

		float r, r1, r2;

		// Zufallspunkt um die Mama rum
		//
		r = (float)(rand()%360);

		r1 = float (rand()%40) + 30;
		r2 = float (rand()%40) + 30;
	
		pGegner->PushGegner (x + (float)(sin(r)) * r1, y + (float)(cos(r)) * r2, SCHLEIMALIEN, 0, 0, ChangeLight);
		pChildren[i] = pGegner->pEnd;
	}
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerSchleimMaul::DoDraw(void)
{
	// etwas durchsichtig rendern
	//
	pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset), 
										   (float)(yPos-pTileEngine->YOffset),
										    AnimPhase, 0xAAFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSchleimMaul::DoKI(void)
{
	SimpleAnimation ();
		
	// Ein Child weg? Dann neu spawnen
	for (int i = 0; i < 8; i++)
	{
		if (pChildren[i]->Energy <= 0.0f)
		{
			float r, r1, r2;

			// Zufallspunkt um die Mama rum
			//
			r = (float)(rand()%360);

			r1 = float (rand()%40) + 30;
			r2 = float (rand()%40) + 30;
		
			pGegner->PushGegner (xPos + (float)(sin(r)) * r1, yPos + (float)(cos(r)) * r2, SCHLEIMALIEN, 20, 0, ChangeLight);
			pChildren[i] = pGegner->pEnd;
		}
	}

	ShotDelay -= 1.0f SYNC;

	if (ShotDelay <= 0.0f)
	{
		ShotDelay = 5.0f + (4 - Skill) * 3.0f;

		pProjectiles->PushProjectile(xPos + 30, yPos + 18, SCHLEIMSHOT);
	}

	Wegschieben(GegnerRect[GegnerArt], 4.0f);
}

// --------------------------------------------------------------------------------------
// SchleimMaul explodiert
// --------------------------------------------------------------------------------------

void GegnerSchleimMaul::GegnerExplode(void)
{
	pPartikelSystem->PushPartikel(xPos, yPos, EXPLOSION_ALIEN);
	
	for (int i = 0; i < 16; i++)
		pPartikelSystem->PushPartikel(xPos + 15 + rand ()% 20, 
									  yPos + 15 + rand ()% 20, SCHLEIM2);

	pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_SCHLEIM);	// Sound ausgeben

	pPlayer[0]->Score += 120;
}
