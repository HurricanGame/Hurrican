// Datei : lightmap.cpp

// -------------------------------------------------------------------------------------- 
//
// Lightmaps für das Hurrican Level
//
// (c) 2005 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "gameplay.h"
#include "stdio.h"
#include "lightmap.h"
#include "Logdatei.h"
#include "Unrarlib.h"

// --------------------------------------------------------------------------------------
// LigthMap Klasse
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

CLightMap::CLightMap(void)
{
	map = NULL;
	xsize = 0;
	ysize = 0;
}

// --------------------------------------------------------------------------------------
// Detruktor
// --------------------------------------------------------------------------------------

CLightMap::~CLightMap(void)
{
	if(map != NULL)
	{
		delete [] (map);
		map = NULL;
	}
}

// --------------------------------------------------------------------------------------
// LightMap "FileName" laden
// --------------------------------------------------------------------------------------

void CLightMap::Load(const char *filename)
{
    HBITMAP             hbm = NULL;
    BITMAP              bm;
    HDC                 hdcImage;   
	LPDIRECT3DTEXTURE8	tempText = NULL;
	char				Name[100];
	char				*pData; 
	unsigned long		Size; 
	
	if (urarlib_get(&pData, &Size, (char*)filename, RARFILENAME, convertText(RARFILEPASSWORD)) == false)
	{
		sprintf_s(Name, "Error loading Lightmap %s !",filename);		
		Protokoll.WriteText(Name, false);
		return;
	}

	FILE *TempFile = NULL;
	fopen_s (&TempFile, "temp.dat", "wb");	// Datei öffnen
	fwrite (pData, Size, 1, TempFile);			// speichern
	fclose (TempFile);							// und schliessen

	//load bimap
	hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), "temp.dat", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	DeleteFile("temp.dat");

	if (hbm == NULL)
	{		
		strcpy_s(Name, strlen("Error loading Lightmap \"") + 1, "Error loading Lightmap \"");
		strcat_s(Name, strlen(filename) + 1, filename);
		strcat_s(Name, 3, "\"!");		
		Protokoll.WriteText(Name, true);
		return;
	}
	
    // get size of the bitmap
    //
    GetObject(hbm, sizeof(bm), &bm);    // get size of bitmap

	hdcImage = CreateCompatibleDC(NULL);
    
	

    SelectObject(hdcImage, hbm);

	// Größe korrigieren (für 2er Potenz Texturen)
	//
	xsize = bm.bmWidth;
	ysize = bm.bmHeight;

	// neues MapArray erstellen
	map = new D3DCOLOR[xsize*ysize];

    // Daten von der Bitmap in das MapArray kopieren
	// dabei gehen wir von 1 bis size - 1
	// holen uns immer vier punkte, die auf ein tile-eck wirken
	// errechnen den durchschnitt und machen daraus eine d3dcolor
	// so ist die lightmap quasi gleich interpoliert
	//
	int count = 0;
	for (int x=0; x<xsize ; ++x)
	 for (int y=0; y<ysize ; ++y)
	 {
		DWORD BGRcolor=GetPixel(hdcImage,x,y);
		map[count] = D3DCOLOR_RGBA(GetRValue(BGRcolor),
								   GetGValue(BGRcolor),
								   GetBValue(BGRcolor),
								   255);
		count++;
	 }
    
	// Bitmap freigeben
    DeleteObject(hbm);	
}