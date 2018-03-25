// Datei : Outtro.hpp

// --------------------------------------------------------------------------------------
//
// Klasse für das Hurrican Outtro
//
// (c) 2006 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _OUTTRO_HPP_
#define _OUTTRO_HPP_

// --------------------------------------------------------------------------------------
// Inlcudces
// --------------------------------------------------------------------------------------

#include "DX8Font.hpp"
#include "DX8Sprite.hpp"
#include "Globals.hpp"
#include "Timer.hpp"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

//----- Zustände

#define OUTTRO_FADEIN 0
#define OUTTRO_TOWER_EXPLODE 1
#define OUTTRO_PLAYER_FLEES 2
#define OUTTRO_SCROLLER 3

// --------------------------------------------------------------------------------------
// Klassen Deklaration für das Outtro
// --------------------------------------------------------------------------------------

class OuttroClass {
  private:
    DirectGraphicsSprite Background;
    DirectGraphicsSprite Tower;
    DirectGraphicsSprite Henry;
    DirectGraphicsSprite Reiter[2];
    bool finished;
    int Zustand;
    float Counter;
    int TextOff;
    float SmokeDelay;
    float TowerOffset;
    float Snow;
    float PlayerSmoke;
    float px[2], py[2], xs[2], ys[2];

    void InitPlayerPos();

  public:
    OuttroClass();    // Konstruktor
    ~OuttroClass();   // Destruktor
    void DoOuttro();  // Intro ablaufen lassen
};

#endif
