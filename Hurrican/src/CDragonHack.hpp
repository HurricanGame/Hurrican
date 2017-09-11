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

#include "GegnerClass.hpp"
#include "DX8Input.hpp"
#include "DX8Sound.hpp"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define STATE_WAIT 0
#define STATE_FLY 1

// --------------------------------------------------------------------------------------
// Dragon-Klasse
// --------------------------------------------------------------------------------------

class CDragonHack
{
private:
    float					m_AppearCount;
    DirectGraphicsSprite	m_GFX;
    float					m_xPos, m_yPos;
    float					m_xSpeed, m_ySpeed;
    float					m_AnimCount;

public:
    int						m_State;

    CDragonHack	 (void);						// CutScene initialisieren
    ~CDragonHack	 (void);						// CutScene freigeben

    void Run			 (void);							// laufen, pardon: "fliegen" lassen
    // abgebrochen wurde
}; // CDragonHack

#endif
