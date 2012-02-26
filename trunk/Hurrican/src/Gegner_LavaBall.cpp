// --------------------------------------------------------------------------------------
// Der Lava Ball
//
// Hopst aus der Lava raus und wieder rein
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_LavaBall.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerLavaBall::GegnerLavaBall(int Wert1, int Wert2, bool Light)
{
	Active			= true;
	Handlung		= GEGNER_EINFLIEGEN;
	Energy			= 100;
	Value1			= Wert1;
	Value2			= Wert2;
	AnimStart		= 0;
	AnimEnde		= 19;
	AnimSpeed		= 0.5f;
	ChangeLight		= Light;
	Destroyable		= false;
	ySpeed			= float (-Value1);
	yAcc			= 4.0f;
	SmokeDelay		= 0.0f;
	FlareDelay		= 0.0f;
	InLava			= true;
	OwnDraw			= true;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerLavaBall::DoDraw(void)
{
	// Render
	pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset), 
										   (float)(yPos-pTileEngine->YOffset), 
										   AnimPhase, 0xFFFFFFFF);	

	// Flare rendern
	DirectGraphics.SetAdditiveMode ();
	float w = yPos/4;
	LavaFlare.RenderSpriteScaledRotated (float (xPos - 60 - pTileEngine->XOffset), 
	 									 float (yPos - 60 - pTileEngine->YOffset), 180, 180, w, 0xFFFFCC88);
	DirectGraphics.SetColorKeyMode ();
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerLavaBall::DoKI(void)
{
	SimpleAnimation();

	// erst erschienen? Dann yPos merken
	//
	if (Handlung == GEGNER_EINFLIEGEN)
	{
		yStart   = yPos;
		Handlung = GEGNER_LAUFEN;
		return;
	}

	if (pTileEngine->BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]) & BLOCKWERT_LIQUID)
	{
		// eben erst reingefallen?
		//
		if (InLava == false)
		{
			for (int i=0; i<12; i++)
				pPartikelSystem->PushPartikel(xPos+16+rand()%20, yPos + GegnerRect[GegnerArt].bottom+8, LAVA_SPRITZER2);

			pSoundManager->PlayWave3D((int)xPos + 30, (int)yPos + 30, 10000 + rand()%2050, SOUND_WATERIN);
		}

		InLava = true;
	}
	else 
	{
		// eben erst rausgekommen?
		//
		if (InLava == true)
		{
			for (int i=0; i<12; i++)
				pPartikelSystem->PushPartikel(xPos+16+rand()%20, yPos + GegnerRect[GegnerArt].bottom+8, LAVA_SPRITZER);

			pSoundManager->PlayWave3D((int)xPos + 30, (int)yPos + 30, 10000 + rand()%2050, SOUND_WATEROUT);
		}

		InLava = false;
	}

	// Rauchen lassen und Funken erzeugen
	//
	SmokeDelay -= 1.0f SYNC;

	if (SmokeDelay < 0.0f)
	{
		SmokeDelay = 0.5f;

		if (InLava == false)
			pPartikelSystem->PushPartikel (xPos - 20 + rand ()%50, yPos + rand ()%40, SMOKEBIG);
	}

	FlareDelay -= 1.0f SYNC;

	if (FlareDelay < 0.0f)
	{
		FlareDelay = 2.0f;
		//pPartikelSystem->PushPartikel (xPos - 16 + rand ()%56, yPos  - 16 + rand ()%56, MINIFLARE);
	}

	// Maximale Fall-Geschwindigkeit
	//
	if (ySpeed > 50.0f)
		ySpeed = 50.0f;

	// verschwinden?
	//
	if (ySpeed > 0.0f && yPos > yStart)
		Energy = 0.0f;

	// Spieler ber�hrt 
	// Explosion passiert schon hier und nicht erst im Destruktor, da der Ball auch, wenn er wieder in die Lava
	// zur�ckfliegt, zerst�rt wird, aber dabei nicht explodieren soll
	//
	for (int i = 0; i < NUMPLAYERS; i++)
	if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt],
						pPlayer[i]->xpos, pPlayer[i]->ypos, pPlayer[i]->CollideRect) == true)
	{
		pPlayer[i]->DamagePlayer(50.0f);
		Energy = 0.0f;

		pSoundManager->PlayWave (100, 128, 9000 + rand()%2000, SOUND_EXPLOSION1);

		pPartikelSystem->PushPartikel (xPos - 10, yPos - 30, EXPLOSION_GIANT);

		for (int i = 0; i < 4; i++)	
			pPartikelSystem->PushPartikel (xPos - 10 + rand ()%40, yPos - 10 + rand ()%40, EXPLOSION_MEDIUM2);

		for (int i = 0; i < 20; i++)	
			pPartikelSystem->PushPartikel (xPos - 10 + rand ()%50, yPos  - 16 + rand ()%56, MINIFLARE);

		for (int i = 0; i < 20; i++)	
			pPartikelSystem->PushPartikel (xPos + rand ()%40, yPos + rand ()%40, SMOKE2);		
	}	
}

// --------------------------------------------------------------------------------------
// LavaBall explodiert
// --------------------------------------------------------------------------------------

void GegnerLavaBall::GegnerExplode(void)
{
}
