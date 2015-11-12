// Datei : HUD.h

// --------------------------------------------------------------------------------------
//
// Funktionen für das Hurrican HUD
// Werte anzeigen usw
//
// (c) 2002 Jörg M. Winterstein
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

#define	ENERGY_BALL_HEIGHT	48					// Höhe der Energyball Grafik (für Umrechnung)
#define ARMOUR_BALL_HEIGHT	48					// dito für Armour
#define BLITZ_STAT_HEIGHT	18					// Höhe der Blitz-Status Grafik

// --------------------------------------------------------------------------------------
// HUD-Klasse
// --------------------------------------------------------------------------------------

class HUDClass
{
private:
    float					xpos, ypos;				// Position  des HUDs

    DirectGraphicsSprite	HUDGfx;					// Grafiken für das HUD
    DirectGraphicsSprite	HUDBomb;				// Smartbomb eingesammelt ?
    DirectGraphicsSprite	HUDFontBig;				// Grosser Font für HUD Darstellung
    DirectGraphicsSprite	HUDBall[2];				// Grafiken des Energy- und Radbalkens
    DirectGraphicsSprite	SelectedWeapon;			// Welche Waffe wurde gewählt ?
    DirectGraphicsSprite	WeaponRahmen;			// Balken um Waffenstufe
    DirectGraphicsSprite	WeaponPunkt;			// Ein Punkt im Waffenrahmen
    DirectGraphicsSprite	HUDBlitz;				// Aktuelle Blitz-Stärke
    DirectGraphicsSprite	HUDAutoFire;			// AutoFire Symbol
    DirectGraphicsSprite	HUDSuperShot;			// SuperShot Symbol

    DirectGraphicsSprite	BossHUD;				// Boss HUD für die Boss Energie
    DirectGraphicsSprite	BossBar;				// Boss Energiebalken

public:
    float					BossHUDActive;			// Boss HUD wird angezeigt ?
    unsigned char			red, green, blue;		// Farbwerte des HUDS
    unsigned char			Alpha;					// Alphawert des HUDS

    DirectGraphicsSprite	Arrow;					// Pfeil fürs Tutorial

    float					ArrowX;					// Werte für den Pfeil im Tutorial Level
    float					ArrowY;
    bool					bShowArrow;

    HUDClass		 (void);					// HUD initialisieren und Grafiken laden
    ~HUDClass		 (void);					// HUD freigeben

    //DKS - Added LoadSprites() function and moved these here from class constructor,
    //      to allow statically-allocated HUD global var (used to be dynamically allocated)
    void LoadSprites(void);

    void UpdateValues  (void);						// Werte im HUD ändern
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

extern HUDClass				HUD;

#endif
