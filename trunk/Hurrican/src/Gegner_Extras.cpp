// --------------------------------------------------------------------------------------
// Die Extras
//
// Fliegen munter herum und warten darauf eingesammelt zu werden
// Value1 gibt den jeweiligen Typ des Extras an
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Die Extras
// Value1 gibt jeweils an, um welches Extra es sich handelt, wobei
// 0  : Spreadshot
// 1  : Laser
// 2  : Bounceshot
// 3  : Blitzstrahl
// 4  : Schutzschild
// 5  : Energie
// 6  : Rad-Power
// 7  : Powerline
// 8  : Grenade
// 9  : SmartBomb
// 10 : Autofire
// 11 : Fettboller
// 12 : Bonuspunkte
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Extras.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerExtras::GegnerExtras(int Wert1, int Wert2, bool Light)
{
	BlickRichtung = LINKS;	
	Energy	    = 1.0f;
	Value1	    = Wert1;
	Value2	    = Wert2;
	AnimPhase	= Value1;
	ChangeLight = false;

	// Rausfliegen?
	if (Value2 == 0)
	{
		Handlung  = GEGNER_SPRINGEN;
		ySpeed    = float (rand()%40)    / 10.0f - 20.0f;
		yAcc	  =   5.0f;

		if (Value1 == 0) xSpeed =  9.0f; else
		if (Value1 == 1) xSpeed = -9.0f; else
		if (Value1 == 2) xSpeed =  0.0f; else
		{
			xSpeed = float(rand()%80 + 20)/8.0f;
			if (rand()%2 == 0)
				xSpeed *= -1;
		}

		xSpeed     += float (rand()%20-10) / 5.0f;	
	}

	// ansonsten liegenbleiben
	else
		Handlung  = GEGNER_SPECIAL;

	Destroyable = false;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerExtras::DoKI(void)
{	
	BlickRichtung = LINKS;

	switch(Handlung)
	{
		case GEGNER_SPRINGEN :			// Extra fällt noch im Bogen raus
		{
			// an der Decke abprallen
			/*if (blocko & BLOCKWERT_WAND &&
				ySpeed < 0.0f)
				ySpeed = -ySpeed;*/

			// an der Wand abprallen
			if (blockl & BLOCKWERT_WAND ||
				blockr & BLOCKWERT_WAND)
				xSpeed = -xSpeed;

			// Fall beendet oder auf den Boden gekommen
			if (ySpeed >= 25.0f)
			{
				ySpeed   = -1.0f;
				xSpeed   = 0;
				yAcc     = 0;
				Handlung = GEGNER_STEHEN;

				// Im Tutorial fliegen die Granaten schneller nach oben =)
				if (RunningTutorial == true &&
					Value1 == 8)
					ySpeed = -3.0f;
			}

		} break;

		case GEGNER_STEHEN :			// Extra schwebt langsam nach oben
		{
			// An der Decke anhalten
			if (blocko & BLOCKWERT_WAND)
				ySpeed = 0;

			// Speed verlangsamen
			if (xSpeed > 0.0f)
			{
				xSpeed -= 0.5f SYNC;

				if (xSpeed < 0.0f)
					xSpeed = 0.0f;
			}

			if (xSpeed < 0.0f)
			{
				xSpeed += 0.5f SYNC;

				if (xSpeed > 0.0f)
					xSpeed = 0.0f;
			}
		} break;

	} // switch

	// Testen, ob der Spieler das Extra eingesammelt hat
	PlayerClass *pCollector = NULL;

	for (int i = 0; i < NUMPLAYERS; i++)
	if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt],
						pPlayer[i]->xpos, pPlayer[i]->ypos, pPlayer[i]->CollideRect) == true)
						pCollector = pPlayer[i];

	if (pCollector != NULL)// &&
		//Handlung != GEGNER_SPRINGEN)
	{
		// Punkte fürs Extras sammeln gebe
		pCollector->Score += 10;

		// "Einsammel" Effekt anzeigen
		pPartikelSystem->PushPartikel (xPos - 4, yPos - 4, EXTRACOLLECTED);

		// Waffen-Upgrade eingesammelt ?
		if (Value1 < 3)
		{
			// Upgraden überhaupt noch notwendig ?
			if (pCollector->CurrentWeaponLevel[Value1] < 8)
			{
				// Anzahl eingesammelter Powerups erhöhen
				//pCollector->CollectedPowerUps[Value1]++;

				// Maximum erreicht ? dann neuen Waffenlevel setzen
				//if (pCollector->CollectedPowerUps[Value1] >= pCollector->NextWeaponLevel[Value1])
				{					
					//pCollector->CollectedPowerUps [Value1] = 0;
					pCollector->CurrentWeaponLevel[Value1]++;
					//pCollector->CalcWeaponLevels();

					// PowerUp Effekt erzeugen
					//
					pPartikelSystem->ClearPowerUpEffects();

					for (int i = 0; i < 300; i++)
					{		
						int p = rand ()%360;
						int r = rand ()%30+100;

						pPartikelSystem->PushPartikel (float (pCollector->xpos + 40 - 6 + sin ((float)p)*r), 
													   float (pCollector->ypos + 40 - 6 + cos ((float)p)*r), KRINGELR + Value1, pCollector);
					}

					pSoundManager->PlayWave (100, 128, 11025, SOUND_UPGRADE);
					pCollector->BlinkCounter = 9.5f;
					pCollector->BlinkColor = Value1+1;
				}
			}
			
			// Aktuelle Waffe auf zuletzt gesammeltes Extra setzen,
			// wenn die Waffe dadurch stärker wird
			//
			if (pCollector->CurrentWeaponLevel[Value1] > 
				pCollector->CurrentWeaponLevel[pCollector->SelectedWeapon] ||
				RunningTutorial == true)
				pCollector->SelectedWeapon = Value1;
		}

		// Blitz-Upgrade eingesammelt ?
		if (Value1 == 3)
		{
			// Upgraden überhaupt noch notwendig ?
			if (pCollector->CurrentWeaponLevel[Value1] < 16)
			{
				// Anzahl eingesammelter Powerups erhöhen
				//pCollector->CollectedPowerUps[Value1]++;

				// Maximum erreicht ? dann neuen Waffenlevel setzen
				//if (pCollector->CollectedPowerUps[Value1] >= pCollector->NextWeaponLevel[Value1])
				{
					//pCollector->CollectedPowerUps [Value1] = 0;
					pCollector->CurrentWeaponLevel[Value1]++;
					//pCollector->NextWeaponLevel   [Value1] = 3;

					// PowerUp Effekt erzeugen
					//
					pPartikelSystem->ClearPowerUpEffects();

					for (int i = 0; i < 300; i++)
					{		
						int p = rand ()%360;
						int r = rand ()%30+100;

						pPartikelSystem->PushPartikel (float (pCollector->xpos + 40 - 6 + sin ((float)p)*r), 
													   float (pCollector->ypos + 40 - 6 + cos ((float)p)*r), KRINGELHB, pCollector);
					}

					pSoundManager->PlayWave (100, 128, 11025, SOUND_UPGRADE);
					pCollector->BlinkCounter = 9.5f;
					pCollector->BlinkColor = 4;
				}
			}
		}

		// Schutzschild
		if (Value1 == 4)
		{
			// Schutzuschild Spawner erzeugen
			if (pCollector->Shield <= 0.0f)
			{
				pProjectiles->PushProjectile (pCollector->xpos, pCollector->ypos, SHIELDSPAWNER, pCollector);
				pProjectiles->PushProjectile (pCollector->xpos, pCollector->ypos, SHIELDSPAWNER2, pCollector);
			}

			// Schild setzen
			pCollector->Shield = 100.0f;
		}

		// Energie auffrischen
		if (Value1 == 5)
			pCollector->Energy = MAX_ENERGY;

		// Rad-Energie auffrischen
		if (Value1 == 6)
			pCollector->Armour = MAX_ARMOUR;

		// Powerline
		if (Value1 == 7 && pCollector->PowerLines < PLAYER_MAXEXTRAS)
			pCollector->PowerLines++;

		// Granate
		if (Value1 == 8 && pCollector->Grenades < PLAYER_MAXEXTRAS)
			pCollector->Grenades++;

		// SmartBomb
		if (Value1 == 9 && pCollector->SmartBombs < 1)
			pCollector->SmartBombs++;

		// Autofire
		if (Value1 == 10)
			pCollector->AutoFireExtra += 120;

		// Supershot
		if (Value1 == 11)
			pCollector->RiesenShotExtra += 120;

		// Score
		if (Value1 == 12)
			pPlayer[0]->Score += 100;

		Energy = 0.0f;

		pSoundManager->PlayWave(100, 128, 11025, SOUND_VOICE_SPREAD + Value1);	// Sound ausgeben

		switch (Value1)
		{
			case 0 :
			case 1 :
			case 2 :
			case 4 :
				pSoundManager->PlayWave(100, 128, 11025, SOUND_COLLECT);	// Sound ausgeben
			break;

			case 8  :
			case 9  :
			case 10 :
				pSoundManager->PlayWave(100, 128, 11000 + rand ()%1000, SOUND_AMMO);	// Sound ausgeben
			break;

			default :
				pSoundManager->PlayWave(100, 128, 11025, SOUND_COLLECT);	// Sound ausgeben
			break;
		}

		pSoundManager->PlayWave(100, 128, 11025, SOUND_COLLECT);	// Sound ausgeben
	}
}

// --------------------------------------------------------------------------------------
// Extra verschwindet
// --------------------------------------------------------------------------------------

void GegnerExtras::GegnerExplode(void)
{
}