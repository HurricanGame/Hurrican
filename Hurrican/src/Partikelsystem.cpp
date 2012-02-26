// Datei : Partikelsystem.cpp

// -------------------------------------------------------------------------------------- 
//
// Partikelsystem für Hurrican
// für Funken, Rauchwolken, Patronenhülsen usw.
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "Partikelsystem.h"
#include "Console.h"
#include "DX8Font.h"
#include "DX8Graphics.h"
#include "DX8Sound.h"
#include "Gameplay.h"
#include "Globals.h"
#include "LogDatei.h"
#include "Player.h"
#include "Tileengine.h"
#include "Projectiles.h"
#include "Timer.h"

// --------------------------------------------------------------------------------------
// externe Variablen
// --------------------------------------------------------------------------------------

extern Logdatei				Protokoll;
extern DirectGraphicsFont   *pFont;
extern PartikelsystemClass	*pPartikelSystem;
extern TileEngineClass		*pTileEngine;
extern ProjectileListClass	*pProjectiles;
extern CSoundManager		*pSoundManager;
extern ConsoleClass			*pConsole;
extern LPDIRECT3DDEVICE8	lpD3DDevice;				// Direct3D Device-Objekt
extern VERTEX2D				v1, v2, v3, v4;						// Vertices zum Partikel rendern
extern VERTEX2D				TriangleStrip[4];					// Strip für einen Partikel

// --------------------------------------------------------------------------------------
// Variablen
// --------------------------------------------------------------------------------------

DirectGraphicsSprite	*pPartikelGrafix[MAX_PARTIKELGFX];	// Grafiken der Partikel
RECT					PartikelRect[MAX_PARTIKELGFX];		// Rechtecke für Level Kollision	
int						CurrentPartikelTexture;				// Aktuelle Textur der Partikel
int						DrawMode;							// normale oder rotierte Partikel?

// --------------------------------------------------------------------------------------
// PartikelKlasse Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

PartikelClass::PartikelClass(void)
{	
	// Partikelwerte auf Null setzen (zur Sicherheit)
	red = green = blue = 255;
	AnimCount = 0.0f;
	xPos	= 0.0f;	yPos	= 0.0f;
	xPosOld = 0.0f; yPosOld = 0.0f;
	xSpeed	= 0.0f;	ySpeed	= 0.0f;
	xAcc	= 0.0f;	yAcc	= 0.0f;
	OwnDraw				= false;
	Rotate				= false;	
	Rot					= 0.0f;
	RemoveWhenOffScreen = true;
	m_pParent = NULL;

	(rand()%2 == 0) ? (RotDir = 1) : (RotDir = -1);
}

// --------------------------------------------------------------------------------------
// Destruktor
// --------------------------------------------------------------------------------------

PartikelClass::~PartikelClass(void)
{
}

// --------------------------------------------------------------------------------------
// Partikel vom Typ "Art" erzeugen
// --------------------------------------------------------------------------------------

bool PartikelClass::CreatePartikel(float x, float y, int Art, PlayerClass *pParent)
{	
	// leucht-Partikel nur im Glow-Modus anzeigen
	// d.h., im nicht-Glow Modus gleich wieder aussteigen, sobald ein
	// solcher Partikel erzeugt werden soll
	//
	if (options_Detail < DETAIL_HIGH &&
		(Art == GRENADEFLARE ||
		 Art == SHOTFLARE    ||
		 Art == SHOTFLARE2   ||
		 Art == EXPLOSIONFLARE))
		return false;

	m_pParent = pParent;
	xPos = x;
	yPos = y;
	xPosOld = x;
	yPosOld = y;
	PartikelArt = Art;
	AnimPhase   = 0;
	AnimCount   = 0.0f;
	AnimEnde    = 0;		// Standardmässig von einem nicht animierten Partikel ausgehen
	AnimSpeed   = 0.0f;
	BounceWalls = false;

	// Zwei Case Anweisungen, einmal für nicht additive und einmal für additive Partikel
	// Wird dadurch im Schnitt hoffentlich doppelt so schnell ;)
	if (Art < ADDITIV_GRENZE)
	switch (Art)
	{
		case BULLET :	// Leere Patronenhülse
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			xSpeed		= (float(rand()%40+20)/5) * float(-m_pParent->Blickrichtung);
			ySpeed		= -float(rand()%40+20)/5;
			xAcc		= 0.0f;
			yAcc		= 5.0f;
			Lebensdauer = 255;
			BounceWalls = false;
			//AnimEnde	= 8;
			AnimSpeed	= 0.0f;

			Rotate		= true;

		} break;

		case BULLET_SKELETOR :	// Leere Patronenhülse vom Skeletor
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			xSpeed		= (float(rand()%40-20)/3);
			ySpeed		= -float(rand()%8+14);
			xAcc		= 0.0f;
			yAcc		= 10.0f;
			Lebensdauer = 255;
			BounceWalls = true;
			AnimSpeed	= 0.0f;

			Rotate		= true;

		} break;

		case GLASSPLITTER:
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			xSpeed		= (float(rand()%40-20)) / 2;
			ySpeed		= -float(rand()%10+20) / 2;
			xAcc		= 0.0f;
			yAcc		= 5.0f;
			Lebensdauer = 255;
			BounceWalls = true;
			AnimPhase = rand()%20;

			Rotate		= true;
		} break;
			 


		case EXPLOSION_MEDIUM :	// Mittelgrosse Explosion
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 192;
			AnimEnde	= 19;
			AnimSpeed	= float((rand()%6+3)/20.0f);

			pPartikelSystem->PushPartikel (x - 30, y - 30, EXPLOSIONFLARE);			

			Rotate = true;

			if (rand()%2 == 0)
				PartikelArt = EXPLOSION_MEDIUM2_ADD;
			
		} break;		

		case EXPLOSION_MEDIUM2 :	// Mittelgrosse Explosion Nr 2
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 192;
			AnimEnde	= 19;
			AnimSpeed	= float((rand()%6+3)/20.0f);

			pPartikelSystem->PushPartikel (x - 30, y - 30, EXPLOSIONFLARE);			

			Rotate = true;

			if (rand()%2 == 0)
				PartikelArt = EXPLOSION_MEDIUM2_ADD;
			
		} break;

		case EXPLOSION_MEDIUM3 :	// Mittelgrosse Explosion Nr 2
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 192;
			AnimEnde	= 19;
			AnimSpeed	= float((rand()%6+3)/20.0f);

			pPartikelSystem->PushPartikel (x - 30, y - 30, EXPLOSIONFLARE);

			if (rand()%2 == 0)
				PartikelArt = EXPLOSION_MEDIUM3_ADD;

			Rotate = true;
			
		} break;

		case EXPLOSION_GIGA :	// Riesig fette Explosion
		{
			x -= 10;
			y -= 10;
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 192;
			AnimEnde	= 19;
			AnimSpeed	= float((rand()%2+5)/10.0f);

			pPartikelSystem->PushPartikel (x + 30, y + 30, EXPLOSIONFLARE);			

			for (int i = 0; i < 20; i++)
			{
				pPartikelSystem->PushPartikel (x + 60 + rand()%40, 
											   y + 60 + rand()%40, FUNKE);
				pPartikelSystem->PushPartikel (x + 60 + rand()%40, 
											   y + 60 + rand()%40, LONGFUNKE);
			}
			
		} break;		

		case EXPLOSION_GREEN :	// Grüne Explosion
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 192;
			AnimEnde	= 22;
			AnimSpeed	= float((rand()%10+5)/15.0f);
			
		} break;

		case EXPLOSION_ALIEN :	// Lilane Alien Explosion
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 224;
			AnimEnde	= 22;
			AnimSpeed	= float((rand()%5+10)/80.0f);

			pPartikelSystem->PushPartikel (x - 30, y - 30, EXPLOSIONFLARE);			
			
		} break;

		case EXPLOSION_BIG :	// Grosse Explosion
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 224;
			AnimEnde	= 19;
			AnimSpeed	= float((rand()%5+10)/30.0f);
			pPartikelSystem->PushPartikel (x + 32, y + 32, EXPLOSION_KRINGEL);

			if (options_Detail >= DETAIL_MAXIMUM)
			for (int i = 0; i < 10; i++)
				pPartikelSystem->PushPartikel (x + 32, y + 32, SMOKEBIG2);

			pPartikelSystem->PushPartikel (x - 10, y - 10, EXPLOSIONFLARE);
		} break;

		case EXPLOSION_GIANT :	// Riesen Explosion
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 192;
			AnimEnde	= 15;
			AnimSpeed	= float((rand()%5+10)/15.0f);
			
		} break;

		case BLUE_EXPLOSION :	// Kleine blaue Explosion
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 224;
			AnimEnde	= 12;
			AnimSpeed	= float((rand()%10+5)/15.0f);
			
		} break;

		case SPLITTER :			// Kleiner animierter Splitter
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 200;
			AnimEnde	= 16;
			AnimSpeed	= 0.3f;
			xSpeed		= ((float)(rand()%160-80)/4);
			ySpeed		=-((float)((rand()%20+50)/2.0f));
			yAcc		= 3.0f;
			BounceWalls = true;
			
		} break;

		case PIRANHATEILE  :		// Fetzen eines kaputten Piranhas
		case PIRANHATEILE2 :		// Fetzen eines kaputten Riesen Piranhas
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 255;
			AnimPhase   = rand()%5;
			if (rand()%2 == 0)
			{
				xSpeed	= -((float)(rand()%20+4)/5);
				xAcc	=  0.1f;
			}
			else
			{
				xSpeed	=  ((float)(rand()%20+4)/5);
				xAcc	= -0.1f;
			}

			yAcc = -(float)(rand()%10+20)/100;
			
		} break;

		case PIRANHABLUT :		// Blut eines kaputten Piranhas
		{
			red	= 255; green = 255;	blue = 255;	alpha = 192;

			Lebensdauer = 192;
			AnimEnde	= 1;
			AnimSpeed	= 2.0f;
			ySpeed		= -(float)(rand()%10+20)/20;
			Rotate		= true;
		} break;

		case MADEBLUT :		// Blut der Made
		{
			red	= 255; green = 255;	blue = 255;	alpha = 192;

			Lebensdauer = 128;
			AnimEnde	= 4;
			AnimSpeed	= 1.0f;
		} break;

		case SPAWNDROP :		// Wassertropfen der vom Spawner ausgespuckt wird
		{
			red	= 230; green = 240;	blue = 255;	alpha = 128;

			Lebensdauer = 128;
			ySpeed	    = float (rand()%10) + 5;
			yAcc		= 4.0f;
			RemoveWhenOffScreen = false;
		} break;

		case BLATT :			// Blatt von kaputter Pflanze
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 255;
			ySpeed	    = - float ((rand()%40) + 25) / 3.0f;
			yAcc	    = 5.0f;
			xSpeed	    = float ((rand()%40) - 20) / 2.0f;			

			Rotate = true;

			AnimPhase = rand()%2;

		} break;

		case BLATT2:			// Blatt vom Spawner
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 255;
			ySpeed	    = - float ((rand()%20) - 10) / 10.0f;
			yAcc	    = - float ((rand()%20) - 10) / 20.0f;
			xSpeed	    = - float (rand()%50 + 30);

			if (WinkelUebergabe != 0.0f)
				xSpeed *= -1;

			Rotate = true;

			AnimPhase = rand()%2;

		} break;

		case NESTLUFT :			// Wespennest nach dem Abschuss
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 255;			
			yAcc	    = 5.0f;
			xSpeed	    =  float ((rand()%40) - 20) / 2.0f;
			ySpeed	    = -float ((rand()%10) + 20);
			AnimSpeed   = 1.5f;

			BounceWalls = true;
			Rotate		= true;			

			RotDir      = 2.0f;

		} break;

		case FOG :		// Nebel in der Luft
		{
			red	= 64; green = 255;	blue = 80; alpha = 0;

			xSpeed =  (float)(rand()%30 + 10) / 50.0f;
			ySpeed = -3.0f * (AnimPhase + 1);

			if (rand()%2 == 0) xSpeed *= -1.0f;

			Lebensdauer = 255;
			AnimPhase = rand()%2;
			
			RemoveWhenOffScreen = false;
			AnimCount = 0.0f;
		} break;

		case HALSWIRBEL :	// Halswirbel des MetalHead Bosses
		{
			red	= 255; green = 255;	blue = 255;	alpha = 0;
			xSpeed		=   (float)(rand()%20+20)/2.0f;
			ySpeed		= - (float)(rand()%20+30);
			yAcc		= 5.0f;
			Lebensdauer = 255;
			AnimCount   = float (rand()%100);
			OwnDraw		= true;
		} break;

		case KAPUTTETURBINE :	// Kaputte Turbine des MetalHead Bosses
		{
			red	= 255; green = 255;	blue = 255;	alpha = 0;
			xSpeed		= 18.0f;
			ySpeed		= -30.0f;
			yAcc		= 5.0f;
			Lebensdauer = 255;
			AnimCount   = 0.0f;
			OwnDraw		= true;
		} break;

		case ROCKSPLITTER :	// Splitter eines Felsblocks
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			xSpeed		= (float(rand()%80-40)/4);
			ySpeed		= -float(rand()%30+15)/4;
			xAcc		= 0.0f;
			yAcc		= 2.8f;
			Lebensdauer = 255;
			BounceWalls = true;
			AnimEnde	= 8;
			AnimSpeed	= float((rand()%10+5)/12.0f);
		} break;

		case ROCKSPLITTERSMALL :	// kleine Splitter eines Felsblocks
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			xSpeed		= (float(rand()%80-40)/4);
			ySpeed		= -float(rand()%40+20)/3;
			xAcc		= 0.0f;
			yAcc		= 2.8f;
			Lebensdauer = 255;
			BounceWalls = true;
			AnimEnde	= 8;
			AnimSpeed	= float((rand()%10+5)/8.0f);
		} break;

		case ROCKSPLITTERSMALLBLUE :	// kleine Splitter eines Stalagtits
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			xSpeed		= (float(rand()%80-40)/4);
			ySpeed		= -float(rand()%40+20)/3;
			xAcc		= 0.0f;
			yAcc		= 2.8f;
			Lebensdauer = 255;
			BounceWalls = true;
			AnimEnde	= 8;
			AnimSpeed	= float((rand()%10+5)/8.0f);
		} break;

		case SPIDERSPLITTER :	// Splitter der Spinne
		{
			PartikelArt += rand()%2;
			int r = rand()%128 + 128;
			red	= green = blue = r; alpha = 255;

			xSpeed		= (float(rand()%80-40)/3.0f);
			ySpeed		= -float(rand()%30+15)/2.0f;
			xAcc		= 0.0f;
			yAcc		= 3.0f;
			Lebensdauer = 255;
			BounceWalls = true;
			AnimEnde	= 16;
			AnimSpeed	= float((rand()%10+5)/12.0f);
		} break;

		case SPIDERGRENADE :	// Granate der Spinne
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			xSpeed		= -14.0f;
			ySpeed		= -15.0f;
			yAcc		=   5.0f;
			Lebensdauer = 255;
			BounceWalls = false;
			AnimEnde	= 4;
			AnimSpeed	= float((rand()%10+5)/12.0f);
		} break;

		case EVILSMOKE:			// Schatten des EvilHurri links
		{
			red	= 255; green = 255;	blue = 255; alpha = 128;

			Lebensdauer = 200;
			BounceWalls = false;
		} break;

		case EVILSMOKE2:		// Schatten des EvilHurri rechts
		{
			red	= 255; green = 255;	blue = 255; alpha = 128;

			Lebensdauer = 200;
			BounceWalls = false;
		} break;

		case STELZE:			// Stelze eines Stelzsacks
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			Lebensdauer = 255;			
			xSpeed		= (float(rand()%40-20)/3.0f);
			ySpeed		= -float(rand()%80+40)/3.0f;
			yAcc		= 5.0f;

			Rotate		= true;
		} break;

		case STELZHEAD:			// Kopf eines Stelzsacks
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			Lebensdauer = 255;
			xSpeed		= (float(rand()%40-20)/5.0f);
			ySpeed		= -float(rand()%80+40)/4.0f;
			yAcc		= 4.0f;
			AnimCount   = 1.0f;
		} break;

		case SMOKE:			// Rauchwolke
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;
			ySpeed		= -(float)(rand()%10)/10.0f;
			yAcc		= -0.1f;

			Lebensdauer = 255;
			Rotate = true;
			Rot = (float)(rand()%360);

		} break;

		case SMOKE2:			// Rauch bei zB Lava Bällen
		{
			red	= 255; green = 255;	blue = 255; alpha = 128;

			Lebensdauer = 80;
			xSpeed		= (float(rand()%40-20)/10.0f);
			ySpeed		= -float(rand()%40-20)/6.0f;

			// Grafik hat sich von 20x20 auf 24x24 pixel geändert :P
			xPos -= 2;
			yPos -= 2;
			
			Rotate = true;

		} break;

		case SMOKE3:			// Aufsteigender Dampf
		case SMOKE3_RO:			// rechts oben
		case SMOKE3_R:			// rechts
		case SMOKE3_RU:			// rechts unten
		case SMOKE3_U:			// unten
		case SMOKE3_LU:			// links unten
		case SMOKE3_L:			// links
		case SMOKE3_LO:			// links oben
		{
			red	= 255; green = 255;	blue = 255; alpha = 64;

			Lebensdauer = 128;

			switch (Art)
			{
				case SMOKE3:
				{
					xSpeed		= (float(rand()%40-20)/15.0f);
					ySpeed		= -float(rand()%10+20) / 2.0f;
				} break;

				case SMOKE3_RO:			// rechts oben
				{
					xSpeed		= float(rand()%10+20) / 3.0f;
					ySpeed		= - xSpeed - (float(rand()%40-20)/15.0f);
				} break;

				case SMOKE3_R:			// rechts
				{
					xSpeed		= float(rand()%10+20) / 2.0f;
					ySpeed		= - (float(rand()%40-20)/15.0f);
				} break;

				case SMOKE3_RU:			// rechts oben
				{
					xSpeed		= float(rand()%10+20) / 3.0f;
					ySpeed		= xSpeed - (float(rand()%40-20)/15.0f);
				} break;

				case SMOKE3_U:			// unten
				{
					xSpeed		= (float(rand()%40-20)/15.0f);
					ySpeed		= float(rand()%10+20) / 2.0f;
				} break;

				case SMOKE3_LU:			// links oben
				{
					xSpeed		= - float(rand()%10+20) / 3.0f;
					ySpeed		= - xSpeed - (float(rand()%40-20)/15.0f);
				} break;

				case SMOKE3_L:			// links
				{
					xSpeed		= - float(rand()%10+20) / 2.0f;
					ySpeed		= - (float(rand()%40-20)/15.0f);
				} break;

				case SMOKE3_LO:			// links oben
				{
					xSpeed		= - float(rand()%10+20) / 3.0f;
					ySpeed		= xSpeed - (float(rand()%40-20)/15.0f);
				} break;
			}			

			PartikelArt = SMOKE3;
			RemoveWhenOffScreen = false;
			Rotate = true;

		} break;		

		case SMOKEBIG_OUTTRO:			// Riesen Rauchwolke
		{
			red	= 128; green = 128;	blue = 128; alpha = 255;

			Lebensdauer = 90;
			xSpeed		= (float(rand()%40-20)/6.0f);
			ySpeed		= -float(rand()%10+10)/5.0f;
			
			AnimPhase = rand()%4;
			Rotate = true;

		} break;		

		case SMOKEBIG:			// Riesen Rauchwolke
		{
			red	= 255; green = 255;	blue = 255; alpha = 140;

			Lebensdauer = 140;
			xSpeed		= (float(rand()%40-20)/10.0f);
			ySpeed		= -float(rand()%20+10)/10.0f;
			
			AnimPhase = rand()%4;
			Rotate = true;

		} break;

		case SMOKEBIG2:			// Riesen Rauchwolke
		{
			red	= 128; green = 128;	blue = 128; alpha = 200;

			Lebensdauer = 200;
			xSpeed		= (float)(rand()%6-3) * 6.0f;
			ySpeed		= (float)(rand()%10-5) * 2.0f;
			yAcc        = -1.0f;
			
			AnimPhase = rand()%4;
			Rotate = true;

		} break;

		case FLOATSMOKE :		// Rauch der kleinen floating Plattform
		{
			red	= 255; green = 255;	blue = 255;	alpha = 144;
			xSpeed		= ((float)(rand()%40-20)/40);
			ySpeed		= float(rand()%10+40)/5;
			Lebensdauer = float(180 + rand()%10);
			PartikelArt = ROCKETSMOKE;
		
		} break;

		case DUST:
		{
			if (rand()%2 == 0)
			{
				red	= 255; green = 224;	blue = 128; alpha = 255;
				yAcc   = -0.8f;
			}
			else
			{
				red	= 255; green = 192;	blue = 64; alpha = 255;
				yAcc   =  0.8f;							
			}

			Lebensdauer = 255;
			AnimPhase = rand()%3;

			xSpeed = -5.0f * (AnimPhase + 1);
		} break;

		case SCHROTT1:
		{
			Lebensdauer = 255;
			AnimEnde = 20;
			AnimSpeed = (rand()%10 + 10) / 20.0f;

			xSpeed		= (float(rand()%80-40)/4.0f);
			ySpeed		= -float(rand()%10+8);
			xAcc		= 0.0f;
			yAcc		= 3.0f;
			Lebensdauer = 255;
			BounceWalls = true;

			if (rand()%2 == 0)
				PartikelArt = SCHROTT2;
		} break;

		case SCHNEEFLOCKE :		// Schneeflocke
		case SCHNEEFLOCKE_END:
		{
			red	= 255; green = 255;	blue = 255;	alpha = 60;
			xSpeed =  0.0f;

			if (rand()%2 == 0)		// Per Zufall nach links
				xAcc   =  0.2f;
			else					// oder rechts
				xAcc   = -0.2f;

			AnimPhase = rand()%3;
			ySpeed = (float)(rand()%10+15)/10;
			ySpeed *= (AnimPhase + 1);

			if (PartikelArt == SCHNEEFLOCKE_END)
			{
				xSpeed /= 2.0f;
				ySpeed /= 2.0f;
			}

			Lebensdauer = 60;			
			RemoveWhenOffScreen = false;						
		} break;		

		case BOULDER_SMALL :	// kleiner blauer Boulder
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			xSpeed		=  (float(rand()%80-40)/3.0f);
			ySpeed		= -(float(rand()%80-20)/3.0f);
			xAcc		= 0.0f;
			yAcc		= 5.0f;
			Lebensdauer = 255;
			BounceWalls = true;
			AnimEnde	= 15;
			AnimSpeed	= float((rand()%10+5)/20.0f);

		} break;		

		case LAVAKRABBE_KOPF :	// Kopf der Lava Krabbe
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			xSpeed		=  (float(rand()%80-40)/3.0f);
			ySpeed		= -(float(rand()%60+20)/2.0f);
			xAcc		= 0.0f;
			yAcc		= 5.0f;
			Lebensdauer = 255;
			BounceWalls = true;
			Rotate		= true;
			Rot			= float (rand()%360);

		} break;		

		case LAVAKRABBE_BEIN :	// Bein der Lava Krabbe
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			xSpeed		=  (float(rand()%80-40)/3.0f);
			ySpeed		= -(float(rand()%60+20)/2.0f);
			xAcc		= 0.0f;
			yAcc		= 5.0f;
			Lebensdauer = 255;
			BounceWalls = true;
			Rotate		= true;	
			Rot			= float (rand()%360);
			AnimPhase   = 1;
			PartikelArt = LAVAKRABBE_KOPF;

		} break;		

		case SPIDERPARTS :	// Spinnenstücke auf dem Fliessband
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;
			Lebensdauer = 255;
			AnimPhase = rand()%4;
			RemoveWhenOffScreen = false;

		} break;		

		case KETTENTEILE:
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;
			Lebensdauer = 255;
			AnimPhase = 0;			

			xSpeed		=  (float(rand()%80-40)/3.0f);
			ySpeed		= -(float(rand()%30+10)/2.0f);
			xAcc		= 0.0f;
			yAcc		= 5.0f;
			BounceWalls = true;
			Rotate		= true;
			Rot			= float (rand()%360);

		} break;

		case KETTENTEILE2:
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;
			Lebensdauer = 255;
			AnimPhase = 1;			

			xSpeed		=  (float(rand()%80-40)/3.0f);
			ySpeed		= -(float(rand()%30+10)/2.0f);
			xAcc		= 0.0f;
			yAcc		= 5.0f;
			BounceWalls = true;
			Rotate		= true;
			Rot			= float (rand()%360);

			PartikelArt = KETTENTEILE;

		} break;		

		case KETTENTEILE3:
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;
			Lebensdauer = 255;
			AnimPhase = 2;			

			xSpeed		=  (float(rand()%80-40)/3.0f);
			ySpeed		= -(float(rand()%30+10)/2.0f);
			xAcc		= 0.0f;
			yAcc		= 5.0f;
			BounceWalls = true;
			Rotate		= true;
			Rot			= float (rand()%360);

			PartikelArt = KETTENTEILE;

		} break;		

		case KETTENTEILE4:
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;
			Lebensdauer = 255;

			xSpeed		=  (float(rand()%80-40)/3.0f);
			ySpeed		= -(float(rand()%30+10)/2.0f);
			xAcc		= 0.0f;
			yAcc		= 5.0f;
			BounceWalls = true;
			Rotate		= true;
			Rot			= float (rand()%360);

		} break;				

		case WASSER_SPRITZER :	// Wasserspritzer, wenn der Spieler aus dem Wasser hopst
		{
			static int count = 0;			

			if (count == 0)
			{
				red   = 255;
				green = 255;				
				blue  = 255;
				count++;
			}
			else
			{
				count = 0;
				red   = pTileEngine->ColR3;
				green = pTileEngine->ColG3;
				blue  = pTileEngine->ColB3;
			}			

			if (rand()%2 == 0)
				Lebensdauer = (float)(rand()%128 + 128);
			else
				Lebensdauer = (float)(rand()%32 + 224);

			AnimSpeed = (float)(rand()%20 + 30);
			AnimCount = (float)(rand()%25 + 5) / 10.0f;

		} break;		

		case WASSER_SPRITZER2 :	// Wasserspritzer, wenn der Spieler ins Wasser hopst
		{
			static int count = 0;			

			if (count == 0)
			{
				red   = 255;
				green = 255;				
				blue  = 255;
				count++;
			}
			else
			{
				count = 0;
				red   = pTileEngine->ColR3;
				green = pTileEngine->ColG3;
				blue  = pTileEngine->ColB3;
			}

			if (rand()%2 == 0)
				Lebensdauer = (float)(rand()%128 + 128);
			else
				Lebensdauer = (float)(rand()%32 + 224);

			AnimSpeed = (float)(rand()%15 + 25);
			AnimCount = (float)(rand()%25 + 5) / 8.0f;

		} break;		

		case LAVA_SPRITZER :	// LavaSpritzer beim raushopsen
		{
			if (rand()%3 == 0)
			{
				red	= 255; green = 80;	blue = 32; alpha = 255;
			}
			else
			{
				red	= 255; green = 180;	blue = 64; alpha = 255;
			}

			Lebensdauer = 255;

			AnimSpeed = (float)(rand()%20 + 30);
			AnimCount = (float)(rand()%25 + 5) / 10.0f;

			PartikelArt = WASSER_SPRITZER;

		} break;		

		case LAVA_SPRITZER2 :	// LavaSpritzer beim reinhopsen
		{
			if (rand()%3 == 0)
			{
				red	= 255; green = 80;	blue = 32; alpha = 255;
			}
			else
			{
				red	= 255; green = 180;	blue = 64; alpha = 255;
			}

			Lebensdauer = 255;

			AnimSpeed = (float)(rand()%15 + 25);
			AnimCount = (float)(rand()%25 + 5) / 8.0f;

			PartikelArt = WASSER_SPRITZER2;

		} break;		

		case HURRITEILE_ARM1  :
		case HURRITEILE_ARM2  :
		case HURRITEILE_BEIN1 :
		case HURRITEILE_BEIN2 :
		case HURRITEILE_KOPF  :
		case HURRITEILE_WAFFE :
		case HURRITEILE_TORSO :
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 255;
			AnimPhase	= PartikelArt - HURRITEILE_ARM1;
			xSpeed		= ((float)(rand()%80-40)/4.0f);
			ySpeed		=-15.0f*(AnimPhase+2)/6.0f;
			yAcc		= 2.0f;
			BounceWalls = true;
			PartikelArt = HURRITEILE;
			
			Rotate = true;

		} break;
		
		case REGENTROPFEN :			// Regentropfen
		{
			red	= 220; green = 240;	blue = 255;	alpha = 192;
			xSpeed =  -24.0f;
			ySpeed =  44.0f;
			Lebensdauer = 255;	
			RemoveWhenOffScreen = false;
		} break;


		default:
		break;


	} // switch < Additiv Grenze

	else	

	switch (Art)
	{

		case EVILROUNDSMOKE:	// Rauch des Rundumschusses des evil hurri
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			xSpeed		= ((float)(rand()%60-30)/10);
			ySpeed		= ((float)(rand()%60-30)/10);

			Lebensdauer = 180;
			BounceWalls = false;
		} break;

		case BEAMSMOKE:		// Rauch des Blitzbeams
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			xSpeed		= ((float)(rand()%20-10)/20);
			ySpeed		= ((float)(rand()%20-10)/20);

			AnimCount   = 30.0f;
			Lebensdauer = 140;
			BounceWalls = false;

			OwnDraw		= true;	// eigene Draw-Routine, da er die Größe ändert
			Rotate		= true;
			RotDir      = 10.0f;
			Rot			= (float)(rand()%360);
		} break;

		case BEAMSMOKE2:	// Rauch beim Aufladen des Blitzbeams
		{
			double	absx, absy, speed;				// Variablen für die Geschwindigkeits-

			AnimPhase = rand()%3;

			if (m_pParent != NULL)
			{
														// berechnung
				absx = m_pParent->BeamX-xPos;				// Differenz der x 
				absy = m_pParent->BeamY-yPos;				// und y Strecke
				
				speed = 1/sqrt(absx*absx + absy*absy);	// Länge der Strecke berechnen
				speed = speed*(4+AnimPhase * 2);		// Geschwindigkeit

				absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
				absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

				xSpeed = float(absx);
				ySpeed = float(absy);
			}
			else
			{
				xSpeed = (float)(rand()%20 - 10) / 2.0f;
				ySpeed = (float)(rand()%20 - 10) / 2.0f;
			}

			red	= 255; green = 255;	blue = 255; alpha = 255;

			Lebensdauer = 255;
		} break;

		case BEAMSMOKE3:	// Rauch des Blitzbeams beim Explodieren
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			AnimPhase = rand()%3;

			float mul = (float)(rand()%100 + 10) / 10.0f;
			float arc = (float)(rand()%360) * PI / 180.0f;

			xSpeed		= (float)(sin(arc) * mul);
			ySpeed		= (float)(cos(arc) * mul);
			
			Lebensdauer = float (rand()%200 + 55);
		} break;

		case BEAMSMOKE4:		// Rauch des Blitzbeams
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			float mul = (float)(rand()%50 + 10) / 10.0f;
			float arc = (float)(rand()%360) * PI / 180.0f;

			xSpeed		= (float)(sin(arc) * mul);
			ySpeed		= (float)(cos(arc) * mul);

			Rotate = true;
			RotDir = (float)(rand()%10 + 15);
			Lebensdauer = float (rand()%200 + 55);
		} break;

		case BEAMSMOKE5:		// Druckwelle beim Beam Explodieren
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;			

			if (int (xPos) % 2 == 0)
				AnimPhase = 20;
			else
				AnimPhase = 40;
			Lebensdauer = 255;
			OwnDraw		= true;
		} break;

		case SNOWFLUSH:		// SchneeGestöber und Wasserfall Rauch
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;

			xSpeed		= ((float)(rand()%60-30)/20);
			ySpeed		= ((float)(rand()%60-30)/20);
			yAcc		= 0.5f;

			Lebensdauer = 140;
			BounceWalls = false;
			Rotate = true;

		} break;

		case WATERFLUSH:	// Wasserfall Dampfwolken
		{
			static int count = 0;			

			if (count == 0)
			{
				red   = 255;
				green = 255;				
				blue  = 255;
				count++;
			}
			else
			{
				count = 0;
				red   = pTileEngine->ColR3;
				green = pTileEngine->ColG3;
				blue  = pTileEngine->ColB3;
			}						

			xSpeed		= ((float)(rand()%80-40)/20);
			ySpeed		= ((float)(rand()%60-10)/20);
			yAcc		= 0.5f;

			Lebensdauer = 75;
			Rotate = true;

		} break;


		case WATERFLUSH2:	// Surf Dampfwolken
		{
			static int count = 0;

			if (count == 0)
			{
				red   = 255;
				green = 255;				
				blue  = 255;
				count++;
			}
			else
			{
				count = 0;
				red   = pTileEngine->ColR3;
				green = pTileEngine->ColG3;
				blue  = pTileEngine->ColB3;
			}						

			xSpeed		= ((float)(rand()%80-40)/20);
			ySpeed		= -((float)(rand()%40+40)/20);
			yAcc		= 1.0f;

			BounceWalls = true;
			Lebensdauer = 75;
			PartikelArt = WATERFLUSH;
			Rotate = true;

		} break;

		case WATERFLUSH_HIGH:	// Wasserminen Dampfwolken
		{
			static int count = 0;

			if (count == 0)
			{
				red   = 255;
				green = 255;				
				blue  = 255;
				count++;
			}
			else
			{
				count = 0;
				red   = pTileEngine->ColR3;
				green = pTileEngine->ColG3;
				blue  = pTileEngine->ColB3;
			}						

			xSpeed		=  ((float)(rand()%80-40)/2.0f);
			ySpeed		= -((float)(rand()%100+100)/5.0f);
			yAcc		= 4.0f;

			Lebensdauer = 128;
			PartikelArt = SNOWFLUSH;
			Rotate = true;

		} break;

		case WATERFLUSH_HIGH2:	// Wasserspritzer Dampfwolken
		{
			static int count = 0;

			if (count == 0)
			{
				red   = 255;
				green = 255;				
				blue  = 255;
				count++;
			}
			else
			{
				count = 0;
				red   = pTileEngine->ColR3;
				green = pTileEngine->ColG3;
				blue  = pTileEngine->ColB3;
			}						

			xSpeed		=  ((float)(rand()%40-20)/4.0f);
			ySpeed		= -((float)(rand()%50+50)/3.0f);
			yAcc		= 6.0f;

			Lebensdauer = 128;
			PartikelArt = SNOWFLUSH;
			Rotate = true;

		} break;

		case UFOLASERFLARE:	// Flare des Ufo Lasers
		{
			red	= 255; green = 64;	blue = 224; alpha = 255;
			Lebensdauer = 255;
		} break;		

		case FUNKE :			// Roter Funken
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;
			xSpeed		= ((float)(rand()%80-40)/5);
			ySpeed		= -(float)(rand()%40+20)/5;
			yAcc		= 5.0f;
			BounceWalls = true;

			Lebensdauer = 255;
			
		} break;

		case FUNKE2 :			// Grüner Funken
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;
			xSpeed		= ((float)(rand()%80-40)/5);
			ySpeed		= -(float)(rand()%40+20)/5;
			yAcc		= 5.0f;
			BounceWalls = true;

			Lebensdauer = 255;
			
		} break;

		case LASERFUNKE :		// Laser-Funken blau
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;
			xSpeed		= ((float)(rand()%80-40)/2);
			ySpeed		= -(float)(rand()%40+20)/5;
			yAcc		= 5.0f;
			BounceWalls = true;

			Lebensdauer = 255;
			
		} break;

		case LASERFUNKE2 :		// Laser-Funken rot
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;
			xSpeed		= ((float)(rand()%20-10)/2);
			ySpeed		= -(float)(rand()%40+20)/5;
			yAcc		= 5.0f;
			BounceWalls = true;
			AnimEnde	= 3;
			AnimSpeed   = 0.75f;

			Lebensdauer = 255;
			
		} break;		

		case PHARAOSMOKE :		// Roter Rauch für den Pharaokopf schuss
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;
			xSpeed		= ((float)(rand()%80-40)/40);
			ySpeed		= -(float)(rand()%80-40)/40;
			Lebensdauer = 255;
			
		} break;

		case ROCKETSMOKE :		// Rauch einer Rakete
		case ROCKETSMOKEBLUE :	// auch in blau =)
		case ROCKETSMOKEGREEN :	// und das selbe nochmal in grün :D
		{
			red	= 255; green = 255;	blue = 255;	alpha = 150;
			xSpeed		= ((float)(rand()%40-20)*0.025f);
			ySpeed		= -(float)(rand()%40-20)*0.025f;
			Lebensdauer = 200;
			
		} break;

		case FLUGSACKSMOKE :	// Rauch des FlugSacks links
		{
			red	= 255; green = 255;	blue = 255;	alpha = 128;
			xSpeed		= - (8.0f + ((float)(rand()%20)/10));
			ySpeed		= 15.0f;
			Lebensdauer = float(150 + rand()%10);

			AnimSpeed = 0.8f;
			AnimEnde  = 16;

			BounceWalls = true;
		} break;

		case FLUGSACKSMOKE2 :	// Rauch des FlugSacks rechts
		{
			PartikelArt = FLUGSACKSMOKE;
			red	= 255; green = 255;	blue = 255;	alpha = 128;
			xSpeed		= 8.0f + ((float)(rand()%20)/10);
			ySpeed		= 15.0f;
			Lebensdauer = float(150 + rand()%10);

			AnimSpeed = 0.8f;
			AnimEnde  = 16;
			BounceWalls = true;
		} break;

		case ROBOMANSMOKE :	// Rauch des RoboMans
		{
			PartikelArt = FLUGSACKSMOKE;
			red	= 255; green = 255;	blue = 255;	alpha = 128;
			ySpeed		= 15.0f + ((float)(rand()%20)/5);
			Lebensdauer = float(150 + rand()%10);

			AnimSpeed = 0.8f;
			AnimEnde  = 16;
			BounceWalls = true;
		} break;

		case STELZFLARE :		// Flare des Stelzsack smokes
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;
			Lebensdauer = 255;
			BounceWalls = false;
		} break;

		case TEXTSECRET:
		{
			Lebensdauer = 255;
			OwnDraw = true;
			ySpeed = -1.0f;
		} break;

		case KRINGELSECRET :	// Secret Kringel
		{
			red	= 255; green = 224;	blue = 32; alpha = 255;
			AnimPhase = rand()%4;
			Lebensdauer = 255;
			BounceWalls = false;

			float mul = (float)(rand()%60 + 40) / 8.0f;
			float arc = (float)(rand()%360) * PI / 180.0f;

			xSpeed		= (float)(sin(arc) * mul);
			ySpeed		= (float)(cos(arc) * mul);

			xPos += xSpeed;
			yPos += ySpeed;
		} break;

		case KRINGELR :			// Roter Kringel
		{
			red	= 255; green = 64;	blue = 0; alpha = 255;
			AnimPhase = rand()%4;
			Lebensdauer = 255;
			BounceWalls = false;
			PartikelArt = KRINGEL;

			double	absx, absy, speed;				// Variablen für die Geschwindigkeits-
													// berechnung
			absx = m_pParent->xpos+35-(xPos+4);		// Differenz der x 
			absy = m_pParent->ypos+40-(yPos+4);		// und y Strecke
				
			speed = 1/sqrt(absx*absx + absy*absy);	// Länge der Strecke berechnen
			speed = speed*(8+AnimPhase);			// Geschwindigkeit ist 4 fach

			absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
			absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

			xSpeed = float(absx);
			ySpeed = float(absy);
		} break;

		case KRINGELG :			// Grüner Kringel
		{
			red	= 64; green = 255;	blue = 0; alpha = 255;
			AnimPhase = rand()%4;
			Lebensdauer = 255;
			BounceWalls = false;
			PartikelArt = KRINGEL;

			double	absx, absy, speed;				// Variablen für die Geschwindigkeits-
													// berechnung
			absx = m_pParent->xpos+35-(xPos+4);		// Differenz der x 
			absy = m_pParent->ypos+40-(yPos+4);		// und y Strecke
				
			speed = 1/sqrt(absx*absx + absy*absy);	// Länge der Strecke berechnen
			speed = speed*(8+AnimPhase);			// Geschwindigkeit ist 4 fach

			absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
			absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

			xSpeed = float(absx);
			ySpeed = float(absy);
		} break;

		case KRINGELB :			// Blauer Kringel
		{
			red	= 24; green = 48;	blue = 255; alpha = 255;
			AnimPhase = rand()%4;
			Lebensdauer = 255;
			BounceWalls = false;
			PartikelArt = KRINGEL;

			double	absx, absy, speed;				// Variablen für die Geschwindigkeits-
													// berechnung
			absx = m_pParent->xpos+35-(xPos+4);		// Differenz der x 
			absy = m_pParent->ypos+40-(yPos+4);		// und y Strecke
				
			speed = 1/sqrt(absx*absx + absy*absy);	// Länge der Strecke berechnen
			speed = speed*(8+AnimPhase);			// Geschwindigkeit ist 4 fach

			absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
			absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

			xSpeed = float(absx);
			ySpeed = float(absy);
		} break;

		case KRINGELHB :		// Hellblauer Kringel
		{
			red	= 64; green = 192;	blue = 255; alpha = 255;
			AnimPhase = rand()%4;
			Lebensdauer = 255;
			BounceWalls = false;
			PartikelArt = KRINGEL;

			double	absx, absy, speed;				// Variablen für die Geschwindigkeits-
													// berechnung
			absx = m_pParent->xpos+35-(xPos+4);		// Differenz der x 
			absy = m_pParent->ypos+40-(yPos+4);		// und y Strecke
				
			speed = 1/sqrt(absx*absx + absy*absy);	// Länge der Strecke berechnen
			speed = speed*(8+AnimPhase);			// Geschwindigkeit ist 4 fach

			absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
			absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

			xSpeed = float(absx);
			ySpeed = float(absy);
		} break;

		case LONGFUNKE :		// Langer Roter Funken
		{
			red	= 255; green = 192;	blue = 24;	alpha = 255;
			xSpeed		= ((float)(rand()%100-50)/2);
			ySpeed		= -(float)(rand()%40+40)/2;
			yAcc		= 5.0f;
			BounceWalls = true;
			OwnDraw		= true;

			Lebensdauer = 255;
			
		} break;

		case WATERFUNKE :		// Wasserspritzer
		{
			red	= 255; green = 192;	blue = 24;	alpha = 255;

			xSpeed		= ((float)(rand()%50-25)*0.4f);
			ySpeed		= -(float)(rand()%30+60)*0.15f;
			yAcc		= 5.0f;
			BounceWalls = true;
			OwnDraw		= true;

			Lebensdauer = 255;
			
		} break;

		case EVILFUNKE:			// Funke des Evil Blitzes
		{
			red	= 255; green = 255;	blue = 255;	alpha = 128;
			ySpeed		= 8.0f + ((float)(rand()%20)/10);
			Lebensdauer = float(150 + rand()%10);

			BounceWalls = true;
		} break;

		case WASSERTROPFEN :	// Tropfen
		{
			red	= 230; green = 240;	blue = 255;	alpha = 128;

			Lebensdauer = 128;
			ySpeed	    = - float(rand()%10 + 2) / 2.0f;
			xSpeed	    =   float(rand()%20 - 10) / 2.0f;
			yAcc		= 4.0f;						
		} break;

		case BUBBLE :		// Luftbläschen im Wasser
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;
			xSpeed =  0.0f;

			if (rand()%2 == 0)
				xAcc = 0.1f;
			else
				xAcc = -0.1f;

			AnimCount = float(rand()%10 + 10) / 10.0f;

			ySpeed		= -(float)(rand()%10+20)/10;

			if (WinkelUebergabe == -1.0f)
			{
				ySpeed *= -3.0f;
				yAcc = -1.0f;
			}

			Lebensdauer = 255;		
			RemoveWhenOffScreen = false;
		} break;		

		case LASERFLAME :			// Leuchteffekt für den Krabbler
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;
			Lebensdauer = 255.0f;
			
		} break;

		case LASERFLAMEPHARAO :		// Leuchteffekt für den Pharao
		{
			red	= 255; green = 64;	blue = 64;	alpha = 255;
			Lebensdauer = 192.0f;
			PartikelArt = LASERFLAME;
			
		} break;

		case SHIELD :				// Schutzschild
		{
			// Schild farbliche an die aktuelle Stärke anpassen
			// Schild voll == hellblau, Schild leer == rot
			float amount = m_pParent->Shield;

			if (amount > 100.0f)
				amount = 100.0f;

			red   = 255 - int(amount*2.55f);
			blue  = 	  int(amount*2.55f);
			green =	0;

			if (red < 0)	 red = 0;
			if (red > 255)	 red = 255;
			if (blue < 0)    blue = 0;
			if (blue > 255)  blue = 255;
			green = int (blue / 2);


			alpha = 128;
			Lebensdauer = 128.0f;

			// Offset zum Spieler merken
			xSpeed = m_pParent->xpos - xPos;
			ySpeed = m_pParent->ypos - yPos;
			
		} break;		

		case TURBINESMOKE :			// Partikel für die Turbine des Metalhead Bosses
		{
			red	= 255; green = 255;	blue = 255; alpha = 255;
			AnimPhase = rand()%3;
			Lebensdauer = 255;
			BounceWalls = false;

			double	absx, absy, speed;				// Variablen für die Geschwindigkeits-
													// berechnung
			absx = pPartikelSystem->xtarget-xPos;	// Differenz der x 
			absy = pPartikelSystem->ytarget-yPos;	// und y Strecke
				
			speed = 1/sqrt(absx*absx + absy*absy);	// Länge der Strecke berechnen
			speed = speed*(8+AnimPhase);			// Geschwindigkeit ist 4 fach

			absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
			absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

			xSpeed = float(absx);
			ySpeed = float(absy);
		} break;

		case MINIFLARE :							// Flare vom Lava Ball
		{
			red	= 255; green = 224;	blue = 192; alpha = 192;
			Lebensdauer = 192;
			BounceWalls = false;

			ySpeed = 2.0f;
			yAcc   = 2.0f;
			xSpeed = float (rand()%10 + 10) / 2.0f;
			ySpeed = - float (rand()%10 + 10);

			if (rand()%2 == 0)
				xSpeed *= -1;
		} break;

		case GRENADEFLARE :							// Aufleuchten bei Granaten Treffer
		{
			red	= 255; green = 224;	blue = 192; alpha = 255;
			Lebensdauer = 255;
			Rotate = true;
		} break;

		case EXPLOSIONFLARE :						// Aufleuchten bei Explosion
		{
			red	= 255; green = 224;	blue = 112; alpha = 192;
			Lebensdauer = 192;
		} break;

		case EXPLOSIONFLARE2 :						// Aufleuchten bei Explosion Lila
		{
			red	= 255; green = 128;	blue = 255; alpha = 192;
			Lebensdauer = 192;
		} break;
		
		case SCHLEIM:			// kleiner Schleimbollen
		case SCHLEIM2:
		{
			red	= 255; green = 255;	blue = 255;	alpha = 224;
			xSpeed		= ((float)(rand()%80-40)/3);
			ySpeed		= -(float)(rand()%40+40)/3;
			yAcc		= 5.0f;
			BounceWalls = true;
			AnimEnde    = 4;
			AnimSpeed	= float((rand()%2+3)/10.0f);

			Lebensdauer = 255;
		} break;

		case SHOCKEXPLOSION:
		{
			red	= 255; green = 192;	blue = 64;	alpha = 255;
			AnimCount = 4.0f;
			Lebensdauer = 255;
			OwnDraw = true;
		} break;

		case SHOTFLARE:
		{
			red	= 255; green = 128;	blue = 32;	alpha = 255;
			Lebensdauer = 255;
		} break;

		case SHOTFLARE2:
		{
			red	= 32; green = 255;	blue = 32;	alpha = 255;
			Lebensdauer = 255;
		} break;

		case EXTRACOLLECTED:
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;
			Lebensdauer = 255;

			OwnDraw = true;
			AnimCount = 1.0f;
			AnimPhase = 1;

		} break;

		case EXPLOSION_KRINGEL:		// Kringel/Druckwelle bei Explosion
		{
			red	= 255; green = 240;	blue = 128;	alpha = 255;
			Lebensdauer = 255;

			OwnDraw = true;
			AnimCount = 5.0f;
			AnimPhase = 2;

			if (WinkelUebergabe == -1.0f)
				AnimPhase = 2;

			PartikelArt = EXTRACOLLECTED;

		} break;

		case DIAMANTCOLLECTED:
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;
			Lebensdauer = 255;

			OwnDraw = true;
			AnimCount = 16.0f;

		} break;

		case LILA:
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;
			Lebensdauer = 255;
		} break;

		case LILA2:
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;
			Lebensdauer = 255;

			ySpeed = -(float)(rand()%30 + 10) / 2.0f;
			xSpeed =  (float)(rand()%40 - 20) / 2.0f;
			yAcc   = 5.0f;
			PartikelArt = LILA;
		} break;

		case LILA3:
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;
			Lebensdauer = 255;

			xSpeed = -(float)(rand()%20 + 120);
			PartikelArt = LILA;
		} break;

		case DRACHE_SMOKE:
		{
			red	= 255; green = 255;	blue = 255;	alpha = 128;
			Lebensdauer = 255;
			xSpeed = -(float)(rand()%20 + 120) / 4.0f;
			ySpeed = (float)(rand()%20 - 10) / 5.0f;

			if (WinkelUebergabe != 0.0f)
				xSpeed *= -1;

			Rotate = true;
		} break;

		case FIREBALL_SMOKE:
		{
			red	= 255; green = 255;	blue = 255;	alpha = 128;
			Lebensdauer = 255;
			xSpeed = (float)(rand()%10 - 5) / 10.0f;
			ySpeed = (float)(rand()%10 - 5) / 10.0f;

			Rotate = true;
		} break;


		case LASERFLARE:
		{
			red	= 32; green = 192;	blue = 255;	alpha = 255;
			Lebensdauer = 255;
		} break;		

		case EXPLOSION_TRACE_END:
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 150;
			AnimEnde	= 17;
			AnimPhase	= 5;
			AnimSpeed	= 3.0f;

			blue = (int)WinkelUebergabe;
			OwnDraw = true;

			Rotate = true;

		} break;

		case LAVADUST:
		{
			red	= 255; green = 128;	blue = 64; alpha = 255;

			if (rand()%2 == 0)
				xAcc = -0.4f;
			else
				xAcc = -0.4f;

			Lebensdauer = 255;
			AnimPhase = rand()%3;

			ySpeed = -3.0f * (AnimPhase + 1);
			RemoveWhenOffScreen = false;
		} break;

		case EXPLOSION_TRACE:
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 255;

			xPos -= 60;
			yPos -= 60;
			
			xSpeed		= ((float)(rand()%80-40));
			ySpeed		= -(float)(rand()%10+50);
			yAcc		= 8.0f;
			
		} break;		

		case EXPLOSION_REGULAR:
		{
			red	= 255; green = 255;	blue = 255;	alpha = 255;

			Lebensdauer = 128;
			AnimEnde	= 17;
			AnimSpeed	= 0.75f;

			Rotate = true;			
		} break;		

		default :
		break;
	} // switch >= Additiv Grenze

	return true;
}

// --------------------------------------------------------------------------------------
// Partikel animieren und bewegen
// --------------------------------------------------------------------------------------

void PartikelClass::Run(void)
{
	int bo, bu, bl, br;

	// Screen verlassen oder eh schon weg wegen der Lebensdauer ?
	//
	if (RemoveWhenOffScreen == true)
	{
		if (yPos-pTileEngine->YOffset > 480 + 20 ||
			yPos-pTileEngine->YOffset+PartikelRect[PartikelArt].bottom < 20 ||
			xPos-pTileEngine->XOffset > 640 + 20 ||
			xPos-pTileEngine->XOffset+PartikelRect[PartikelArt].right < 20)
			Lebensdauer = 0.0f;
	}

	// wird diesen Frame gelöscht? Dann nicht mehr animieren und rendern
	//
	if (Lebensdauer <= 0.0f)
		return;

	// Bewegen (nur wenn sichtbar)
	if (alpha > 0)
	{
		xSpeed += xAcc   SYNC;
		ySpeed += yAcc   SYNC;
		xPos   += xSpeed SYNC;
		yPos   += ySpeed SYNC;
	}

	if (BounceWalls == true)
	{
		bo = pTileEngine->BlockOben		   (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
		bl = pTileEngine->BlockLinks	   (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
		br = pTileEngine->BlockRechts	   (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
		bu = pTileEngine->BlockUnten       (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
	}

	else
	{
		bo = bl = br = 0;
		bu = pTileEngine->BlockUntenNormal (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
	}

	// Auf Kollision mit dem Level Testen
	if (BounceWalls == true)
	{
		// Testen,ob der Partikel sich in einer Wand verfangen hat, wenn ja
		// dann verschwinden lassen
	
		if((bo & BLOCKWERT_WAND && bu & BLOCKWERT_WAND && bl & BLOCKWERT_WAND) ||
		   (bo & BLOCKWERT_WAND && bu & BLOCKWERT_WAND && br & BLOCKWERT_WAND))
		    Lebensdauer = 0.0f;

		// An der Decke abgeprallt?
		if (ySpeed < 0.0f &&
			bo & BLOCKWERT_WAND) 
			ySpeed *= -1.0f;

		// Am Boden abgesprungen?
		if (ySpeed > 0.0f &&
			(bu & BLOCKWERT_WAND ||
			 bu & BLOCKWERT_PLATTFORM))
		{	
			ySpeed = -ySpeed/2.0f;	

			if (ySpeed > -0.5f)
			{				
				ySpeed  = 0.0f;
				xSpeed *= 0.99f;
			}			
		}

		if ((xSpeed > 0.0f && br & BLOCKWERT_WAND) ||
			(xSpeed < 0.0f && bl & BLOCKWERT_WAND))
		xSpeed *= -1.0f;
	}

	// Animieren
	if (AnimEnde > 0)						// Soll überhaupt anmiert werden ?
	{
		AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
		if (AnimCount > AnimSpeed)			// Grenze überschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen
			AnimPhase++;					// Und nächste Animationsphase
		}
	}

//----- Verschiedene Partikel unterschiedlich abhandeln
// Zwei Case Anweisungen, einmal für nicht additive und einmal für additive Partikel
// Wird dadurch im Schnitt hoffentlich doppelt so schnell ;)

	if (PartikelArt < ADDITIV_GRENZE)
	switch(PartikelArt)
	{

		case GLASSPLITTER:
		{
			Rot += xSpeed * 10.0f SYNC;

			Lebensdauer -= 20.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
		} break;
		
		case BULLET_SKELETOR:
		case BULLET:						// Patronenhülse
		{
			bo = pTileEngine->BlockOben		   (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
			bl = pTileEngine->BlockLinks	   (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
			br = pTileEngine->BlockRechts	   (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
			bu = pTileEngine->BlockUnten       (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);

			// Testen,ob der Partikel sich in einer Wand verfangen hat, wenn ja
			// dann verschwinden lassen
		
			if((bo & BLOCKWERT_WAND && bu & BLOCKWERT_WAND && bl & BLOCKWERT_WAND) ||
			   (bo & BLOCKWERT_WAND && bu & BLOCKWERT_WAND && br & BLOCKWERT_WAND))
				Lebensdauer = 0.0f;

			// An der Decke abgeprallt?
			if (ySpeed < 0.0f &&
				bo & BLOCKWERT_WAND) 
				ySpeed *= -1.0f;

			// Am Boden abgesprungen?
			if (ySpeed > 0.0f &&
				(bu & BLOCKWERT_WAND ||
				 bu & BLOCKWERT_PLATTFORM))
			{	
				ySpeed = -ySpeed/2.0f;	
				xSpeed *= 0.5f;

				if (ySpeed > -0.5f)
				{				
					yAcc = 0.0f;
					ySpeed = 0.0f;
					xSpeed = 0.0f;
				}
			}

			if ((xSpeed > 0.0f && br & BLOCKWERT_WAND) ||
				(xSpeed < 0.0f && bl & BLOCKWERT_WAND))
				xSpeed *= -1.0f;

			Rot += xSpeed * 10.0f SYNC;

			Lebensdauer -= 5.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

		} break;
		
		case EXPLOSION_GREEN:				// Mittlere Grüne Explosion
		case EXPLOSION_ALIEN:				// Mittlere Alien Explosion
		case EXPLOSION_MEDIUM:				// Mittlere Explosion
		case EXPLOSION_MEDIUM2:				// Mittlere Explosion Nr 2
		case EXPLOSION_MEDIUM3:				// Mittlere Explosion Nr 3
		case EXPLOSION_GIGA:				// Riesen fette Explosion
		case EXPLOSION_BIG:					// Grosse Explosion
		case EXPLOSION_GIANT:					// Grosse Explosion		
		{
			Lebensdauer -= 5.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
			if (AnimPhase >= AnimEnde)		// Animation zu Ende	?
				Lebensdauer = 0;			// Dann Explosion verschwinden lassen

			if (Rotate == true)
				Rot += RotDir * AnimSpeed * 10.0f SYNC;

		} break;		

		case BLUE_EXPLOSION:				// Kleine blaue Explosion
		{
			Lebensdauer -= 12.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
			if (AnimPhase >= AnimEnde)		// Animation zu Ende	?
				Lebensdauer = 0;			// Dann Explosion verschwinden lassen

		} break;

		case SPLITTER:						// Kleiner animierter Splitter
		{
			Lebensdauer -= 7.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
			if (AnimPhase == AnimEnde)		// Animation zu Ende	?
				AnimPhase = 0;				// Dann von vorne beginnen lassen

			if (AnimCount == 0.0f)
				pPartikelSystem->PushPartikel(xPos - 12, yPos - 14, SMOKE);

		} break;

		case HURRITEILE:					// Teile des explodierten Hurris
		{
			Lebensdauer -= 5.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			// Partikel drehen
			Rot   += (float)(AnimPhase+1) * 8.0f SYNC;

		} break;

		case LAVAKRABBE_KOPF:				// Teile der explodierten Lavakrabbe
		{
			Lebensdauer -= 10.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			// Partikel drehen
			Rot   += 32.0f SYNC;

		} break;

		case KETTENTEILE:
		case KETTENTEILE2:
		case KETTENTEILE3:
		case KETTENTEILE4:
		{
			Lebensdauer -= 10.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			// Partikel drehen
			Rot += 40.0f SYNC;

		} break;	

		case SPIDERPARTS:				// Spinnenstücke auf dem Fliessband
		{
			bo = pTileEngine->BlockOben		   (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
			bl = pTileEngine->BlockLinks	   (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
			br = pTileEngine->BlockRechts	   (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
			bu = pTileEngine->BlockUntenNormal (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);

			// an der Wand abprallen oder aufhören und verschwinden
			if (bl & BLOCKWERT_WAND ||
				br & BLOCKWERT_WAND)
			{
				// abprallen
				if (ySpeed > 0.0f)
				{
					if (bl & BLOCKWERT_WAND)
						xSpeed = 11.0f;
					else
						xSpeed = -11.0f;
				}			
				
				// oder verschwinden
				else
					Lebensdauer = 0.0f;
			}				

			// auf dem Fliessband bewegen
			if (bu & BLOCKWERT_FLIESSBANDL)
				xSpeed = -11.0f;

			if (bu & BLOCKWERT_FLIESSBANDR)
				xSpeed = 11.0f;

			// runterfallen?
			if (!(bu & BLOCKWERT_WAND))
				yAcc = 5.0f;
			else
			{
				yAcc	= 0.0f;
				ySpeed	= 0.0f;
			}

			// unten aus dem Screen raus? Dann verschwinden lassen
			if (yPos > pTileEngine->YOffset + 500)
				Lebensdauer = 0.0f;

		} break;

		case PIRANHATEILE  :					// Teile eines kaputten Piranhas
		case PIRANHATEILE2 :					// Teile eines kaputten Riesen Piranhas
		{
			bo = pTileEngine->BlockOben (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);

			if (ySpeed < -2.0f)
				ySpeed = -2.0f;

			if (ySpeed == 0.0f)				// an der Oberfläche langsam ausfaden lassen
				Lebensdauer -= 7.0f SYNC;	
			alpha		 = (int)Lebensdauer;

			if ((xAcc > 0.0f && xSpeed > 0.0f) ||
				(xAcc < 0.0f && xSpeed < 0.0f))
			{
				xAcc   = 0.0f;
				xSpeed = 0.0f;
			}


			// An der Wand Seitwärtsbewegung einstellen
			if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND)
			{
				xSpeed = 0.0f;
				xAcc   = 0.0f;
			}

			// An der Decke oder der Oberfläche Aufwärtsbewegung einstellen
			if (!(bo & BLOCKWERT_LIQUID))
			{
				xSpeed = 0.0f;
				ySpeed = 0.0f;
				yAcc   = 0.0f;
			}

		} break;

		case PIRANHABLUT :					// Blut eines kaputten Piranhas
		{
			bo = pTileEngine->BlockOben (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);

			Lebensdauer -= 5.0f SYNC;	// langsam ausfaden lassen
			alpha = (int)Lebensdauer;

			// Nicht mehr im Wasser ? Dann gleich verschwinden lassen
			if(!(bo & BLOCKWERT_LIQUID))
				Lebensdauer = 0;

			// Animation von vorne beginnen
			if (AnimPhase == AnimEnde)
				AnimPhase = 0;

			Rot += 5.0f SYNC;
		} break;

		case MADEBLUT :					// Blut einer kaputten Made
		{
			Lebensdauer -= 15.0f SYNC;	// ausfaden lassen
			alpha = (int)Lebensdauer;

			// Animation von vorne beginnen
			if (AnimPhase == AnimEnde)
				AnimPhase = 0;
		} break;

		case SPAWNDROP :				// Wassertropfen aus dem Spawner
		{
			if (ySpeed > 30.0f)
				yAcc = 0.0f;

			// Im Wasser oder auf dem Boden gelandet ?
			//
			int blocku = pTileEngine->BlockUnten (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);

			if (blocku & BLOCKWERT_WAND		||
				blocku & BLOCKWERT_PLATTFORM ||
				blocku & BLOCKWERT_LIQUID)
			{
				Lebensdauer = 0;

				for (int i = 0; i < 3; i++)
					pPartikelSystem->PushPartikel(xPos, yPos, WATERFUNKE);

				// Platschen im Wasser
				//
				int   vol;
				float xdiff, ydiff, Abstand;

				xdiff = ((m_pParent->xpos + 45)  - xPos);
				ydiff = ((m_pParent->ypos + 45)  - yPos);

				Abstand = float(sqrt((xdiff * xdiff) + (ydiff * ydiff)));

				vol = int((100-float(Abstand/6.0f)) * 1.5f);
				if (vol < 0)
					vol = 0;

				if (pSoundManager->its_Sounds[SOUND_DROP]->isPlaying == false)
					pSoundManager->PlayWave (vol, 128, 6000 + rand ()%6000, SOUND_DROP);
			}

		} break;

		case BLATT:
		{
			if (ySpeed > 6.0f)
				ySpeed = 6.0f;

			int bu = pTileEngine->BlockUntenNormal (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);

			if (bu & BLOCKWERT_WAND ||bu & BLOCKWERT_PLATTFORM)
			{
				xSpeed   = 0.0f;
				xAcc     = 0.0f;

				Lebensdauer -= 10.0f SYNC;
				alpha = int (Lebensdauer);
			}

			if (xSpeed > 0.0f)
				xAcc = -0.5f;
			else
				xAcc = 0.5f;

			Rot += RotDir * ySpeed * 8.0f SYNC;

			Lebensdauer -= 5.0f SYNC;
			alpha = int (Lebensdauer);

		} break;

		case BLATT2:
		{
			Rot += -xSpeed/10.0f * RotDir * ySpeed * 8.0f SYNC;
		} break;

		case NESTLUFT:
		{
			Lebensdauer -= 5.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			// drehen
			if (yAcc != 0.0f)
				Rot += RotDir;

			while(Rot > 360.0f)
				  Rot -= 360.0f;

			int bu = pTileEngine->BlockUnten (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);

			if (yAcc != 0.0f &&
				(bu & BLOCKWERT_WAND ||
				 bu & BLOCKWERT_PLATTFORM))
			{
				ySpeed = -ySpeed / 1.5f;

				if (ySpeed > -1.0f)
				{
					xSpeed = 0.0f;
					ySpeed = 0.0f;
					yAcc   = 0.0f;
				}
			}

		} break;

		case FOG:							// Nebel (fadet ein und aus)
		{
			// Nebel faden
			//
			AnimCount += 4.0f SYNC;

			// Fadet ein ?
			//
			if (AnimCount < 128.0f)	
				alpha = int (AnimCount);

			// oder aus ?
			else   				
				alpha = 256 - int (AnimCount);

			if (AnimCount >= 256.0f)
				Lebensdauer = 0;
		} break;

		case ROCKSPLITTER:					// Splitter eines Felsblocks
		case ROCKSPLITTERSMALL:		
		case ROCKSPLITTERSMALLBLUE:		
		case BOULDER_SMALL:
		{
			if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
				AnimPhase = 0;				// Dann wieder von vorne beginnen

			Lebensdauer -= 6.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
		} break;

		case WASSER_SPRITZER:
		{
			Lebensdauer -= (float)(AnimSpeed) SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
			OwnDraw		 = true;
		} break;

		case WASSER_SPRITZER2:
		{
			Lebensdauer -= (float)(AnimSpeed) SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
			OwnDraw		 = true;
		} break;

		case SPIDERSPLITTER:
		case SPIDERSPLITTER2:
		{
			if (AnimPhase == AnimEnde)		// Animation von zu Ende	?
				AnimPhase = 0;				// Dann wieder von vorne beginnen

			Lebensdauer -= 6.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
		} break;

		case SPIDERGRENADE:
		{
			if (AnimPhase == AnimEnde)		// Animation von zu Ende	?
				AnimPhase = 0;				// Dann wieder von vorne beginnen

			// Am Boden aufgetroffen ? Dann Feuerwalze erzeugen
			if (bu & BLOCKWERT_WAND)
			{
				Lebensdauer = 0;
//				pProjectiles->PushProjectile(xPos - 30, yPos, FLAMEWALL);
			}
		} break;

		case EVILSMOKE:
		case EVILSMOKE2:
		case EVILROUNDSMOKE:
		{
			Lebensdauer -= 25.0f SYNC;		// schnell ausfaden lassen
			alpha		 = (int)Lebensdauer;
		} break;

		case STELZE:						// Stelze eines StelzSacks
		{
			if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
				AnimPhase = 0;

			Rot += xSpeed * 2.0f SYNC;
		} break;
				
		case STELZHEAD:
		{
			AnimCount -= 1.0f SYNC;

			if (AnimCount < 0.0f)
			{
				AnimCount = 0.5f;
				pPartikelSystem->PushPartikel (xPos - 20 + rand()%40, yPos - 20 + rand()%40, EXPLOSION_MEDIUM2);
			}
		} break;						

		case SMOKEBIG:						// Riesen Rauch
		{
			Lebensdauer -= 4.5f SYNC;		// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			Rot += RotDir * ySpeed * 3.0f SYNC;
			
		} break;

		case SMOKEBIG2:						// Riesen Rauch
		{
			Lebensdauer -= 4.5f SYNC;		// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			Rot += RotDir * ySpeed SYNC;

			if (ySpeed < -5.0f)
			{
				yAcc = 0.0f;
				ySpeed = -5.0f;
			}

			if (xSpeed > 0.0)
				xSpeed -= 3.0f SYNC;

			if (xSpeed < 0.0f)
				xSpeed += 3.0f SYNC;
			
		} break;

		case SMOKEBIG_OUTTRO:				// Riesen Rauch
		{
			Lebensdauer -= 1.0f SYNC;		// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			Rot += RotDir * ySpeed * 1.0f SYNC;
			
		} break;

		case SMOKE:							// Rauchwolke
		{
			Lebensdauer -= 30.0f SYNC;		// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			Rot += RotDir * ySpeed * 5.0f SYNC;
		} break;

		case SMOKE2:						// Rauchwolke bei LavaBall zb
		case SMOKE3:
		{
			Lebensdauer -= 4.5f SYNC;		// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			Rot += RotDir * ySpeed * 3.0f SYNC;

			// Rauch im Wasser ? Dann zur Blubberblase werden lassen
			//
			if (pTileEngine->Tiles[(int)((xPos + 5) / TILESIZE_X)][(int)((yPos + 5) / TILESIZE_Y)].Block & BLOCKWERT_LIQUID)				
			{
				int off = 0;

				if (PartikelArt == SMOKE2)
					off = 7;

				Lebensdauer = 0;
				pPartikelSystem->PushPartikel (xPos + 5, yPos + off, BUBBLE);
			}
		} break;		

		case REGENTROPFEN:							// Verschwindet wenn Wand oder Boden getroffen
		{
			// Aus dem Level rausgeflogen ?
			if (yPos > pTileEngine->LEVELPIXELSIZE_Y)
				Lebensdauer = 0;

			// Auf den Boden gekommen oder an die Wand geklatscht ? Dann verschwinden zerplatzen
			if (bl & BLOCKWERT_WAND   ||
				br & BLOCKWERT_WAND   ||
				bu & BLOCKWERT_WAND   ||
				bu & BLOCKWERT_LIQUID ||
				bu & BLOCKWERT_PLATTFORM)
			{
				Lebensdauer = 0;

				// Ein paar Spritzer erzeugen
				if (options_Detail >= DETAIL_HIGH)
				for (int i=0; i<2; i++)
					pPartikelSystem->PushPartikel(xPos+8, yPos+16, WASSERTROPFEN);
			}			

		} break;		

		case HALSWIRBEL:
		case KAPUTTETURBINE:
		{
			Lebensdauer -= 5.0f SYNC;		// schnell ausfaden lassen
			alpha		 = (int)Lebensdauer;
			AnimCount   += 0.8f SYNC;

			while (AnimCount > 2 * PI)
				   AnimCount -= 2 * PI;
		} break;

		case DUST:		
		{
			Lebensdauer -= 1.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			// Richtung umdrehen ?
			if (ySpeed < -4.0f)
				yAcc = 0.8f;

			if (ySpeed >  4.0f)
				yAcc = -0.8f;
		} break;

		case SCHROTT1:
		case SCHROTT2:
		{
			Lebensdauer -= 5.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;			

			if (AnimPhase >= AnimEnde)
				AnimPhase = 0;
		} break;

		case SCHNEEFLOCKE:					// Verschwindet am unteren Level-Rand
		{
			// Richtung umdrehen ?
			if (xSpeed < -2.0f)
				xAcc = 0.2f;

			if (xSpeed >  2.0f)
				xAcc = -0.2f;

			// Aus dem Level rausgeflogen ?
			if (yPos > pTileEngine->LEVELPIXELSIZE_Y)
				Lebensdauer = 0;

			// Auf den Boden gekommen ? Dann langsam ausfaden
			if (bu & BLOCKWERT_WAND   ||
				bu & BLOCKWERT_LIQUID ||
				bu & BLOCKWERT_PLATTFORM)
			{
				xSpeed = 0.0f;
				ySpeed = 0.0f;
				xAcc = 0.0f;
				yAcc = 0.0f;
			}

			// Ausfaden am Boden
			if (ySpeed == 0.0f)
			{
				Lebensdauer -= float(10.0 SYNC);
				alpha = int(Lebensdauer);
			}

		} break;		

		case SCHNEEFLOCKE_END:					// Verschwindet am unteren Level-Rand
		{
			// Richtung umdrehen ?
			if (xSpeed < -2.0f)
				xAcc = 0.1f;

			if (xSpeed >  2.0f)
				xAcc = -0.1f;

			// Unten aus dem Screen raus? Dann verschwinden lassen			
			if (yPos > 480.0f)
				Lebensdauer = 0.0f;
		} break;		

		default : break;
	} // switch < Additiv Grenze

	else	

	switch (PartikelArt)
	{
		// Rauch des Blitzbeams, wird langsam größer

		case BEAMSMOKE:
		{
			Lebensdauer -= 15.0f SYNC;		// langsam
			alpha		 = (int)Lebensdauer;

			AnimCount += 3.0f SYNC;

		} break;

		
		case BEAMSMOKE3:
		case BEAMSMOKE4:
		{
			Rot += RotDir SYNC;
			Lebensdauer -= 10.0f SYNC;		// langsam
			alpha		 = (int)Lebensdauer;
		} break;

		case BEAMSMOKE5:
		{
			Lebensdauer -= AnimPhase SYNC;		// langsam
			alpha		 = (int)Lebensdauer;
		} break;

		case BEAMSMOKE2:
		{
			Lebensdauer -= 20.0f SYNC;		// langsam
			alpha = (int)Lebensdauer;

			if (m_pParent != NULL)
			{
				// Bewegungsrichtung anpassen
				double absx = m_pParent->BeamX-xPos;				// Differenz der x 
				double absy = m_pParent->BeamY-yPos;				// und y Strecke
					
				double speed = 1/sqrt(absx*absx + absy*absy);	// Länge der Strecke berechnen
				speed = speed*(4+AnimPhase * 2);				// Geschwindigkeit

				absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
				absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

				xSpeed = float(absx);
				ySpeed = float(absy);
			}
		} break;

		case SNOWFLUSH:
		{
			Lebensdauer -= 8.0f SYNC;		

			// Im Wasser gelandet ?
			if (pTileEngine->BlockUnten (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]) & BLOCKWERT_LIQUID)
				Lebensdauer -= 20.0f SYNC;		

			alpha		 = (int)Lebensdauer;

			Rot += RotDir * 10.0f SYNC;
		} break;

		case WATERFLUSH:
		{
			Lebensdauer -= 1.5f SYNC;		

			// Im Wasser gelandet ?
			if (pTileEngine->BlockUnten (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]) & BLOCKWERT_LIQUID    ||
				pTileEngine->BlockUnten (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]) & BLOCKWERT_PLATTFORM ||
				pTileEngine->BlockUnten (xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]) & BLOCKWERT_WAND)
				Lebensdauer -= 4.0f SYNC;		

			alpha		 = (int)Lebensdauer;

			Rot += RotDir * 10.0f SYNC;
		} break;

		case UFOLASERFLARE:
		{
			Lebensdauer -= 20.0f SYNC;		
			alpha		 = (int)Lebensdauer;
		} break;

		case FUNKE:							// RoterFunken
		{
			Lebensdauer -= 14.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
		} break;				

		case FUNKE2:						// Grüner Funken
		{
			Lebensdauer -= 16.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
		} break;

		case LONGFUNKE:
		{
			Lebensdauer -= 12.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			// steckt in der Wand?
			//
			if (yPosOld == yPos)
				Lebensdauer -= 100.0f SYNC;
			 
			if (Lebensdauer < 0.0f)
				Lebensdauer = 0.0f;

		} break;

		case WATERFUNKE:
		{
			// In Flüssigkeit? Dann gleich verschwinden
			if (bu & BLOCKWERT_LIQUID)
				Lebensdauer = 0.0f;

			// langsam ausfaden lassen
			Lebensdauer -= 18.0f SYNC;	
			alpha		 = (int)Lebensdauer;			
		} break;

		case LASERFUNKE:					// Laser-Funken
		{
			Lebensdauer -= 16.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
		} break;

		case LASERFUNKE2:					// Laser-Funken
		{
			Lebensdauer -= 16.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
			if (AnimPhase >= AnimEnde)
				AnimPhase = 0;
		} break;		

		case PHARAOSMOKE:					// Rauch des Pharao-Schusses
		{
			Lebensdauer -= 20.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
		} break;
		
		case ROCKETSMOKE:					// Rauch einer Rakete
		case ROCKETSMOKEBLUE:				// in blau
		{
			Lebensdauer -= 50.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
		} break;

		case ROCKETSMOKEGREEN:				// in grün
		{
			Lebensdauer -= 20.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
		} break;

		case FLUGSACKSMOKE:				
		{
			Lebensdauer -= 30.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
		} break;

		case EVILFUNKE:
		{
			Lebensdauer -= 30.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
		} break;

		case WASSERTROPFEN:					// Verschindet an der Wasseroberfläche
		{
			Lebensdauer -= 16.0f SYNC;	// langsam ausfaden lassen

			if (Lebensdauer > 255.0f)
				alpha = 0;
			else
				alpha = (int)Lebensdauer;

			if (bu & BLOCKWERT_LIQUID &&
				ySpeed > 0.0f)
				Lebensdauer = 0;
		} break;

		case BUBBLE:						// Zerplatzt an der Wasseroberfläche
		{
			// Richtung umdrehen ?
			if ((xSpeed < -AnimCount &&
			 	xAcc < 0.0f) ||
			   (xSpeed > AnimCount &&
				xAcc > 0.0f))
				xAcc *= -1;

			if (ySpeed < 0.0f)
			{
				bo = pTileEngine->BlockOben(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt], false);
				if (!(bo & BLOCKWERT_LIQUID) ||
				   (bo & BLOCKWERT_WAND))

				{
					for (int i=0; i<3; i++)
						pPartikelSystem->PushPartikel(xPos, yPos, WATERFUNKE);

					// Und Luftblase verschwinden lassen
					Lebensdauer = 0;
				}
			}
		} break;		

		case LASERFLAME:			// Wird immer durchsichtiger
		case STELZFLARE:		
		{
			Lebensdauer -= float(50.0 SYNC);
			alpha = int (Lebensdauer);

		} break;

		case SHIELD:				// Verschwindet nach Ablauf der Animation
		{
			Lebensdauer -= 20.0f SYNC;			
			alpha = int (Lebensdauer);

			// an Spieler anpassen
			xPos = m_pParent->xpos + xSpeed + 37;
			yPos = m_pParent->ypos + ySpeed + 57;

			if (m_pParent->Handlung == RADELN ||
				m_pParent->Handlung == RADELN_FALL)
				yPos += 30;
		} break;

		case TEXTSECRET:
		{
			Lebensdauer -= float(10.0 SYNC);
			alpha = int (Lebensdauer);
		} break;

		case KRINGELSECRET:
		{
			Lebensdauer -= float(10.0 SYNC + AnimPhase);
			alpha = int (Lebensdauer);

			xSpeed -= (xSpeed * 0.1f) SYNC;			
			ySpeed -= (ySpeed * 0.1f) SYNC;			
		} break;

		case KRINGEL:
		{
			Lebensdauer -= float(20.0 SYNC);
			alpha = int (Lebensdauer);

			xPos += m_pParent->xspeed SYNC;
			yPos += m_pParent->yspeed SYNC;

			// Richtung neu berechnen
			//
			double	absx, absy, speed;				// Variablen für die Geschwindigkeits-
													// berechnung
			absx = m_pParent->xpos+35-(xPos+4);		// Differenz der x 
			absy = m_pParent->ypos+40-(yPos+4);		// und y Strecke
				
			speed = 1/sqrt(absx*absx + absy*absy);	// Länge der Strecke berechnen
			speed = speed*(8+AnimPhase);			// Geschwindigkeit ist 4 fach

			absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
			absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

			xSpeed = float(absx);
			ySpeed = float(absy);
		} break;

		case TURBINESMOKE:							// Partikel, die in die Turbine des Metalhead Bosses gesaugt werden
		{
			Lebensdauer -= float(10.0 SYNC);
			alpha = int (Lebensdauer);

			// Richtung neu berechnen
			//
			double	absx, absy, speed;				// Variablen für die Geschwindigkeits-
													// berechnung
			absx = pPartikelSystem->xtarget-xPos;	// Differenz der x 
			absy = pPartikelSystem->ytarget-yPos;	// und y Strecke
				
			speed = 1/sqrt(absx*absx + absy*absy);	// Länge der Strecke berechnen
			speed = speed*(15+AnimPhase*2);			// Geschwindigkeit ist 4 fach

			absx = speed*absx;						// Und jeweilige Geschwindigkeit setzen
			absy = speed*absy;						// (xSpeed*ySpeed ergibt 4)

			xSpeed = float(absx);
			ySpeed = float(absy);
		} break;

		case MINIFLARE:								// Flare beim Lava Ball
		{
			Lebensdauer -= float(5.0 SYNC);
			alpha = int (Lebensdauer);

			if (xSpeed >  2.0f) xAcc = -1.0f;
			if (xSpeed < -2.0f) xAcc =  1.0f;

			if (ySpeed > 20.0f)
				ySpeed = 20.0f;

		} break;

		case GRENADEFLARE:							// Flare beim Granaten Treffer
		{
			Lebensdauer -= float(20.0 SYNC);
			alpha = int (Lebensdauer);
			Rot += RotDir * 20.0f SYNC;

		} break;

		case EXPLOSIONFLARE:						// Flare bei Explosion
		case EXPLOSIONFLARE2:						// Flare bei Explosion
		{
			Lebensdauer -= float(50.0 SYNC);
			alpha = int (Lebensdauer);

			// LightMap rendern
			int a;

			a = (int) Lebensdauer * 2;

			if (a > 255)
				a = 255;

			if (PartikelArt == EXPLOSIONFLARE)
				pTileEngine->DrawLightmap(LIGHTMAP_EXPLOSION, xPos + 80, yPos + 80, a);
			else
			if (PartikelArt == EXPLOSIONFLARE2)
				pTileEngine->DrawLightmap(LIGHTMAP_LILA, xPos + 80, yPos + 80, a);


		} break;

		case SCHLEIM:						// kleiner Schleimbollen
		case SCHLEIM2:						// kleiner Alien Schleimbollen
		{
			Lebensdauer -= 18.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			if (AnimPhase >= AnimEnde)
				AnimPhase = 0;
		} break;		

		case SHOCKEXPLOSION:				// Schockwelle bei Spieler Explosion
		{
			Lebensdauer -= 10.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
			AnimCount += 130.0f SYNC;			
		} break;		

		case SHOTFLARE:						// Leuchten bei Schuss-Aufprall
		case SHOTFLARE2:
		{
			Lebensdauer -= 100.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
		} break;		

		case EXTRACOLLECTED:
		{
			Lebensdauer -= (100.0f - AnimPhase * 20.0f) SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			AnimCount += AnimPhase * 40.0f SYNC;
		} break;

		case DIAMANTCOLLECTED:
		{
			Lebensdauer -= 120.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			AnimCount += 40.0f SYNC;
		} break;

		case DRACHE_SMOKE:
		{
			Lebensdauer -= 25.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			Rot += RotDir * 10.0f SYNC;
		} break;

		case FIREBALL_SMOKE:
		{
			Lebensdauer -= 150.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			Rot += RotDir * 10.0f SYNC;
		} break;

		case LILA:
		{
			Lebensdauer -= 30.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
		} break;

		case LASERFLARE:
		{
			Lebensdauer -= 150.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
		} break;

		case EXPLOSION_TRACE_END:
		{
			Lebensdauer -= 4.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
			if (AnimPhase >= AnimEnde)		// Animation zu Ende	?
				Lebensdauer = 0;			// Dann Explosion verschwinden lassen

			Rot += RotDir * AnimSpeed * 3.0f SYNC;

		} break;
		
		case LAVADUST:		
		{
			Lebensdauer -= 1.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;

			// Richtung umdrehen ?
			if (xSpeed < -2.0f)
				xAcc = 0.4f;

			if (xSpeed >  2.0f)
				xAcc = -0.4f;
		} break;

		case EXPLOSION_MEDIUM2_ADD:
		case EXPLOSION_MEDIUM3_ADD:
		case EXPLOSION_REGULAR:		
		{
			Lebensdauer -= 5.0f SYNC;	// langsam ausfaden lassen
			alpha		 = (int)Lebensdauer;
			if (AnimPhase >= AnimEnde)		// Animation zu Ende	?
				Lebensdauer = 0;			// Dann Explosion verschwinden lassen

			Rot += RotDir * AnimSpeed * 10.0f SYNC;

		} break;		

		case EXPLOSION_TRACE:
		{
			Lebensdauer -= 5.0f SYNC;	// langsam ausfaden lassen

			// Explosion spawnen
			AnimCount -= 1.0f SYNC;
			if (AnimCount < 0.0f)
			{
				AnimCount = 0.4f;
				pPartikelSystem->PushPartikel(xPos + 30, yPos + 30, EXPLOSION_REGULAR);
			}
		} break;

	} // switch >= Additiv Grenze

	// alte Position für Kollisionsabfrage sichern
	xPosOld = xPos;
	yPosOld = yPos;
}

// --------------------------------------------------------------------------------------
// Partikel anzeigen, und dabei nur die Partikel-Textur setzen, wenn diese nicht
// schon gesetzt ist, denn das bringt Speeeeeeeeeeed ... hoffentlich =)
// --------------------------------------------------------------------------------------

bool PartikelClass::Render(void)
{
	if (alpha < 0)
		alpha = 0;

	// Partikel rotieren?
	if (Rotate == true)
	{
		DrawMode = MODE_ROTATED;

		if (Rot <   0.0f) Rot += 360.0f;
		if (Rot > 360.0f) Rot -= 360.0f;

		DirectGraphics.SetFilterMode (true);			

		D3DXMATRIX	matRot, matTrans, matTrans2;
		int width  = pPartikelGrafix[PartikelArt]->itsXFrameSize;
		int height = pPartikelGrafix[PartikelArt]->itsYFrameSize;

		// Rotationsmatrix
		D3DXMatrixRotationZ  (&matRot, DegreetoRad[int(Rot)]);

		float x = (float)(xPos - pTileEngine->XOffset);
		float y = (float)(yPos - pTileEngine->YOffset);

		// Transformation zum Ursprung
		D3DXMatrixTranslation(&matTrans,-x-(width) /2,
										-y-(height)/2, 0.0f);

		// Transformation wieder zurück
		D3DXMatrixTranslation(&matTrans2,x+(width) /2,
										 y+(height)/2, 0.0f);

		// Verschieben und rotieren
		D3DXMatrixMultiply	 (&matWorld, &matTrans, &matRot);	

		// und wieder zurück verschieben
		D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans2);
		lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);			
	}
	else

	// Partikel nicht rotieren?
	if (Rotate == false &&
		DrawMode != MODE_NORMAL)
	{
		DrawMode = MODE_NORMAL;

		DirectGraphics.SetFilterMode (false);

		// Normale Projektions-Matrix wieder herstellen
		D3DXMatrixRotationZ (&matWorld, 0.0f);
		lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
	}

	// Normaler Partikel (aus Grafik) zeichnen
	if (OwnDraw == false)						
	{
		float l,  r,  o,  u;					// Vertice Koordinaten
		float tl, tr, to, tu;					// Textur Koordinaten

		float xs, ys;
		int   xfs, yfs, xfc;

		xs  = pPartikelGrafix[PartikelArt]->GetXSize();
		ys  = pPartikelGrafix[PartikelArt]->GetYSize();
		xfs = pPartikelGrafix[PartikelArt]->GetXFrameSize();
		yfs = pPartikelGrafix[PartikelArt]->GetYFrameSize();
		xfc = pPartikelGrafix[PartikelArt]->GetXFrameCount();

		RECT Rect;

		// Ausschnitt berechnen
		Rect.top	= (AnimPhase/xfc) * yfs;
		Rect.left	= (AnimPhase%xfc) * xfs;
		Rect.right  = Rect.left + xfs;
		Rect.bottom = Rect.top  + yfs;

		l = float(-pTileEngine->XOffset+xPos-0.5f);								// Links
		r = float(-pTileEngine->XOffset+xPos+(Rect.right -Rect.left-1)+0.5f);	// Rechts
		o = float(-pTileEngine->YOffset+yPos-0.5f);								// Oben
		u = float(-pTileEngine->YOffset+yPos+(Rect.bottom-Rect.top-1) +0.5f);	// Unten

		tl = Rect.left  /xs;	// Links
		tr = Rect.right /xs;	// Rechts
		to = Rect.top   /ys;	// Oben
		tu = Rect.bottom/ys;	// Unten

		v1.color = v2.color = v3.color = v4.color = D3DCOLOR_RGBA(red, green, blue, alpha);
		v1.z	 = v2.z		= v3.z	   = v4.z	  = 0.0f;

		v1.x		= l;		// Links oben
		v1.y		= o;
		v1.tu		= tl;
		v1.tv		= to;

		v2.x		= r;		// Rechts oben
		v2.y		= o;
		v2.tu		= tr;
		v2.tv		= to;

		v3.x		= l;		// Links unten
		v3.y		= u;
		v3.tu		= tl;
		v3.tv		= tu;

		v4.x		= r;		// Rechts unten
		v4.y		= u;
		v4.tu		= tr;
		v4.tv		= tu;

		TriangleStrip[0] = v1;
		TriangleStrip[1] = v2;
		TriangleStrip[2] = v3;
		TriangleStrip[3] = v4;

		if (PartikelArt != CurrentPartikelTexture)
		{
			lpD3DDevice->SetTexture (0, pPartikelGrafix[PartikelArt]->itsTexture);							// Textur setzen
			CurrentPartikelTexture = PartikelArt;
		}	

		// Sprite zeichnen
		DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2, &TriangleStrip[0]);
		
	}
	else
	if (PartikelArt == TEXTSECRET)
	{
		D3DCOLOR col = D3DCOLOR_RGBA(255, 224, 64, alpha);
		pMenuFont->DrawTextCenterAlign((float)(xPos - pTileEngine->XOffset),
									   (float)(yPos - pTileEngine->YOffset),
									   TextArray[TEXT_SECRET], col);

		CurrentPartikelTexture = -1;
	}

	// Langer Funke (Linie)
	else 
	if (PartikelArt == EXPLOSION_TRACE_END)
	{
		D3DCOLOR col = D3DCOLOR_RGBA(255, 255, 255, (int)(Lebensdauer));
		pPartikelGrafix[PartikelArt]->itsRect = pPartikelGrafix[PartikelArt]->itsPreCalcedRects[AnimPhase];
		pPartikelGrafix[PartikelArt]->RenderSpriteScaledRotated(xPos, yPos, 
																(float)blue, (float)blue,  Rot, col);
		CurrentPartikelTexture = EXPLOSION_TRACE_END;
	}
	else
	if (PartikelArt == LONGFUNKE ||
		PartikelArt == WATERFUNKE)
	{
		lpD3DDevice->SetTexture (0, NULL);																	// Keine Textur setzen

		D3DXVECTOR2 pos, dir;

		pos.x = float (xPos - pTileEngine->XOffset);
		pos.y = float (yPos - pTileEngine->YOffset);
		dir.x = pos.x - xSpeed;
		dir.y = pos.y - ySpeed;

		if (PartikelArt == LONGFUNKE)
			RenderLine (dir, pos, D3DCOLOR_RGBA(255, 64, 16, int (Lebensdauer/3.0f)), D3DCOLOR_RGBA(255, 192, 32, int (Lebensdauer))); 

		else if (PartikelArt == WATERFUNKE)
		{
			dir.x = pos.x - xSpeed * 0.5f;
			dir.y = pos.y - ySpeed * 0.5f;

			RenderLine (dir, pos, D3DCOLOR_RGBA(pTileEngine->ColR1, pTileEngine->ColG1, pTileEngine->ColB1, int (Lebensdauer / 3.0f)), 
								  D3DCOLOR_RGBA(pTileEngine->ColR2, pTileEngine->ColG2, pTileEngine->ColB2, int (Lebensdauer))); 
		}

		CurrentPartikelTexture = -1;
	}

	// Halswirbel oder Turbine des Metalhead Bosses (dreht sich, daher extrawurscht)
	else 
	if (PartikelArt == HALSWIRBEL	  ||
		PartikelArt == KAPUTTETURBINE)
	{
		lpD3DDevice->SetTexture (0, pPartikelGrafix[PartikelArt]->itsTexture);							// Textur setzen

		pPartikelGrafix[PartikelArt]->RenderSpriteRotated (float (xPos - pTileEngine->XOffset), 
														   float (yPos - pTileEngine->YOffset), 
														   AnimCount / PI * 180.0f, D3DCOLOR_RGBA(red, green, blue, alpha));

		CurrentPartikelTexture = PartikelArt;
	}

	// Druckwelle bei der Explosion des Beams
	else 
	if (PartikelArt == BEAMSMOKE5)
	{
		float a = (float)(255 - alpha + 64);

		pPartikelGrafix[PartikelArt]->RenderSpriteScaledRotated (float (xPos - a / 2.0f - pTileEngine->XOffset) + 30, 
																 float (yPos - a / 2.0f - pTileEngine->YOffset) + 30, 
																 a, a, Rot, D3DCOLOR_RGBA(255, 255, 255, alpha));

		CurrentPartikelTexture = PartikelArt;
	}

	// Druckwelle bei der Explosion des Spieler
	else 
	if (PartikelArt == SHOCKEXPLOSION)
	{
		int a = 255 - alpha + 64;

		pPartikelGrafix[PartikelArt]->RenderSpriteScaled (float (xPos - AnimCount / 2.0f - pTileEngine->XOffset),
														  float (yPos - AnimCount / 2.0f - pTileEngine->YOffset),
														  (int)(AnimCount), (int)(AnimCount), D3DCOLOR_RGBA(red, green, blue, alpha));

		CurrentPartikelTexture = PartikelArt;
	}

	// Leuchten beim Extra Einsammeln
	else 
	if (PartikelArt == EXTRACOLLECTED)
	{
		pPartikelGrafix[PartikelArt]->RenderSpriteScaled (float (xPos + 16 - AnimCount / 2.0f - pTileEngine->XOffset),
														  float (yPos + 16 - AnimCount / 2.0f - pTileEngine->YOffset),
														  (int)(AnimCount), (int)(AnimCount), D3DCOLOR_RGBA(red, green, blue, alpha));

		CurrentPartikelTexture = PartikelArt;
	}

	// Leuchten beim Diamant Einsammeln
	else 
	if (PartikelArt == DIAMANTCOLLECTED)
	{
		DirectGraphics.SetAdditiveMode();

		pPartikelGrafix[PartikelArt]->RenderSpriteScaled (float (xPos + 15 - AnimCount / 2.0f - pTileEngine->XOffset),
														  float (yPos + 15 - AnimCount / 2.0f - pTileEngine->YOffset),
														  (int)(AnimCount), (int)(AnimCount), D3DCOLOR_RGBA(red, green, blue, alpha));

		CurrentPartikelTexture = PartikelArt;
	}

	// Langer Funke/Wasserspritzer
	else
	if (PartikelArt == WASSER_SPRITZER ||
		PartikelArt == WASSER_SPRITZER2)
	{
		int h, b;

		h = 100;
		if (PartikelArt == WASSER_SPRITZER)
			b = (int)(200 - alpha / 2);
		else	
			b = (int)(AnimCount * 30);
		
		pPartikelGrafix[PartikelArt]->RenderSpriteScaled (float (xPos - pTileEngine->XOffset) - b/4,
														  float (yPos - pTileEngine->YOffset) - h/2,
														  b, h, D3DCOLOR_RGBA(red, green, blue, alpha));

		CurrentPartikelTexture = PartikelArt;
	}
	else
	if (PartikelArt == BEAMSMOKE)
	{
		pPartikelGrafix[PartikelArt]->RenderSpriteScaledRotated (float (xPos - pTileEngine->XOffset) - AnimCount / 2.0f,
															 	 float (yPos - pTileEngine->YOffset) - AnimCount / 2.0f,
															     AnimCount, AnimCount, (float)Rot, D3DCOLOR_RGBA(red, green, blue, alpha));

		CurrentPartikelTexture = PartikelArt;
	}


	return true;
}

// --------------------------------------------------------------------------------------
// PartikelsystemKlasse Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor : laden der Partikel Grafiken
// --------------------------------------------------------------------------------------

PartikelsystemClass::PartikelsystemClass(void)
{
	pStart 		 = NULL;
	pEnd	 	 = NULL;
	NumPartikel  = 0;
	MAX_PARTIKEL = 2000;
	SetParticleCount();

	for(int i=0; i<MAX_PARTIKELGFX; i++)
		pPartikelGrafix[i] = new(DirectGraphicsSprite);

	// Patronenhülse
	pPartikelGrafix[BULLET]->LoadImage("Bullet.bmp", 8, 8, 8, 8, 1, 1);
	PartikelRect[BULLET].left = 2;	PartikelRect[BULLET].right  = 6;
	PartikelRect[BULLET].top  = 2;	PartikelRect[BULLET].bottom = 6;

	// Patronenhülse vom Skeletor
	pPartikelGrafix[BULLET_SKELETOR]->LoadImage("skeletor_bullet.bmp", 15, 6, 15, 6, 1, 1);
	PartikelRect[BULLET_SKELETOR].left = 2;	PartikelRect[BULLET_SKELETOR].right  = 13;
	PartikelRect[BULLET_SKELETOR].top  = 1;	PartikelRect[BULLET_SKELETOR].bottom = 5;

	// Glassplitter
	pPartikelGrafix[GLASSPLITTER]->LoadImage("glassplitter.png", 100, 80, 20, 20, 5, 4);
	PartikelRect[GLASSPLITTER].left = 8;	PartikelRect[GLASSPLITTER].right  = 12;
	PartikelRect[GLASSPLITTER].top  = 8;	PartikelRect[GLASSPLITTER].bottom = 12;

	// Mittlere Explosion
	pPartikelGrafix[EXPLOSION_MEDIUM]->LoadImage("explosion-medium.png", 300, 240, 60, 60, 5, 4);
	PartikelRect[EXPLOSION_MEDIUM].left = 0;	PartikelRect[EXPLOSION_MEDIUM].right  = 60;
	PartikelRect[EXPLOSION_MEDIUM].top  = 0;	PartikelRect[EXPLOSION_MEDIUM].bottom = 60;

	// Mittlere Explosion2
	pPartikelGrafix[EXPLOSION_MEDIUM2]->LoadImage("explosion-medium2.png", 300, 240, 60, 60, 5, 4);
	PartikelRect[EXPLOSION_MEDIUM2].left = 0;	PartikelRect[EXPLOSION_MEDIUM2].right  = 60;
	PartikelRect[EXPLOSION_MEDIUM2].top  = 0;	PartikelRect[EXPLOSION_MEDIUM2].bottom = 60;

	// Mittlere Explosion2
	pPartikelGrafix[EXPLOSION_MEDIUM2_ADD]->LoadImage("explosion-medium2.png", 300, 240, 60, 60, 5, 4);
	PartikelRect[EXPLOSION_MEDIUM2_ADD].left = 0;	PartikelRect[EXPLOSION_MEDIUM2_ADD].right  = 60;
	PartikelRect[EXPLOSION_MEDIUM2_ADD].top  = 0;	PartikelRect[EXPLOSION_MEDIUM2_ADD].bottom = 60;

	// Mittlere Explosion3
	pPartikelGrafix[EXPLOSION_MEDIUM3]->LoadImage("explosion-medium3.png", 300, 240, 60, 60, 5, 4);
	PartikelRect[EXPLOSION_MEDIUM3].left = 0;	PartikelRect[EXPLOSION_MEDIUM3].right  = 60;
	PartikelRect[EXPLOSION_MEDIUM3].top  = 0;	PartikelRect[EXPLOSION_MEDIUM3].bottom = 60;

	// Mittlere Explosion3
	pPartikelGrafix[EXPLOSION_MEDIUM3_ADD]->LoadImage("explosion-medium3.png", 300, 240, 60, 60, 5, 4);
	PartikelRect[EXPLOSION_MEDIUM3_ADD].left = 0;	PartikelRect[EXPLOSION_MEDIUM3_ADD].right  = 60;
	PartikelRect[EXPLOSION_MEDIUM3_ADD].top  = 0;	PartikelRect[EXPLOSION_MEDIUM3_ADD].bottom = 60;

	// Grüne Explosion
	pPartikelGrafix[EXPLOSION_GREEN]->LoadImage("explosion-green.png", 300, 300, 60, 60, 5, 5);
	PartikelRect[EXPLOSION_GREEN].left = 0;	PartikelRect[EXPLOSION_GREEN].right  = 60;
	PartikelRect[EXPLOSION_GREEN].top  = 0;	PartikelRect[EXPLOSION_GREEN].bottom = 60;

	// Alien Explosion
	pPartikelGrafix[EXPLOSION_ALIEN]->LoadImage("alienexplosion.png", 300, 300, 60, 60, 5, 5);
	PartikelRect[EXPLOSION_ALIEN].left = 0;	PartikelRect[EXPLOSION_ALIEN].right  = 60;
	PartikelRect[EXPLOSION_ALIEN].top  = 0;	PartikelRect[EXPLOSION_ALIEN].bottom = 60;

	// Grosse Explosion
	pPartikelGrafix[EXPLOSION_BIG]->LoadImage("explosion-big.png", 512, 512, 100, 100, 5, 5);
	PartikelRect[EXPLOSION_BIG].left = 0;	PartikelRect[EXPLOSION_BIG].right  = 100;
	PartikelRect[EXPLOSION_BIG].top  = 0;	PartikelRect[EXPLOSION_BIG].bottom = 100;

	// Grosse Explosion
	pPartikelGrafix[EXPLOSION_GIANT]->LoadImage("explosion-big2.png", 480, 480, 120, 120, 4, 4);
	PartikelRect[EXPLOSION_GIANT].left = 0;	PartikelRect[EXPLOSION_GIANT].right  = 120;
	PartikelRect[EXPLOSION_GIANT].top  = 0;	PartikelRect[EXPLOSION_GIANT].bottom = 120;

	// Explosions Trace
	pPartikelGrafix[EXPLOSION_TRACE]->LoadImage("explosion-trace.bmp", 120, 120, 120, 120, 1, 1);
	PartikelRect[EXPLOSION_TRACE].left = 0;	PartikelRect[EXPLOSION_TRACE].right  = 120;
	PartikelRect[EXPLOSION_TRACE].top  = 0;	PartikelRect[EXPLOSION_TRACE].bottom = 120;

	// Kleine blaue Explosion
	pPartikelGrafix[BLUE_EXPLOSION]->LoadImage("blue-explosion.png", 96, 72, 24, 24, 4, 3);
	PartikelRect[BLUE_EXPLOSION].left = 0;	PartikelRect[BLUE_EXPLOSION].right  = 24;
	PartikelRect[BLUE_EXPLOSION].top  = 0;	PartikelRect[BLUE_EXPLOSION].bottom = 24;

	// Riesen fette Explosion
	pPartikelGrafix[EXPLOSION_GIGA]->LoadImage("explosion-giga.png", 1000, 800, 200, 200, 5, 4);
	PartikelRect[EXPLOSION_GIGA].left = 0;	PartikelRect[EXPLOSION_GIGA].right  = 200;
	PartikelRect[EXPLOSION_GIGA].top  = 0;	PartikelRect[EXPLOSION_GIGA].bottom = 200;

	// Kleine animierter Splitter
	pPartikelGrafix[SPLITTER]->LoadImage("splitter.bmp", 32, 32, 8, 8, 4, 4);
	PartikelRect[SPLITTER].left = 0;	PartikelRect[SPLITTER].right  = 8;
	PartikelRect[SPLITTER].top  = 0;	PartikelRect[SPLITTER].bottom = 8;

	// Fetzen eines kaputten Piranhas
	pPartikelGrafix[PIRANHATEILE]->LoadImage("piranhateile.bmp", 100, 16, 20, 16, 5, 1);
	PartikelRect[PIRANHATEILE].left = 0;	PartikelRect[PIRANHATEILE].right  = 20;
	PartikelRect[PIRANHATEILE].top  = 0;	PartikelRect[PIRANHATEILE].bottom = 16;

	// Fetzen eines kaputten Riesen Piranhas
	pPartikelGrafix[PIRANHATEILE2]->LoadImage("piranhateile2.bmp", 240, 180, 120, 90, 2, 2);
	PartikelRect[PIRANHATEILE2].left = 0;	PartikelRect[PIRANHATEILE2].right  = 120;
	PartikelRect[PIRANHATEILE2].top  = 0;	PartikelRect[PIRANHATEILE2].bottom = 90;

	// Blut eines kaputten Piranhas
	pPartikelGrafix[PIRANHABLUT]->LoadImage("piranhablut.bmp", 160, 40, 40, 40, 4, 1);
	PartikelRect[PIRANHABLUT].left = 0;	PartikelRect[PIRANHABLUT].right  = 40;
	PartikelRect[PIRANHABLUT].top  = 0;	PartikelRect[PIRANHABLUT].bottom = 40;

	// Blut einer kaputten Made
	pPartikelGrafix[MADEBLUT]->LoadImage("MADEblut.bmp", 48, 12, 12, 12, 4, 1);
	PartikelRect[MADEBLUT].left = 0;	PartikelRect[MADEBLUT].right  = 12;
	PartikelRect[MADEBLUT].top  = 0;	PartikelRect[MADEBLUT].bottom = 12;

	// Wassertropfen der vom Spawner kommt
	pPartikelGrafix[SPAWNDROP]->LoadImage("tropfen.png", 5, 5, 5, 5, 1, 1);
	PartikelRect[SPAWNDROP].left = 0;	PartikelRect[SPAWNDROP].right  = 5;
	PartikelRect[SPAWNDROP].top  = 0;	PartikelRect[SPAWNDROP].bottom = 5;

	// Blatt
	pPartikelGrafix[BLATT]->LoadImage("blatt.png", 32, 10, 16, 10, 2, 1);
	PartikelRect[BLATT].left = 0;	PartikelRect[BLATT].right  = 16;
	PartikelRect[BLATT].top  = 0;	PartikelRect[BLATT].bottom = 8;

	// Blatt2
	pPartikelGrafix[BLATT2]->LoadImage("blatt.png", 32, 10, 16, 10, 2, 1);
	PartikelRect[BLATT2].left = 0;	PartikelRect[BLATT2].right  = 16;
	PartikelRect[BLATT2].top  = 0;	PartikelRect[BLATT2].bottom = 8;

	// Wespennest nach Abschuss
	pPartikelGrafix[NESTLUFT]->LoadImage("nest.bmp", 43, 60, 43, 60, 1, 1);
	PartikelRect[NESTLUFT].left = 0;	PartikelRect[NESTLUFT].right  = 43;
	PartikelRect[NESTLUFT].top  = 0;	PartikelRect[NESTLUFT].bottom = 60;

	// Fels-Splitter
	pPartikelGrafix[ROCKSPLITTER]->LoadImage("rocksplitter.bmp", 96, 48, 24, 24, 4, 2);
	PartikelRect[ROCKSPLITTER].left = 0;	PartikelRect[ROCKSPLITTER].right  = 24;
	PartikelRect[ROCKSPLITTER].top  = 0;	PartikelRect[ROCKSPLITTER].bottom = 24;

	// kleine Fels-Splitter
	pPartikelGrafix[ROCKSPLITTERSMALL]->LoadImage("rocksplittersmall.bmp", 48, 24, 12, 12, 4, 2);
	PartikelRect[ROCKSPLITTERSMALL].left = 0;	PartikelRect[ROCKSPLITTERSMALL].right  = 12;
	PartikelRect[ROCKSPLITTERSMALL].top  = 0;	PartikelRect[ROCKSPLITTERSMALL].bottom = 12;

	// kleine Stalagtit-Splitter
	pPartikelGrafix[ROCKSPLITTERSMALLBLUE]->LoadImage("rocksplittersmallblue.bmp", 48, 24, 12, 12, 4, 2);
	PartikelRect[ROCKSPLITTERSMALLBLUE].left = 0;	PartikelRect[ROCKSPLITTERSMALLBLUE].right  = 12;
	PartikelRect[ROCKSPLITTERSMALLBLUE].top  = 0;	PartikelRect[ROCKSPLITTERSMALLBLUE].bottom = 12;

	// Spinnen Splitter
	pPartikelGrafix[SPIDERSPLITTER]->LoadImage("spidersplitter.bmp", 64, 64, 16, 16, 4, 4);
	PartikelRect[SPIDERSPLITTER].left = 2;	PartikelRect[SPIDERSPLITTER].right  = 14;
	PartikelRect[SPIDERSPLITTER].top  = 2;	PartikelRect[SPIDERSPLITTER].bottom = 14;

	pPartikelGrafix[SPIDERSPLITTER2]->LoadImage("spidersplitter2.bmp", 64, 64, 16, 16, 4, 4);
	PartikelRect[SPIDERSPLITTER2].left = 2;	PartikelRect[SPIDERSPLITTER2].right  = 14;
	PartikelRect[SPIDERSPLITTER2].top  = 2;	PartikelRect[SPIDERSPLITTER2].bottom = 14;

	// Granate der Spinne
	pPartikelGrafix[SPIDERGRENADE]->LoadImage("spidergrenade.bmp", 64, 16, 16, 16, 4, 1);
	PartikelRect[SPIDERGRENADE].left = 0;	PartikelRect[SPIDERGRENADE].right  = 16;
	PartikelRect[SPIDERGRENADE].top  = 0;	PartikelRect[SPIDERGRENADE].bottom = 16;

	// Schatten des Evil Hurri
	pPartikelGrafix[EVILSMOKE]->LoadImage("evil-smoke.bmp", 70, 80, 70, 80, 1, 1);
	PartikelRect[EVILSMOKE].left = 0;	PartikelRect[EVILSMOKE].right  = 70;
	PartikelRect[EVILSMOKE].top  = 0;	PartikelRect[EVILSMOKE].bottom = 80;
	pPartikelGrafix[EVILSMOKE2]->LoadImage("evil-smoke2.bmp", 70, 80, 70, 80, 1, 1);
	PartikelRect[EVILSMOKE2].left = 0;	PartikelRect[EVILSMOKE2].right  = 70;
	PartikelRect[EVILSMOKE2].top  = 0;	PartikelRect[EVILSMOKE2].bottom = 80;

	// Stelze eines StelzSacks
	pPartikelGrafix[STELZE]->LoadImage("stelze.bmp", 400, 200, 100, 100, 4, 2);
	PartikelRect[STELZE].left = 30;	PartikelRect[STELZE].right  = 70;
	PartikelRect[STELZE].top  = 30;	PartikelRect[STELZE].bottom = 70;

	// Kopf eines StelzSacks
	pPartikelGrafix[STELZHEAD]->LoadImage("stelzhead.bmp", 64, 60, 64, 60, 1, 1);
	PartikelRect[STELZHEAD].left = 0;	PartikelRect[STELZHEAD].right  = 64;
	PartikelRect[STELZHEAD].top  = 0;	PartikelRect[STELZHEAD].bottom = 60;

	// Smoke2
	pPartikelGrafix[SMOKE2]->LoadImage("smoke2.png", 24, 24, 24, 24, 1, 1);
	PartikelRect[SMOKE2].left = 0;	PartikelRect[SMOKE2].right  = 24;
	PartikelRect[SMOKE2].top  = 0;	PartikelRect[SMOKE2].bottom = 24;

	// Smoke3
	pPartikelGrafix[SMOKE3]->LoadImage("smoke2.png", 24, 24, 24, 24, 1, 1);
	PartikelRect[SMOKE3].left = 0;	PartikelRect[SMOKE3].right  = 24;
	PartikelRect[SMOKE3].top  = 0;	PartikelRect[SMOKE3].bottom = 24;

	// Riesen Rauch
	pPartikelGrafix[SMOKEBIG]->LoadImage("smokebig.png", 240, 60, 60, 60, 4, 1);
	PartikelRect[SMOKEBIG].left = 0;	PartikelRect[SMOKEBIG].right  = 60;
	PartikelRect[SMOKEBIG].top  = 0;	PartikelRect[SMOKEBIG].bottom = 60;

	// Riesen Rauch
	pPartikelGrafix[SMOKEBIG2]->LoadImage("smokebig.png", 240, 60, 60, 60, 4, 1);
	PartikelRect[SMOKEBIG2].left = 0;	PartikelRect[SMOKEBIG2].right  = 60;
	PartikelRect[SMOKEBIG2].top  = 0;	PartikelRect[SMOKEBIG2].bottom = 60;

	// Riesen Rauch
	pPartikelGrafix[SMOKEBIG_OUTTRO]->LoadImage("smokebig.png", 240, 60, 60, 60, 4, 1);
	PartikelRect[SMOKEBIG_OUTTRO].left = 0;	PartikelRect[SMOKEBIG_OUTTRO].right  = 60;
	PartikelRect[SMOKEBIG_OUTTRO].top  = 0;	PartikelRect[SMOKEBIG_OUTTRO].bottom = 60;

	// Rauch des Blitzbeams
	pPartikelGrafix[BEAMSMOKE]->LoadImage("beamsmoke.bmp", 24, 24, 24, 24, 1, 1);
	PartikelRect[BEAMSMOKE].left = 0;	PartikelRect[BEAMSMOKE].right  = 24;
	PartikelRect[BEAMSMOKE].top  = 0;	PartikelRect[BEAMSMOKE].bottom = 24;

	// Rauch des Blitzbeams
	pPartikelGrafix[BEAMSMOKE4]->LoadImage("beamsmoke.bmp", 24, 24, 24, 24, 1, 1);
	PartikelRect[BEAMSMOKE4].left = 0;	PartikelRect[BEAMSMOKE4].right  = 24;
	PartikelRect[BEAMSMOKE4].top  = 0;	PartikelRect[BEAMSMOKE4].bottom = 24;

	// Rauch beim Aufladen des Blitzbeams
	pPartikelGrafix[BEAMSMOKE2]->LoadImage("beamsmoke2.bmp", 24, 8, 8, 8, 3, 1);
	PartikelRect[BEAMSMOKE2].left = 0;	PartikelRect[BEAMSMOKE2].right  = 8;
	PartikelRect[BEAMSMOKE2].top  = 0;	PartikelRect[BEAMSMOKE2].bottom = 8;

	// Rauch beim Aufladen des Blitzbeams
	pPartikelGrafix[BEAMSMOKE3]->LoadImage("beamsmoke2.bmp", 24, 8, 8, 8, 3, 1);
	PartikelRect[BEAMSMOKE3].left = 0;	PartikelRect[BEAMSMOKE3].right  = 8;
	PartikelRect[BEAMSMOKE3].top  = 0;	PartikelRect[BEAMSMOKE3].bottom = 8;

	// Druckwelle bei der Explosion des Beams
	pPartikelGrafix[BEAMSMOKE5]->LoadImage("beamsmoke5.bmp", 64, 64, 64, 64, 1, 1);
	PartikelRect[BEAMSMOKE5].left = 0;	PartikelRect[BEAMSMOKE5].right  = 64;
	PartikelRect[BEAMSMOKE5].top  = 0;	PartikelRect[BEAMSMOKE5].bottom = 64;

	// Schneegestöber
	pPartikelGrafix[SNOWFLUSH]->LoadImage("snowflush.bmp", 32, 32, 32, 32, 1, 1);
	PartikelRect[SNOWFLUSH].left = 0;	PartikelRect[SNOWFLUSH].right  = 32;
	PartikelRect[SNOWFLUSH].top  = 0;	PartikelRect[SNOWFLUSH].bottom = 32;

	// Wasserfall Dampf
	pPartikelGrafix[WATERFLUSH]->LoadImage("snowflush.bmp", 32, 32, 32, 32, 1, 1);
	PartikelRect[WATERFLUSH].left = 0;	PartikelRect[WATERFLUSH].right  = 32;
	PartikelRect[WATERFLUSH].top  = 0;	PartikelRect[WATERFLUSH].bottom = 32;

	// Ufo Laser Flare
	pPartikelGrafix[UFOLASERFLARE]->LoadImage("ufolaserflare.bmp", 160, 160, 160, 160, 1, 1);
	PartikelRect[UFOLASERFLARE].left = 0;	PartikelRect[UFOLASERFLARE].right  = 160;
	PartikelRect[UFOLASERFLARE].top  = 0;	PartikelRect[UFOLASERFLARE].bottom = 160;
	
	// Additive Partikel
	//
	// Roter Funken
	pPartikelGrafix[FUNKE]->LoadImage("funke.bmp", 3, 3, 3, 3, 1, 1);
	PartikelRect[FUNKE].left = 0;	PartikelRect[FUNKE].right  = 3;
	PartikelRect[FUNKE].top  = 0;	PartikelRect[FUNKE].bottom = 2;

	// Langer Funken
	pPartikelGrafix[LONGFUNKE]->LoadImage("funke.bmp", 3, 3, 3, 3, 1, 1);
	PartikelRect[LONGFUNKE].left = 0;	PartikelRect[LONGFUNKE].right  = 3;
	PartikelRect[LONGFUNKE].top  = 0;	PartikelRect[LONGFUNKE].bottom = 2;

	// Wasser Funken
	PartikelRect[WATERFUNKE].left = 0;	PartikelRect[WATERFUNKE].right  = 3;
	PartikelRect[WATERFUNKE].top  = 0;	PartikelRect[WATERFUNKE].bottom = 2;

	// Grüner Funken
	pPartikelGrafix[FUNKE2]->LoadImage("funke2.bmp", 3, 3, 3, 3, 1, 1);
	PartikelRect[FUNKE2].left = 0;	PartikelRect[FUNKE2].right  = 3;
	PartikelRect[FUNKE2].top  = 0;	PartikelRect[FUNKE2].bottom = 2;

	// Laserfunke
	pPartikelGrafix[LASERFUNKE]->LoadImage("laserfunke.bmp", 9, 9, 9, 9, 1, 1);
	PartikelRect[LASERFUNKE].left = 0;	PartikelRect[LASERFUNKE].right  = 9;
	PartikelRect[LASERFUNKE].top  = 0;	PartikelRect[LASERFUNKE].bottom = 9;

	// Laserfunke2
	pPartikelGrafix[LASERFUNKE2]->LoadImage("laserfunke2.bmp", 27, 9, 9, 9, 3, 1);
	PartikelRect[LASERFUNKE2].left = 0;	PartikelRect[LASERFUNKE2].right  = 9;
	PartikelRect[LASERFUNKE2].top  = 0;	PartikelRect[LASERFUNKE2].bottom = 9;

	// Rauchwolke
	pPartikelGrafix[SMOKE]->LoadImage("smoke.png", 30, 30, 30, 30, 1, 1);
	PartikelRect[SMOKE].left = 0;	PartikelRect[SMOKE].right  = 30;
	PartikelRect[SMOKE].top  = 0;	PartikelRect[SMOKE].bottom = 30;

	// Rauch für den Pharao Schuss
	pPartikelGrafix[PHARAOSMOKE]->LoadImage("pharaosmoke.bmp", 12, 12, 12, 12, 1, 1);
	PartikelRect[PHARAOSMOKE].left = 0;	PartikelRect[PHARAOSMOKE].right  = 12;
	PartikelRect[PHARAOSMOKE].top  = 0;	PartikelRect[PHARAOSMOKE].bottom = 12;

	// Rauch einer Rakete
	pPartikelGrafix[ROCKETSMOKE]->LoadImage("rocketsmoke.bmp", 12, 12, 12, 12, 1, 1);
	PartikelRect[ROCKETSMOKE].left = 0;	PartikelRect[ROCKETSMOKE].right  = 12;
	PartikelRect[ROCKETSMOKE].top  = 0;	PartikelRect[ROCKETSMOKE].bottom = 12;

	// Rauch einer Rakete in blau
	pPartikelGrafix[ROCKETSMOKEBLUE]->LoadImage("rocketsmokeblue.bmp", 12, 12, 12, 12, 1, 1);
	PartikelRect[ROCKETSMOKEBLUE].left = 0;	PartikelRect[ROCKETSMOKEBLUE].right  = 12;
	PartikelRect[ROCKETSMOKEBLUE].top  = 0;	PartikelRect[ROCKETSMOKEBLUE].bottom = 12;

	// Rauch einer Rakete in grün
	pPartikelGrafix[ROCKETSMOKEGREEN]->LoadImage("rocketsmokegreen.bmp", 12, 12, 12, 12, 1, 1);
	PartikelRect[ROCKETSMOKEGREEN].left = 0;	PartikelRect[ROCKETSMOKEGREEN].right  = 12;
	PartikelRect[ROCKETSMOKEGREEN].top  = 0;	PartikelRect[ROCKETSMOKEGREEN].bottom = 12;

	// Rauch des Flugsacks
	pPartikelGrafix[FLUGSACKSMOKE]->LoadImage("flugsacksmoke.bmp", 64, 64, 16, 16, 4, 4);
	PartikelRect[FLUGSACKSMOKE].left = 0;	PartikelRect[FLUGSACKSMOKE].right  = 16;
	PartikelRect[FLUGSACKSMOKE].top  = 0;	PartikelRect[FLUGSACKSMOKE].bottom = 16;

	// Funke des Evil Blitzes
	pPartikelGrafix[EVILFUNKE]->LoadImage("evilfunke.bmp", 8, 8, 8, 8, 1, 1);
	PartikelRect[EVILFUNKE].left = 0;	PartikelRect[EVILFUNKE].right  = 8;
	PartikelRect[EVILFUNKE].top  = 0;	PartikelRect[EVILFUNKE].bottom = 8;

	// Wassertropfen
	pPartikelGrafix[WASSERTROPFEN]->LoadImage("tropfen2.png", 3, 3, 3, 3, 1, 1);
	PartikelRect[WASSERTROPFEN].left = 0;	PartikelRect[WASSERTROPFEN].right  = 3;
	PartikelRect[WASSERTROPFEN].top  = 0;	PartikelRect[WASSERTROPFEN].bottom = 3;

	// Säuretropfen
	pPartikelGrafix[WASSERTROPFEN2]->LoadImage("wassertropfen2.bmp", 16, 4, 4, 4, 4, 1);
	PartikelRect[WASSERTROPFEN2].left = 0;	PartikelRect[WASSERTROPFEN2].right  = 4;
	PartikelRect[WASSERTROPFEN2].top  = 0;	PartikelRect[WASSERTROPFEN2].bottom = 4;

	// Leuchteffekt für den Krabblerlaser
	pPartikelGrafix[LASERFLAME]->LoadImage("laserflame.bmp", 48, 48, 48, 48, 1, 1);
	PartikelRect[LASERFLAME].left = 0;	PartikelRect[LASERFLAME].right  = 48;
	PartikelRect[LASERFLAME].top  = 0;	PartikelRect[LASERFLAME].bottom = 48;

	// Luftblase
	pPartikelGrafix[BUBBLE]->LoadImage("bubble.bmp", 6, 6, 6, 6, 1, 1);
	PartikelRect[BUBBLE].left = 0;	PartikelRect[BUBBLE].right  = 6;
	PartikelRect[BUBBLE].top  = 0;	PartikelRect[BUBBLE].bottom = 6;

	// Schneeflocke
	pPartikelGrafix[SCHNEEFLOCKE]->LoadImage("snow.png", 30, 10, 10, 10, 3, 1);
	PartikelRect[SCHNEEFLOCKE].left = 0;	PartikelRect[SCHNEEFLOCKE].right  = 10;
	PartikelRect[SCHNEEFLOCKE].top  = 0;	PartikelRect[SCHNEEFLOCKE].bottom = 3;

	// Schneeflocke
	pPartikelGrafix[SCHNEEFLOCKE_END]->LoadImage("snow.png", 30, 10, 10, 10, 3, 1);
	PartikelRect[SCHNEEFLOCKE_END].left = 0;	PartikelRect[SCHNEEFLOCKE_END].right  = 10;
	PartikelRect[SCHNEEFLOCKE_END].top  = 0;	PartikelRect[SCHNEEFLOCKE_END].bottom = 3;

	// HurriTeile
	pPartikelGrafix[HURRITEILE]->LoadImage("hurri-teile.bmp", 217, 32, 32, 32, 7, 1);
	PartikelRect[HURRITEILE].left = 10;	PartikelRect[HURRITEILE].right  = 22;
	PartikelRect[HURRITEILE].top  = 10;	PartikelRect[HURRITEILE].bottom = 22;

	// Kleiner blauer Boulder
	pPartikelGrafix[BOULDER_SMALL]->LoadImage("boulder_small.png", 60, 33, 12, 11, 5, 3);
	PartikelRect[BOULDER_SMALL].left = 0;	PartikelRect[BOULDER_SMALL].right  = 12;
	PartikelRect[BOULDER_SMALL].top  = 0;	PartikelRect[BOULDER_SMALL].bottom = 11;

	// Der Wasserspritzer, wenn der Spieler aus dem Wasser hopst
	pPartikelGrafix[WASSER_SPRITZER]->LoadImage("wasserspritzer.png", 64, 128, 40, 128, 1, 1);
	PartikelRect[WASSER_SPRITZER].left = 0;	PartikelRect[WASSER_SPRITZER].right  = 40;
	PartikelRect[WASSER_SPRITZER].top  = 0;	PartikelRect[WASSER_SPRITZER].bottom = 128;

	// Der Wasserspritzer, wenn der Spieler ins Wasser hopst
	pPartikelGrafix[WASSER_SPRITZER2]->LoadImage("wasserspritzer.png", 40, 142, 40, 142, 1, 1);
	PartikelRect[WASSER_SPRITZER2].left = 0;	PartikelRect[WASSER_SPRITZER2].right  = 40;
	PartikelRect[WASSER_SPRITZER2].top  = 0;	PartikelRect[WASSER_SPRITZER2].bottom = 142;

	// Schrott der Lava Krabbe
	pPartikelGrafix[LAVAKRABBE_KOPF]->LoadImage("lavakrabbe_teile.bmp", 80, 40, 40, 40, 2, 1);
	PartikelRect[LAVAKRABBE_KOPF].left = 12;	PartikelRect[LAVAKRABBE_KOPF].right  = 28;
	PartikelRect[LAVAKRABBE_KOPF].top  = 12;	PartikelRect[LAVAKRABBE_KOPF].bottom = 28;	

	// Spinnenteile
	pPartikelGrafix[SPIDERPARTS]->LoadImage("spiderparts.bmp", 192, 40, 48, 40, 4, 1);
	PartikelRect[SPIDERPARTS].left = 0;	PartikelRect[SPIDERPARTS].right  = 48;
	PartikelRect[SPIDERPARTS].top  = 0;	PartikelRect[SPIDERPARTS].bottom = 36;	

	// Kettenteile
	pPartikelGrafix[KETTENTEILE]->LoadImage("kettenteile.png", 126, 50, 42, 50, 3, 1);
	PartikelRect[KETTENTEILE].left = 10;	PartikelRect[KETTENTEILE].right  = 40;
	PartikelRect[KETTENTEILE].top  = 10;	PartikelRect[KETTENTEILE].bottom = 32;	

	// Kettenteile 4
	pPartikelGrafix[KETTENTEILE4]->LoadImage("kettenteil2.bmp", 11, 21, 11, 21, 1, 1);
	PartikelRect[KETTENTEILE4].left = 0;	PartikelRect[KETTENTEILE4].right  = 11;
	PartikelRect[KETTENTEILE4].top  = 0;	PartikelRect[KETTENTEILE4].bottom = 21;	

	// Regentropfen
	pPartikelGrafix[REGENTROPFEN]->LoadImage("rain.png", 16, 32, 16, 32, 1, 1);
	PartikelRect[REGENTROPFEN].left = 0;	PartikelRect[REGENTROPFEN].right  = 16;
	PartikelRect[REGENTROPFEN].top  = 0;	PartikelRect[REGENTROPFEN].bottom = 28;

	// Schutzschild
	pPartikelGrafix[SHIELD]->LoadImage("shield.bmp", 48, 48, 48, 48, 1, 1);
	PartikelRect[SHIELD].left = 0;	PartikelRect[SHIELD].right  = 48;
	PartikelRect[SHIELD].top  = 0;	PartikelRect[SHIELD].bottom = 48;	

	// Rauch des Rundumschusses des evil hurri
	pPartikelGrafix[EVILROUNDSMOKE]->LoadImage("evilroundsmoke.bmp", 12, 12, 12, 12, 1, 1);
	PartikelRect[EVILROUNDSMOKE].left = 0;	PartikelRect[EVILROUNDSMOKE].right  = 12;
	PartikelRect[EVILROUNDSMOKE].top  = 0;	PartikelRect[EVILROUNDSMOKE].bottom = 12;

	// Leuchteffekt für den Stelsack laser
	pPartikelGrafix[STELZFLARE]->LoadImage("giantspiderflare.bmp", 128, 128, 128, 128, 1, 1);
	PartikelRect[STELZFLARE].left = 0;	PartikelRect[STELZFLARE].right  = 128;
	PartikelRect[STELZFLARE].top  = 0;	PartikelRect[STELZFLARE].bottom = 128;

	// Kringel laden
	pPartikelGrafix[KRINGELSECRET]->LoadImage("kringel.bmp", 48, 12, 12, 12, 4, 1);
	PartikelRect[KRINGELSECRET].left = 0;	PartikelRect[KRINGELSECRET].right  = 12;
	PartikelRect[KRINGELSECRET].top  = 0;	PartikelRect[KRINGELSECRET].bottom = 12;

	// Kringel laden
	pPartikelGrafix[KRINGEL]->LoadImage("kringel.bmp", 48, 12, 12, 12, 4, 1);
	PartikelRect[KRINGEL].left = 0;	PartikelRect[KRINGEL].right  = 12;
	PartikelRect[KRINGEL].top  = 0;	PartikelRect[KRINGEL].bottom = 12;

	// Fog laden
	pPartikelGrafix[FOG]->LoadImage("dust.png", 30, 10, 10, 10, 3, 1);
	PartikelRect[FOG].left = 0;	PartikelRect[FOG].right  = 10;
	PartikelRect[FOG].top  = 0;	PartikelRect[FOG].bottom = 10;

	// Partikel für die Turbine
	pPartikelGrafix[TURBINESMOKE]->LoadImage("turbinesmoke.bmp", 24, 8, 8, 8, 3, 1);
	PartikelRect[TURBINESMOKE].left = 0;	PartikelRect[TURBINESMOKE].right  = 8;
	PartikelRect[TURBINESMOKE].top  = 0;	PartikelRect[TURBINESMOKE].bottom = 8;

	// MiniFlare
	pPartikelGrafix[MINIFLARE]->LoadImage("MINIFLARE.bmp", 32, 32, 32, 32, 1, 1);
	PartikelRect[MINIFLARE].left = 0;	PartikelRect[MINIFLARE].right  = 32;
	PartikelRect[MINIFLARE].top  = 0;	PartikelRect[MINIFLARE].bottom = 32;

	// Leuchten bei Granateneinschlag
	pPartikelGrafix[GRENADEFLARE]->LoadImage("grenadeflare.bmp", 256, 256, 256, 256, 1, 1);
	PartikelRect[GRENADEFLARE].left = 0;	PartikelRect[GRENADEFLARE].right  = 256;
	PartikelRect[GRENADEFLARE].top  = 0;	PartikelRect[GRENADEFLARE].bottom = 256;

	// Leuchten bei Explosion
	pPartikelGrafix[EXPLOSIONFLARE]->LoadImage("lavaflare.bmp", 120, 120, 120, 120, 1, 1);
	PartikelRect[EXPLOSIONFLARE].left = 0;	PartikelRect[EXPLOSIONFLARE].right  = 120;
	PartikelRect[EXPLOSIONFLARE].top  = 0;	PartikelRect[EXPLOSIONFLARE].bottom = 120;

	// Leuchten bei Explosion2
	pPartikelGrafix[EXPLOSIONFLARE2]->LoadImage("lavaflare.bmp", 120, 120, 120, 120, 1, 1);
	PartikelRect[EXPLOSIONFLARE2].left = 0;	PartikelRect[EXPLOSIONFLARE2].right  = 120;
	PartikelRect[EXPLOSIONFLARE2].top  = 0;	PartikelRect[EXPLOSIONFLARE2].bottom = 120;

	// Halswirbel des MetalHead laden
	pPartikelGrafix[HALSWIRBEL]->LoadImage("metalhead_halsteil.png", 64, 16, 64, 16, 1, 1);
	PartikelRect[HALSWIRBEL].left = 0;	PartikelRect[HALSWIRBEL].right  = 64;
	PartikelRect[HALSWIRBEL].top  = 0;	PartikelRect[HALSWIRBEL].bottom = 64;

	// Kaputte Turbine des MetalHead laden
	pPartikelGrafix[KAPUTTETURBINE]->LoadImage("metalhead_turbine.bmp",  98,  49, 98, 49, 1, 1);
	PartikelRect[KAPUTTETURBINE].left = 0;	PartikelRect[KAPUTTETURBINE].right  = 98;
	PartikelRect[KAPUTTETURBINE].top  = 0;	PartikelRect[KAPUTTETURBINE].bottom = 49;

	// Schleim
	pPartikelGrafix[SCHLEIM]->LoadImage("schleim.png", 32, 8, 8, 8, 4, 1);
	PartikelRect[SCHLEIM].left = 1;	PartikelRect[SCHLEIM].right  = 7;
	PartikelRect[SCHLEIM].top  = 1;	PartikelRect[SCHLEIM].bottom = 7;

	// Schleim2
	pPartikelGrafix[SCHLEIM2]->LoadImage("schleim2.png", 32, 8, 8, 8, 4, 1);
	PartikelRect[SCHLEIM2].left = 1;	PartikelRect[SCHLEIM2].right  = 7;
	PartikelRect[SCHLEIM2].top  = 1;	PartikelRect[SCHLEIM2].bottom = 7;

	// ShockWelle bei Spieler Explosion
	pPartikelGrafix[SHOCKEXPLOSION]->LoadImage("shockexplosion.bmp", 128, 128, 128, 128, 1, 1);
	PartikelRect[SHOCKEXPLOSION].left = 0;	PartikelRect[SHOCKEXPLOSION].right  = 128;
	PartikelRect[SHOCKEXPLOSION].top  = 0;	PartikelRect[SHOCKEXPLOSION].bottom = 128;

	// Leuchten bei Aufprall des Schusses
	pPartikelGrafix[SHOTFLARE]->LoadImage("shotflare.bmp", 16, 16, 16, 16, 1, 1);
	PartikelRect[SHOTFLARE].left = 0;	PartikelRect[SHOTFLARE].right  = 16;
	PartikelRect[SHOTFLARE].top  = 0;	PartikelRect[SHOTFLARE].bottom = 16;

	// Leuchten bei Aufprall des Schusses
	pPartikelGrafix[SHOTFLARE2]->LoadImage("shotflare.bmp", 16, 16, 16, 16, 1, 1);
	PartikelRect[SHOTFLARE2].left = 0;	PartikelRect[SHOTFLARE2].right  = 16;
	PartikelRect[SHOTFLARE2].top  = 0;	PartikelRect[SHOTFLARE2].bottom = 16;

	// Leuchten beim Einsammeln eines Extras
	pPartikelGrafix[EXTRACOLLECTED]->LoadImage("extracollected.bmp", 32, 32, 32, 32, 1, 1);
	PartikelRect[EXTRACOLLECTED].left = 0;	PartikelRect[EXTRACOLLECTED].right  = 32;
	PartikelRect[EXTRACOLLECTED].top  = 0;	PartikelRect[EXTRACOLLECTED].bottom = 32;

	// Leuchten beim Einsammeln eines Diamants
	pPartikelGrafix[DIAMANTCOLLECTED]->LoadImage("diamantcollected.bmp", 29, 29, 29, 29, 1, 1);
	PartikelRect[DIAMANTCOLLECTED].left = 0;	PartikelRect[DIAMANTCOLLECTED].right  = 29;
	PartikelRect[DIAMANTCOLLECTED].top  = 0;	PartikelRect[DIAMANTCOLLECTED].bottom = 29;

	// Lila
	pPartikelGrafix[LILA]->LoadImage("spidershotsmoke.bmp", 24, 24, 24, 24, 1, 1);
	PartikelRect[LILA].left = 0;	PartikelRect[LILA].right  = 29;
	PartikelRect[LILA].top  = 0;	PartikelRect[LILA].bottom = 29;

	// Rauch des Drachen
	pPartikelGrafix[DRACHE_SMOKE]->LoadImage("drache_smoke.bmp", 60, 60, 60, 60, 1, 1);
	PartikelRect[DRACHE_SMOKE].left = 0;	PartikelRect[DRACHE_SMOKE].right  = 60;
	PartikelRect[DRACHE_SMOKE].top  = 0;	PartikelRect[DRACHE_SMOKE].bottom = 60;

	// Rauch des Feuerballs
	pPartikelGrafix[FIREBALL_SMOKE]->LoadImage("fireball_smoke.bmp", 24, 24, 24, 24, 1, 1);
	PartikelRect[FIREBALL_SMOKE].left = 0;	PartikelRect[FIREBALL_SMOKE].right  = 24;
	PartikelRect[FIREBALL_SMOKE].top  = 0;	PartikelRect[FIREBALL_SMOKE].bottom = 24;

	// Leuchten beim Einsammeln eines Extras
	pPartikelGrafix[LASERFLARE]->LoadImage("lavaflare.bmp", 120, 120, 120, 120, 1, 1);
	PartikelRect[LASERFLARE].left = 0;	PartikelRect[LASERFLARE].right  = 120;
	PartikelRect[LASERFLARE].top  = 0;	PartikelRect[LASERFLARE].bottom = 120;	

	// Reguläre Explosion
	pPartikelGrafix[EXPLOSION_REGULAR]->LoadImage("explosion-regular.bmp", 380, 304, 76, 76, 5, 4);
	PartikelRect[EXPLOSION_REGULAR].left = 0;	PartikelRect[EXPLOSION_REGULAR].right  = 76;
	PartikelRect[EXPLOSION_REGULAR].top  = 0;	PartikelRect[EXPLOSION_REGULAR].bottom = 76;

	// Reguläre Explosion
	pPartikelGrafix[EXPLOSION_TRACE_END]->LoadImage("explosion-regular.bmp", 380, 304, 76, 76, 5, 4);
	PartikelRect[EXPLOSION_TRACE_END].left = 0;	PartikelRect[EXPLOSION_TRACE_END].right  = 76;
	PartikelRect[EXPLOSION_TRACE_END].top  = 0;	PartikelRect[EXPLOSION_TRACE_END].bottom = 76;

	// Staub
	pPartikelGrafix[DUST]->LoadImage("dust.png", 30, 10, 10, 10, 3, 1);
	PartikelRect[DUST].left = 0;	PartikelRect[DUST].right  = 10;
	PartikelRect[DUST].top  = 0;	PartikelRect[DUST].bottom = 10;

	// Staub
	pPartikelGrafix[LAVADUST]->LoadImage("dust.png", 30, 10, 10, 10, 3, 1);
	PartikelRect[LAVADUST].left = 0;	PartikelRect[LAVADUST].right  = 10;
	PartikelRect[LAVADUST].top  = 0;	PartikelRect[LAVADUST].bottom = 10;

	// Schrott
	pPartikelGrafix[SCHROTT1]->LoadImage("schrott1.png", 200, 200, 40, 40, 5, 4);
	PartikelRect[SCHROTT1].left = 10;	PartikelRect[SCHROTT1].right  = 30;
	PartikelRect[SCHROTT1].top  = 10;	PartikelRect[SCHROTT1].bottom = 30;

	pPartikelGrafix[SCHROTT2]->LoadImage("schrott2.png", 150, 120, 30, 30, 5, 4);
	PartikelRect[SCHROTT2].left = 5;	PartikelRect[SCHROTT2].right  = 25;
	PartikelRect[SCHROTT2].top  = 5;	PartikelRect[SCHROTT2].bottom = 25;

	ThunderAlpha = 0.0f;

	for (int i = 0; i < 3; i++)
		ThunderColor [i] = 0;

	DrawMode = MODE_NORMAL;
}

// --------------------------------------------------------------------------------------
// Destruktor : Löschen der ganzen Liste und Freigabe der Partikel-Grafiken
// --------------------------------------------------------------------------------------

PartikelsystemClass::~PartikelsystemClass(void)
{
	// Partikel-Liste komplett leeren
	ClearAll();

	// Partikel Grafiken freigeben
	for(int i=0; i<MAX_PARTIKELGFX; i++)
		if(pPartikelGrafix[i] != NULL)
		{
			delete(pPartikelGrafix[i]);
			pPartikelGrafix[i] = NULL;
		}
}

// --------------------------------------------------------------------------------------
// Partikel "Art" hinzufügen
// --------------------------------------------------------------------------------------

bool PartikelsystemClass::PushPartikel(float x, float y, int Art, PlayerClass* pParent)
{	
	if(NumPartikel >= MAX_PARTIKEL)			// Grenze überschritten ?
		return false;

    PartikelClass *pNew = new PartikelClass;		// Neuer zu erstellender Partikel

	if (pNew->CreatePartikel(x, y, Art, pParent) == false)	// neuen Partikel erzeugen	
		return false;

	if(pStart==NULL)						// Liste leer ?
	{
		pStart = pNew;						// Ja, dann neuer Partikel gleich erstes
		pEnd   = pNew;						// und letzter Partikel

		pStart->pNext=NULL;					// Next/Previous gibts nich, da wir
		pStart->pPrev=NULL;					// nur 1 Partikel haben
	} 
	else									// Liste ist NICHT leer
	{
		pEnd->pNext = pNew;					// Letzter Partikel zeigt auf den neuen
		pNew->pPrev = pEnd;					// Letzter Partikel ist nicht mehr das letzte
		
		pNew->pNext = NULL;					// Nach dem neuen Partikel kommt keiner mehr
		pEnd		= pNew;					// da es jetzt das letzte in der Liste ist
	}

	NumPartikel++;							// Partikelanzahl erhöhen
	return true;
}

// --------------------------------------------------------------------------------------
// Bestimmten Partikel der Liste löschen
// --------------------------------------------------------------------------------------

void PartikelsystemClass::DelSel(PartikelClass *pTemp)
{
	PartikelClass  *pN;
    PartikelClass  *pP;

	if(pTemp!=NULL)						// zu löschender Partikel existiert 
	{
		pN = pTemp->pNext;			
		pP = pTemp->pPrev;
  
		if(pP == NULL)					// Wird der erste Partikel gelöscht ?
			pStart = pN;				// Dann wird dessen Nächster zum Ersten
		else
			pP->pNext = pN;	   	        // ansonsten normal eins aufrücken

		if(pN == NULL)					// Wird der letzte Partikel gelöscht ?
			pEnd = pP;					// Dann wir der letzte Partikel zum ersten
		else
		pN->pPrev = pP;
  
		delete (pTemp);					// Speicher freigeben
		pTemp = NULL;

		NumPartikel--;					// Partikelzahl verringern
	}
}

// --------------------------------------------------------------------------------------
// Alle Partikel der Liste löschen
// --------------------------------------------------------------------------------------

void PartikelsystemClass::ClearAll(void)
{
    PartikelClass *pTemp    = pStart;				// Zeiger auf den ersten   Partikel
	PartikelClass *pNaechst;						// Zeiger auf den nächsten Partikel (falls
													// das eine gelöscht wird)
    while (pTemp != NULL)							// Ende der Liste erreicht ?
    {
		pNaechst = pTemp->pNext;					// Zeiger auf das nächste Element
		DelSel(pTemp);								// Das aktuelle löschen
        pTemp = pNaechst;							// und das nächste bearbeiten
    }

	pStart = NULL;
	pEnd   = NULL;
}

// --------------------------------------------------------------------------------------
// Zahl der Partikel zurückliefern
// --------------------------------------------------------------------------------------

int PartikelsystemClass::GetNumPartikel(void)
{
	return NumPartikel;
}

// --------------------------------------------------------------------------------------
// Alle Partikel der Liste nur anzeigen
// --------------------------------------------------------------------------------------

void PartikelsystemClass::DrawOnly(void)
{
	PartikelClass *pTemp = pStart;			// Anfang der Liste

	CurrentPartikelTexture = -1;			// Aktuelle Textur gibt es noch keine

	DrawMode = MODE_NORMAL;

//----- Partikel, die normal oder mit Alphablending gerendert werden, durchlaufen

	DirectGraphics.SetColorKeyMode();
	while (pTemp != NULL)					// Noch nicht alle durch ?
	{
		if (pTemp->PartikelArt < ADDITIV_GRENZE)
			pTemp->Render();

		pTemp = pTemp->pNext;				// Und nächsten Partikel anhandeln
	}

//----- Partikel, die mit additivem Alphablending gerendert werden, durchlaufen

	pTemp = pStart;
	CurrentPartikelTexture = -1;			// Aktuelle Textur gibt es noch keine
	DirectGraphics.SetAdditiveMode();
	while (pTemp != NULL)					// Noch nicht alle durch ?
	{
		if (pTemp->PartikelArt >= ADDITIV_GRENZE)
			pTemp->Render();

		pTemp = pTemp->pNext;				// Und nächsten Partikel anhandeln
	}

	DirectGraphics.SetColorKeyMode();

	// Normale Projektions-Matrix wieder herstellen
	D3DXMatrixRotationZ (&matWorld, 0.0f);
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
}

// --------------------------------------------------------------------------------------
// Alle Partikel der Liste animieren und anzeigen, aber speziale
// --------------------------------------------------------------------------------------

void PartikelsystemClass::DoPartikelSpecial(bool ShowThem)
{
	if (pConsole->Showing == true)
	{
		DrawOnly();
		return;
	}		

	PartikelClass *pTemp = pStart;			// Anfang der Liste
	PartikelClass *pNext = NULL;			// Nächster Partikel in der Liste

	CurrentPartikelTexture = -1;			// Aktuelle Textur gibt es noch keine

	DrawMode = MODE_NORMAL;

//----- Partikel, die normal oder mit Alphablending gerendert werden, durchlaufen

	DirectGraphics.SetColorKeyMode();
	while (pTemp != NULL)					// Noch nicht alle durch ?
	{
		if (ShowThem == false &&
			pTemp->PartikelArt < ADDITIV_GRENZE)
		{
			pTemp->Run();					// Partikel animieren/bewegen

			if (pTemp->Lebensdauer > 0.0f)
				pTemp->Render();
		}

		pNext = pTemp->pNext;				// Nächstes sichern

		if (pTemp->Lebensdauer <= 0.0f) 	// ggf Partikel löschen (bei Lebensdauer == 0)
			DelSel(pTemp);

		pTemp = pNext;						// Und nächsten Partikel anhandeln
	}

//----- Partikel, die mit additivem Alphablending gerendert werden, durchlaufen

	pTemp = pStart;
	pNext = NULL;
	CurrentPartikelTexture = -1;			// Aktuelle Textur gibt es noch keine
	DirectGraphics.SetAdditiveMode();
	while (pTemp != NULL)					// Noch nicht alle durch ?
	{
		if ((ShowThem == true &&
			(pTemp->PartikelArt == SCHNEEFLOCKE_END ||
			 pTemp->PartikelArt == EXPLOSION_TRACE_END)) ||
			(ShowThem == false &&
			 pTemp->PartikelArt >= ADDITIV_GRENZE))
		{
			pTemp->Run();		// Partikel animieren/bewegen

			if (pTemp->Lebensdauer > 0.0f)
				pTemp->Render();
		}

		pNext = pTemp->pNext;				// Nächstes sichern

		if (pTemp->Lebensdauer <= 0.0f) 	// ggf Partikel löschen (bei Lebensdauer == 0)
			DelSel(pTemp);

		pTemp = pNext;						// Und nächsten Partikel anhandeln
	}

	DirectGraphics.SetColorKeyMode();

	// Normale Projektions-Matrix wieder herstellen
	D3DXMatrixRotationZ (&matWorld, 0.0f);
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
}

// --------------------------------------------------------------------------------------
// Alle Partikel der Liste animieren und anzeigen
// --------------------------------------------------------------------------------------

void PartikelsystemClass::DoPartikel(void)
{
	if (pConsole->Showing == true)
	{
		DrawOnly();
		return;
	}		

	PartikelClass *pTemp = pStart;			// Anfang der Liste
	PartikelClass *pNext = NULL;			// Nächster Partikel in der Liste

	CurrentPartikelTexture = -1;			// Aktuelle Textur gibt es noch keine

	DrawMode = MODE_NORMAL;

//----- Partikel, die normal oder mit Alphablending gerendert werden, durchlaufen

	DirectGraphics.SetColorKeyMode();
	while (pTemp != NULL)					// Noch nicht alle durch ?
	{
		if (pTemp->PartikelArt < ADDITIV_GRENZE)
		{
			pTemp->Run();		// Partikel animieren/bewegen
			pTemp->Render();
		}

		pNext = pTemp->pNext;				// Nächstes sichern

		if (pTemp->Lebensdauer <= 0.0f) 	// ggf Partikel löschen (bei Lebensdauer == 0)
			DelSel(pTemp);

		pTemp = pNext;						// Und nächsten Partikel anhandeln
	}

//----- Partikel, die mit additivem Alphablending gerendert werden, durchlaufen

	pTemp = pStart;
	pNext = NULL;
	CurrentPartikelTexture = -1;			// Aktuelle Textur gibt es noch keine
	DirectGraphics.SetAdditiveMode();
	while (pTemp != NULL)					// Noch nicht alle durch ?
	{
		if (pTemp->PartikelArt >= ADDITIV_GRENZE)
		{
			pTemp->Run();		// Partikel animieren/bewegen
			pTemp->Render();
		}

		pNext = pTemp->pNext;				// Nächstes sichern

		if (pTemp->Lebensdauer <= 0.0f) 	// ggf Partikel löschen (bei Lebensdauer == 0)
			DelSel(pTemp);

		pTemp = pNext;						// Und nächsten Partikel anhandeln
	}

	DirectGraphics.SetColorKeyMode();

	// Normale Projektions-Matrix wieder herstellen
	D3DXMatrixRotationZ (&matWorld, 0.0f);
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
}

// --------------------------------------------------------------------------------------
// Blitz und Döner zeigen ;)
// --------------------------------------------------------------------------------------

void PartikelsystemClass::DoThunder (void)
{
	if (ThunderAlpha > 0.0f)
	{
		D3DCOLOR col = D3DCOLOR_RGBA (ThunderColor[0], ThunderColor[1], ThunderColor[2], int (ThunderAlpha));
		RenderRect (0, 0, 640, 480, col);
		ThunderAlpha -= 40.0f SYNC;
	}
}

// --------------------------------------------------------------------------------------
// PowerUp Partikel löschen, falls schon welche vorhanden
// damit kein so bunter bonbon scheiss entsteht, wenn mehrere extras gleichzeitig aufgepowert werden
// --------------------------------------------------------------------------------------

void PartikelsystemClass::ClearPowerUpEffects(void)
{
	PartikelClass* pTemp;
	PartikelClass* pNext;

	pTemp = pPartikelSystem->pStart;

	while (pTemp != NULL)
	{
		pNext = pTemp->pNext;

		if (pTemp->PartikelArt == KRINGEL)
			pPartikelSystem->DelSel(pTemp);

		pTemp = pNext;
	}
}

// --------------------------------------------------------------------------------------
// Je nach Detailstufe wird ein anderer Wert für MAX_PARTIKEL gesetzt
// --------------------------------------------------------------------------------------

void PartikelsystemClass::SetParticleCount(void)
{
	switch(options_Detail)
	{
		case DETAIL_LOW:
			MAX_PARTIKEL = 2000;
			break;

		case DETAIL_MEDIUM:
			MAX_PARTIKEL = 3000;
			break;

		case DETAIL_HIGH:
			MAX_PARTIKEL = 4000;
			break;

		case DETAIL_MAXIMUM:
			MAX_PARTIKEL = 5000;
			break;
	}
}