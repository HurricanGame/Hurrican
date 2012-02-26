// --------------------------------------------------------------------------------------
// Der fliegende ReitFettsack
//
// Fliegt rum bis der Hurri ihn abknallt, dann stürzt er ab und bleibt liegen, bis der
// Hurri auf ihn draufhopst. Dann geht das Fluglevel los und der Fettsack trägt
// den Hurri dabei Huckepack ;)
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_ReitFlugsack.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerReitFlugsack::GegnerReitFlugsack(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	Energy			= 50;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= true;
	xSpeed			= 0.0f;
	ySpeed			= 0.0f;
	xAcc			= 2.0f;
	yAcc			= 2.0f;
	AnimSpeed       = 1.5f;
	AnimEnde		= 10;
	ShotCount		= 1.0f;
	SmokeCount		= 0.2f;
	OwnDraw			= true;	
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerReitFlugsack::DoDraw(void)
{
	bool mirror = BlickRichtung != LINKS;

	pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos - pTileEngine->XOffset),
										   (float)(yPos - pTileEngine->YOffset), AnimPhase, 0xFFFFFFFF, mirror);

	if (Handlung == GEGNER_SPECIAL)
	{
		ArrowCount -= 0.2f SYNC;
		if (ArrowCount < 0.0f)
			ArrowCount = 2.0f;

		if ((int)(ArrowCount) % 2 == 0)
			pHUD->Arrow.RenderMirroredSprite((float)(xPos - pTileEngine->XOffset) + 35,
											 (float)(yPos - pTileEngine->YOffset) - 40,
											  0xFF00FF00,
											  false, true);
	}
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerReitFlugsack::DoKI(void)
{
	SimpleAnimation();

// Je nach Handlung richtig verhalten
	switch (Handlung)
	{
		case GEGNER_LAUFEN:						// Normal fliegen und dabei ab und zu schiessen
		{
			if (pAim->xpos + 45 <= xPos + 40)
				BlickRichtung = LINKS;
			else
				BlickRichtung = RECHTS;

			// umherfliegen
			if (ySpeed >  6.0f) yAcc = -2.0f;
			if (ySpeed < -6.0f) yAcc =  2.0f;
			if (xSpeed >  5.0f) xAcc = -2.0f;
			if (xSpeed < -5.0f) xAcc =  2.0f;

			// Spieler verfolgen
			if (pAim->ypos + 45 < yPos + 45 && !(blocko & BLOCKWERT_WAND)) yPos -= 4.0f SYNC;
			if (pAim->ypos + 45 > yPos + 45 && !(blocku & BLOCKWERT_WAND)) yPos += 4.0f SYNC;

			if (blocko & BLOCKWERT_WAND || blocko & BLOCKWERT_GEGNERWAND)	ySpeed =  4.0f;
			if (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_GEGNERWAND)	ySpeed = -4.0f;
			if (blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND)	xSpeed =  4.0f;
			if (blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND)	xSpeed = -4.0f;

			// Rauch ausgeben
			SmokeCount -= 1.0f SYNC;
			if (SmokeCount < 0.0f)
			{
				SmokeCount += 0.1f;

				if (BlickRichtung == LINKS)
					pPartikelSystem->PushPartikel(xPos + 66, yPos+50, FLUGSACKSMOKE2);
				else
					pPartikelSystem->PushPartikel(xPos, yPos+50, FLUGSACKSMOKE);
			}
		} break;

		// Sack stürzt ab
		case GEGNER_FALLEN:
		{
			// Am Boden abhopfen ?
			if (ySpeed > 0.0f &&
				(blocku & BLOCKWERT_WAND ||
				 blocku & BLOCKWERT_PLATTFORM))
			{
				ySpeed *= -0.3f;

				// nicht mehr abhopfen ?
				if (ySpeed > -1.0f)
				{
					ySpeed = 0.0f;
					yAcc   = 0.0f;
					Handlung = GEGNER_SPECIAL;		// Warten dass der Hurri draufhopst
					ArrowCount = 2.0f;
				}
			}

			// Grenze der Fallgeschwindigkeit
			if (ySpeed > 35.0f)
				ySpeed = 35.0f;

			// FlugSack rauchen lassen
			if (AnimCount == 0.0f)
			{
				pPartikelSystem->PushPartikel(xPos+20+rand()%40, yPos+20+rand()%30, SMOKE);
			}

			if (AnimCount == 0.0f && AnimPhase%2 == 0 && rand()%2 == 0)
			{
				pPartikelSystem->PushPartikel(xPos+rand()%80-30, yPos+rand()%70-30, EXPLOSION_MEDIUM2);
				pSoundManager->PlayWave(100, 128, 11025 + rand()%2000, SOUND_EXPLOSION1);
			}

		} break;

		case GEGNER_SPECIAL:
		{	
			// FlugSack rauchen lassen
			if (AnimCount == 0.0f)
			{
				pPartikelSystem->PushPartikel(xPos+20+rand()%40, yPos+20+rand()%30, SMOKE);
			}

			// Testen, ob der Spieler den Sack berührt hat
			PlattformTest(GegnerRect[GegnerArt]);

			// Spieler steht drauf ?
			for (int i = 0; i < NUMPLAYERS; i++)
			if (pPlayer[i]->AufPlattform == this &&
				pPlayer[i]->Handlung != SACKREITEN &&
				pPlayer[i]->Handlung != DREHEN)
			{
				pPlayer[i]->Handlung = SACKREITEN;
				pPlayer[i]->CollideRect.left   = 15;		// Kollisionsrechteck anpassen
				pPlayer[i]->CollideRect.right  = 75;
				pPlayer[i]->CollideRect.top    = 10;
				pPlayer[i]->CollideRect.bottom = 80;
				pPlayer[i]->SmokeCount = 1.0f;
				Energy = 0.0f;

				FlugsackFliesFree = (Value1 != 1);

				// Flugsack Musik spielen
				if (Value1 == 1 &&
					pPlayer[0]->Riding())
				{
					pSoundManager->StopSong(MUSIC_STAGEMUSIC, true);
					pSoundManager->StopSong(MUSIC_BOSS, false);
					pSoundManager->StopSong(MUSIC_PUNISHER, false);
					pSoundManager->PlaySong(MUSIC_FLUGSACK, false);
				}
			}
			
		} break;

		default : break;
	} // switch

	// Soviel Energie verloren, dass der Spacko abstürzt ?
	if (Energy <= 0.0f && Handlung != GEGNER_FALLEN && Handlung != GEGNER_SPECIAL)
	{
		Destroyable	= false;
		Handlung    = GEGNER_FALLEN;
		Energy	    = 40.0f;
		xSpeed	    = 0.0f;
		ySpeed      = 3.0f;
		xAcc        = 0.0f;
		yAcc	    = 2.0f;
		AnimSpeed   = 0.5f;
	}
}

// --------------------------------------------------------------------------------------
// Flugsack explodiert (ohne viel tamm tamm *g*)
// --------------------------------------------------------------------------------------

void GegnerReitFlugsack::GegnerExplode(void)
{
}
