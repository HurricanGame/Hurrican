// --------------------------------------------------------------------------------------
// Der Punisher
//
// Erscheint, wenn die Zeit abgelaufen ist.
// Der Punisher verfolgt den Spieler ständig, dafür recht langsam.
// Er ist nahezu unbesiegbar, und bei Berührung verliert der Spieler ein Leben und alle
// Waffen-Powerups, erhält dafür aber wieder volle Zeit
// Besiegt der Spieler den Punisher, so erhält er dafür ein Extraleben, voll Zeit
// und massenhaft Extras (muss ich mir noch ausdenken, welche)
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Punisher.h"
#include "HUD.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPunisher::GegnerPunisher(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_INIT;
	Energy			= 20000;

	ChangeLight		= Light;
	Destroyable		= false;
	Value1			= Wert1;
	Value2			= Wert2;
	OwnDraw			= true;
	TestBlock		= false;	
	Active			= true;
	alpha			= 0.0f;
	ShotDelay		= 0.0f;

	AnimPhase		= 15;
	AnimCount		= 15.0f;
}	

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerPunisher::DoDraw(void)
{
	// Je nach Handlung entsprechend anders rendern
	//
	switch (Handlung)
	{
		// in Schwabbelstreifen rendern
		case GEGNER_INIT2:
		case GEGNER_SPECIAL:
		{
			for (int i = 0; i < 170; i++)
			{
				pGegnerGrafix[GegnerArt]->SetRect(3 * 170, 2 * 170 + i, 4 * 170, 2 * 170 + i+1);
				pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset + (float)(sin((alpha / 20.0f) + i / 10.0f) * ((255.0f - alpha) / 255.0f * 200.0f))),
													   (float)(yPos-pTileEngine->YOffset + i), 
															   D3DCOLOR_RGBA (255, 255, 255, (int)alpha));
			}
		} break;

		// normal rendern
		case GEGNER_LAUFEN:
		{			
			pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset), 
												   (float)(yPos-pTileEngine->YOffset), 
														   AnimPhase, D3DCOLOR_RGBA (255, 255, 255, 255), false);
		} break;
	}
}

// --------------------------------------------------------------------------------------
// Spieler verfolgen
// --------------------------------------------------------------------------------------

void GegnerPunisher::FollowPlayer(void)
{
	if (xPos + 50 < pAim->xpos + 35) xAcc =  2.0f;
	if (xPos + 50 > pAim->xpos + 35) xAcc = -2.0f;

	if (yPos + 100 < pAim->ypos + 40) yAcc =  2.0f;
	if (yPos + 100 > pAim->ypos + 40) yAcc = -2.0f;

	if (xSpeed >  15.0f) xSpeed =  15.0f;
	if (xSpeed < -15.0f) xSpeed = -15.0f;
	if (ySpeed >  15.0f) ySpeed =  15.0f;
	if (ySpeed < -15.0f) ySpeed = -15.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerPunisher::DoKI(void)
{
	switch (Handlung)
	{
		// initialisieren
		case GEGNER_INIT:
		{	
			// zentrieren
			xPos = (float)(pTileEngine->XOffset + 320 - 100/2.0f);
			yPos = (float)(pTileEngine->YOffset + 240 - 95/2.0f);

			alpha = 0.0f;
			Handlung = GEGNER_INIT2;

			pSoundManager->StopSong(MUSIC_STAGEMUSIC, true);		
			pSoundManager->PlaySong(MUSIC_PUNISHER, false);

		} break;

		//einfaden
		case GEGNER_INIT2:
		{			
			// einfaden
			alpha += 5.0f SYNC;

			if (alpha > 255.0f)
			{
				alpha = 255.0f;
				Handlung = GEGNER_LAUFEN;
				Destroyable	= true;
			}

		} break;

		// Spieler verfolgen
		case GEGNER_LAUFEN:
		{		
			// Spieler verfolgen
			FollowPlayer();

			// Testen, ob der Spieler den Punisher berührt hat
			for (int p = 0; p < NUMPLAYERS; p++)
			if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt],
								pPlayer[p]->xpos, pPlayer[p]->ypos, pPlayer[p]->CollideRect) == true)
			{
				// Spieler stirbt
				pPlayer[p]->Energy = 0.0f;

				// und verliert Waffen
				for (int i = 0; i < 4; i++)
					if (pPlayer[p]->CurrentWeaponLevel[i] > 1)
						pPlayer[p]->CurrentWeaponLevel[i]--;
				
				pPlayer[p]->CalcWeaponLevels();

				// bekommt dafür aber wieder Zeit
				pTileEngine->Timelimit = pTileEngine->TimelimitSave;

				// ausfaden
				Energy = 0.0f;
			}		

			// Animphase setzen
			if (xAcc < 0.0f)
			{
				if (AnimCount > 20.0f)	AnimCount = 20.0f;
				AnimCount -= 1.0f SYNC;
				if (AnimCount < 0.0f)	AnimCount = 10.0f;				
			}
			else
			{
				if (AnimCount < 10.0f)	AnimCount = 10.0f;
				AnimCount += 1.0f SYNC;
				if (AnimCount > 30.0f)	AnimCount = 20.0f;				
			}

			AnimPhase = (int)(AnimCount);
		} break;

		// ausfaden
		case GEGNER_SPECIAL:
		{
			xAcc = 0.0f;
			yAcc = 0.0f;
			xSpeed = 0.0f;
			ySpeed = 0.0f;

			// ausfaden
			alpha -= 5.0f SYNC;

			if (alpha < 0.0f)
			{
				alpha = 0.0f;
				Energy = 0.0f;
			}
			
		} break;

		default : break;
	} // switch		

	// Boss oder SummaryScreen ? Dann verschwinden
	if (pHUD->BossHUDActive != 0.0f ||
		ShowSummary == true)
		Energy = 0.0f;

	if (Energy <= 0.0f && 
		alpha > 0.0f   &&
		Handlung != GEGNER_SPECIAL)
		Vanish();		
}

// --------------------------------------------------------------------------------------
// Punisher verschwindet
// --------------------------------------------------------------------------------------

void GegnerPunisher::Vanish(void)
{
	if (ShowSummary == true)
		pSoundManager->StopSong(MUSIC_PUNISHER, false);		
	else
		pSoundManager->FadeSong(MUSIC_PUNISHER, -2.0f, 0, false);		

	Handlung = GEGNER_SPECIAL;
	Energy = 1.0f;		
	Destroyable	= false;		
}

// --------------------------------------------------------------------------------------
// Punisher explodiert
// --------------------------------------------------------------------------------------

void GegnerPunisher::GegnerExplode(void)
{		
	pTileEngine->Timelimit += 100;

	pPlayer[0]->PunisherActive = false;
	pPlayer[1]->PunisherActive = false;

	pSoundManager->SetSongVolume(MUSIC_STAGEMUSIC, 0);
	pSoundManager->FadeSong(MUSIC_STAGEMUSIC, 2.0f, 100, true);		
}
