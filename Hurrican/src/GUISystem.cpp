#include "Timer.h"
#include "GUISystem.h"
#include "Main.h"

//
// Konstruktor
//

CGUISystem::CGUISystem(void)
{
}

//
// Destruktor
//

CGUISystem::~CGUISystem(void)
{
}

//
// GUISystem initialisieren
//

void CGUISystem::InitGUISystem(void)
{
    m_Rahmen.LoadImage("guiback.png", 60, 60, 20, 20, 3, 3);

    m_BoxSize.top	 = 0;
    m_BoxSize.left	 = 0;
    m_BoxSize.right	 = 0;
    m_BoxSize.bottom = 0;

    m_FadingAlpha = 0.0f;
    m_FadeMode    = INVISIBLE;

    m_BoxLines = 0;

    m_TextID = -1;
}

//
// Hinweisbox rendern
//

void CGUISystem::RenderBox(void)
{
    D3DCOLOR color;
    int		 alpha = int (m_FadingAlpha);

    if (alpha < 0)
        return;

    color = D3DCOLOR_RGBA (0, 255, 0, alpha);

    // Ecken
    //
    m_Rahmen.RenderSprite(m_xPos,
                          m_yPos, 0, color);

    m_Rahmen.RenderSprite(m_xPos + m_BoxSize.right + TILESIZE,
                          m_yPos, 2, color);

    m_Rahmen.RenderSprite(m_xPos,
                          m_yPos + m_BoxSize.bottom + TILESIZE, 6, color);

    m_Rahmen.RenderSprite(m_xPos + m_BoxSize.right  + TILESIZE,
                          m_yPos + m_BoxSize.bottom + TILESIZE, 8, color);

    // Rand oben/unten
    for (int i = 0; i < (m_BoxSize.right) / TILESIZE; i++)
    {
        m_Rahmen.RenderSprite(m_xPos + (i + 1) * TILESIZE, m_yPos, 1, color);
        m_Rahmen.RenderSprite(m_xPos + (i + 1) * TILESIZE, m_yPos + m_BoxSize.bottom + 20, 7, color);
    }

    // Rand links/rechts
    for (int i = 0; i < (m_BoxSize.bottom) / TILESIZE; i++)
    {
        m_Rahmen.RenderSprite(m_xPos, m_yPos + (i + 1) * TILESIZE, 3, color);
        m_Rahmen.RenderSprite(m_xPos + m_BoxSize.right + 20, m_yPos + (i + 1) * TILESIZE, 5, color);
    }

    // Hintergrund in der Mitte
    for (int i = 0; i < (m_BoxSize.right) / TILESIZE; i++)
        for (int j = 0; j < (m_BoxSize.bottom) / TILESIZE; j++)
            m_Rahmen.RenderSprite(m_xPos + (i + 1) * TILESIZE,
                                  m_yPos + (j + 1) * TILESIZE, 4, color);

    // Text rendern
    int y_txt_offset = -5;

    if (CommandLineParams.LowRes) {
        // No updwards shift needed on low-res devices using scaled font:
        y_txt_offset = 0;
    }

    pDefaultFont->DrawText(m_xPos + TILESIZE * 3 / 2 - 8,
                            m_yPos + TILESIZE * 3 / 2 + y_txt_offset,
                            m_BoxText, color);
}

//
// Hinweisbox für Tutorial Level einblenden
//

//DKS - Made line splitting more flexible, centered display of boxes on-screen and added low-resolution support
void CGUISystem::ShowBox(char Text[BOXTEXTLENGTH], int yoff, int xoff /* = 320 */)
{

    char buf1[BOXTEXTLENGTH+1], buf2[BOXTEXTLENGTH+1], srcbuf[BOXTEXTLENGTH+1];
    bool done = false;
    int longest_length = 0;
    int this_length = 0;
    const int max_length = BOXSIZEMAX - 60;

    // Start with a full buffer in srcbuf
    strcpy_s(srcbuf, Text);

    // And an empty one in m_BoxText
    m_BoxText[0] = '\0';
    m_BoxLines = 0;

    while (!done) {
        // Copy as much as will fit into buf1, remainder without leading whitespace into buf2.
        done = !ExtractStringOfLength(buf1, buf2, srcbuf, max_length, pDefaultFont);

        if (done) {
            // Line did not need to be split, so nothing was placed in buf1 or buf2
            strcat_s(m_BoxText, srcbuf);    // Copy all of srcbuf into m_BoxText
            this_length = pDefaultFont->StringLength(srcbuf);
        } else {
            // Line was split, part that will fit is in dst1, remainder in dst2
            strcat_s(m_BoxText, buf1);
            strcpy_s(srcbuf, buf2); // Use remainder as source buffer
            this_length = pDefaultFont->StringLength(buf1);
        }

        if (this_length > longest_length) {
            longest_length = this_length;
        }

        m_BoxLines++;
    }
        
    m_BoxSize.right  = longest_length / TILESIZE * TILESIZE + TILESIZE;
    m_BoxSize.bottom = (m_BoxLines * (pDefaultFont->GetYCharSize() + 6)) / TILESIZE * TILESIZE + TILESIZE;

    m_xPos = xoff - m_BoxSize.right/2 - TILESIZE;
    m_yPos = RENDERHEIGHT/2 - m_BoxSize.bottom/2 - TILESIZE;

    if (yoff > -1)
        m_yPos = (float) (yoff);

    m_FadeMode = FADEIN;
}

//
// Hinweisbox an x/y mit breite/höhe w/h setzen
//

void CGUISystem::ShowBox(int xoff, int yoff, int w, int h)
{
    m_BoxSize.right  = w;
    m_BoxSize.bottom = h;
    //DKS - Fixed off-center box display
    //    m_xPos = (float)xoff;
    //    m_yPos = (float)yoff;
    m_xPos = (float)xoff - TILESIZE;
    m_yPos = (float)yoff - TILESIZE;

    strcpy_s(m_BoxText, 1, "");

    m_FadeMode = FADEIN;
}

//
// Hinweisbox für Tutorial Level ausblenden
//

void CGUISystem::HideBox(void)
{
    m_FadeMode = FADEOUT;
}

//
// Hinweisbox für Tutorial Level gleich komplett abschalten
//

void CGUISystem::HideBoxFast(void)
{
    m_FadingAlpha = 0.0f;
    m_FadeMode    = INVISIBLE;
}


//
// GUI anzeigen und evtl faden
//

void CGUISystem::Run(void)
{
    // nicht faden? dann gleich abhauen
    //
    if (m_FadeMode == INVISIBLE)
        return;

    // Box rendern
    //

    RenderBox();

    // Einfaden
    //

    if (m_FadeMode == FADEIN)
    {
        if (m_FadingAlpha < MAXFADE)
            m_FadingAlpha += FADESPEED SYNC;
        else
        {
            m_FadingAlpha = MAXFADE;
            m_FadeMode    = VISIBLE;
        }
    }

    // Ausfaden
    //

    if (m_FadeMode == FADEOUT)
    {
        if (m_FadingAlpha > 0.0f)
            m_FadingAlpha -= FADESPEED SYNC;
        else
        {
            m_FadingAlpha = 0.0f;
            m_FadeMode    = INVISIBLE;
        }
    }
}
