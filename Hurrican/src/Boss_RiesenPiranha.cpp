// --------------------------------------------------------------------------------------
// Der Riesen Piranha
//
// Schwimmt von einer Beckenseite zur anderen und spuckt wenn der Spieler in der Nähe ist
// kleine Piranhas
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_RiesenPiranha.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerRiesenPiranha::GegnerRiesenPiranha(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	AnimStart		= 0;
	AnimEnde		= 4;
	AnimSpeed		= 2.0f;
	xSpeed			= -7.0;
	ySpeed			=  0.0f;
	xAcc			=  0.0f;
	yAcc			=  0.0f;
	Energy			= 3000;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerRiesenPiranha::DoKI(void)
{
// Animieren
	if (AnimEnde > 0)						// Soll überhaupt anmiert werden ?
	{
		AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
		if (AnimCount > AnimSpeed)			// Grenze überschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen
			AnimPhase++;					// Und nächste Animationsphase
			if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
				AnimPhase = AnimStart;		// Dann wieder von vorne beginnen
		}
	} // animieren

// Per Zufall Bläschen erzeugen
	if (rand()%10 == 0)
		pPartikelSystem->PushPartikel(xPos + 10 + rand()%230, yPos + 10 + rand ()%190, BUBBLE);

// Nach links bzw rechts auf Kollision prüfen und dann ggf umkehren
	if (BlickRichtung == LINKS)
	  if (blockl & BLOCKWERT_WAND ||
		  blockl & BLOCKWERT_GEGNERWAND)
		{
			xSpeed	      = 0;
			Handlung	  = GEGNER_DREHEN;
			AnimPhase	  = 4;
			AnimStart	  = 0;
			AnimEnde	  = 8;
		}

	if (BlickRichtung == RECHTS)
	  if (blockr & BLOCKWERT_WAND ||
		  blockr & BLOCKWERT_GEGNERWAND)
		{
			xSpeed		  = 0;
			xPos		 -= 1.0f;
			Handlung	  = GEGNER_DREHEN;
			AnimPhase	  = 4;
			AnimStart	  = 0;
			AnimEnde	  = 8;
		}

// Spieler im Wasser und in Sichtweite ?
	  if (pAim->InLiquid == true &&	
		  PlayerAbstand() <= 300)
	  {
		  // ggf kleinen Piranha ausspucken
		  if (AnimPhase == 0 && AnimCount == 0.0f && rand()%2 == 0)
		  {
			  pGegner->PushGegner(xPos + 110, yPos + 100, PIRANHA, 0, 0, ChangeLight);
		  }

  		if (pAim->ypos-70 < yPos &&	// Spieler oberhalb oder
			!(blocko & BLOCKWERT_WAND) &&
			!(blocko & BLOCKWERT_GEGNERWAND))
			yPos -= 6.0f SYNC;

		if (pAim->ypos-70 > yPos &&	// unterhalb des Piranhas
			!(blocku & BLOCKWERT_WAND) &&
			!(blocku & BLOCKWERT_GEGNERWAND))
			yPos += 6.0f SYNC;			// Dann auf ihn zu schwimmen

	  }

// Je nach Handlung richtig verhalten
	switch (Handlung)
	{
		case GEGNER_LAUFEN:						// Piranha schwimmt rum
		{
			if (pAim->InLiquid == true &&	// Spieler im Wasser und
				PlayerAbstand() <= 300)			// in Sichtweite ?
			{
				// Piranha links oder rechts am Spieler vorbei ?
				// Dann umdrehen und weiter verfolgen
				if (Handlung == GEGNER_LAUFEN)
				{
					if ((BlickRichtung == LINKS &&
						pAim->xpos > xPos + GegnerRect[GegnerArt].right-20) ||

						(BlickRichtung == RECHTS &&
						pAim->xpos + pAim->CollideRect.right < xPos))
					{
						xSpeed	      = 0;			
						Handlung	  = GEGNER_DREHEN;
						AnimPhase	  = 4;
						AnimStart	  = 0;
						AnimEnde	  = 8;
					}
				}
			}
			else
			{
				if (BlickRichtung == LINKS)
					xSpeed = -7.0f;
				else
					xSpeed =  7.0f;
			}
		} break;
		
		case GEGNER_DREHEN:						// Piranha dreht sich um
		{
			if (AnimPhase == AnimStart)
			{
				Handlung  = GEGNER_LAUFEN;
				AnimEnde  = 4;
				AnimStart = 0;
				AnimPhase = 0;

				if (BlickRichtung == LINKS)		// Ab jetzt in die andere Richtung schwimmen
				{
					BlickRichtung = RECHTS;
					xSpeed = 7.0f;
				}
				else
				{
					BlickRichtung = LINKS;
					xSpeed = -7.0f;
				}
			}
		} break;

		default : break;
	} // switch

	// Testen, ob der Spieler den Piranha berührt hat
	TestDamagePlayers(5.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Riesen Piranha explodiert
// --------------------------------------------------------------------------------------

void GegnerRiesenPiranha::GegnerExplode(void)
{
	// Fetzen erzeugen
	int i = 0;
	for (int i=0; i<8; i++)
		pPartikelSystem->PushPartikel(float(xPos + 64 + rand()%64), 
									  float(yPos + 64 + rand()%64), PIRANHATEILE2);


	// und noch n paar Luftblässchen dazu
	for (i=0; i<100; i++)
		pPartikelSystem->PushPartikel(float(xPos - 10  + rand()%256), 
									  float(yPos + 10  + rand()%210), BUBBLE);

	// Blutwolken dazu
	for (i=0; i<70; i++)
		pPartikelSystem->PushPartikel(float(xPos - 10  + rand()%256), 
									  float(yPos + 10  + rand()%210), PIRANHABLUT);

	// und Extra Leben rauslassen
	pGegner->PushGegner(xPos + 110, yPos + 100, ONEUP, 0, 0, ChangeLight);

	//pSoundManager->PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION1);	// Sound ausgeben

	pPlayer[0]->Score += 4000;
}
