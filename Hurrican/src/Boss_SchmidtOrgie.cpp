// --------------------------------------------------------------------------------------
// T.R.Schmidts Gegnerorgie im Cave Level
//
// Gegnermassen hopsen von der Seite rein
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_SchmidtOrgie.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchmidtOrgie::GegnerSchmidtOrgie(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_VERFOLGEN;
	BlickRichtung	= LINKS;
	Energy			= 4500;
	ChangeLight		= Light;
	Destroyable		= false;
	Value1			= Wert1;
	Value2			= Wert2;
	ActionCounter	= 1.0f;
	DontMove		= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSchmidtOrgie::DoKI(void)
{
	// Linseneffekt ausfaden
	if (Handlung != GEGNER_EXPLODIEREN)
	{
		if (ShadowAlpha > 0.0f)
			ShadowAlpha -= 5.0f SYNC;
		else
			ShadowAlpha = 0.0f;
	}	
	else
	{
		if (ShadowAlpha < 255.0f)
			ShadowAlpha += 5.0f SYNC;
		else
			ShadowAlpha = 255.0f;
	}

	// Energie anzeigen
	if (Handlung != GEGNER_INIT		 && 
		Handlung != GEGNER_VERFOLGEN &&
		Handlung != GEGNER_EXPLODIEREN)
		pHUD->ShowBossHUD(4500, Energy);

	// Boss aktivieren und Mucke laufen lassen
	//
	if (Active == true				 &&
		Handlung != GEGNER_VERFOLGEN &&
		pTileEngine->Zustand == ZUSTAND_SCROLLBAR)
	{
		pTileEngine->ScrollLevel((float)Value1, 
								 (float)Value2, ZUSTAND_SCROLLTOLOCK);		// Level auf den Boss zentrieren

		pSoundManager->FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
		pGegner->PushGegner (xPos, yPos - 40, EXTRAS, 10, 0, false);
	}

	// Hat der Boss keine Energie mehr ? Dann explodiert er
	if (Energy <= 100.0f && Handlung != GEGNER_EXPLODIEREN)
	{
		Handlung   = GEGNER_EXPLODIEREN;

		// Endboss-Musik ausfaden und abschalten
		pSoundManager->FadeSong(MUSIC_BOSS, -2.0f, 0, false);
	}

// Je nach Handlung richtig verhalten
	switch (Handlung)
	{
		case GEGNER_INIT:			// Warten bis der Screen zentriert wurde
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
					Handlung = GEGNER_STEHEN;
				}	
			}
		} break;

		case GEGNER_STEHEN:			
		{
			// ActionCounter runterzählen
			// bei null Gegner spawnen
			ActionCounter -= 1.5f SYNC;

			float x;
			if (rand ()%2 == 0)
				x = -40.0f;
			else
				x = 620.0f;

			if (ActionCounter <= 0.0f)
			{
				ActionCounter = 1.2f + Energy / 600.0f;

				// Gegner per Zufall wählen und spawnen
				//				
				int spawn = rand()%2;
	
				if (spawn == 0) 
					pGegner->PushGegner (float (Value1) + x, float (Value2) + 50 + rand ()%200, FLEDERMAUS, 0, 1, false);

				if (spawn == 1) 
					pGegner->PushGegner (float (Value1) + x, float (Value2) + 200 + rand ()%50, SCHLEIMBOLLER, 24, 0, false);

				Energy -= 10;

				// ab und zu ein Paar Augen einstreuen
				//
				if (int (Energy) % 1000 == 0)
				{
					pGegner->PushGegner (float (Value1) - 20,  float (Value2) + 130, AUGE, rand()%10, 0, false);
					pGegner->PushGegner (float (Value1) + 620, float (Value2) + 200, AUGE, rand()%10, 0, false);
				}

				// oder eine Stachelkugel, einen Firewalker oder ein großer Schleimboller;)
				//
				if (int (Energy) % 500 == 0)
				{
					int j = rand()%3;

					switch(j)
					{
						case 0:
							pGegner->PushGegner (float (Value1) + x,  float (Value2) + 100, STACHELBEERE, 0, 0, false);
						break;

						case 1:
							pGegner->PushGegner (float (Value1) + x, float (Value2) + 100, SCHLEIMBOLLER, 46, 0, false);
						break;

						case 2:
							pGegner->PushGegner (float (Value1) + x, float (Value2) + 250, FIESERWALKER2, 0, 0, false);
						break;
					}									
				}
			}
		} break;

		// warten, bis der Spieler den Gegner berührt hat, und dann aktivieren
		//
		case GEGNER_VERFOLGEN:	
		{
			if (PlayerAbstand() < 600 &&
				pAim->xpos + 80 > xPos)
			
				Handlung = GEGNER_INIT;
	
		} break;
		
		case GEGNER_EXPLODIEREN:
		{			
				Energy = 0.0f;
		} break;

		default : break;
	} // switch

}

// --------------------------------------------------------------------------------------
// SchmidtOrgie explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerSchmidtOrgie::GegnerExplode(void)
{
	// Extra Leben
	pGegner->PushGegner (xPos, yPos - 250, ONEUP, 10, 0, false);
		
	ScrolltoPlayeAfterBoss();
}
