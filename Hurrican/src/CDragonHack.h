// Datei : CDragonHack.h

// --------------------------------------------------------------------------------------
//
// Klasse für den Drache, der im Hintergrund beim Turmlevel rumfliegt
//
// --------------------------------------------------------------------------------------

#ifndef _DRAGONHACK_h_
#define _DRAGONHACK_h_

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "GegnerClass.h"
#include "DX8Input.h"
#include "DX8Sound.h"

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
