// Datei : DX8Graphics.h

// -------------------------------------------------------------------------------------- 
//
// Direct Graphics Klasse
// zum initialisieren von DirectX8
// beinhaltet zudem verschiedene Grafik-Funktionen zum Speichern von Screenshots usw
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __DX8Graphics_h__
#define __DX8Graphics_h__

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include <d3d8.h>
#include <d3dx8.h>

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define D3DFVF_TLVERTEX (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_DIFFUSE)	// Eigenes Flexible Vertex Format

// --------------------------------------------------------------------------------------
// Strukturen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Struktur für einen 2D Vertex
// --------------------------------------------------------------------------------------

struct VERTEX2D
{ 
	float		x, y, z;		// x,y,z Koordinaten
	D3DCOLOR	color;			// Vertex-Color
	float		tu, tv;			// Textur-Koordinaten
};  

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// DirectGraphics Klasse
// --------------------------------------------------------------------------------------

class DirectGraphicsClass
{
	private:
		bool					useVSync;						// VSync ein/aus ?
		LPDIRECT3D8				lpD3D;							// Direct3D Hauptobjekt
		D3DDISPLAYMODE			d3ddm;							// Display Mode
		D3DPRESENT_PARAMETERS	d3dpp;							// Aktuelle Einstellungen
		
	public:
		bool					SquareOnly;						// Nur quadratische Texturen
		bool					PowerOfTwo;						// Nur 2er Potenz Texturen

	     DirectGraphicsClass(void);							// Konstruktor
		~DirectGraphicsClass(void);							// Desktruktor

		bool Init(HWND hwnd, DWORD dwBreite, DWORD dwHoehe,
                  DWORD dwZ_Bits, DWORD dwStencil_Bits, bool VSync);
		bool Exit(void);										// D3D beenden

		bool TakeScreenshot(char Filename[100]);				// Screenshot machen
		bool PutPixel(int x, int y, UCHAR rot,UCHAR gruen,		// Pixel setzen	
					  UCHAR blau, LPDIRECT3DSURFACE8 lpDDSurf);
		void SetColorKeyMode(void);								// Alpha für Colorkey oder
		void SetAdditiveMode(void);								// Additive-Blending nutzen
};

#endif