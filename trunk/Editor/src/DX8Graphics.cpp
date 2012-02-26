// Datei : DX8Graphics.cpp

// -------------------------------------------------------------------------------------- 
//
// Direct Graphics Klasse
// zum initialisieren von DirectX8
// beinhaltet zudem verschiedene Grafik-Funktionen zum Speichern von Screenshots usw
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "Globals.h"
#include "Logdatei.h"
#include "DX8Graphics.h"
#include <stdexcept>
#include <string>

// --------------------------------------------------------------------------------------
// externe Variablen
// --------------------------------------------------------------------------------------

extern Logdatei				Protokoll;					// Protokoll Datei
extern LPDIRECT3DDEVICE8	lpD3DDevice;				// Direct3D Device-Objekt
extern LPDIRECT3DSURFACE8	lpBackbuffer;				// Der Backbuffer

// --------------------------------------------------------------------------------------
// sonstige Variablen
// --------------------------------------------------------------------------------------

D3DXMATRIX					matProj;					// Projektionsmatrix
D3DXMATRIX					matWorld;					// Weltmatrix
float						DegreetoRad[360];			// Tabelle mit Rotationswerten

// --------------------------------------------------------------------------------------
// Klassenfunktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// DirectGraphics Klasse
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

DirectGraphicsClass::DirectGraphicsClass(void)
{
	lpD3D = NULL;
}

// --------------------------------------------------------------------------------------
// Desktruktor
// --------------------------------------------------------------------------------------

DirectGraphicsClass::~DirectGraphicsClass(void)
{
}

// --------------------------------------------------------------------------------------
// D3D Initialisieren
// --------------------------------------------------------------------------------------

bool DirectGraphicsClass::Init(HWND hwnd, DWORD dwBreite, DWORD dwHoehe,
                           DWORD dwZ_Bits, DWORD dwStencil_Bits,bool VSync)
{
	Protokoll.WriteText("\n--> Direct3D init <--\n", false);
	Protokoll.WriteText(  "---------------------\n\n", false);

	if(VSync == true)
		Protokoll.WriteText(  "using VSync...\n", false);
	else
		Protokoll.WriteText(  "NOT using VSync...\n", false);

   	D3DPRESENT_PARAMETERS	PresentParams;
	D3DDISPLAYMODE			DisplayMode;
	HRESULT					Res, hr;

	lpD3DDevice = NULL;

	ZeroMemory(&PresentParams,sizeof(PresentParams));

	lpD3D = Direct3DCreate8(D3D_SDK_VERSION);	

	Res = lpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &DisplayMode);

	PresentParams.Windowed							= RUNWINDOWMODE;				// Fenster Modus ?
	PresentParams.BackBufferCount					= 1;							// 1 Backbuffer setzen
	PresentParams.EnableAutoDepthStencil			= true;
	PresentParams.AutoDepthStencilFormat			= D3DFMT_D16;
	PresentParams.hDeviceWindow						= hwnd;							// Fenster Handle
	PresentParams.BackBufferWidth					= SCREENWIDTH;					// ScreenBreite
	PresentParams.BackBufferHeight					= SCREENHEIGHT;					// Screenhöhe
	PresentParams.BackBufferFormat					= D3DFMT_X8R8G8B8;				

	if(VSync == true)	PresentParams.SwapEffect	= D3DSWAPEFFECT_COPY_VSYNC;		// VSync an	
				else	PresentParams.SwapEffect	= D3DSWAPEFFECT_DISCARD;		// VSync aus

	if(RUNWINDOWMODE == false)
	{
		PresentParams.FullScreen_RefreshRateInHz		= D3DPRESENT_RATE_DEFAULT;		// Refresh Rate
		if(VSync == true)														// VSYNC anlassen
			PresentParams.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_ONE;		
		else																	// VSYNC auslassen
			PresentParams.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;		
	}

	PresentParams.Flags								= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;	// Backbuffer 

	// ok, now try some video modes
	
	// 32 Bit / Hardware  X8R8G8B8
	PresentParams.BackBufferFormat = D3DFMT_X8R8G8B8;
	if (SUCCEEDED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
                          D3DDEVTYPE_HAL,
                          hwnd,
                          D3DCREATE_HARDWARE_VERTEXPROCESSING,
                          &PresentParams,
                          &lpD3DDevice)))
		goto _ModeFound;
		
	// 16 Bit / Hardware R5B6G5
	PresentParams.BackBufferFormat = D3DFMT_R5G6B5;
	if (SUCCEEDED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
                          D3DDEVTYPE_HAL,
                          hwnd,
                          D3DCREATE_HARDWARE_VERTEXPROCESSING,
                          &PresentParams,
                          &lpD3DDevice)))
		goto _ModeFound;
		
	// 32 Bit / Software X8R8G8B8
	PresentParams.BackBufferFormat = D3DFMT_X8R8G8B8;
	if (SUCCEEDED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
                          D3DDEVTYPE_HAL,
                          hwnd,
                          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                          &PresentParams,
                          &lpD3DDevice)))
		goto _ModeFound;

	// 16 Bit / Software R5G6B5
	PresentParams.BackBufferFormat = D3DFMT_R5G6B5;
	if (SUCCEEDED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
                          D3DDEVTYPE_HAL,
                          hwnd,
                          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                          &PresentParams,
                          &lpD3DDevice)))
		goto _ModeFound;

	// nothing so far? OK... go for 32 bit reference rasterizer = ultraslow!
	// note: this does not work on my machine, so i have no idea if that helps
	PresentParams.BackBufferFormat = D3DFMT_X8R8G8B8;
	if (SUCCEEDED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
                          D3DDEVTYPE_REF,
                          hwnd,
                          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                          &PresentParams,
                          &lpD3DDevice)))
		goto _ModeFound;

	// no mode found!
	Protokoll.WriteText("No compatible Graphics Mode found!\n", true);
	return false;

_ModeFound:

	Protokoll.WriteText("DX8 Device initialised!\n", false);
   
   /* Jetzt haben wir die Informationen zusammen und machen weiter */

	// Rausfinden, wie ich die Texturen laden muss (nur rechteckig, 2er Potenz)

	// Device kann nur quadratische Texturen
	D3DCAPS8 d3dCaps;

	lpD3DDevice->GetDeviceCaps(&d3dCaps);
	
	if (d3dCaps.TextureCaps | D3DPTEXTURECAPS_SQUAREONLY)	
		SquareOnly = true;
	else
		SquareOnly = false;

	// Device kann nur Texturen mit 2er-Potenz-Grösse
	if (d3dCaps.TextureCaps | D3DPTEXTURECAPS_POW2)
		PowerOfTwo = true; 
	else
		PowerOfTwo = false;

   // Globale Variable mit dem tatsächlichen BackBuffer füllen
   lpD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &lpBackbuffer);

   // Licht, Cullmodus und Z-Buffer aktivieren
   lpD3DDevice->SetRenderState(D3DRS_AMBIENT,D3DCOLOR_XRGB(255,255,255));
   lpD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
   lpD3DDevice->SetRenderState(D3DRS_CULLMODE, false); 
   lpD3DDevice->SetRenderState(D3DRS_ZENABLE,  D3DZB_FALSE);

   	// Projektionsmatrix einstellen
   //
	D3DXMatrixOrthoOffCenterLH(&matProj, 0, SCREENWIDTH, SCREENHEIGHT, 0, 0.0f, 1.0f);

	hr = lpD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj); 
	
	if(hr != D3D_OK)
	{
		Protokoll.WriteText("\n-> SetTransform error!\n", true);
		return false;
	}

	// Flexible Vertex Format setzen
	hr = lpD3DDevice->SetVertexShader (D3DFVF_TLVERTEX);		
	if(hr != D3D_OK)
	{
		Protokoll.WriteText("\n-> SetVertexShader error!\n", true);
		return false;
	}

	// Alphablending aktivieren
	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	Protokoll.WriteText("\n-> Direct3D init successfull!\n\n", false);


	SetColorKeyMode();

	return true;
}

// --------------------------------------------------------------------------------------
// Direct3D beenden
// --------------------------------------------------------------------------------------

bool DirectGraphicsClass::Exit(void)
{
	SafeRelease(lpBackbuffer);
	SafeRelease(lpD3DDevice);
	SafeRelease(lpD3D);
	Protokoll.WriteText("-> Direct3D erfolgreich beendet !\n", false);
	return true;
}

// --------------------------------------------------------------------------------------
// Screenshot machen
// bei Filename000.bmp anfangen und dann weiter bis 999, falls diese schon existieren
// --------------------------------------------------------------------------------------

bool DirectGraphicsClass::TakeScreenshot(char Filename[100])
{
	FILE *f = NULL;

	LPDIRECT3DSURFACE8	lpSurface	= NULL;			// Surface, in die wir den FrontBuffer kopieren
	LPBYTE				Bits		= NULL;			// Array mit Pixeldaten zur Konvertierung
	D3DLOCKED_RECT		LockedRect;					// Rect mit den Surface Infos

	lpD3DDevice->CreateImageSurface(SCREENWIDTH, SCREENHEIGHT,	// 32bpp Surface erstellen
									D3DFMT_A8R8G8B8, 
									&lpSurface);
	lpD3DDevice->GetFrontBuffer(lpSurface);						// FrontBuffer in Surface kopieren
	Bits = new BYTE[SCREENWIDTH * SCREENHEIGHT * 3 + 1];		// Array mit Pixeldaten erstellen (für die Konvertierung in 24 bit)

	if (!Bits)
	{
		Protokoll.WriteText("\n-> TakeScreenshot Fehler : Nicht genug Speicher um Array zu erzeugen !\n", false);
		return false;
	}

	
	if (FAILED(lpSurface->LockRect(&LockedRect, NULL,			// Surface locken
		D3DLOCK_READONLY)))
	{
		Protokoll.WriteText("\n-> TakeScreenshot LockRect Fehler !\n", false);
		return false;
	}

	// Bild vertikal spiegeln und von 32-bit in 24-bit umkonvertieren
	LPDWORD		lpSrc;				// Quelle
	LPBYTE		lpDest = Bits;		// Ziel

	for (int y = SCREENHEIGHT - 1; y >= 0; y--)					// Unten anfangen
	{
		// Adresse der aktuellen Zeile berechnen
		lpSrc = reinterpret_cast<LPDWORD>(LockedRect.pBits) + y * SCREENWIDTH;
		for (int x = 0; x < SCREENWIDTH; x++)
		{
			// Source Pixel in Bitmap Array schreiben
			*reinterpret_cast<LPDWORD>(lpDest) = *lpSrc;
			lpSrc++;        // Source pointer	   um 1 DWORD weiter (r,g,b MIT  alpha)
			lpDest += 3;    // Destination pointer um 3 bytes weiter (r,g,b OHNE alpha)
		}
	}

	lpSurface->UnlockRect();									// Surface unlocken
	SafeRelease(lpSurface);										// und freigeben

	// Bitmap InfoHeader vorbereiten
	BITMAPINFOHEADER bmih;
	bmih.biSize				= sizeof(bmih);
	bmih.biWidth			= SCREENWIDTH;
	bmih.biHeight			= SCREENHEIGHT;
	bmih.biPlanes			= 1;
	bmih.biBitCount			= 24;
	bmih.biCompression		= BI_RGB;
	bmih.biSizeImage		= SCREENWIDTH * SCREENHEIGHT * 3;
	bmih.biXPelsPerMeter	= 0;
	bmih.biYPelsPerMeter	= 0;
	bmih.biClrUsed			= 0;
	bmih.biClrImportant		= 0;

	// Bitmap FileHeader vorbereiten
	BITMAPFILEHEADER bmfh;
	bmfh.bfType = 'MB';
	bmfh.bfSize = sizeof(bmfh) + sizeof(bmih) + bmih.biSizeImage;
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(bmfh) + sizeof(bmih);


	// BMP Datei erzeugen, wobei bereits existierende Dateien nicht überschrieben werden
	// so entstehen dann Screenshot000 - Screenshot999

	char	TempName[100];
	char	Buffer[10];
	int		i=0;

	do
	{
		_itoa(i, Buffer, 10);

		// Datei Namen zusammensetzen
		strcpy(TempName, Filename);					// Name
		if(i<10)  strcat(TempName, "0"); 			// evtl noch 0
		if(i<100) strcat(TempName, "0");			// oder gar  00 davor schreiben
		strcat(TempName, Buffer);					// Aktuelle Zahl
		strcat(TempName, ".BMP");					// und BMP Endung
		f = fopen(TempName, "rb");
		i++;
	}
	while (f != NULL);

	// Datei neu erzeugen und zum schreiben öffnen
	f = fopen(TempName, "wb");
	if (!f)
	{
		Protokoll.WriteText("\n-> TakeScreenshot Fehler beim erzeugen der Screenshot-Datei !\n", false);    
		return false;
	}

	// Header sichern
	if (!fwrite(reinterpret_cast<void*>(&bmfh), sizeof(bmfh), 1, f))	
		Protokoll.WriteText("\n-> TakeScreenshot Fehler beim Schreiben in die Screenshot-Datei !\n", false);    

	// InfoHeader sichern
	if (!fwrite(reinterpret_cast<void*>(&bmih), sizeof(bmih), 1, f))	
		Protokoll.WriteText("\n-> TakeScreenshot Fehler beim Schreiben in die Screenshot-Datei !\n", false);    

	// Bitmap Daten sichern
	if (fwrite(reinterpret_cast<void*>(Bits), sizeof(BYTE),				
	    bmih.biSizeImage, f) < bmih.biSizeImage)
		Protokoll.WriteText("\n-> TakeScreenshot Fehler beim Schreiben in die Screenshot-Datei !\n", false);    

	// Datei schliessen
	fclose(f);					
	
	char Meldung[200];

	strcpy(Meldung, "-> Screenshot '");
	strcat(Meldung, TempName);
	strcat(Meldung,"' wurde gemacht !\n");
	Protokoll.WriteText(Meldung, false);
	
	return true;
}

// --------------------------------------------------------------------------------------
// Pixel an x/y mit Farbe "rot gruen blau" setzen
// --------------------------------------------------------------------------------------

bool DirectGraphicsClass::PutPixel(int x, int y, UCHAR rot,UCHAR gruen, UCHAR blau, LPDIRECT3DSURFACE8 lpDDSurf)
{
	int Zeilenbreite;			// Zeilenbreite
	int DDS_Breite;				// Breite der Surface
	int DDS_Hoehe;				// Höhe der Surface
	USHORT *Vram;				// Speicheradresse der Surface
	D3DSURFACE_DESC d3dsd;		// SurfaceDesc Struktur
	D3DLOCKED_RECT  rectLock;	// Enthält Speicheradresse

	// Breite und Höhen stehen in SurfaceDesc
	if (FAILED(lpDDSurf->GetDesc(&d3dsd))) 
	{
		Protokoll.WriteText("\n-> GetDesc() Fehler !\n", false);
		return false;
    }

	DDS_Breite = d3dsd.Width;
	DDS_Hoehe  = d3dsd.Height;

	// Surface verriegeln
	if (FAILED(lpDDSurf->LockRect(&rectLock, NULL, 0))) 
	{
		Protokoll.WriteText("\n-> LockRect() Fehler !\n", false);
		return false;
    }

	Zeilenbreite = rectLock.Pitch >> 1;
    USHORT Farbe = RGB16BIT565(rot,gruen,blau);

    // Prüfen, ob der Pixel überhaupt im sichtbaren Bereich
    // der Surface gemalt werden soll!!!
    if ( x<0 || x>(DDS_Breite-1) || y<0 || y>(DDS_Hoehe-1) )
		return false;

	Vram = (USHORT*)rectLock.pBits;
	Vram[x + y*Zeilenbreite] = Farbe;
	lpDDSurf->UnlockRect();
	return true;
} 

// --------------------------------------------------------------------------------------
// Renderstates für Sprites mit ColorKey setzen
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::SetColorKeyMode(void)
{
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);	// Colorkey
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// einschalten    
}

// --------------------------------------------------------------------------------------
// Renderstates für Sprites mit Additivem Alphablending setzen
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::SetAdditiveMode(void)
{
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);	// Additive Blending 
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);	// einschalten
}