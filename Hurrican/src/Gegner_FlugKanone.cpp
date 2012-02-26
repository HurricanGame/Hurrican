// --------------------------------------------------------------------------------------
// Die FlugKanone
//
// Umkreist den Spieler und Ballert auf ihn runter
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_FlugKanone.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFlugKanone::GegnerFlugKanone(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	Energy			= 30;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= true;
	xSpeed			= 0.0f;
	ySpeed			= 0.0f;
	yAcc			= 0.0f;
	AnimSpeed       = 1.0f;
	AnimEnde		= 19;
	ShotDelay		= 0.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFlugKanone::DoKI(void)
{
	// Animieren
	if (AnimEnde > 0)						// Soll �berhaupt anmiert werden ?
	{
		AnimCount += SpeedFaktor;			// Animationscounter weiterz�hlen
		if (AnimCount > AnimSpeed)			// Grenze �berschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen
			AnimPhase++;					// Und n�chste Animationsphase
			if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
				AnimPhase = AnimStart;		// Dann wieder von vorne beginnen
		}
	} // animieren

	//in Richtung Spieler fliegen
	if (xPos + 30 < pAim->xpos + 35) xAcc =  2.5f;
	if (xPos + 30 > pAim->xpos + 35) xAcc = -2.5f;

	if (yPos < pAim->ypos - 100) yAcc =  2.5f;
	if (yPos > pAim->ypos - 100) yAcc = -2.5f;

	if (xSpeed >  20.0f) xSpeed =  20.0f;
	if (xSpeed < -20.0f) xSpeed = -20.0f;
	if (ySpeed >  10.0f) ySpeed =  10.0f;
	if (ySpeed < -10.0f) ySpeed = -10.0f;

	if (ShotDelay > 0.0f) ShotDelay -= 1.0f SYNC;
	if (ShotDelay <= 0.0f)
	{
		if (Skill == 0) ShotDelay = 10.0f;
		if (Skill == 1) ShotDelay = 7.5f;
		if (Skill >= 2) ShotDelay = 5.0f;

		if (abs(int(pAim->xpos - xPos)) < 100 &&	PlayerAbstand() < 300)
		{
			pProjectiles->PushProjectile(xPos + 30, yPos + 40, FLUGLASER);
			pPartikelSystem->PushPartikel(xPos + 24, yPos + 30, SMOKE);
			pSoundManager->PlayWave(50, 128, 30000, SOUND_LASERSHOT);
		}
	}

	if ((blocko & BLOCKWERT_WAND ||blocko & BLOCKWERT_GEGNERWAND) && ySpeed < 0.0f) ySpeed *= 1.0f;
	if ((blocku & BLOCKWERT_WAND ||blocku & BLOCKWERT_GEGNERWAND) && ySpeed > 0.0f) ySpeed *= 1.0f;
	if ((blockl & BLOCKWERT_WAND ||blockl & BLOCKWERT_GEGNERWAND) && xSpeed < 0.0f) xSpeed *= 1.0f;
	if ((blockr & BLOCKWERT_WAND ||blockr & BLOCKWERT_GEGNERWAND) && xSpeed > 0.0f) xSpeed *= 1.0f;

// Je nach Handlung richtig verhalten
	switch (Handlung)
	{
		case GEGNER_LAUFEN:					// Durch die Gegend rasen
		{
		} break;

		default : break;
	} // switch

	// Testen, ob der Spieler die FlugKanone ber�hrt hat
	TestDamagePlayers(1.0f SYNC);
}

// --------------------------------------------------------------------------------------
// FlugKanone explodiert
// --------------------------------------------------------------------------------------

void GegnerFlugKanone::GegnerExplode(void)
{
	for (int i=0; i<5; i++)
		pPartikelSystem->PushPartikel(float(xPos - 20 + rand()%45), 
									  float(yPos - 20 + rand()%45), EXPLOSION_MEDIUM2);
	for (int i=0; i<20; i++)
		pPartikelSystem->PushPartikel(float(xPos + rand()%65), 
									  float(yPos + rand()%35), LASERFUNKE2);

	pSoundManager->PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION1);	// Sound ausgeben

	pPlayer[0]->Score += 150;
}
