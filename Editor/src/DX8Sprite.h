// Datei : DX8Sprite.h

// -------------------------------------------------------------------------------------- 
//
// DX8 Sprite Klasse
// zum laden und anzeigen von dx8 Surfaces und D3DSprites
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __DX8Sprite_h__
#define __DX8Sprite_h__

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include <d3d8.h>
#include <d3dx8.h>
#include <d3dx8tex.h>

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define	MAX_SPRITES		32768									// Maximalzahl Sprites am Screen
#define MAX_SPRITEGFX	500										// Maximal ladbare Sprites

// --------------------------------------------------------------------------------------
// Strukturen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Globale Spritefunktionen ausserhalb der Klassen
// --------------------------------------------------------------------------------------

void SetClippingArea(int Left, int Top, int Right, int Bottom);	// Neue ClippingArea setzen
RECT GetClippingArea(void);										// Aktuelle Clipping Area abfragen

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Surface Klasse für das Laden und Anzeigen (copyrects) 
// von Bildern (ohne Colorkey und anderen Effekten)
// --------------------------------------------------------------------------------------

class DirectGraphicsSurface
{
	private:
		LPDIRECT3DSURFACE8	itsSurface;							// Surface mit Grafikdaten
		RECT				itsRect;							// zu zeigender Ausschnitt

	public:
			  DirectGraphicsSurface(void);						// Konstruktor (leer)
			 ~DirectGraphicsSurface(void);						// Surface freigeben
		bool LoadImage(char Filename[100],						// Laden des Bildes "Filename"
					   int xSize, int ySize);					// mit Grösse xSize, ySize
		bool  SetRect	  (int left,  int top, 
						   int right, int bottom);				// Neuen Ausschnitt setzen
		RECT  GetRect	  (void);								// Ausschnitt holen
		bool  DrawSurface (LPDIRECT3DSURFACE8 &Temp,			// Bild auf Surface Temp anzeigen
						   int xPos, int yPos);					// an Position xPos/yPos
};

// --------------------------------------------------------------------------------------
// Sprite Klasse für das Laden und Anzeigen
// von Sprites (MIT Colorkey und anderen Effekten, hoffentlich schnell :)
// mit Hilfe von Vertices und Texturen drübergeklebt
// --------------------------------------------------------------------------------------

class DirectGraphicsSprite
{
	private:
		RECT				itsRect;							// zu zeigender Ausschnitt
		
	public:
		LPDIRECT3DTEXTURE8	itsTexture;							// Textur mit Grafikdaten
		float				itsXSize;							// x-Grösse der Textur
		float				itsYSize;							// y-Grösse der Textur
		int					itsXFrameSize;						// Framegrösse in X-Richtung
		int					itsYFrameSize;						// Framegrösse in Y-Richtung
		int					itsXFrameCount;						// Frameanzahl in X-Richtung
		int					itsYFrameCount;						// Frameanzahl in Y-Richtung

			  DirectGraphicsSprite(void);						// Konstruktor (leer)
			 ~DirectGraphicsSprite(void);						// Textur freigeben
		bool  LoadImage(char Filename[100],						// Laden des Bildes "Filename"
					    int xs, int ys, int xfs, int yfs,		// x/y Size und Framegrösse
						int xfc,  int yfc);						// Frameanzahl
		bool  LoadFromGfx(char Filename[100],						// Laden des Bildes "Filename"
					    int xs, int ys, int xfs, int yfs,		// x/y Size und Framegrösse
						int xfc,  int yfc);						// Frameanzahl
		bool  SetRect	  (int left,  int top, 
						   int right, int bottom);				// Neuen Ausschnitt setzen
		RECT  GetRect	  (void);								// Ausschnitt holen
		bool  RenderSprite(float x, float y, D3DCOLOR Color);	// Sprite rendern
		bool  RenderSprite(float x, float y, int Anim,			// Sprite rendern mit Animation
						   D3DCOLOR Color);	
		bool  RenderSpriteScaled(float x, float y,				// Sprite in beliebiger Grösse
								 float width, float height);	
		bool  RenderSpriteRotated(float x, float y, int Winkel,// Sprite rotiert darstellen
								  D3DCOLOR Color);				
};

// --------------------------------------------------------------------------------------
// Prototypen von sonstigen Funktionen
// --------------------------------------------------------------------------------------

void  RenderRect(float x, float y, float width,			// Rechteck anzeigen 
				 float height, D3DCOLOR Color);

#endif