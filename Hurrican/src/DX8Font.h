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

#include <d3d8.h>
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
		int						mXTextureSize;					// x-Gr�sse der gesamten Textur
		int						mYTextureSize;					// y-Gr�sse der gesamten Textur		
		int						mXChars;						// Anzahl Zeichen in X-Richtung		

	public:
		char					mCharLength[256];				// L�nge der Zeichen in Pixel
		int						mXCharSize;						// X-Gr�sse eines Zeichens
		int						mYCharSize;						// Y-Gr�sse eines Zeichens

			  DirectGraphicsFont(void);							// Konstruktor (leer)
			 ~DirectGraphicsFont(void);							// Textur freigeben
		bool LoadFont(char *Filename,						// Laden des Bildes "Filename"
					  int xts,									// Textur-Gr�sse x
					  int yts,									// Textur-Gr�sse y
					  int xCharsize,							// x-Gr�sse eines Zeichens
					  int yCharsize,							// y-Gr�sse eines Zeichens
					  int xChars,								// Anzahl der Zeichen pro Zeile
					  int yChars);								// Anzahl der Zeichen pro Spalte
		bool DrawText(float x, float y,							// Text an xPos,yPos mit Farbe
					  char Text[], D3DCOLOR Color);				// Color ausgeben
		bool DrawValue(float x, float y,						// Zahlenwert an xPos,yPos mit Farbe
					  float Value, D3DCOLOR Color);				// Color ausgeben
		void DrawTextRightAlign(float x, float y,				// Text an xPos,yPos mit Farbe
								char Text[], D3DCOLOR Color,
								int Spacing = 1);				// Color ausgeben right aligned ausgeben
		void DrawTextCenterAlign(float x, float y,				// Text an xPos,yPos mit Farbe
								char Text[], D3DCOLOR Color,
								int Spacing = 1);				// Color ausgeben right aligned ausgeben
		bool DrawDemoChar(float x, float y,							// Text an xPos,yPos mit Farbe
					  char Text, D3DCOLOR Color);				// Color ausgeben
		bool DrawDemoText(float x, float y,							// Text an xPos,yPos mit Farbe
					  char Text[], D3DCOLOR Color);				// Color ausgeben
		bool DrawText(float x, float y,							// Text an xPos,yPos mit Farbe
					  char Text[], D3DCOLOR Color, int Spacing);// Color ausgeben
		int  DemoStringLength(char Text[]);						// L�nge eines Strings in Pixeln zur�ckliefern
		int  StringLength(char Text[]);							// L�nge eines Strings in Pixeln zur�ckliefern
		int  StringLength(char Text[], int Spacing);			// L�nge eines Strings in Pixeln zur�ckliefern
		void ShowFPS (void);									// FPS Werte anzeigen
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern DirectGraphicsFont *pMenuFont;
extern DirectGraphicsFont *pDefaultFont;
extern DirectGraphicsFont *pFont;

#endif