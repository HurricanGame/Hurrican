// --------------------------------------------------------------------------------------
// Die riesige Stahlfaust (im Eislevel) - Zwischenboss
//
// fliegt über dem Spieler und versucht, diesen zu zerquetschen
// fliegt ab und zu nach oben aus dem Screen und donnert auf den Hurri runter
// saust von links nach rechts über den screen und lässt Eiszapfen fallen
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_EisFaust.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerEisFaust::GegnerEisFaust(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_NOTVISIBLE;
	BlickRichtung	= LINKS;
	Energy			= 7000;
	Value1			= Wert1;
	Value2			= Wert2;
	AnimPhase		= 0;
	ChangeLight		= Light;
	AnimCount		= 50.0f;		// Counter für Spezial Aktion
	Destroyable		= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerEisFaust::DoKI(void)
{
	// Energie anzeigen
	if (Handlung != GEGNER_NOTVISIBLE && Handlung != GEGNER_EXPLODIEREN)
		pHUD->ShowBossHUD(7000, Energy);

	// Levelausschnitt auf die Faust zentrieren, sobald dieses sichtbar wird
	if (Active == true && pTileEngine->Zustand == ZUSTAND_SCROLLBAR)
	{
		pTileEngine->ScrollLevel((float)Value1, 
								 (float)Value2, ZUSTAND_SCROLLTOLOCK);		// Level auf die Faust zentrieren
		yPos -= 300;												// und Faust aus dem Screen setzen

		pSoundManager->FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
	}

	// Zwischenboss blinkt nicht so lange wie die restlichen Gegner
	if (DamageTaken > 0.0f)
		DamageTaken -= 100 SYNC;				// Rotwerden langsam ausfaden lassen
	else
		DamageTaken = 0.0f;						// oder ganz anhalten

	// Schon schwer angeschlagen ? Dann raucht die Faust =)
	if (Energy < 2000 &&
		rand()%2 == 0)
		pPartikelSystem->PushPartikel(xPos + rand()%200+20, yPos + rand()%200+60, SMOKE);

	// Hat die Faust keine Energie mehr ? Dann explodiert sie
	if (Energy <= 100.0f && Handlung != GEGNER_EXPLODIEREN)
	{
		Handlung  = GEGNER_EXPLODIEREN;
		xSpeed    = 0.0f;
		ySpeed    = 0.0f;
		xAcc      = 0.0f;
		yAcc      = 0.0f;
		AnimCount = 50.0f;

		// Endboss-Musik ausfaden und abschalten
		pSoundManager->FadeSong(MUSIC_BOSS, -2.0f, 0, false);
	}

// Je nach Handlung richtig verhalten
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

		case GEGNER_EINFLIEGEN2:		// Gegner kommt DAS ERSTE MAL in den Screen geflogen
		{
			Energy = 7000;
			DamageTaken = 0.0f;

			yPos += float(8.0 SYNC);					// Faust nach unten bewegen
			if (yPos >= pTileEngine->ScrolltoY)			// Weit genug unten ?
			{
				Handlung = GEGNER_LAUFEN;
				xAcc	 = -8.0f;
			}
		} break;

		case GEGNER_EINFLIEGEN:		// Gegner kommt in den Screen geflogen
		{
			yPos += float(8.0 SYNC);					// Faust nach unten bewegen
			if (yPos >= pTileEngine->ScrolltoY)			// Weit genug unten ?
			{
				Handlung = GEGNER_LAUFEN;
				xAcc	 = -8.0f;
			}
		} break;

		case GEGNER_LAUFEN:			// Über dem Spieler schweben und ggf runtersausen
		{
			AnimCount -= SpeedFaktor;

			// Rechts vom Spieler oder zu nahe am rechten Rand ?
			if (pAim->xpos + pAim->CollideRect.right < xPos ||
				xPos > pTileEngine->ScrolltoX + 480)
				xAcc = -8.0f;							// Dann nach Links fliegen

			// Links vom Spieler oder zu nahe am linken Rand ?
			if (pAim->xpos > xPos + GegnerRect[GegnerArt].right ||
				xPos < pTileEngine->ScrolltoX - 90)
				xAcc = 8.0f;							// Dann nach Rechts fliegen

			// Speed nicht zu hoch werde lassen
			if (xSpeed < -18.0f) xSpeed = -18.0f;
			if (xSpeed >  18.0f) xSpeed =  18.0f;

			// Spieler unter der Faust ? Dann crushen
			if (pAim->xpos < xPos + GegnerRect[GegnerArt].right-115 &&
				pAim->xpos + pAim->CollideRect.right > xPos +155 &&
				rand()%2 == 0)
			{
				Handlung = GEGNER_CRUSHEN;
				ySpeed   = 0.0f;
				yAcc	 = 10.0f;
				xSpeed   = 0.0f;
				xAcc	 = 0.0f;
			}

			// Faust Spezial Aktion und oben rausfliegen ?
			if (AnimCount <= 0.0f)
			{
				AnimCount = 50.0f;			// Nächste Spezial Aktion planen
				Handlung  = GEGNER_SPRINGEN;
				xAcc   = 0.0f;
				xSpeed = 0.0f;
				ySpeed = -5.0f;
				yAcc   = -5.0f;
			}

		} break;

		// Faust zerquetscht den Hurri
		case GEGNER_CRUSHEN:
		{
			// Auf den Boden gecrashed ?
			if (blocku & BLOCKWERT_WAND)
			{
				// Sound ausgeben
				pSoundManager->PlayWave(100, 128, 6000, SOUND_EXPLOSION1);
				pSoundManager->PlayWave(100, 128, 6000, SOUND_LANDEN);

				// Schnee am Boden erzeugen
				for (int i=0; i<80; i++)
					pPartikelSystem->PushPartikel(xPos + rand()%200, yPos + GegnerRect[GegnerArt].bottom - 40 + rand ()% 20, WATERFLUSH2);
			
				// Beschleunigung und Geschwindigkeit wieder richtig setzen um hochzufliegen
				yAcc   = -1.5f;
				ySpeed =  0.0f;

				// Screen Wackeln lassen
				ShakeScreen(2.5);

				Handlung = GEGNER_CRUSHENERHOLEN;
			}
		} break;

		// Faust fliegt nach dem Crushen wieder nach oben
		case GEGNER_CRUSHENERHOLEN:
		{
			// Nach dem nach oben fliegen wieder ganz oben ?
			if (ySpeed < 0.0f &&
				yPos <= pTileEngine->ScrolltoY)
			{
				Handlung = GEGNER_LAUFEN;
				ySpeed = 0.0f;
				yAcc   = 0.0f;
				yPos   = float(pTileEngine->ScrolltoY);
				xAcc   = -8.0f;
			}
		} break;

		// Faust fliegt oben raus
		case GEGNER_SPRINGEN:
		{
			// Oben umkehren neue Aktion machen?
			if (yPos <= pTileEngine->ScrolltoY - 280.0f)
			{
				if (rand()%2 == 0)
				{
					Handlung  = GEGNER_FALLEN;
					AnimPhase = 1;
					yAcc	  = 0;
					ySpeed    = 50.0f;
					xPos	  = pAim->xpos-110;
				}
				else
				{
					Handlung  = GEGNER_BOMBARDIEREN;

					xPos      = float (Value1) - 250.0f;
					yPos      = float (Value2) - 200.0f;
					yAcc	  = 0;
					ySpeed    = 10.0f;
					yAcc	  = -0.4f;
					xSpeed    = 20.0f;
					AnimCount = 8.0f;
				}
			}
		} break;

		// Faust fliegt von rechts nach links und wirft dabei Eiszapfen
		case GEGNER_BOMBARDIEREN:
		{
			if (xSpeed > 0.0f)
				BlickRichtung = RECHTS;
			else
				BlickRichtung = LINKS;

			AnimCount -= 1.0f SYNC;

			while (AnimCount <= 0.0f)
			{
				AnimCount += 4.0f;

				if (xPos + 100 > pTileEngine->XOffset		   &&
					xPos + 100 < pTileEngine->XOffset + 620.0f &&
					yPos + 220 > pTileEngine->XOffset		   &&
					yPos + 220 < pTileEngine->XOffset + 440.0f)
				pGegner->PushGegner (xPos + 100, yPos + 220, EISZAPFEN, 1, 0, false);
			}

			if (xSpeed > 0.0f &&
				xPos > float (Value1) + 640.0f + 30.0f)
			{
				xSpeed	  = -20.0f;
				ySpeed    = 10.0f;
				yPos      = float (Value2) - 100.0f;
			}

			if (xSpeed < 0.0f &&
				xPos < float (Value1) - 300.0f)
			{
				xSpeed	  = 0.0f;
				AnimCount = 50.0f;

				AnimPhase = 0;					// Wieder Faust von der Seite
				Handlung = GEGNER_EINFLIEGEN;
				xSpeed = 0.0f;
				xAcc   = 0.0f;
				ySpeed = 0.0f;
				yAcc   = 0.0f;
				yPos   = float(pTileEngine->ScrolltoY - 400.0f);
				xPos   = float (Value1) + 200;
			}


		} break;

		// Faust fällt auf den Hurri drauf und fliegt dann wieder oben raus
		case GEGNER_FALLEN:
		{
			// Auf den Boden gecrashed ?
			if (blocku & BLOCKWERT_WAND)
			{
				// Sound ausgeben
				pSoundManager->PlayWave(100, 128, 6000, SOUND_EXPLOSION1);
				pSoundManager->PlayWave(100, 128, 6000, SOUND_LANDEN);

				// Schnee am Boden erzeugen
				for (int i=0; i<80; i++)
					pPartikelSystem->PushPartikel(xPos + 30 + rand()%180, yPos + GegnerRect[GegnerArt].bottom - 40 + rand ()% 20, WATERFLUSH2);
			
				// Beschleunigung und Geschwindigkeit wieder richtig setzen um hochzufliegen
				yAcc   = -0.5f;
				ySpeed =  0.0f;

				// Screen Wackeln lassen
				ShakeScreen(5);

				Handlung = GEGNER_STEHEN;
			}
		} break;

		// Faust fliegt nach dem Crushen wieder nach oben raus
		case GEGNER_STEHEN:
		{
			// Nach dem nach oben fliegen wieder ganz oben ?
			if (yPos <= pTileEngine->ScrolltoY - 280.0f)
			{
				AnimPhase = 0;					// Wieder Faust von der Seite
				Handlung = GEGNER_EINFLIEGEN;
				ySpeed = 0.0f;
				yAcc   = 0.0f;
				yPos   = float(pTileEngine->ScrolltoY - 250.0f);
			}
		} break;

		// Faust fliegt in die Luft
		case GEGNER_EXPLODIEREN:
		{
			AnimCount -= SpeedFaktor;

			Energy = 100.0f;

			if (rand()%5 == 0)
			{
				pPartikelSystem->PushPartikel(xPos + rand()%200, yPos + rand()%200+20, EXPLOSION_MEDIUM2);
				pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);
			}

			if (rand()%8 == 0)
				pPartikelSystem->PushPartikel(xPos + rand()%200, yPos + rand()%200+20, EXPLOSION_BIG);

			if (rand()%20 == 0)
				pPartikelSystem->PushPartikel(xPos + rand()%100+60, yPos + rand()%100+60, SPLITTER);

			// Fertig explodiert ? Dann wird sie ganz zerlegt
			if (AnimCount <= 0.0f)
				Energy = 0.0f;
		} break;

		default : break;
	} // switch

	// Hat die Faust den Hurri getroffen ?
	if (Handlung != GEGNER_EXPLODIEREN)
		TestDamagePlayers(5.0f SYNC);
}

// --------------------------------------------------------------------------------------
// EisFaust explodiert
// --------------------------------------------------------------------------------------

void GegnerEisFaust::GegnerExplode(void)
{
	// Splitter
	for (int i=0; i<20; i++)
		pPartikelSystem->PushPartikel(xPos + 60 + rand()%60, 
									  yPos + 80 + rand()%40, SPLITTER);

	for (int i=0; i<60; i++)
	{
		pPartikelSystem->PushPartikel(xPos + 20 + rand()%100, 
									  yPos + 40 + rand()%100, SPIDERSPLITTER2);

		pPartikelSystem->PushPartikel(xPos + 60 + rand()%60, 
									  yPos + 80 + rand()%40, WATERFLUSH2);
	}

	pPlayer[0]->Score += 5000;

	ScrolltoPlayeAfterBoss();
}
