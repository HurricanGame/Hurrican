// --------------------------------------------------------------------------------------
// Das Ufo
//
// fliegt von links nach rechts und andersrum über dem Spieler und ballert dabei
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_Ufo.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerUfo::GegnerUfo(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_NOTVISIBLE;
	BlickRichtung	= LINKS;
	Energy			= 4000;
	Value1			= Wert1;
	Value2			= Wert2;
	AnimEnde		= 21;
	AnimSpeed		= 1.0f;
	ChangeLight		= Light;
	Destroyable		= true;
	ActionDelay     = 40.0f;
	SmokeDelay		= 0.0f;
	OwnDraw			= true;
	LastAction		= -1;

	pTileEngine->MaxOneUps++;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerUfo::DoDraw(void)
{
	pGegnerGrafix[GegnerArt]->RenderSprite(xPos - (float)pTileEngine->XOffset,
										   yPos - (float)pTileEngine->YOffset,
										   AnimPhase, 0xFFFFFFFF);

	if (AlreadyDrawn == false)
	{
		DirectGraphics.SetAdditiveMode();
		LavaFlare.RenderSpriteRotated(xPos + 40.0f - (float)pTileEngine->XOffset,
									  yPos + 20.0f - (float)pTileEngine->YOffset,
									  xPos * 0.5f, 0xFFFF2288);

//		LavaFlare.RenderSpriteRotated(xPos + 40.0f - (float)pTileEngine->XOffset,
//									  yPos + 30.0f - (float)pTileEngine->YOffset,
//									  360.0f - xPos * 0.5f, 0x88FFFF88);

		DirectGraphics.SetColorKeyMode();
	}

	AlreadyDrawn = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerUfo::DoKI(void)
{
	SmokeDelay -= 1.0f SYNC;

	// Energie anzeigen
	if (Handlung != GEGNER_INIT && Handlung != GEGNER_EXPLODIEREN)
		pHUD->ShowBossHUD(4000, Energy);

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
		yPos -= 300;												// und Boss aus dem Screen setzen
		Handlung = GEGNER_INIT;

		pSoundManager->FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
	}

	// Zwischenboss blinkt nicht so lange wie die restlichen Gegner
	if (DamageTaken > 0.0f)
		DamageTaken -= 100 SYNC;				// Rotwerden langsam ausfaden lassen
	else
		DamageTaken = 0.0f;						// oder ganz anhalten

	// Schon schwer angeschlagen ? Dann raucht das Ufo =)
	if (Energy < 700 && rand()%5 == 0)
		pPartikelSystem->PushPartikel(xPos + rand()%190, yPos + rand()%60+25, SMOKE);

	// Hat der Boss keine Energie mehr ? Dann explodiert er
	if (Energy <= 100.0f && Handlung != GEGNER_EXPLODIEREN)
	{
		Handlung  = GEGNER_EXPLODIEREN;
		xSpeed    = 0.0f;
		ySpeed    = 0.0f;
		xAcc      = 0.0f;
		yAcc      = 0.5f;
		AnimEnde  = 0;
		AnimCount = 1.0f;

		// Endboss-Musik ausfaden und abschalten
		pSoundManager->FadeSong(MUSIC_BOSS, -2.0f, 0, false);
	}

	// Über dem Spieler schweben
	//
	if (Handlung != GEGNER_INIT		  && 
		Handlung != GEGNER_EINFLIEGEN &&
		Handlung != GEGNER_EXPLODIEREN)
	{
		if (xPos + 100 < Value1 + 320) 
				xAcc =  3.0f;
			else
				xAcc = -3.0f;
	}

// Je nach Handlung richtig verhalten
	switch (Handlung)
	{
		case GEGNER_INIT:			// Warten bis der Screen zentriert wurde
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

		case GEGNER_EINFLIEGEN:		// Gegner kommt in den Screen geflogen
		{
			Energy = 4000;
			DamageTaken = 0.0f;

			yPos += float(8.0 SYNC);					// Ufo nach unten bewegen
			if (yPos >= pTileEngine->ScrolltoY + 100)	// Weit genug unten ?
			{
				yPos	 = float (pTileEngine->ScrolltoY + 100);
				Handlung = GEGNER_LAUFEN;
				xSpeed	 = - 35.0f;
			}
		} break;

		case GEGNER_LAUFEN:			// Über dem Spieler schweben
		{
			// Nach unten fliegen oder oben
			if (yPos < Value2 + 100) yPos += 5.0f SYNC;
			if (yPos > Value2 + 100) yPos -= 5.0f SYNC;

			// Wenig Energie? Dann Gegner spawnen
			//if (Energy <= 2000.0f)
			{
				ShotDelay -= 1.0f SYNC;

				if (ShotDelay < 0.0f)
				{	
					pGegner->PushGegner(xPos + 45.0f, yPos + 40.0f, FETTERAKETE, 360, 99, false);
					pGegner->PushGegner(xPos + 135.0f, yPos + 40.0f, FETTERAKETE, 0, 99, false);
					ShotDelay = 18.0f;
					pSoundManager->PlayWave(100, 128, 8000 + rand()%1000, SOUND_GRANATE);
				}
			}


			// Neue Action ausführen ?
			//
			ActionDelay -= 1.0f SYNC;

			if (ActionDelay < 0.0f)
			{
				int j = rand()%3;

				while (j == LastAction)
					j = rand()%3;

  				LastAction = j;

				if (j == 0)	Handlung = GEGNER_SCHIESSEN;
				if (j == 1)	Handlung = GEGNER_BOMBARDIEREN;
				if (j == 2)	
				{	
					Handlung  = GEGNER_FALLEN;
					ySpeed    =  28.0f;
					yAcc      = - 2.0f;
					ShotDelay = 1.0f;
				}

				ShotDelay    = 10.0f;
				ActionDelay  = 60.0f;
			}
		} break;

		case GEGNER_SCHIESSEN:		// Mit Suchschuss auf den Spieler schiessen
		{
			// Nach unten fliegen
			if (yPos > Value2 + 50) 
				yPos -= 5.0f SYNC;
			else
				yPos = Value2 + 50.0f;

			// Wieder nur rumschweben ?
			ActionDelay -= 1.0f SYNC;
			if (ActionDelay < 0.0f)
			{
				ActionDelay = 80.0f;
				Handlung	= GEGNER_LAUFEN;
			}

			if (ShotDelay > 0.0f)
				ShotDelay -= 1.0f SYNC;

			if (ShotDelay <= 0.0f)
			{
				pSoundManager->PlayWave(50, 128, 14000 + rand()%2000, SOUND_GOLEMSHOT);					
				pProjectiles->PushProjectile(xPos + 20.0f, yPos + 40.0f, SUCHSCHUSS2);
				pProjectiles->PushProjectile(xPos + 165.0f, yPos + 40.0f, SUCHSCHUSS2);
				ShotDelay = 5.0f;
			}
		} break;

		case GEGNER_BOMBARDIEREN:		// Mit Laser auf den Spieler schiessen
		{
			// Nach oben fliegen
			if (yPos > Value2 + 50) 
				yPos -= 5.0f SYNC;
			else
				yPos = Value2 + 50.0f;

			// Wieder nur rumschweben ?
			ActionDelay -= 1.0f SYNC;
			if (ActionDelay < 0.0f)
			{
				ActionDelay = 60.0f;
				Handlung	= GEGNER_LAUFEN;
			}

			if (ShotDelay > 0.0f)
				ShotDelay -= 1.0f SYNC;

			if (ShotDelay <= 0.0f)
			{
				ShotDelay = 4.0f;

				pSoundManager->PlayWave (100, 128, 10000 + rand()%500, SOUND_LASERSHOT);

				pProjectiles->PushProjectile  (xPos + 100 - 21, yPos + 60, UFOLASER);
				pPartikelSystem->PushPartikel (xPos + 100 - 80, yPos + 30, UFOLASERFLARE);
			}
		} break;

		case GEGNER_FALLEN:
		{
			// Fertig mit abtauchen ?
			if (ySpeed < -28.0f)
			{
				yAcc        = 0.0f;
				ySpeed      = 0.0f;
				ActionDelay = 30.0f;
				Handlung	= GEGNER_LAUFEN;
			}
		} break;

		// Ufo fliegt in die Luft
		case GEGNER_EXPLODIEREN:
		{
			Energy = 100.0f;
			AnimCount -= 1.0f SYNC;

			if (AnimCount < 0.0f)
			{
				pPartikelSystem->PushPartikel(xPos + rand()%180, yPos + rand()%70+20, EXPLOSION_MEDIUM2);
				pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);

				if (rand()%4 == 0)
					pPartikelSystem->PushPartikel(xPos + rand()%180 - 40, yPos + rand()%40, EXPLOSION_BIG);

				AnimCount = 2.0f;
			}

			if (ySpeed > 20.0f)
				ySpeed = 20.0f;

			// Auf den Boden gecrasht ? Dann wird das Ufo ganz zerlegt
			if (blocku & BLOCKWERT_WAND ||
				blocku & BLOCKWERT_PLATTFORM)
			{
				yAcc   = 0.0f;
				ySpeed = 0.0f;
				Energy = 0.0f;
			}
		} break;

		default : break;
	} // switch

	// Hat das Ufo den Hurri berührt ?
	TestDamagePlayers(20.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Ufo explodiert
// --------------------------------------------------------------------------------------

void GegnerUfo::GegnerExplode(void)
{
	pSoundManager->PlayWave(100, 128, 11025, SOUND_EXPLOSION2);
	ShakeScreen(5.0f);

	// Splitter
	for (int i=0; i<20; i++)
	pPartikelSystem->PushPartikel(xPos + rand()%190, 
								  yPos + rand()%60 + 30, SPLITTER);

	pPlayer[0]->Score += 4000;
	pGegner->PushGegner(xPos + 80.0f, yPos + 20.0f, ONEUP, 0, 0, false);

	ScrolltoPlayeAfterBoss();
}
