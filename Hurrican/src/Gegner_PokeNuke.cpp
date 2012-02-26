// --------------------------------------------------------------------------------------
// Die riesen Poke Nuke
//
// kracht am Fahrstuhl auseinander und fetzt den Hurri wenn er nicht ganz am Rand steht
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_PokeNuke.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPokeNuke::GegnerPokeNuke(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	Energy			= 10;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= false;
	ySpeed			= 27.0f;
	SmokeDelay		= 0.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerPokeNuke::DoKI(void)
{
	BlickRichtung = LINKS;

	switch (Handlung)
	{		
		case GEGNER_STEHEN:
		{
			Handlung = GEGNER_FALLEN;
			yPos -= 480.0f + 217.0f;
		} break;

		// Stein fällt runter
		case GEGNER_FALLEN:
		{
			if (yPos > g_Fahrstuhl_yPos - 217.0f)
				Energy = 0.0f;

			SmokeDelay -= 1.0f SYNC;

			if (SmokeDelay <= 0.0f)
			{
				SmokeDelay = 0.2f;
				pPartikelSystem->PushPartikel(xPos - rand()%26 + 24, yPos - 16, SMOKEBIG);
			}
		} break;

		default : break;
	} // switch

	// Testen, ob der Spieler den Stein berührt hat
	TestDamagePlayers(100.0f, true);
}

// --------------------------------------------------------------------------------------
// PokeNuke explodiert
// --------------------------------------------------------------------------------------

void GegnerPokeNuke::GegnerExplode(void)
{
	pSoundManager->PlayWave (100, 128, 8000 + rand ()%4000, SOUND_EXPLOSION2);
	ShakeScreen (7);

	pAim = pPlayer[0];
	if (PlayerAbstand() < 160)	
		pAim->DamagePlayer (50);

	if (NUMPLAYERS == 2)
	{
		pAim = pPlayer[1];
		if (PlayerAbstand() < 160)	
			pAim->DamagePlayer (50);
	}

	for (int i = 0; i < 16; i++)	pPartikelSystem->PushPartikel (xPos - 60 + rand ()%90, yPos - 40 + rand ()%200, SMOKEBIG);	
	for (int i = 0; i < 8; i++)			pPartikelSystem->PushPartikel (xPos - 50 + rand ()%90, yPos - 30 + rand ()%200, EXPLOSION_BIG);
	for (int i = 0; i < 12; i++)		pPartikelSystem->PushPartikel (xPos - 30 + rand ()%90, yPos - 20 + rand ()%200, EXPLOSION_MEDIUM2);
	for (int i = 0; i < 12; i++)		pPartikelSystem->PushPartikel (xPos - 30 + rand ()%90, yPos + 100 + rand ()%100, SPLITTER);
	for (int i = 0; i < 200; i++)		pPartikelSystem->PushPartikel (xPos + rand ()%90, yPos + 50 + rand ()%150, FUNKE);

	for (int i = 0; i < 48; i++)		pPartikelSystem->PushPartikel (xPos - 67 + rand ()%130, yPos - 30 + rand ()%200, EXPLOSIONFLARE);
}
