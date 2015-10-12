// Datei : DX8Font.h

// --------------------------------------------------------------------------------------
//
// Font Klasse
// zum Anzeigen von Schriften
//
// (c) 2002 J�rg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __DX8Font_h__
#define __DX8Font_h__

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#if defined(PLATFORM_DIRECTX)
#include <d3d8.h>
#endif
#include "DX8Sprite.h"

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Font Klasse
// --------------------------------------------------------------------------------------

class DirectGraphicsFont
{
private:
    DirectGraphicsSprite	*mTexture;						// Textur mit Font-Grafikdaten
    int						mXChars;						// Anzahl Zeichen in X-Richtung

public:
    //DKS - Character widths are now hard-coded arrays we merely point to:
    //char					mCharLength[256];				// L�nge der Zeichen in Pixel
    uint8_t					*mCharLength;				// L�nge der Zeichen in Pixel
    int						mXCharSize;						// X-Gr�sse eines Zeichens
    int						mYCharSize;						// Y-Gr�sse eines Zeichens


    //DKS - Added support for font scaling
    int                   mScaleFactor;                 // Font will be scaled this much (default is 1, no scaling)

    DirectGraphicsFont(void);							// Konstruktor (leer)
    ~DirectGraphicsFont(void);							// Textur freigeben
    bool LoadFont(const char *Filename,						// Laden des Bildes "Filename"
                  int xts,									// Textur-Gr�sse x
                  int yts,									// Textur-Gr�sse y
                  int xCharsize,							// x-Gr�sse eines Zeichens
                  int yCharsize,							// y-Gr�sse eines Zeichens
                  int xChars,								// Anzahl der Zeichen pro Zeile
                  int yChars,								// Anzahl der Zeichen pro Spalte
                  uint8_t *width_values);                   // DKS - Array of character widths to use
                                                            // (Added this so that we can use compressed
                                                            //  font textures and not need to read PNGs
                                                            //  directly)
    bool DrawText(float x, float y,							// Text an xPos,yPos mit Farbe
                  const char Text[], D3DCOLOR Color);				// Color ausgeben
    bool DrawValue(float x, float y,						// Zahlenwert an xPos,yPos mit Farbe
                   float Value, D3DCOLOR Color);				// Color ausgeben
    void DrawTextRightAlign(float x, float y,				// Text an xPos,yPos mit Farbe
                            const char Text[], D3DCOLOR Color,
                            int Spacing = 1);				// Color ausgeben right aligned ausgeben
    void DrawTextCenterAlign(float x, float y,				// Text an xPos,yPos mit Farbe
                             const char Text[], D3DCOLOR Color,
                             int Spacing = 1);				// Color ausgeben right aligned ausgeben
    bool DrawDemoChar(float x, float y,							// Text an xPos,yPos mit Farbe
                      const char Text, D3DCOLOR Color);				// Color ausgeben
    bool DrawDemoText(float x, float y,							// Text an xPos,yPos mit Farbe
                      const char Text[], D3DCOLOR Color);				// Color ausgeben
    bool DrawText(float x, float y,							// Text an xPos,yPos mit Farbe
                  const char Text[], D3DCOLOR Color, int Spacing);// Color ausgeben
    int  DemoStringLength(const char Text[]);						// L�nge eines Strings in Pixeln zur�ckliefern
    int  StringLength(const char Text[]);							// L�nge eines Strings in Pixeln zur�ckliefern
    int  StringLength(const char Text[], int Spacing);			// L�nge eines Strings in Pixeln zur�ckliefern
    void ShowFPS (void);									// FPS Werte anzeigen

    //DKS - New functions added to facilitate scaled fonts, primarily the "default" font on 320x240 devices
    int  GetYCharSize(void);
    int  GetScaleFactor(void);
    void SetScaleFactor(int scale_factor);
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern DirectGraphicsFont *pMenuFont;
extern DirectGraphicsFont *pDefaultFont;
extern DirectGraphicsFont *pFont;
//DKS - Added these three hard-coded lists of character widths for the fonts:
extern uint8_t demofont_charwidths[256];
extern uint8_t menufont_charwidths[256];
extern uint8_t smallfont_charwidths[256];

#endif

