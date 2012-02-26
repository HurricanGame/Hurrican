// Datei : DX8Font.h

// -------------------------------------------------------------------------------------- 
//
// Font Klasse
// zum Anzeigen von Schriften
//
// (c) 2002 Jörg M. Winterstein
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
		int						mXTextureSize;					// x-Grösse der gesamten Textur
		int						mYTextureSize;					// y-Grösse der gesamten Textur		
		int						mXChars;						// Anzahl Zeichen in X-Richtung		

	public:
		char					mCharLength[256];				// Länge der Zeichen in Pixel
		int						mXCharSize;						// X-Grösse eines Zeichens
		int						mYCharSize;						// Y-Grösse eines Zeichens

			  DirectGraphicsFont(void);							// Konstruktor (leer)
			 ~DirectGraphicsFont(void);							// Textur freigeben
		bool LoadFont(char *Filename,						// Laden des Bildes "Filename"
					  int xts,									// Textur-Grösse x
					  int yts,									// Textur-Grösse y
					  int xCharsize,							// x-Grösse eines Zeichens
					  int yCharsize,							// y-Grösse eines Zeichens
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
		int  DemoStringLength(char Text[]);						// Länge eines Strings in Pixeln zurückliefern
		int  StringLength(char Text[]);							// Länge eines Strings in Pixeln zurückliefern
		int  StringLength(char Text[], int Spacing);			// Länge eines Strings in Pixeln zurückliefern
		void ShowFPS (void);									// FPS Werte anzeigen
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern DirectGraphicsFont *pMenuFont;
extern DirectGraphicsFont *pDefaultFont;
extern DirectGraphicsFont *pFont;

#endif