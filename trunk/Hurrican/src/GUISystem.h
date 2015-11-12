#ifndef _GUISYSTEM_H_
#define _GUISYSTEM_H_

#include "DX8Font.h"
#include "DX8Graphics.h"
#include "DX8Sprite.h"

// --------------------------------------------------------------------------------------
// GUI Klasse
// --------------------------------------------------------------------------------------

class CGUISystem
{
#define TILESIZE		20

#define INVISIBLE		0
#define VISIBLE			1
#define FADEIN			3
#define FADEOUT			4
#define FADESPEED		40.0f
#define MAXFADE			200.0f

#define BOXTEXTLENGTH	2000

#define BOXSIZEMAX		480

private:
    float					m_xPos, m_yPos;
    DirectGraphicsSprite	m_Rahmen;
    RECT					m_BoxSize;
    char					m_BoxText[BOXTEXTLENGTH];
    int						m_BoxLines;

public:

    int						m_FadeMode;
    float					m_FadingAlpha;
    int						m_TextID;

    CGUISystem(void);
    ~CGUISystem(void);

    void InitGUISystem(void);

    void Run();
    void RenderBox();
    void ShowBox(char Text[BOXTEXTLENGTH], int yoff, int xoff = 320);
    void ShowBox(int xoff, int yoff, int w, int h);
    void HideBox();
    void HideBoxFast();
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern CGUISystem GUI;

#endif
