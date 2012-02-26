// --------------------------------------------------------------------------------------
// Der Bonus Block
//
// Schiesst man auf ihn, und seine Energie sinkt auf 0, so springt ein Extra heraus,
// Value 1 wird verringert und die Energie neu gesetzt, bis Value1 == 0 ist
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_PowerBlock.h"

//					    	Spread Laser Bounce	Blitz Shield Energy	Rad	Powerline Granate Smartbomb Autofire Supershot   Score
int	WaffenVerteilung[14] = {  100,   200,  300,   500,  505,   530,  550,   600,     650,     675,     680,     685,     1000,     1000};


// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPowerBlock::GegnerPowerBlock(int Wert1, int Wert2, bool Light)
{
	Handlung    = GEGNER_STEHEN;
	AnimPhase	= 0;
	Energy	    = 100.0f;

	for (int i = 0; i < 16; i++)
		AlreadySpawned[i] = false;

	// Itemanzahl standard mässig auf 16 setzen, wenn keine Items drin sind (weil Turri das immer vergisst im Editor)
	if (Wert1 == 0)
		Value1 = 10;
	else
		Value1 = Wert1;

	// Je mach Skill weniger Extras
	//
	if (Skill == 0)	Value1 = int (Value1 * 1.0f);
	if (Skill == 1)	Value1 = int (Value1 * 0.7f);
	if (Skill == 2)	Value1 = int (Value1 * 0.5f);	
	if (Skill == 3)	Value1 = int (Value1 * 0.3f);

	Value1 = 4;

	// Typ des Extras merken
	// Ist der Typ == 0, werden Extras per Zufall gespawnt
	ExtraType     = Wert2;
	ChangeLight   = Light;
	Destroyable   = true;
	OwnDraw		  = true;
	WeaponSpawned = NUMPLAYERS; // Maximal "Anzahl Spieler" Waffen Powerup spawnen
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerPowerBlock::DoDraw(void)
{
	DirectGraphics.SetColorKeyMode();

	//normal
	if (AnimPhase > 0)
	{
		pGegnerGrafix[GegnerArt]->RenderSprite ((float)(xPos-pTileEngine->XOffset),
											    (float)(yPos-pTileEngine->YOffset),
											    1 + pTileEngine->DateiAppendix.UsedPowerblock * 2, 0xFFFFFFFF, false);
	}
	
	// nochmal leuchtend drüber (ausfadend)
	if (AnimPhase == 2)
	{
		D3DCOLOR col = D3DCOLOR_RGBA(255, 255, 255, int(DamageTaken));
		pGegnerGrafix[GegnerArt]->RenderSprite ((float)(xPos-pTileEngine->XOffset),
											    (float)(yPos-pTileEngine->YOffset),
											    AnimPhase + pTileEngine->DateiAppendix.UsedPowerblock * 2, col, false);
	}
}

// --------------------------------------------------------------------------------------
// Bewegungs "KI"
// --------------------------------------------------------------------------------------

void GegnerPowerBlock::DoKI(void)
{
	if (AnimPhase == 0)
		Value2   = int(yPos);			// yPos sichern

	if (AnimPhase > 0)
		PlattformTest(GegnerRect[GegnerArt]);

	// Bei Treffer erscheinen lassen
	if (DamageTaken != 0.0f &&
		AnimPhase == 0)				
	{
		AnimPhase = 1;
		pPlayer[0]->BlocksFullGame++;
		pPlayer[0]->BlocksThisLevel++;
		Energy = 12.0f;
	}

	// Bei Treffer andere AnimPhase setzen
	//
	if (AnimPhase != 0)
	{
		if (AnimPhase == 2 &&
			DamageTaken == 0.0f)
			AnimPhase = 1;
	}

	// Spieler ist dagegengesprungen
	//
	for (int i = 0; i < NUMPLAYERS; i++)
	if (AnimPhase == 1    &&
		pPlayer[i]->yspeed < 0.0f &&
		pPlayer[i]->ypos + pPlayer[i]->CollideRect.top <= yPos + GegnerRect[GegnerArt].bottom &&
		pPlayer[i]->ypos + pPlayer[i]->CollideRect.top >= yPos &&
		pPlayer[i]->xpos + pPlayer[i]->CollideRect.left  <= xPos + GegnerRect[GegnerArt].right &&
		pPlayer[i]->xpos + pPlayer[i]->CollideRect.right >= xPos)
	{
		Energy    = 0;					// Extra verlieren
		AnimPhase = 2;
		Handlung  = GEGNER_SPRINGEN;		// Block spingen lassen		

		yAcc =  4.0f;

		// Sprung-Geschwindigkeit setzen
		if (ySpeed == 0.0f)
			ySpeed = -8.0f;
		else
		if (ySpeed > 0.0f)
			ySpeed *= -1.0f;

		pPlayer[i]->yspeed = -pPlayer[i]->yspeed * 2/3;	// Spieler prallt ab

	}

	// Block nach Springen wieder unten ?
	if (ySpeed != 0.0f)
	{
		if (yPos > Value2)					// Alte Position erreicht ?
		{
			yPos	 = float(Value2);
			ySpeed	 = 0.0f;
			yAcc	 = 0.0f;
			Handlung = GEGNER_STEHEN;
		}
	}

	if (Energy <= 0.0f)				// Ein Extra verlieren ?
	{
		AnimPhase = 2;
		pPlayer[0]->Score += 10;		// Punkte geben

		int r, extra;

		// Spezielle Items ausspucken?
		//
		if (ExtraType != 0)
			extra = ExtraType; 

		// Ansonsten per Zufall
		//
		else
		{
			// erstes Powerup immer Energie
			if (Value1 == 4)
				extra = 5;
			else
			{
				extra = 5;

				while (AlreadySpawned[extra])
				{

					// Per Zufallsverteilung entsprechendes Extra setzen
					if (WeaponSpawned > 0)
						r = rand()%675;
					else
						r = rand()%175 + 500;

					for (int i = 0; i < 12; i++)
					if (r < WaffenVerteilung[i])
					{
						if (i <= 2)
							WeaponSpawned--;

						extra = i;
						break;
					}
				}
			}

			AlreadySpawned[extra] = true;
		}

		pGegner->PushGegner(xPos + 10, yPos, EXTRAS, extra, 0, true);

		Value1--;					// Extra-Speicher eins verringern
		if (Value1 > 0)				// und wenn noch eins drin ist, 
			Energy = 12.0f;			// dann die Energy wieder hochsetzen
		else
			Energy = 0.0f;			// ansonsten kaputt machen =)
	}	
}

// --------------------------------------------------------------------------------------
// explodiert
// --------------------------------------------------------------------------------------

void GegnerPowerBlock::GegnerExplode(void)
{
	for (int i = 0; i < NUMPLAYERS; i++)
	if (pPlayer[i]->AufPlattform == this)
	{
		pPlayer[i]->AufPlattform = NULL;
		pPlayer[i]->JumpPossible = false;
		pPlayer[i]->Handlung  = SPRINGEN;
		pPlayer[i]->AnimPhase = 0;
		pPlayer[i]->yspeed    = 0.5f;
		pPlayer[i]->JumpAdd   = PLAYER_JUMPADDSPEED;
	}

	// Punkte geben
	pPlayer[0]->Score += 100;

	// Explosion erzeugen
	pPartikelSystem->PushPartikel(xPos-30, yPos-30, EXPLOSION_BIG);

	pSoundManager->PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION1);	// Sound ausgeben
}
