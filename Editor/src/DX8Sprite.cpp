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

#include <d3dx8.h>										// Für die Texturen
#include <d3dx8math.h>									// Für D3DXVECTOR2
#include "Globals.h"
#include "Logdatei.h"
#include "DX8Graphics.h"
#include "DX8Sprite.h"

// --------------------------------------------------------------------------------------
// Variablen
// --------------------------------------------------------------------------------------

RECT					ClippingArea;						// Ränder für das Clipping
VERTEX2D				v1, v2, v3, v4;						// Vertices zum Sprite rendern
VERTEX2D				TriangleStrip[4];					// Strip für ein Sprite
unsigned int			LoadedTextures = 0;					// Wieviele Texturen geladen ?

// --------------------------------------------------------------------------------------
// externe Variablen
// --------------------------------------------------------------------------------------

extern Logdatei					Protokoll;					// Protokoll Datei
extern LPDIRECT3DDEVICE8		lpD3DDevice;				// Direct3D Device-Objekt
extern D3DFORMAT				D3DFormat;					// Format der Primary Surface
extern LPDIRECT3DSURFACE8		lpBackbuffer;				// Der Backbuffer
extern D3DCAPS8					d3dcaps;					// Möglichkeiten der Hardware
extern LPDIRECT3DVERTEXBUFFER8	lpVBSprite;					// VertexBuffer für die Sprites
extern DirectGraphicsClass		DirectGraphics;				// DirectGraphics Klasse
extern D3DXMATRIX				matProj;					// Projektionsmatrix
extern D3DXMATRIX				matWorld;					// Weltmatrix
extern float					DegreetoRad[360];			// Tabelle mit Rotationswerten
extern char						HurriPath[255];				// Pfad, in dem Hurri liegt

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

bool DirectGraphicsSurface::LoadImage(char Filename[100], int xSize, int ySize)
{
	HRESULT hresult;
	char	Temp[140];

	// Surface erstellen
	hresult = lpD3DDevice->CreateImageSurface(xSize, ySize, D3DFormat, &itsSurface);

	// Fehler beim Surface erstellen ?
	if(hresult != D3D_OK)
	{
		strcpy(Temp, "Fehler beim Surface-Erstellen für ");
		strcat(Temp, Filename);
		strcat(Temp, " !");
		Protokoll.WriteText(Temp, true);
		return false;
	}

	// Bild laden
	hresult = D3DXLoadSurfaceFromFile(itsSurface, NULL, NULL, Filename, NULL, 
									  D3DX_FILTER_NONE, 0, NULL);

	// Fehler beim Laden ?
	if(hresult != D3D_OK)
	{
		strcpy(Temp, "Fehler beim Laden von ");
		strcat(Temp, Filename);
		strcat(Temp, " !\n");
		Protokoll.WriteText(Temp, true);
		return false;
	}
	
	// Grösse setzen
	itsRect.left   = 0;
	itsRect.top    = 0;
	itsRect.right  = xSize;
	itsRect.bottom = ySize;

	// Bild korrekt geladen
	strcpy(Temp, "Bitmap laden : ");
	strcat(Temp, Filename);
	strcat(Temp, " erfolgreich !\n");
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
		itsTexture->Release();
		itsTexture = NULL;
		LoadedTextures--;
//		Protokoll.WriteText("-> Sprite Textur erfolgreich freigegeben ! \n", false);

		_itoa(LoadedTextures, Buffer, 10);
//		Protokoll.WriteText(Buffer, false);
//		Protokoll.WriteText(" Sprite Textur(en) übrig !\n", false);
	}
}

// --------------------------------------------------------------------------------------
// Laden des Bildes "Filename" 
// xfs, yfs Grösse eines Frames
// xfc, yfc Anzahl der Frames
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::LoadImage(char Filename[100], int xs, int ys, int xfs, int yfs, 
														 int xfc,  int yfc)
{
	HRESULT hresult;
	char	Temp[140];
	char	Name[200];

	strcpy(Name, GFXPATH);
	strcat(Name, Filename);

	// Textur laden
	hresult = D3DXCreateTextureFromFileEx(
			  lpD3DDevice,
			  Name,
			  NULL, NULL,				  // x und y Grösse des Sprites (aus Datei übernehmen)
              1,                          // Nur eine Version der Textur
              0,                          // Immer 0 setzen
              D3DFMT_UNKNOWN,	          // Format aus der Datei lesen
              D3DPOOL_DEFAULT,            // DX bestimmt wo die Textur gespeichert wird 
              D3DX_FILTER_NONE,			  // Keine Filter verwenden
              D3DX_FILTER_NONE,
              0xFFFF00FF,                 // Colorkeyfarbe (Lila)
              NULL,						  // Keine Image Info
              NULL,						  // Keine Palette angeben
              &itsTexture);

	//hresult = D3DXCreateTextureFromFile(lpD3DDevice, Filename, &itsTexture);

	// Fehler beim Laden ?
	if(hresult != D3D_OK)
	{
		strcpy(Temp, "Fehler beim Laden von ");
		strcat(Temp, Filename);
		strcat(Temp, " !\n");
		Protokoll.WriteText(Temp, true);
		return false;
	}

	// Grösse der Textur anpassen
	D3DSURFACE_DESC desc;
	itsTexture->GetLevelDesc(0,&desc);
	
	xs = desc.Width;
	ys = desc.Height;

	// Grösse setzen
	itsXSize		= float(xs);
	itsYSize		= float(ys);
	itsXFrameCount	= xfc;
	itsYFrameCount	= yfc;
	itsXFrameSize	= xfs;
	itsYFrameSize	= yfs;
	itsRect.left	= 0;
	itsRect.top		= 0;
	itsRect.right	= xfs;
	itsRect.bottom	= yfs;

	// Bild korrekt geladen
	strcpy(Temp, "Bitmap laden : ");			// Logdatei Eintrag
	strcat(Temp, Filename);		
	strcat(Temp, " erfolgreich ! \n");
	Protokoll.WriteText(Temp, false);			
	LoadedTextures++;							// Anzahl der geladenen Texturen erhöhen	
	_itoa(LoadedTextures, Temp, 10);
//	Protokoll.WriteText(Temp, false);
//	Protokoll.WriteText(" Sprite Textur(en) geladen !\n", false);
	return true;
}

// --------------------------------------------------------------------------------------
// Laden des Bildes "Filename" 
// xfs, yfs Grösse eines Frames
// xfc, yfc Anzahl der Frames
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::LoadFromGfx(char Filename[100], int xs, int ys, int xfs, int yfs, 
														 int xfc,  int yfc)
{
	char	Temp[512];

	strcpy (Temp, HurriPath);
	strcat (Temp, "data/");
	strcat (Temp, Filename);
	LoadImage (Temp, xs, ys, xfs, yfs, xfc, yfc);
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
// Sprite ganz normal zeichnen mit aktuellem Surfaceausschnitt und Colorkey
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderSprite(float x, float y, D3DCOLOR Color)
{
	float l,  r,  o,  u;					// Vertice Koordinaten
	float tl, tr, to, tu;					// Textur Koordinaten

	l = x-0.5f;									// Links
	r = x+(itsRect.right-itsRect.left-1)+0.5f;	// Rechts
	o = y-0.5f;									// Oben
	u = y+(itsRect.bottom-itsRect.top-1)+0.5f;	// Unten

	tl = itsRect.left  /itsXSize;	// Links
	tr = itsRect.right /itsXSize;	// Rechts
	to = itsRect.top   /itsYSize;	// Oben
	tu = itsRect.bottom/itsYSize;	// Unten

	v1.color = v2.color = v3.color = v4.color = Color;
	v1.z	 = v2.z		= v3.z	   = v4.z	  = 0.0f;

	v1.x		= l;		// Links oben
	v1.y		= o;
	v1.tu		= tl;
	v1.tv		= to;

	v2.x		= r;		// Rechts oben
	v2.y		= o;
	v2.tu		= tr;
	v2.tv		= to;

	v3.x		= l;		// Links unten
	v3.y		= u;
	v3.tu		= tl;
	v3.tv		= tu;

	v4.x		= r;		// Rechts unten
	v4.y		= u;
	v4.tu		= tr;
	v4.tv		= tu;

	TriangleStrip[0] = v1;
	TriangleStrip[1] = v2;
	TriangleStrip[2] = v3;
	TriangleStrip[3] = v4;

	lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen

    // Sprite zeichnen
    lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,&TriangleStrip[0],sizeof(VERTEX2D));
	return true;
}

// --------------------------------------------------------------------------------------
// Sprite mit bestimmter Animationsphase rendern
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderSprite(float x, float y, int Anim, D3DCOLOR Color)
{
	RECT Rect;

	// Ausschnitt berechnen
	Rect.top	= (Anim/itsXFrameCount) * itsYFrameSize;
	Rect.left	= (Anim%itsXFrameCount) * itsXFrameSize;
	Rect.right  = Rect.left + itsXFrameSize;
	Rect.bottom = Rect.top  + itsYFrameSize;

	// Ausschnitt einstellen
	SetRect(Rect.left, Rect.top, Rect.right, Rect.bottom);

	// Und Sprite rendern
	RenderSprite(x, y, Color);

	return true;
}

// --------------------------------------------------------------------------------------
// Sprite skaliert zeichnen mit aktuellem Surfaceausschnitt (mit textur Filter)
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderSpriteScaled(float x, float y,float width, float height)										
{
	float l,  r,  o,  u;					// Vertice Koordinaten
	float tl, tr, to, tu;					// Textur Koordinaten

	l = x-320-0.5f;			// Links
	r = x+width-320+0.5f;	// Rechts
	o = 240-y+0.5f;			// Oben
	u = 240-y-height-0.5f;	// Unten

	tl = itsRect.left  /itsXSize;	// Links
	tr = itsRect.right /itsXSize;	// Rechts
	to = itsRect.top   /itsYSize;	// Oben
	tu = itsRect.bottom/itsYSize;	// Unten

	v1.color = v2.color = v3.color = v4.color = 0xFFFFFFFF;
	v1.z	 = v2.z		= v3.z	   = v4.z	  = 0.0f;

	v1.x		= l;		// Links oben
	v1.y		= o;
	v1.tu		= tl;
	v1.tv		= to;

	v2.x		= r;		// Rechts oben
	v2.y		= o;
	v2.tu		= tr;
	v2.tv		= to;

	v3.x		= l;		// Links unten
	v3.y		= u;
	v3.tu		= tl;
	v3.tv		= tu;

	v4.x		= r;		// Rechts unten
	v4.y		= u;
	v4.tu		= tr;
	v4.tv		= tu;

	TriangleStrip[0] = v1;
	TriangleStrip[1] = v2;
	TriangleStrip[2] = v3;
	TriangleStrip[3] = v4;

	lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen

	// Filter-Einstellungen
/*	lpD3DDevice->SetTextureStageState(0,   D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	lpD3DDevice->SetTextureStageState(0,   D3DTSS_MAGFILTER, D3DTEXF_LINEAR);*/
	
    // Sprite zeichnen
    lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,&TriangleStrip[0],sizeof(VERTEX2D));

	// und Filter wieder ausschalten
/*	lpD3DDevice->SetTextureStageState(0,   D3DTSS_MINFILTER, D3DTEXF_NONE);
	lpD3DDevice->SetTextureStageState(0,   D3DTSS_MAGFILTER, D3DTEXF_NONE);*/

	return true;
}

// --------------------------------------------------------------------------------------
// Sprite mit übergebenem Winkel rotiert darstellen
// --------------------------------------------------------------------------------------

bool DirectGraphicsSprite::RenderSpriteRotated(float x, float y, int Winkel, D3DCOLOR Color)
{
	float l,  r,  o,  u;					// Vertice Koordinaten
	float tl, tr, to, tu;					// Textur Koordinaten

	l = x-0.5f;									// Links
	r = x+(itsRect.right-itsRect.left-1)+0.5f;	// Rechts
	o = y-0.5f;									// Oben
	u = y+(itsRect.bottom-itsRect.top-1)+0.5f;	// Unten

	tl = itsRect.left  /itsXSize;	// Links
	tr = itsRect.right /itsXSize;	// Rechts
	to = itsRect.top   /itsYSize;	// Oben
	tu = itsRect.bottom/itsYSize;	// Unten

	v1.color = v2.color = v3.color = v4.color = Color;
	v1.z	 = v2.z		= v3.z	   = v4.z	  = 0.0f;

	v1.x		= l;		// Links oben
	v1.y		= o;
	v1.tu		= tl;
	v1.tv		= to;

	v2.x		= r;		// Rechts oben
	v2.y		= o;
	v2.tu		= tr;
	v2.tv		= to;

	v3.x		= l;		// Links unten
	v3.y		= u;
	v3.tu		= tl;
	v3.tv		= tu;

	v4.x		= r;		// Rechts unten
	v4.y		= u;
	v4.tu		= tr;
	v4.tv		= tu;

	TriangleStrip[0] = v1;
	TriangleStrip[1] = v2;
	TriangleStrip[2] = v3;
	TriangleStrip[3] = v4;

	lpD3DDevice->SetTexture (0, itsTexture);							// Textur setzen

//----- Sprite rotieren

	D3DXMATRIX	matRot, matTrans, matTrans2;

	// Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
	if (Winkel > 360) Winkel -= 360;
	if (Winkel < 0)	  Winkel += 360;

	// Rotationsmatrix
	D3DXMatrixRotationZ  (&matRot, DegreetoRad[Winkel]);

	// Transformation zum Ursprung
	D3DXMatrixTranslation(&matTrans,-x-(itsRect.right  - itsRect.left)/2,
									-y-(itsRect.bottom - itsRect.top) /2, 0.0f);

	// Transformation wieder zurück
	D3DXMatrixTranslation(&matTrans2,x+(itsRect.right  - itsRect.left)/2,
									 y+(itsRect.bottom - itsRect.top) /2, 0.0f);

	// Verschieben und rotieren
	D3DXMatrixMultiply	 (&matWorld, &matTrans, &matRot);	

	// und wieder zurück verschieben
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans2);
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// Filter einstellen
	lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);

    // Sprite zeichnen
    lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,&TriangleStrip[0],sizeof(VERTEX2D));

	// und Filter wieder ausschalten
	lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);

	// Normale Projektions-Matrix wieder herstellen
	D3DXMatrixRotationZ (&matWorld, 0.0f);
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	return true;
}

// --------------------------------------------------------------------------------------
// Sonstige Funtkionen, die nicht innerhalb eines Sprite Objektes liegen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Rechteck enzeigen
// --------------------------------------------------------------------------------------

void RenderRect(float x, float y, float width, float height, D3DCOLOR Color)
{
	float l,  r,  o,  u;					// Vertice Koordinaten

	l = x-0.5f;			// Links
	r = x+width+0.5f;	// Rechts
	o = y-0.5f;			// Oben
	u = y+height+0.5f;	// Unten

	v1.color = v2.color = v3.color = v4.color = Color;
	v1.z	 = v2.z		= v3.z	   = v4.z	  = 0.0f;

	v1.x		= l;		// Links oben
	v1.y		= o;

	v2.x		= r;		// Rechts oben
	v2.y		= o;

	v3.x		= l;		// Links unten
	v3.y		= u;

	v4.x		= r;		// Rechts unten
	v4.y		= u;

	TriangleStrip[0] = v1;
	TriangleStrip[1] = v2;
	TriangleStrip[2] = v3;
	TriangleStrip[3] = v4;

	lpD3DDevice->SetTexture (0, NULL);								// Textur setzen

    // Rechteck zeichnen
    lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,&TriangleStrip[0],sizeof(VERTEX2D));
}