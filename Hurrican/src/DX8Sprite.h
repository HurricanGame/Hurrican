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

#include "dx8Graphics.h"

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
		bool LoadImage(const char *Filename,					// Laden des Bildes "Filename"
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
		int					itsXFrameCount;						// Frameanzahl in X-Richtung
		int					itsYFrameCount;						// Frameanzahl in Y-Richtung

	public:		
		RECT				itsRect;							// zu zeigender Ausschnitt		
		float				itsXSize;							// x-Grösse der Textur
		float				itsYSize;							// y-Grösse der Textur		
		int					itsXFrameSize;						// Framegrösse in X-Richtung
		int					itsYFrameSize;						// Framegrösse in Y-Richtung


		RECT				itsPreCalcedRects[256];				// vorberechnete Ausschnitte für die Frames
		LPDIRECT3DTEXTURE8	itsTexture;							// Textur mit Grafikdaten
			  DirectGraphicsSprite(void);						// Konstruktor (leer)
			 ~DirectGraphicsSprite(void);						// Textur freigeben

		bool  LoadImage(char *Filename,							// Laden des Bildes "Filename"
					    int xs, int ys, int xfs, int yfs,		// x/y Size und Framegrösse
						int xfc,  int yfc);						// Frameanzahl
		bool  SetRect	  (int left,  int top, 
						   int right, int bottom);				// Neuen Ausschnitt setzen
		RECT  GetRect	  (void);								// Ausschnitt holen
		bool  RenderTexture(float x,  float y,  float w,  float h, 
							float tl, float to, float tr, float tu, 
							D3DCOLOR Color);
		bool  RenderSprite(float x, float y, D3DCOLOR Color);	// Sprite rendern
		bool  RenderSprite(float x, float y, int Anim,
											 D3DCOLOR c1,
											 D3DCOLOR c2,
											 D3DCOLOR c3,
											 D3DCOLOR c4);		// Sprite rendern mit vier farbigen Eckpunkten
		bool  RenderMirroredSprite(float x, float y,			// Sprite spiegelverkehrt rendern 
								   D3DCOLOR Color);	
		bool  RenderMirroredSprite(float x, float y,			// Sprite Vertikal spiegelverkehrt rendern 
								    D3DCOLOR Color, bool h, bool v);	
		bool  RenderSprite(float x, float y, int Anim,			// Sprite rendern mit Animation
						   D3DCOLOR Color);	
		bool  RenderSprite(float x, float y, int Anim,			// Sprite rendern mit Animation, wahlweise
						   D3DCOLOR Color, bool mirrored);		// spiegelverkehrt
		bool  RenderSpriteScaled(float x, float y,				// Sprite in beliebiger Grösse
								 int width, int height,
								 D3DCOLOR col);	
		bool  RenderSpriteScaled(float x, float y,				// Sprite in beliebiger Grösse mit Animationsphase
								 int width, int height,
								 int Anim,
								 D3DCOLOR col);	
		bool  RenderSpriteRotated(float x, float y, float Winkel,// Sprite rotiert darstellen
								  D3DCOLOR Color);				
		bool  RenderSpriteRotated(float x, float y, float Winkel,// Sprite rotiert mit animphase darstellen
								  int Anim, D3DCOLOR Color);
		bool  RenderSpriteRotated(float x, float y, float Winkel,// Sprite rotiert mit animphase gespiegelt darstellen
								  int Anim, D3DCOLOR Color, bool mirror);
		bool  RenderSpriteScaledRotated(float x, float y,		// beliebige Grösse und rotiert
								 float width, float height,
								 float Winkel, D3DCOLOR Color);
		bool  RenderSpriteRotatedOffset(float x, float y, float Winkel,	// Sprite rotiert darstellen mit Verschiebung
										float offx, float offy, D3DCOLOR Color, bool mirrored = false);	

		float GetXSize(void);										// Membervariablen holen
		float GetYSize(void);
		int   GetXFrameSize(void);
		int   GetYFrameSize(void);
		int   GetXFrameCount(void);
		int   GetYFrameCount(void);
};

// --------------------------------------------------------------------------------------
// Prototypen von sonstigen Funktionen
// --------------------------------------------------------------------------------------

void  RenderRect(float x, float y, float width,									// Rechteck anzeigen 
				 float height, D3DCOLOR Color);

void  RenderRect4(float x, float y, float width,									// Rechteck anzeigen mit 4 farbigen Ecken
				 float height, 
				 D3DCOLOR c1, 
				 D3DCOLOR c2, 
				 D3DCOLOR c3, 
				 D3DCOLOR c4);

void  RenderLine(D3DXVECTOR2 p1, D3DXVECTOR2 p2,								// Linie rendern
				 D3DCOLOR Color);
void RenderLine (D3DXVECTOR2 p1, D3DXVECTOR2 p2,								// Linie rendern (zwei farbig)
				 D3DCOLOR Color1, D3DCOLOR Color2);

void RenderCircle(float x, float y, float r, D3DCOLOR col);


bool  SpriteCollision(float x1, float y1, RECT rect1,							// Kollision zwischen zwei Sprites
					  float x2, float y2, RECT rect2);

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern VERTEX2D				v1, v2, v3, v4;						// Vertices zum Schuss rendern
extern VERTEX2D				TriangleStrip[4];					// Strip für einen Schuss
extern DirectGraphicsSprite LoadingScreen;
extern DirectGraphicsSprite LoadingBar;
extern float				LoadingProgress;
extern float				LoadingItemsToLoad;
extern int					LoadingItemsLoaded;
extern DirectGraphicsSprite Preview;
extern DirectGraphicsSprite	LavaFlare;							// Leuchten des lava Balles



#endif