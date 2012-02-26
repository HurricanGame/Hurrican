// Datei : Tileengine.cpp

// -------------------------------------------------------------------------------------- 
//
// 2D Tile-Engine für Hurrican
// bestehend aus einem Vordergrund-Layer in verschiedenen Helligkeitsstufen
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include <stdio.h>
#include "Tileengine.h"
#include "DX8Graphics.h"
#include "Logdatei.h"

// --------------------------------------------------------------------------------------
// externe Variablen
// --------------------------------------------------------------------------------------

extern LPDIRECT3DDEVICE8	lpD3DDevice;				// Direct3D Device-Objekt
extern Logdatei				Protokoll;
extern DirectGraphicsClass  DirectGraphics;
extern bool					bCopyLight;

float				SpeedFaktor = 1.0f;					// Geschwindigkeits Faktor

// --------------------------------------------------------------------------------------
// Klassendefunktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// TileEngine Klasse
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

TileEngineClass::TileEngineClass(void)
{
	XOffset = 0;
	YOffset = 0;

	LEVELSIZE_X = 128;
	LEVELSIZE_Y = 96;

	ScrollSpeedX  = 0.0f;
	ScrollSpeedY  = 0.0f;
	CloudMovement = 0.0f;
	TileAnimCount = 0.0f;
	TileAnimPhase = 0;

	for(int i=0; i<MAX_LEVELSIZE_X; i++)
	 for(int j=0; j<MAX_LEVELSIZE_Y; j++)
	 {
		 Tiles[i][j].Block		  = 0;
		 Tiles[i][j].TileSetBack  = 0;
		 Tiles[i][j].TileSetFront = 0;
		 Tiles[i][j].BackArt	  = 0;
		 Tiles[i][j].FrontArt	  = 0;
		 Tiles[i][j].Red		  = 255;
		 Tiles[i][j].Green		  = 255;
		 Tiles[i][j].Blue		  = 255;
		 Tiles[i][j].Alpha		  = 255;
	 }

	 for(i=0; i<MAX_TILESETS; i++)
		 TileGfx[i].itsTexture = NULL;
}

// --------------------------------------------------------------------------------------
// Destruktor
// --------------------------------------------------------------------------------------

TileEngineClass::~TileEngineClass(void)
{
	// Grafiken freigeben
	Background.~DirectGraphicsSprite();

	for (int i=0; i<LoadedTilesets; i++)
		TileGfx[i].~DirectGraphicsSprite();
}

// --------------------------------------------------------------------------------------
// Neues, leeres Level der Grösse xSize/ySize erstellen
// --------------------------------------------------------------------------------------

void TileEngineClass::InitNewLevel(int xSize, int ySize)
{
	LEVELSIZE_X = xSize;
	LEVELSIZE_Y = ySize;

	ScrollSpeedX = 0.0f;
	ScrollSpeedY = 0.0f;

	for(int i=0; i<MAX_LEVELSIZE_X; i++)
	 for(int j=0; j<MAX_LEVELSIZE_Y; j++)
	 {
		 Tiles[i][j].Block			= 0;
		 Tiles[i][j].TileSetBack	= 0;
		 Tiles[i][j].TileSetFront	= 0;
		 Tiles[i][j].FrontArt		= 0;
		 Tiles[i][j].BackArt		= 0;
		 Tiles[i][j].Red			= 255;
		 Tiles[i][j].Green			= 255;
		 Tiles[i][j].Blue			= 255;
		 Tiles[i][j].Alpha			= 255;
	 }
}

// --------------------------------------------------------------------------------------
// Level laden
// --------------------------------------------------------------------------------------

bool TileEngineClass::LoadLevel(char Filename[100])
{
/*	FileHeader				DateiHeader;					// Header der Level-Datei
	FILE					*Datei = NULL;					// Level-Datei
	LevelObjectStruct		LoadObject;

	// Prüfen, ob die Datei existiert
	Datei = fopen(Filename, "rb");

	if(!Datei)
	{
		Protokoll.WriteText("\n-> Fehler beim Level laden !", true);
		return false;
	}

	// DateiHeader auslesen
	fread(&DateiHeader, sizeof(DateiHeader), 1, Datei);

	// und Werte übertragen
	LEVELSIZE_X		 = DateiHeader.SizeX;
	LEVELSIZE_Y		 = DateiHeader.SizeY;
	LEVELPIXELSIZE_X = LEVELSIZE_X*20;
	LEVELPIXELSIZE_Y = LEVELSIZE_Y*20;
	LoadedTilesets   = DateiHeader.UsedTilesets;
	strcpy(Beschreibung, DateiHeader.Beschreibung);

	// Benutzte Texturen laden
	for (unsigned int i=0; i<LoadedTilesets; i++)
		TileGfx[i].LoadImage(DateiHeader.SetNames[i], 256, 256, 20, 20, 12, 12);

	// LevelDaten laden
	InitNewLevel(LEVELSIZE_X, LEVELSIZE_Y);

	for(i=0; i<LEVELSIZE_X; i++)
	 for(unsigned int j=0; j<LEVELSIZE_Y; j++) 
	 {
		 fread(&Tiles[i][j], sizeof(Tiles[i][j]), 1, Datei);

		// TileSets angleichen, da diese im Editor um eins erhöht
		// gespeichert werden, da TileSet[0] == kein TileSet im Editor

//		if(Tiles[i][j].TileSetBack  > 0) Tiles[i][j].TileSetBack--;
//		if(Tiles[i][j].TileSetFront > 0) Tiles[i][j].TileSetFront =1;
	 }

	// Objekt Daten laden und gleich Liste mit Objekten erstellen
	if (DateiHeader.NumObjects > 0)
	for(i=0; i < DateiHeader.NumObjects; i++)
	{
		fread(&LoadObject, sizeof(LoadObject), 1, Datei);				// Objekt laden

		if(LoadObject.ObjectID == 0)
		{
//			pPlayer->xpos = (float)(LoadObject.XPos);
//			pPlayer->ypos = (float)(LoadObject.YPos);
		}
		//pObjectList->Push(LoadObject);								// und in die Liste setzen
	}

	// Datei schliessen
	fclose(Datei);

	// Level korrekt geladen
	char Temp[100];
	strcpy(Temp, "Level laden : ");
	strcat(Temp, Filename);
	strcat(Temp, " erfolgreich !\n");
	Protokoll.WriteText(Temp, false);
*/
	return true;
}

// --------------------------------------------------------------------------------------
// Neue Scrollspeed setzen
// --------------------------------------------------------------------------------------

void TileEngineClass::SetScrollSpeed(float xSpeed, float ySpeed)
{
	ScrollSpeedX = xSpeed;
	ScrollSpeedY = ySpeed;
}

// --------------------------------------------------------------------------------------
// Hintergrund Parallax Layer anzeigen
// --------------------------------------------------------------------------------------

void TileEngineClass::DrawBackground(void)
{
	int		xoff;
	float	yoff;

//----- Hintergrund-Bild

	xoff = (int)(XOffset / 5) % 640;

	if (bScrollBackground == true)				// Hintergrundbild mitscrollen
	{
		// Linke Hälfte
		Background.SetRect(0, 0, xoff, 480);
		Background.RenderSprite(640-(float)(xoff), 0, 0xFFFFFFFF);

		// Rechte Hälfte
		Background.SetRect(xoff, 0, 640, 480);
		Background.RenderSprite(0, 0, 0xFFFFFFFF);
	}
	else										// oder statisch ?
	{
		Background.SetRect(0, 0, 640, 480);
		Background.RenderSprite(0, 0, 0xFFFFFFFF);
	}

//----- Layer ganz hinten (ausser im Flugsack Level)


	xoff = (int)(XOffset / 3) % 640;
	yoff = (float)((LEVELSIZE_Y-SCREENSIZE_Y)*20);	// Grösse des Levels in Pixeln (-1 Screen)
	yoff = (float)(220-150/yoff*YOffset);			// y-Offset des Layers berechnen
	yoff -= 40;

	// Linke Hälfte
	ParallaxLayer[0].SetRect(0, 0, xoff, 480);
	ParallaxLayer[0].RenderSprite(640-(float)(xoff), yoff, 0xFFFFFFFF);

	// Rechte Hälfte
	ParallaxLayer[0].SetRect(xoff, 0, 640, 480);
	ParallaxLayer[0].RenderSprite(0, yoff, 0xFFFFFFFF);


//----- vorletzter Layer 

	yoff = (float)((LEVELSIZE_Y-SCREENSIZE_Y)*20);	// Grösse des Levels in Pixeln (-1 Screen)
	yoff = (float)(200-200/yoff*YOffset);			// y-Offset des Layers berechnen
	xoff = (int)(XOffset / 2) % 640;

	// Linke Hälfte
	ParallaxLayer[1].SetRect(0, 0, xoff, 480);
	ParallaxLayer[1].RenderSprite(640-(float)(xoff), yoff, 0xFFFFFFFF);

	// Rechte Hälfte
	ParallaxLayer[1].SetRect(xoff, 0, 640, 480);
	ParallaxLayer[1].RenderSprite(0, yoff, 0xFFFFFFFF);


//----- Wolken Layer (Wenn Focus des Level GANZ oben, dann wird er GANZ angezeigt)


	// Wolken bewegen
	//CloudMovement += SpeedFaktor;
	//if (CloudMovement > 640.0f)
	//	CloudMovement = 0.0f;

	DirectGraphics.SetAdditiveMode();

	xoff = int(XOffset / 4 + CloudMovement) % 640;
	yoff = float((LEVELSIZE_Y-SCREENSIZE_Y)*20);	// Grösse des Levels in Pixeln (-1 Screen)
	yoff = float(320/yoff*YOffset);					// y-Offset des Layers berechnen

	// Linke Hälfte
	CloudLayer.SetRect(0, int(yoff), int(xoff), 240);
	CloudLayer.RenderSprite(640-(float)(xoff), 0, 0xFFFFFFFF);

	// Rechte Hälfte
	CloudLayer.SetRect(int(xoff), int(yoff), 640, 240);
	CloudLayer.RenderSprite(0, 0, 0xFFFFFFFF);

	DirectGraphics.SetColorKeyMode();

}

// --------------------------------------------------------------------------------------
// Level Hintergrund anzeigen
// --------------------------------------------------------------------------------------

void TileEngineClass::DrawBackLevel(void)
{
	RECT			Rect;						// Texturausschnitt im Tileset
	long			xLevel,	yLevel;				// Position im Level-Array
	int				xTileOffs, yTileOffs;		// Tile-Verschiebung am Screen (0-19)
	int				NumToRender;				// Wieviele Vertices zu rendern ?
	int				ActualTexture;				// Aktuelle Textur
	float			xScreen, yScreen;			// Wo sind wir grad am Screen ? (damit nich immer i*20 j*20)
	float			l,  r,  o,  u;				// Vertice Koordinaten
	float			tl, tr, to, tu;				// Textur Koordinaten
	char			red,green,blue,alpha;		// Komponenten des Tiles
	D3DCOLOR		Color;						// Farbe des Tiles
	unsigned int	Type;						// TileNR des Tiles

	// Am Anfang noch keine Textur gewählt
	ActualTexture = -1;

	// Position im Level Array errechnen
	xLevel = (long)(XOffset / TILESIZE_X);
	yLevel = (long)(YOffset / TILESIZE_Y);

	// Offsets der Tiles berechnen (0-19)
	xTileOffs = (int)(XOffset) % TILESIZE_X;
	yTileOffs = (int)(YOffset) % TILESIZE_Y;

	xScreen = (float)(-xTileOffs);
	yScreen = (float)(-yTileOffs);

	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);			// Alpha-Blending
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// für Colorkey
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// einschalten

	// Noch keine Tiles zum rendern
	NumToRender = 0;

	for(int j=0; j<SCREENSIZE_Y + 1; j++)
	{
		xScreen = (float)(-xTileOffs);								// Anfangs Position

		for(int i=0; i<SCREENSIZE_X + 1; i++)
		{
			if (Tiles[xLevel+i][yLevel+j].BackArt > 0)
			{
				// Neue Textur ?
				if (Tiles[xLevel+i][yLevel+j].TileSetBack != ActualTexture)
				{
					// Aktuelle Textur sichern
					ActualTexture = Tiles[xLevel+i][yLevel+j].TileSetBack;

					// Tiles zeichnen
					if (NumToRender > 0)
						lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
													 NumToRender*2,
													 &TilesToRender[0],sizeof(VERTEX2D));

					// Neue aktuelle Textur setzen
					lpD3DDevice->SetTexture (0, TileGfx[ActualTexture].itsTexture);


					// Und beim rendern wieder von vorne anfangen
					NumToRender = 0;
				}

				red   = Tiles[xLevel+i][yLevel+j].Red;	// Farbe zusammensetzen
				green = Tiles[xLevel+i][yLevel+j].Green;
				blue  = Tiles[xLevel+i][yLevel+j].Blue;
				alpha = Tiles[xLevel+i][yLevel+j].Alpha;

				if (bCopyLight)
					Color = D3DCOLOR_RGBA(red, green, blue, alpha);
				else
					Color = 0xFFFFFFFF;

				Type = Tiles[xLevel+i][yLevel+j].BackArt - INCLUDE_ZEROTILE;

				// Animiertes Tile ?
				if (Tiles[xLevel+i][yLevel+j].Block == ANIMATED_WALL)
					Type += 36*TileAnimPhase;
			
				// richtigen Ausschnitt für das aktuelle Tile berechnen
				Rect.top	= (Type/12) * 20;
				Rect.left	= (Type%12) * 20;
				Rect.right  = Rect.left + 20;
				Rect.bottom = Rect.top  + 20;
	
				// Screen-Koordinaten der Vertices
				l = xScreen   -0.5f;						// Links
				o = yScreen   -0.5f;						// Oben
				r = xScreen+19+0.5f;						// Rechts
				u = yScreen+19+0.5f;						// Unten

				// Textur-Koordinaten
				tl = Rect.left  /TILESETSIZE_X;				// Links
				tr = Rect.right /TILESETSIZE_X;				// Rechts
				to = Rect.top   /TILESETSIZE_Y;				// Oben
				tu = Rect.bottom/TILESETSIZE_Y;				// Unten
	
				// Vertices definieren
				v1.z	 = v2.z		= v3.z	   = v4.z	  = 0.0f;
				v1.color = v2.color = v3.color = v4.color = Color;

				v1.x		= l;							// Links oben
				v1.y		= o;
				v1.tu		= tl;
				v1.tv		= to;

				v2.x		= r;							// Rechts oben
				v2.y		= o;
				v2.tu		= tr;
				v2.tv		= to;

				v3.x		= l;							// Links unten
				v3.y		= u;
				v3.tu		= tl;
				v3.tv		= tu;

				v4.x		= r;							// Rechts unten
				v4.y		= u;
				v4.tu		= tr;
				v4.tv		= tu;

				// Zu rendernde Vertices ins Array schreiben
				TilesToRender[NumToRender*6]   = v1;	// Jeweils 2 Dreicke als
				TilesToRender[NumToRender*6+1] = v2;	// als ein viereckiges
				TilesToRender[NumToRender*6+2] = v3;	// Tile ins Array kopieren
				TilesToRender[NumToRender*6+3] = v3;	
				TilesToRender[NumToRender*6+4] = v2;	
				TilesToRender[NumToRender*6+5] = v4;	

				NumToRender++;				// Weiter im Vertex Array

			}
			xScreen += TILESIZE_X;		// Am Screen weiter
		}
		yScreen += TILESIZE_Y;			// Am Screen weiter
	}

	if (NumToRender > 0)
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, NumToRender*2, &TilesToRender[0],sizeof(VERTEX2D));


/*	RECT			Rect;						// Texturausschnitt im Tileset
	long			xLevel,	yLevel;				// Position im Level-Array
	int				xTileOffs, yTileOffs;		// Tile-Verschiebung am Screen (0-19)
	int				NumToRender;				// Wieviele Vertices zu rendern ?
	float			xScreen, yScreen;			// Wo sind wir grad am Screen ? (damit nich immer i*20 j*20)
	float			l,  r,  o,  u;				// Vertice Koordinaten
	float			tl, tr, to, tu;				// Textur Koordinaten
	char			red,green,blue;				// Komponenten des Tiles
	D3DCOLOR		Color;						// Farbe des Tiles
	unsigned int	Type;						// TileNR des Tiles

	// Position im Level Array errechnen
	xLevel = (long)(XOffset / TILESIZE_X);
	yLevel = (long)(YOffset / TILESIZE_Y);

	// Offsets der Tiles berechnen (0-19)
	xTileOffs = (int)(XOffset) % TILESIZE_X;
	yTileOffs = (int)(YOffset) % TILESIZE_Y;

	xScreen = (float)(-xTileOffs);
	yScreen = (float)(-yTileOffs);

	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);			// Alpha-Blending
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// für Colorkey
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// einschalten

	// Alle möglichen Tilesets durchmachen
	for(unsigned int k=0; k<LoadedTilesets; k++)
	{
		NumToRender = 0;

		// Textur setzen
		lpD3DDevice->SetTexture (0, TileGfx[k].itsTexture);

		for(int j=0; j<SCREENSIZE_Y + 1; j++)
		{
			xScreen = (float)(-xTileOffs);								// Anfangs Position

			for(int i=0; i<SCREENSIZE_X + 1; i++)
			{
				if (Tiles[xLevel+i][yLevel+j].TileSetBack == k &&
					Tiles[xLevel+i][yLevel+j].BackArt > 0)
				{
					red   = Tiles[xLevel+i][yLevel+j].Red;	// Farbe zusammensetzen
					green = Tiles[xLevel+i][yLevel+j].Green;
					blue  = Tiles[xLevel+i][yLevel+j].Blue;
					Color = D3DCOLOR_RGBA(red, green, blue, 255);

					Type = Tiles[xLevel+i][yLevel+j].BackArt - INCLUDE_ZEROTILE;

					// Animiertes Tile ?
					if (Tiles[xLevel+i][yLevel+j].Block == ANIMATED_WALL)
						Type += 36*TileAnimPhase;
			
					// richtigen Ausschnitt für das aktuelle Tile berechnen
					Rect.top	= (Type/12) * 20;
					Rect.left	= (Type%12) * 20;
					Rect.right  = Rect.left + 20;
					Rect.bottom = Rect.top  + 20;
	
					// Screen-Koordinaten der Vertices
					l = xScreen   -0.5f;						// Links
					o = yScreen   -0.5f;						// Oben
					r = xScreen+19+0.5f;						// Rechts
					u = yScreen+19+0.5f;						// Unten

					// Textur-Koordinaten
					tl = Rect.left  /TILESETSIZE_X;				// Links
					tr = Rect.right /TILESETSIZE_X;				// Rechts
					to = Rect.top   /TILESETSIZE_Y;				// Oben
					tu = Rect.bottom/TILESETSIZE_Y;				// Unten
	
					// Vertices definieren

					v1.z	 = v2.z		= v3.z	   = v4.z	  = 0.0f;
					v1.color = v2.color = v3.color = v4.color = Color;

					v1.x		= l;							// Links oben
					v1.y		= o;
					v1.tu		= tl;
					v1.tv		= to;

					v2.x		= r;							// Rechts oben
					v2.y		= o;
					v2.tu		= tr;
					v2.tv		= to;

					v3.x		= l;							// Links unten
					v3.y		= u;
					v3.tu		= tl;
					v3.tv		= tu;

					v4.x		= r;							// Rechts unten
					v4.y		= u;
					v4.tu		= tr;
					v4.tv		= tu;

					// Zu rendernde Vertices ins Array schreiben
					TilesToRender[NumToRender*6]   = v1;	// Jeweils 2 Dreicke als
					TilesToRender[NumToRender*6+1] = v2;	// als ein viereckiges
					TilesToRender[NumToRender*6+2] = v3;	// Tile ins Array kopieren
					TilesToRender[NumToRender*6+3] = v3;	
					TilesToRender[NumToRender*6+4] = v2;	
					TilesToRender[NumToRender*6+5] = v4;	

					NumToRender++;				// Weiter im Vertex Array
				}
					xScreen += TILESIZE_X;		// Am Screen weiter
			}
			yScreen += TILESIZE_Y;			// Am Screen weiter
		}

		// Tiles zeichnen
		lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST,NumToRender*2,&TilesToRender[0],sizeof(VERTEX2D));
	}*/
}

// --------------------------------------------------------------------------------------
// Level Vodergrund anzeigen
// --------------------------------------------------------------------------------------

void TileEngineClass::DrawFrontLevel(void)
{
	RECT			Rect;						// Texturausschnitt im Tileset
	long			xLevel,	yLevel;				// Position im Level-Array
	int				xTileOffs, yTileOffs;		// Tile-Verschiebung am Screen (0-19)
	int				NumToRender;				// Wieviele Vertices zu rendern ?
	float			xScreen, yScreen;			// Wo sind wir grad am Screen ? (damit nich immer i*20 j*20)
	float			l,  r,  o,  u;				// Vertice Koordinaten
	float			tl, tr, to, tu;				// Textur Koordinaten
	char			red,green,blue;				// Komponenten des Tiles
	D3DCOLOR		Color;						// Farbe des Tiles
	unsigned int	Type;						// TileNR des Tiles

	// Position im Level Array errechnen
	xLevel = (long)(XOffset / TILESIZE_X);
	yLevel = (long)(YOffset / TILESIZE_Y);

	// Offsets der Tiles berechnen (0-19)
	xTileOffs = (int)(XOffset) % TILESIZE_X;
	yTileOffs = (int)(YOffset) % TILESIZE_Y;

	xScreen = (float)(-xTileOffs);
	yScreen = (float)(-yTileOffs);

	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);			// Alpha-Blending
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// für Colorkey
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// einschalten

	// Alle möglichen Tilesets durchmachen
	for(unsigned int k=0; k<LoadedTilesets; k++)
	{
		NumToRender = 0;

		// Textur setzen
		lpD3DDevice->SetTexture (0, TileGfx[k].itsTexture);

		for(int j=0; j<SCREENSIZE_Y + 1; j++)
		{
			xScreen = (float)(-xTileOffs);								// Anfangs Position

			for(int i=0; i<SCREENSIZE_X + 1; i++)
			{
				if(Tiles[xLevel+i][yLevel+j].TileSetFront == k &&
				   Tiles[xLevel+i][yLevel+j].Block != OVERLAY &&
				   Tiles[xLevel+i][yLevel+j].FrontArt > 0)
				{
					red   = Tiles[xLevel+i][yLevel+j].Red;	// Farbe zusammensetzen
					green = Tiles[xLevel+i][yLevel+j].Green;
					blue  = Tiles[xLevel+i][yLevel+j].Blue;

					if (bCopyLight)
						Color = D3DCOLOR_RGBA(red, green, blue, 255);
					else
						Color = 0xFFFFFFFF;

					Type = Tiles[xLevel+i][yLevel+j].FrontArt - INCLUDE_ZEROTILE;
			
					// richtigen Ausschnitt für das aktuelle Tile berechnen
					Rect.top	= (Type/12) * 20;
					Rect.left	= (Type%12) * 20;
					Rect.right  = Rect.left + 20;
					Rect.bottom = Rect.top  + 20;
	
					// Screen-Koordinaten der Vertices
					l = xScreen   -0.5f;						// Links
					o = yScreen   -0.5f;						// Oben
					r = xScreen+19+0.5f;						// Rechts
					u = yScreen+19+0.5f;						// Unten

					// Textur-Koordinaten
					tl = Rect.left  /TILESETSIZE_X;				// Links
					tr = Rect.right /TILESETSIZE_X;				// Rechts
					to = Rect.top   /TILESETSIZE_Y;				// Oben
					tu = Rect.bottom/TILESETSIZE_Y;				// Unten
	
					// Vertices definieren

					v1.z	 = v2.z		= v3.z	   = v4.z	  = 0.0f;
					v1.color = v2.color = v3.color = v4.color = Color;

					v1.x		= l;							// Links oben
					v1.y		= o;
					v1.tu		= tl;
					v1.tv		= to;

					v2.x		= r;							// Rechts oben
					v2.y		= o;
					v2.tu		= tr;
					v2.tv		= to;

					v3.x		= l;							// Links unten
					v3.y		= u;
					v3.tu		= tl;
					v3.tv		= tu;

					v4.x		= r;							// Rechts unten
					v4.y		= u;
					v4.tu		= tr;
					v4.tv		= tu;

					// Zu rendernde Vertices ins Array schreiben
					TilesToRender[NumToRender*6]   = v1;	// Jeweils 2 Dreicke als
					TilesToRender[NumToRender*6+1] = v2;	// als ein viereckiges
					TilesToRender[NumToRender*6+2] = v3;	// Tile ins Array kopieren
					TilesToRender[NumToRender*6+3] = v3;	
					TilesToRender[NumToRender*6+4] = v2;	
					TilesToRender[NumToRender*6+5] = v4;	

					NumToRender++;				// Weiter im Vertex Array
				}
					xScreen += TILESIZE_X;		// Am Screen weiter
			}
			yScreen += TILESIZE_Y;			// Am Screen weiter
		}

		// Tiles zeichnen
		lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST,NumToRender*2,&TilesToRender[0],sizeof(VERTEX2D));
	}
}

// --------------------------------------------------------------------------------------
// Die Levelstücke zeigen, die den Spieler verdecken
// --------------------------------------------------------------------------------------

void TileEngineClass::DrawOverlayLevel(void)
{
	long			xLevel,	yLevel;				// Position im Level-Array
	int				xTileOffs, yTileOffs;		// Tile-Verschiebung am Screen (0-19)
	int				Type;
	float			xScreen, yScreen;			// Wo sind wir grad am Screen ? (damit nich immer i*20 j*20)
	D3DCOLOR		Col;

	// Position im Level Array errechnen
	xLevel = (long)(XOffset / TILESIZE_X);
	yLevel = (long)(YOffset / TILESIZE_Y);

	// Offsets der Tiles berechnen (0-19)
	xTileOffs = (int)(XOffset) % TILESIZE_X;
	yTileOffs = (int)(YOffset) % TILESIZE_Y;

	xScreen = (float)(-xTileOffs);
	yScreen = (float)(-yTileOffs);

	for(int i=0; i<33; i++)
		for (int j=0; j<25; j++)
			if(Tiles[i+xLevel][j+yLevel].FrontArt > 0 &&
			  ((Tiles[i+xLevel][j+yLevel].Block & OVERLAY) ||
			   (Tiles[i+xLevel][j+yLevel].Block & ANIMATED_OVERLAY)))
			    
			{
				if (bCopyLight)
					Col = D3DCOLOR_RGBA(Tiles[i+xLevel][j+yLevel].Red,
										Tiles[i+xLevel][j+yLevel].Green,
										Tiles[i+xLevel][j+yLevel].Blue,
										Tiles[i+xLevel][j+yLevel].Alpha);
				else
					Col = 0xFFFFFFFF;

				Type = Tiles[i+xLevel][j+yLevel].FrontArt-INCLUDE_ZEROTILE;

				// Animiertes Overlay Tile ?
				if(Tiles[i+xLevel][j+yLevel].Block == ANIMATED_OVERLAY)
					Type += 36*TileAnimPhase;

				TileGfx[Tiles[i+xLevel][j+yLevel].TileSetFront].RenderSprite((float)(i*20-xTileOffs), 
																			 (float)(j*20-yTileOffs), 
																			 Type, Col);
			}
}

// --------------------------------------------------------------------------------------
// Level scrollen und Tiles animieren usw
// --------------------------------------------------------------------------------------

void TileEngineClass::UpdateLevel(void)
{
	// Grenzen des Levels checken
	if (XOffset < 0.0f)
		XOffset = 0.0f;

	if (YOffset < 0.0f)
		YOffset = 0.0f;

	if (XOffset > LEVELPIXELSIZE_X - 640)
		XOffset = LEVELPIXELSIZE_X - 640;

	if (YOffset > LEVELPIXELSIZE_Y - 480)
		YOffset = LEVELPIXELSIZE_Y - 480;

	XOffset += ScrollSpeedX * SpeedFaktor;
	YOffset += ScrollSpeedY * SpeedFaktor;

	// Tiles animieren
	TileAnimCount += SpeedFaktor;				// Counter erhöhen
	if (TileAnimCount > TILEANIM_SPEED)			// auf Maximum prüfen
	{
		TileAnimCount = 0.0f;					// Counter wieder auf 0 setzen
		TileAnimPhase++;						// und nächste Animphase setzen
		if (TileAnimPhase >=4)					// Animation wieer von vorne ?
			TileAnimPhase = 0;
	}
}

// --------------------------------------------------------------------------------------
// Zurückliefern, welche BlockArt sich Rechts vom übergebenen Rect befindet
// --------------------------------------------------------------------------------------

int	TileEngineClass::BlockRechts(float &x, float &y, RECT rect)
{
	int			 Art	= 0;
	unsigned int xlevel = (unsigned int)((x+rect.right+1) / TILESIZE_X);
	unsigned int ylevel;

	for(int i=rect.top; i<rect.bottom; i++)
	{
		ylevel = (unsigned int)((y+i) / TILESIZE_Y);

		if (Tiles[xlevel][ylevel].Block > 0 && Art < WALL)
		{
			Art = Tiles[xlevel][ylevel].Block;
			if (Art >= WALL)									// x-Position begradigen ?
				x = (float)(xlevel*TILESIZE_X-rect.right-1);	// wegen Fehlern bei kleiner fps
		}
	}

	return Art;
}

// --------------------------------------------------------------------------------------
// Zurückliefern, welche BlockArt sich Links vom übergebenen Rect befindet
// --------------------------------------------------------------------------------------

int	TileEngineClass::BlockLinks(float &x, float &y, RECT rect)
{
	int			 Art    = 0;
	unsigned int xlevel = (unsigned int)((x+rect.left-1) / TILESIZE_X);
	unsigned int ylevel;

	for(int i=rect.top; i<rect.bottom; i++)
	{
		ylevel = (unsigned int)((y+i) / TILESIZE_Y);

		if (Tiles[xlevel][ylevel].Block > 0 && Art < WALL)
		{
			Art = Tiles[xlevel][ylevel].Block;
			if (Art >= WALL)									// x-Position begradigen ?
				x = (float)(xlevel*TILESIZE_X+rect.left+21);	// wegen Fehlern bei kleiner fps
		}
	}

	return Art;
}

// --------------------------------------------------------------------------------------
// Zurückliefern, welche BlockArt sich Links vom übergebenen Rect befindet (beim Spieler)
// Komischerweise versetzt es den Spieler beim Linksblocke und einer "+21 Begradigung"
// zu weit nach rechts, während es bei anderen Objekten funzt, daher eine Extra Funtkion
// für den Spieler
// --------------------------------------------------------------------------------------

int	TileEngineClass::BlockLinksPlayer(float &x, float &y, RECT rect)
{
	int			 Art    = 0;
	unsigned int xlevel = (unsigned int)((x+rect.left-1) / TILESIZE_X);
	unsigned int ylevel;

	for(int i=rect.top; i<rect.bottom; i++)
	{
		ylevel = (unsigned int)((y+i) / TILESIZE_Y);

		if (Tiles[xlevel][ylevel].Block > 0 && Art < WALL)
		{
			Art = Tiles[xlevel][ylevel].Block;
			if (Art >= WALL)									// x-Position begradigen ?
				x = (float)(xlevel*TILESIZE_X+rect.left);		// wegen Fehlern bei kleiner fps
		}
	}

	return Art;
}

// --------------------------------------------------------------------------------------
// Zurückliefern, welche BlockArt sich oberhalb vom übergebenen Rect befindet
// --------------------------------------------------------------------------------------

int	TileEngineClass::BlockOben(float &x, float &y, RECT rect)
{
	int			 Art;
	unsigned int ylevel = (unsigned int)((y+rect.top-1) / TILESIZE_Y);
	unsigned int xlevel;

	for(int i=rect.left; i<rect.right; i++)
	{
		xlevel = (unsigned int)((x+i) / TILESIZE_X);

		Art = Tiles[xlevel][ylevel].Block;
		if(Art > 0) return Art;
	}

	return 0;
}

// --------------------------------------------------------------------------------------
// Zurückliefern, welche BlockArt sich unterhalb vom übergebenen Rect befindet
// und dabei nicht "begradigen" sprich die y-Position an das Tile angleichen
// --------------------------------------------------------------------------------------

int	TileEngineClass::BlockUntenNormal(float &x, float &y, RECT rect)
{
	int			 Art    = 0;
	unsigned int ylevel = (unsigned int)((y+rect.bottom+1) / TILESIZE_Y);
	unsigned int xlevel;


	for(int i=rect.left; i<rect.right; i++)
	{
		xlevel = (unsigned int)((x+i) / TILESIZE_X);

		if (Tiles[xlevel][ylevel].Block > 0 && Art < PLATFORM)
			Art = Tiles[xlevel][ylevel].Block;
	}

	return Art;
}

// --------------------------------------------------------------------------------------
// Zurückliefern, welche BlockArt sich unterhalb vom übergebenen Rect befindet
// --------------------------------------------------------------------------------------

int	TileEngineClass::BlockUnten(float &x, float &y, RECT rect)
{
	int			 Art    = 0;
	unsigned int ylevel = (unsigned int)((y+rect.bottom+1) / TILESIZE_Y);
	unsigned int xlevel;


	for(int i=rect.left; i<rect.right; i++)
	{
		xlevel = (unsigned int)((x+i) / TILESIZE_X);

		if (Tiles[xlevel][ylevel].Block > 0 && Art < PLATFORM)
		{
			Art = Tiles[xlevel][ylevel].Block;
			if (Art == WALL		||								// y-Position begradigen ?
				Art == PLATFORM ||
				Art == ANIMATED_WALL)
				y = (float)(ylevel*TILESIZE_Y-rect.bottom-1);	// wegen Fehlern bei kleiner fps
		}
	}

	return Art;
}
