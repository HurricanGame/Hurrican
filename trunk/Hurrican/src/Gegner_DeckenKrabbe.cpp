// --------------------------------------------------------------------------------------
// Die Decken Krabbe
//
// H�ngt an der Decke und f�llt bei Beschuss oder wenn der Spieler drunter ist runter
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_DeckenKrabbe.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerDeckenKrabbe::GegnerDeckenKrabbe(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	Energy			= 80;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= true;
	zRot			= 180.0f;
	OwnDraw			= true;
	AnimSpeed		= 0.75f;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerDeckenKrabbe::DoDraw(void)
{
	bool mirror;

	if (BlickRichtung == LINKS)
		mirror = false;
	else
		mirror = true;
	
	pGegnerGrafix[GegnerArt]->RenderSpriteRotated((float)(xPos-pTileEngine->XOffset),
							  			 		  (float)(yPos-pTileEngine->YOffset), zRot, AnimPhase, 0xFFFFFFFF, mirror);

	// Testen, ob der Spieler die Krabbe ber�hrt hat
	TestDamagePlayers(8.0f SYNC);	
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerDeckenKrabbe::DoKI(void)
{
	if (zRot > 360.0f)	zRot = 0.0f;
	if (zRot < 0.0f)	zRot = 360.0f;	

	// runterfallen?
	//
	if (Handlung != GEGNER_STEHEN)
	{		
		if (!(blocku & BLOCKWERT_SCHRAEGE_R) && 
			!(blocku & BLOCKWERT_SCHRAEGE_L))
		{
			blocku = pTileEngine->BlockUnten (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

			if (!(blocku & BLOCKWERT_WAND) && 
				!(blocku & BLOCKWERT_PLATTFORM))
			{
				if (ySpeed == 0.0f)
					ySpeed = 5.0f;

				yAcc = 3.0f;
			}

			else
			{
				ySpeed = 0.0f;
				yAcc   = 0.0f;
			}
		}		
	}

	if (Handlung == GEGNER_LAUFEN ||
		Handlung == GEGNER_DREHEN ||
		Handlung == GEGNER_DREHEN2)
	{
		// Schr�gen checken
		//
		if (ySpeed >= 0.0f)
			blocku = pTileEngine->BlockSlopes(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt], ySpeed, true);
	}

	switch (Handlung)
	{
		// Krabbe h�ngt an der Decke
		case GEGNER_STEHEN:
		{
			// angeschossen oder Spieler drunter? Dann runterfallen
			//
			if (DamageTaken != 0.0 ||
				(PlayerAbstandHoriz() < 100 &&
				 PlayerAbstandVert()  < 800 &&
				 pAim->ypos > yPos))
			{
				// Rotationsspeed errechnen, je nachdem, wie weit der Boden entfernt ist
				//
				int a = 0;
				int b = pTileEngine->Tiles[(int)(xPos / TILESIZE_X)][(int)(yPos / TILESIZE_Y) + a].Block;

				while (a < 20 && !(b & BLOCKWERT_WAND))
				{
					b = pTileEngine->Tiles[(int)(xPos / TILESIZE_X)][(int)(yPos / TILESIZE_Y) + a].Block;
					a++;
				}

				zSpeed = 50.0f - a * 1.8f;
				ySpeed = 5.0f;
				yAcc   = 5.0f;
				Handlung = GEGNER_FALLEN;
			}
		}
		break;

		// Krabbe ist gelandet und dreht sich in Richtung Spieler
		case GEGNER_DREHEN:
		{
			zRot = 0.0f;
			AnimCount += SpeedFaktor;			// Animationscounter weiterz�hlen
			if (AnimCount > AnimSpeed)			// Grenze �berschritten ?
			{
				AnimCount = 0.0f;				// Dann wieder auf Null setzen
				AnimPhase--;					// Und n�chste Animationsphase
				if (AnimPhase <= 0) 			// Animation von zu Ende	?
				{
					AnimPhase = 0;
					AnimCount = 0.0f;
					Handlung = GEGNER_DREHEN2;
					BlickRichtung *= -1;
				}
			}
		} break;

		// Krabbe ist gelandet und dreht sich in Richtung Spieler
		case GEGNER_DREHEN2:
		{
			zRot = 0.0f;
			AnimCount += SpeedFaktor;			// Animationscounter weiterz�hlen
			if (AnimCount > AnimSpeed)			// Grenze �berschritten ?
			{
				AnimCount = 0.0f;				// Dann wieder auf Null setzen
				AnimPhase++;					// Und n�chste Animationsphase
				if (AnimPhase >= 4) 			// Animation von zu Ende	?
				{
					AnimPhase = 5;
					AnimCount = 0.0f;
					Handlung = GEGNER_LAUFEN;
				}
			}
		} break;

		// Krabbe ist gelandet und Krabbelt auf den Spieler zu
		case GEGNER_LAUFEN:
		{			
			// an Schr�gen drehen?
			//
			if (Handlung != GEGNER_STEHEN &&
				Handlung != GEGNER_FALLEN)
			{
				// schr�ge links hochlaufen
				if (BlickRichtung == RECHTS)
				{
					if (blockl & BLOCKWERT_SCHRAEGE_L)
					{
						if (zRot < 45.0f)
						{
							zRot += 20.0f SYNC;
							if (zRot > 45.0f)				
								zRot = 45.0f;
						}			
					}
					else
					if (zRot > 0.0f)
					{
						zRot -= 20.0f SYNC;
						if (zRot < 0.0f)				
							zRot = 0.0f;
					}
				}

				if (BlickRichtung == LINKS)
				{
					if (blockr & BLOCKWERT_SCHRAEGE_R)
					{
						if (zRot < 45.0f)
						{
							zRot += 20.0f SYNC;
							if (zRot > 45.0f)				
								zRot = 45.0f;
						}			
					}
					else
					if (zRot > 0.0f)
					{
						zRot -= 20.0f SYNC;
						if (zRot < 0.0f)				
							zRot = 0.0f;
					}
				}
			}

			xPos += 7.0f * BlickRichtung * -1 SYNC;

			AnimCount += SpeedFaktor;			// Animationscounter weiterz�hlen
			if (AnimCount > AnimSpeed)			// Grenze �berschritten ?
			{
				AnimCount = 0.0f;				// Dann wieder auf Null setzen
				AnimPhase++;					// Und n�chste Animationsphase
				if (AnimPhase >= 14) 			// Animation von zu Ende	?
					AnimPhase = 5;
			}

			// Rumdrehen ?
			if ((BlickRichtung == LINKS  && xPos + 35 > pAim->xpos + 35) ||
				(BlickRichtung == RECHTS && xPos + 35 < pAim->xpos + 35))
			{
				Handlung  = GEGNER_DREHEN;
				AnimCount = 0.0f;
				AnimPhase = 4;
			}
		} break;

		// Krabbe fliegt von der Decke runter
		case GEGNER_FALLEN:
		{
			if (ySpeed > 30.0f)
			{
				ySpeed = 30.0f;
				yAcc   = 0.0f;
			}

			if (zRot >= 180.0f)
				zRot += zSpeed SYNC;
			else
				zRot = 0.0f;

			// Auf Boden gelandet ?
			if (blocku & BLOCKWERT_WAND)
			{
				Handlung = GEGNER_DREHEN2;
				ySpeed = 0.0f;
				yAcc   = 0.0f;
				AnimPhase = 1;
				AnimEnde  = 4;
				AnimSpeed = 0.75f;
				Destroyable = true;

				if (xPos + 35 < pAim->xpos + 35) BlickRichtung = LINKS;
				if (xPos + 35 > pAim->xpos + 35) BlickRichtung = RECHTS;
			}
		} break;

		default : break;
	} // switch	
}

// --------------------------------------------------------------------------------------
// DeckenKrabbe explodiert
// --------------------------------------------------------------------------------------

void GegnerDeckenKrabbe::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, 128, 9000 + rand()%2000, SOUND_EXPLOSION3);

	pPartikelSystem->PushPartikel (xPos + 15, yPos + 10, LAVAKRABBE_KOPF);

	for (int i = 0; i < 4; i++)	
		pPartikelSystem->PushPartikel (xPos - 10 + rand ()%30, yPos - 10 + rand ()%10, EXPLOSION_MEDIUM2);

	for (int i = 0; i < 4; i++)	
		pPartikelSystem->PushPartikel (xPos - 10 + rand ()%30, yPos - 10 + rand ()%10, LAVAKRABBE_BEIN);
}
