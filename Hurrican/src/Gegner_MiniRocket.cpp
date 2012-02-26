// --------------------------------------------------------------------------------------
// Die zielsuchende MiniRakete
//
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_MiniRocket.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerMiniRocket::GegnerMiniRocket(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_INIT;
	Energy			= 20;
	ChangeLight		= Light;
	Destroyable		= true;
	Value1			= Wert1;
	Value2			= Wert2;
	SmokeDelay		= 0.0f;
	FlareSin		= 0.0f;
	OwnDraw			= true;
	ySpeed			= -20.0f;
	AnimCount		= 3.0f;
	rot				= 0.0f;
}
	
// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerMiniRocket::DoDraw(void)
{	
	// Flare rendern
	//
	DirectGraphics.SetAdditiveMode();

	float xoff = (float)(sin((360 - rot) / 180.0f * PI) * 12.0f);
	float yoff = (float)(cos((360 - rot) / 180.0f * PI) * 12.0f);

	LavaFlare.RenderSpriteScaled ((float)(xPos-pTileEngine->XOffset) - 15.0f - (float)(sin(FlareSin) * 1.0f) + xoff, 
								  (float)(yPos-pTileEngine->YOffset) - 15.0f - (float)(sin(FlareSin) * 1.0f) + yoff, 
								 (int)(40.0f + (float)sin(FlareSin) * 2.0f),
								 (int)(40.0f + (float)sin(FlareSin) * 2.0f),
								 0, 0xFFFF8822);

	DirectGraphics.SetColorKeyMode();

	// Rakete rendern
	//
	pGegnerGrafix[GegnerArt]->RenderSpriteRotated ((float)(xPos-pTileEngine->XOffset), 
										  		   (float)(yPos-pTileEngine->YOffset), 
												   (float)rot, 0, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerMiniRocket::DoKI(void)
{
	BlickRichtung = LINKS;

	// Am Anfang erst etwas senkrecht hoch fliegen
	if (AnimCount > 0.0f)
		AnimCount -= 1.0f SYNC;

	// Flare pulsieren lassen
	//
	FlareSin += 3.0f SYNC;

	if (FlareSin > 2*PI)
		FlareSin = 0;

	if (AnimCount <= 0.0f)
	{
		// Winkel zum Spieler ausrechnen
		//
		float dx, dy;

		// Abstände berechnen
		dx = (xPos + 6) - (pAim->xpos + 35);
		dy = (yPos + 12) - (pAim->ypos + 20);

		// Division durch Null verhinden
		if (dy == 0.0f)
			dy = 0.01f;

		float w;

		w = float(atan(dx / dy) * 360.0f / (D3DX_PI * 2));

		if (dx >= 0 && dy >= 0) rot = w;		  else
		if (dx > 0  && dy < 0 ) rot = 180 + w;  else
		if (dx < 0  && dy > 0 ) rot = 360 + w;  else
		if (dx < 0  && dy < 0 ) rot = 180 + w;
		rot = 360.0f - rot;

		// Auf Spieler zubewegen
		//
		if (xPos + 6 < pAim->xpos + 35) 
			xAcc =  2.5f;
		else
			xAcc = -2.5f;

		if (yPos + 12 < pAim->ypos + 40) 
			yAcc =  2.5f;
		else
			yAcc = -2.5f;

		if (xSpeed < -10.0f) xSpeed = -10.0f;
		if (xSpeed >  10.0f) xSpeed =  10.0f;
		if (ySpeed < -10.0f) ySpeed = -10.0f;
		if (ySpeed >  10.0f) ySpeed =  10.0f;
	}

	// Rauchen lassen
	SmokeDelay -= 1.0f SYNC;

	while (SmokeDelay < 0.0f)
	{
		SmokeDelay += 0.5f;
		pPartikelSystem->PushPartikel(xPos - 8 + float(sin((360 - rot) / 180.0f * PI) * 15.0f), 
									  yPos - 8 + float(cos((360 - rot) / 180.0f * PI) * 15.0f), SMOKE);
	}

	// Spieler getroffen?
	// Dann explodieren lassen und Energy abziehen
	//
	TestDamagePlayers(8.0f, true);

	// an der Wand explodieren
	if (blockl & BLOCKWERT_WAND ||
		blockr & BLOCKWERT_WAND ||
		blocko & BLOCKWERT_WAND ||
		blocku & BLOCKWERT_WAND)
		Energy = 0.0f;
}

// --------------------------------------------------------------------------------------
// FetteRakete explodiert
// --------------------------------------------------------------------------------------

void GegnerMiniRocket::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, 128, 8000 + rand ()%4000, SOUND_EXPLOSION1);

	int i = 0;
	for (i=0; i < 5; i++)	
		pPartikelSystem->PushPartikel (xPos + rand ()%20, yPos - 10 + rand()%20, SMOKE);

	for (i=0; i < 5; i++)	
		pPartikelSystem->PushPartikel (xPos + rand ()%20, yPos - 10 + rand()%20, MINIFLARE);

	pPlayer[0]->Score += 50;
}
