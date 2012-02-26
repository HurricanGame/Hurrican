// --------------------------------------------------------------------------------------
// Der hingerotzte Ballerpott
//
// Schiesst genau auf den Spieler
// Ist der Spieler unterhalb des Potts, schiesst er im Bogen
// ansonsten schiesst er Laser wie die Zitrone
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Rotzpott.h"

extern DirectGraphicsSprite	*pDroneGun;					// Flamme der Zitrone

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerRotzpott::GegnerRotzpott(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	Energy			= 70;
	ChangeLight		= Light;
	Destroyable		= true;
	GunWinkel		= 0.0f;
	NewWinkel		= 0.0f;
	OwnDraw			= true;
	TestBlock		= false;
	ShotDelay		= 5.0f;
}

// --------------------------------------------------------------------------------------
// Knarre zeigt auf Hurrican
// --------------------------------------------------------------------------------------

void GegnerRotzpott::CalcKnarreWinkel(void)
{
	// Gegner auf Spieler ausrichten
	//
	float dx, dy;
	static float dir = 0.0f;

	while (GunWinkel >  360.0f) GunWinkel -= 360.0f;
	while (GunWinkel <	  0.0f) GunWinkel += 360.0f;

	if (pAim->ypos > yPos + 30)
	{
		dx = (xPos + 27) - (pAim->xpos + 35);
		dy = (yPos + 20) - (pAim->ypos + 40);

		if (dx > 0.0f)
			NewWinkel = dx * 0.12f + dy * 0.033f;
		else
			NewWinkel = dx * 0.12f - dy * 0.033f;
	}
	else
	{
		// Abstände berechnen
		dx = (xPos + 27) - (pAim->xpos + 35);
		dy = (yPos + 20) - (pAim->ypos + 40);

		// Division durch Null verhinden
		if (dy == 0.0f)
			dy = 0.01f;		

		float w;

		w = float(atan(dx / dy) * 360.0f / (D3DX_PI * 2));

		if (dx >= 0 && dy >= 0) NewWinkel = w;		  else
		if (dx > 0  && dy < 0 ) NewWinkel = 180 + w;  else
		if (dx < 0  && dy > 0 ) NewWinkel = 360 + w;  else
		if (dx < 0  && dy < 0 ) NewWinkel = 180 + w;
	}

	while (NewWinkel >  360.0f) NewWinkel -= 360.0f;
	while (NewWinkel <	  0.0f) NewWinkel += 360.0f;

	// neue Drehrichtung festlegen
	if (NewWinkel > GunWinkel)
	{
		if (NewWinkel - GunWinkel > 180.0f)
			dir = -30.0f;
		else
			dir =  30.0f;
	}

	if (NewWinkel < GunWinkel)
	{
		if (GunWinkel - NewWinkel < 180.0f)
			dir = -30.0f;
		else
			dir =  30.0f;
	}

	// Knarre an neuen Winkel annähern
	if (dir < 0.0f)
	{
		GunWinkel += dir SYNC;

		if (GunWinkel < NewWinkel)
			GunWinkel = NewWinkel;
	}
	else
	if (dir > 0.0f)
	{
		GunWinkel += dir SYNC;

		if (GunWinkel > NewWinkel)
			GunWinkel = NewWinkel;
	}
	
}

// --------------------------------------------------------------------------------------
// Draw
// --------------------------------------------------------------------------------------

void GegnerRotzpott::DoDraw(void)
{
	// Knarre	
	pDroneGun->RenderSpriteRotatedOffset((float)-pTileEngine->XOffset + xPos + 20.0f,
										 (float)-pTileEngine->YOffset + yPos - 28.0f, GunWinkel, 
										 0, 0, 0xFFFFFFFF);


	// Körper	
	pGegnerGrafix[GegnerArt]->RenderSprite((float)-pTileEngine->XOffset + xPos,
										   (float)-pTileEngine->YOffset + yPos,
										   0, 0xFFFFFFFF);

	AlreadyDrawn = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerRotzpott::DoKI(void)
{
	// Knarre ausrichten	

	// Testen, ob der Spieler den gegner berührt hat
	//
	TestDamagePlayers(2.0f SYNC);

	CalcKnarreWinkel();

	// schiessen, wenn Spieler in der Nähe und Turm kurz
	// vor finalem Winkel?
	if (PlayerAbstand() < 400 &&
		abs((int)(GunWinkel - NewWinkel)) < 10)
		ShotDelay -= 1.0f SYNC;

	if (ShotDelay <= 0.0f)
	{
		pPartikelSystem->PushPartikel(xPos - (float)sin((180 - GunWinkel) / 180.0f * PI) * 30.0f, 
									  yPos - 18.0f + (float)cos((180 - GunWinkel) / 180.0f * PI) * 38.0f, LASERFLAMEPHARAO);

		// Wie schiesst er? Im Bogen oder direkt?				
		if (pAim->ypos > yPos + 30)
		{			
			ShotDelay = 9.0f;
			pSoundManager->PlayWave(100, 128, 16000 + rand()%500, SOUND_LASERSHOT);
			WinkelUebergabe = 360.0f - GunWinkel + ((float)(rand()%10 - 5)) / 2.0f;
			pProjectiles->PushProjectile(xPos + 20.0f - (float)sin((180 - GunWinkel) / 180.0f * PI) * 25.0f, 
										 yPos - 12.0f + (float)cos((180 - GunWinkel) / 180.0f * PI) * 20.0f, ROTZSHOT);
		}
		else
		{
			ShotDelay = 7.0f;
			pSoundManager->PlayWave(100, 128, 24000 + rand()%500, SOUND_LASERSHOT);
			WinkelUebergabe = 360.0f - GunWinkel;
			pProjectiles->PushProjectile(xPos + 24.0f - (float)sin((180 - GunWinkel) / 180.0f * PI) * 45.0f, 
										 yPos - 12.0f + (float)cos((180 - GunWinkel) / 180.0f * PI) * 45.0f, FLUGLASER);
		}

	}
}


// --------------------------------------------------------------------------------------
// GegnerRotzpott explodiert
// --------------------------------------------------------------------------------------

void GegnerRotzpott::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);

	int i = 0;
	for (int i = 0; i < 8; i++)
	{
		pPartikelSystem->PushPartikel (xPos + rand ()%40-10, yPos - 10 + rand ()%20, EXPLOSION_MEDIUM2);
		pPartikelSystem->PushPartikel (xPos + rand ()%40-10, yPos - 10 + rand ()%20, SPIDERSPLITTER);
	}

	for (int i = 0; i < 4; i++)
		pPartikelSystem->PushPartikel (xPos - 10 + rand ()%40, yPos - 10 + rand ()%20, SCHROTT1);

	pPlayer[0]->Score += 350;
}
