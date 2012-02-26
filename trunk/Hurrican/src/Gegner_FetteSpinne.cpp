// --------------------------------------------------------------------------------------
// Die fette Spinne
//
// Krabbelt auf den Hurri zu und ballert Laser
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_FetteSpinne.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFetteSpinne::GegnerFetteSpinne(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	Energy			= 180;
	Value1			= Wert1;
	Value2			= Wert2;
	AnimStart		= 0;
	AnimEnde		= 9;
	AnimSpeed		= 0.5f;
	ChangeLight		= Light;
	Destroyable		= true;
	ShotDelay		= 10.0f;
	WalkState		= 0;
	OwnDraw			= true;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerFetteSpinne::DoDraw(void)
{
	bool h, v;

	// normal
	if (WalkState == 0) 
		v = false;

	// oder auf dem Kopf?
	else
		v = true;

	if (BlickRichtung == RECHTS)
		h = true;
	else
		h = false;

	pGegnerGrafix[GegnerArt]->itsRect = pGegnerGrafix[GegnerArt]->itsPreCalcedRects[AnimPhase];
	pGegnerGrafix[GegnerArt]->RenderMirroredSprite ((float)(xPos-pTileEngine->XOffset), 
												    (float)(yPos-pTileEngine->YOffset), 
													0xFFFFFFFF, h, v);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFetteSpinne::DoKI(void)
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

	// Umdrehen wenn der Spieler von hinten drauf schiesst
	//
	if (DamageTaken > 0.0f)
	{
		if (pAim->xpos + 40 < xPos + 70)
			BlickRichtung = -1;
		else
			BlickRichtung = 1;
	}

	// Je nach Handlung richtig verhalten
	//
	switch (Handlung)
	{
		case GEGNER_LAUFEN :
		{
			// Speed setzen
			xAcc = 5.0f * BlickRichtung;

			if (xSpeed >  12.0f) xSpeed =  12.0f;
			if (xSpeed < -12.0f) xSpeed = -12.0f;

			// An Mauern umdrehen
			//
			if ((xSpeed < 0.0f && (blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND)) ||
				(xSpeed > 0.0f && (blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND)))
				 BlickRichtung *= -1;

			// Am Boden? Dann checken : Spieler schiesst auf die Spinne?
			if (WalkState == 0) 		
			if (((pAim->xpos + 35 < xPos + 50 &&
				  pAim->Blickrichtung == RECHTS &&
				  BlickRichtung == LINKS) ||

				(pAim->xpos + 35 >= xPos + 50 &&
				 pAim->Blickrichtung == LINKS &&
				 BlickRichtung == RECHTS)) &&
				 pAim->Aktion[AKTION_SHOOT])
			{
				// Decke �ber der Spinne suchen
				bool block = false;
				int a = 0;
				int b = pTileEngine->Tiles[(int)(xPos / TILESIZE_X)][(int)(yPos / TILESIZE_Y) + a].Block;

				while (a < 10 && block == false)
				{
					b = pTileEngine->Tiles[(int)(xPos / TILESIZE_X)][(int)(yPos / TILESIZE_Y) + a].Block;

					// Decke gefunden?
					if (b & BLOCKWERT_WAND)
						block = true;

					a++;
				}

				// Decke dr�ber? Dann hochspringen
				if (block == true)
				{
					Handlung = GEGNER_SPECIAL;
					AnimPhase = 20;
					AnimEnde  = 0;
					AnimSpeed = 0.0f;
					AnimCount = 2.0f;
					xSpeed	  = 0.0f;
					xAcc	  = 0.0f;
				}
			}

			// An der Decke? Dann auf den Spieler fallen lassen
			if (WalkState == 1)
			{
				if (yPos < pAim->ypos &&
					PlayerAbstandHoriz() < 20)
				{
					Handlung = GEGNER_SPECIAL;
					AnimPhase = 20;
					AnimEnde  = 0;
					AnimSpeed = 0.0f;
					AnimCount = 0.0f;
					xSpeed	  = 0.0f;
					xAcc	  = 0.0f;
				}
			}

			// Zu weit vom Spieler weg? Dann umdrehen
			if (xPos + 50 < pAim->xpos + 35 - 200.0f) BlickRichtung = RECHTS;
			if (xPos + 50 > pAim->xpos + 35 + 200.0f) BlickRichtung = LINKS;

		} break;

		// kurz in die Hocke gehen
		case GEGNER_SPECIAL :
		{
			AnimCount -= 1.0f SYNC;

			// abspringen
			if (AnimCount < 0.0f)
			{
				Handlung  = GEGNER_SPRINGEN;
				AnimPhase = 10;
				AnimEnde  = 19;
				AnimSpeed = 0.5f;
				AnimStart = 19;

				if (WalkState == 0)
					ySpeed = -25.0f;
				else
				{
					ySpeed = 20.0f;
					yAcc   = 2.0f;
				}
			}
		} break;

		// im Flug drehen
		case GEGNER_SPRINGEN :
		{	
			// An der Decke oder am Boden angekommen?
			if ((ySpeed < 0.0f && blocko & BLOCKWERT_WAND) ||
				(ySpeed > 0.0f && blocku & BLOCKWERT_WAND) ||
				(ySpeed > 0.0f && blocku & BLOCKWERT_PLATTFORM))				
			{
				AnimPhase = 0;
				AnimStart = 0;
				AnimEnde  = 9;
				AnimSpeed = 0.5f;

				ySpeed = 0.0f;
				yAcc = 0.0f;

				Handlung = GEGNER_LAUFEN;

				// umdrehen
				if (WalkState == 0)
					WalkState = 1; 
				else
					WalkState = 0;
			}

		} break;


		default :
		break;

	} // switch	
}


// --------------------------------------------------------------------------------------
// Fette Spinne explodiert
// --------------------------------------------------------------------------------------

void GegnerFetteSpinne::GegnerExplode(void)
{
	for (int i = 0; i < 15; i++)
		pPartikelSystem->PushPartikel (xPos-30+rand()%170, yPos-30+rand()%72, EXPLOSION_MEDIUM2);

	for (int i = 0; i < 4; i++)
		pPartikelSystem->PushPartikel (xPos+rand()%100, yPos+rand()%30, SPLITTER);

	for (int i = 0; i < 16; i++)
		pPartikelSystem->PushPartikel (xPos+rand()%100, yPos+rand()%30, SPIDERSPLITTER);

	pSoundManager->PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION3);

	pPlayer[0]->Score += 800;		// Punkte geben
}
