#include "Timer.h"
#include "GUISystem.h"

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
	ExitGUISystem();
}

//
// GUISystem initialisieren
//

void CGUISystem::InitGUISystem(void)
{
	m_Rahmen.LoadImage("guiback.bmp", 60, 60, 20, 20, 3, 3);

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
// GUISystem beenden
//

void CGUISystem::ExitGUISystem(void)
{
	m_Rahmen.~DirectGraphicsSprite();
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
	 //
	 pDefaultFont->DrawText(m_xPos + TILESIZE, 
							m_yPos + (TILESIZE * 2 + m_BoxSize.bottom - ((pDefaultFont->mYCharSize + 6) * (m_BoxLines + 1))) / 2 + 4, m_BoxText, color);
}

//
// Hinweisbox für Tutorial Level einblenden
//

void CGUISystem::ShowBox(char Text[BOXTEXTLENGTH], int yoff, int xoff)
{	

	int FontLength = 0;
	m_BoxLines     = 0;

	FontLength = pDefaultFont->StringLength(Text) / TILESIZE * TILESIZE;

	// Muss der Text in mehrere Zeilen aufgeteilt werden?
	if (FontLength > BOXSIZEMAX)
	{
		FontLength = BOXSIZEMAX;

		char buffer[1000];			// buffer, der jeweils die aktuelle Zeile beinhaltet
		int  offset       = 0;		// Offset im übergebenen Text
		int  offsetbox	  = 0;		// Offset im BoxText (der später angezeigt wird)		
		int  offsetbuffer = 0;		// Offset im Buffer

		strcpy_s (buffer, 1, "");
		strcpy_s (m_BoxText, 1, "");

		// Text Zeichen für Zeichen durchgehen
		while (Text[offset] != '\0')
		{
			// aktuelles Zeichen der Zeile kopieren
			buffer   [offsetbuffer]     = Text[offset];
			buffer   [offsetbuffer + 1]	= '\0';
			m_BoxText[offsetbox]        = Text[offset];
			m_BoxText[offsetbox + 1]    = '\0';
			

			// Länge für eine Zeile überschritten und gerade ein Leerzeichen?
			// Dann Zeilenumbruch einfügen
			if (pDefaultFont->StringLength(buffer) > BOXSIZEMAX - 60 &&
				buffer[offsetbuffer] == 32)
			{
				// Zeilenpuffer löschen
				strcpy_s(buffer, 1, "");
				offsetbuffer = 0;

				// Umbruch einfügen
				offsetbox++;
				m_BoxText[offsetbox] = '\n';				

				// Zeilenanzahl erhöhen
				m_BoxLines++;
			}			
			else
				// ein Zeichen im Buffer weiter
				offsetbuffer++;

			// ein Zeichen im Text und im BoxText weiter
			offset++;		
			offsetbox++;
		}				
	}	
	else
		strcpy_s (m_BoxText, strlen(Text) + 1, Text);

	m_BoxSize.right  = FontLength;
	m_BoxSize.bottom = (m_BoxLines * (pDefaultFont->mYCharSize + 6)) / 20 * 20 + 20;

	m_xPos = xoff - (m_BoxSize.right - 40) / 2.0f;
	m_yPos = (480 - m_BoxSize.bottom - 40) / 2.0f;

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
	m_xPos = (float)xoff;
	m_yPos = (float)yoff;

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