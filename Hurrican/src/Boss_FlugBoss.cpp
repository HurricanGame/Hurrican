// --------------------------------------------------------------------------------------
// Der Flugboss
//
// Muss noch erfunden werden ;)
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_FlugBoss.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFlugBoss::GegnerFlugBoss(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_NOTVISIBLE;
	Energy			= 6000;
	Value1			= Wert1;
	Value2			= Wert2;
	AnimPhase		= 0;
	AnimEnde		= 0;
	AnimSpeed		= 0.0f;
	ChangeLight		= Light;
	Destroyable		= true;
	TestBlock		= false;
	OwnDraw			= true;
	SchienePos		= 320.0f;
	tempSpeed		= 0.0f;

	Winkel[0] =
	Winkel[1] = 0.0f;

	alphaAuge   = 0.0f;
	alphaSpeed  = 10.0f;
	alphaDir    = 1.0f;

	ShotDelay   = 0.0f;
	ShotCount   = 0.0f;

	yKanone[0] =
	yKanone[1] =
	yKanone[2] =
	yKanone[3] = 0.0f;

	xKanone = 0.0f;
	xHit    = rand ()%440 + 100.0f;
	yHit    = 100.0f;
	dHit    = -1.0f;

	// Zusätzliche Grafiken laden
	//
	SchieneL.LoadImage ("flugboss_schiene_l.bmp", 320, 73, 320, 73, 1, 1);
	SchieneR.LoadImage ("flugboss_schiene_r.bmp", 320, 73, 320, 73, 1, 1);

	Kanone_big.LoadImage    ("flugboss_kanone_fett.bmp",  21, 85, 21, 85, 1, 1);
	Kanone_medium.LoadImage ("flugboss_kanone_klein.bmp", 11, 48, 11, 48, 1, 1);
	Kanone_small.LoadImage  ("flugboss_kanone_mini.bmp",   8, 63,  8, 63, 1, 1);
	Geschuetz.LoadImage     ("flugboss_geschuetz.png",  62, 97, 62, 97, 1, 1);

	Auge.LoadImage		    ("flugboss_auge.bmp",  29, 30, 29, 30, 1, 1);

	Hitzone[0].LoadImage    ("flugboss_hitzone.png",  52, 100, 52, 100, 1, 1);
	Hitzone[1].LoadImage    ("flugboss_hitzone2.bmp", 52,  31, 52,  31, 1, 1);
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerFlugBoss::DoDraw(void)
{
		// Schienen
	SchieneL.RenderSprite(-SchienePos, 
						   (float)(yPos-pTileEngine->YOffset) + 161,
							0xFFFFFFFF);

	SchieneR.RenderSprite(320 + SchienePos, 
						   (float)(yPos-pTileEngine->YOffset) + 161,
							0xFFFFFFFF);

	// Fette Kanone
	Kanone_big.RenderSprite((float) (- pTileEngine->XOffset + xPos + 121),
						    (float) (- pTileEngine->YOffset + yPos + 249 - yKanone[0]),
							0xFFFFFFFF);

	// Medium Kanone
	Kanone_medium.RenderSprite((float) (- pTileEngine->XOffset + xPos + 148),
					  	       (float) (- pTileEngine->YOffset + yPos + 254 - yKanone[1]),
							   0xFFFFFFFF);

	// Mini Kanone 1
	Kanone_small.RenderSprite((float) (- pTileEngine->XOffset + xPos + 68),
					  	      (float) (- pTileEngine->YOffset + yPos + 261 - yKanone[2]),
							  0xFFFFFFFF);

	// Mini Kanone 2
	Kanone_small.RenderSprite((float) (- pTileEngine->XOffset + xPos + 78),
					  	      (float) (- pTileEngine->YOffset + yPos + 251 - yKanone[3]),
							  0xFFFFFFFF);

	// Schatten
	RenderRect ((float)(xPos-pTileEngine->XOffset + 255),
				(float)(yPos-pTileEngine->YOffset + 161),
				18, 80, 0x88000000);

	// Main
	pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset),
										   (float)(yPos-pTileEngine->YOffset),
										   AnimPhase, 0xFFFFFFFF);	

	int a = int (alphaAuge);

	// Hit Zone
	Hitzone[0].RenderSprite(xHit, 380.0f + yHit, 0, 0xFFFFFFFF);
	Hitzone[1].RenderSprite(xHit, 401.0f + yHit, 0, 0xFFFFFFFF);

	GegnerRect[GegnerArt].left   = int (xHit + pTileEngine->XOffset - xPos);
	GegnerRect[GegnerArt].right  = int (GegnerRect[GegnerArt].left + 52);
	GegnerRect[GegnerArt].top    = int (380 + pTileEngine->YOffset - yPos + yHit);
	GegnerRect[GegnerArt].bottom = int (GegnerRect[GegnerArt].top + 100);

	// Geschuetz
	Geschuetz.RenderSpriteRotated(xKanone - 62,
					  			 (float) (- pTileEngine->YOffset + yPos + 160), Winkel[0], 0,
 								  0xFFFFFFFF);

	// Geschuetz
 	Geschuetz.RenderSpriteRotated(640 - xKanone,
					  			 (float) (- pTileEngine->YOffset + yPos + 160), Winkel[1], 0,
								 0xFFFFFFFF);

	// Auge
	alphaAuge += alphaSpeed * alphaDir SYNC;

	// Grenzen für Alpha checken
	if (alphaAuge > 255.0f)
	{
		alphaAuge = 255.0f;
		alphaDir  = -1.0f;
	}

	if (alphaAuge < 0.0f)
	{
		alphaAuge = 0.0f;
		alphaDir  = 1.0f;
	}
	
	Auge.RenderSprite((float) (- pTileEngine->XOffset + xPos + 175),
					  (float) (- pTileEngine->YOffset + yPos + 182), 0,
					  0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFlugBoss::DoKI(void)
{
/*
	BlickRichtung	= LINKS;

	// Energie anzeigen
	if (Handlung != GEGNER_INIT && Handlung != GEGNER_EXPLODIEREN)
		pHUD->ShowBossHUD(6000, Energy);

	// Animieren
	if (AnimEnde > 0)						// Soll überhaupt anmiert werden ?
	{
		AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
		if (AnimCount > AnimSpeed)			// Grenze überschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen
			AnimPhase++;					// Und nächste Animationsphase
			if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
				AnimPhase = AnimStart;		// Dann wieder von vorne beginnen
		}
	} // animieren

	// Levelausschnitt auf den Boss zentrieren, sobald dieser sichtbar wird
	if (Active == true && pTileEngine->Zustand == ZUSTAND_SCROLLBAR)
	{
		pTileEngine->ScrollLevel((float)Value1, 
								 (float)Value2, ZUSTAND_SCROLLTOLOCK);		// Level auf den Boss zentrieren

		pSoundManager->FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren

		yPos -= 480;
	}

	// Zwischenboss blinkt nicht so lange wie die restlichen Gegner
	if (DamageTaken > 0.0f)
		DamageTaken -= 100 SYNC;				// Rotwerden langsam ausfaden lassen
	else
		DamageTaken = 0.0f;						// oder ganz anhalten

	// Hat der Boss keine Energie mehr ? Dann explodiert er
	//
	if (Energy <= 100.0f && Handlung != GEGNER_EXPLODIEREN)
	{
		Handlung  = GEGNER_EXPLODIEREN;

		// Endboss-Musik ausfaden und abschalten
		pSoundManager->FadeSong(MUSIC_BOSS, -2.0f, 0, false);
	}

	// Je nach Handlung richtig verhalten
	//
	switch (Handlung)
	{
		case GEGNER_INIT:			// Warten bis der Screen zentriert wurde
		{
			if (pTileEngine->Zustand == ZUSTAND_LOCKED)
			{
				tempSpeed = 35.0f;
				Handlung  = GEGNER_INIT2;

				pSoundManager->PlayWave (100, 128, 3500, SOUND_ROCKET);
				pSoundManager->PlayWave (100, 128, 4500, SOUND_ROCKET);
			}
		} break;


		// von oben einfliegen
		//
		case GEGNER_INIT2 :
		{			
			tempSpeed -= 1.35f SYNC;

			if (tempSpeed < 3.0f)
				tempSpeed = 3.0f;

			yPos += tempSpeed SYNC;

			// Endposition erreicht? Dann Schienen einfahren
			//
			if (yPos >= float (Value2 - 60))
			{
				yPos	  = float (Value2 - 60);				
				tempSpeed = 15.0f;
				Handlung  = GEGNER_INIT3;
				
				pSoundManager->StopWave (SOUND_ROCKET);
				pSoundManager->PlayWave (100, 128, 11025, SOUND_DOORSTOP);
				pSoundManager->PlayWave (100, 128, 11025, SOUND_DOOR);
			}

		} break;

		// Schienen einfahren
		//
		case GEGNER_INIT3:
		{
			tempSpeed -= 0.2f SYNC;

			if (tempSpeed < 2.0f)
				tempSpeed = 2.0f;

			SchienePos -= tempSpeed SYNC;

			if (SchienePos <= 0.0f)
			{
				xKanone    = 0.0f;
				AnimCount  = 10.0f;
				Handlung   = GEGNER_LAUFEN;
				SchienePos = 0.0f;
				pSoundManager->StopWave (SOUND_DOOR);
				pSoundManager->PlayWave (100, 128, 13000, SOUND_DOORSTOP);
				pSoundManager->PlayWave (100, 128, 11000, SOUND_ROCKET);
				tempSpeed = 12.0f;

				// Boss-Musik abspielen, sofern diese noch nicht gespielt wird
				//
				if (FMUSIC_IsPlaying(pSoundManager->its_Songs[MUSIC_BOSS]->SongData) == false)
					pSoundManager->PlaySong(MUSIC_BOSS, false);
			}

		} break;

		// Geschütze reinfahren
		//
		case GEGNER_EINFAHREN:
		{
			tempSpeed -= 0.5f SYNC;

			if (tempSpeed < 2.0f)
				tempSpeed = 2.0f;

			xKanone += tempSpeed SYNC;

			if (xKanone >= 110.0f)
			{
				AnimCount  = 10.0f;
				Handlung   = GEGNER_SPECIAL;
				ShotDelay  = 1.0f;
				ShotCount  = 20.0f;
				xKanone	   = 110.0f;
				pSoundManager->PlayWave (100, 128, 15000, SOUND_DOORSTOP);
			}

		} break;

		// Geschütze rausfahren
		//
		case GEGNER_AUSFAHREN:
		{
			tempSpeed += 0.5f SYNC;

			if (tempSpeed > 10.0f)
				tempSpeed = 10.0f;

			xKanone -= tempSpeed SYNC;

			if (xKanone <= 0.0f)
			{
				AnimCount  = 10.0f;
				Handlung   = GEGNER_EINFAHREN;
				xKanone	   = 0.0f;
				tempSpeed  = 12.0f;
				pSoundManager->StopWave (SOUND_DOOR);
				pSoundManager->PlayWave (100, 128, 15000, SOUND_DOORSTOP);
				pSoundManager->PlayWave (100, 128, 11000, SOUND_ROCKET);
			}

		} break;

		// Mit Geschützen ballern
		//
		case GEGNER_SPECIAL:
		{			
			// Kanonen ausrichten
			//
			float dx, dy;
			float w, winkel;

			// Kanone Links

			// Abstände berechnen
			dx = (xPos - 120) - (pPlayer->xpos + 35);
			dy = (yPos + 194) - (pPlayer->ypos + 40);

			// Division durch Null verhinden
			if (dy == 0.0f)
				dy = 0.01f;			

			w = float(atan(dx / dy) * 360.0f / (D3DX_PI * 2));

			if (dx >= 0 && dy >= 0) winkel = w;		  else
			if (dx > 0  && dy < 0 ) winkel = 180 + w;  else
			if (dx < 0  && dy > 0 ) winkel = 360 + w;  else
			if (dx < 0  && dy < 0 ) winkel = 180 + w;

			winkel = 180.0f - winkel;

			if (Winkel[0] < winkel)	
				Winkel[0] += 5.0f SYNC;
			if (Winkel[0] > winkel)	Winkel[0] -= 5.0f SYNC;

			// Kanone rechts

			// Abstände berechnen
			dx = (xPos + 370) - (pPlayer->xpos + 35);
			dy = (yPos + 194) - (pPlayer->ypos + 40);

			// Division durch Null verhinden
			if (dy == 0.0f)
				dy = 0.01f;			

			w = float(atan(dx / dy) * 360.0f / (D3DX_PI * 2));

			if (dx >= 0 && dy >= 0) winkel = w;		  else
			if (dx > 0  && dy < 0 ) winkel = 180 + w;  else
			if (dx < 0  && dy > 0 ) winkel = 360 + w;  else
			if (dx < 0  && dy < 0 ) winkel = 180 + w;

			winkel = 180.0f - winkel;

			if (Winkel[1] < winkel)	
				Winkel[1] += 5.0f SYNC;
			if (Winkel[1] > winkel)	Winkel[1] -= 5.0f SYNC;

			// Schiessen lassen
			//
			ShotDelay -= 1.0f SYNC;

			while (ShotDelay <= 0.0f)
			{
				ShotDelay += 0.2f;
				ShotCount -= 1.0f;

				pSoundManager->PlayWave (100, 128, 8000 + rand()%1000, SOUND_CANON);

				if (int (ShotCount) % 2 == 0)
				{
					float xo, yo;

					xo = xPos - 116;
					yo = yPos + 210;

					xo += float (60 * sin (PI * winkel / 180.0f));
					yo += float (60 * cos (PI * winkel / 180.0f));

					pProjectiles->PushProjectile (xo, yo, SUCHSCHUSS);
				}
				else
					pProjectiles->PushProjectile (xPos + 370, yPos + 210, SUCHSCHUSS);
			}


		} break;

		case GEGNER_EXPLODIEREN:
		{
			//Energy = 100.0f;
			AnimCount -= 1.0f SYNC;

			if (AnimCount < 0.0f)
			{
				pPartikelSystem->PushPartikel(xPos + rand()%500, yPos + rand()%300, EXPLOSION_MEDIUM2);
				pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);

				AnimCount = 1.0f;
			}
		}

		// Wait Counter runterzählen und neue Aktion bestimmen
		//
		case GEGNER_LAUFEN:
		{
			AnimCount -= 1.0f SYNC;

			if (AnimCount < 0.0f)
			{
				AnimCount = 0.0f;

				// neue Aktion festlegen
				//
				int j = rand ()% 3;

				j = 1;

				switch (j)
				{
					// aus den zwei kleinen Geschützen ballern?
					//
					case 0 : 
					{
						Handlung = GEGNER_SCHIESSEN;

						ShotDelay = 1.0f;
						ShotCount = 50;
						xSpeed    = -10.0f;
					} break;

					// Geschütze einfahren und ballern
					//
					case 1 : 
					{
						xKanone    = 0.0f;
						AnimCount  = 10.0f;
						Handlung   = GEGNER_EINFAHREN;
						SchienePos = 0.0f;
						pSoundManager->PlayWave (100, 128, 11000, SOUND_ROCKET);
						tempSpeed = 12.0f;
					} break;

					case 2 : 
					{
					} break;

				} // switch

			} break;

			// Boss schiesst mit den zwei kleinen Kanonen
			//
			case GEGNER_SCHIESSEN:
			{
				ShotDelay -= 1.0f SYNC;

				while (ShotDelay <= 0.0f)
				{
					ShotDelay += 3.0f;

					pSoundManager->PlayWave (100, 128, 15000, SOUND_LASERSHOT);
					ShotCount -= 1.0f;

					if (int (ShotCount) % 2 == 0)
					{
						yKanone[2] = 20;
						
						pProjectiles->PushProjectile (xPos + 68,
													  yPos + 261 + 30, FLUGLASER);
					}
					else
					{
						yKanone[3] = 20;

						pProjectiles->PushProjectile (xPos + 78,
													  yPos + 251 + 30, FLUGLASER);
					}

					if (ShotCount <= 0.0f)
					{
						Handlung = GEGNER_LAUFEN;
					}
				}

				if ((xSpeed < 0.0f && xPos - pTileEngine->XOffset < 100) ||
					(xSpeed > 0.0f && xPos - pTileEngine->XOffset > 640 - 280 - 100))
					xSpeed *= -1.0f;

			} break;
		}

		default : break;
	} // switch

	// Kanonen wieder zurückziehen, wenn sie geschossen haben
	//
	for (int i = 0; i < 4; i++)
	{
		if (yKanone[i] > 0.0f)
			yKanone[i] -= 10.0f SYNC;
		else
			yKanone[i] = 0.0f;
	}*/

	// Kanone reindrehen
	//
	if (xKanone < 150.0f)
	{		
		/*Winkel[0] = xKanone + 180.0f * 1.4f;
		

		if (Winkel[0] > 360.0f)
			Winkel[0] = 0.0f;

		Winkel[1] = 360.0f - Winkel[0];*/
	}

	// Hitzone erscheinen lassen
	//
	yHit += dHit SYNC;

	if (yHit < 0.0f)
	{
		yHit = 0.0f;
		dHit = 0.75f;
	}

	if (yHit > 100.0f)
	{
		yHit = 100.0f;
		dHit = -3.0f;
		xHit = rand ()%440 + 100.0f;
	}	
}

// --------------------------------------------------------------------------------------
// FlugBoss explodiert
// --------------------------------------------------------------------------------------

void GegnerFlugBoss::GegnerExplode(void)
{
	// Splitter
	for (int i=0; i<20; i++)
	pPartikelSystem->PushPartikel(xPos + rand()%500 + 30, 
								  yPos + rand()%2000 + 30, SPLITTER);

	pPlayer[0]->Score += 6000;

/*
	// Level wieder zum Spieler scrollen und dann weiterscrollen lassen
	// dabei aufpassen, dass der Spieler nicht zu weit links fliegt und der Scrollbereich im
	// negativen liegt, da sonst das Spiel hängenbleibt =)
	//
	if (pPlayer->xpos - 300 <= 0)
		pTileEngine->ScrollLevel(0, 
								 pPlayer->ypos - 280, ZUSTAND_SCROLLTOPLAYER);
	else

		pTileEngine->ScrollLevel(pPlayer->xpos - 300, 
								 pPlayer->ypos - 280, ZUSTAND_SCROLLTOPLAYER);
								 */

	// Level Musik wieder einfaden lassen (aus Pause Zustand)
	pSoundManager->FadeSong(MUSIC_STAGEMUSIC, 2.0f, 100, true);
}
