// --------------------------------------------------------------------------------------
// Der Schädel vom Skeletor
//
// Der Counter, wann sich der Schädel wieder auf die Maschine setzt, darf erst abgezogen werden,
// wenn der Schädel in der Nähe der Maschine ist. Er kann ja auch vom Skeletor gespawnt werden.
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_Skull.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSkull::GegnerSkull(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_INIT;
	Value1			= Wert1;
	Value2			= Wert2;
	Energy			= 100000;
	ChangeLight		= Light;
	Destroyable		= true;
	TestBlock		= false;
	SmokeDelay		= 0.0f;
	ShotDelay		= 10.0f;
	ShotCount		= 0;
	Disappear	    = 20.0f;
	pMachine		= NULL;	

	AnimCount = 0.0f;
	if (Value2 == 99)
		AnimCount = 20.0f;

	Handlung = GEGNER_SPECIAL;

	// Position der Endboss Wand rausfinden
	GegnerClass *pTemp;
	pTemp = pGegner->pStart;
	while (pTemp != NULL)
	{
		if (pTemp->GegnerArt == THEWALL)
		{
			mxPos = pTemp->xPos + 5;
			myPos = pTemp->yPos + 229;
			pMachine = pTemp;

			pTemp = NULL;
		}
		else
			pTemp = pTemp->pNext;
	}
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSkull::DoKI(void)
{	
	Energy = 100000;
	BlickRichtung = LINKS;

	// Animphase abhängig von Position zum Spieler setzen
	//	
	float dx = (float)(pAim->xpos + 35) - (xPos + 16);
	AnimPhase = (int)(dx / 30);
	if (AnimPhase < -5) AnimPhase =  -5;
	if (AnimPhase > 9)	AnimPhase = 9;
	AnimPhase += 5;	
	if (xSpeed < 0.0f)
		AnimPhase = 20 - AnimPhase;
	if (AnimPhase < 0)  AnimPhase =  0;
	if (AnimPhase > 19)	AnimPhase = 19;

	// Antrieb
	//	
	SmokeDelay -= 1.0f SYNC;
	if (SmokeDelay <= 0.0f)
	{
		SmokeDelay = 5.0f;

//		pPartikelSystem->PushPartikel(xPos, yPos + 20, SMOKE);

		if (rand()%20 == 0)
			for (int i = 0; i < 10; i++)
			pPartikelSystem->PushPartikel(xPos + 10, yPos + 40, FUNKE);
	}

	// Je nach Handlung richtig verhalten
	//
	switch (Handlung)
	{
		// In die Mitte fliegen und explodieren
		case GEGNER_SPECIAL3:
		{			
			Disappear -= 1.0f SYNC;

			float endwert;
			endwert = 10.0f - (Disappear / 20.0f * 10.0f);

			if (endwert < 1.0f)
				endwert = 1.0f;

			pTimer->SetMoveSpeed(endwert);			

			// Explosionen
			if (AnimCount > 0.0f)
				AnimCount -= 1.0f SYNC;
			else
			{
				AnimCount = 0.8f;

				if (pSoundManager->its_Sounds[SOUND_EXPLOSION2]->isPlaying)
					pSoundManager->StopWave(SOUND_EXPLOSION2);

				pSoundManager->PlayWave(75, 128, 8000 + (int)(endwert * 800), SOUND_EXPLOSION2);

				pPartikelSystem->PushPartikel(xPos + rand()%30 - 30, yPos + rand()%30 - 30, EXPLOSION_MEDIUM2);

				for (int i = 0; i < 3; i++)
					pPartikelSystem->PushPartikel(xPos + rand()%30 - 10, yPos + rand()%20 - 10, SPIDERSPLITTER);
			}

			if (xPos + 16 > pTileEngine->XOffset + 320.0f) 
				xAcc = -4.0f;
			else
				xAcc = 4.0f;

			if (yPos + 20 > pTileEngine->YOffset + 200.0f)
				yAcc = -4.0f;
			else
				yAcc = 4.0f;

			if (xSpeed >  25.0f) xSpeed =  25.0f;
			if (xSpeed < -25.0f) xSpeed = -25.0f;
			if (ySpeed >  15.0f) ySpeed =  15.0f;
			if (ySpeed < -15.0f) ySpeed = -15.0f;			

			if (Disappear < 0.0f)
			{
				// explodieren lassen
				pSoundManager->PlayWave(100, 128, 11025, SOUND_EXPLOSION2);
				pSoundManager->PlayWave(100, 128, 9000, SOUND_EXPLOSION3);
				pSoundManager->PlayWave(100, 128, 7000, SOUND_EXPLOSION1);
				ShakeScreen(10.0f);
				Energy = 0.0f;
				pSoundManager->PlayWave(100, 128, 7000, SOUND_MUTANT);

				pPartikelSystem->PushPartikel (xPos + 20, yPos + 20, SHOCKEXPLOSION);
				pPartikelSystem->PushPartikel (xPos + 20, yPos + 20, SHOCKEXPLOSION);
				pPartikelSystem->PushPartikel (xPos + 20, yPos + 20, SHOCKEXPLOSION);
				
				int i;
				for (i = 0; i < 20; i++)
					pPartikelSystem->PushPartikel(xPos + rand()%30,
												  yPos + rand()%30,
												  SPIDERSPLITTER);

				for (i = 0; i < 20; i++)
					pPartikelSystem->PushPartikel(xPos - 50 + rand()%100, 
												  yPos - 50 + rand()%100, EXPLOSION_BIG);

				for (i = 0; i < 20; i++)
					pPartikelSystem->PushPartikel(xPos - 50 + rand()%100, 
												  yPos - 50 + rand()%100, SPLITTER);

				for (int i = 0; i < 20; i++)
					pPartikelSystem->PushPartikel(xPos - 50 + rand()%100, 
												  yPos - 50 + rand()%100, SCHROTT1);

				pPartikelSystem->PushPartikel(xPos-80, 
											  yPos-80, EXPLOSION_GIGA);

				pTimer->SetMoveSpeed(10.0f);

				// Flugsäcke spawnen
				pGegner->PushGegner((float)(pTileEngine->XOffset + 200.0f),
									(float)(pTileEngine->YOffset - 40.0f),
									REITFLUGSACK, 1, 0, false);

				if (NUMPLAYERS ==2 )
					pGegner->PushGegner((float)(pTileEngine->XOffset + 300.0f),
										(float)(pTileEngine->YOffset - 50.0f),
										REITFLUGSACK, 1, 0, false);				
			}
		} break;

		// Der Schädel fliegt um den Spieler herum. Darf nicht aus dem Screen gedrängt werden
		// So lange, bis der Counter abgelaufen ist
		// dann setzt er sich wieder auf die Endboss Maschine
		case GEGNER_SPECIAL:			
		{			
			if (AnimCount > 0.0f)
				AnimCount -= 1.0f SYNC;
			else
			{
				if (xPos + 16 > pAim->xpos + 35) 
					xAcc = -2.0f;
				else
					xAcc = 2.0f;

				if (yPos + 20 > pAim->ypos + 10)
					yAcc = -2.0f;
				else
					yAcc = 2.0f;

				if (xSpeed >  25.0f) xSpeed =  25.0f;
				if (xSpeed < -25.0f) xSpeed = -25.0f;
				if (ySpeed >  15.0f) ySpeed =  15.0f;
				if (ySpeed < -15.0f) ySpeed = -15.0f;

				if ((xSpeed < 0.0f && xPos < pTileEngine->XOffset) ||
					(xSpeed > 0.0f && xPos > pTileEngine->XOffset + 640.0f))
					xSpeed *= -1.0f;

				if ((ySpeed < 0.0f && yPos < pTileEngine->YOffset) ||
					(ySpeed > 0.0f && yPos > pTileEngine->YOffset + 640.0f))
					ySpeed *= -1.0f;
			}
		} break;

		// Auf die Maschine zufliegen?
		case GEGNER_SPECIAL2:
		{					
			xAcc = 0.0f;
			yAcc = 0.0f;								

			if (xSpeed < 0.0f)	xSpeed += 1.0f SYNC;
			if (xSpeed > 0.0f)	xSpeed -= 1.0f SYNC;
			if (ySpeed < 0.0f)	ySpeed += 1.0f SYNC;
			if (ySpeed > 0.0f)	ySpeed -= 1.0f SYNC;

			if (xPos < mxPos)
			{
				xPos += 10.0f SYNC;

				if (xPos > mxPos)
					xPos = mxPos;
			}

			if (xPos > mxPos)
			{
				xPos -= 10.0f SYNC;

				if (xPos < mxPos)
					xPos = mxPos;
			}

			if (yPos < myPos)
			{
				yPos += 10.0f SYNC;

				if (yPos > myPos)
					yPos = myPos;
			}

			if (yPos > myPos)
			{
				yPos -= 10.0f SYNC;

				if (yPos < myPos)
					yPos = myPos;
			}

			// angekommen?
			if (xPos == mxPos &&
				yPos == myPos)
			{
				Energy = 0.0f;
				pMachine->Value1 = 1;

				pSoundManager->PlayWave(50, 128, 15000, SOUND_KLONG);				

				if (FMUSIC_IsPlaying(pSoundManager->its_Songs[MUSIC_BOSS]->SongData) == false)
					pSoundManager->PlaySong(MUSIC_BOSS, false);
			}
		} break;

		default : break;
	} // switch

	// Auf den Spieler schiessen?
	if (Value1 == 99)
	{
		if (pMachine->Energy < 1500.0f)
			ShotDelay -= 1.0f SYNC;

		if (ShotDelay < 0.0f)
		{
			ShotDelay = 15.0f;
			pProjectiles->PushProjectile(xPos - 10, yPos, SUCHSCHUSS2, pAim);
			pSoundManager->PlayWave(50, 128, 14000 + rand()%2000, SOUND_GOLEMSHOT);
		}		
	}

	if (Handlung != GEGNER_SPECIAL3)
		TestDamagePlayers(1.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Schädel explodiert
// --------------------------------------------------------------------------------------

void GegnerSkull::GegnerExplode(void)
{	
}
