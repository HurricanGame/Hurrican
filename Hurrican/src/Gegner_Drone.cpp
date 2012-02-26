// --------------------------------------------------------------------------------------
// Die Drone
//
// Schwirrt um Hurri rum und ballert dabei
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Drone.h"

extern DirectGraphicsSprite	*pDroneFlame;						// Flamme der Drone

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerDrone::GegnerDrone(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	Energy			= 125;
	AnimEnde		= 7;
	AnimSpeed		= 0.75f;
	AnimCount		= 0.0f;
	ChangeLight		= Light;
	Destroyable		= true;

	ShotCount		= 0;
	ShotDelay		= 0.0f;
	ActionDelay		= 5.0f;

	Value1			= Wert1;
	
	// Aus der Spinnenmaschine ausgespuckt?
	if (Wert1 == 99)
		ySpeed = -22.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerDrone::DoKI(void)
{
	// Animieren
	//
	AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
	if (Handlung == GEGNER_LAUFEN)
	{
		if (AnimCount > AnimSpeed)			// Grenze überschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen
			AnimPhase++;					// Und nächste Animationsphase
			if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
				AnimPhase = AnimStart;		// Dann wieder von vorne beginnen
		}
	} 
	else
	if (Handlung == GEGNER_DREHEN)
	{
		if (AnimCount > AnimSpeed)			// Grenze überschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen
			AnimPhase++;					// Und nächste Animationsphase
			if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
			{
				AnimCount = 0.0f;
				Handlung  = GEGNER_DREHEN2;
				BlickRichtung *= -1;
			}
		}
	} 
	else
	if (Handlung == GEGNER_DREHEN2)
	{
		if (AnimCount > AnimSpeed)			// Grenze überschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen
			AnimPhase--;					// Und nächste Animationsphase
			if (AnimPhase <= 6)				// Animation von zu Ende	?
			{
				AnimPhase = AnimStart;		// Dann wieder von vorne beginnen
				AnimEnde  = 7;
				Handlung  = GEGNER_LAUFEN;
				ActionDelay = 1.0f;
			}
		}
	} 

	//in Richtung Spieler fliegen
	//
	if (Handlung != GEGNER_FALLEN)
	{
		if (xPos + 30 < pAim->xpos + 35) xAcc =  1.5f;
		if (xPos + 30 > pAim->xpos + 35) xAcc = -1.5f;

		if (yPos < pAim->ypos - 60) yAcc =  1.5f;
		if (yPos > pAim->ypos - 60) yAcc = -1.5f;

		if (xSpeed >  20.0f) xSpeed =  20.0f;
		if (xSpeed < -20.0f) xSpeed = -20.0f;
		if (ySpeed >  10.0f) ySpeed =  10.0f;
		if (ySpeed < -10.0f &&
			Value1 != 99) 
			ySpeed = -10.0f;

		// An der Wand umdrehen
		//
		if ((blocko & BLOCKWERT_WAND ||blocko & BLOCKWERT_GEGNERWAND) && ySpeed < 0.0f) ySpeed *= 1.0f;
		if ((blocku & BLOCKWERT_WAND ||blocku & BLOCKWERT_GEGNERWAND) && ySpeed > 0.0f) ySpeed *= 1.0f;
		if ((blockl & BLOCKWERT_WAND ||blockl & BLOCKWERT_GEGNERWAND) && xSpeed < 0.0f) xSpeed *= 1.0f;
		if ((blockr & BLOCKWERT_WAND ||blockr & BLOCKWERT_GEGNERWAND) && xSpeed > 0.0f) xSpeed *= 1.0f;
	}

	switch (Handlung)
	{
		// In der Luft rumdümpeln
		//
		case GEGNER_LAUFEN:
			if ((xPos + 30 < pAim->xpos + 35 && BlickRichtung == LINKS) ||
				(xPos + 30 > pAim->xpos + 35 && BlickRichtung == RECHTS))
			{
				Handlung = GEGNER_DREHEN;
				AnimPhase = 6;
				AnimCount = 0.0f;
				AnimEnde  = 11;
			}

			ActionDelay -= 1.0f SYNC;

			if (ActionDelay <= 0.0f && PlayerAbstand() < 600)
			{
				Handlung = GEGNER_SCHIESSEN;

				if (Skill == 0) ShotCount = 4;
				if (Skill == 1) ShotCount = 6;
				if (Skill == 2) ShotCount = 8;
				if (Skill == 3) ShotCount = 10;

				ShotDelay = 20.0f;

				pSoundManager->PlayWave3D (int (xPos), int (yPos), 12000, SOUND_DRONE);

				pPartikelSystem->PushPartikel (xPos + 33, yPos + 60, BULLET, pPlayer[0]);
				
				if (BlickRichtung == RECHTS)
					pProjectiles->PushProjectile  (xPos + 58, yPos + 68, DRONEBULLET);
				else
					pProjectiles->PushProjectile  (xPos  - 10, yPos + 68, DRONEBULLET2);
			}
		break;

		// Auf den Spieler ballern
		//
		case GEGNER_SCHIESSEN:
			if ((xPos + 30 < pAim->xpos + 35 && BlickRichtung == LINKS) ||
				(xPos + 30 > pAim->xpos + 35 && BlickRichtung == RECHTS))
			{
				Handlung = GEGNER_DREHEN;
				AnimPhase = 6;
				AnimCount = 0.0f;
				AnimEnde  = 11;
			}

			ShotDelay -= 12.0f SYNC;

			// Salve zuende und weiterfliegen ?
			//
			if (ShotCount <= 0 && ShotDelay <12.0f)
			{
				ActionDelay = 10.0f;
				Handlung = GEGNER_LAUFEN;
			}

			// Schuss abgeben
			//
			if (ShotDelay <= 0.0f && PlayerAbstand() < 600)
			{
				pSoundManager->PlayWave3D (int (xPos), int (yPos), 12000, SOUND_DRONE);
				ShotDelay = 20.0f;
				ShotCount--;

				pPartikelSystem->PushPartikel (xPos + 36, yPos + 57, BULLET, pPlayer[0]);

				if (BlickRichtung == RECHTS)
					pProjectiles->PushProjectile  (xPos + 58, yPos + 68, DRONEBULLET);
				else
					pProjectiles->PushProjectile  (xPos  - 10, yPos + 68, DRONEBULLET2);
			}

			// Flamme anzeigen
			//
			if (int (ShotDelay) >= 12)
			{
				DirectGraphics.SetAdditiveMode ();

				int anim;
				if (BlickRichtung == -1)
					anim = 0; 
				else
					anim = 1; 

				pDroneFlame->RenderSprite((float)(xPos-pTileEngine->XOffset) + BlickRichtung * 56 - 5, 
										  (float)(yPos-pTileEngine->YOffset) + 60, 
										  anim, 0xFFFFFFFF);
				DirectGraphics.SetColorKeyMode ();
			}
		break;

		// Drone stürzt ab
		//
		case GEGNER_FALLEN:
		{
			// An die Wand gekracht ?
			if (blockl & BLOCKWERT_WAND ||
			    blockr & BLOCKWERT_WAND ||
				blocko & BLOCKWERT_WAND ||
				blocku & BLOCKWERT_WAND ||
				blocku & BLOCKWERT_PLATTFORM)
				Energy = 0.0f;

			// Grenze der Fallgeschwindigkeit
			if (ySpeed > 35.0f)
				yAcc = 0.0f;

			// Drone rauchen lassen
			if (AnimCount >= 0.5f)
			{
				AnimCount = 0.0f;
				pPartikelSystem->PushPartikel(xPos+rand()%30 + 20, yPos+10+rand()%40, SMOKE);
				pPartikelSystem->PushPartikel(xPos+rand()%30 + 20, yPos+10+rand()%40, SMOKE3);
			}
		} break;

	}

	// Drone stürzt ab ?
	//
	if (Energy <= 0.0f && Handlung != GEGNER_FALLEN)
	{
		Handlung = GEGNER_FALLEN;
		AnimCount = 0.0f;
		Energy	 = 50.0f;
		ySpeed   = 1.0f;
		yAcc	 = 2.0f;
		xAcc     = 0.0f;
	}
}

// --------------------------------------------------------------------------------------
// Drone explodiert
// --------------------------------------------------------------------------------------

void GegnerDrone::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION3);

	for (int i = 0; i < 5; i++)
	{
		pPartikelSystem->PushPartikel (xPos - 30 + rand ()%70, yPos - 30 + rand ()%80, EXPLOSION_MEDIUM2);
		pPartikelSystem->PushPartikel (xPos - 30 + rand ()%70, yPos - 30 + rand ()%80, SPIDERSPLITTER);
	}

	for (int i = 0; i < 10; i++)
		pPartikelSystem->PushPartikel (xPos - 30 + rand ()%70, yPos - 30 + rand ()%80, SPLITTER);

	pPartikelSystem->PushPartikel (xPos-24, yPos-16, EXPLOSION_GIANT);

	pPlayer[0]->Score += 300;
}
