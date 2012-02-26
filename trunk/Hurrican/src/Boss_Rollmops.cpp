// --------------------------------------------------------------------------------------
// Der rollende Mops kam in die Küche, und stahl dem Turri seine Eier
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_Rollmops.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerRollmops::GegnerRollmops(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_EINFLIEGEN;
	Energy			= 4000;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= false;
	TestBlock		= false;
	OwnDraw			= true;
	SmokeCount		= 0.0f;
	ShotDelay		= 0.0f;
	GunWinkel		= 0.0f;
	HookX			= 0.0f;
	HookY			= 0.0f;
	Schwung			= 0.0f;
	SchwungDir      = 0.0f;
	ShotCount 		= 0.0f;
	Delay			= 0.0f;

	Rollen.LoadImage("rollmops_rollen.png", 650, 260, 130, 130, 5, 2);
	Aufklappen.LoadImage("rollmops_aufklappen.png", 390, 390, 130, 130, 3, 3);
	Gun.LoadImage("rollmops_gun.png", 53, 52, 53, 52, 1, 1);

	for (int i = 0; i < NUM_KETTENGLIEDER; i++)
	{
		pGegner->PushGegner(0, 0, KETTENGLIED, 0, 0, false);
		pKettenTeile[i] = (GegnerKettenglied*)(pGegner->pEnd);
		pKettenTeile[i]->pParent = this;
	}	
}

// --------------------------------------------------------------------------------------
// Winkel zum Spieler ausrechnen
// --------------------------------------------------------------------------------------

void GegnerRollmops::CalcGunWinkel(void)
{
	float xdiv, ydiv;
	float neww;

	ydiv = (pAim->ypos + 40) - (yPos + 60);
	if (ydiv == 0.0f)
		ydiv  = 0.00001f;

	if (xPos < Value1 + 320.0f)
	{
		xdiv = (pAim->xpos + 35) - (xPos + 50);
		neww = (float)atan(ydiv / xdiv) * 180.0f / PI;
	}
	else
	{
		xdiv = (pAim->xpos + 35) - (xPos + 50);
		neww = -(float)atan(ydiv / xdiv) * 180.0f / PI;
	}
				

	if (neww < -30.0f)  neww = -30.0f;
	if (neww >  90.0f)  neww =  90.0f;

	if (GunWinkel < neww)
		GunWinkel += 5.0f SYNC;

	if (GunWinkel > neww)
		GunWinkel -= 5.0f SYNC;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerRollmops::DoDraw(void)
{	
	bool mirrored = false;

	// Je nach Handlung andere Grafik rendern
	//
	switch (Handlung)
	{
		case GEGNER_AUSSPUCKEN:
		{			
			float x, y;

			x = ((xPos + 65 - 9) - HookX) / NUM_KETTENGLIEDER;
			y = ((yPos + 65.0f) - HookY) / NUM_KETTENGLIEDER;

			for (int i = 0; i < NUM_KETTENGLIEDER; i++)
			{				
				// Position setzen
				pKettenTeile[i]->xPos = HookX + i * x;
				pKettenTeile[i]->yPos = HookY + i * y;

				// Winkel setzen
				pKettenTeile[i]->AnimCount = 360.0f - (Schwung * 180.0f / PI);
			}

			Rollen.RenderSprite((float)(xPos-pTileEngine->XOffset),
								(float)(yPos-pTileEngine->YOffset), AnimPhase, 0xFFFFFFFF, mirrored);

		} break;

		case GEGNER_LAUFEN:
		case GEGNER_LAUFEN_LINKS:
		case GEGNER_LAUFEN_RECHTS:
		case GEGNER_LAUFEN_LINKS2:
		case GEGNER_LAUFEN_RECHTS2:
		case GEGNER_SPECIAL:
		case GEGNER_SPECIAL2:
		case GEGNER_SPECIAL3:
		case GEGNER_WARTEN:
		case GEGNER_EXPLODIEREN:
		{
			Rollen.RenderSprite((float)(xPos-pTileEngine->XOffset),
								(float)(yPos-pTileEngine->YOffset), AnimPhase, 0xFFFFFFFF, mirrored);
		} break;

		case GEGNER_INIT:
		case GEGNER_STEHEN:
		{
			pGegnerGrafix[GegnerArt]->RenderSpriteScaled((float)(xPos-pTileEngine->XOffset) + 16.0f,
														 (float)(yPos-pTileEngine->YOffset), 
														 120, 120,
														 AnimPhase, 0xFFFFFFFF);
		} break;

		case GEGNER_OEFFNEN:
		case GEGNER_SCHLIESSEN:
		{
			mirrored = (xPos < Value1 + 320.0f);

			Aufklappen.RenderSprite((float)(xPos-pTileEngine->XOffset),
									(float)(yPos-pTileEngine->YOffset), 								 
									AnimPhase, 0xFFFFFFFF, mirrored);
		} break;

		case GEGNER_VERFOLGEN:
		{	
			mirrored = (xPos < Value1 + 320.0f);

			float xoff = 0.0f;

			if (mirrored) 
				xoff = 65.0f;
				
			// Knarre rendern
			Gun.RenderSpriteRotatedOffset((float)(xPos-pTileEngine->XOffset) + 6.0f + xoff,
										  (float)(yPos-pTileEngine->YOffset) + 38.0f,
										  GunWinkel, 
										  53, 20, 0xFFFFFFFF, mirrored);

			// Mops ohne Knarre rendern			
			Aufklappen.RenderSprite((float)(xPos-pTileEngine->XOffset),
									(float)(yPos-pTileEngine->YOffset),
									8, 0xFFFFFFFF, mirrored);			
		} break;

	}
}

// --------------------------------------------------------------------------------------
// Neue Aktion festlegen
// --------------------------------------------------------------------------------------

void GegnerRollmops::NeueAktion(void)
{
	GunAusfahren(true);
}

// --------------------------------------------------------------------------------------
// Aufklappen zum Schiessen
// --------------------------------------------------------------------------------------

void GegnerRollmops::GunAusfahren(bool Auf)
{
	xSpeed = 0.0f;
	ySpeed = 0.0f;
	xAcc   = 0.0f;
	yAcc   = 0.0f;

	pSoundManager->PlayWave(100, 128, 14000, SOUND_STEAM);

	if (Auf == true)
	{
		Handlung = GEGNER_OEFFNEN;

		AnimPhase = 0;
		AnimStart = 0;
		AnimEnde = 9;		
		HitsToTake = Energy;
	}
	else
	{		
		Handlung = GEGNER_SCHLIESSEN;

		AnimPhase = 7;
		AnimStart = 7;
		AnimEnde = 0;
		Delay    = 0.0f;
	}
	
	AnimCount = 0.0f;
	AnimSpeed = 1.2f;
}

// --------------------------------------------------------------------------------------
// Gurkensalat machen
// --------------------------------------------------------------------------------------

void GegnerRollmops::Losrollen(void)
{
}

// --------------------------------------------------------------------------------------
// Abhopsen am Boden
// --------------------------------------------------------------------------------------

void GegnerRollmops::Abhopsen(float mul)
{
	if (ySpeed > 0.0f &&
		yPos > (float)(Value2 + 480.0f - 100.0f - 40.0f))
	{
		yPos = (float)(Value2 + 480.0f - 100.0f - 40.0f);

		pSoundManager->PlayWave(100, 128, 8000 + rand()%2000, SOUND_LANDEN);

		if (mul != -0.6f)
			pSoundManager->PlayWave(40, 128, 10000 + rand()%1000, SOUND_KLONG);

		ySpeed *= mul;

		for (int i = 0; i < 10; i++)
			pPartikelSystem->PushPartikel(xPos + 10 + rand()%80,
										  yPos + 100, SNOWFLUSH);

		if (ySpeed > -1.0f)
		{
			ySpeed = 0.0f;
			yAcc   = 0.0f;
		}
	}
}

// --------------------------------------------------------------------------------------
// Rundherum schiessen
// single: true  = einzelschuss rundherum
//		   false = alle gleichzeitig
// --------------------------------------------------------------------------------------

void GegnerRollmops::RoundShot(bool single)
{
	static float WinkelCount = 0;

	if (single)
	{
		WinkelUebergabe = WinkelCount;
		pProjectiles->PushProjectile(xPos + 50 + (float)sin(WinkelCount / 180.0f * PI) * 40.0f, 
									 yPos + 50 - (float)cos(WinkelCount / 180.0f * PI) * 40.0f, EISZAPFENSHOT);

		WinkelCount += 30;

		if (WinkelCount >= 360)
			WinkelCount = 0;
	}
	else
	for (WinkelCount = (float)(rand()%30); WinkelCount < 360.0f; WinkelCount += 30)
	{
		WinkelUebergabe = WinkelCount;
		pProjectiles->PushProjectile(xPos + 50 + (float)sin(WinkelCount / 180.0f * PI) * 50.0f, 
									 yPos + 50 - (float)cos(WinkelCount / 180.0f * PI) * 50.0f, EISZAPFENSHOT);
	}

	pSoundManager->PlayWave(50, 128, 16000 + rand()%2000, SOUND_STONEFALL);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerRollmops::DoKI(void)
{
	// Energie anzeigen
	if (Handlung != GEGNER_INIT && 
		Handlung != GEGNER_EXPLODIEREN)
		pHUD->ShowBossHUD(4000, Energy);

	// Levelausschnitt auf den Mops zentrieren, sobald dieser sichtbar wird
	if (pTileEngine->Zustand == ZUSTAND_SCROLLBAR)
	{
		pTileEngine->ScrollLevel((float)Value1, 
								 (float)Value2, ZUSTAND_SCROLLTOLOCK);

		// Mops aus Screen bringen
		xPos = Value1 + 640.0f;
		//yPos = Value2 + 100.0f;		
		pSoundManager->FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
	}

	// Boss blinkt nicht so lange wie die restlichen Gegner
	if (DamageTaken > 0.0f)
		DamageTaken -= 100 SYNC;				// Rotwerden langsam ausfaden lassen
	else
		DamageTaken = 0.0f;						// oder ganz anhalten

	if (Delay > 0.0f)
	{
		Delay -= 1.0f SYNC;
	}
	else
	// Je nach Handlung richtig verhalten
	switch (Handlung)
	{
		// warten bis Kamera am richtigen Fleck ist, dann reinhopsen lassen
		case GEGNER_EINFLIEGEN:
		{
			// Kamera an der richtigen Position?
			if (pTileEngine->Zustand == ZUSTAND_LOCKED)
			{
				// dann reinhopsen
				xSpeed = -10.0f;
				yAcc = 4.0f;
				Handlung = GEGNER_INIT;
				AnimEnde  = 19;
				AnimSpeed = 0.25f;
//				NeueAktion();
				
				// Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
				if (FMUSIC_IsPlaying(pSoundManager->its_Songs[MUSIC_BOSS]->SongData) == false)
					pSoundManager->PlaySong(MUSIC_BOSS, false);
			}
		} break;

		// Schneekugel hopst rein
		case GEGNER_INIT:
		{						
			SimpleAnimation();

			AnimSpeed += 0.02f SYNC;

			Abhopsen();

			// ausrollen
			if (ySpeed == 0.0f)
			{				
				xAcc = 1.0f;
				AnimSpeed += 0.02f SYNC;

				// ausgerollt? Dann kette ausfahren
				if (xSpeed >= 0.0f)
				{
					xAcc = 0.0f;
					Handlung = GEGNER_STEHEN;
					HookX = xPos + 65 - 9;
					HookY = yPos;
					Schwung = 0.0f;
					SchwungDir = 0.0f;
					ShotDelay = 5.0f;	
				}
			}
		} break;

		// Haken nach oben schiessen
		case GEGNER_AUSSPUCKEN:
		{		
			static bool shot = false;

			if (AnimCount > 0.0f)
				AnimCount -= 1.0f SYNC;

			HookY -= 40.0f SYNC;

			if (HookY < Value2 + 20)
			{
				// evtl schiessen
				if (yPos < Value2 + 130 &&
					shot == false)
				{
					RoundShot(false);
					shot = true;
				}

				HookY = (float)Value2 + 20;

				//xPos = pPlayer->xpos;
				xPos = (float)(Value1 + 320.0f - 65.0f + sin(Schwung) * 300.0f);
				yPos = (float)(Value2 - 500.0f + cos(Schwung * 0.75f) * 830.0f);

				SchwungDir += 0.16f SYNC;
				if (SchwungDir > 2*PI)
					SchwungDir = 0.0f;

				Schwung = (float)sin(SchwungDir);

				AnimPhase = - (int)(((Value1 + 640.0f) - xPos) / 200.0f);

				if (AnimPhase < 0)
					AnimPhase += 10;

				if (((SchwungDir > PI - 0.2f &&
					  SchwungDir < PI + 0.2f) ||

				 	 (SchwungDir > 2*PI - 0.2f   &&
				 	  SchwungDir < 2*PI + 0.2f)) && 
					rand()%2 == 0)
				{
					shot = false;
					pPartikelSystem->PushPartikel(xPos + 60 + rand()%20,
												  yPos + 120.0f,
												  LONGFUNKE);

					pPartikelSystem->PushPartikel(xPos + 60 + rand()%20,
												  yPos + 100.0f,
												  FUNKE);

					if (pSoundManager->its_Sounds[SOUND_KLONG]->isPlaying == false)
						pSoundManager->PlayWave(50, 128, 14000 + rand()%2000, SOUND_KLONG);

				}
			}

			// abgeschossen?
			if (Energy < 2000.0f)
			{
				Handlung  = GEGNER_WARTEN;
				ySpeed    = -20.0f;
				yAcc      = 6.0f;
				AnimSpeed = 10.0f;

				// Kette fliegt weg
				for (int i = 0; i < NUM_KETTENGLIEDER; i++)
					pKettenTeile[i]->Handlung = GEGNER_SPECIAL;
			}
		} break;
			

		case GEGNER_LAUFEN_LINKS:
		case GEGNER_LAUFEN_LINKS2:
		{
			// Speed begrenzen
			if (xSpeed < -60.0f)
				xSpeed = -60.0f;

			// Grenze überschritten, ab der abgebremst wird?
			if (xPos + 65 < Value1 + 135.0)
				xAcc = 19.0f;

			// in der Mitte stoppen und abspringen
			if (Handlung == GEGNER_LAUFEN_LINKS2 &&
				ySpeed == 0.0f)
			{
				if (xPos + 65 < Value1 + 380.0)
					xAcc = 23.0f;

				if (xSpeed > 0.0f)
				{
					xSpeed = 0.0f;
					ySpeed = 0.0f;
					xAcc   = 0.0f;
					yAcc   = 10.0f;
					Handlung = GEGNER_SPECIAL2;
					ShotCount = 2.0f;
					ySpeed = -77.0f;
					break;
				}
			}

			// Animphase anpassen
			AnimCount -= xSpeed * 0.1f SYNC;

			if (AnimCount > AnimSpeed)
			{
				AnimCount = 0.0f;
				AnimPhase++;
			}

			if (AnimPhase > AnimEnde)
				AnimPhase = 0;

			// Richtung umdrehen?
			if (xSpeed > 0.0f)
			{
				Handlung = GEGNER_LAUFEN_RECHTS;
				ShotDelay -= 1.0f;

				// abspringen?
				if ((int)(ShotDelay) % 3 == 0)
				{
					xSpeed = 12.0f;
					ySpeed = -60.0f;
					yAcc = 15.0f;
				}		
				
				if (ShotDelay <= 0.0f)
				{
					GunAusfahren(true);
				}
			}

			Abhopsen(-0.4f);

		} break;

		case GEGNER_LAUFEN_RECHTS:
		case GEGNER_LAUFEN_RECHTS2:
		{
			// Speed begrenzen
			if (xSpeed > 60.0f)
				xSpeed = 60.0f;

			// Grenze überschritten, ab der abgebremst wird?
			if (xPos + 65 > Value1 + 505.0)
				xAcc = -19.0f;

			// in der Mitte stoppen und abspringen
			if (Handlung == GEGNER_LAUFEN_RECHTS2 &&
				ySpeed == 0.0f)
			{
				if (xPos + 65 > Value1 + 300.0)
					xAcc = -23.0f;

				if (xSpeed < 0.0f)
				{
					xSpeed = 0.0f;
					ySpeed = 0.0f;
					xAcc   = 0.0f;
					yAcc   = 8.0f;
					Handlung = GEGNER_SPECIAL3;
					ShotCount = 2.0f;
					ySpeed = -65.0f;
					break;
				}
			}

			// Animphase anpassen
			AnimCount += xSpeed * 0.1f SYNC;

			if (AnimCount > AnimSpeed)
			{
				AnimCount = 0.0f;
				AnimPhase--;
			}

			if (AnimPhase < 0)
				AnimPhase = AnimEnde;

			// Richtung umdrehen?				
			if (xSpeed < 0.0f)
			{
				Handlung = GEGNER_LAUFEN_LINKS;				
				ShotDelay -= 1.0f;

				// abspringen?
				if ((int)(ShotDelay) % 3 == 0)
				{
					xSpeed = -16.0f;
					ySpeed = -60.0f;
					yAcc = 12.0f;
				}

				if (ShotDelay <= 0.0f)
				{
					GunAusfahren(true);
				}
			}

			Abhopsen(-0.4f);

		} break;

		case GEGNER_STEHEN:
		{
			ShotDelay -= 1.0f SYNC;

			if (ShotDelay <= 0.0f)
			{
				ShotDelay = 0.0f;
				Handlung = GEGNER_AUSSPUCKEN;
				AnimPhase = 0;
				AnimStart = 0;
				AnimEnde = 10;
				AnimSpeed = 0.5f;
//				xSpeed = -5.0f;

				pSoundManager->PlayWave(100, 128, 7000, SOUND_LANDEN);
				pSoundManager->PlayWave(100, 128, 9000, SOUND_LANDEN);

				pSoundManager->PlayWave(80, 128, 11025, SOUND_STEAM);

				int i;
				for (i = 0; i < 30; i++)
					pPartikelSystem->PushPartikel(xPos + 10 + rand()%80, yPos + 10 + rand()%80, SNOWFLUSH);

				for (i = 0; i < 8; i++)
					pProjectiles->PushProjectile(xPos + 10 + rand()%80, yPos + 10 + rand()%80, SNOWBOMBSMALL);
			}
		} break;

		case GEGNER_LAUFEN :
		{
			Abhopsen(-0.4f);
			SimpleAnimation();			
		} break;

		case GEGNER_WARTEN:
		{
			Abhopsen(-0.4f);

			if (ySpeed == 0.0f)
				AnimSpeed -= 1.0f SYNC;

			if (AnimSpeed <= 0.0f)
			{
				ShotDelay = 5.0f;

				Handlung = GEGNER_LAUFEN_LINKS;

				xSpeed = -60.0f;
				xAcc = 0.0f;
				AnimPhase = 0;
				AnimEnde  = 9;
				AnimStart = 0;
				AnimSpeed = 1.0f;
			}
		} break;

		// 
		case GEGNER_OEFFNEN :
		case GEGNER_SCHLIESSEN :
		{
			bool fertig = false;

			// auf/zuklappen
			AnimCount += 1.0f SYNC;

			if (AnimCount > AnimSpeed)
			{
				AnimCount = 0.0f;

				if (Handlung == GEGNER_OEFFNEN)
				{
					AnimPhase++;

					if (AnimPhase >= AnimEnde - 1)
					{
						fertig = true;
						Delay = 10.0f;
						GunWinkel = 0.0f;
					}
				}
				else
				{
					AnimPhase--;

					if (AnimPhase <= AnimEnde)
						fertig = true;
				}
			}

			if (fertig)
			{				
				AnimPhase = AnimEnde;

				// schiessen?
				if (Handlung == GEGNER_OEFFNEN) 
				{
					ShotCount = (float)(rand()%15) + 15;
					Handlung = GEGNER_VERFOLGEN;
					Destroyable = true;
				}
				// wieder rollen
				else
				{
					Destroyable = false;

					ShotDelay = 5.0f;
					xAcc = 0.0f;
					AnimPhase = 0;
					AnimEnde  = 9;
					AnimStart = 0;
					AnimSpeed = 1.0f;

					if (xPos > Value1 + 320.0f)
					{
						if (Energy < 1000.0f)
							Handlung = GEGNER_LAUFEN_LINKS2;
						else
							Handlung = GEGNER_LAUFEN_LINKS;

						xSpeed = -40.0f;
					}
					else
					{
						if (Energy < 1000.0f)
							Handlung = GEGNER_LAUFEN_RECHTS2;
						else
							Handlung = GEGNER_LAUFEN_RECHTS;

						xSpeed = 40.0f;
					}
				}

			}
		} break;

		// Mit der Kanone zielen
		case GEGNER_VERFOLGEN :
		{
			CalcGunWinkel();			

			// schiessen
			ShotDelay -= 1.0f SYNC;
			if (ShotDelay < 0.0f)
			{
				pSoundManager->PlayWave(50, 128, 16000 + rand()%2000, SOUND_STONEFALL);

				float xoff = 0.0f;

				if (xPos > Value1 + 320.0f)
					WinkelUebergabe = 270.0f - GunWinkel;
				else
				{
					WinkelUebergabe = GunWinkel + 90.0f;
					xoff = -70.0f;
				}

				pPartikelSystem->PushPartikel(xPos + xoff + 85 + (float)sin(WinkelUebergabe / 180.0f * PI) * 105.0f, 
											 yPos + 46 - (float)cos(WinkelUebergabe / 180.0f * PI) * 90.0f, SNOWFLUSH);

				pProjectiles->PushProjectile(xPos + xoff + 90 + (float)sin(WinkelUebergabe / 180.0f * PI) * 105.0f, 
											 yPos + 46 - (float)cos(WinkelUebergabe / 180.0f * PI) * 90.0f, EISZAPFENSHOT);

				ShotDelay = 3.0f;
				ShotCount -= 1.0f;

				if (ShotCount <= 0.0f ||
					(HitsToTake - Energy) > 100.0f)
					GunAusfahren(false);
			}
		} break;

		case GEGNER_SPECIAL2:
		{	
			Abhopsen(-0.3f);

			if (ySpeed == 0.0f)
			{
				ShotDelay = 3.0f;
				Handlung = GEGNER_LAUFEN_LINKS;
				xSpeed = -30.0f;
				xAcc   = -10.0f;
			}

			if (yPos < Value2 + 260.0f)
			{
				ShotCount -= 1.0f SYNC;

				if (ShotCount <= 0.0f)
				{
					RoundShot(false);
					ShotCount = 4.0f;
				}
			}			
		} break;

		// hochspringen, kreiseln und in alle Richtungen schiessen
		case GEGNER_SPECIAL3:
		{	
			Abhopsen(-0.3f);

			if (ySpeed == 0.0f)
			{
				ShotDelay = 3.0f;
				Handlung = GEGNER_LAUFEN_RECHTS;
				xSpeed = 30.0f;
				xAcc   = 10.0f;
			}

			if (yPos < Value2 + 250.0f)
			{
				ShotCount -= 1.0f SYNC;

				if (ShotCount <= 0.0f)
				{
					RoundShot(true);
					ShotCount = 0.5f;
				}
			}			
		} break;

		case GEGNER_EXPLODIEREN:
		{
			Abhopsen(-0.3f);

			// animieren
			AnimCount += 1.0f SYNC;

			while (AnimCount > AnimSpeed)
			{
				pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION3);

				pPartikelSystem->PushPartikel(xPos - 20 + rand()%140, yPos - 20 + rand()%140, EXPLOSION_MEDIUM2 + rand()%1);

				if (rand()%3 == 0)
					pPartikelSystem->PushPartikel(xPos + rand()%100, yPos + rand()%100, SPLITTER);

				AnimCount -= AnimSpeed;
				AnimSpeed -= 0.01f;
		
				AnimPhase++;

				if (AnimPhase > AnimEnde)
					AnimPhase = 0;

				if (AnimSpeed <= 0.1f)
					Energy = -500.0f;
			}
		} break;

		default : break;
	} // switch

	// Schnee beim Rollen?
	if (Handlung == GEGNER_LAUFEN_RECHTS ||
		Handlung == GEGNER_LAUFEN_LINKS ||
		Handlung == GEGNER_LAUFEN ||
		Handlung == GEGNER_SPECIAL)
	{
		if (ySpeed == 0.0f)
			SmokeCount -= 1.0f SYNC;

		if (SmokeCount < 0.0f)
		{
			pPartikelSystem->PushPartikel(xPos + 10 + rand()%80, yPos + 80 + rand()%10, SNOWFLUSH);
			SmokeCount = 0.8f;
		}
	}

	// Testen, ob der Spieler den Boss berührt hat
	GegnerRect[GegnerArt].top    = 20;
	GegnerRect[GegnerArt].left   = 20;
	GegnerRect[GegnerArt].right  = 80;
	GegnerRect[GegnerArt].bottom = 80;
	TestDamagePlayers((float) 20.0 SYNC);

	// Keine Energie mehr? Dann explodieren
	if (Energy <= 0.0f &&
		Energy > -500.0f &&
		Handlung != GEGNER_EXPLODIEREN)
	{
		Handlung = GEGNER_EXPLODIEREN;
		xSpeed = 0.0f;
		ySpeed = 0.0f;
		xAcc   = 0.0f;

		Energy = 1.0f;
		Destroyable = false;
		AnimSpeed = 1.0f;
		ShotDelay = 1.5f;

		AnimPhase = 0;
		AnimEnde  = 9;
		AnimStart = 0;

		// Endboss-Musik ausfaden und abschalten
		pSoundManager->FadeSong(MUSIC_BOSS, -2.0f, 0, false);
	}
}

// --------------------------------------------------------------------------------------
// Rollmops explodiert
// --------------------------------------------------------------------------------------

void GegnerRollmops::GegnerExplode(void)
{	
	pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION2);

	int i;
	for (i = 0; i < 5; i++)
		pPartikelSystem->PushPartikel(xPos - 20 + rand()%140, yPos - 20 + rand()%140, EXPLOSION_TRACE);

	for (i = 0; i < 150; i++)
		pPartikelSystem->PushPartikel(xPos - 20 + rand()%140, yPos - 20 + rand()%140, WATERFLUSH_HIGH);

	ShakeScreen(5.0f);

	pPlayer[0]->Score += 5000;

	ScrolltoPlayeAfterBoss();
}
