// --------------------------------------------------------------------------------------
// Die riesen Piranha
//
// im Tempel Level
// Spuckt kleine Piranhas aus
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_BigFish.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerBigFish::GegnerBigFish(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_NOTVISIBLE;
	BlickRichtung	= LINKS;
	Energy			= 2000;
	Value1			= Wert1;
	Value2			= Wert2;
	AnimPhase		= 0;
	ChangeLight		= Light;
	Destroyable		= true;
	TestBlock		= false;
	OwnDraw			= true;
	SinOff			= 0.0f;
	MaulWinkel	    = 0.0f;
	ShotCount		= 0;
	Moving          = false;
	NewX			= 0.0f;
	NewY			= 0.0f;
	rotz			= 0.0f;
	KugelCount		= 150.0f;

	// Zusatzgrafiken laden
	Maul.LoadImage("BigFishMaul.bmp", 106, 55, 106, 55, 1, 1);
	FlosseKlein.LoadImage("bigfishflosseklein.bmp", 66, 38, 66, 38, 1, 1);
	FlosseGross.LoadImage("bigfishflossebig.bmp", 88, 157, 88, 157, 1, 1);
	FlosseOben.LoadImage("bigfishflosseoben.bmp", 74, 59, 74, 59, 1, 1);
	FlosseUnten.LoadImage("bigfishflosseunten.bmp", 97, 72, 97, 72, 1, 1);

}

// --------------------------------------------------------------------------------------
// An neue Position moven
// --------------------------------------------------------------------------------------

void GegnerBigFish::MoveTo(float x, float y)
{
	Moving = true;

	NewX = x;
	NewY = y;

	xSpeed = ySpeed = 0.0f;

	if (xPos < x) xSpeed =  10.0f;
	if (xPos > x) xSpeed = -10.0f;
	if (yPos < y) ySpeed =  10.0f;
	if (yPos > y) ySpeed = -10.0f;
}

void GegnerBigFish::DoMove(void)
{
	if (xSpeed < 0.0f &&
		NewX > xPos)
	{
		xSpeed += 1.0f SYNC;

		if (xSpeed > 0.0f)
			xSpeed = 0.0f;
	}

	if (xSpeed > 0.0f &&
		NewX < xPos)
	{
		xSpeed -= 1.0f SYNC;

		if (xSpeed < 0.0f)
			xSpeed = 0.0f;
	}

	if (ySpeed < 0.0f &&
		NewY > yPos)
	{
		ySpeed += 1.0f SYNC;

		if (ySpeed > 0.0f)
			ySpeed = 0.0f;
	}

	if (ySpeed > 0.0f &&
		NewY < yPos)
	{
		ySpeed -= 1.0f SYNC;

		if (ySpeed < 0.0f)
			ySpeed = 0.0f;
	}

	if (xSpeed == 0.0f &&
		ySpeed == 0.0f)
	{
		Moving = false;
	}
}

// --------------------------------------------------------------------------------------
// Neue Aktion starten
// --------------------------------------------------------------------------------------

void GegnerBigFish::NewAction(void)
{
	static int lastaction = -1;

	// Spieler rechts vom Fisch? Dann den Fisch bewegen
	if (pAim->xpos > xPos)
	{
		float newx = pAim->xpos;

		if (newx > Value1 + 400)
			newx = (float)Value1 + 400;

		MoveTo(newx, (float)Value2 + 200);
	}

	// neue Aktion festlegen
	int j = rand()%3;

	while (lastaction == j)
		j = rand()%3;

	lastaction = j;

	switch(j)
	{
		case 0:
		{
			ShotCount = 1;
			Handlung = GEGNER_SPECIAL;
		} break;

		case 1:
		{
			ShotCount = 5;
			Handlung = GEGNER_SCHIESSEN;
		} break;

		case 2:
		{
			ShotCount = 3;
			Handlung = GEGNER_LAUFEN;
		} break;
	}
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerBigFish::DoDraw(void)
{	
	static float FlossenSin = 0.0f;
	static float FlossenSin2 = 0.0f;
	float mw = (float)(sin(-PI/2 + MaulWinkel) * 35.0f);
	float fs = (float)sin(FlossenSin) * 40.0f;
	float fs2 = (float)sin(FlossenSin2) * 10.0f;
	float YOff = (float)(sin(SinOff) * 5.0f);
	
	// Maul rendern
	Maul.RenderSpriteRotatedOffset((float)(xPos-pTileEngine->XOffset) + 51.0f,
								   (float)(yPos-pTileEngine->YOffset) + YOff + 94.0f, 
								   mw,
								   12, -17, 0xFFFFFFFF, false);

	// Schwanzflosse rendern
	FlosseGross.RenderSpriteScaled((float)(xPos-pTileEngine->XOffset) + 203.0f,
								   (float)(yPos-pTileEngine->YOffset) + YOff - 9.0f, 
								   88 + (int)fs2, 157, 0, 0xFFFFFFFF);

	// obere Floße rendern
	FlosseOben.RenderSpriteScaled((float)(xPos-pTileEngine->XOffset) + 151.0f,
								  (float)(yPos-pTileEngine->YOffset) + YOff - 20.0f + (int)fs2, 
								  74, 59 -  (int)fs2, 0, 0xFFFFFFFF);

	// untere Floße rendern
	FlosseUnten.RenderSpriteScaled((float)(xPos-pTileEngine->XOffset) + 140.0f,
			 					   (float)(yPos-pTileEngine->YOffset) + YOff + 120.0f, 
								   97, 72  + (int)fs2, 0, 0xFFFFFFFF);

	// Fisch rendern
	pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset),
										   (float)(yPos-pTileEngine->YOffset) + YOff, 
										   0, 0xFFFFFFFF, false);

	// kleine Floße rendern
	FlosseKlein.RenderSpriteRotatedOffset((float)(xPos-pTileEngine->XOffset) + 184.0f,
										  (float)(yPos-pTileEngine->YOffset) + YOff + 80.0f, 
										  fs,
										  -28, -9, 0xFFFFFFFF, false);	


	if (AlreadyDrawn == false)
	{
		DirectGraphics.SetAdditiveMode();

		// Leuchtflare rendern
		D3DCOLOR col;
		int a;

		a = (int)(Energy / 2000.0f * 255.0f);

		col = D3DCOLOR_RGBA(255, 224, 80, a);

		for (int i = 0; i < 2; i++)
		LavaFlare.RenderSprite((float)(xPos-pTileEngine->XOffset) - 49,
							   (float)(yPos-pTileEngine->YOffset) - 22 + YOff, 
							   0, col, false);

		// Flossen bewegen		
		float fact;

		fact = (xSpeed * xSpeed + ySpeed * ySpeed + 20.0f) / 200.0f;


		FlossenSin += fact SYNC;
		if (FlossenSin > 2 * PI)
			FlossenSin = 0.0f;

		FlossenSin2 += fact SYNC;
		if (FlossenSin2 > 2 * PI)
			FlossenSin2 = 0.0f;
	}

	//DirectGraphics.SetColorKeyMode();
	AlreadyDrawn = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerBigFish::DoKI(void)
{
	static bool IsKugel = false;

	KugelCount -= 1.0f SYNC;

	if (KugelCount <= 0.0f)
	{		
		int LeftOrRight = 1;
		int Art = 2;
		KugelCount = 80.0f;
		
		if (Energy > 500)	
			Art = 1;

		if (Energy > 1500)	
			Art = 0;
		
		if (rand()%2 == 0)
			LeftOrRight *= -1;
		
		if (IsKugel) 
			pGegner->PushGegner((float)pTileEngine->XOffset + 300.0f + 300 * LeftOrRight,
								(float)pTileEngine->YOffset + 200, KUGELKLEIN + Art, 5, 0, false);
		else
			pGegner->PushGegner((float)pTileEngine->XOffset + 300.0f + 320 * LeftOrRight,
								(float)pTileEngine->YOffset + 350, SWIMWALKER, 5, 0, false);
		IsKugel = !IsKugel;

	}

	// Schwimmbewegung
	SinOff += 0.2f SYNC;

	if (SinOff > 2*PI)
		SinOff = 0.0f;

	// Energie anzeigen
	if (Handlung != GEGNER_INIT && Handlung != GEGNER_EXPLODIEREN)
		pHUD->ShowBossHUD(2000, Energy);

	// Levelausschnitt auf den Boss zentrieren, sobald dieser sichtbar wird
	if (Active == true && 
		Handlung != GEGNER_EXPLODIEREN &&
		pTileEngine->Zustand == ZUSTAND_SCROLLBAR && 
		Energy > 0.0f)
	{
		pTileEngine->ScrollLevel((float)Value1, 
								 (float)Value2, ZUSTAND_SCROLLTOLOCK);		// Level auf die Faust zentrieren
		pSoundManager->FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
		xPos  += 400;
	}

	// Zwischenboss blinkt nicht so lange wie die restlichen Gegner
	if (DamageTaken > 0.0f)
		DamageTaken -= 100 SYNC;				// Rotwerden langsam ausfaden lassen
	else
		DamageTaken = 0.0f;						// oder ganz anhalten

	// Schon schwer angeschlagen ? Dann blutet der Fish
	if (Energy < 1000 &&
		rand()%20 == 0)
		pPartikelSystem->PushPartikel(xPos + rand()%200+40, yPos + 50 + rand()%100, PIRANHABLUT);

	// Hat der Boss keine Energie mehr ? Dann explodiert er
	if (Energy <= 100.0f && Handlung != GEGNER_EXPLODIEREN)
	{
		// Endboss-Musik ausfaden und abschalten
		pSoundManager->FadeSong(MUSIC_BOSS, -2.0f, 0, false);

		// zum Spieler scrollen
		ScrolltoPlayeAfterBoss();

		Handlung  = GEGNER_EXPLODIEREN;
		xSpeed    = 0.0f;
		ySpeed    = 2.0f;
		xAcc      = 0.0f;
		yAcc      = 1.0f;
		AnimCount = 50.0f;

		Moving = false;		
	}

	// Je nach Handlung richtig verhalten
	if (Moving == true)
		DoMove();
	else
	switch (Handlung)
	{
		case GEGNER_NOTVISIBLE:			// Warten bis der Screen zentriert wurde
		{
			if (pTileEngine->Zustand == ZUSTAND_LOCKED)
			{
				// Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
				if (FMUSIC_IsPlaying(pSoundManager->its_Songs[MUSIC_BOSS]->SongData) == false)
					pSoundManager->PlaySong(MUSIC_BOSS, false);

				// Und Boss erscheinen lassen				
				Handlung = GEGNER_EINFLIEGEN;
			}
		} break;

		case GEGNER_EINFLIEGEN:		// Gegner kommt DAS ERSTE MAL in den Screen geflogen
		{
			Energy = 2000;
			DamageTaken = 0.0f;

			NewAction();
			MoveTo((float)Value1 + 450, yPos);
		} break;

		case GEGNER_LAUFEN:
		{
			MoveTo((float)Value1 + 50 + rand()%300, (float)Value2 + 50 + rand()%200);
			ShotCount--;

			if (ShotCount <= 0)
				NewAction();
		} break;

		// einzelne Fische ausspucken
		case GEGNER_SCHIESSEN:
		{
			static bool shot = false;

			// und Maulbewegung
			MaulWinkel += 0.6f SYNC;

			if (MaulWinkel > PI &&
				shot == false)
			{
				shot = true;
				pGegner->PushGegner(xPos + 80.0f, yPos + 90.0f, PIRANHA, 99, 1, true);
				ShotCount--;
			}

			if (MaulWinkel > 2*PI)
			{
				shot = false;
				MaulWinkel = 0.0f;
				pSoundManager->PlayWave(50, 128, 6000 + rand()%500, SOUND_KLONG);

				if (ShotCount <= 0)
					NewAction();
			}

		} break;

		// mehrere Fische ausspucken
		case GEGNER_SPECIAL:
		{
			static bool shot = false;

			// und Maulbewegung
			MaulWinkel += 0.1f SYNC;

			if (MaulWinkel > PI &&
				shot == false)
			{
				shot = true;

				for (int i = 0; i < 9; i++)
					pGegner->PushGegner(xPos + 80.0f, yPos + 90.0f, PIRANHA, 98, i-4, true);

				ShotCount--;
			}

			if (MaulWinkel > 2*PI)
			{
				shot = false;
				MaulWinkel = 0.0f;
				pSoundManager->PlayWave(50, 128, 6000 + rand()%500, SOUND_KLONG);

				if (ShotCount <= 0)
					NewAction();
			}

		} break;

		// Boss fliegt in die Luft
		case GEGNER_EXPLODIEREN:
		{
			Energy = 100;
			if (yPos > Value2 + 300.0f)
				Energy = 0.0f;
		} break;

		default : break;
	} // switch

	// Testen, ob der Fisch den Spieler berührt hat
	GegnerRect[BIGFISH].left   = 60;	
	GegnerRect[BIGFISH].right  = 280;
	GegnerRect[BIGFISH].top    = 28   + (int)(sin(SinOff) * 5.0f);
	GegnerRect[BIGFISH].bottom = 156 + (int)(sin(SinOff) * 5.0f);

	TestDamagePlayers(8.0f SYNC);

	// Normales Cliprect setzen
	GegnerRect[BIGFISH].left   = 0;	
	GegnerRect[BIGFISH].right  = 17;
	GegnerRect[BIGFISH].top    = 28 + (int)(sin(SinOff) * 5.0f);
	GegnerRect[BIGFISH].bottom = 50 + (int)(sin(SinOff) * 5.0f);
}

// --------------------------------------------------------------------------------------
// Explodieren
// --------------------------------------------------------------------------------------

void GegnerBigFish::GegnerExplode(void)
{
	pSoundManager->PlayWave(100, 128, 11025, SOUND_EXPLOSION2);

	// Blut
	for (int i=0; i<80; i++)
		pPartikelSystem->PushPartikel(xPos + rand()%200+40, 
									  yPos + 50 + rand()%100, 
									  PIRANHABLUT);

	for (int i=0; i<20; i++)
		pGegner->PushGegner(xPos + rand()%200+40, 
							yPos + 30 + rand()%80, PIRANHA, 99, 0, false);

	for (int i = 0; i < NUMPLAYERS; i++)
		DirectInput.Joysticks[pPlayer[i]->JoystickIndex].ForceFeedbackEffect(FFE_BIGRUMBLE);

	ShakeScreen(4);

	pPlayer[0]->Score += 5000;
}
