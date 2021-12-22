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
    StateEnum State;
    float ScrollOffset, ScrollOffset2;
    float SinPos;
    int ScrollPos, ScrollPos2;
    bool b_running;

    StarStruct Stars[200];

  public:
    CCracktro();   // Cracktro initialisieren
    ~CCracktro();  // Cracktro freigeben

    void Run();  // Cracktro laufen lassen
    void Main();
    void Load();

    bool IsRunning() const { return b_running; }
};  // CCracktro

#endif
