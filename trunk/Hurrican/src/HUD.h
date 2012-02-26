// Datei : HUD.h

// -------------------------------------------------------------------------------------- 
//
// Funktionen f�r das Hurrican HUD
// Werte anzeigen usw
//
// (c) 2002 J�rg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __HUD_h__
#define __HUD_h__

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "Globals.h"
#include "DX8Sprite.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define	ENERGY_BALL_HEIGHT	48					// H�he der Energyball Grafik (f�r Umrechnung)
#define ARMOUR_BALL_HEIGHT	48					// dito f�r Armour
#define BLITZ_STAT_HEIGHT	18					// H�he der Blitz-Status Grafik

// --------------------------------------------------------------------------------------
// HUD-Klasse
// --------------------------------------------------------------------------------------

class HUDClass
{
	private:
		float					xpos, ypos;				// Position  des HUDs

		DirectGraphicsSprite	HUDGfx;					// Grafiken f�r das HUD
		DirectGraphicsSprite	HUDBomb;				// Smartbomb eingesammelt ?
		DirectGraphicsSprite	HUDFontBig;				// Grosser Font f�r HUD Darstellung
		DirectGraphicsSprite	HUDBall[2];				// Grafiken des Energy- und Radbalkens
		DirectGraphicsSprite	SelectedWeapon;			// Welche Waffe wurde gew�hlt ?
		DirectGraphicsSprite	WeaponRahmen;			// Balken um Waffenstufe
		DirectGraphicsSprite	WeaponPunkt;			// Ein Punkt im Waffenrahmen
		DirectGraphicsSprite	HUDBlitz;				// Aktuelle Blitz-St�rke
		DirectGraphicsSprite	HUDAutoFire;			// AutoFire Symbol
		DirectGraphicsSprite	HUDSuperShot;			// SuperShot Symbol		

		DirectGraphicsSprite	BossHUD;				// Boss HUD f�r die Boss Energie
		DirectGraphicsSprite	BossBar;				// Boss Energiebalken			

	public:
		float					BossHUDActive;			// Boss HUD wird angezeigt ?
		unsigned char			red, green, blue;		// Farbwerte des HUDS
		unsigned char			Alpha;					// Alphawert des HUDS

		DirectGraphicsSprite	Arrow;					// Pfeil f�rs Tutorial

		float					ArrowX;					// Werte f�r den Pfeil im Tutorial Level
		float					ArrowY;
		bool					bShowArrow;

		     HUDClass		 (void);					// HUD initialisieren und Grafiken laden
			~HUDClass		 (void);					// HUD freigeben

		void UpdateValues  (void);						// Werte im HUD �ndern
		void ShowHUD	   (void);						// HUD anzeigen
		void RenderBossHUD (void);						// Boss HUD anzeigen
		void DoHUD	       (void);						// Alles machen, was da HUD betrifft =)
		void ShowBossHUD   (float max, float act);		// Boss Energie anzeigen
		void ShowArrow     (float x, float y);			// Pfeil in diesem Frame anzeigen
		void RenderArrow   (void);						// Pfeil rendern
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern HUDClass				*pHUD;

#endif