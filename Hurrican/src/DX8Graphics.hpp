// Datei : DX8Graphics.hpp

// --------------------------------------------------------------------------------------
//
// Direct Graphics Klasse
// zum initialisieren von DirectX8
// beinhaltet zudem verschiedene Grafik-Funktionen zum Speichern von Screenshots usw
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _DX8GRAPHICS_HPP_
#define _DX8GRAPHICS_HPP_

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#if defined(PLATFORM_SDL)
#include "SDL_port.hpp"
#if defined(USE_GL2)
#include "cshader.hpp"
#if defined(USE_FBO)
#include "cfbo.h"
#endif /* USE_FBO */
#endif /* USE_GL2 */
#else
#include <D3dx8math.h>
#include <d3d8.h>
#include <d3dx8.h>
#include <d3dx8tex.h>
#endif

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define D3DFVF_TLVERTEX (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_DIFFUSE)  // Eigenes Flexible Vertex Format

#define BackBufferSize 10  // Vertrex Unterteilungen des Backbuffers, muss gerade sein

#define ADDITIV_MODE 0
#define COLORKEY_MODE 1
#define WHITE_MODE 2

#if defined(USE_GL2)
enum { PROGRAM_COLOR = 0, PROGRAM_TEXTURE, PROGRAM_TOTAL, PROGRAM_NONE };
#endif

// --------------------------------------------------------------------------------------
// Strukturen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Struktur für einen 2D Vertex
// --------------------------------------------------------------------------------------

// DKS - Removed unnecessary z-coordinate:
struct VERTEX2D {
    float x, y;      // x,y Koordinaten
    D3DCOLOR color;  // Vertex-Color
    float tu, tv;    // Textur-Koordinaten
};

// DKS - Added
struct QUAD2D {
    VERTEX2D v1, v2, v3, v4;
};

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// DirectGraphics Klasse
// --------------------------------------------------------------------------------------

class DirectGraphicsClass {
  private:
    bool VSyncEnabled;  // VSync ein/aus ?
    bool FilterMode;    // Linearer Filter an/aus?
    const char *glextensions;
    bool use_texture;
    int MaxTextureUnits;
#if defined(USE_GL2)
    GLuint ProgramCurrent;
    CShader Shaders[PROGRAM_TOTAL];
#endif

  public:
    int BlendMode;  // Additiv, Colorkey oder White mode aktiviert?
    bool SupportedETC1;
    bool SupportedPVRTC;
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_Window *Window;
    SDL_GLContext GLcontext;
#else /* SDL 1.2 */
    SDL_Surface *Screen;
#endif
    SDL_Rect WindowView;
    SDL_Rect RenderView;
    SDL_Rect RenderRect;
#if defined(USE_GL2) && defined(USE_FBO)
    CFbo RenderBuffer;
#endif
    void ShowBackBuffer();  // Present aufrufen

    DirectGraphicsClass();   // Konstruktor
    ~DirectGraphicsClass();  // Desktruktor

    bool Init(std::uint32_t dwBreite, std::uint32_t dwHoehe, std::uint32_t dwZ_Bits, bool VSync);
    bool Exit();  // D3D beenden
    bool SetDeviceInfo();

    bool TakeScreenshot(const char Filename[100], int screenx,
                        int screeny);   // Screenshot machen
    void SetColorKeyMode();         // Alpha für Colorkey oder
    void SetAdditiveMode();         // Additive-Blending nutzen
    void SetWhiteMode();            // Komplett weiss rendern
    void SetFilterMode(bool filteron);  // Linearer Textur Filter ein/aus

    void RendertoBuffer(GLenum PrimitiveType,          // Rendert in den Buffer, der am Ende
                        std::uint32_t PrimitiveCount,  // eines jeden Frames komplett in
                        void *pVertexStreamZeroData);  // den Backbuffer gerendert wird

    void DisplayBuffer();  // Render den Buffer auf den Backbuffer
    // DKS - SetTexture is now used for both GL and DirectX, and uses new TexturesystemClass:
    void SetTexture(int idx);
#if defined(PLATFORM_SDL)
    bool ExtensionSupported(const char *ext);
    void SetupFramebuffers();
    void ClearBackBuffer();
    void SelectBuffer(bool active);
    void DrawTouchOverlay();
    void DrawCircle(uint16_t x, uint16_t y, uint16_t radius);
#endif
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern LPDIRECT3D8 lpD3D;                   // Direct3D Hauptobjekt
extern LPDIRECT3DDEVICE8 lpD3DDevice;       // Direct3D Device-Objekt
extern DirectGraphicsClass DirectGraphics;  // DirectGraphics Klasse
extern LPDIRECT3DSURFACE8 lpBackbuffer;     // Der Backbuffer
extern LPDIRECT3DVERTEXBUFFER8 lpVBSprite;  // VertexBuffer für die Sprites
extern D3DXMATRIX matProj;                  // Projektionsmatrix
extern D3DXMATRIX matWorld;                 // Weltmatrix
extern float DegreetoRad[360];              // Tabelle mit Rotationswerten

#endif
