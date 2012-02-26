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

#include <time.h>
#include <stdexcept>
#include <string>
#include "Globals.h"
#include "Gameplay.h"
#include "Logdatei.h"
#include "Main.h"
#include "DX8Font.h"
#include "DX8Graphics.h"

const double BackBufferX = 1024.0f;
const double BackBufferY = 1024.0f;

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
                           DWORD dwZ_Bits, bool VSync)
{
	Protokoll.WriteText("\n--> Direct3D init <--\n", false);
	Protokoll.WriteText(  "---------------------\n\n", false);

	if(VSync == true)
		Protokoll.WriteText(  "using VSync...\n", false);
	else
		Protokoll.WriteText(  "NOT using VSync...\n", false);

	//D3DDISPLAYMODE			DisplayMode;
	//HRESULT					Res;

	lpD3DDevice = NULL;

	ZeroMemory(&d3dpp,sizeof(d3dpp));

	lpD3D = Direct3DCreate8(D3D_SDK_VERSION);	

	//Res = lpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &DisplayMode);

	d3dpp.Windowed							= CommandLineParams.RunWindowMode;	// Fenster Modus ?
	d3dpp.BackBufferCount					= 1;							// 1 Backbuffer setzen
	d3dpp.EnableAutoDepthStencil			= true;
	d3dpp.AutoDepthStencilFormat			= D3DFMT_D16;
	d3dpp.hDeviceWindow						= hwnd;							// Fenster Handle
	d3dpp.BackBufferWidth					= SCREENWIDTH;					// ScreenBreite
	d3dpp.BackBufferHeight					= SCREENHEIGHT;					// Screenhöhe
	d3dpp.BackBufferFormat					= D3DFMT_X8R8G8B8;				

	d3dpp.SwapEffect	= D3DSWAPEFFECT_COPY_VSYNC;		// VSync an	
	//if(VSync == true)	d3dpp.SwapEffect	= D3DSWAPEFFECT_COPY_VSYNC;		// VSync an	
//				else	
					d3dpp.SwapEffect	= D3DSWAPEFFECT_DISCARD;		// VSync aus

	if(CommandLineParams.RunWindowMode == false)
	{
		d3dpp.FullScreen_RefreshRateInHz		= D3DPRESENT_RATE_DEFAULT;		// Refresh Rate
		if(VSync == true)														// VSYNC anlassen
			d3dpp.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_ONE;		
		else																	// VSYNC auslassen
			d3dpp.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;		
	}

	d3dpp.Flags								= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;	// Backbuffer 

	// ok, now try some video modes
	
	// 32 Bit / Hardware  X8R8G8B8
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	if (SUCCEEDED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
                          D3DDEVTYPE_HAL,
                          g_hwnd,
                          D3DCREATE_HARDWARE_VERTEXPROCESSING,
                          &d3dpp,
                          &lpD3DDevice)))
		goto _ModeFound;
		
	// 16 Bit / Hardware R5B6G5
	d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
	if (SUCCEEDED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
                          D3DDEVTYPE_HAL,
                          g_hwnd,
                          D3DCREATE_HARDWARE_VERTEXPROCESSING,
                          &d3dpp,
                          &lpD3DDevice)))
		goto _ModeFound;
		
	// 32 Bit / Software X8R8G8B8
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	if (SUCCEEDED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
                          D3DDEVTYPE_HAL,
                          g_hwnd,
                          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                          &d3dpp,
                          &lpD3DDevice)))
		goto _ModeFound;

	// 16 Bit / Software R5G6B5
	d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
	if (SUCCEEDED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
                          D3DDEVTYPE_HAL,
                          g_hwnd,
                          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                          &d3dpp,
                          &lpD3DDevice)))
		goto _ModeFound;

	// nothing so far? OK... go for 32 bit reference rasterizer = ultraslow!
	// note: this does not work on my machine, so i have no idea if that helps
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	if (SUCCEEDED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
                          D3DDEVTYPE_REF,
                          g_hwnd,
                          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                          &d3dpp,
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
	
	Protokoll.WriteText("Texture restrictions:\n", false);

	if (d3dCaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY)	
	{
		Protokoll.WriteText("Square Only: TRUE\n", false);
		SquareOnly = true;
	}
	else
	{
		Protokoll.WriteText("Square Only: FALSE\n", false);
		SquareOnly = false;
	}

	// Device kann nur Texturen mit 2er-Potenz-Grösse
	if (d3dCaps.TextureCaps & D3DPTEXTURECAPS_POW2)
	{
		Protokoll.WriteText("Power of Two: TRUE\n", false);
		PowerOfTwo = true; 
	}
	else
	{
		Protokoll.WriteText("Power of Two: FALSE\n", false);
		PowerOfTwo = false;
	}

	SetDeviceInfo();

	Protokoll.WriteText("\n-> Direct3D init successfull!\n\n", false);

	// DegreetoRad-Tabelle füllen
	for(int i=0; i<360; i++)
		DegreetoRad[i] = float(PI * i / 180);

	SetColorKeyMode();

	return true;
}

// --------------------------------------------------------------------------------------
// Direct3D beenden
// --------------------------------------------------------------------------------------

bool DirectGraphicsClass::Exit(void)
{
	SafeRelease (lpD3DDevice);
	SafeRelease (lpD3D);
	Protokoll.WriteText("-> Direct3D erfolgreich beendet !\n", false);
	return true;
}

// --------------------------------------------------------------------------------------
// Infos für Device Objekt setzen
// Für Init und nach Task Wechsel
// --------------------------------------------------------------------------------------

bool DirectGraphicsClass::SetDeviceInfo(void)
{
	HRESULT hr;

	// Globale Variable mit dem tatsächlichen BackBuffer füllen	
	lpD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &lpBackbuffer);

   // Licht, Cullmodus und Z-Buffer aktivieren
   lpD3DDevice->SetRenderState(D3DRS_AMBIENT,D3DCOLOR_XRGB(255,255,255));
   lpD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
   lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); 
   lpD3DDevice->SetRenderState(D3DRS_ZENABLE,  D3DZB_FALSE);

   	// Projektionsmatrix einstellen
   //
	D3DXMatrixOrthoOffCenterLH(&matProj, 0, 640, 480, 0, 0.0f, 1.0f);

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

	return true;
}

// --------------------------------------------------------------------------------------
// Screenshot machen
// bei Filename000.bmp anfangen und dann weiter bis 999, falls diese schon existieren
// --------------------------------------------------------------------------------------

bool DirectGraphicsClass::TakeScreenshot(char Filename[100], int screenx, int screeny)
{	
	FILE*				f = NULL;					// Datei
	HRESULT				hr;							// Für Fehler-Prüfung
	IDirect3DSurface8*	FrontBuffer;				// Zeiger auf Frontbuffer

	// Surface erzeugen, in die das Bild kopiert wird
	lpD3DDevice->CreateImageSurface(screenx, screeny, D3DFMT_A8R8G8B8, &FrontBuffer);

	// Front Buffer hineinkopieren
	hr = lpD3DDevice->GetFrontBuffer(FrontBuffer);

	// Fehler ?
	if(hr != D3D_OK)
	{
		Protokoll.WriteText("\n-> TakeScreenshot error!\n", false);
		FrontBuffer->Release();
		return false;
	}

	// BMP Datei erzeugen, wobei bereits existierende Dateien nicht überschrieben werden
	// so entstehen dann Screenshot000 - Screenshot999

	char	TempName[100];
	char	TempDate[128];
	char	Buffer[10];
	int		i=0;

	_tzset();
	_strdate_s(TempDate);

	// Zeichen im Datum ersetzen
	int len = strlen(TempDate);
	for (int i=0; i<len; i++)
		if (TempDate[i] == '/')
			TempDate[i] = '-';

	i = 0;

	do
	{
		_itoa_s(i, Buffer, 10);

		// Datei Namen zusammensetzen
		strcpy_s(TempName, strlen(Filename) + 1, Filename);					// Name
		strcat_s(TempName, 2, "_");
		strcat_s(TempName, strlen(TempDate) + 1, TempDate);
		strcat_s(TempName, 2, "_");
		if(i<10)  strcat_s(TempName, 2, "0"); 			// evtl noch 0
		if(i<100) strcat_s(TempName, 2, "0");			// oder gar  00 davor schreiben
		strcat_s(TempName, strlen(Buffer) + 1, Buffer);					// Aktuelle Zahl
		strcat_s(TempName, 5, ".BMP");					// und BMP Endung
		fopen_s(&f, TempName, "rb");
		i++;
	}
	while (f != NULL);

	// Screenshot Datei erzeugen
	D3DXSaveSurfaceToFile(TempName, D3DXIFF_BMP, FrontBuffer, NULL, NULL);

	// Surface wieder freigeben
	FrontBuffer->Release();

	return true;
}

// --------------------------------------------------------------------------------------
// Renderstates für Sprites mit ColorKey setzen
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::SetColorKeyMode(void)
{
	if (BlendMode == COLORKEY_MODE)
		return;

	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);	// Colorkey
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// einschalten    

	BlendMode = COLORKEY_MODE;
}

// --------------------------------------------------------------------------------------
// Renderstates für Sprites setzen, die komplett weiss gerendert werden
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::SetWhiteMode(void)
{
	if (BlendMode == WHITE_MODE)
		return;

	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_INVSRCALPHA  );	// White
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA );	// einschalten    

	BlendMode = WHITE_MODE;
}

// --------------------------------------------------------------------------------------
// Renderstates für Sprites mit Additivem Alphablending setzen
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::SetAdditiveMode(void)
{
	if (BlendMode == ADDITIV_MODE)
		return;

	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);	// Additive Blending 
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);	// einschalten

	BlendMode = ADDITIV_MODE;
}

// --------------------------------------------------------------------------------------
// Renderstates für linearen Texturfilter ein/ausschalten
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::SetFilterMode (bool filteron)
{
	// Filter schon an? Dann nichts ändern
	//

	if (filteron == FilterMode)
		return;

	// Filter einschalten
	//
	if (filteron == true)
	{		
		lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	}

	// andernfalls Filter ausschalten
	//
	else
	{
		lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
		lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
	}
		
	FilterMode = filteron;
}

// --------------------------------------------------------------------------------------
// Rendert in den Buffer, der am Ende eines jeden Frames komplett 
// in den Backbuffer gerendert wird
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::RendertoBuffer (D3DPRIMITIVETYPE PrimitiveType,
										  UINT PrimitiveCount,
										  CONST void* pVertexStreamZeroData)
{
	lpD3DDevice->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, sizeof(VERTEX2D));
}

// --------------------------------------------------------------------------------------
// Render den Buffer auf den Backbuffer
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::DisplayBuffer  (void)
{
	// Darstellung beenden
	lpD3DDevice->EndScene();

	// Backbuffer mit Frontbuffer tauschen
	ShowBackBuffer();	
}

// --------------------------------------------------------------------------------------
// Present aufrufen
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::ShowBackBuffer(void)
{
	HRESULT hresult;
	
	hresult = lpD3DDevice->Present(NULL, NULL, 0, NULL);		// Frontbuffer anzeigen
}
