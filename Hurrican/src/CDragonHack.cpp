// Datei : CDragonHack.cpp

// --------------------------------------------------------------------------------------
//
// Klasse für den Drache, der im Hintergrund beim Turmlevel rumfliegt
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "CDragonHack.hpp"
#include "Tileengine.hpp"
#include "Timer.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

CDragonHack::CDragonHack() {
    m_GFX.LoadImage("drache_klein.png", 160, 160, 160, 40, 1, 4);

    m_AnimCount = 0.0f;
    m_xPos = 0.0f;
    m_yPos = 0.0f;
    m_xSpeed = 0.0f;
    m_ySpeed = 0.0f;
    m_State = DragonState::WAIT;

    m_AppearCount = 5.0f;
}

// --------------------------------------------------------------------------------------
// Destruktor
// --------------------------------------------------------------------------------------

CDragonHack::~CDragonHack() {}

// --------------------------------------------------------------------------------------
// rumfliegen lassen
// --------------------------------------------------------------------------------------

void CDragonHack::Run() {
    m_AnimCount += Timer.sync(3.0f);

    if (m_AnimCount >= 4.0f)
        m_AnimCount = 0.0f;

    switch (m_State) {
        // nicht zu sehen
        case DragonState::WAIT: {
            if (m_AppearCount > 0.0f)
                m_AppearCount -= Timer.sync(1.0f);
            else {
                m_AppearCount = 20.0f;
                m_State = DragonState::FLY;

                // von links
                if (random(2) == 0) {
                    m_xPos = TileEngine.XOffset - 160;
                    m_yPos = TileEngine.YOffset + 200;
                    m_xSpeed = 10.0f;
                }

                // von rechts
                else {
                    m_xPos = TileEngine.XOffset + RENDERWIDTH + 160.0f;
                    m_yPos = TileEngine.YOffset + 200;
                    m_xSpeed = -10.0f;
                }

                m_ySpeed = static_cast<float>(random(9) - 4) / 5.0f;
            }
        } break;

        // fliegen lassen, bis er aus dem Bild ist
        case DragonState::FLY: {
            bool mirrored = m_xSpeed > 0.0f;

            m_GFX.RenderSprite(m_xPos - TileEngine.XOffset,
                               m_yPos - TileEngine.YOffset, m_AnimCount, 0xDDFFFFFF, mirrored);

            m_xPos += Timer.sync(m_xSpeed);
            m_yPos += Timer.sync(m_ySpeed);

            // Ausserhalb des sichtbaren Bereichs? Dann verschwinden lassen
            if (m_xPos + 260.0f < TileEngine.XOffset || m_xPos - 260.0f > TileEngine.XOffset + RENDERWIDTH ||
                m_yPos + 240.0f < TileEngine.YOffset || m_yPos - 240.0f > TileEngine.YOffset + RENDERHEIGHT) {
                m_State = DragonState::WAIT;
                m_AppearCount = random(100) + 50.0f;
            }
        } break;
    }
}
