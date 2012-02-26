// --------------------------------------------------------------------------------------
// Extra-Leben
//
// Bringt beim Einsammeln ein Extra Leben, was sonst 
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_OneUp.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerOneUp::GegnerOneUp(int Wert1, int Wert2, bool Light)
{
	Handlung    = GEGNER_FALLEN;			// Zu Beginn fällt das Extraleben
	Energy	    = 1.0f;
	Value1	    = Wert1;
	Value2	    = Wert2;

	if (Value2 != 99)
		yAcc		= 2.0f;

	AnimSpeed   = 0.75f;
	ChangeLight = Light;
	Destroyable = false;
	AnimEnde	= 20;
	AnimSpeed   = 0.5f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerOneUp::DoKI(void)
{
	SimpleAnimation();

	// auf dem Fliessband bewegen
	if (blocku & BLOCKWERT_FLIESSBANDL)
		xSpeed = -11.0f;

	if (blocku & BLOCKWERT_FLIESSBANDR)
		xSpeed = 11.0f;

	BlickRichtung = LINKS;

	switch(Handlung)
	{
		case GEGNER_FALLEN :			// Leben fällt runter
		{
			if (ySpeed > 50.0f)			// Fall-Limit nicht überschreiten
				ySpeed = 50.0f;
			// Am Boden abhopfen ?
			if (pTileEngine->BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]) & BLOCKWERT_WAND ||
				pTileEngine->BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]) & BLOCKWERT_PLATTFORM)
			{
				yPos  -= 1.0f;
				ySpeed = -ySpeed * 0.2f;
				if (ySpeed > -0.5f)				// Wieder abspringen oder liegenbleiben ?
				{
					xSpeed	 = 0.0f;
					ySpeed   = 0.0f;
					yAcc     = 0.0f;
					Handlung = GEGNER_SPECIAL;
				}
			}
		} break;

		case GEGNER_STEHEN :
		{		
			// Wieder runterfallen
			//
			if (!(pTileEngine->BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]) & BLOCKWERT_WAND) &&
				!(pTileEngine->BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]) & BLOCKWERT_PLATTFORM))
			{
				Handlung = GEGNER_FALLEN;
				yAcc	 = 2.0f;
			}
		} break;

	} // switch

	// Testen, ob der Spieler das Extraleben eingesammelt hat
	for (int i = 0; i < NUMPLAYERS; i++)
	if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt],
						pPlayer[i]->xpos, pPlayer[i]->ypos, pPlayer[i]->CollideRect) == true)
	{
		pPlayer[0]->Score += 1000;					// Punkte vergeben
		pPlayer[i]->Lives++;						// Anzahl der Leben erhöhen
		pPlayer[0]->LivesFullGame++;
		pPlayer[0]->LivesThisLevel++;
		Energy = 0.0f;							// Und Leben verschwinden lassen

	pPartikelSystem->PushPartikel(xPos - 40, yPos - 40, EXPLOSIONFLARE);
		pPartikelSystem->PushPartikel(xPos - 40, yPos - 40, EXPLOSIONFLARE);

		for (int i = 0; i < 200; i++)
			pPartikelSystem->PushPartikel(xPos + 20, yPos + 20, KRINGELSECRET);
	}
}

// --------------------------------------------------------------------------------------
// Extra Leben verschwindet
// --------------------------------------------------------------------------------------

void GegnerOneUp::GegnerExplode(void)
{
	pSoundManager->PlayWave(100, 128, 11025, SOUND_COLLECT);		// Sound ausgeben
	pSoundManager->PlayWave(100, 128, 11025, SOUND_VOICE_EXTRALIFE);	// Sound ausgeben
}
