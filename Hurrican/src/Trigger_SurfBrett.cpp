// --------------------------------------------------------------------------------------
// Das Surfbrett
//
// Wird aktiviert, sobald der Spieler daraufspringt
// Dann raste es los, der Level scrollt mit
// Bleibt immer auf x-Höhe des Spielers
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_SurfBrett.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSurfBrett::GegnerSurfBrett(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_STEHEN;
    BlickRichtung	= RECHTS;
    Energy			= 100;
    Value1			= Wert1;
    Value2			= Wert2;
    ChangeLight		= Light;
    Destroyable		= false;
    SmokeCount	    = 0.0f;
    yAcc			= 0.25f;
}

// --------------------------------------------------------------------------------------
// "SurfBrett KI"
// --------------------------------------------------------------------------------------

void GegnerSurfBrett::DoKI(void)
{
    /*
    SimpleAnimation();
    BlickRichtung	= LINKS;

    // Testen, ob der Spieler den Sack berührt hat
    //
    PlattformTest(GegnerRect[GegnerArt]);

    // LevelMusik beeinflussen ?
    // Denn in stage xx geht die Musik erst los, wenn der Spieler auf das SurfBrett springt !
    //
    if (MUSIC_IsPlaying(SoundManager.its_Songs[MUSIC_STAGEMUSIC]->SongData))
    	SoundManager.StopSong(MUSIC_STAGEMUSIC, true);

    // Draufgesprungen, dann gehts los
    //
    if (pPlayer->AufPlattform == this &&
    	Handlung == GEGNER_STEHEN)
    {
    	Handlung = GEGNER_LAUFEN;
    	pPlayer->Handlung = SURFEN;
    	TileEngine.ScrollLevel (xPos - 150, yPos - 350,  ZUSTAND_SCROLLTOLOCK);

    	// Kurz abgehen
    	//
    	yPos = float (Value1);
    	pPlayer->JumpedOnSurfboard = true;
    	SoundManager.PlayWave(100, 128, 11025, SOUND_EXPLOSION1);
    	PartikelSystem.PushPartikel (xPos - 20, yPos - 20, EXPLOSION_MEDIUM2);
    }

    // Je nach Handlung richtig verhalten
    switch (Handlung)
    {
    	// Auf den Hurri warten
    	//
    	case GEGNER_STEHEN:
    	{
    		// schaukeln lassen
    		//
    		if (int (yPos) <= Value1) yAcc =  0.25f;
    		if (int (yPos) >  Value1) yAcc = -0.25f;

    		if (ySpeed >  1.0f)	ySpeed =  1.0f;
    		if (ySpeed < -1.0f)	ySpeed = -1.0f;
    	} break;

    	case GEGNER_SPRINGEN:
    	{
    		// xPos an und yPos von Spielerpos angleichen
    		//
    		xPos = pPlayer->xpos;

    		if (pPlayer->AufPlattform == this)
    			pPlayer->ypos = yPos + GegnerRect [GegnerArt].top - 64;

    		// wieder absenken
    		if (Value2 - int (pPlayer->ypos) > 50 ||
    			pPlayer->Aktion [AKTION_JUMP] == false)
    			yAcc = 8.0f;

    		// Wieder gelandet
    		if (int (yPos) > Value1)
    		{
    			ySpeed = 0.0f;
    			yAcc = 0.0f;
    			Handlung = GEGNER_LAUFEN;
    			pPlayer->Handlung = SURFEN;
    		}
    	} break;

    	case GEGNER_LAUFEN:						// Normal fliegen und dabei ab und zu schiessen
    	{
    		// schaukeln lassen
    		//
    		if (int (yPos) <= Value1) yAcc =  0.25f;
    		if (int (yPos) >  Value1) yAcc = -0.25f;

    		if (ySpeed >  1.0f)	ySpeed =  1.0f;
    		if (ySpeed < -1.0f)	ySpeed = -1.0f;

    		// xPos an und yPos von Spielerpos angleichen
    		//
    		xPos = pPlayer->xpos;

    		if (pPlayer->AufPlattform == this)
    			pPlayer->ypos = yPos + GegnerRect [GegnerArt].top - 64;

    		// Spritzer erzeugen
    		//
    		SmokeCount -= 1.0f SYNC;
    		if (SmokeCount < 0.0f)
    		{
    			SmokeCount += 0.5f;

    			PartikelSystem.PushPartikel(xPos - 25 + rand()%20, yPos + 12, WATERFLUSH2);

    			for (int i = 0; i < 5; i++)
    				PartikelSystem.PushPartikel(xPos  + rand()%50, yPos+30, WASSERTROPFEN);
    		}

    		// Springen
    		//
    		if (pPlayer->Handlung == SURFENJUMP &&
    			pPlayer->JumpPossible == true)
    		{
    			pPlayer->JumpPossible = false;
    			Value2 = int (pPlayer->ypos);
    			ySpeed = -30;
    			yAcc   = 0.0f;
    			Handlung = GEGNER_SPRINGEN;
    		}
    	} break;
    }

    // "Sprungschanzen" hochfahren ?
    if (blockr & BLOCKWERT_GEGNERWAND)
    {
    	ySpeed = 0.0f;
    	yAcc   = 0.0f;
    	yPos -= (pPlayer->xadd + pPlayer->AutoScrollspeed) SYNC;
    }

    // "Sprungschanzen" abspringen ?
    if (blockr & BLOCKWERT_WENDEPUNKT ||
    	blocku & BLOCKWERT_WENDEPUNKT ||
    	blockl & BLOCKWERT_WENDEPUNKT)
    {
    	ySpeed = -30.0f;
    	yAcc   = 8.0f;
    	Handlung = GEGNER_SPRINGEN;
    }
    */
}

// --------------------------------------------------------------------------------------
// SurfBrett explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerSurfBrett::GegnerExplode(void)
{
}
