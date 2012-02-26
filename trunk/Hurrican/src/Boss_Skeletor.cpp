// --------------------------------------------------------------------------------------
// Der letzte Endboss ... Robo-Skeletor :)
//
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_Skeletor.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSkeletor::GegnerSkeletor(int Wert1, int Wert2, bool Light)
{
	AnimPhase		= 10;
	Handlung		= GEGNER_NOTVISIBLE;
	BlickRichtung	= RECHTS;
	Value1			= Wert1;
	Value2			= Wert2;
	Energy			= 7000;	
	ChangeLight		= Light;
	Destroyable		= false;
	TestBlock		= false;
	OwnDraw			= true;
	SmokeDelay		= 0.0f;
	ShotDelay		= 0.0f;
	ShotCount		= 0;
	GunWinkel		= 0.0f;
	HasLaughed		= false;
	LastHandlung	= -1;
	DrawNow			= false;

	// Zusätzliche Grafiken laden
	//
	Flamme.LoadImage ("skeletor_flame.bmp",  160, 140, 160, 70, 1, 2);
}

// --------------------------------------------------------------------------------------
// Lachen ;)
// --------------------------------------------------------------------------------------

void GegnerSkeletor::Laugh()
{
	AnimPhase = 2;
	pSoundManager->PlayWave(100, 128, 11025, SOUND_LAUGH);

	xSpeed = 0.0f;
	ySpeed = 0.0f;
	xAcc = 0.0f;
	yAcc = 0.0f;
	Handlung = GEGNER_SPECIAL;
	AnimCount = 20.0f;
}

// --------------------------------------------------------------------------------------
// Winkel zum Spieler berechnen
// --------------------------------------------------------------------------------------

void GegnerSkeletor::CalcGunWinkel()
{
	float xdiv, ydiv;

	ydiv = (pAim->ypos + 40) - (yPos + 100);
	if (ydiv == 0.0f)
		ydiv  = 0.00001f;

	xdiv = (pAim->xpos + 35) - (xPos + 60);
	GunWinkel = 90.0f + (float)atan(ydiv / xdiv) * 180.0f / PI;

	if (GunWinkel > 120) GunWinkel = 120;
	if (GunWinkel < 50)  GunWinkel = 50;

}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerSkeletor::DoDraw(void)
{
	if (DrawNow == false)
		return;

	if (Handlung == GEGNER_EXPLODIEREN)
		return;

	D3DCOLOR Color;
	float yoff = 0.0f;

	int	Wert = 255-(int(DamageTaken));
	Color = D3DCOLOR_RGBA(255, Wert, Wert, 255);
	bool mirror = BlickRichtung == RECHTS;

	if (AnimPhase >= 10)
		yoff = 10.0f;

	/*
	if (Handlung == GEGNER_SPRINGEN)
	{
		DirectGraphics.SetAdditiveMode();
		LavaFlare.RenderSpriteScaled(xPos - (float)pTileEngine->XOffset - 90,
									 yPos - (float)pTileEngine->YOffset - 50,
									 300, 300, 0, 0x88FF8822);									 

		LavaFlare.RenderSpriteScaled(xPos - (float)pTileEngine->XOffset - 40,
									 yPos - (float)pTileEngine->YOffset,
									 200, 200, 0, 0x88FFCC66);									 

		DirectGraphics.SetColorKeyMode();
	}
	*/

	pGegnerGrafix[GegnerArt]->RenderSprite(xPos - (float)pTileEngine->XOffset,
										   yPos - (float)pTileEngine->YOffset + yoff,
										   AnimPhase, Color, mirror);

	if (Handlung == GEGNER_SPECIAL2)
	{
		for (int i = 0; i < 4; i++)
		{
			D3DCOLOR col = D3DCOLOR_RGBA(255, 255, 255, 200 - i * 30);
			pGegnerGrafix[GegnerArt]->RenderSprite(xPos - (float)pTileEngine->XOffset - (xSpeed * i * 2),
												   yPos - (float)pTileEngine->YOffset + yoff,
												   AnimPhase, col, mirror);
		}
	}

	// Flamme rendern	
	if (Handlung == GEGNER_SCHIESSEN &&
		ShotDelay > 0.5f)
	{
		DirectGraphics.SetAdditiveMode();

		int foff = -120;
		if (mirror)
			foff = 87;

		Flamme.itsRect = Flamme.itsPreCalcedRects[ShotCount % 2];

		if (BlickRichtung == RECHTS)
			Flamme.RenderSpriteRotatedOffset(xPos - (float)pTileEngine->XOffset + foff, 
										     yPos - (float)pTileEngine->YOffset + 60, 90 - GunWinkel,
											 -50, 0, 0xFFFFFFFF, !mirror);
		else
			Flamme.RenderSpriteRotatedOffset(xPos - (float)pTileEngine->XOffset + foff, 
										     yPos - (float)pTileEngine->YOffset + 60, GunWinkel - 90,
											 -50, 0, 0xFFFFFFFF, !mirror);

		DirectGraphics.SetColorKeyMode();
	}

}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSkeletor::DoKI(void)
{	
	CalcGunWinkel();

	// Energie anzeigen
	if (Handlung != GEGNER_NOTVISIBLE && Handlung != GEGNER_EXPLODIEREN)
		pHUD->ShowBossHUD(7000, Energy);

	// Boss aktivieren und Stagemusik ausfaden
	//
	if (Active == true && pTileEngine->Zustand == ZUSTAND_SCROLLBAR)
	{
		// Ausserhalb des Screens setzen
		xPos = (float)pTileEngine->XOffset + 100.0f;
		yPos = (float)pTileEngine->YOffset - 250.0f;
		DrawNow = true;

		pTileEngine->ScrollLevel((float)Value1, 
								 (float)Value2, ZUSTAND_SCROLLTOLOCK);		// Level auf den Boss zentrieren

		pSoundManager->FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren		
	}

	// Je nach Handlung richtig verhalten
	//
	switch (Handlung)
	{
		// Abwarten nach dem Lachen
		case GEGNER_SPECIAL:
		{
			AnimCount -= 1.0f SYNC;
			if (AnimCount < 0.0f)
			{
				Handlung = GEGNER_STEHEN;
			}
		} break;

		case GEGNER_CRUSHENERHOLEN:
		{
			AnimCount += 1.0f SYNC;
	
			if (AnimCount > 10.0f && 
				AnimPhase == 0)
			{
				//pSoundManager->PlayWave(100, 128, 11025, SOUND_STANDUP);
				AnimPhase = 1;
			}

			if (AnimCount > 20.0f &&
				AnimPhase == 1)
			{
				AnimPhase = 2;

				if (HasLaughed == false)
					pSoundManager->PlayWave(100, 128, 11025, SOUND_LAUGH);				
			}

			if (AnimCount > 25.0f)
			{				
				Handlung = GEGNER_STEHEN;
				if (HasLaughed == false)
				{
					HasLaughed = true;
					pSoundManager->PlaySong(MUSIC_BOSS, false);
				}
			}

		} break;

		case GEGNER_STEHEN:
		{
			int j = rand()%4;

			while (j == LastHandlung)
				j = rand()%4;

			LastHandlung = j;

			switch(j)
			{
				case 0:
				{
					ShotDelay = 0.0f;
					ShotCount = 40;

					Handlung = GEGNER_SCHIESSEN;
					AnimOffset = 0;
				} break;

				case 1:	
				{
					ShotDelay = 0.0f;
					ShotCount = 20;

					Handlung = GEGNER_BOMBARDIEREN;
					AnimOffset = 0;
				} break;

				case 2:	
				{
					ShotDelay = 3.0f;
					ShotCount = 1;
					Handlung = GEGNER_SPRINGEN;
					xSpeed = 25.0f;
					ySpeed = -60.0f;
					yAcc   = 6.0f;			
				} break;

				case 3:
				{
					Handlung = GEGNER_SPECIAL2;
					xSpeed = 0.0f;
					xAcc   = 2.0f;
					ShotDelay = 9.0f;
					ShotCount = 5;
				}
			}

			if (xPos > Value1 + 320)
			{
				xSpeed *= -1;
				xAcc *= -1;
			}
		} break;

		case GEGNER_SPECIAL2:
		{
			// Rakete schiessen
			if (ShotCount > 0)
				ShotDelay -= 1.0f SYNC;

			if (ShotDelay < 0.0f)
			{
				WinkelUebergabe = 2.0f * ShotCount + 8.0f;
				pSoundManager->PlayWave(100, 128, 10000 + rand()%2000, SOUND_ROCKET);
				pProjectiles->PushProjectile(xPos + 50.0f, yPos + 80.0f, TURRIEXTRAWURST);
													
				ShotDelay = 3.0f;
				ShotCount--;
			}

			// Animationsphase setzen
			int a;			

			if ((xSpeed >  31.0f &&
				 xAcc > 0.0f) ||
				(xSpeed < -31.0f &&
				 xAcc < 0.0f))
				xAcc *= -1.0f;			

			if ((xAcc > 0.0f && xSpeed < 0.0f && xSpeed > -2.0f) ||
				(xAcc < 0.0f && xSpeed > 0.0f && xSpeed <  2.0f))
				xAcc = 0.0f;

			if (xPos + 60 < Value1 + 320)
			{
				a = (int)(xPos - pTileEngine->XOffset) / 56;
				BlickRichtung = RECHTS;
				AnimPhase = 10 + a;
			}
			else
			{
				a = (int)(xPos - pTileEngine->XOffset - 320 + 60) / 56;
				BlickRichtung = LINKS;
				AnimPhase = 15 - a;
			}

			// Angekommen?
			if (xPos < Value1 + 20.0f ||
				xPos > Value1 + 500)
			{
				if (xPos < Value1 + 50)
					xPos = Value1 + 20.0f ;
				else
					xPos = Value1 + 500.0f;

				xSpeed = 0.0f;
				ySpeed = 0.0f;
				xAcc = 0.0f;
				yAcc = 0.0f;

				Laugh();
			}
		} break;

		case GEGNER_SPRINGEN:
		{
			// Animationsphase setzen
			int a;			
			
			if (xPos + 60 < Value1 + 320)
			{
				a = (int)(xPos - pTileEngine->XOffset) / 56;
				BlickRichtung = RECHTS;
				AnimPhase = 10 + a;
			}
			else
			{
				a = (int)(xPos - pTileEngine->XOffset - 320 + 60) / 56;
				BlickRichtung = LINKS;
				AnimPhase = 15 - a;
			}

			// 5er Schuss abgeben?
			if (ShotCount == 1)
			{
				if (xPos + 62.0f>  Value1 + 300 &&
					xPos + 62.0f < Value1 + 340)
				{
					pSoundManager->PlayWave(100, 128, 8000 + rand()%2000, SOUND_FIREBALL);
					ShotCount = 0;

					for (int i = 0; i < 5; i++)
					{
						WinkelUebergabe = 180.0f - 50 + i * 25;

						pProjectiles->PushProjectile(xPos + 50, yPos + 70, FIREBALL_BIG);
					}
				}
			}
			
			// Aufgekommen?
			if (yPos > Value2 + 250)
			{
				yPos = (float)Value2 + 250;
				xSpeed = 0.0f;
				ySpeed = 0.0f;
				xAcc = 0.0f;
				yAcc = 0.0f;

				AnimPhase = 0;
				ySpeed = 0.0f;
				Handlung = GEGNER_CRUSHENERHOLEN;
				AnimCount = 0.0f;

				for (int i = 0; i < 10; i++)
					pPartikelSystem->PushPartikel(xPos + rand()%100 - 20, yPos + rand()%20 + 130, SMOKEBIG);

				for (int i = 0; i < 10; i++)
					pPartikelSystem->PushPartikel(xPos + rand()%100, yPos + 180 + rand()%10, SPIDERSPLITTER);

				ShakeScreen(5.0f);
				pSoundManager->PlayWave(100, 128, 11025, SOUND_DOORSTOP);
			}
		} break;

		// Granaten
		case GEGNER_BOMBARDIEREN:
		{
			ShotDelay -= 0.1f SYNC;
			
			AnimCount -= 1.0f SYNC;

			if (AnimCount <= 0.0f)
			{				
				AnimOffset++;

				if (AnimOffset % 2 == 0)
					AnimCount = 2.0f;
				else
				{
					// Granate abfeuern
					int off = 0;

					if (BlickRichtung == LINKS)
						off = -95;

					AnimCount = 1.5f;
					pSoundManager->PlayWave(100, 128, 6000 + rand()%2000, SOUND_GRANATE);
					pPartikelSystem->PushPartikel(xPos + 50 + off, yPos + 35, EXPLOSIONFLARE);
					pPartikelSystem->PushPartikel(xPos + 50 + off, yPos + 35, EXPLOSIONFLARE);

					WinkelUebergabe = 40.0f - (float)(AnimPhase) * 4 - rand()%8;

					if (BlickRichtung == LINKS)
						WinkelUebergabe += 1;

					if (WinkelUebergabe < 0.0f)
						WinkelUebergabe = 0.0f;

					WinkelUebergabe *= BlickRichtung;

					pProjectiles->PushProjectile(xPos + 100 + off, yPos + 80, SKELETORGRANATE);

					ShotCount--;				
				}
				
				int a = AnimOffset % 14;

				if (a < 8)
					AnimPhase = a + 2;
				else
					AnimPhase = 16 - a;

				if (AnimOffset > 42)
					AnimOffset = 0;

				// fertig?
				if (ShotCount < 0)
					Laugh();
			}			
		} break;

		// Gatling
		case GEGNER_SCHIESSEN:
		{
			ShotDelay -= 0.5f SYNC;

			// Animationsphase setzen
			if (ShotDelay > 0.5f)
				AnimPhase = 3;
			else
				AnimPhase = 2;

			// schuss abgeben
			if (ShotDelay <= 0.0f)
			{											
				ShotCount--;

				ShotDelay = 1.0f;

				// Hülse
				int off = 70;
				if (BlickRichtung == LINKS)
					off = 30;

				pPartikelSystem->PushPartikel(xPos + off, yPos + 75, BULLET_SKELETOR);				

				// Sound
				pSoundManager->PlayWave(100, 128, 10000 + rand()%400, SOUND_GATLING);
				
				// Schuss
				WinkelUebergabe = GunWinkel + rand()%4 - 2;

				if (BlickRichtung == LINKS)
					WinkelUebergabe += 180;

				off = 100;

				if (BlickRichtung == LINKS)
					off = 20;

				pProjectiles->PushProjectile(xPos + off, yPos + 75, SKELETOR_SHOT);

				// fertig?
				if (ShotCount < 0)
					Laugh();
			}
		} break;

		case GEGNER_NOTVISIBLE:			// Warten bis der Screen zentriert wurde
		{
			if (pTileEngine->Zustand == ZUSTAND_LOCKED)
			{
				// Boss erscheinen lassen
				//				
				pSoundManager->PlayWave(100, 128, 11025, SOUND_STONEFALL);
				Handlung = GEGNER_EINFLIEGEN;
				xSpeed = 0.0f;
				ySpeed = 80.0f;
				Destroyable = true;
			}
		} break;

		case GEGNER_EINFLIEGEN:		// Kopf erhebt sich aus dem Schrotthaufen
		{
			BlickRichtung	= RECHTS;

			if (yPos > Value2 + 250)
			{
				yPos = (float)Value2 + 250;
				AnimPhase = 0;
				ySpeed = 0.0f;
				Handlung = GEGNER_CRUSHENERHOLEN;
				AnimCount = 0.0f;

				for (int i = 0; i < 50; i++)
					pPartikelSystem->PushPartikel(xPos + rand()%100, yPos + rand()%150 + 30, SMOKEBIG);

				for (int i = 0; i < 20; i++)
					pPartikelSystem->PushPartikel(xPos + rand()%100, yPos + 180 + rand()%10, SPIDERSPLITTER);

				ShakeScreen(5.0f);
				pSoundManager->PlayWave(100, 128, 11025, SOUND_DOORSTOP);
			}
		} break;
		
		case GEGNER_EXPLODIEREN:
		{
			ShotDelay -= 1.0f SYNC;

			if (ShotDelay < 0.0f)
				Energy = 0.0f;

		} break;

		default : break;
	} // switch

	if (Energy < 0.0f &&
		Handlung != GEGNER_EXPLODIEREN)
	{
		Energy = 1.0f;
		Destroyable = false;
		Handlung = GEGNER_EXPLODIEREN;
		ShotDelay = 20.0f;

		// Schädel spawnen
		pGegner->PushGegner(xPos + 50, yPos + 50, SKULL, 0, 99, false);

		// Explosionen
		int i = 0;		

		for (int i = 0; i < 15; i++)
			pPartikelSystem->PushPartikel(xPos + rand()% 100,
										  yPos + rand()% 200,
										  SPLITTER);

		for (int i = 0; i < 40; i++)
			pPartikelSystem->PushPartikel(xPos + rand()% 200 - 50,
										  yPos + rand()% 250 - 20,
										  SPIDERSPLITTER);

		for (int i = 0; i < 40; i++)
			pPartikelSystem->PushPartikel(xPos + rand()% 200 - 50,
										  yPos + rand()% 250 - 20,
										  SCHROTT1);

		for (int i = 0; i < 60; i++)
			pPartikelSystem->PushPartikel(xPos + rand()% 200 - 50,
										  yPos + rand()% 200 - 50,
										  EXPLOSION_MEDIUM2);
		
		pPartikelSystem->PushPartikel (xPos + 62, yPos + 100, SHOCKEXPLOSION);
		pSoundManager->PlayWave(100, 128, 11025, SOUND_EXPLOSION2);
		pPlayer[0]->Score += 12500;
		ShakeScreen(5.0f);

		// Endboss-Musik ausfaden und abschalten
		pSoundManager->FadeSong(MUSIC_BOSS, -2.0f, 0, false);
	}

	TestDamagePlayers(10.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Skeletor explodiert
// --------------------------------------------------------------------------------------

void GegnerSkeletor::GegnerExplode(void)
{		
	ScrolltoPlayeAfterBoss();	
}
