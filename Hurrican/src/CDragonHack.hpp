// Datei : CDragonHack.hpp

// --------------------------------------------------------------------------------------
//
// Klasse für den Drache, der im Hintergrund beim Turmlevel rumfliegt
//
// --------------------------------------------------------------------------------------

#ifndef _DRAGONHACK_HPP_
#define _DRAGONHACK_HPP_

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "DX8Input.hpp"
#include "DX8Sound.hpp"
#include "GegnerClass.hpp"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

enum class DragonState {
  WAIT,
  FLY
};

// --------------------------------------------------------------------------------------
// Dragon-Klasse
// --------------------------------------------------------------------------------------

class CDragonHack {
  private:
    float m_AppearCount;
    DirectGraphicsSprite m_GFX;
    float m_xPos, m_yPos;
    float m_xSpeed, m_ySpeed;
    float m_AnimCount;

  public:
    DragonState m_State;

    CDragonHack();   // CutScene initialisieren
    ~CDragonHack();  // CutScene freigeben

    void Run();  // laufen, pardon: "fliegen" lassen
    // abgebrochen wurde
};  // CDragonHack

#endif
