// --------------------------------------------------------------------------------------
// Die Spinnenmaschine
//
// Öffnet den Topfdeckel und lässt Gegner raus
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_Spinnenmaschine.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSpinnenmaschine::GegnerSpinnenmaschine(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_INIT;
	BlickRichtung	= LINKS;
	Energy			= 4000;
	ChangeLight		= Light;
	Destroyable		= true;
	Value1			= Wert1;
	Value2			= Wert2;
	TestBlock		= false;
	OwnDraw			= true;
	ShotDelay		= 5.0f;

	GegnerRect[SPINNENMASCHINE].left = 0;	GegnerRect[SPINNENMASCHINE].right  = 400;
	GegnerRect[SPINNENMASCHINE].top  = 0;	GegnerRect[SPINNENMASCHINE].bottom = 300;

	// Zusätzliche Grafiken laden
	//	
	Display.LoadImage("spinnenmaschine_states.bmp", 280, 84, 70, 84, 4, 1);

	Deckel.LoadImage("spinnenmaschine_topf.png", 372, 264, 186, 44, 2, 6);	
	Unten[0].LoadImage ("spinnenmaschine_unten.png", 228, 129, 228, 129, 1, 1);
	Unten[1].LoadImage ("spinnenmaschine_unten2.png", 228, 129, 228, 129, 1, 1);

	Strahl.LoadImage("blitztexture.bmp", 64, 64, 64, 64, 1, 1);	

	DeckelPhase = 0;
	DeckelOffset = 0.0f;
	DeckelCount = 0.0f;
	DeckelStatus = ZU;	
	OpenCounter = TIME_TILL_OPEN;

	HochCounter = TIME_TILL_HOCH;
	HochStatus = ZU;

	SpawnDelay = 8.0f;

	SmokeDelay  = 0.0f;
	SmokeDelay2 = 0.0f;

	LightRayCount = 0.0f;

	AktionFertig = false;

	DisplayState    = 0;
	OldDisplayState = 0;
	AnimUnten = 0;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerSpinnenmaschine::DoDraw(void)
{
	D3DCOLOR Color;

	int	Wert = 255-(int(DamageTaken));

	if (DirectGraphics.BlendMode == ADDITIV_MODE)
		Color = D3DCOLOR_RGBA(255, 255, 255, Wert);
	else
		Color = 0xFFFFFFFF;

	if (Handlung != GEGNER_SPECIAL)
	{
		// evtl strahl rendern, wenn der deckel aufgeht
		if (DeckelStatus != ZU)
		{
			DirectGraphics.SetAdditiveMode();
			Strahl.RenderSpriteScaled((float)(xPos - pTileEngine->XOffset) - LightRayCount * 8.0f + 170,
									  (float)(yPos - pTileEngine->YOffset),
									  (int)(LightRayCount * 16.0f),
									  190, 0xFFFF8822);
			DirectGraphics.SetColorKeyMode();
		}	

		// Oberteil
		DeckelOffset = -((float)(cos(DeckelCount) * 20.0f) + (float)(cos(PI) * 20));
		pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos - pTileEngine->XOffset),
											   (float)(yPos - pTileEngine->YOffset) - (int)DeckelOffset,
												0, Color, true);	

		// Anzeige
		Display.RenderSprite((float)(xPos - pTileEngine->XOffset) + 133,
							 (float)(yPos - pTileEngine->YOffset) + 263 - (int)DeckelOffset,
							 DisplayState, Color, true);

		// Topfdeckel
		Deckel.RenderSprite((float)(xPos - pTileEngine->XOffset) + 75,
							(float)(yPos - pTileEngine->YOffset) - (int)DeckelOffset + 159,
							DeckelPhase, Color, true);
	}

	// Unterteil
	Unten[AnimUnten].RenderSprite((float)(xPos - pTileEngine->XOffset) + 45,
					   (float)(yPos - pTileEngine->YOffset) + 352,
					   0, 0xFFFFFFFF, true);}

// --------------------------------------------------------------------------------------
// Deckel hoch und runterklappen
// --------------------------------------------------------------------------------------

void GegnerSpinnenmaschine::DoDeckel(void)
{
	switch (DeckelStatus) 
	{
		// deckel ist zu und Counter zählt, wann er auf geht
		case ZU :
		{
			OpenCounter -= 1.0f SYNC;

			if (OpenCounter < 0.0f)
			{
				OpenCounter  = 0.0f;
				DeckelStatus = OEFFNEN;
				AnimCount = 0.0f;
			}
		}
		break;

		// Deckel öffnet sich gerade
		case OEFFNEN :
		{
			LightRayCount += 1.0f SYNC;

			AnimCount += 1.0f SYNC;

			if (AnimCount > 0.8f)
			{
				AnimCount = 0.0f;
				DeckelPhase += 1;
			}

			if (DeckelPhase > 10)
			{
				DeckelPhase = 10;
				DeckelStatus = OFFEN;
				OpenCounter = TIME_TILL_CLOSE;
			}			
		}
		break;

		// deckel ist offen und Counter zählt, wann er zugeht
		case OFFEN :
		{
			OpenCounter -= 1.0f SYNC;

			if (OpenCounter < 0.0f)
			{
				OpenCounter  = 0.0f;
				DeckelStatus = SCHLIESSEN;
				AnimCount = 0.0f;
			}

			// Gegner spawnen
			SpawnDelay -= 1.0f SYNC;
			if (SpawnDelay < 0.0f)
			{				
				// Je nach Art der grünen Anzeige vorne einen anderen Gegner spawnen
				switch (DisplayState)
				{
					// Climber
					case 1 : 
					{
						SpawnDelay = 6.0f;
						pGegner->PushGegner(xPos + 100 + rand()%60 , yPos + 190 - DeckelOffset, CLIMBSPIDER, 99, 0, false, false);
					} break;
						
					// Dronen
					case 2 : 
					{
						SpawnDelay = 20.0f;
						pGegner->PushGegner(xPos + 135, yPos + 190 - DeckelOffset, DRONE, 99, 0, false, false);
					} break;

					// Spinnenbombe
					case 3 : 
					{
						SpawnDelay = 15.0f;
						pGegner->PushGegner(xPos + 100 + rand()%80 , yPos + 180 - DeckelOffset, SPIDERBOMB, 99, 0, false, false);
					} break;
				}							
			}
		}
		break;

		// Deckel schliesst sich gerade
		case SCHLIESSEN :
		{
			LightRayCount -= 1.0f SYNC;

			AnimCount += 1.0f SYNC;

			if (AnimCount > 0.8f)
			{
				AnimCount = 0.0f;
				DeckelPhase -= 1;
			}

			if (DeckelPhase < 0)
			{
				DeckelPhase = 0;
				DeckelStatus = ZU;
				OpenCounter = TIME_TILL_OPEN;
				LightRayCount = 0.0f;

				AktionFertig = true;
			}
		}
		break;

	} // switch (DeckelStatus)
}

// --------------------------------------------------------------------------------------
// Kopf hoch und runterfahren
// --------------------------------------------------------------------------------------

void GegnerSpinnenmaschine::DoHoch(void)
{
	switch (HochStatus) 
	{
		// Kopf ist unten, Counter zählt, wann er hochgeht
		case ZU :
		{
			HochCounter -= 1.0f SYNC;

			if (HochCounter < 0.0f)
			{
				HochCounter  = 0.0f;
				HochStatus = OEFFNEN;

				pSoundManager->PlayWave(100, 128, 11025, SOUND_STEAM);				
			}
		}
		break;

		// Kopf fährt gerade hoch
		case OEFFNEN :
		{
			DeckelCount += 0.2f SYNC;			

			if (DeckelCount > PI)
			{
				DeckelCount = PI;
				HochStatus = OFFEN;
				HochCounter = TIME_TILL_HOCH * 2;
				ShotDelay = 10.0f;

				pSoundManager->PlayWave(50, 128, 14000, SOUND_DOORSTOP);
			}

			if (DeckelCount < PI)
			{
				SmokeDelay -= 1.0f SYNC;

				if (SmokeDelay < 0.0f)
				{
					SmokeDelay = 0.4f;
					pPartikelSystem->PushPartikel(xPos + 55.0f, yPos + 375, SMOKE3_LU);
					pPartikelSystem->PushPartikel(xPos + 245.0f, yPos + 375, SMOKE3_RU);
				}
			}
		}
		break;

		// Kopf ist oben und Counter zählt, wann er runtergeht
		case OFFEN :
		{
			HochCounter -= 1.0f SYNC;

			if (HochCounter < 0.0f)
			{
				HochCounter  = 0.0f;
				HochStatus = SCHLIESSEN;

				pSoundManager->PlayWave(50, 128, 11025, SOUND_STEAM);
			}		
			
			ShotDelay -= 1.0f SYNC;

			// schuss abgeben
			if (ShotDelay <= 0.0f)
			{
				ShotDelay = 15.0f;

				pProjectiles->PushProjectile(xPos + 230, yPos + 310, PHARAOLASER, pAim);

				// Sound ausgeben
				pSoundManager->PlayWave(50, 128, 22050, SOUND_PHARAODIE);
				pSoundManager->PlayWave(70, 128, 11025, SOUND_LASERSHOT);
			}
		}
		break;

		// Kopf geht wieder runter
		case SCHLIESSEN :
		{
			DeckelCount -= 0.2f SYNC;

			if (DeckelCount <= 0.0f)
			{
				DeckelCount = 0.0f;
				HochStatus = ZU;
				HochCounter = TIME_TILL_HOCH;

				// Rauch
				for (int i = 1; i < 10; i++)
					pPartikelSystem->PushPartikel(xPos + i * 25.0f, yPos + 330, SMOKEBIG);

				pSoundManager->PlayWave(100, 128, 11025, SOUND_DOORSTOP);

				AktionFertig = true;
			}
		}
		break;

	} // switch (DeckelStatus)
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSpinnenmaschine::DoKI(void)
{
	// Energie anzeigen
	if (Handlung != GEGNER_INIT && 
		Handlung != GEGNER_SPECIAL &&
		Handlung != GEGNER_EXPLODIEREN)
		pHUD->ShowBossHUD(4000, Energy);

	// Boss aktivieren und Mucke laufen lassen
	//
	if (Active == true && 
		Handlung != GEGNER_SPECIAL &&
		pTileEngine->Zustand == ZUSTAND_SCROLLBAR)
	{
		if (PlayerAbstandHoriz() < 450)
		{
			pTileEngine->ScrollLevel((float)Value1, 
									 (float)Value2, ZUSTAND_SCROLLTOLOCK);		// Level auf den Boss zentrieren
			pSoundManager->FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
		}
	}

	// Zwischenboss blinkt nicht so lange wie die restlichen Gegner
	if (DamageTaken > 0.0f)
		DamageTaken -= 50 SYNC;					// Rotwerden langsam ausfaden lassen
	else
		DamageTaken = 0.0f;						// oder ganz anhalten

	// Hat die Maschine keine Energie mehr ? Dann explodiert sie
	if (Energy <= 100.0f && 
		Handlung != GEGNER_EXPLODIEREN &&
		Handlung != GEGNER_SPECIAL)
	{
		Handlung   = GEGNER_EXPLODIEREN;
		SpawnDelay = 0.0f;
		xSpeed     = 0.0f;
		ySpeed     = 0.0f;
		xAcc       = 0.0f;
		yAcc       = 0.0f;
		DeathCount = 40.0f;		

		pGegner->PushGegner(140, 820, ONEUP, 0, 0, false);

		pTileEngine->MaxOneUps++;

		// Endboss-Musik ausfaden und abschalten
		pSoundManager->FadeSong(MUSIC_BOSS, -2.0f, 0, false);
	}

	
	// Bei Damage dampfen lassen
	if (Handlung != GEGNER_SPECIAL)
		SmokeDelay2 -= 1.0f SYNC;
	
	if (SmokeDelay2 < 0.0f)
	{
		SmokeDelay2 = 0.3f;

		// Links oben rausdampfen lassen
		if (Energy < 3800)
			pPartikelSystem->PushPartikel(xPos + 45.0f, yPos + 200.0f - DeckelOffset, SMOKE3_LO);

		// Am Schlauch dampfen lassen
		if (Energy < 3600)
			pPartikelSystem->PushPartikel(xPos + 247.0f, yPos + 278.0f - DeckelOffset, SMOKE3_RO);

		if (Energy < 3400)
			pPartikelSystem->PushPartikel(xPos + 228.0f, yPos + 202.0f - DeckelOffset, SMOKE3_RO);
		
		if (Energy < 3100)
			pPartikelSystem->PushPartikel(xPos + 40.0f, yPos + 228.0f - DeckelOffset, SMOKE3_LO);

		if (Energy < 2600)
			pPartikelSystem->PushPartikel(xPos + 270.0f, yPos + 310.0f - DeckelOffset, SMOKE3_R);

		if (Energy < 2200)
			pPartikelSystem->PushPartikel(xPos + 178.0f, yPos + 205.0f - DeckelOffset, SMOKE3);

		if (Energy < 1800)
			pPartikelSystem->PushPartikel(xPos + 178.0f, yPos + 205.0f - DeckelOffset, SMOKE3);

		if (Energy < 1400)
			pPartikelSystem->PushPartikel(xPos + 73.0f, yPos + 280.0f - DeckelOffset, SMOKE3_L);

		if (Energy < 1000)
			pPartikelSystem->PushPartikel(xPos + 108.0f, yPos + 389.0f, SMOKE3_LU);
	}
	
// Je nach Handlung richtig verhalten
	switch (Handlung)
	{
		// Warten bis der Screen zentriert wurde
		case GEGNER_INIT:			
		{
			if (pTileEngine->Zustand == ZUSTAND_LOCKED)
			{
				// Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
				//
				if (FMUSIC_IsPlaying(pSoundManager->its_Songs[MUSIC_BOSS]->SongData) == false)
				{
					pSoundManager->PlaySong(MUSIC_BOSS, false);

					// Und Boss erscheinen lassen
					//
					Handlung = GEGNER_EINFLIEGEN;
				}				
			}
		} break;

		case GEGNER_EINFLIEGEN:		// Gegner kommt in den Screen geflogen
		{
			Energy = 4000;
			DamageTaken = 0.0f;

			DisplayState = rand()%3 + 1;
			OldDisplayState = DisplayState;

			Handlung = GEGNER_LAUFEN;

		} break;

		// Kopf hoch- /runterfahren
		case GEGNER_STEHEN:
		{
			// Abhandeln, wann der Kopf hochfährt
			DoHoch();

			if (AktionFertig == true)
			{
				AktionFertig = false;
				Handlung = GEGNER_LAUFEN;

				do
				{
					DisplayState = rand()%3 + 1;
				}
				while (DisplayState == OldDisplayState);

				OldDisplayState = DisplayState;
			}
		}
		break;

		// Deckel öffnen
		case GEGNER_LAUFEN:
		{
			// Deckel abhandeln
			DoDeckel();	

			if (AktionFertig == true)
			{
				AktionFertig = false;
				Handlung = GEGNER_STEHEN;				
				DisplayState = 0;
			}
		} break;

		case GEGNER_SPECIAL:
		{			
			if (PlayerAbstand(true) < 800)
			{
				SmokeDelay -= 1.0f SYNC;

				if (SmokeDelay < 0.0f)
				{
					SmokeDelay = 1.0f;
					pPartikelSystem->PushPartikel(xPos + rand()%250, yPos + 300 + rand()%100, SMOKEBIG);
				}
			}
		} break;

		case GEGNER_EXPLODIEREN:
		{			
			Energy = 100.0f;

			SpawnDelay -= 1.0f SYNC;

			if (SpawnDelay < 0.0f)
			{
				SpawnDelay = 0.4f;

				int xo = rand()%300;
				int yo = rand()%400;

				pPartikelSystem->PushPartikel(xPos + xo, yPos + yo, EXPLOSION_MEDIUM2);

				// ggf. Rauch
				if (rand()%2 == 0)
					pPartikelSystem->PushPartikel(xPos + rand()%300, yPos + rand()%400, SMOKEBIG);

				// ggf Explosion Traces
				if (rand()%10 == 0)
					pPartikelSystem->PushPartikel(xPos + 100 + rand()%100, yPos + 200 + rand()%200, EXPLOSION_TRACE);

				// ggf. Sound
				if (rand()%3 == 0)
					pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION3 + rand()%2);

				// ggf. Splitter erzeugen
				if (yo > 100 && rand()%5 == 0)
				for (int i = 0; i < 10; i++)
					pPartikelSystem->PushPartikel(xPos + xo - 10 + rand()%20, yPos + yo - 10 + rand()%20, SPIDERSPLITTER);
			}

			DeathCount -= 1.0f SYNC;

			// fertig explodiert? Dann ganz zerlegen, Unterteil bleibt stehen
			if (DeathCount < 0.0f)
			{
				Handlung = GEGNER_SPECIAL;
				Energy = 1.0f;
				Destroyable = false;
				pPlayer[0]->Score += 8000;

				pSoundManager->PlayWave(100, 128, 11025, SOUND_EXPLOSION2);
				ScrolltoPlayeAfterBoss();

				ShakeScreen(5.0f);

				// Splitter und Große Explosionen
				for (int i = 0; i < 10; i++)
				{
					pPartikelSystem->PushPartikel(xPos + rand()%300, yPos + rand()%400, SPIDERSPLITTER);
					pPartikelSystem->PushPartikel(xPos + 50 + rand()%200, yPos + 100 + rand()%300, EXPLOSION_TRACE);
				}

				// Explosionen und Rauch
				for (int i = 0; i < 50; i++)
				{
					pPartikelSystem->PushPartikel(xPos + rand()%300, yPos + rand()%300 + 100, EXPLOSION_MEDIUM2);
					pPartikelSystem->PushPartikel(xPos + rand()%300, yPos + rand()%300 + 100, SMOKEBIG);
				}

				// Funken
				for (int i = 0; i < 300; i++)
					pPartikelSystem->PushPartikel(xPos + rand()%300, yPos + rand()%300 + 100, FUNKE);

				// Unterteilanim == kaputt
				AnimUnten  = 1;
			}
		} break;

		default : break;
	} // switch

	// Testen, ob der Spieler die Spinnenmaschine berührt hat
	// dafür nehmen wir ein anderes Rect, weil das normale GegnerRect nur das Auge ist, wo man den Gegner treffen kann
	//
	RECT rect;

	rect.top    = 0;
	rect.left   = 0;
	rect.bottom = 317;
	rect.right  = 400;

	//if (SpriteCollision(xPos, yPos, rect,
	//					pPlayer->xpos, pPlayer->ypos, pPlayer->CollideRect) == true)
	//	pPlayer->DamagePlayer(float(4.0 SYNC));

	// Deckel zu? Dann kann der Boss nicht getroffen werden
	if (HochStatus == ZU)
	{
		GegnerRect[GegnerArt].left = 0;
		GegnerRect[GegnerArt].top = 0;
		GegnerRect[GegnerArt].bottom = 0;
		GegnerRect[GegnerArt].right = 0;

		Destroyable = false;
	}

	// andererseits kann man ihn am Auge treffen
	else
	if (Handlung != GEGNER_SPECIAL)
	{
		GegnerRect[GegnerArt].left = 204;
		GegnerRect[GegnerArt].top = 350 - (int)(DeckelOffset);
		GegnerRect[GegnerArt].bottom = 350;
		GegnerRect[GegnerArt].right = 204 + 47;

		Destroyable = true;
	}

	// Spieler kommt nicht dran vorbei
	if (Handlung != GEGNER_SPECIAL)
	for (int p = 0; p < NUMPLAYERS; p++)
	if(pPlayer[p]->xpos < xPos + 250)
	{
		if (pPlayer[p]->Handlung == RADELN ||
			pPlayer[p]->Handlung == RADELN_FALL)
		{
			if (pPlayer[p]->Blickrichtung == LINKS)
				pPlayer[p]->Blickrichtung = RECHTS;
		}
		else
			pPlayer[p]->xpos = xPos + 250;
	}

}

// --------------------------------------------------------------------------------------
// Spinnenmaschine explodiert nicht, sondern bleibt kaputt stehen
// --------------------------------------------------------------------------------------

void GegnerSpinnenmaschine::GegnerExplode(void)
{		
}
