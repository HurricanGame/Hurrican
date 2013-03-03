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

#include "Gameplay.h"
#include "stdio.h"
#include "lightmap.h"
#include "Logdatei.h"
#include "unrarlib.h"

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
#if defined(PLATFORM_DIRECTX)
    HBITMAP             hbm = NULL;
    BITMAP              bm;
    HDC                 hdcImage;
	LPDIRECT3DTEXTURE8	tempText = NULL;
#elif defined(PLATFORM_SDL)
    uint32_t*           hbm = NULL;
    image_t             image;
#endif
	char				Name[100];
	char				Temp[100];
	char				*pData;
	unsigned long		Size;
	FILE 				*TempFile = NULL;

	sprintf_s(Temp, "data/%s", filename);
	if (FileExists(Temp))
		goto loadfile;

	if (urarlib_get(&pData, &Size, (char*)filename, RARFILENAME, convertText(RARFILEPASSWORD)) == false)
	{
		sprintf_s(Name, "Error loading Lightmap %s !\n",filename);
		Protokoll.WriteText(Name, false);
		return;
	}

	fopen_s (&TempFile, TEMP_FILE_PREFIX "temp.dat", "wb");	// Datei öffnen
	fwrite (pData, Size, 1, TempFile);			// speichern
	fclose (TempFile);							// und schliessen

	strcpy_s(Temp, sizeof(Temp), TEMP_FILE_PREFIX "temp.dat");

loadfile:

#if defined(PLATFORM_DIRECTX)
	//load bimap

	hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), Temp, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
#elif defined(PLATFORM_SDL)
    loadImage( image, Temp );
    hbm = (uint32_t*)image.data;
#endif

	if (TempFile) DeleteFile(TEMP_FILE_PREFIX "temp.dat");

	if (hbm == NULL)
	{
		strcpy_s(Name, strlen("Error loading Lightmap \"") + 1, "Error loading Lightmap \"");
		strcat_s(Name, strlen(filename) + 1, filename);
		strcat_s(Name, 3, "\"!");
		Protokoll.WriteText(Name, true);
		return;
	}

#if defined(PLATFORM_DIRECTX)
    // get size of the bitmap
    //
    GetObject(hbm, sizeof(bm), &bm);    // get size of bitmap

	hdcImage = CreateCompatibleDC(NULL);

    SelectObject(hdcImage, hbm);


	// Größe korrigieren (für 2er Potenz Texturen)
	//
	xsize = bm.bmWidth;
	ysize = bm.bmHeight;
#elif defined(PLATFORM_SDL)
	xsize = image.w;
	ysize = image.h;
#endif

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
#if defined(PLATFORM_DIRECTX)
		DWORD BGRcolor=GetPixel(hdcImage,x,y);
		map[count] = D3DCOLOR_RGBA(GetRValue(BGRcolor),
								   GetGValue(BGRcolor),
								   GetBValue(BGRcolor),
								   255);
#elif defined(PLATFORM_SDL)
        //SDL_Color components;
        map[count] = hbm[count];
        /*
		DWORD BGRcolor=getpixel(hbm,x,y);
		int_to_rgb( BGRcolor, components );
		map[count] = D3DCOLOR_RGBA(components.r,
								   components.g,
								   components.b,
								   255);
								   */
#endif
		count++;
	 }

#if defined(PLATFORM_DIRECTX)
	// Bitmap freigeben
    DeleteObject(hbm);
#elif defined(PLATFORM_SDL)
    delete [] hbm;
#endif
}
