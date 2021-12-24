#ifndef _GUISYSTEM_HPP_
#define _GUISYSTEM_HPP_

#include "DX8Font.hpp"
#include "DX8Graphics.hpp"
#include "DX8Sprite.hpp"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

constexpr int TILESIZE = 20;

constexpr int BOXTEXTLENGTH = 2000;

enum class FadeMode {
  INVISIBLE,
  VISIBLE,
  FADEIN,
  FADEOUT,
};

// --------------------------------------------------------------------------------------
// GUI Klasse
// --------------------------------------------------------------------------------------

class CGUISystem {
  private:
    float m_xPos, m_yPos;
    DirectGraphicsSprite m_Rahmen;
    RECT_struct m_BoxSize;
    char m_BoxText[BOXTEXTLENGTH];
    int m_BoxLines;

    FadeMode m_FadeMode;
    float m_FadingAlpha;
    int m_TextID;

  public:
    CGUISystem();
    ~CGUISystem();

    void InitGUISystem();

    void Run();
    void RenderBox();
    void ShowBox(const char Text[BOXTEXTLENGTH], int yoff, int xoff = 320);
    void ShowBox(int xoff, int yoff, int w, int h);
    void HideBox();
    void HideBoxFast();

    FadeMode GetFadeMode() const { return m_FadeMode; }
    float GetFadingAlpha() const { return m_FadingAlpha; }
    void SetTextID(int TextID) { m_TextID = TextID; }
    int GetTextID() const { return m_TextID; }
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern CGUISystem GUI;

#endif
