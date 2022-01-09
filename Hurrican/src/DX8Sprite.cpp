// Datei : DX8Sprite.cpp

// --------------------------------------------------------------------------------------
//
// DX8 Sprite Klasse
// zum laden und anzeigen von dx8 Surfaces und D3DSprites
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include <string>

#include "DX8Texture.hpp"
#include "SDLPort/SDL_port.hpp"
#include "SDLPort/texture.hpp"

#include "DX8Graphics.hpp"
#include "DX8Sprite.hpp"
#include "Gameplay.hpp"
#include "Globals.hpp"
#include "Logdatei.hpp"
#include "Main.hpp"

#ifdef USE_UNRARLIB
#include "unrarlib.h"
#endif

extern bool GameRunning;  // Läuft das Spiel noch ?

// --------------------------------------------------------------------------------------
// Klassenfunktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// DirectGraphicsSprite Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Desktruktor gibt die Textur wieder frei
// --------------------------------------------------------------------------------------

DirectGraphicsSprite::~DirectGraphicsSprite() {
// DKS - itsPreCalcedRects array is now dynamically allocated:
#ifdef NDEBUG
    // DKS - When in debug mode, itsPreCalcedRects is bound-checked vector wrapper,
    //      otherwise it is a dynamically-allocated array
    delete[] itsPreCalcedRects;
    itsPreCalcedRects = nullptr;
#endif

    Textures.UnloadTexture(itsTexIdx);

    //    if(itsTexture != 0)
    //    {
    //        delete_texture( itsTexture );
    //        itsTexture = -1;
    //        itsTexture = (LPDIRECT3DTEXTURE8)nullptr;
    //        LoadedTextures--;
    ////		Protokoll << "-> Sprite texture successfully released !" << std::endl;
    //
    ////        _itoa_s(LoadedTextures, Buffer, 10);
    ////		Protokoll << Buffer;
    ////		Protokoll << " Sprite Textur(en) übrig !" << std::endl;
    //    }
}

// --------------------------------------------------------------------------------------
// Laden des Bildes "Filename"
// xfs, yfs Grösse eines Frames
// xfc, yfc Anzahl der Frames
// --------------------------------------------------------------------------------------

// DKS - Textures have been overhauled and handled through new class, TexturesystemClass
//      I have left the original version of this function commented-out here for reference.
#if 0
bool DirectGraphicsSprite::LoadImage(const char *Filename, int xs, int ys, int xfs, int yfs,
                                     int xfc,  int yfc)
{
    if(GameRunning == false)
        return false;

    bool			fromrar;
    HRESULT			hresult;
    char			*pData;
    char			Temp[256];
#if defined(USE_PVRTC) || defined(USE_ETC1)
    char			compresstex[256];
#endif
    unsigned long	Size;
#if defined(PLATFORM_SDL)
    ()hresult;

    SDL_Rect        dims;
#endif

    // zuerst eine evtl benutzte Textur freigeben
    delete_texture( itsTexture );
    itsTexture = 0;

    fromrar = false;

    // Zuerst checken, ob sich die Grafik in einem MOD-Ordner befindet
    if (CommandLineParams.RunOwnLevelList == true)
    {
        sprintf_s(Temp, "%s/levels/%s/%s", g_storage_ext, CommandLineParams.OwnLevelList, Filename);

#if defined(USE_ETC1)
        if (DirectGraphics.SupportedETC1 == true) {
            sprintf_s( compresstex, "%s/levels/%s/%s.pkm", g_storage_ext, CommandLineParams.OwnLevelList, Filename );
            if (FileExists(compresstex))
                goto loadfile;
        }
#endif

#if defined(USE_PVRTC)
        if (DirectGraphics.SupportedPVRTC == true) {
            sprintf_s( compresstex, "%s/levels/%s/%s.pvr", g_storage_ext,  CommandLineParams.OwnLevelList, Filename );
            if (FileExists(compresstex))
                goto loadfile;
        }
#endif

        if (FileExists(Temp))
            goto loadfile;
    }

    // Dann checken, ob sich das File im Standard Ordner befindet
    //DKS - All textures are now stored in their own data/textures/ subdir:
    sprintf_s(Temp, "%s/data/textures/%s", g_storage_ext, Filename);

#if defined(USE_ETC1)
    if (DirectGraphics.SupportedETC1 == true) {
        sprintf_s( compresstex, "%s/data/textures/etc1/%s.pkm", g_storage_ext, Filename );
        if (FileExists(compresstex))
        {
            sprintf_s( Temp, "%s/data/textures/etc1/%s", g_storage_ext, Filename );
            goto loadfile;
        }
    }
#endif

#if defined(USE_PVRTC)
    if (DirectGraphics.SupportedPVRTC == true) {
        sprintf_s( compresstex, "%s/data/textures/pvr/%s.pvr", g_storage_ext, Filename );
        if (FileExists(compresstex))
        {
            sprintf_s( Temp, "%s/data/textures/pvr/%s", g_storage_ext, Filename );
            goto loadfile;
        }
    }
#endif

    if (FileExists(Temp))
        goto loadfile;

#if defined(USE_UNRARLIB)
    // Auch nicht? Dann ist es hoffentlich im RAR file

    if (urarlib_get(&pData, &Size, Filename, RARFILENAME, convertText(RARFILEPASSWORD)) == false)
    {
        Protokoll << "\n-> Error loading " << Filename << " from Archive !" << std::endl;
        GameRunning = false;
        return false;
    }
    else
        fromrar = true;
#else
    Protokoll << "\n-> Error loading " << Temp << "!" << std::endl;
    GameRunning = false;
    return false;
#endif  // USE_UNRARLIB


loadfile:

    // normal von Platte laden?
    if (fromrar == false)
    {
        itsTexture = LoadTexture( Temp, dims, 0 );
    }
    else
    {
        itsTexture = LoadTexture( pData, dims, Size );
        free(pData);
    }

    // Grösse setzen
    itsXSize		= (float)dims.w;
    itsYSize		= (float)dims.h;
    itsXFrameCount	= xfc;
    itsYFrameCount	= yfc;
    itsXFrameSize	= xfs;
    itsYFrameSize	= yfs;
    itsRect.left	= 0;
    itsRect.top		= 0;
    itsRect.right	= static_cast<int>(i)tsXSize;
    itsRect.bottom	= static_cast<int>(i)tsYSize;

    // Ausschnitte vorberechnen
    //DKS - array is now dynamically allocated
    if (itsPreCalcedRects != nullptr) 
        free(itsPreCalcedRects);
    itsPreCalcedRects = (RECT_struct*)malloc(xfc * yfc * sizeof(RECT_struct));

    for (int i = 0; i < xfc * yfc; i++)
    {
        itsPreCalcedRects[i].top	= (i/itsXFrameCount) * itsYFrameSize;
        itsPreCalcedRects[i].left	= (i%itsXFrameCount) * itsXFrameSize;
        itsPreCalcedRects[i].right  = itsPreCalcedRects[i].left + itsXFrameSize;
        itsPreCalcedRects[i].bottom = itsPreCalcedRects[i].top  + itsYFrameSize;
    }

    itsRect = itsPreCalcedRects[0];

    // Bild korrekt geladen
    sprintf_s(Temp, "%s %s %s", TextArray [TEXT_LADE_BITMAP], Filename, TextArray [TEXT_LADEN_ERFOLGREICH]);
    Protokoll << Temp << std::endl;

    DisplayLoadInfo(Temp);

    return true;
}
#endif  // 0 // END REFERENCE COPY OF ORIGINAL LoadImage()
bool DirectGraphicsSprite::LoadImage(const std::string &filename,
                                     uint16_t xs,
                                     uint16_t ys,
                                     uint16_t xfs,
                                     uint16_t yfs,
                                     uint16_t xfc,
                                     uint16_t yfc) {
    if (!GameRunning || filename.empty())
        return false;

    if (xfc == 0 || yfc == 0) {
#ifndef NDEBUG
        Protokoll << "Error: xfc or yfc parameters to DirectGraphicsSprite::LoadImage() are 0! xfc:" << xfc
                  << " yfc:" << yfc << std::endl;
        GameRunning = false;
#endif
    }

    if (itsTexIdx != -1) {
        Textures.UnloadTexture(itsTexIdx);
        itsTexIdx = -1;
    }

    itsTexIdx = Textures.LoadTexture(filename);

    if (itsTexIdx == -1) {
        Protokoll << "Textures.LoadTexture() returned error loading file " << filename << std::endl;
        GameRunning = false;
        return false;
    }

    // DKS - Added scale factors that are combined with correction factors from any
    //      nearest-power-of-two expansion to make one single factor that allows
    //      flexible texture-coordinate computation with no divisions.
    //      This also means we no longer need member vars itsXSize and itsYSize.
    itsXTexScale = static_cast<float>(Textures[itsTexIdx].npot_scalex / static_cast<double>(xs));
    itsYTexScale = static_cast<float>(Textures[itsTexIdx].npot_scaley / static_cast<double>(ys));

    itsXFrameCount = xfc;
    itsYFrameCount = yfc;
    itsXFrameSize = xfs;
    itsYFrameSize = yfs;

    // Ausschnitte vorberechnen
    // DKS - itsPreCalcedRects array is now dynamically allocated
#ifndef NDEBUG
    // DKS - When in debug mode, itsPreCalcedRects is bound-checked vector wrapper
    itsPreCalcedRects.Clear();
#else
    if (itsPreCalcedRects != nullptr)
        delete[] itsPreCalcedRects;
    itsPreCalcedRects = new RECT_struct[xfc * yfc];
#endif

    for (int i = 0; i < xfc * yfc; i++) {
        RECT_struct r;
        r.top = (i / itsXFrameCount) * itsYFrameSize;
        r.left = (i % itsXFrameCount) * itsXFrameSize;
        r.right = r.left + itsXFrameSize;
        r.bottom = r.top + itsYFrameSize;
#ifndef NDEBUG
        // DKS - When in debug mode, itsPreCalcedRects is bound-checked vector wrapper
        itsPreCalcedRects.PushBack(r);
#else
        itsPreCalcedRects[i] = r;
#endif
    }

    itsRect = itsPreCalcedRects[0];

    // char Temp[255];
    // sprintf_s(Temp, "%s %s %s", TextArray [TEXT_LADE_BITMAP], filename.c_str(), TextArray [TEXT_LADEN_ERFOLGREICH]);
    // Protokoll << Temp << std::endl;
    // DisplayLoadInfo(Temp);

    std::string msg(filename);
    ReplaceAll(msg, ".png", "");
    msg = "Loaded texture " + msg;
    Protokoll << msg << std::endl;
    DisplayLoadInfo("");

    return true;
}

// --------------------------------------------------------------------------------------
// Sprite ganz normal zeichnen mit aktuellem Surfaceausschnitt und Colorkey
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderSprite(float x, float y, D3DCOLOR Color) {
    float l, r, o, u;      // Vertice Koordinaten
    float tl, tr, to, tu;  // Textur Koordinaten

    l = x;                                   // Links
    r = x + (itsRect.right - itsRect.left);  // Rechts
    o = y;                                   // Oben
    u = y + (itsRect.bottom - itsRect.top);  // Unten

    tl = itsRect.left * itsXTexScale;    // Links
    tr = itsRect.right * itsXTexScale;   // Rechts
    to = itsRect.top * itsYTexScale;     // Oben
    tu = itsRect.bottom * itsYTexScale;  // Unten

    VERTEX2D TriangleStrip[4];  // DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;

    TriangleStrip[0].x = l;  // Links oben
    TriangleStrip[0].y = o;
    TriangleStrip[0].tu = tl;
    TriangleStrip[0].tv = to;

    TriangleStrip[1].x = r;  // Rechts oben
    TriangleStrip[1].y = o;
    TriangleStrip[1].tu = tr;
    TriangleStrip[1].tv = to;

    TriangleStrip[2].x = l;  // Links unten
    TriangleStrip[2].y = u;
    TriangleStrip[2].tu = tl;
    TriangleStrip[2].tv = tu;

    TriangleStrip[3].x = r;  // Rechts unten
    TriangleStrip[3].y = u;
    TriangleStrip[3].tu = tr;
    TriangleStrip[3].tv = tu;

    DirectGraphics.SetTexture(itsTexIdx);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip[0]);
}

// --------------------------------------------------------------------------------------
// Sprite ganz normal zeichnen mit aktuellem Surfaceausschnitt und Colorkey, dabei
// alle vier Eckpunkte anders einfärben
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderSprite(float x,
                                        float y,
                                        int Anim,
                                        D3DCOLOR c1,
                                        D3DCOLOR c2,
                                        D3DCOLOR c3,
                                        D3DCOLOR c4) {
    float l, r, o, u;      // Vertice Koordinaten
    float tl, tr, to, tu;  // Textur Koordinaten

    // Ausschnitt einstellen
    Anim %= 255;

    if (Anim > -1)
        itsRect = itsPreCalcedRects[Anim];

    l = x;                                   // Links
    r = x + (itsRect.right - itsRect.left);  // Rechts
    o = y;                                   // Oben
    u = y + (itsRect.bottom - itsRect.top);  // Unten

    tl = itsRect.left * itsXTexScale;    // Links
    tr = itsRect.right * itsXTexScale;   // Rechts
    to = itsRect.top * itsYTexScale;     // Oben
    tu = itsRect.bottom * itsYTexScale;  // Unten

    VERTEX2D TriangleStrip[4];  // DKS - Added local declaration

    TriangleStrip[0].x = l;  // Links oben
    TriangleStrip[0].y = o;
    TriangleStrip[0].color = c1;
    TriangleStrip[0].tu = tl;
    TriangleStrip[0].tv = to;

    TriangleStrip[1].x = r;  // Rechts oben
    TriangleStrip[1].y = o;
    TriangleStrip[1].color = c2;
    TriangleStrip[1].tu = tr;
    TriangleStrip[1].tv = to;

    TriangleStrip[2].x = l;  // Links unten
    TriangleStrip[2].y = u;
    TriangleStrip[2].color = c3;
    TriangleStrip[2].tu = tl;
    TriangleStrip[2].tv = tu;

    TriangleStrip[3].x = r;  // Rechts unten
    TriangleStrip[3].y = u;
    TriangleStrip[3].color = c4;
    TriangleStrip[3].tu = tr;
    TriangleStrip[3].tv = tu;

    DirectGraphics.SetTexture(itsTexIdx);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip[0]);
}

// --------------------------------------------------------------------------------------
// Sprite vertikal spiegelverkehrt zeichnen
// --------------------------------------------------------------------------------------
void DirectGraphicsSprite::RenderMirroredSprite(float x, float y, D3DCOLOR Color, bool h, bool v) {
    float l, r, o, u;      // Vertice Koordinaten
    float tl, tr, to, tu;  // Textur Koordinaten

    if (!v) {
        o = y;                                   // Unten
        u = y + (itsRect.bottom - itsRect.top);  // Oben
    } else {
        u = y;                                   // Unten
        o = y + (itsRect.bottom - itsRect.top);  // Oben
    }

    if (!h) {
        r = x + (itsRect.right - itsRect.left);
        l = x;
    } else {
        l = x + (itsRect.right - itsRect.left);
        r = x;
    }

    tl = itsRect.left * itsXTexScale;    // Links
    tr = itsRect.right * itsXTexScale;   // Rechts
    to = itsRect.top * itsYTexScale;     // Oben
    tu = itsRect.bottom * itsYTexScale;  // Unten

    // tl = 0; tr = 0.5f; to = 0; tu = 0.5f;

    VERTEX2D TriangleStrip[4];  // DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;

    TriangleStrip[0].x = l;  // Links oben
    TriangleStrip[0].y = o;
    TriangleStrip[0].tu = tl;
    TriangleStrip[0].tv = to;

    TriangleStrip[1].x = r;  // Rechts oben
    TriangleStrip[1].y = o;
    TriangleStrip[1].tu = tr;
    TriangleStrip[1].tv = to;

    TriangleStrip[2].x = l;  // Links unten
    TriangleStrip[2].y = u;
    TriangleStrip[2].tu = tl;
    TriangleStrip[2].tv = tu;

    TriangleStrip[3].x = r;  // Rechts unten
    TriangleStrip[3].y = u;
    TriangleStrip[3].tu = tr;
    TriangleStrip[3].tv = tu;

    DirectGraphics.SetTexture(itsTexIdx);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip[0]);
}

// --------------------------------------------------------------------------------------
// Sprite spiegelverkehrt zeichnen mit aktuellem Surfaceausschnitt und Colorkey
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderMirroredSprite(float x, float y, D3DCOLOR Color) {
    float l, r, o, u;      // Vertice Koordinaten
    float tl, tr, to, tu;  // Textur Koordinaten

    r = x;                                   // Links
    l = x + (itsRect.right - itsRect.left);  // Rechts
    o = y;                                   // Oben
    u = y + (itsRect.bottom - itsRect.top);  // Unten

    tl = itsRect.left * itsXTexScale;    // Links
    tr = itsRect.right * itsXTexScale;   // Rechts
    to = itsRect.top * itsYTexScale;     // Oben
    tu = itsRect.bottom * itsYTexScale;  // Unten

    VERTEX2D TriangleStrip[4];  // DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;

    TriangleStrip[0].x = l;  // Links oben
    TriangleStrip[0].y = o;
    TriangleStrip[0].tu = tl;
    TriangleStrip[0].tv = to;

    TriangleStrip[1].x = r;  // Rechts oben
    TriangleStrip[1].y = o;
    TriangleStrip[1].tu = tr;
    TriangleStrip[1].tv = to;

    TriangleStrip[2].x = l;  // Links unten
    TriangleStrip[2].y = u;
    TriangleStrip[2].tu = tl;
    TriangleStrip[2].tv = tu;

    TriangleStrip[3].x = r;  // Rechts unten
    TriangleStrip[3].y = u;
    TriangleStrip[3].tu = tr;
    TriangleStrip[3].tv = tu;

    DirectGraphics.SetTexture(itsTexIdx);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip[0]);
}

// --------------------------------------------------------------------------------------
// Sprite skaliert zeichnen mit aktuellem Surfaceausschnitt (mit textur Filter)
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderSpriteScaled(float x, float y, int width, int height, D3DCOLOR col) {
    float l, r, o, u;      // Vertice Koordinaten
    float tl, tr, to, tu;  // Textur Koordinaten

    l = x;           // Links
    r = x + width;   // Rechts
    o = y;           // Oben
    u = y + height;  // Unten

    tl = itsRect.left * itsXTexScale;    // Links
    tr = itsRect.right * itsXTexScale;   // Rechts
    to = itsRect.top * itsYTexScale;     // Oben
    tu = itsRect.bottom * itsYTexScale;  // Unten

    VERTEX2D TriangleStrip[4];  // DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = col;

    TriangleStrip[0].x = l;  // Links oben
    TriangleStrip[0].y = o;
    TriangleStrip[0].tu = tl;
    TriangleStrip[0].tv = to;

    TriangleStrip[1].x = r;  // Rechts oben
    TriangleStrip[1].y = o;
    TriangleStrip[1].tu = tr;
    TriangleStrip[1].tv = to;

    TriangleStrip[2].x = l;  // Links unten
    TriangleStrip[2].y = u;
    TriangleStrip[2].tu = tl;
    TriangleStrip[2].tv = tu;

    TriangleStrip[3].x = r;  // Rechts unten
    TriangleStrip[3].y = u;
    TriangleStrip[3].tu = tr;
    TriangleStrip[3].tv = tu;

    DirectGraphics.SetTexture(itsTexIdx);

    DirectGraphics.SetFilterMode(true);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip[0]);

    DirectGraphics.SetFilterMode(false);
}

// --------------------------------------------------------------------------------------
// Sprite skaliert zeichnen mit angegebener Animationsphase (mit textur Filter)
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderSpriteScaled(float x, float y, int width, int height, int Anim, D3DCOLOR col) {
    float l, r, o, u;      // Vertice Koordinaten
    float tl, tr, to, tu;  // Textur Koordinaten

    l = x;           // Links
    r = x + width;   // Rechts
    o = y;           // Oben
    u = y + height;  // Unten

    tl = ((Anim % itsXFrameCount) * itsXFrameSize) * itsXTexScale;                  // Links
    tr = ((Anim % itsXFrameCount) * itsXFrameSize + itsXFrameSize) * itsXTexScale;  // Rechts
    to = ((Anim / itsXFrameCount) * itsYFrameSize) * itsYTexScale;                  // Oben
    tu = ((Anim / itsXFrameCount) * itsYFrameSize + itsYFrameSize) * itsYTexScale;  // Unten

    VERTEX2D TriangleStrip[4];  // DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = col;

    TriangleStrip[0].x = l;  // Links oben
    TriangleStrip[0].y = o;
    TriangleStrip[0].tu = tl;
    TriangleStrip[0].tv = to;

    TriangleStrip[1].x = r;  // Rechts oben
    TriangleStrip[1].y = o;
    TriangleStrip[1].tu = tr;
    TriangleStrip[1].tv = to;

    TriangleStrip[2].x = l;  // Links unten
    TriangleStrip[2].y = u;
    TriangleStrip[2].tu = tl;
    TriangleStrip[2].tv = tu;

    TriangleStrip[3].x = r;  // Rechts unten
    TriangleStrip[3].y = u;
    TriangleStrip[3].tu = tr;
    TriangleStrip[3].tv = tu;

    DirectGraphics.SetTexture(itsTexIdx);

    DirectGraphics.SetFilterMode(true);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip[0]);

    DirectGraphics.SetFilterMode(false);
}

// --------------------------------------------------------------------------------------
// Sprite mit übergebenem Winkel rotiert darstellen
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderSpriteRotated(float x, float y, float Winkel, D3DCOLOR Color) {
    float l, r, o, u;      // Vertice Koordinaten
    float tl, tr, to, tu;  // Textur Koordinaten

    l = x;                                   // Links
    r = x + (itsRect.right - itsRect.left);  // Rechts
    o = y;                                   // Oben
    u = y + (itsRect.bottom - itsRect.top);  // Unten

    tl = itsRect.left * itsXTexScale;    // Links
    tr = itsRect.right * itsXTexScale;   // Rechts
    to = itsRect.top * itsYTexScale;     // Oben
    tu = itsRect.bottom * itsYTexScale;  // Unten

    VERTEX2D TriangleStrip[4];  // DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;

    TriangleStrip[0].x = l;  // Links oben
    TriangleStrip[0].y = o;
    TriangleStrip[0].tu = tl;
    TriangleStrip[0].tv = to;

    TriangleStrip[1].x = r;  // Rechts oben
    TriangleStrip[1].y = o;
    TriangleStrip[1].tu = tr;
    TriangleStrip[1].tv = to;

    TriangleStrip[2].x = l;  // Links unten
    TriangleStrip[2].y = u;
    TriangleStrip[2].tu = tl;
    TriangleStrip[2].tv = tu;

    TriangleStrip[3].x = r;  // Rechts unten
    TriangleStrip[3].y = u;
    TriangleStrip[3].tu = tr;
    TriangleStrip[3].tv = tu;

    DirectGraphics.SetTexture(itsTexIdx);

    //----- Sprite rotieren

    glm::mat4x4 matRot, matTrans, matTrans2;

    // Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
    clampAngle(Winkel);

    // Rotationsmatrix
    matRot = glm::rotate(glm::mat4x4(1.0f), PI * Winkel / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    // Transformation zum Ursprung
    D3DXMatrixTranslation(&matTrans, -x - (itsRect.right - itsRect.left) / 2.0f,
                          -y - (itsRect.bottom - itsRect.top) / 2.0f, 0.0f);

    // Transformation wieder zurück
    D3DXMatrixTranslation(&matTrans2, x + (itsRect.right - itsRect.left) / 2.0f,
                          y + (itsRect.bottom - itsRect.top) / 2.0f, 0.0f);

    matWorld = matTrans * matWorld;   // Verschieben
    matWorld = matRot * matWorld;     // rotieren
    matWorld = matTrans2 * matWorld;  // und wieder zurück verschieben
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

    DirectGraphics.SetFilterMode(true);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip[0]);

    DirectGraphics.SetFilterMode(false);

    // Normale Projektions-Matrix wieder herstellen
    matWorld = glm::mat4x4(1.0f);
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif
}

// --------------------------------------------------------------------------------------
// Sprite mit übergebenem Winkel rotiert darstellen
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderSpriteRotated(float x, float y, float Winkel, int Anim, D3DCOLOR Color, bool mirror) {
    float l, r, o, u;      // Vertice Koordinaten
    float tl, tr, to, tu;  // Textur Koordinaten

    // Ausschnitt einstellen
    Anim %= 255;
    itsRect = itsPreCalcedRects[Anim];

    // DKS - Pulled duplicated math out of logic below:
    float height = itsRect.bottom - itsRect.top;
    float width = itsRect.right - itsRect.left;

    // normal
    //
    if (!mirror) {
        l = x;          // Links
                        // r = x+(itsRect.right-itsRect.left-1)+0.5f;	// Rechts   //DKS
        r = x + width;  // Rechts

        Winkel = 360.0f - Winkel;
    }

    // oder gespiegelt
    //
    else {
        r = x;          // Links
                        // l = x+(itsRect.right-itsRect.left-1)-0.5f;	// Rechts   //DKS
        l = x + width;  // Rechts
    }

    o = y;           // Oben
                     // u = y+(itsRect.bottom-itsRect.top-1)+0.5f;	// Unten    //DKS
    u = y + height;  // Unten

    tl = itsRect.left * itsXTexScale;    // Links
    tr = itsRect.right * itsXTexScale;   // Rechts
    to = itsRect.top * itsYTexScale;     // Oben
    tu = itsRect.bottom * itsYTexScale;  // Unten

    VERTEX2D TriangleStrip[4];  // DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;

    TriangleStrip[0].x = l;  // Links oben
    TriangleStrip[0].y = o;
    TriangleStrip[0].tu = tl;
    TriangleStrip[0].tv = to;

    TriangleStrip[1].x = r;  // Rechts oben
    TriangleStrip[1].y = o;
    TriangleStrip[1].tu = tr;
    TriangleStrip[1].tv = to;

    TriangleStrip[2].x = l;  // Links unten
    TriangleStrip[2].y = u;
    TriangleStrip[2].tu = tl;
    TriangleStrip[2].tv = tu;

    TriangleStrip[3].x = r;  // Rechts unten
    TriangleStrip[3].y = u;
    TriangleStrip[3].tu = tr;
    TriangleStrip[3].tv = tu;

    DirectGraphics.SetTexture(itsTexIdx);

    //----- Sprite rotieren

    glm::mat4x4 matRot, matTrans, matTrans2;

    // Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
    clampAngle(Winkel);

    // Rotationsmatrix
    matRot = glm::rotate(glm::mat4x4(1.0f), PI * Winkel / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    // Transformation zum Ursprung
    D3DXMatrixTranslation(&matTrans, -x - (itsRect.right - itsRect.left) / 2.0f,
                          -y - (itsRect.bottom - itsRect.top) / 2.0f, 0.0f);

    // Transformation wieder zurück
    D3DXMatrixTranslation(&matTrans2, x + (itsRect.right - itsRect.left) / 2.0f,
                          y + (itsRect.bottom - itsRect.top) / 2.0f, 0.0f);

    matWorld = matTrans * matWorld;   // Verschieben
    matWorld = matRot * matWorld;     // rotieren
    matWorld = matTrans2 * matWorld;  // und wieder zurück verschieben
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

    DirectGraphics.SetFilterMode(true);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip[0]);

    DirectGraphics.SetFilterMode(false);

    // Normale Projektions-Matrix wieder herstellen
    matWorld = glm::mat4x4(1.0f);
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif
}

// --------------------------------------------------------------------------------------
// Sprite rotiert darstellen mit Verschiebung
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderSpriteRotatedOffset(float x,
                                                     float y,
                                                     float Winkel,
                                                     float offx,
                                                     float offy,
                                                     D3DCOLOR Color,
                                                     bool mirrored) {
    float l, r, o, u;      // Vertice Koordinaten
    float tl, tr, to, tu;  // Textur Koordinaten

    // DKS - Pulled duplicated math out of logic below:
    float height = itsRect.bottom - itsRect.top;
    float width = itsRect.right - itsRect.left;

    l = x;          // Links
                    // r = x+(itsRect.right-itsRect.left-1)+0.5f;	// Rechts       //DKS
    r = x + width;  // Rechts

    if (mirrored) {
        std::swap(l, r);

        offx = -offx;
    } else
        Winkel = 360.0f - Winkel;

    o = y;           // Oben
                     // u = y+(itsRect.bottom-itsRect.top-1)+0.5f;	// Unten        //DKS
    u = y + height;  // Unten

    tl = itsRect.left * itsXTexScale;    // Links
    tr = itsRect.right * itsXTexScale;   // Rechts
    to = itsRect.top * itsYTexScale;     // Oben
    tu = itsRect.bottom * itsYTexScale;  // Unten

    VERTEX2D TriangleStrip[4];  // DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;

    TriangleStrip[0].x = l;  // Links oben
    TriangleStrip[0].y = o;
    TriangleStrip[0].tu = tl;
    TriangleStrip[0].tv = to;

    TriangleStrip[1].x = r;  // Rechts oben
    TriangleStrip[1].y = o;
    TriangleStrip[1].tu = tr;
    TriangleStrip[1].tv = to;

    TriangleStrip[2].x = l;  // Links unten
    TriangleStrip[2].y = u;
    TriangleStrip[2].tu = tl;
    TriangleStrip[2].tv = tu;

    TriangleStrip[3].x = r;  // Rechts unten
    TriangleStrip[3].y = u;
    TriangleStrip[3].tu = tr;
    TriangleStrip[3].tv = tu;

    DirectGraphics.SetTexture(itsTexIdx);

    //----- Sprite rotieren

    glm::mat4x4 matRot, matTrans, matTrans2;

    // Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
    clampAngle(Winkel);

    // Rotationsmatrix
    matRot = glm::rotate(glm::mat4x4(1.0f), PI * Winkel / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    // Transformation zum Ursprung
    D3DXMatrixTranslation(&matTrans, -x - (itsRect.right - itsRect.left) / 2.0f - offx,
                          -y - (itsRect.bottom - itsRect.top) / 2.0f - offy, 0.0f);

    // Transformation wieder zurück
    D3DXMatrixTranslation(&matTrans2, x + (itsRect.right - itsRect.left) / 2.0f + offx,
                          y + (itsRect.bottom - itsRect.top) / 2.0f + offy, 0.0f);

    matWorld = matTrans * matWorld;   // Verschieben
    matWorld = matRot * matWorld;     // rotieren
    matWorld = matTrans2 * matWorld;  // und wieder zurück verschieben
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

    DirectGraphics.SetFilterMode(true);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip[0]);

    DirectGraphics.SetFilterMode(false);

    // Normale Projektions-Matrix wieder herstellen
    matWorld = glm::mat4x4(1.0f);
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif
}

// --------------------------------------------------------------------------------------
// Sprite mit übergebenem Winkel rotiert darstellen in beliebiger Grösse
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderSpriteScaledRotated(float x,
                                                     float y,
                                                     float width,
                                                     float height,
                                                     float Winkel,
                                                     D3DCOLOR Color) {
    float l, r, o, u;      // Vertice Koordinaten
    float tl, tr, to, tu;  // Textur Koordinaten

    l = x;           // Links
    r = x + width;   // Rechts
    o = y;           // Oben
    u = y + height;  // Unten

    tl = itsRect.left * itsXTexScale;    // Links
    tr = itsRect.right * itsXTexScale;   // Rechts
    to = itsRect.top * itsYTexScale;     // Oben
    tu = itsRect.bottom * itsYTexScale;  // Unten

    VERTEX2D TriangleStrip[4];  // DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;

    TriangleStrip[0].x = l;  // Links oben
    TriangleStrip[0].y = o;
    TriangleStrip[0].tu = tl;
    TriangleStrip[0].tv = to;

    TriangleStrip[1].x = r;  // Rechts oben
    TriangleStrip[1].y = o;
    TriangleStrip[1].tu = tr;
    TriangleStrip[1].tv = to;

    TriangleStrip[2].x = l;  // Links unten
    TriangleStrip[2].y = u;
    TriangleStrip[2].tu = tl;
    TriangleStrip[2].tv = tu;

    TriangleStrip[3].x = r;  // Rechts unten
    TriangleStrip[3].y = u;
    TriangleStrip[3].tu = tr;
    TriangleStrip[3].tv = tu;

    DirectGraphics.SetTexture(itsTexIdx);

    //----- Sprite rotieren

    glm::mat4x4 matRot, matTrans, matTrans2;

    // Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
    clampAngle(Winkel);

    // Rotationsmatrix
    matRot = glm::rotate(glm::mat4x4(1.0f), PI * Winkel / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    // Transformation zum Ursprung
    D3DXMatrixTranslation(&matTrans, -x - (width) / 2, -y - (height) / 2, 0.0f);

    // Transformation wieder zurück
    D3DXMatrixTranslation(&matTrans2, x + (width) / 2, y + (height) / 2, 0.0f);

    // Verschieben und rotieren
    matWorld = matRot * matTrans;

    // und wieder zurück verschieben
    matWorld = matTrans2 * matWorld;
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

    DirectGraphics.SetFilterMode(true);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip[0]);

    DirectGraphics.SetFilterMode(false);

    // Normale Projektions-Matrix wieder herstellen
    matWorld = glm::mat4x4(1.0f);
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif
}

// --------------------------------------------------------------------------------------
// Sonstige Funtkionen, die nicht innerhalb eines Sprite Objektes liegen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Rechteck enzeigen
// --------------------------------------------------------------------------------------

void RenderRect4(float x, float y, float width, float height, D3DCOLOR c1, D3DCOLOR c2, D3DCOLOR c3, D3DCOLOR c4) {
    float l, r, o, u;  // Vertice Koordinaten

    l = x;           // Links
    r = x + width;   // Rechts
    o = y;           // Oben
    u = y + height;  // Unten

    VERTEX2D TriangleStrip[4];  // DKS - Added local declaration

    TriangleStrip[0].color = c1;
    TriangleStrip[0].x = l;  // Links oben
    TriangleStrip[0].y = o;

    TriangleStrip[1].color = c2;
    TriangleStrip[1].x = r;  // Rechts oben
    TriangleStrip[1].y = o;

    TriangleStrip[2].color = c3;
    TriangleStrip[2].x = l;  // Links unten
    TriangleStrip[2].y = u;

    TriangleStrip[3].color = c4;
    TriangleStrip[3].x = r;  // Rechts unten
    TriangleStrip[3].y = u;

    DirectGraphics.SetTexture(-1);

    // Rechteck zeichnen
    DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip[0]);
}

// --------------------------------------------------------------------------------------
// Linie rendern
// --------------------------------------------------------------------------------------

void RenderLine(D3DXVECTOR2 p1, D3DXVECTOR2 p2, D3DCOLOR Color) {
    VERTEX2D TriangleStrip[2];  // DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = Color;

    TriangleStrip[0].x = p1.x;  // p1
    TriangleStrip[0].y = p1.y;

    TriangleStrip[1].x = p2.x;  // p2
    TriangleStrip[1].y = p2.y;

    DirectGraphics.SetTexture(-1);

    // Linie zeichnen
    DirectGraphics.RendertoBuffer(GL_LINES, 1, &TriangleStrip[0]);
}

// --------------------------------------------------------------------------------------
// Linie rendern (zwei farbig)
// --------------------------------------------------------------------------------------

void RenderLine(D3DXVECTOR2 p1, D3DXVECTOR2 p2, D3DCOLOR Color1, D3DCOLOR Color2) {
    VERTEX2D TriangleStrip[2];  // DKS - Added local declaration

    TriangleStrip[0].color = Color1;
    TriangleStrip[1].color = Color2;

    TriangleStrip[0].x = p1.x;  // p1
    TriangleStrip[0].y = p1.y;

    TriangleStrip[1].x = p2.x;  // p2
    TriangleStrip[1].y = p2.y;

    DirectGraphics.SetTexture(-1);

    // Linie zeichnen
    DirectGraphics.RendertoBuffer(GL_LINES, 1, &TriangleStrip[0]);
}
