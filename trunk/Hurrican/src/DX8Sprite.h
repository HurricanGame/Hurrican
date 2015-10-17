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
#include <map>
#include <string>

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

//DKS - DirectGraphicsSurface class was never used anywhere in the game, even in the
//      original DirectX release, so disabled it:
#if 0
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

    #if defined(PLATFORM_DIRECTX)
    bool  DrawSurface (LPDIRECT3DSURFACE8 &Temp,			// Bild auf Surface Temp anzeigen
                       int xPos, int yPos);					// an Position xPos/yPos
    #endif
};
#endif //0

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

    //DKS - Made these shorts instead of ints:
    int16_t				itsXFrameCount;						// Frameanzahl in X-Richtung
    int16_t				itsYFrameCount;						// Frameanzahl in Y-Richtung
    int16_t				itsXFrameSize;						// Framegrösse in X-Richtung
    int16_t				itsYFrameSize;						// Framegrösse in Y-Richtung

    //DKS - Textures are now accessed through an index passed to TexturesystemClass:
    int16_t             itsTexIdx;                          // Index to pass to SetTexture()

    //DKS - Added these.. Instead of each sprite needing to divide each of its four
    //      texture-coordinates by itsXSize or itsYSize each time it is drawn, it simply
    //      multiplies them by these factors, which is 1/total_tex_size in each dimension. It
    //      also corrects for any nearest-power-of-two resizing that was done when loading it,
    //      allowing the flexibility to load textures from disk that are resized. In
    //      other words, itsXTexScale = (1 / x-dimen-in-game) * (x-dimen-on-disk / x-dimen-after-npot-resize)
    float               itsXTexScale;                       // Scale factor for X-dimension
    float               itsYTexScale;                       // Scale factor for Y-dimension
     

    //DKS - This array is now dynamically allocated, instead of wasting
    //      lots of RAM needlessly. Also, when in debug-mode, it is a bounds-
    //      checked wrapper to a vector, as I found the game was accessing
    //      past its end in Projectiles.cpp and Partikelsystem.cpp
    //RECT				itsPreCalcedRects[256];				// vorberechnete Ausschnitte für die Frames
#ifdef _DEBUG
    class BoundCheckedArray {
    public:
        RECT& operator[](int i)
        {
            if (i < 0 || i >= (int)rects.size()) {
                Protokoll.WriteText( true, "Error: index %d out of bounds of itsPreCalcedRects[] (size:%d)\n", i, rects.size());
                abort();
            }

            return rects[i];
        }

        void PushBack(RECT &r)
        {
            rects.push_back(r);
        }

        void Clear()
        {
            rects.clear();
        }
    private:
        std::vector<RECT> rects;
    };
    BoundCheckedArray itsPreCalcedRects;
#else
    //DKS - When not in debug-mode, this is the pointer to the dynamically allocated array of RECTs
    RECT				*itsPreCalcedRects;				// vorberechnete Ausschnitte für die Frames
#endif

    DirectGraphicsSprite(void):
        itsXFrameCount(0), itsYFrameCount(0),
        itsXFrameSize(0),  itsYFrameSize(0),
        itsTexIdx(-1),
        itsXTexScale(1.0), itsYTexScale(1.0)
#ifndef _DEBUG
        //DKS - When not in debug-mode, this is the pointer to the dynamically allocated array of RECTs
        ,itsPreCalcedRects(NULL)
#endif
    {
        itsRect.top = itsRect.bottom = itsRect.left = itsRect.right = 0;
    }

    ~DirectGraphicsSprite(void);						// Textur freigeben

    bool  LoadImage(const std::string &filename,							// Laden des Bildes "Filename"
                    uint16_t xs, uint16_t ys, uint16_t xfs, uint16_t yfs,		// x/y Size und Framegrösse
                    uint16_t xfc,  uint16_t yfc);						// Frameanzahl

    //DKS - Made inline
    void SetRect	  (const int32_t left,  const int32_t top,
                       const int32_t right, const int32_t bottom)				// Neuen Ausschnitt setzen
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
        Anim %= 255;
        itsRect = itsPreCalcedRects [Anim];

        // Und Sprite rendern
        RenderSprite(x, y, Color);
    }

    //DKS - Inlined:
    void  RenderSprite(float x, float y, int Anim, D3DCOLOR Color, bool mirrored)
    {
        // Ausschnitt einstellen
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
static inline bool SpriteCollision(const float x1, const float y1, const RECT &rect1,
                                    const float x2, const float y2, const RECT &rect2)
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

//DKS - never actually used:
//extern DirectGraphicsSprite Preview;
#endif
