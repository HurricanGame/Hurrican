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
#include "DX8Sprite.h"
#include <d3dx8.h>										// Für die Texturen
#include <d3dx8math.h>									// Für D3DXVECTOR2
#include "DX8Graphics.h"
#include "Gameplay.h"
#include "Globals.h"
#include "Logdatei.h"
#include "Main.h"
#include "Unrarlib.h"

extern bool					GameRunning;				// Läuft das Spiel noch ?

// --------------------------------------------------------------------------------------
// Variablen
// --------------------------------------------------------------------------------------

RECT					ClippingArea;						// Ränder für das Clipping
VERTEX2D				TriangleStrip[4];					// Strip für ein Sprite
VERTEX2D				v1, v2, v3, v4;						// Für Sprites =)
int						LoadedTextures = 0;					// Wieviele Texturen geladen ?
int						ActualTexture  = -1;				// aktuelle Textur (damit wir uns ein paar
															// "SetTextures" sparen können

// --------------------------------------------------------------------------------------
// Globale Spritefunktionen ausserhalb der Klassen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Neue ClippingArea setzen
// --------------------------------------------------------------------------------------

void SetClippingArea(int Left, int Top, int Right, int Bottom)
{
	ClippingArea.left	= Left;
	ClippingArea.top	= Top;
	ClippingArea.right	= Right;
	ClippingArea.bottom = Bottom;
}

// --------------------------------------------------------------------------------------
// Aktuelle Clipping Area abfragen
// --------------------------------------------------------------------------------------

RECT GetClippingArea(void)
{
	return ClippingArea;
}

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
	itsSurface = NULL;
}

// --------------------------------------------------------------------------------------
// Desktruktor gibt die Surface wieder frei
// --------------------------------------------------------------------------------------

DirectGraphicsSurface::~DirectGraphicsSurface(void)
{
	SafeRelease(itsSurface);
	Protokoll.WriteText("Surface freigeben		: erfolgreich !\n", false);
}

// --------------------------------------------------------------------------------------
// Laden des Bildes "Filename" mit Grösse xSize, ySize in die Surface
// --------------------------------------------------------------------------------------

bool DirectGraphicsSurface::LoadImage(const char *Filename, int xSize, int ySize)
{	
	HRESULT hresult;
	char	Temp[140];

	// Surface erstellen
	hresult = lpD3DDevice->CreateImageSurface(xSize, ySize, D3DFormat, &itsSurface);

	// Fehler beim Surface erstellen ?
	if(hresult != D3D_OK)
	{
		strcpy_s(Temp, strlen("Fehler beim Surface-Erstellen für ") + 1, "Fehler beim Surface-Erstellen für ");
		strcat_s(Temp, strlen(Filename) + 1, Filename);
		strcat_s(Temp, 3, " !");
		Protokoll.WriteText(Temp, true);
		return false;
	}

	// Bild laden
	hresult = D3DXLoadSurfaceFromFile(itsSurface, NULL, NULL, Filename, NULL, 
									  D3DX_FILTER_NONE, 0, NULL);

	// Fehler beim Laden ?
	if(hresult != D3D_OK)
	{
		strcpy_s(Temp, strlen("Fehler beim Laden von ") + 1, "Fehler beim Laden von ");
		strcat_s(Temp, strlen(Filename) + 1, Filename);
		strcat_s(Temp, 5, " !\n");
		Protokoll.WriteText(Temp, true);
		return false;
	}
	
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
	Protokoll.WriteText(Temp, false);
	return true;
}

// --------------------------------------------------------------------------------------
// Neuen Ausschnitt setzen
// --------------------------------------------------------------------------------------

bool DirectGraphicsSurface::SetRect(int left,  int top, int right, int bottom)
{
	itsRect.left   = left;										// Linker  Rand
	itsRect.right  = right;										// Rechter Rand
	itsRect.top    = top;										// Oberer  Rand
	itsRect.bottom = bottom;									// Unterer Rand
	return true;
}

// --------------------------------------------------------------------------------------
// Ausschnitt holen
// --------------------------------------------------------------------------------------
		
RECT DirectGraphicsSurface::GetRect(void)
{
	return itsRect;
}

// --------------------------------------------------------------------------------------
// Bild auf Surface anzeigen
// --------------------------------------------------------------------------------------

bool DirectGraphicsSurface::DrawSurface(LPDIRECT3DSURFACE8 &Temp, int xPos, int yPos)
{
	POINT Dest = {(int)(xPos), (int)(yPos)};						// Zielkoordinaten
	lpD3DDevice->CopyRects(itsSurface, &itsRect, 1, Temp, &Dest);	// anzeigen
	return true;
}

// --------------------------------------------------------------------------------------
// DirectGraphicsSprite Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor (leer)
// --------------------------------------------------------------------------------------

DirectGraphicsSprite::DirectGraphicsSprite(void)
{
	itsTexture = NULL;
}

// --------------------------------------------------------------------------------------
// Desktruktor gibt die Textur wieder frei
// --------------------------------------------------------------------------------------

DirectGraphicsSprite::~DirectGraphicsSprite(void)
{
	char	Buffer[10];

	if(itsTexture != NULL)
	{
		SafeRelease(itsTexture);
		itsTexture = NULL;
		LoadedTextures--;
//		Protokoll.WriteText("-> Sprite Textur erfolgreich freigegeben ! \n", false);

		_itoa_s(LoadedTextures, Buffer, 10);
//		Protokoll.WriteText(Buffer, false);
//		Protokoll.WriteText(" Sprite Textur(en) übrig !\n", false);
	}
}

// --------------------------------------------------------------------------------------
// Laden des Bildes "Filename" 
// xfs, yfs Grösse eines Frames
// xfc, yfc Anzahl der Frames
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::LoadImage(char *Filename, int xs, int ys, int xfs, int yfs, 
														 int xfc,  int yfc)
{
	if(GameRunning == false) 
		return false;

	bool			fromrar;
	HRESULT			hresult;
	char			*pData; 
	char			Temp[256];
	unsigned long	Size; 

	// zuerst eine evtl benutzte Textur freigeben
	SafeRelease(itsTexture);

	fromrar = false;

	// Zuerst checken, ob sich die Grafik in einem MOD-Ordner befindet
	if (CommandLineParams.RunOwnLevelList == true)
	{
		sprintf_s(Temp, "levels/%s/%s", CommandLineParams.OwnLevelList, Filename);
		if (FileExists(Temp))
			goto loadfile;
	}

	// Dann checken, ob sich das File im Standard Ordner befindet
	sprintf_s(Temp, "data/%s", Filename);
	if (FileExists(Temp))
		goto loadfile;

	// Auch nicht? Dann ist es hoffentlich im RAR file
	if (urarlib_get(&pData, &Size, Filename, RARFILENAME, convertText(RARFILEPASSWORD)) == false)
	{		
		sprintf_s(Temp, "\n-> Error loading %s from Archive !\n", Filename);
		Protokoll.WriteText(Temp, true);
		return false;
	}
	else
		fromrar = true;

loadfile:

	// normal von Platte laden?
	if (fromrar == false)
	{
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
	}
	else
	{
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

		free(pData);
	}

	// Fehler beim Laden ?
	if(hresult != D3D_OK)
	{
		strcpy_s(Temp, strlen("Fehler beim Laden von ") + 1, "Fehler beim Laden von ");
		strcat_s(Temp, strlen(Filename) + 1, Filename);
		strcat_s(Temp, 4, " !\n");
		Protokoll.WriteText(Temp, true);
		return false;
	}
	
	// Grösse der Textur anpassen
	D3DSURFACE_DESC desc;
	itsTexture->GetLevelDesc(0,&desc);	

	// Grösse setzen
	itsXSize		= (float)desc.Width;
	itsYSize		= (float)desc.Height;
	itsXFrameCount	= xfc;
	itsYFrameCount	= yfc;
	itsXFrameSize	= xfs;
	itsYFrameSize	= yfs;
	itsRect.left	= 0;
	itsRect.top		= 0;
	itsRect.right	= (int)itsXSize;
	itsRect.bottom	= (int)itsYSize;

	// Ausschnitte vorberechnen
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
	Protokoll.WriteText(Temp, false);			

	DisplayLoadInfo(Temp);

	LoadedTextures++;							// Anzahl der geladenen Texturen erhöhen
	//_itoa_s(LoadedTextures, Temp, 10);
//	Protokoll.WriteText(Temp, false);
//	Protokoll.WriteText(" Sprite Textur(en) geladen !\n", false);
	return true;
}

// --------------------------------------------------------------------------------------
// Neuen Ausschnitt setzen
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::SetRect(int left,  int top, int right, int bottom)
{
	itsRect.left   = left;										// Linker  Rand
	itsRect.right  = right;										// Rechter Rand
	itsRect.top    = top;										// Oberer  Rand
	itsRect.bottom = bottom;									// Unterer Rand
	return true;
}

// --------------------------------------------------------------------------------------
// aktuellen Ausschnitt holen
// --------------------------------------------------------------------------------------

RECT DirectGraphicsSprite::GetRect(void)
{
	return itsRect;
}

// --------------------------------------------------------------------------------------
// Textur ganz normal zeichnen mit u und v als Texturkoordinaten
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderTexture(float x, float y, float w, float h, float tl, float to, float tr, float tu, D3DCOLOR Color)
{
	float l,  r,  o,  u;					// Vertice Koordinaten

	x = float (int (x));
	y = float (int (y));

	l = x;		// Links
	r = x+w;	// Rechts
	o = y;		// Oben
	u = y+h;	// Unten

	l -= 0.5f;
	r += 0.5f;
	o -= 0.5f;
	u += 0.5f;

	TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;
	TriangleStrip[0].z	   = TriangleStrip[1].z		= TriangleStrip[2].z	 = TriangleStrip[3].z	  = 0.0f;

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

	lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen

    // Sprite zeichnen	
	DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);	
    
	return true;
}

// --------------------------------------------------------------------------------------
// Sprite ganz normal zeichnen mit aktuellem Surfaceausschnitt und Colorkey
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderSprite(float x, float y, D3DCOLOR Color)
{
	float l,  r,  o,  u;					// Vertice Koordinaten
	float tl, tr, to, tu;					// Textur Koordinaten

	x = float (int (x));
	y = float (int (y));

	l = x;									// Links
	r = x+(itsRect.right-itsRect.left) - 1;	// Rechts
	o = y;									// Oben
	u = y+(itsRect.bottom-itsRect.top) - 1;	// Unten

	l -= 0.5f;
	r += 0.5f;
	o -= 0.5f;
	u += 0.5f;

	tl = itsRect.left  /itsXSize;	// Links
	tr = itsRect.right /itsXSize;	// Rechts
	to = itsRect.top   /itsYSize;	// Oben
	tu = itsRect.bottom/itsYSize;	// Unten

	TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;
	TriangleStrip[0].z	   = TriangleStrip[1].z		= TriangleStrip[2].z	 = TriangleStrip[3].z	  = 0.0f;

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

	//if (ActualTexture != (int)itsTexture)
	//{
		lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen
	//	ActualTexture = (int)itsTexture;
	//}

    // Sprite zeichnen	
	DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);	
    
	return true;
}

// --------------------------------------------------------------------------------------
// Sprite ganz normal zeichnen mit aktuellem Surfaceausschnitt und Colorkey, dabei
// alle vier Eckpunkte anders einfärben
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderSprite(float x, float y, int Anim, D3DCOLOR c1, D3DCOLOR c2, D3DCOLOR c3, D3DCOLOR c4)
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

	l -= 0.5f;
	r += 0.5f;
	o -= 0.5f;
	u += 0.5f;

	tl = itsRect.left  /itsXSize;	// Links
	tr = itsRect.right /itsXSize;	// Rechts
	to = itsRect.top   /itsYSize;	// Oben
	tu = itsRect.bottom/itsYSize;	// Unten

	TriangleStrip[0].color = c1;
	TriangleStrip[1].color = c2;
	TriangleStrip[2].color = c3;
	TriangleStrip[3].color = c4;
	TriangleStrip[0].z	   = TriangleStrip[1].z		= TriangleStrip[2].z	 = TriangleStrip[3].z	  = 0.0f;

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

	lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen

    // Sprite zeichnen	
	DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);		

	return true;
}

// --------------------------------------------------------------------------------------
// Sprite vertikal spiegelverkehrt zeichnen
// --------------------------------------------------------------------------------------
bool DirectGraphicsSprite::RenderMirroredSprite(float x, float y, D3DCOLOR Color, bool h, bool v)
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

	TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;
	TriangleStrip[0].z	   = TriangleStrip[1].z		= TriangleStrip[2].z     = TriangleStrip[3].z	  = 0.0f;

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

	lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen

    // Sprite zeichnen
	DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);
    
	return true;
}

// --------------------------------------------------------------------------------------
// Sprite spiegelverkehrt zeichnen mit aktuellem Surfaceausschnitt und Colorkey
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderMirroredSprite(float x, float y, D3DCOLOR Color)
{
	float l,  r,  o,  u;					// Vertice Koordinaten
	float tl, tr, to, tu;					// Textur Koordinaten

	x = float (int (x));
	y = float (int (y));

	r = x;									// Links
	l = x+(itsRect.right-itsRect.left) - 1;	// Rechts
	o = y;									// Oben
	u = y+(itsRect.bottom-itsRect.top) - 1;	// Unten

	l += 0.5f;
	r -= 0.5f;
	o -= 0.5f;
	u += 0.5f;

	tl = itsRect.left  /itsXSize;	// Links
	tr = itsRect.right /itsXSize;	// Rechts
	to = itsRect.top   /itsYSize;	// Oben
	tu = itsRect.bottom/itsYSize;	// Unten

	//tl = 0; tr = 0.5f; to = 0; tu = 0.5f;

	TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;
	TriangleStrip[0].z	   = TriangleStrip[1].z		= TriangleStrip[2].z     = TriangleStrip[3].z	  = 0.0f;

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

	lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen

    // Sprite zeichnen
	DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);
    
	return true;
}

// --------------------------------------------------------------------------------------
// Sprite mit bestimmter Animationsphase rendern
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderSprite(float x, float y, int Anim, D3DCOLOR Color)
{
	// Ausschnitt einstellen
	Anim %= 255;
	itsRect = itsPreCalcedRects [Anim];

	// Und Sprite rendern	
	RenderSprite(x, y, Color);	

	return true;
}

// --------------------------------------------------------------------------------------
// Sprite mit bestimmter Animationsphase rendern, wahlweise spiegelverkehrt
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderSprite(float x, float y, int Anim, D3DCOLOR Color, bool mirrored)
{
	// Ausschnitt einstellen
	Anim %= 255;
	itsRect = itsPreCalcedRects [Anim];

	// Und Sprite rendern
	if (mirrored == false)
		RenderSprite(x, y, Color);
	else
		RenderMirroredSprite(x, y, Color);

	return true;
}

// --------------------------------------------------------------------------------------
// Sprite skaliert zeichnen mit aktuellem Surfaceausschnitt (mit textur Filter)
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderSpriteScaled(float x, float y,int width, int height, D3DCOLOR col)										
{
	float l,  r,  o,  u;					// Vertice Koordinaten
	float tl, tr, to, tu;					// Textur Koordinaten

	l = x;			// Links
	r = x+width;	// Rechts
	o = y;			// Oben
	u = y+height;	// Unten

	l -= 0.5f;
	r += 0.5f;
	o -= 0.5f;
	u += 0.5f;

	tl = itsRect.left  /itsXSize;	// Links
	tr = itsRect.right /itsXSize;	// Rechts
	to = itsRect.top   /itsYSize;	// Oben
	tu = itsRect.bottom/itsYSize;	// Unten

	TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = col;
	TriangleStrip[0].z	   = TriangleStrip[1].z		= TriangleStrip[2].z	 = TriangleStrip[3].z	  = 0.0f;

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

	lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen

	DirectGraphics.SetFilterMode (true);

    // Sprite zeichnen
	DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);

	DirectGraphics.SetFilterMode (false);

	return true;
}

// --------------------------------------------------------------------------------------
// Sprite skaliert zeichnen mit angegebener Animationsphase (mit textur Filter)
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderSpriteScaled(float x, float y,int width, int height, int Anim, D3DCOLOR col)										
{
	float l,  r,  o,  u;					// Vertice Koordinaten
	float tl, tr, to, tu;					// Textur Koordinaten

	l = x;			// Links
	r = x+width;	// Rechts
	o = y;			// Oben
	u = y+height;	// Unten

	l -= 0.5f;
	r += 0.5f;
	o -= 0.5f;
	u += 0.5f;

	tl = ((Anim%itsXFrameCount) * itsXFrameSize)				 / itsXSize;	// Links
	tr = ((Anim%itsXFrameCount) * itsXFrameSize + itsXFrameSize) / itsXSize;	// Rechts
	to = ((Anim/itsXFrameCount) * itsYFrameSize)				 / itsYSize;	// Oben
	tu = ((Anim/itsXFrameCount) * itsYFrameSize + itsYFrameSize) / itsYSize;	// Unten

	TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = col;
	TriangleStrip[0].z	   = TriangleStrip[1].z		= TriangleStrip[2].z	 = TriangleStrip[3].z	  = 0.0f;

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

	lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen

	DirectGraphics.SetFilterMode (true);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);

	DirectGraphics.SetFilterMode (false);

	return true;
}

// --------------------------------------------------------------------------------------
// Sprite mit übergebenem Winkel rotiert darstellen
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderSpriteRotated(float x, float y, float Winkel, D3DCOLOR Color)
{
	float l,  r,  o,  u;					// Vertice Koordinaten
	float tl, tr, to, tu;					// Textur Koordinaten

	l = x;									// Links
	r = x+(itsRect.right-itsRect.left) - 1;	// Rechts
	o = y;									// Oben
	u = y+(itsRect.bottom-itsRect.top) - 1;	// Unten

	l -= 0.5f;
	r += 0.5f;
	o -= 0.5f;
	u += 0.5f;

	tl = itsRect.left  /itsXSize;	// Links
	tr = itsRect.right /itsXSize;	// Rechts
	to = itsRect.top   /itsYSize;	// Oben
	tu = itsRect.bottom/itsYSize;	// Unten

	TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;
	TriangleStrip[0].z     = TriangleStrip[1].z		= TriangleStrip[2].z	 = TriangleStrip[3].z	  = 0.0f;

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

	lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen

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
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	DirectGraphics.SetFilterMode (true);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);

	DirectGraphics.SetFilterMode (false);

	// Normale Projektions-Matrix wieder herstellen
	D3DXMatrixRotationZ (&matWorld, 0.0f);
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	return true;
}

// --------------------------------------------------------------------------------------
// Sprite mit übergebenem Winkel rotiert darstellen (Animationsphase)
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderSpriteRotated(float x, float y, float Winkel, int Anim, D3DCOLOR Color)
{
	// Ausschnitt einstellen
	Anim %= 255;
	itsRect = itsPreCalcedRects [Anim];

	// Und Sprite rendern
	RenderSpriteScaledRotated (x, y, 
							   float (itsRect.right  - itsRect.left), 
							   float (itsRect.bottom - itsRect.top), Winkel, Color);

	return true;
}

// --------------------------------------------------------------------------------------
// Sprite mit übergebenem Winkel rotiert darstellen
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderSpriteRotated(float x, float y, float Winkel, int Anim, D3DCOLOR Color, bool mirror)
{
	float l,  r,  o,  u;					// Vertice Koordinaten
	float tl, tr, to, tu;					// Textur Koordinaten

	// Ausschnitt einstellen
	Anim %= 255;
	itsRect = itsPreCalcedRects [Anim];

	// normal
	//
	if (mirror == false)
	{
		l = x-0.5f;									// Links
		r = x+(itsRect.right-itsRect.left-1)+0.5f;	// Rechts

		Winkel = 360 - Winkel;
	}

	// oder gespiegelt
	//
	else
	{
		r = x+0.5f;									// Links
		l = x+(itsRect.right-itsRect.left-1)-0.5f;	// Rechts		
	}

	o = y-0.5f;									// Oben
	u = y+(itsRect.bottom-itsRect.top-1)+0.5f;	// Unten

	to = itsRect.top   /itsYSize;	// Oben
	tu = itsRect.bottom/itsYSize;	// Unten
	tl = itsRect.left  /itsXSize;	// Links
	tr = itsRect.right /itsXSize;	// Rechts

	TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;
	TriangleStrip[0].z     = TriangleStrip[1].z		= TriangleStrip[2].z	 = TriangleStrip[3].z	  = 0.0f;

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

	lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen

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
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	DirectGraphics.SetFilterMode (true);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);

	DirectGraphics.SetFilterMode (false);

	// Normale Projektions-Matrix wieder herstellen
	D3DXMatrixRotationZ (&matWorld, 0.0f);
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	return true;
}

// --------------------------------------------------------------------------------------
// Sprite rotiert darstellen mit Verschiebung
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderSpriteRotatedOffset(float x, float y, float Winkel, float offx, float offy, D3DCOLOR Color, bool mirrored)
{
	float l,  r,  o,  u;					// Vertice Koordinaten
	float tl, tr, to, tu;					// Textur Koordinaten

//	itsRect = itsPreCalcedRects [0];

	l = x-0.5f;									// Links
	r = x+(itsRect.right-itsRect.left-1)+0.5f;	// Rechts

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
	u = y+(itsRect.bottom-itsRect.top-1)+0.5f;	// Unten

	to = itsRect.top   /itsYSize;	// Oben
	tu = itsRect.bottom/itsYSize;	// Unten
	tl = itsRect.left  /itsXSize;	// Links
	tr = itsRect.right /itsXSize;	// Rechts

	TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;
	TriangleStrip[0].z     = TriangleStrip[1].z		= TriangleStrip[2].z	 = TriangleStrip[3].z	  = 0.0f;

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

	lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen

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
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	DirectGraphics.SetFilterMode (true);

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);

	DirectGraphics.SetFilterMode (false);

	// Normale Projektions-Matrix wieder herstellen
	D3DXMatrixRotationZ (&matWorld, 0.0f);
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	return true;
}

// --------------------------------------------------------------------------------------
// Sprite mit übergebenem Winkel rotiert darstellen in beliebiger Grösse
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderSpriteScaledRotated(float x, float y, 
													 float width, float height,
													 float Winkel, D3DCOLOR Color)
{
	float l,  r,  o,  u;					// Vertice Koordinaten
	float tl, tr, to, tu;					// Textur Koordinaten

	l = x;			// Links
	r = x+width;	// Rechts
	o = y;			// Oben
	u = y+height;	// Unten

	l -= 0.5f;
	r += 0.5f;
	o -= 0.5f;
	u += 0.5f;

	tl = itsRect.left  /itsXSize;	// Links
	tr = itsRect.right /itsXSize;	// Rechts
	to = itsRect.top   /itsYSize;	// Oben
	tu = itsRect.bottom/itsYSize;	// Unten

	TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = Color;
	TriangleStrip[0].z	   = TriangleStrip[1].z		= TriangleStrip[2].z	 = TriangleStrip[3].z	  = 0.0f;

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

	lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen

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
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	DirectGraphics.SetFilterMode (true);	

    // Sprite zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);

	DirectGraphics.SetFilterMode (false);

	// Normale Projektions-Matrix wieder herstellen
	D3DXMatrixRotationZ (&matWorld, 0.0f);
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	return true;
}

// --------------------------------------------------------------------------------------
// Memberfunktionen zum Abfragen der Membervariablen
// --------------------------------------------------------------------------------------

float DirectGraphicsSprite::GetXSize(void)
{
	return itsXSize;
}

float DirectGraphicsSprite::GetYSize(void)
{
	return itsYSize;

}

int DirectGraphicsSprite::GetXFrameSize(void)
{
	return itsXFrameSize;
}

int DirectGraphicsSprite::GetYFrameSize(void)
{
	return itsYFrameSize;
}

int DirectGraphicsSprite::GetXFrameCount(void)
{
	return itsXFrameCount;
}

int DirectGraphicsSprite::GetYFrameCount(void)
{
	return itsYFrameCount;
}

// --------------------------------------------------------------------------------------
// Sonstige Funtkionen, die nicht innerhalb eines Sprite Objektes liegen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Rechteck enzeigen
// --------------------------------------------------------------------------------------

void RenderRect(float x, float y, float width, float height, D3DCOLOR Color)
{
	RenderRect4(x, y , width, height, Color, Color, Color, Color);
}

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

	l -= 0.5f;
	r += 0.5f;
	o -= 0.5f;
	u += 0.5f;

	TriangleStrip[0].z = TriangleStrip[1].z = TriangleStrip[2].z = TriangleStrip[3].z = 0.0f;

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

	lpD3DDevice->SetTexture (0, NULL);								// Textur setzen

    // Rechteck zeichnen
    DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);
}

// --------------------------------------------------------------------------------------
// Linie rendern
// --------------------------------------------------------------------------------------

void RenderLine(D3DXVECTOR2 p1, D3DXVECTOR2 p2,	D3DCOLOR Color)
{
	TriangleStrip[0].color = TriangleStrip[1].color = Color;
	TriangleStrip[0].z     = TriangleStrip[1].z	    = 0.0f;

	TriangleStrip[0].x		= p1.x;		// p1
	TriangleStrip[0].y		= p1.y;

	TriangleStrip[1].x		= p2.x;		// p2
	TriangleStrip[1].y		= p2.y;;

	lpD3DDevice->SetTexture (0, NULL);								// Textur setzen

    // Linie zeichnen
    lpD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST,1,&TriangleStrip[0],sizeof(VERTEX2D));
}

// --------------------------------------------------------------------------------------
// Linie rendern (zwei farbig)
// --------------------------------------------------------------------------------------

void RenderLine(D3DXVECTOR2 p1, D3DXVECTOR2 p2,	D3DCOLOR Color1, D3DCOLOR Color2)
{
	TriangleStrip[0].color = Color1;
	TriangleStrip[1].color = Color2;

	TriangleStrip[0].z     = TriangleStrip[1].z	    = 0.0f;

	TriangleStrip[0].x		= p1.x;		// p1
	TriangleStrip[0].y		= p1.y;

	TriangleStrip[1].x		= p2.x;		// p2
	TriangleStrip[1].y		= p2.y;;

	lpD3DDevice->SetTexture (0, NULL);								// Textur setzen

    // Linie zeichnen
    lpD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST,1,&TriangleStrip[0],sizeof(VERTEX2D));
}

// --------------------------------------------------------------------------------------
// Kreis an x/y zeichnen mit Radius r und Farbe col
// --------------------------------------------------------------------------------------

void RenderCircle(float x, float y, float r, D3DCOLOR col)
{
	#define step 0.2f

	D3DXVECTOR2 p1, p2;
	for (float i = 0; i < 2*PI; i += step)
	{
		p1.x = x + (float)sin(i) * r;
		p1.y = y + (float)cos(i) * r;

		p2.x = x + (float)sin(i + step) * r;
		p2.y = y + (float)cos(i + step) * r;

		RenderLine(p1, p2, col);
	}
}

// --------------------------------------------------------------------------------------
// Kollision zwischen zwei Sprites ermitteln (bisher nur Bounding-Box)
// --------------------------------------------------------------------------------------


bool  SpriteCollision(float x1, float y1, RECT rect1,
					  float x2, float y2, RECT rect2)
{
	// Kollision der Bounding-Boxes ?
	if (x1 + rect1.left < x2 + rect2.right   &&
		x2 + rect2.left < x1 + rect1.right   &&
		y1 + rect1.top  < y2 + rect2.bottom  &&
		y2 + rect2.top  < y1 + rect1.bottom)
		return true;

	return false;
}
