// --------------------------------------------------------------------------------------
// Die Säule
//
// Steht in Level rum und fällt beim abschiessen um. Dabei dreht sich das Level
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_Column.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerColumn::GegnerColumn(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	BlickRichtung	= LINKS;
	Energy			= 120;
	Value1			= Wert1;			// yPos der Plattform
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= true;
	TestBlock		= false;
	OwnDraw			= true;
	FallSpeed		= 0.0f;
	FallValue		= 0.0f;
}

// --------------------------------------------------------------------------------------
// Column KI
// --------------------------------------------------------------------------------------

void GegnerColumn::DoDraw(void)
{
	D3DXMATRIX	matWorld, matRot, matTrans, matTrans2;	// Rotations und Translations Matrizen
	int			Winkel;									// Rotationswinkel

	Winkel = int(AnimCount);

	// Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
	//
	if (Winkel > 360) Winkel -= 360;
	if (Winkel < 0)	  Winkel += 360;
	D3DXMatrixRotationZ  (&matRot, DegreetoRad[Winkel]);
	
	D3DXMatrixTranslation(&matTrans, float (-(xPos-pTileEngine->XOffset+40)),float (-(yPos-pTileEngine->YOffset+100)), 0.0f);		// Transformation zum Ursprung
	D3DXMatrixTranslation(&matTrans2,float   (xPos-pTileEngine->XOffset+40), float (  yPos-pTileEngine->YOffset+100),  0.0f);		// Transformation wieder zurück
	
	D3DXMatrixIdentity	 (&matWorld);
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans);		// Verschieben
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matRot);			// rotieren
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans2);		// und wieder zurück verschieben
	
	// rotierte Matrix setzen
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);


	pGegnerGrafix[GegnerArt]->RenderSprite ((float)(xPos-pTileEngine->XOffset), 
										    (float)(yPos-pTileEngine->YOffset), 0, 0xFFFFFFFF);

	// Normale Projektions-Matrix wieder herstellen
	D3DXMatrixRotationZ (&matWorld, 0.0f);
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	SetScreenShake();
}

// --------------------------------------------------------------------------------------
// Column KI
// --------------------------------------------------------------------------------------

void GegnerColumn::DoKI(void)
{
	// Säule wird umgeschossen ?
	//
	if (Energy < 100.0f && 
		Handlung != GEGNER_EXPLODIEREN)
	{
		Handlung = GEGNER_EXPLODIEREN;

		for (int i = 0; i < 20; i++)
			pPartikelSystem->PushPartikel (xPos + rand ()%40, yPos + rand()%15 - 20, SMOKE);

		for (int i = 0; i < 10; i++)
			pPartikelSystem->PushPartikel (xPos + rand ()%40, yPos + rand()%10 - 5, ROCKSPLITTERSMALL);

		FallSpeed = 2.0f;

		pSoundManager->PlayWave(100, 128, 11025 + rand()%2000, SOUND_STONEFALL);		
		pSoundManager->PlayWave (100, 128, 8000 + rand()%4000, SOUND_COLUMN);
		pPartikelSystem->ThunderAlpha = 255;
		pPartikelSystem->ThunderColor [0] = 255;
		pPartikelSystem->ThunderColor [1] = 255;
		pPartikelSystem->ThunderColor [2] = 255;
		pPartikelSystem->ThunderAlpha = 255;
	}

	switch (Handlung)
	{
		case GEGNER_STEHEN:
		{
		} break;

		// Säule fällt gerade um ?
		//
		case GEGNER_EXPLODIEREN:
		{			
			FallSpeed += 0.5f SYNC;
			AnimCount += FallSpeed SYNC;

			FallValue += 0.5f SYNC;
			if (FallValue < 6.0f)
				ScreenWinkel -= 0.5f SYNC;			

			Energy = 100.0f;

			// Komplett umgefallen ?
			//
			if (AnimCount >= 90 || AnimCount <= -90.0f)
				Energy = 0.0f;
		} break;
	}	

	// Testen, ob der Spieler die Säule berührt hat
	//
	if (Handlung == GEGNER_STEHEN)
		Wegschieben(GegnerRect[GegnerArt], 0.0f);
}

// --------------------------------------------------------------------------------------
// Column fliegt auseinander
// --------------------------------------------------------------------------------------

void GegnerColumn::GegnerExplode(void)
{
	for (int i = 0; i < 30; i++)
		pPartikelSystem->PushPartikel (xPos + rand ()%100, yPos + 80 + rand()%40, SMOKE);

	for (int i = 0; i < 40; i++)
	{
		pPartikelSystem->PushPartikel (xPos + rand ()%90 + 10, yPos + 60 + rand()%40, ROCKSPLITTERSMALL);
		pPartikelSystem->PushPartikel (xPos + rand ()%90 + 10, yPos + 60 + rand()%40, ROCKSPLITTER);
	}

	pSoundManager->PlayWave(100, 128, 11025 + rand()%2000, SOUND_STONEEXPLODE);

	ShakeScreen (2.0f);
}
