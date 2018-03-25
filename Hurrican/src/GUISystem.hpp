#ifndef _GUISYSTEM_HPP_
#define _GUISYSTEM_HPP_

#include "DX8Font.hpp"
#include "DX8Graphics.hpp"
#include "DX8Sprite.hpp"

// --------------------------------------------------------------------------------------
// GUI Klasse
// --------------------------------------------------------------------------------------

class CGUISystem {
#define TILESIZE 20

#define INVISIBLE 0
#define VISIBLE 1
#define FADEIN 3
#define FADEOUT 4
#define FADESPEED 40.0f
#define MAXFADE 200.0f

#define BOXTEXTLENGTH 2000

#define BOXSIZEMAX 480

  private:
    float m_xPos, m_yPos;
    DirectGraphicsSprite m_Rahmen;
    RECT m_BoxSize;
    char m_BoxText[BOXTEXTLENGTH];
    int m_BoxLines;

  public:
    int m_FadeMode;
    float m_FadingAlpha;
    int m_TextID;

    CGUISystem();
    ~CGUISystem();

    void InitGUISystem();

    void Run();
    void RenderBox();
    void ShowBox(const char Text[BOXTEXTLENGTH], int yoff, int xoff = 320);
    void ShowBox(int xoff, int yoff, int w, int h);
    void HideBox();
    void HideBoxFast();
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern CGUISystem GUI;

#endif
