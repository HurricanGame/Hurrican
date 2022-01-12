// Datei : HUD.hpp

// --------------------------------------------------------------------------------------
//
// Funktionen für das Hurrican HUD
// Werte anzeigen usw
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _HUD_HPP_
#define _HUD_HPP_

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "DX8Sprite.hpp"
#include "Globals.hpp"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

constexpr int ENERGY_BALL_HEIGHT = 48;  // Höhe der Energyball Grafik (für Umrechnung)
constexpr int ARMOUR_BALL_HEIGHT = 48;  // dito für Armour
constexpr int BLITZ_STAT_HEIGHT = 18;   // Höhe der Blitz-Status Grafik

// --------------------------------------------------------------------------------------
// HUD-Klasse
// --------------------------------------------------------------------------------------

class HUDClass {
  private:
    float xpos, ypos;       // Position  des HUDs
    float BossHUDActive;    // Boss HUD wird angezeigt ?

    DirectGraphicsSprite HUDGfx;          // Grafiken für das HUD
    DirectGraphicsSprite HUDBomb;         // Smartbomb eingesammelt ?
    DirectGraphicsSprite HUDFontBig;      // Grosser Font für HUD Darstellung
    DirectGraphicsSprite HUDBall[2];      // Grafiken des Energy- und Radbalkens
    DirectGraphicsSprite SelectedWeapon;  // Welche Waffe wurde gewählt ?
    DirectGraphicsSprite WeaponRahmen;    // Balken um Waffenstufe
    DirectGraphicsSprite WeaponPunkt;     // Ein Punkt im Waffenrahmen
    DirectGraphicsSprite HUDBlitz;        // Aktuelle Blitz-Stärke
    DirectGraphicsSprite HUDAutoFire;     // AutoFire Symbol
    DirectGraphicsSprite HUDSuperShot;    // SuperShot Symbol

    DirectGraphicsSprite BossHUD;  // Boss HUD für die Boss Energie
    DirectGraphicsSprite BossBar;  // Boss Energiebalken

  public:
    unsigned char red, green, blue;  // Farbwerte des HUDS
    unsigned char Alpha;             // Alphawert des HUDS

    DirectGraphicsSprite Arrow;  // Pfeil fürs Tutorial

    float ArrowX;  // Werte für den Pfeil im Tutorial Level
    float ArrowY;
    bool bShowArrow;

    HUDClass();   // HUD initialisieren und Grafiken laden
    ~HUDClass();  // HUD freigeben

    // DKS - Added LoadSprites() function and moved these here from class constructor,
    //      to allow statically-allocated HUD global var (used to be dynamically allocated)
    void LoadSprites();

    void UpdateValues();                 // Werte im HUD ändern
    void ShowHUD();                      // HUD anzeigen
    void RenderBossHUD();                // Boss HUD anzeigen
    void DoHUD();                        // Alles machen, was da HUD betrifft =)
    void ShowBossHUD(float max, float act);  // Boss Energie anzeigen
    void ShowArrow(float x, float y);        // Pfeil in diesem Frame anzeigen
    void RenderArrow();                  // Pfeil rendern

    inline bool isBossHUDActive() const { return BossHUDActive != 0.0f; }
    inline void deactivateBossHUD() { BossHUDActive = 0.0f; }
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern HUDClass HUD;

#endif
