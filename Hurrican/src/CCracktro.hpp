// Datei : CCracktro.hpp

// --------------------------------------------------------------------------------------
//
// Klasse für das Cracktro am Anfang
//
// (c) 2005 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _CRACKTRO_HPP_
#define _CRACKTRO_HPP_

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "DX8Font.hpp"
#include "DX8Input.hpp"
#include "DX8Sprite.hpp"
#include "Globals.hpp"
#include "Menu.hpp"
#include "Timer.hpp"

// --------------------------------------------------------------------------------------
// Cracktro-Klasse
// --------------------------------------------------------------------------------------

class CCracktro {
  private:
    enum class StateEnum { MAIN, LOAD };

  private:
    DirectGraphicsSprite Bars[3];
    DirectGraphicsSprite Logo[2];
    DirectGraphicsSprite Zahlen;
    DirectGraphicsSprite Star;
    DirectGraphicsFont *pFont;
    float ScrollOffset, ScrollOffset2;
    int ScrollPos, ScrollPos2;
    float SinPos;
    StateEnum State;

    StarStruct Stars[200];

  public:
    bool b_running;

    CCracktro();   // Cracktro initialisieren
    ~CCracktro();  // Cracktro freigeben

    void Run();  // Cracktro laufen lassen
    void Main();
    void Load();
};  // CCracktro

#endif
