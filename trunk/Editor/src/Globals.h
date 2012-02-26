// Datei : Globals.h

// -------------------------------------------------------------------------------------- 
//
// Header Datei mit global gültigen Makros, Defines usw
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __Globals_h__
#define __Globals_h__

// --------------------------------------------------------------------------------------
// Defines 
// --------------------------------------------------------------------------------------

#define GFXPATH ""

#define SCREENWIDTH		640							// D3D Screen Breite
#define	SCREENHEIGHT	480							// D3D Screen Höhe
#define	RUNWINDOWMODE	true						// Fenster oder Vollbild ?

// Tasten Abfragen (simple Methode)
#define SimpleKeydown(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

// Schnittstellen erst prüfen, ob sie existieren, und dann freigeben
#define SafeRelease(x) if (x != NULL) { x->Release(); x=NULL; }

#define RGB16BIT555(r,g,b) ((b%32) + ((g%32) << 5) + ((r%32) << 10))	// Pixelformat 555
#define RGB16BIT565(r,g,b) ((b%32) + ((g%64) << 5) + ((r%32) << 11))	// Pixelformat 565


#endif