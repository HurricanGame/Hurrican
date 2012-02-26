// --------------------------------------------------------------------------------------
// Der Schuss des Schneekönigs
//
// kann vom Spieler angeschossen werden und ändert dann die Richtung
// --------------------------------------------------------------------------------------

#include "math.h"
#include "stdafx.h"
#include "Gegner_Schneekoppe.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSchneekoppe::GegnerSchneekoppe(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_LAUFEN;
	Energy			= 600;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= true;

	xSpeed = (float)sin(Wert1 * PI / 180.0f) * Value2;
	ySpeed = (float)cos(Wert1 * PI / 180.0f) * Value2;
	
	yAcc   = 6.0f;
//	AnimSpeed = 0.5f;
//	AnimEnde  = 20;
	isHit = false;
	OwnDraw = true;
}

// --------------------------------------------------------------------------------------
// Eigene Drawroutine
// --------------------------------------------------------------------------------------

void GegnerSchneekoppe::DoDraw(void)
{
	// Drehwinkel aus der Geschwindigkeit errechnen
	
	float w = 180.0f - float(atan(xSpeed / ySpeed) * 360.0f / (D3DX_PI * 2));

	if (xSpeed >= 0 && ySpeed >= 0) w = w;		  else
	if (xSpeed > 0  && ySpeed < 0 ) w = 180 + w;  else
	if (xSpeed < 0  && ySpeed > 0 ) w = 360 + w;  else
	if (xSpeed < 0  && ySpeed < 0 ) w = 180 + w;

	pGegnerGrafix[GegnerArt]->RenderSpriteRotated((float)(xPos-pTileEngine->XOffset), 
										  		  (float)(yPos-pTileEngine->YOffset), 
												   w, AnimPhase, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSchneekoppe::DoKI(void)
{
	BlickRichtung = RECHTS;

	SimpleAnimation();

	if (DamageTaken > 0.0f)
		isHit = true;

	// Links oder rechts an der Wand abgeprallt ?
	//
	if ((blockl & BLOCKWERT_WAND) ||
		(blockr & BLOCKWERT_WAND) ||
		(blocko & BLOCKWERT_WAND) ||
		(blocku & BLOCKWERT_WAND) ||
		(blocku & BLOCKWERT_PLATTFORM))
	{
		Energy = 0.0f;
	}

	// Spieler verletzen ?
	//
	TestDamagePlayers(20.0f, true);
}

// --------------------------------------------------------------------------------------
// Auge explodiert
// --------------------------------------------------------------------------------------

void GegnerSchneekoppe::GegnerExplode(void)
{
	for (int i = 0; i < 10; i++)
		pPartikelSystem->PushPartikel(xPos - 10 + rand ()%20, 
									  yPos - 10 + rand ()%20, BLUE_EXPLOSION);

	pSoundManager->PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION1);	// Sound ausgeben
}
