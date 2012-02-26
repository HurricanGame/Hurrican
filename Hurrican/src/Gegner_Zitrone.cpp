// --------------------------------------------------------------------------------------
// Die Zitrone (Zieh Drone)
//
// Zieht dem Hurrican hinterher und ballert
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Zitrone.h"

extern DirectGraphicsSprite	*pDroneGun;					// Flamme der Zitrone

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerZitrone::GegnerZitrone(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	Energy			= 200;
	ChangeLight		= Light;
	Destroyable		= true;
	SmokeDelay		= 1.0f;
	WackelOffset	= 0.0f;
	OwnDraw			= true;
	KnarreWinkel	= 0.0f;
	ShotDelay		= 0.0f;
	AlreadyDrawn	= false;	

	FollowCount = 0.0f;
}

// --------------------------------------------------------------------------------------
// Knarre zeigt auf Hurrican
// --------------------------------------------------------------------------------------

void GegnerZitrone::CalcKnarreWinkel(void)
{
	float xdiv, ydiv, newwinkel;

	ydiv = (pAim->ypos + 40) - (yPos + 40);

	if (ydiv == 0.0f)
		ydiv  = 0.00001f;

	xdiv = (pAim->xpos + 35) - (xPos + 60);	

	newwinkel = (float)atan(xdiv / ydiv) * 180.0f / D3DX_PI + 180.0f;

	if (xdiv >= 0 && ydiv >= 0) newwinkel = newwinkel;		  else
	if (xdiv > 0  && ydiv < 0 ) newwinkel = 180 + newwinkel;  else
	if (xdiv < 0  && ydiv > 0 ) newwinkel = 360 + newwinkel;  else
	if (xdiv < 0  && ydiv < 0 ) newwinkel = 180 + newwinkel;

	// Winkel begrenzen
	while (newwinkel > 360.0f) newwinkel -= 360.0f;
	while (newwinkel <   0.0f) newwinkel += 360.0f;

	if (newwinkel < 100.0f)	newwinkel = 100.0f;
	if (newwinkel > 260.0f)	newwinkel = 260.0f;

	if (KnarreWinkel < newwinkel) KnarreWinkel += 20.0f SYNC;
	if (KnarreWinkel > newwinkel) KnarreWinkel -= 20.0f SYNC;
}

// --------------------------------------------------------------------------------------
// Rendern
//
// Flares links und rechts am Antrieb werden je nach Animationsphase
// vor oder hinter dem Gegner gerendert
//
// --------------------------------------------------------------------------------------

void GegnerZitrone::DoDraw(void)
{
	int a;

	a = AnimPhase;

	if (a < 3) a = 3;
	if (a > 7) a = 7;

	float yoff = (float)(sin(WackelOffset) * 10.0f);

	if (AlreadyDrawn == false)
	{
		DirectGraphics.SetAdditiveMode();
		if (AnimPhase >= 5)
			LavaFlare.RenderSprite ((float)-pTileEngine->XOffset + xPos - 60 + a * 4, 
									(float)-pTileEngine->YOffset + yPos - 10 + yoff, 0, 0xBBFFAA66);

		if (AnimPhase <= 5)
			LavaFlare.RenderSprite ((float)-pTileEngine->XOffset + xPos + 40 + (a - 5) * 4, 
									(float)-pTileEngine->YOffset + yPos - 10 + yoff, 0, 0xBBFFAA66);
		DirectGraphics.SetColorKeyMode();
	}

	// Knarre	
	pDroneGun->RenderSpriteRotatedOffset((float)-pTileEngine->XOffset + xPos + 73.0f - (a) * 5,
										 (float)-pTileEngine->YOffset + yPos + 48.0f + yoff, KnarreWinkel, 
										 0, 0, 0xFFFFFFFF);


	// Körper	
	pGegnerGrafix[GegnerArt]->RenderSprite((float)-pTileEngine->XOffset + xPos,
										   (float)-pTileEngine->YOffset + yPos + yoff,
										   AnimPhase, 0xFFFFFFFF);

	if (AlreadyDrawn == false)
	{
		DirectGraphics.SetAdditiveMode();
		if (AnimPhase < 5)
			LavaFlare.RenderSprite ((float)-pTileEngine->XOffset + xPos - 70 + a * 4, 
									(float)-pTileEngine->YOffset + yPos - 10 + yoff, 0, 0xBBFFAA66);

		if (AnimPhase > 5)
			LavaFlare.RenderSprite ((float)-pTileEngine->XOffset + xPos + 60 - (a - 5) * 4, 
									(float)-pTileEngine->YOffset + yPos - 10 + yoff, 0, 0xBBFFAA66);
		DirectGraphics.SetColorKeyMode();
	}

	AlreadyDrawn = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerZitrone::DoKI(void)
{	
	blocko = pTileEngine->BlockOben(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt], true);
	blocku = pTileEngine->BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt], true);
	blockl = pTileEngine->BlockLinks(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt], true);
	blockr = pTileEngine->BlockRechts(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt], true);

	CalcKnarreWinkel();

	// Wackeln lassen
	WackelOffset += 0.25f SYNC;
	while (WackelOffset > 2*PI)
		WackelOffset -= 2*PI;

	// Richtige Animationsphase setzen
	// abhängig von der Position zum Spieler
	// 
	float dummy;
	dummy = (pAim->xpos + 35) - (xPos + 57);
	dummy /= 50.0f;

	AnimPhase = 5 - (int)dummy;

	if(AnimPhase > 10)
	   AnimPhase = 10;

	if(AnimPhase < 0)
	   AnimPhase = 0;

	if (blockl & BLOCKWERT_WAND ||
		blockr & BLOCKWERT_WAND)
		xSpeed = 0.0f;

	if (blocko & BLOCKWERT_WAND ||
		blocku & BLOCKWERT_WAND)
		ySpeed = 0.0f;

	switch (Handlung)
	{
		// In der Luft rumdümpeln
		//
		case GEGNER_STEHEN:
		{
			// Abstand zu groß? 
			// Oder getroffen?
			// Dann gleich hinterherflitzen			
			if (PlayerAbstand() > 300)// ||
//				DamageTaken > 0.0f)
				FollowCount = 0.0f;

			// neuen punkt aussuchen?
			//
			FollowCount -= 1.0f SYNC;

			if (FollowCount < 0.0f)
			{
				FollowCount = 20.0f;

				int dx;

				dx = rand()%60 + 30;
				if (rand()%2 == 0)
					dx *= -1;

				NewX = pAim->xpos + dx;
				NewY = pAim->ypos - rand()%80 - 80.0f;

				if (NewX > xPos)
					xAcc = 5.0f;
				else
					xAcc = -5.0f;

				if (NewY > yPos)
					yAcc = 5.0f;
				else
					yAcc = -5.0f;

				Handlung = GEGNER_LAUFEN;
			}

			// Schiessen
			ShotDelay -= 1.0f SYNC;

			if (ShotDelay < 0.0f)
			{
				ShotDelay = 5.0f;

				WinkelUebergabe = 360.0f - KnarreWinkel;
				pProjectiles->PushProjectile(xPos + 53.0f - (float)sin((180 - KnarreWinkel) / 180.0f * PI) * 45.0f, 
											 yPos + 56.0f + (float)cos((180 - KnarreWinkel) / 180.0f * PI) * 45.0f + (float)(sin(WackelOffset) * 10.0f), FLUGLASER);

				pSoundManager->PlayWave(100, 128, 24000 + rand()%500, SOUND_LASERSHOT);
			}
		} break;

		// Zu neuem Punkt hinfliegen
		//
		case GEGNER_LAUFEN:
		{	
			// speed begrenzen
			if (xSpeed >  20.0f) xSpeed =  20.0f;
			if (xSpeed < -20.0f) xSpeed = -20.0f;

			if (ySpeed >  20.0f) ySpeed =  20.0f;
			if (ySpeed < -20.0f) ySpeed = -20.0f;

			// xpunkt erreicht?
			if (xSpeed < 0.0f && xPos < NewX)
			{				
				xAcc   = 0.0f;

				xSpeed += 7.0f SYNC;
				if (xSpeed > 0.0f)
					xSpeed = 0.0f;
			}

			if (xSpeed > 0.0f && xPos > NewX)
			{				
				xAcc   = 0.0f;

				xSpeed -= 7.0f SYNC;
				if (xSpeed < 0.0f)
					xSpeed = 0.0f;
			}

			// ypunkt erreicht?
			if (ySpeed < 0.0f && yPos < NewY)
			{				
				yAcc   = 0.0f;

				ySpeed += 7.0f SYNC;
				if (ySpeed > 0.0f)
					ySpeed = 0.0f;
			}

			if (ySpeed > 0.0f && yPos > NewY)
			{				
				yAcc   = 0.0f;

				ySpeed -= 7.0f SYNC;
				if (ySpeed < 0.0f)
					ySpeed = 0.0f;
			}

			// Punkt komplett erreicht?
			if (xSpeed == 0.0f &&
				ySpeed == 0.0f)
			{
				Handlung = GEGNER_STEHEN;
				FollowCount = 20.0f;
				ShotDelay   = 5.0f;
			}
		} break;

		// Drone stürzt ab
		//
		case GEGNER_FALLEN:
		{
			// An die Wand gekracht ?
			if (blockl & BLOCKWERT_WAND ||
			    blockr & BLOCKWERT_WAND ||
				blocko & BLOCKWERT_WAND ||
				blocku & BLOCKWERT_WAND ||
				blocku & BLOCKWERT_PLATTFORM)
				Energy = 0.0f;

			// Grenze der Fallgeschwindigkeit
			if (ySpeed > 35.0f)
				yAcc = 0.0f;

			// Drone rauchen lassen
			SmokeDelay -= 1.0f SYNC;
			if (SmokeDelay <= 0.0f)
			{
				SmokeDelay = 0.1f;
				pPartikelSystem->PushPartikel(xPos+rand()%30 + 20, yPos+10+rand()%40, SMOKE);
				pPartikelSystem->PushPartikel(xPos+rand()%30 + 20, yPos+10+rand()%40, SMOKE3);
			}
		} break;

	}

	// Drone stürzt ab ?
	//
	if (Energy <= 0.0f && Handlung != GEGNER_FALLEN)
	{
		Handlung = GEGNER_FALLEN;
		AnimCount = 0.0f;
		Energy	 = 200.0f;
		ySpeed   = 4.0f;
		yAcc	 = 2.0f;
		xAcc     = 0.0f;
		xSpeed   = 5.0f;

		if (rand()%2 == 0)
			xSpeed *= -1;

		pSoundManager->PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION4);

		for (int i = 0; i < 8; i++)
		{
			pPartikelSystem->PushPartikel (xPos + rand ()%80, yPos - 10 + rand ()%90, EXPLOSION_MEDIUM2);
			pPartikelSystem->PushPartikel (xPos + rand ()%80, yPos - 10 + rand ()%90, SPIDERSPLITTER);
		}	

	}
}

// --------------------------------------------------------------------------------------
// Drone explodiert
// --------------------------------------------------------------------------------------

void GegnerZitrone::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION4);

	for (int i = 0; i < 12; i++)
	{
		pPartikelSystem->PushPartikel (xPos + rand ()%80, yPos - 10 + rand ()%90, EXPLOSION_MEDIUM2);
		pPartikelSystem->PushPartikel (xPos + rand ()%80, yPos - 10 + rand ()%90, SPIDERSPLITTER);
		pPartikelSystem->PushPartikel (xPos + 10 + rand ()%80, yPos - 10 + rand ()%90, SCHROTT1);
	}

	for (int i = 0; i < 4; i++)
		pPartikelSystem->PushPartikel (xPos - 30 + rand ()%70, yPos - 30 + rand ()%80, SPLITTER);

	pPlayer[0]->Score += 400;
}
