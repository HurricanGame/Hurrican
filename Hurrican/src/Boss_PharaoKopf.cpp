// --------------------------------------------------------------------------------------
// Die Pharao-Kopf Endgegner
//
// rammt seitlich gegen die Wände und versucht den Hurri zu zerquetschen
// lässt ggf Steine von der Decke rieseln
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_PharaoKopf.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPharaoKopf::GegnerPharaoKopf(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_NOTVISIBLE;
	BlickRichtung	= LINKS;

	Energy = 6000;
	Value1			= Wert1;
	Value2			= Wert2;
	AnimPhase		= 0;
	ChangeLight		= Light;
	AnimCount		= 100.0f;		// Counter für Spezial Aktion
	Destroyable		= true;
	TestBlock	    = false;
}

bool GegnerPharaoKopf::Links(void)
{
	if ((int)(xPos) < Value1 + BORDER)
	{
		xPos = (float)(Value1 + BORDER);
		return true;
	}
	
	return false;
}

bool GegnerPharaoKopf::Rechts(void)
{
	if ((int)(xPos) + GegnerRect[GegnerArt].right > Value1 + 652 - BORDER)
	{
		xPos = (float)(Value1 + 652 - BORDER - GegnerRect[GegnerArt].right);
		return true;
	}

	return false;
}

bool GegnerPharaoKopf::Unten(void)
{
	if ((int)(yPos) + GegnerRect[GegnerArt].bottom > Value2 + 480 - BORDER2)
	{
		yPos = (float)(Value2 + 480 - BORDER2 - GegnerRect[GegnerArt].bottom);
		return true;
	}
	
	return false;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerPharaoKopf::DoKI(void)
{
	// Energie anzeigen
	if (Handlung != GEGNER_NOTVISIBLE && Handlung != GEGNER_EXPLODIEREN)
		pHUD->ShowBossHUD(6000, Energy);

//----- Pharao dient auch als Plattform
	PlattformTest(GegnerRect[GegnerArt]);

//----- Pharao Bewegungs Code

	// Levelausschnitt auf den PharaoKopf zentrieren, sobald dieser sichtbar wird
	if (Active == true && pTileEngine->Zustand == ZUSTAND_SCROLLBAR)
	{
		pTileEngine->ScrollLevel((float)Value1, 
								 (float)Value2, ZUSTAND_SCROLLTOLOCK);		// Level auf die Faust zentrieren

		pSoundManager->FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
	}

	// Zwischenboss blinkt nicht so lange wie die restlichen Gegner
	if (DamageTaken > 0.0f)
		DamageTaken -= 100 SYNC;				// Rotwerden langsam ausfaden lassen
	else
		DamageTaken = 0.0f;						// oder ganz anhalten

	// Hat der PharaoKopf keine Energie mehr ? Dann explodiert er
	if (Energy <= 100.0f && Handlung != GEGNER_EXPLODIEREN)
	{
		Handlung  = GEGNER_EXPLODIEREN;
		xSpeed    = 0.0f;
		ySpeed    = 0.0f;
		xAcc      = 0.0f;
		yAcc      = 0.0f;
		AnimCount = 20.0f;

		pSoundManager->PlayWave(100, 128, 11024, SOUND_EXPLOSION1);
		pSoundManager->PlayWave(100, 128, 11025, SOUND_PHARAODIE);

		// Endboss-Musik ausfaden und abschalten
		pSoundManager->FadeSong(MUSIC_BOSS, -2.0f, 0, false);
	}

	// Kopf zieht dem Spieler Energie an wenn er ihn berührt
	TestDamagePlayers(2.0f SYNC);


	if (Energy >= 3000 && AnimPhase > 1)
		AnimPhase = 0;

	// Hälfte der Energie verloren ? Dann Risse Zeigen und kurz Brocken fliegen lassen
	if (Energy < 3000 && AnimPhase < 2)
	{
		AnimPhase = 2;

		// Brocken
		for (int i=0; i<100; i++)
		{
			pPartikelSystem->PushPartikel(xPos + rand()%224, yPos + rand()%224, ROCKSPLITTER);
			pPartikelSystem->PushPartikel(xPos + rand()%224, yPos + rand()%224, ROCKSPLITTERSMALL);
		}

		pSoundManager->PlayWave(100, 128, 11025, SOUND_PHARAORAMM);
		pSoundManager->PlayWave(100, 128, 11025, SOUND_PHARAODIE);
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

				AnimCount = 1.0f;
			}
		} break;

		case GEGNER_EINFLIEGEN:		// Gegner kommt aus dem Boden raus
		{
			Energy = 6000;
			DamageTaken = 0.0f;

			yPos -= float(4.5 SYNC);					// Pharao erscheint langsam

			AnimCount -= 1.0f SYNC;

			// Boden brodeln lassen wo er auftaucht
			if (AnimCount <= 0.0f)
			{
				pPartikelSystem->PushPartikel(xPos + float(rand()%230)-30, float(Value2 + 400 + rand()%12), SMOKEBIG);
				pPartikelSystem->PushPartikel(xPos + float(rand()%230)-12, float(Value2 + 405 + rand()%12), ROCKSPLITTER);
				pPartikelSystem->PushPartikel(xPos + float(rand()%230)-12, float(Value2 + 410 + rand()%12), ROCKSPLITTERSMALL);

				AnimCount += 0.2f;
			}

			if (yPos <= (float)Value2 + 480 - BORDER2 - GegnerRect[GegnerArt].bottom)			// Weit genug unten ?
			{
				for (int p = 0; p < NUMPLAYERS; p++)
				if (pPlayer[p]->AufPlattform == this)
				{
					ySpeed = -10.0f;
					yAcc   = - 3.0f;
					goto _weiter;
				}
				
				if (pAim->xpos < xPos)
				{
					xSpeed = -1.0f;
					xAcc   = -3.0f;
				}
				else
				{
					xSpeed = 1.0f;
					xAcc   = 3.0f;
				}
_weiter:
				yPos = (float)Value2 + 480 - BORDER2 - GegnerRect[GegnerArt].bottom;
				pSoundManager->PlayWave(100, 128, 8000, SOUND_STONEFALL);

				Handlung = GEGNER_LAUFEN;
			}
		} break;

		case GEGNER_LAUFEN:			// In Richtung Spieler rutschen
		{
			xSpeed = 0.0f;
			ySpeed = 0.0f;

			// Spieler steht auf dem Pharao Kopf ?
			for (int p = 0; p < NUMPLAYERS; p++)
			if (pPlayer[p]->AufPlattform == this)
			{
				ySpeed = -10.0f;
				yAcc   = - 3.0f;
				goto _weiter2;
			}

			// Spieler ist am Boden
			else
			{
				if (pAim->xpos < xPos)
				{
					xSpeed = -1.0f;
					xAcc   = -3.0f;
				}
				else
				{
					xSpeed = 1.0f;
					xAcc   = 3.0f;
				}

				for (p = 0; p < NUMPLAYERS; p++)
				if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], 
									pPlayer[p]->xpos, pPlayer[p]->ypos, pPlayer[p]->CollideRect) == true)
				{
									xSpeed = -xSpeed;
									xAcc   = -xAcc;
				}
			}
_weiter2:

			Handlung = GEGNER_CRUSHEN;
		} break;

		case GEGNER_CRUSHEN:		// An die Wand klatschen und an der Wand anhalten =)
		{
			// Partikel am Boden
			if (ySpeed == 0)
				pPartikelSystem->PushPartikel(xPos + rand()%224, yPos + 210, ROCKSPLITTERSMALL);

			// Links an die Wand rutschen ?
			if (xSpeed < 0)
			{
				// Hurri verschieben
				for (int p = 0; p < NUMPLAYERS; p++)
				if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], 
									pPlayer[p]->xpos, pPlayer[p]->ypos, pPlayer[p]->CollideRect) == true &&
					!(pTileEngine->BlockLinks(pPlayer[p]->xpos, pPlayer[p]->ypos, pPlayer[p]->xposold, pPlayer[p]->yposold, pPlayer[p]->CollideRect) & BLOCKWERT_WAND))
					pPlayer[p]->xpos += xSpeed SYNC;

				// an die Wand gekommen?
				if (Links())
				{					
					xSpeed = 0.0f;
					xAcc   = 0.0f;

					for (int p = 0; p < NUMPLAYERS; p++)
						DirectInput.Joysticks[pPlayer[p]->JoystickIndex].ForceFeedbackEffect(FFE_MEDIUMRUMBLE);

					for (int i = 0; i < 20; i++)
					{
						pPartikelSystem->PushPartikel(xPos-10, yPos - 30 + rand()%224, SMOKEBIG);
						pPartikelSystem->PushPartikel(xPos-10, yPos + rand()%224, ROCKSPLITTER);
						pPartikelSystem->PushPartikel(xPos-10, yPos + rand()%224, ROCKSPLITTERSMALL);
					}

					pSoundManager->PlayWave(100, 128, 11024, SOUND_PHARAORAMM);
					Handlung  = GEGNER_CRUSHENERHOLEN;
					AnimCount = 10.0f;

					// Hurri dabei erwischt ?
					TestDamagePlayers(1000.0f);

					// Screen Wackeln lassen
					ShakeScreen(3);
				}
			}

			// Rechts an die Wand rutschen ?
			if (xSpeed > 0)
			{
				// Hurri verschieben
				for (int p = 0; p < NUMPLAYERS; p++)
				if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], 
									pPlayer[p]->xpos, 
									pPlayer[p]->ypos, 
									pPlayer[p]->CollideRect) == true &&
									!(blockr & BLOCKWERT_WAND))
					pPlayer[p]->xpos += xSpeed SYNC;

				// an die Wand gekommen?
				if (Rechts())
				{			
					for (int p = 0; p < NUMPLAYERS; p++)
						DirectInput.Joysticks[pPlayer[p]->JoystickIndex].ForceFeedbackEffect(FFE_MEDIUMRUMBLE);

					xSpeed = 0.0f;
					xAcc   = 0.0f;
					for (int i = 0; i < 20; i++)
					{
						pPartikelSystem->PushPartikel(xPos+175, yPos + rand()%224 - 30, SMOKEBIG);
						pPartikelSystem->PushPartikel(xPos+212, yPos + rand()%224, ROCKSPLITTER);
						pPartikelSystem->PushPartikel(xPos+212, yPos + rand()%224, ROCKSPLITTERSMALL);
					}

					pSoundManager->PlayWave(100, 128, 11024, SOUND_PHARAORAMM);
					Handlung  = GEGNER_CRUSHENERHOLEN;
					AnimCount = 10.0f;

					// Hurri dabei erwischt ?
 					TestDamagePlayers(1000.0f);

					// Screen Wackeln lassen
					ShakeScreen(3);
				}
			}

			// An die Decke gekommen ?
			if (ySpeed < 0)
			{
				if (yPos < (float)Value2)
				{
					for (int i = 0; i < 20; i++)
					{
						pPartikelSystem->PushPartikel(xPos - 30 + rand()%224, yPos, SMOKEBIG);
						pPartikelSystem->PushPartikel(xPos + rand()%224, yPos + 10, ROCKSPLITTER);
						pPartikelSystem->PushPartikel(xPos + rand()%224, yPos + 10, ROCKSPLITTERSMALL);
					}

					pSoundManager->PlayWave(100, 128, 11024, SOUND_PHARAORAMM);
					yPos	= (float)Value2;
					ySpeed  = 5.0f;
					yAcc	= 10.0f;

					for (int p = 0; p < NUMPLAYERS; p++)
					{
						if (pPlayer[p]->AufPlattform == this)
							pPlayer[p]->DamagePlayer(200.0f);

						pPlayer[p]->AufPlattform = NULL;
					}							

					// Screen Wackeln lassen
					ShakeScreen(3);
				}
			} else

			// An den Boden gekommen ?
			if (ySpeed > 0.0f)
			{
				// Hurri dabei erwischt ?
				for (int p = 0; p < NUMPLAYERS; p++)
				if (pPlayer[p]->Energy > 0 &&
					pPlayer[p]->AufPlattform == NULL &&
					SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], 
									pPlayer[p]->xpos, 
									pPlayer[p]->ypos, 
									pPlayer[p]->CollideRect) == true)
				{
					pPlayer[p]->DamagePlayer(25.0f);

					// Wieder hoch fliegen
					ySpeed = -10.0f;
					yAcc   = - 3.0f;
				}

				if (Unten())
				{
					ySpeed = 0.0f;
					yAcc   = 0.0f;
					for (int i = 0; i < 20; i++)
					{
						pPartikelSystem->PushPartikel(xPos - 30 + rand()%224, yPos + 170, SMOKEBIG);
						pPartikelSystem->PushPartikel(xPos + rand()%224, yPos + 220, ROCKSPLITTER);
						pPartikelSystem->PushPartikel(xPos + rand()%224, yPos + 220, ROCKSPLITTERSMALL);
					}

					pSoundManager->PlayWave(100, 128, 11024, SOUND_PHARAORAMM);
					pTileEngine->BlockUnten(xPos, yPos, xPos, yPos, GegnerRect[GegnerArt]);
					AnimCount = 20.0f;
					Handlung  = GEGNER_CRUSHENERHOLEN;
				}
				// Screen Wackeln lassen
				ShakeScreen(3);
			}
		} break;

		case GEGNER_CRUSHENERHOLEN:
		{
			// ErholungsCounter runterzählen
			AnimCount -= 1.0f SYNC;

			if (AnimCount <= 0.0f)
			{
				int j = rand()%3;

				// Schiessen ?
				if (j == 0 && AnimPhase > 1)
				{
					Handlung = GEGNER_SCHIESSEN;
					AnimPhase = 3;
					AnimCount = 8.0f;
				}
				else 
					j = rand()%2+1;

				// Steine von der Decke rieseln lassen ?
				if (j == 1)
				{
					// Springen
					ySpeed    = - 10.0f;
					yAcc      =   2.0f;

					// böse kucken
					if (AnimPhase == 0)
						AnimPhase = 1;
					else 
						AnimPhase = 3;

					Handlung = GEGNER_SPECIAL;
					pTileEngine->BlockUnten (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
				}

				// Wieder zerquetschen
				bool PlayerOn = false;
				for (int p = 0; p < NUMPLAYERS; p++)
					if (pPlayer[p]->AufPlattform == this)
						PlayerOn = true;

				if (j == 2 || 
					PlayerOn)
				{
					Handlung = GEGNER_LAUFEN;

					if (j == 2)
						pSoundManager->PlayWave(100, 128, 8000, SOUND_STONEFALL);
				}
			}
		} break;

		case GEGNER_SPECIAL:		// Boss hopft auf und ab und lässt Steine von der Decke rieseln
		{
			if (Unten() && ySpeed > 0.0f)
			{
				// Stein erscheinen lassen
				pGegner->PushGegner(pAim->xpos, float(Value2), FALLINGROCK, 0, 0, false);

				// Partikel unter Boss
				for (int i = 0; i < 20; i++)
				{
					pPartikelSystem->PushPartikel(xPos - 30 + rand()%224, yPos + 170, SMOKEBIG);
					pPartikelSystem->PushPartikel(xPos + rand()%224, yPos + 220, ROCKSPLITTER);
					pPartikelSystem->PushPartikel(xPos + rand()%224, yPos + 220, ROCKSPLITTERSMALL);
				}

				pSoundManager->PlayWave(100, 128, 11024, SOUND_PHARAORAMM);

				ySpeed    = 0.0f;
				yAcc      = 0.0f;
				AnimCount = 10.0f;
				Handlung  = GEGNER_CRUSHENERHOLEN;

				// Normal kucken
				if (AnimPhase == 1)
					AnimPhase = 0;
				else
					AnimPhase = 2;
			}
		} break;

		case GEGNER_SCHIESSEN:		// Gegner schiesst aus den Augen
		{
			AnimCount -= 1.0f SYNC;

			if (AnimCount <= 0)
			{
				// Augen leuchten lassen
				pPartikelSystem->PushPartikel(xPos +  68, yPos + 77, LASERFLAMEPHARAO);
				pPartikelSystem->PushPartikel(xPos +  68, yPos + 77, LASERFLAMEPHARAO);
				pPartikelSystem->PushPartikel(xPos + 112, yPos + 77, LASERFLAMEPHARAO);
				pPartikelSystem->PushPartikel(xPos + 112, yPos + 77, LASERFLAMEPHARAO);

				// Schüsse erzeugen
				pProjectiles->PushProjectile(xPos +  68, yPos + 77, PHARAOLASER, pAim);
				pProjectiles->PushProjectile(xPos + 112, yPos + 77, PHARAOLASER, pAim);

				// Sound ausgeben
				pSoundManager->PlayWave(50, 128, 22050, SOUND_PHARAODIE);
				pSoundManager->PlayWave(70, 128, 11025, SOUND_LASERSHOT);

				// Je nach SchwierigkeitsGrad die Schuss Frequenz setzen
				if (Skill == 0)	AnimCount = 10.0f;
				if (Skill == 1)	AnimCount =  8.5f;
				if (Skill == 2)	AnimCount =  7.0f;
				if (Skill == 3)	AnimCount =  6.0f;

				if (rand()%2 == 0)
				{
					AnimPhase = 2;
					Handlung = GEGNER_CRUSHENERHOLEN;
				}
			}
		} break;

		// Pharao Kopf explodiert 
		case GEGNER_EXPLODIEREN:
		{
			AnimCount -= SpeedFaktor;
			Energy = 100.0f;

			// brodeln lassen
			pPartikelSystem->PushPartikel(xPos + rand()%224, yPos + rand()%224, ROCKSPLITTER);
			pPartikelSystem->PushPartikel(xPos + rand()%224, yPos + rand()%224, ROCKSPLITTERSMALL);

			// Fertig explodiert ? Dann wird er ganz zerlegt
			if (AnimCount <= 0.0f)
				Energy = 0.0f;
		} break;

		default : break;
	} // switch
}

// --------------------------------------------------------------------------------------
// PharaoKopf explodiert
// --------------------------------------------------------------------------------------

void GegnerPharaoKopf::GegnerExplode(void)
{
	for (int p = 0; p < NUMPLAYERS; p++)
	{
		if (pPlayer[p]->AufPlattform == this)
			pPlayer[p]->AufPlattform = NULL;

		DirectInput.Joysticks[pPlayer[p]->JoystickIndex].ForceFeedbackEffect(FFE_BIGRUMBLE);
	}

	// Splitter
	for (int i=0; i<50; i++)
	{
		pPartikelSystem->PushPartikel(xPos + rand()%224, yPos + rand()%224, ROCKSPLITTER);
		pPartikelSystem->PushPartikel(xPos + rand()%224, yPos + rand()%224, ROCKSPLITTERSMALL);
		pPartikelSystem->PushPartikel(xPos + rand()%224, yPos + rand()%224, SMOKEBIG);
	}

	pPlayer[0]->Score += 8000;

	pSoundManager->PlayWave(100, 128, 11024, SOUND_EXPLOSION2);
	pSoundManager->PlayWave(100, 128, 11025, SOUND_PHARAORAMM);

	ShakeScreen(4);	

	pHUD->BossHUDActive = false;

	ScrolltoPlayeAfterBoss();
}
