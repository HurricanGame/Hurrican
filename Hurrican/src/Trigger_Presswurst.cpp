// --------------------------------------------------------------------------------------
// Die Presswurst
//
// Kommt runter, zermatscht den Hurrican und pflanzt Spinnen
// und wartet dann solange wie es Value2 angibt
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_Presswurst.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPresswurst::GegnerPresswurst(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_INIT;
	BlickRichtung	= LINKS;
	Energy			= 100;
	Value1			= Wert1;
	Value2			= Wert2;
	AnimPhase		= 0;
	ChangeLight		= Light;
	Destroyable		= false;
	Active			= true;
	OwnDraw			= true;
	TestBlock		= false;
	oldy			= 0.0f;
	SmokeCount		= 0.0f;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerPresswurst::DoDraw(void)
{
	// rendern
	//
	int size = (int)(oldy - yPos) + GegnerRect[GegnerArt].top;
	pGegnerGrafix[GegnerArt]->SetRect(0, size, 185, 357);
	pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset), 
										   (float)(yPos-pTileEngine->YOffset) + size, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// Bewegungs KI
// --------------------------------------------------------------------------------------

void GegnerPresswurst::DoKI(void)
{
	// Spieler kann von unten nicht durchspringen
	//
	for (int p = 0; p < NUMPLAYERS; p++)
	if (SpriteCollision(pPlayer[p]->xpos, 
						pPlayer[p]->ypos, 
						pPlayer[p]->CollideRect,
						xPos, yPos, GegnerRect[GegnerArt]) == true &&
						yPos < pPlayer[p]->ypos					   && 
						pPlayer[p]->yspeed < 0.0f)
						pPlayer[p]->yspeed *= -0.25f;

	// Kollisionsrechteck fürs Wegschieben
	GegnerRect[GegnerArt].top = 0;	
	Wegschieben(GegnerRect[GegnerArt], 0.0f);	

	// Kollisionsrechteck für das Unterteil	
	GegnerRect[GegnerArt].top = 251;	

	blocko = pTileEngine->BlockOben		  (xPos, yPos, yPosOld, yPosOld, GegnerRect[GegnerArt]);
	blocku = pTileEngine->BlockUntenNormal(xPos, yPos, yPosOld, yPosOld, GegnerRect[GegnerArt]);	

	// Ja nach Handlung richtig verhalten
	switch (Handlung)
	{
		// Am Anfang einmal initialisieren
		case GEGNER_INIT:
		{			
			yPos -= GegnerRect[GegnerArt].top;
			oldy = yPos;
			Handlung = GEGNER_INIT2;

		} break;

		// Auf Spieler warten
		case GEGNER_INIT2:
		{
			for (int p = 0; p < NUMPLAYERS; p++)
			if (pPlayer[p]->ypos > yPos		&&
				PlayerAbstandHoriz(pPlayer[p]) < 200  &&
				PlayerAbstandVert(pPlayer[p])  < 500)
			{
				Handlung = GEGNER_LAUFEN;
				ySpeed = 10.0f;
				yAcc = 2.5f;

				if (PlayerAbstand(true) < 600)
					pSoundManager->PlayWave3D((int)(xPos + 90), (int)(yPos + 330), 11025, SOUND_PRESSE);
			}
		} break;

		// Pressen
		case GEGNER_LAUFEN :
		{
			// Spieler beim Runterfallen berührt? Dann stirbt er leider ;)
			//
			for (int p = 0; p < NUMPLAYERS; p++)
			if (SpriteCollision(pPlayer[p]->xpos, 
								pPlayer[p]->ypos, 
								pPlayer[p]->CollideRect,
								xPos, yPos, GegnerRect[GegnerArt]) == true &&
								pPlayer[p]->ypos > yPos)
			{
				// wenn er steht, dann gleich zerquetschen
				if (pPlayer[p]->Handlung != SPRINGEN)
					pPlayer[p]->DamagePlayer(500.0f);

				// wenn er springt, dann runterdrücken
				else
				{
					//pPlayer->yspeed = 0.0f;
					//pPlayer->yadd = 0.0f;
					pPlayer[p]->ypos = yPos + GegnerRect[PRESSWURST].bottom;
				}
			}
			

			// Am Boden ? Dann Partikel erzeugen und zum "Produzieren" wechseln
			if (ySpeed > 0.0f &&
			   (blocku & BLOCKWERT_WAND		  ||
				blocku & BLOCKWERT_GEGNERWAND))
			{		
				// Spieler beim Runterfallen berührt? Dann stirbt er leider ;)
				//
				for (int p = 0; p < NUMPLAYERS; p++)
				if (SpriteCollision(pPlayer[p]->xpos, 
									pPlayer[p]->ypos, 
									pPlayer[p]->CollideRect,
									xPos, yPos, GegnerRect[GegnerArt]) == true &&
									pPlayer[p]->ypos > yPos)
									pPlayer[p]->DamagePlayer(500.0f);

				pTileEngine->BlockUnten(xPos, yPos, yPosOld, yPosOld, GegnerRect[GegnerArt]);	

				ySpeed = 0.0f;
				yAcc   = 0.0f;
				SmokeCount = 5.0f;
				
				if (PlayerAbstand() < 600)
					pSoundManager->PlayWave3D((int)(xPos + 90), (int)(yPos + 330), 11025, SOUND_DOORSTOP);

				ShakeScreen(2.0f);

				Handlung = GEGNER_SPECIAL;
			}

			// An der Decke ? Dann wieder in den Wartezustand setzen
			if (ySpeed < 0.0f &&
			   (blocko & BLOCKWERT_WAND		  ||
				blocko & BLOCKWERT_GEGNERWAND))
			{				
				ySpeed = 0.0f;
				yAcc   = 0.0f;				
							
				Handlung = GEGNER_INIT2;
			}

		} break;

		// Presse wartet unten und spuckt dann Dampf
		case GEGNER_SPECIAL:
		{
			SmokeCount -= 1.0f SYNC;

			if (SmokeCount <= 0.0f)
			{
				Handlung  = GEGNER_SPECIAL2;
				AnimCount = 28.0f;
				if (PlayerAbstand() < 600)
					pSoundManager->PlayWave3D((int)(xPos + 90), (int)(yPos + 330), 13000, SOUND_STEAM2);
			}
		} break;

		// Presse dampft
		case GEGNER_SPECIAL2:
		{
			// rauchen lassen
			SmokeCount -= 1.0f SYNC;
			if (SmokeCount <= 0.0f)
			{
				SmokeCount = 0.2f;

				pPartikelSystem->PushPartikel(xPos + 30.0f, yPos + 300.0f, SMOKE3_LU);
				pPartikelSystem->PushPartikel(xPos + 135.0f, yPos + 300.0f, SMOKE3_RU);
			}

			// Spinne spawnen
			AnimCount -= 1.0f SYNC;
			if (AnimCount < 0.0f)
			{
				// Spinne spawnen
				pGegner->PushGegner(xPos + 60.0f, yPos + 357 - GegnerRect[SPIDERBOMB].bottom, SPIDERBOMB, 0, 0, false, true);

				// wieder hochfahren
				Handlung = GEGNER_LAUFEN;
				ySpeed   = -15.0f;
				yAcc     = 0.2f;

				if (PlayerAbstand() < 600)
					pSoundManager->PlayWave3D((int)(xPos + 90), (int)(yPos + 330), 11025, SOUND_PRESSE);
			}
		} break;

	} // switch	
}

// --------------------------------------------------------------------------------------
// Presswurst explodiert (geht ja garnich *g*)
// --------------------------------------------------------------------------------------

void GegnerPresswurst::GegnerExplode(void)
{
}