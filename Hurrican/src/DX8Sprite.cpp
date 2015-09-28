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

#include <stdio.h>
#if defined(PLATFORM_DIRECTX)
#include <d3dx8.h>										// Für die Texturen
#include <d3dx8math.h>									// Für D3DXVECTOR2
#endif
#include "DX8Sprite.h"
#include "DX8Graphics.h"
#include "Gameplay.h"
#include "Globals.h"
#include "Logdatei.h"
#include "Main.h"
#include "unrarlib.h"

extern bool					GameRunning;				// Läuft das Spiel noch ?

// --------------------------------------------------------------------------------------
// Variablen
// --------------------------------------------------------------------------------------

int						LoadedTextures = 0;					// Wieviele Texturen geladen ?
int						ActualTexture  = -1;				// aktuelle Textur (damit wir uns ein paar

// --------------------------------------------------------------------------------------
// Klassenfunktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// DirectGraphicsSurface Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

DirectGraphicsSurface::DirectGraphicsSurface(void)
{
    itsSurface = (LPDIRECT3DSURFACE8)NULL;
}

// --------------------------------------------------------------------------------------
// Desktruktor gibt die Surface wieder frei
// --------------------------------------------------------------------------------------

DirectGraphicsSurface::~DirectGraphicsSurface(void)
{
#if defined(PLATFORM_DIRECTX)
    SafeRelease(itsSurface);
#elif defined(PLATFORM_SDL)
    delete_texture( itsSurface );
    itsSurface = -1;
#endif
    Protokoll.WriteText( false, "Surface release		: successful !\n" );
}

// --------------------------------------------------------------------------------------
// Laden des Bildes "Filename" mit Grösse xSize, ySize in die Surface
// --------------------------------------------------------------------------------------

bool DirectGraphicsSurface::LoadImage(const char *Filename, int xSize, int ySize)
{
    HRESULT hresult;
    char	Temp[140];

#if defined(PLATFORM_DIRECTX)
    // Surface erstellen
    hresult = lpD3DDevice->CreateImageSurface(xSize, ySize, D3DFormat, &itsSurface);

    // Fehler beim Surface erstellen ?
    if(hresult != D3D_OK)
    {
        strcpy_s(Temp, strlen("Failed to create surface-for ") + 1, "Failed to create surface-for ");
        strcat_s(Temp, strlen(Filename) + 1, Filename);
        strcat_s(Temp, 3, " !");
        Protokoll.WriteText( true, Temp );
        return false;
    }

    // Bild laden
    hresult = D3DXLoadSurfaceFromFile(itsSurface, NULL, NULL, Filename, NULL,
                                      D3DX_FILTER_NONE, 0, NULL);

    // Fehler beim Laden ?
    if(hresult != D3D_OK)
    {
        strcpy_s(Temp, strlen("Failed to load ") + 1, "Failed to load ");
        strcat_s(Temp, strlen(Filename) + 1, Filename);
        strcat_s(Temp, 5, " !\n");
        Protokoll.WriteText( true, Temp );
        return false;
    }
#elif defined(PLATFORM_SDL)
    (void)hresult;

    SDL_Rect dims;
    itsSurface = LoadTexture( Filename, dims, 0 );
#endif

    // Grösse setzen
    itsRect.left   = 0;
    itsRect.top    = 0;
    itsRect.right  = xSize;
    itsRect.bottom = ySize;

    // Bild korrekt geladen
    strcat_s(Temp, strlen(TextArray [TEXT_LADE_BITMAP]) + 1, TextArray [TEXT_LADE_BITMAP]);
    strcat_s(Temp, strlen(Filename) + 1, Filename);
    strcat_s(Temp, strlen(TextArray [TEXT_LADEN_ERFOLGREICH]) + 1, TextArray [TEXT_LADEN_ERFOLGREICH]);
    strcat_s(Temp, 3, "\n");
    Protokoll.WriteText( false, Temp );

    return true;
}

// --------------------------------------------------------------------------------------
// Bild auf Surface anzeigen
// --------------------------------------------------------------------------------------

//DKS - This was never used anywhere, disabled:
//#if defined(PLATFORM_DIRECTX)
//bool DirectGraphicsSurface::DrawSurface(LPDIRECT3DSURFACE8 &Temp, int xPos, int yPos)
//{
//    POINT Dest = {(int)(xPos), (int)(yPos)};						// Zielkoordinaten
//    lpD3DDevice->CopyRects(itsSurface, &itsRect, 1, Temp, &Dest);	// anzeigen
//
//    return true;
//}
//#endif

// --------------------------------------------------------------------------------------
// DirectGraphicsSprite Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Desktruktor gibt die Textur wieder frei
// --------------------------------------------------------------------------------------

DirectGraphicsSprite::~DirectGraphicsSprite(void)
{
    //DKS - itsPreCalcedRects array is now dynamically allocated:
    if (itsPreCalcedRects != NULL) {
        free(itsPreCalcedRects);
        itsPreCalcedRects = NULL;
    }

    //char	Buffer[10]; //DKS - Disabled unused var

    if(itsTexture != 0)
    {
#if defined(PLATFORM_DIRECTX)
        SafeRelease(itsTexture);
#elif defined(PLATFORM_SDL)
        delete_texture( itsTexture );
        itsTexture = -1;
#endif
        itsTexture = (LPDIRECT3DTEXTURE8)NULL;
        LoadedTextures--;
//		Protokoll.WriteText( false, "-> Sprite texture successfully released ! \n" );

//        _itoa_s(LoadedTextures, Buffer, 10);
//		Protokoll.WriteText( false, Buffer );
//		Protokoll.WriteText( false, " Sprite Textur(en) übrig !\n" );
    }
}

// --------------------------------------------------------------------------------------
// Laden des Bildes "Filename"
// xfs, yfs Grösse eines Frames
// xfc, yfc Anzahl der Frames
// --------------------------------------------------------------------------------------

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
    (void)hresult;

    SDL_Rect        dims;
#endif

    // zuerst eine evtl benutzte Textur freigeben
#if defined(PLATFORM_DIRECTX)
    SafeRelease(itsTexture);
#elif defined(PLATFORM_SDL)
    delete_texture( itsTexture );
    itsTexture = 0;
#endif

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
        Protokoll.WriteText( true, "\n-> Error loading %s from Archive !\n", Filename );
        return false;
    }
    else
        fromrar = true;
#else
    Protokoll.WriteText( true, "\n-> Error loading %s!\n", Temp );
    return false;
#endif // USE_UNRARLIB


loadfile:

    // normal von Platte laden?
    if (fromrar == false)
    {
#if defined(PLATFORM_DIRECTX)
        // Textur laden
        hresult = D3DXCreateTextureFromFileEx(
                      lpD3DDevice,
                      Temp,
                      NULL, NULL,				  // x und y Grösse des Sprites (aus Datei übernehmen)
                      1,                          // Nur eine Version der Textur
                      0,                          // Immer 0 setzen
                      D3DFMT_UNKNOWN,			  // Format aus der Datei lesen
                      D3DPOOL_MANAGED,            // DX bestimmt wo die Textur gespeichert wird
                      D3DX_FILTER_NONE,			  // Keine Filter verwenden
                      D3DX_FILTER_NONE,
                      0xFFFF00FF,                 // Colorkeyfarbe (Lila)
                      NULL,						  // Keine Image Info
                      NULL,						  // Keine Palette angeben
                      &itsTexture);

#elif defined(PLATFORM_SDL)
        itsTexture = LoadTexture( Temp, dims, 0 );
#endif
    }
    else
    {
#if defined(PLATFORM_DIRECTX)
        // Textur aus Speicher erzeugen
        hresult = D3DXCreateTextureFromFileInMemoryEx(
                      lpD3DDevice,
                      (LPVOID)pData,
                      Size,
                      NULL, NULL,				  // x und y Grösse des Sprites (aus Datei übernehmen)
                      1,                          // Nur eine Version der Textur
                      0,                          // Immer 0 setzen
                      D3DFMT_UNKNOWN,			  // Format aus der Datei lesen
                      D3DPOOL_MANAGED,            // DX bestimmt wo die Textur gespeichert wird
                      D3DX_FILTER_NONE,			  // Keine Filter verwenden
                      D3DX_FILTER_NONE,
                      0xFFFF00FF,                 // Colorkeyfarbe (Lila)
                      NULL,						  // Keine Image Info
                      NULL,						  // Keine Palette angeben
                      &itsTexture);
#elif defined(PLATFORM_SDL)
        itsTexture = LoadTexture( pData, dims, Size );
#endif
        free(pData);
    }

#if defined(PLATFORM_DIRECTX)
    // Fehler beim Laden ?
    if(hresult != D3D_OK)
    {
        strcpy_s(Temp, strlen("Fehler beim Laden von ") + 1, "Fehler beim Laden von ");
        strcat_s(Temp, strlen(Filename) + 1, Filename);
        strcat_s(Temp, 4, " !\n");
        Protokoll.WriteText( true, Temp );
        return false;
    }

    // Grösse der Textur anpassen
    D3DSURFACE_DESC desc;
    itsTexture->GetLevelDesc(0,&desc);
#endif

    // Grösse setzen
#if defined(PLATFORM_DIRECTX)
    itsXSize		= (float)desc.Width;
    itsYSize		= (float)desc.Height;
#elif defined(PLATFORM_SDL)
    itsXSize		= (float)dims.w;
    itsYSize		= (float)dims.h;
#endif
    itsXFrameCount	= xfc;
    itsYFrameCount	= yfc;
    itsXFrameSize	= xfs;
    itsYFrameSize	= yfs;
    itsRect.left	= 0;
    itsRect.top		= 0;
    itsRect.right	= (int)itsXSize;
    itsRect.bottom	= (int)itsYSize;

    // Ausschnitte vorberechnen
    //DKS - array is now dynamically allocated
    if (itsPreCalcedRects != NULL) 
        free(itsPreCalcedRects);
    itsPreCalcedRects = (RECT*)malloc(xfc * yfc * sizeof(RECT));

    for (int i = 0; i < xfc * yfc; i++)
    {
        itsPreCalcedRects[i].top	= (i/itsXFrameCount) * itsYFrameSize;
        itsPreCalcedRects[i].left	= (i%itsXFrameCount) * itsXFrameSize;
        itsPreCalcedRects[i].right  = itsPreCalcedRects[i].left + itsXFrameSize;
        itsPreCalcedRects[i].bottom = itsPreCalcedRects[i].top  + itsYFrameSize;
    }

    itsRect = itsPreCalcedRects[0];

    // Bild korrekt geladen
    sprintf_s(Temp, "%s %s %s %s", TextArray [TEXT_LADE_BITMAP], Filename, TextArray [TEXT_LADEN_ERFOLGREICH], "\n");
    Protokoll.WriteText( false, Temp );

    DisplayLoadInfo(Temp);

    LoadedTextures++;							// Anzahl der geladenen Texturen erhöhen
    //_itoa_s(LoadedTextures, Temp, 10);
//	Protokoll.WriteText( false, Temp );
//	Protokoll.WriteText( false, " Sprite Textur(en) loaded !\n" );
    return true;
}

// --------------------------------------------------------------------------------------
// Sprite ganz normal zeichnen mit aktuellem Surfaceausschnitt und Colorkey
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderSprite(float x, float y, D3DCOLOR Color)
{
    float l,  r,  o,  u;					// Vertice Koordinaten
    float tl, tr, to, tu;					// Textur Koordinaten

    x = float (int (x));
    y = float (int (y));

    l = x;									// Links
    r = x+(itsRect.right-itsRect.left) - 1;	// Rechts
    o = y;									// Oben
    u = y+(itsRect.bottom-itsRect.top) - 1;	// Unten

    l -= TEXTURE_COORD_OFFSET;
    r += TEXTURE_COORD_OFFSET;
    o -= TEXTURE_COORD_OFFSET;
    u += TEXTURE_COORD_OFFSET;

    tl = itsRect.left  /itsXSize;	// Links
    tr = itsRect.right /itsXSize;	// Rechts
    to = itsRect.top   /itsYSize;	// Oben
    tu = itsRect.bottom/itsYSize;	// Unten

    VERTEX2D TriangleStrip[4]; //DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;

    TriangleStrip[0].x		= l;		// Links oben
    TriangleStrip[0].y		= o;
    TriangleStrip[0].tu		= tl;
    TriangleStrip[0].tv		= to;

    TriangleStrip[1].x		= r;		// Rechts oben
    TriangleStrip[1].y		= o;
    TriangleStrip[1].tu		= tr;
    TriangleStrip[1].tv		= to;

    TriangleStrip[2].x		= l;		// Links unten
    TriangleStrip[2].y		= u;
    TriangleStrip[2].tu		= tl;
    TriangleStrip[2].tv		= tu;

    TriangleStrip[3].x		= r;		// Rechts unten
    TriangleStrip[3].y		= u;
    TriangleStrip[3].tu		= tr;
    TriangleStrip[3].tv		= tu;

#if defined(PLATFORM_DIRECTX)
    //if (ActualTexture != (int)itsTexture)
    //{
    lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen
    //	ActualTexture = (int)itsTexture;
    //}
#elif defined(PLATFORM_SDL)
    DirectGraphics.SetTexture( itsTexture );
#endif

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);
}

// --------------------------------------------------------------------------------------
// Sprite ganz normal zeichnen mit aktuellem Surfaceausschnitt und Colorkey, dabei
// alle vier Eckpunkte anders einfärben
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderSprite(float x, float y, int Anim, D3DCOLOR c1, D3DCOLOR c2, D3DCOLOR c3, D3DCOLOR c4)
{
    float l,  r,  o,  u;					// Vertice Koordinaten
    float tl, tr, to, tu;					// Textur Koordinaten

    x = float (int (x));
    y = float (int (y));

    // Ausschnitt einstellen
    Anim %= 255;

    if (Anim > -1)
        itsRect = itsPreCalcedRects [Anim];

    l = x;									// Links
    r = x+(itsRect.right-itsRect.left) - 1;	// Rechts
    o = y;									// Oben
    u = y+(itsRect.bottom-itsRect.top) - 1;	// Unten

    l -= TEXTURE_COORD_OFFSET;
    r += TEXTURE_COORD_OFFSET;
    o -= TEXTURE_COORD_OFFSET;
    u += TEXTURE_COORD_OFFSET;

    tl = itsRect.left  /itsXSize;	// Links
    tr = itsRect.right /itsXSize;	// Rechts
    to = itsRect.top   /itsYSize;	// Oben
    tu = itsRect.bottom/itsYSize;	// Unten

    VERTEX2D TriangleStrip[4]; //DKS - Added local declaration

    TriangleStrip[0].x		= l;		// Links oben
    TriangleStrip[0].y		= o;
    TriangleStrip[0].color  = c1;
    TriangleStrip[0].tu		= tl;
    TriangleStrip[0].tv		= to;

    TriangleStrip[1].x		= r;		// Rechts oben
    TriangleStrip[1].y		= o;
    TriangleStrip[1].color  = c2;
    TriangleStrip[1].tu		= tr;
    TriangleStrip[1].tv		= to;

    TriangleStrip[2].x		= l;		// Links unten
    TriangleStrip[2].y		= u;
    TriangleStrip[2].color  = c3;
    TriangleStrip[2].tu		= tl;
    TriangleStrip[2].tv		= tu;

    TriangleStrip[3].x		= r;		// Rechts unten
    TriangleStrip[3].y		= u;
    TriangleStrip[3].color  = c4;
    TriangleStrip[3].tu		= tr;
    TriangleStrip[3].tv		= tu;

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen
#elif defined(PLATFORM_SDL)
    DirectGraphics.SetTexture(  itsTexture );
#endif

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);
}

// --------------------------------------------------------------------------------------
// Sprite vertikal spiegelverkehrt zeichnen
// --------------------------------------------------------------------------------------
void DirectGraphicsSprite::RenderMirroredSprite(float x, float y, D3DCOLOR Color, bool h, bool v)
{
    float l,  r,  o,  u;					// Vertice Koordinaten
    float tl, tr, to, tu;					// Textur Koordinaten

    x = float (int (x));
    y = float (int (y));

    if (v == false)
    {
        o = y-0.5f;									// Unten
        u = y+(itsRect.bottom-itsRect.top-1)+0.5f;	// Oben
    }
    else
    {
        u = y-0.5f;									// Unten
        o = y+(itsRect.bottom-itsRect.top-1)+0.5f;	// Oben
    }

    if (h == false)
    {
        r = x+(itsRect.right-itsRect.left-1)+0.5f;
        l = x-0.5f;
    }
    else
    {
        l = x+(itsRect.right-itsRect.left-1)+0.5f;
        r = x-0.5f;
    }

    tl = itsRect.left  /itsXSize;	// Links
    tr = itsRect.right /itsXSize;	// Rechts
    to = itsRect.top   /itsYSize;	// Oben
    tu = itsRect.bottom/itsYSize;	// Unten

    //tl = 0; tr = 0.5f; to = 0; tu = 0.5f;

    VERTEX2D TriangleStrip[4]; //DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;

    TriangleStrip[0].x		= l;		// Links oben
    TriangleStrip[0].y		= o;
    TriangleStrip[0].tu		= tl;
    TriangleStrip[0].tv		= to;

    TriangleStrip[1].x		= r;		// Rechts oben
    TriangleStrip[1].y		= o;
    TriangleStrip[1].tu		= tr;
    TriangleStrip[1].tv		= to;

    TriangleStrip[2].x		= l;		// Links unten
    TriangleStrip[2].y		= u;
    TriangleStrip[2].tu		= tl;
    TriangleStrip[2].tv		= tu;

    TriangleStrip[3].x		= r;		// Rechts unten
    TriangleStrip[3].y		= u;
    TriangleStrip[3].tu		= tr;
    TriangleStrip[3].tv		= tu;

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen
#elif defined(PLATFORM_SDL)
    DirectGraphics.SetTexture(  itsTexture );
#endif

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);
}

// --------------------------------------------------------------------------------------
// Sprite spiegelverkehrt zeichnen mit aktuellem Surfaceausschnitt und Colorkey
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderMirroredSprite(float x, float y, D3DCOLOR Color)
{
    float l,  r,  o,  u;					// Vertice Koordinaten
    float tl, tr, to, tu;					// Textur Koordinaten

    x = float (int (x));
    y = float (int (y));

    r = x;									// Links
    l = x+(itsRect.right-itsRect.left) - 1;	// Rechts
    o = y;									// Oben
    u = y+(itsRect.bottom-itsRect.top) - 1;	// Unten

    l += TEXTURE_COORD_OFFSET;
    r -= TEXTURE_COORD_OFFSET;
    o -= TEXTURE_COORD_OFFSET;
    u += TEXTURE_COORD_OFFSET;

    tl = itsRect.left  /itsXSize;	// Links
    tr = itsRect.right /itsXSize;	// Rechts
    to = itsRect.top   /itsYSize;	// Oben
    tu = itsRect.bottom/itsYSize;	// Unten

    //DKS - Was already commented out in original code:
    //tl = 0; tr = 0.5f; to = 0; tu = 0.5f;

    VERTEX2D TriangleStrip[4]; //DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;

    TriangleStrip[0].x		= l;		// Links oben
    TriangleStrip[0].y		= o;
    TriangleStrip[0].tu		= tl;
    TriangleStrip[0].tv		= to;

    TriangleStrip[1].x		= r;		// Rechts oben
    TriangleStrip[1].y		= o;
    TriangleStrip[1].tu		= tr;
    TriangleStrip[1].tv		= to;

    TriangleStrip[2].x		= l;		// Links unten
    TriangleStrip[2].y		= u;
    TriangleStrip[2].tu		= tl;
    TriangleStrip[2].tv		= tu;

    TriangleStrip[3].x		= r;		// Rechts unten
    TriangleStrip[3].y		= u;
    TriangleStrip[3].tu		= tr;
    TriangleStrip[3].tv		= tu;

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen
#elif defined(PLATFORM_SDL)
    DirectGraphics.SetTexture(  itsTexture );
#endif

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);
}

// --------------------------------------------------------------------------------------
// Sprite skaliert zeichnen mit aktuellem Surfaceausschnitt (mit textur Filter)
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderSpriteScaled(float x, float y,int width, int height, D3DCOLOR col)
{
    float l,  r,  o,  u;					// Vertice Koordinaten
    float tl, tr, to, tu;					// Textur Koordinaten

    l = x;			// Links
    r = x+width;	// Rechts
    o = y;			// Oben
    u = y+height;	// Unten

    l -= TEXTURE_COORD_OFFSET;
    r += TEXTURE_COORD_OFFSET;
    o -= TEXTURE_COORD_OFFSET;
    u += TEXTURE_COORD_OFFSET;

    tl = itsRect.left  /itsXSize;	// Links
    tr = itsRect.right /itsXSize;	// Rechts
    to = itsRect.top   /itsYSize;	// Oben
    tu = itsRect.bottom/itsYSize;	// Unten

    VERTEX2D TriangleStrip[4]; //DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = col;

    TriangleStrip[0].x		= l;		// Links oben
    TriangleStrip[0].y		= o;
    TriangleStrip[0].tu		= tl;
    TriangleStrip[0].tv		= to;

    TriangleStrip[1].x		= r;		// Rechts oben
    TriangleStrip[1].y		= o;
    TriangleStrip[1].tu		= tr;
    TriangleStrip[1].tv		= to;

    TriangleStrip[2].x		= l;		// Links unten
    TriangleStrip[2].y		= u;
    TriangleStrip[2].tu		= tl;
    TriangleStrip[2].tv		= tu;

    TriangleStrip[3].x		= r;		// Rechts unten
    TriangleStrip[3].y		= u;
    TriangleStrip[3].tu		= tr;
    TriangleStrip[3].tv		= tu;

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen
#elif defined(PLATFORM_SDL)
    DirectGraphics.SetTexture(  itsTexture );
#endif

    DirectGraphics.SetFilterMode (true);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);

    DirectGraphics.SetFilterMode (false);
}

// --------------------------------------------------------------------------------------
// Sprite skaliert zeichnen mit angegebener Animationsphase (mit textur Filter)
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderSpriteScaled(float x, float y,int width, int height, int Anim, D3DCOLOR col)
{
    float l,  r,  o,  u;					// Vertice Koordinaten
    float tl, tr, to, tu;					// Textur Koordinaten

    l = x;			// Links
    r = x+width;	// Rechts
    o = y;			// Oben
    u = y+height;	// Unten

    l -= TEXTURE_COORD_OFFSET;
    r += TEXTURE_COORD_OFFSET;
    o -= TEXTURE_COORD_OFFSET;
    u += TEXTURE_COORD_OFFSET;

    tl = ((Anim%itsXFrameCount) * itsXFrameSize)				 / itsXSize;	// Links
    tr = ((Anim%itsXFrameCount) * itsXFrameSize + itsXFrameSize) / itsXSize;	// Rechts
    to = ((Anim/itsXFrameCount) * itsYFrameSize)				 / itsYSize;	// Oben
    tu = ((Anim/itsXFrameCount) * itsYFrameSize + itsYFrameSize) / itsYSize;	// Unten

    VERTEX2D TriangleStrip[4]; //DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = col;

    TriangleStrip[0].x		= l;		// Links oben
    TriangleStrip[0].y		= o;
    TriangleStrip[0].tu		= tl;
    TriangleStrip[0].tv		= to;

    TriangleStrip[1].x		= r;		// Rechts oben
    TriangleStrip[1].y		= o;
    TriangleStrip[1].tu		= tr;
    TriangleStrip[1].tv		= to;

    TriangleStrip[2].x		= l;		// Links unten
    TriangleStrip[2].y		= u;
    TriangleStrip[2].tu		= tl;
    TriangleStrip[2].tv		= tu;

    TriangleStrip[3].x		= r;		// Rechts unten
    TriangleStrip[3].y		= u;
    TriangleStrip[3].tu		= tr;
    TriangleStrip[3].tv		= tu;

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen
#elif defined(PLATFORM_SDL)
    DirectGraphics.SetTexture(  itsTexture );
#endif

    DirectGraphics.SetFilterMode (true);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);

    DirectGraphics.SetFilterMode (false);
}

// --------------------------------------------------------------------------------------
// Sprite mit übergebenem Winkel rotiert darstellen
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderSpriteRotated(float x, float y, float Winkel, D3DCOLOR Color)
{
    float l,  r,  o,  u;					// Vertice Koordinaten
    float tl, tr, to, tu;					// Textur Koordinaten

    l = x;									// Links
    r = x+(itsRect.right-itsRect.left) - 1;	// Rechts
    o = y;									// Oben
    u = y+(itsRect.bottom-itsRect.top) - 1;	// Unten

    l -= TEXTURE_COORD_OFFSET;
    r += TEXTURE_COORD_OFFSET;
    o -= TEXTURE_COORD_OFFSET;
    u += TEXTURE_COORD_OFFSET;

    tl = itsRect.left  /itsXSize;	// Links
    tr = itsRect.right /itsXSize;	// Rechts
    to = itsRect.top   /itsYSize;	// Oben
    tu = itsRect.bottom/itsYSize;	// Unten

    VERTEX2D TriangleStrip[4]; //DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;

    TriangleStrip[0].x		= l;		// Links oben
    TriangleStrip[0].y		= o;
    TriangleStrip[0].tu		= tl;
    TriangleStrip[0].tv		= to;

    TriangleStrip[1].x		= r;		// Rechts oben
    TriangleStrip[1].y		= o;
    TriangleStrip[1].tu		= tr;
    TriangleStrip[1].tv		= to;

    TriangleStrip[2].x		= l;		// Links unten
    TriangleStrip[2].y		= u;
    TriangleStrip[2].tu		= tl;
    TriangleStrip[2].tv		= tu;

    TriangleStrip[3].x		= r;		// Rechts unten
    TriangleStrip[3].y		= u;
    TriangleStrip[3].tu		= tr;
    TriangleStrip[3].tv		= tu;

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen
#elif defined(PLATFORM_SDL)
    DirectGraphics.SetTexture(  itsTexture );
#endif

//----- Sprite rotieren

    D3DXMATRIX	matRot, matTrans, matTrans2;

    // Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
    if (Winkel > 360) Winkel -= 360;
    if (Winkel < 0)	  Winkel += 360;

    // Rotationsmatrix
    D3DXMatrixRotationZ  (&matRot, float(PI * Winkel / 180));

    // Transformation zum Ursprung
    D3DXMatrixTranslation(&matTrans,-x-(itsRect.right  - itsRect.left)/2.0f,
                          -y-(itsRect.bottom - itsRect.top )/2.0f, 0.0f);

    // Transformation wieder zurück
    D3DXMatrixTranslation(&matTrans2,x+(itsRect.right  - itsRect.left)/2.0f,
                          y+(itsRect.bottom - itsRect.top )/2.0f, 0.0f);

    D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans);		// Verschieben
    D3DXMatrixMultiply	 (&matWorld, &matWorld, &matRot);		// rotieren
    D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans2);	// und wieder zurück verschieben
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
#elif defined(PLATFORM_SDL)
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix( GL_MODELVIEW, g_matModelView.data() );
#endif
#endif

    DirectGraphics.SetFilterMode (true);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);

    DirectGraphics.SetFilterMode (false);

    // Normale Projektions-Matrix wieder herstellen
    D3DXMatrixRotationZ (&matWorld, 0.0f);
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
#elif defined(PLATFORM_SDL)
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix( GL_MODELVIEW, g_matModelView.data() );
#endif
#endif
}

// --------------------------------------------------------------------------------------
// Sprite mit übergebenem Winkel rotiert darstellen
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderSpriteRotated(float x, float y, float Winkel, int Anim, D3DCOLOR Color, bool mirror)
{
    float l,  r,  o,  u;					// Vertice Koordinaten
    float tl, tr, to, tu;					// Textur Koordinaten

    // Ausschnitt einstellen
    Anim %= 255;
    itsRect = itsPreCalcedRects [Anim];

    //DKS - Pulled duplicated math out of logic below:
    float height = itsRect.bottom - itsRect.top;
    float width = itsRect.right - itsRect.left;

    // normal
    //
    if (mirror == false)
    {
        l = x-0.5f;									// Links
        //r = x+(itsRect.right-itsRect.left-1)+0.5f;	// Rechts   //DKS
		r = x+(width-1)+0.5f;	// Rechts

        Winkel = 360 - Winkel;
    }

    // oder gespiegelt
    //
    else
    {
        r = x+0.5f;									// Links
        //l = x+(itsRect.right-itsRect.left-1)-0.5f;	// Rechts   //DKS
		l = x+(width-1)-0.5f;	// Rechts
    }

    o = y-0.5f;									// Oben
    //u = y+(itsRect.bottom-itsRect.top-1)+0.5f;	// Unten    //DKS
	u = y+(height-1)+0.5f;	// Unten

    to = itsRect.top   /itsYSize;	// Oben
    tu = itsRect.bottom/itsYSize;	// Unten
    tl = itsRect.left  /itsXSize;	// Links
    tr = itsRect.right /itsXSize;	// Rechts

    VERTEX2D TriangleStrip[4]; //DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;

    TriangleStrip[0].x		= l;		// Links oben
    TriangleStrip[0].y		= o;
    TriangleStrip[0].tu		= tl;
    TriangleStrip[0].tv		= to;

    TriangleStrip[1].x		= r;		// Rechts oben
    TriangleStrip[1].y		= o;
    TriangleStrip[1].tu		= tr;
    TriangleStrip[1].tv		= to;

    TriangleStrip[2].x		= l;		// Links unten
    TriangleStrip[2].y		= u;
    TriangleStrip[2].tu		= tl;
    TriangleStrip[2].tv		= tu;

    TriangleStrip[3].x		= r;		// Rechts unten
    TriangleStrip[3].y		= u;
    TriangleStrip[3].tu		= tr;
    TriangleStrip[3].tv		= tu;

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen
#elif defined(PLATFORM_SDL)
    DirectGraphics.SetTexture(  itsTexture );
#endif

//----- Sprite rotieren

    D3DXMATRIX	matRot, matTrans, matTrans2;

    // Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
    if (Winkel > 360) Winkel -= 360;
    if (Winkel < 0)	  Winkel += 360;

    // Rotationsmatrix
    D3DXMatrixRotationZ  (&matRot, float(PI * Winkel / 180));

    // Transformation zum Ursprung
    D3DXMatrixTranslation(&matTrans,-x-(itsRect.right  - itsRect.left)/2.0f,
                          -y-(itsRect.bottom - itsRect.top )/2.0f, 0.0f);

    // Transformation wieder zurück
    D3DXMatrixTranslation(&matTrans2,x+(itsRect.right  - itsRect.left)/2.0f,
                          y+(itsRect.bottom - itsRect.top )/2.0f, 0.0f);

    D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans);		// Verschieben
    D3DXMatrixMultiply	 (&matWorld, &matWorld, &matRot);		// rotieren
    D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans2);	// und wieder zurück verschieben
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
#elif defined(PLATFORM_SDL)
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix( GL_MODELVIEW, g_matModelView.data() );
#endif
#endif

    DirectGraphics.SetFilterMode (true);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);

    DirectGraphics.SetFilterMode (false);

    // Normale Projektions-Matrix wieder herstellen
    D3DXMatrixRotationZ (&matWorld, 0.0f);
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
#elif defined(PLATFORM_SDL)
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix( GL_MODELVIEW, g_matModelView.data() );
#endif
#endif
}

// --------------------------------------------------------------------------------------
// Sprite rotiert darstellen mit Verschiebung
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderSpriteRotatedOffset(float x, float y, float Winkel, float offx, float offy, D3DCOLOR Color, bool mirrored)
{
    float l,  r,  o,  u;					// Vertice Koordinaten
    float tl, tr, to, tu;					// Textur Koordinaten

//	itsRect = itsPreCalcedRects [0];    //DKS - This was commented out already in original source

    //DKS - Pulled duplicated math out of logic below:
    float height = itsRect.bottom - itsRect.top;
    float width = itsRect.right - itsRect.left;

    l = x-0.5f;									// Links
    //r = x+(itsRect.right-itsRect.left-1)+0.5f;	// Rechts       //DKS
	r = x+(width-1)+0.5f;	// Rechts

    if (mirrored)
    {
        float t = l;
        l = r;
        r = t;

        offx = -offx;
        Winkel = 360.0f - Winkel;
    }

    Winkel = 360 - Winkel;

    o = y-0.5f;									// Oben
    //u = y+(itsRect.bottom-itsRect.top-1)+0.5f;	// Unten        //DKS
	u = y+(height-1)+0.5f;	// Unten

    to = itsRect.top   /itsYSize;	// Oben
    tu = itsRect.bottom/itsYSize;	// Unten
    tl = itsRect.left  /itsXSize;	// Links
    tr = itsRect.right /itsXSize;	// Rechts

    VERTEX2D TriangleStrip[4]; //DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;

    TriangleStrip[0].x		= l;		// Links oben
    TriangleStrip[0].y		= o;
    TriangleStrip[0].tu		= tl;
    TriangleStrip[0].tv		= to;

    TriangleStrip[1].x		= r;		// Rechts oben
    TriangleStrip[1].y		= o;
    TriangleStrip[1].tu		= tr;
    TriangleStrip[1].tv		= to;

    TriangleStrip[2].x		= l;		// Links unten
    TriangleStrip[2].y		= u;
    TriangleStrip[2].tu		= tl;
    TriangleStrip[2].tv		= tu;

    TriangleStrip[3].x		= r;		// Rechts unten
    TriangleStrip[3].y		= u;
    TriangleStrip[3].tu		= tr;
    TriangleStrip[3].tv		= tu;

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen
#elif defined(PLATFORM_SDL)
    DirectGraphics.SetTexture(  itsTexture );
#endif


//----- Sprite rotieren

    D3DXMATRIX	matRot, matTrans, matTrans2;

    // Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
    while (Winkel > 360) Winkel -= 360;
    while (Winkel < 0)	  Winkel += 360;

    // Rotationsmatrix
    D3DXMatrixRotationZ  (&matRot, float(PI * Winkel / 180));

    // Transformation zum Ursprung
    D3DXMatrixTranslation(&matTrans,-x-(itsRect.right  - itsRect.left)/2.0f-offx,
                          -y-(itsRect.bottom - itsRect.top )/2.0f-offy, 0.0f);

    // Transformation wieder zurück
    D3DXMatrixTranslation(&matTrans2,x+(itsRect.right  - itsRect.left)/2.0f+offx,
                          y+(itsRect.bottom - itsRect.top )/2.0f+offy, 0.0f);

    D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans);		// Verschieben
    D3DXMatrixMultiply	 (&matWorld, &matWorld, &matRot);		// rotieren
    D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans2);	// und wieder zurück verschieben
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
#elif defined(PLATFORM_SDL)
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix( GL_MODELVIEW, g_matModelView.data() );
#endif
#endif

    DirectGraphics.SetFilterMode (true);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);

    DirectGraphics.SetFilterMode (false);

    // Normale Projektions-Matrix wieder herstellen
    D3DXMatrixRotationZ (&matWorld, 0.0f);
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
#elif defined(PLATFORM_SDL)
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix( GL_MODELVIEW, g_matModelView.data() );
#endif
#endif
}

// --------------------------------------------------------------------------------------
// Sprite mit übergebenem Winkel rotiert darstellen in beliebiger Grösse
// --------------------------------------------------------------------------------------

void DirectGraphicsSprite::RenderSpriteScaledRotated(float x, float y,
        float width, float height,
        float Winkel, D3DCOLOR Color)
{
    float l,  r,  o,  u;					// Vertice Koordinaten
    float tl, tr, to, tu;					// Textur Koordinaten

    l = x;			// Links
    r = x+width;	// Rechts
    o = y;			// Oben
    u = y+height;	// Unten

    l -= TEXTURE_COORD_OFFSET;
    r += TEXTURE_COORD_OFFSET;
    o -= TEXTURE_COORD_OFFSET;
    u += TEXTURE_COORD_OFFSET;

    tl = itsRect.left  /itsXSize;	// Links
    tr = itsRect.right /itsXSize;	// Rechts
    to = itsRect.top   /itsYSize;	// Oben
    tu = itsRect.bottom/itsYSize;	// Unten

    VERTEX2D TriangleStrip[4]; //DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;

    TriangleStrip[0].x		= l;		// Links oben
    TriangleStrip[0].y		= o;
    TriangleStrip[0].tu		= tl;
    TriangleStrip[0].tv		= to;

    TriangleStrip[1].x		= r;		// Rechts oben
    TriangleStrip[1].y		= o;
    TriangleStrip[1].tu		= tr;
    TriangleStrip[1].tv		= to;

    TriangleStrip[2].x		= l;		// Links unten
    TriangleStrip[2].y		= u;
    TriangleStrip[2].tu		= tl;
    TriangleStrip[2].tv		= tu;

    TriangleStrip[3].x		= r;		// Rechts unten
    TriangleStrip[3].y		= u;
    TriangleStrip[3].tu		= tr;
    TriangleStrip[3].tv		= tu;

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTexture (0, itsTexture);								// Textur setzen
#elif defined(PLATFORM_SDL)
    DirectGraphics.SetTexture(  itsTexture );
#endif

//----- Sprite rotieren

    D3DXMATRIX	matRot, matTrans, matTrans2;

    // Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
    if (Winkel > 360) Winkel -= 360;
    if (Winkel < 0)	  Winkel += 360;

    // Rotationsmatrix
    D3DXMatrixRotationZ  (&matRot, float(PI * Winkel / 180));

    // Transformation zum Ursprung
    D3DXMatrixTranslation(&matTrans,-x-(width) /2,
                          -y-(height)/2, 0.0f);

    // Transformation wieder zurück
    D3DXMatrixTranslation(&matTrans2,x+(width) /2,
                          y+(height)/2, 0.0f);

    // Verschieben und rotieren
    D3DXMatrixMultiply	 (&matWorld, &matTrans, &matRot);

    // und wieder zurück verschieben
    D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans2);
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
#elif defined(PLATFORM_SDL)
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix( GL_MODELVIEW, g_matModelView.data() );
#endif
#endif

    DirectGraphics.SetFilterMode (true);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);

    DirectGraphics.SetFilterMode (false);

    // Normale Projektions-Matrix wieder herstellen
    D3DXMatrixRotationZ (&matWorld, 0.0f);
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
#elif defined(PLATFORM_SDL)
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix( GL_MODELVIEW, g_matModelView.data() );
#endif
#endif
}

// --------------------------------------------------------------------------------------
// Sonstige Funtkionen, die nicht innerhalb eines Sprite Objektes liegen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Rechteck enzeigen
// --------------------------------------------------------------------------------------

void RenderRect4(float x, float y, float width, float height,
                 D3DCOLOR c1,
                 D3DCOLOR c2,
                 D3DCOLOR c3,
                 D3DCOLOR c4)
{
    float l,  r,  o,  u;					// Vertice Koordinaten

    l = x;			// Links
    r = x+width - 1;	// Rechts
    o = y;			// Oben
    u = y+height - 1;	// Unten

    l -= TEXTURE_COORD_OFFSET;
    r += TEXTURE_COORD_OFFSET;
    o -= TEXTURE_COORD_OFFSET;
    u += TEXTURE_COORD_OFFSET;

    VERTEX2D TriangleStrip[4]; //DKS - Added local declaration

    TriangleStrip[0].color = c1;
    TriangleStrip[0].x		= l;		// Links oben
    TriangleStrip[0].y		= o;

    TriangleStrip[1].color = c2;
    TriangleStrip[1].x		= r;		// Rechts oben
    TriangleStrip[1].y		= o;

    TriangleStrip[2].color = c3;
    TriangleStrip[2].x		= l;		// Links unten
    TriangleStrip[2].y		= u;

    TriangleStrip[3].color = c4;
    TriangleStrip[3].x		= r;		// Rechts unten
    TriangleStrip[3].y		= u;

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTexture (0, NULL);								// Textur setzen
#elif defined(PLATFORM_SDL)
    DirectGraphics.SetTexture( -1 );
#endif

    // Rechteck zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2, &TriangleStrip[0]);
}

// --------------------------------------------------------------------------------------
// Linie rendern
// --------------------------------------------------------------------------------------

void RenderLine(D3DXVECTOR2 p1, D3DXVECTOR2 p2,	D3DCOLOR Color)
{
    VERTEX2D TriangleStrip[2]; //DKS - Added local declaration

    TriangleStrip[0].color = TriangleStrip[1].color = Color;

    TriangleStrip[0].x		= p1.x;		// p1
    TriangleStrip[0].y		= p1.y;

    TriangleStrip[1].x		= p2.x;		// p2
    TriangleStrip[1].y		= p2.y;;

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTexture (0, NULL);								// Textur setzen
#elif defined(PLATFORM_SDL)
    DirectGraphics.SetTexture( -1 );
#endif

    // Linie zeichnen
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST,1,&TriangleStrip[0],sizeof(VERTEX2D));
#elif defined(PLATFORM_SDL)
    DirectGraphics.RendertoBuffer (D3DPT_LINELIST, 1, &TriangleStrip[0]);
#endif
}

// --------------------------------------------------------------------------------------
// Linie rendern (zwei farbig)
// --------------------------------------------------------------------------------------

void RenderLine(D3DXVECTOR2 p1, D3DXVECTOR2 p2,	D3DCOLOR Color1, D3DCOLOR Color2)
{
    VERTEX2D TriangleStrip[2]; //DKS - Added local declaration

    TriangleStrip[0].color = Color1;
    TriangleStrip[1].color = Color2;

    TriangleStrip[0].x		= p1.x;		// p1
    TriangleStrip[0].y		= p1.y;

    TriangleStrip[1].x		= p2.x;		// p2
    TriangleStrip[1].y		= p2.y;;

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTexture (0, NULL);								// Textur setzen
#elif defined(PLATFORM_SDL)
    DirectGraphics.SetTexture( -1 );
#endif

    // Linie zeichnen
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST,1,&TriangleStrip[0],sizeof(VERTEX2D));
#elif defined(PLATFORM_SDL)
    DirectGraphics.RendertoBuffer (D3DPT_LINELIST, 1, &TriangleStrip[0]);
#endif
}

// --------------------------------------------------------------------------------------
// Kreis an x/y zeichnen mit Radius r und Farbe col
// --------------------------------------------------------------------------------------
//DKS - disabled this as it was only used in a single debug code block in Trigger_FadeMusic.cpp
#if 0
void RenderCircle(float x, float y, float r, D3DCOLOR col)
{
	#define step 0.2f

	D3DXVECTOR2 p1, p2;
	for (float i = 0; i < 2*PI; i += step)
	{
      //SKD - converted sin/cos to sinf/cosf:
		p1.x = x + (float)sinf(i) * r;
		p1.y = y + (float)cosf(i) * r;

		p2.x = x + (float)sinf(i + step) * r;
		p2.y = y + (float)cosf(i + step) * r;

		RenderLine(p1, p2, col);
	}
}
#endif
