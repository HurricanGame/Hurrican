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

#include "DX8Graphics.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

//DKS - Fixed OpenGL edge overdraw - Original value was 0.6f, but under OpenGL, this
//      allows grid overdraw patterns to occur visibly in blended sprites like 
//      waterfalls and water. Presumably, hidden overdraw also is occuring elsewhere.
//      Using 0.5f instead of 0.6f fixes this.
#if defined(PLATFORM_DIRECTX)
#define TEXTURE_COORD_OFFSET  0.6f
#else // OpenGL:
#define TEXTURE_COORD_OFFSET  0.5f
#endif // PLATFORM_DIRECTX

#define	MAX_SPRITES		      32768    // Maximalzahl Sprites am Screen
#define MAX_SPRITEGFX	      500      // Maximal ladbare Sprites

// --------------------------------------------------------------------------------------
// Strukturen
// --------------------------------------------------------------------------------------

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
    //DKS - these are never used anywhere, disabled:
    //bool  SetRect	  (int left,  int top,
    //                   int right, int bottom);				// Neuen Ausschnitt setzen
    //RECT  GetRect	  (void);								// Ausschnitt holen

    //DKS - This was never used anywhere, disabled:
    //#if defined(PLATFORM_DIRECTX)
    //bool  DrawSurface (LPDIRECT3DSURFACE8 &Temp,			// Bild auf Surface Temp anzeigen
    //                   int xPos, int yPos);					// an Position xPos/yPos
    //#endif
};

// --------------------------------------------------------------------------------------
// Sprite Klasse für das Laden und Anzeigen
// von Sprites (MIT Colorkey und anderen Effekten, hoffentlich schnell :)
// mit Hilfe von Vertices und Texturen drübergeklebt
// --------------------------------------------------------------------------------------

class DirectGraphicsSprite
{
public:
    //DKS - Moved itsRect to the top, since it is used most-frequently:
    RECT				itsRect;							// zu zeigender Ausschnitt

private:
    //DKS - Made these shorts instead of ints:
    int16_t				itsXFrameCount;						// Frameanzahl in X-Richtung
    int16_t				itsYFrameCount;						// Frameanzahl in Y-Richtung

public:
    float				itsXSize;							// x-Grösse der Textur
    float				itsYSize;							// y-Grösse der Textur
    //DKS - Made these shorts instead of ints:
    int16_t				itsXFrameSize;						// Framegrösse in X-Richtung
    int16_t				itsYFrameSize;						// Framegrösse in Y-Richtung


    //DKS - these are now dynamically allocated:
    //RECT				itsPreCalcedRects[256];				// vorberechnete Ausschnitte für die Frames
    RECT				*itsPreCalcedRects;				// vorberechnete Ausschnitte für die Frames

    LPDIRECT3DTEXTURE8	itsTexture;							// Textur mit Grafikdaten

    //DKS - inlined
    DirectGraphicsSprite(void)
    {
#if defined(PLATFORM_DIRECTX)
        itsTexture = (LPDIRECT3DTEXTURE8)NULL;
#elif defined(PLATFORM_SDL)
        itsTexture = -1;
#endif
        itsPreCalcedRects = NULL;
    }

    ~DirectGraphicsSprite(void);						// Textur freigeben

    bool  LoadImage(const char *Filename,							// Laden des Bildes "Filename"
                    int xs, int ys, int xfs, int yfs,		// x/y Size und Framegrösse
                    int xfc,  int yfc);						// Frameanzahl

    //DKS - Made inline
    void SetRect	  (int left,  int top,
                       int right, int bottom)				// Neuen Ausschnitt setzen
    {
        itsRect.left   = left;										// Linker  Rand
        itsRect.right  = right;										// Rechter Rand
        itsRect.top    = top;										// Oberer  Rand
        itsRect.bottom = bottom;									// Unterer Rand
    }
    //DKS - this is never used anywhere, disabled:
    //RECT  GetRect	  (void);								// Ausschnitt holen

    //DKS - All of the following rendering functions no longer pointlessly return bool value of true:
    void  RenderSprite(float x, float y, D3DCOLOR Color);	// Sprite rendern

    void  RenderSprite(float x, float y, int Anim,
                       D3DCOLOR c1,
                       D3DCOLOR c2,
                       D3DCOLOR c3,
                       D3DCOLOR c4);		// Sprite rendern mit vier farbigen Eckpunkten

    void  RenderMirroredSprite(float x, float y,			// Sprite spiegelverkehrt rendern
                               D3DCOLOR Color);
    void  RenderMirroredSprite(float x, float y,			// Sprite Vertikal spiegelverkehrt rendern
                               D3DCOLOR Color, bool h, bool v);

    //DKS - Inlined:
    void RenderSprite(float x, float y, int Anim, D3DCOLOR Color)
    {
        // Ausschnitt einstellen
        //DKS - TODO: can this be changed to a '& 0xFF'?
        Anim %= 255;
        itsRect = itsPreCalcedRects [Anim];

        // Und Sprite rendern
        RenderSprite(x, y, Color);
    }

    //DKS - Inlined:
    void  RenderSprite(float x, float y, int Anim, D3DCOLOR Color, bool mirrored)
    {
        // Ausschnitt einstellen
        //DKS - can we change this to '& 0xFF'?
        Anim %= 255;
        itsRect = itsPreCalcedRects [Anim];

        // Und Sprite rendern
        if (mirrored == false)
            RenderSprite(x, y, Color);
        else
            RenderMirroredSprite(x, y, Color);
    }

    void  RenderSpriteScaled(float x, float y,				// Sprite in beliebiger Grösse
                             int width, int height,
                             D3DCOLOR col);

    void  RenderSpriteScaled(float x, float y,				// Sprite in beliebiger Grösse mit Animationsphase
                             int width, int height,
                             int Anim,
                             D3DCOLOR col);
    
    void  RenderSpriteRotated(float x, float y, float Winkel,// Sprite rotiert darstellen
                              D3DCOLOR Color);

    void  RenderSpriteRotated(float x, float y, float Winkel,// Sprite rotiert mit animphase gespiegelt darstellen
                              int Anim, D3DCOLOR Color, bool mirror);

    //DKS - Inlined:
    void RenderSpriteRotated(float x, float y, float Winkel,// Sprite rotiert mit animphase darstellen
            int Anim, D3DCOLOR Color)
    {
        // Ausschnitt einstellen
        Anim %= 255;
        itsRect = itsPreCalcedRects [Anim];

        // Und Sprite rendern
        RenderSpriteScaledRotated (x, y,
                float (itsRect.right  - itsRect.left),
                float (itsRect.bottom - itsRect.top), Winkel, Color);
    }

    void  RenderSpriteScaledRotated(float x, float y,		// beliebige Grösse und rotiert
                                    float width, float height,
                                    float Winkel, D3DCOLOR Color);

    void  RenderSpriteRotatedOffset(float x, float y, float Winkel,	// Sprite rotiert darstellen mit Verschiebung
                                    float offx, float offy, D3DCOLOR Color, bool mirrored = false);

    //DKS - inlined these:
    float GetXSize(void)        { return itsXSize; }
    float GetYSize(void)        { return itsYSize; }
    int   GetXFrameSize(void)   { return itsXFrameSize; }
    int   GetYFrameSize(void)   { return itsYFrameSize; }
    int   GetXFrameCount(void)  { return itsXFrameCount; }
    int   GetYFrameCount(void)  { return itsYFrameCount; }
};

// --------------------------------------------------------------------------------------
// Prototypen von sonstigen Funktionen
// --------------------------------------------------------------------------------------

void  RenderRect4(float x, float y, float width,									// Rechteck anzeigen mit 4 farbigen Ecken
                  float height,
                  D3DCOLOR c1,
                  D3DCOLOR c2,
                  D3DCOLOR c3,
                  D3DCOLOR c4);

//DKS - Inlined:
static inline void RenderRect(float x, float y, float width, float height, D3DCOLOR Color)
{
	RenderRect4(x, y , width, height, Color, Color, Color, Color);
}

void  RenderLine(D3DXVECTOR2 p1, D3DXVECTOR2 p2,								// Linie rendern
                 D3DCOLOR Color);
void RenderLine (D3DXVECTOR2 p1, D3DXVECTOR2 p2,								// Linie rendern (zwei farbig)
                 D3DCOLOR Color1, D3DCOLOR Color2);

//DKS - Disabled, as it is no longer used anywhere:
//void RenderCircle(float x, float y, float r, D3DCOLOR col);


//DKS - Inlined this, since it is simple and used extremely frequently and takes many parameters:
//bool  SpriteCollision(float x1, float y1, RECT rect1,							// Kollision zwischen zwei Sprites
//                      float x2, float y2, RECT rect2);
static inline bool  SpriteCollision(float x1, float y1, RECT &rect1,
                                    float x2, float y2, RECT &rect2)
{
    // Kollision der Bounding-Boxes ?
    if (x1 + rect1.left < x2 + rect2.right   &&
        x2 + rect2.left < x1 + rect1.right   &&
        y1 + rect1.top  < y2 + rect2.bottom  &&
        y2 + rect2.top  < y1 + rect1.bottom)
        return true;
    else
        return false;
}

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern DirectGraphicsSprite LoadingScreen;
extern DirectGraphicsSprite LoadingBar;
extern float				LoadingProgress;
extern float				LoadingItemsToLoad;
extern int					LoadingItemsLoaded;
extern DirectGraphicsSprite Preview;
extern DirectGraphicsSprite	LavaFlare;							// Leuchten des lava Balles



#endif
