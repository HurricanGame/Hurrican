// Datei : CCracktro.h

// --------------------------------------------------------------------------------------
//
// Klasse für das Cracktro am Anfang
//
// (c) 2005 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _Cracktro_h_
#define _Cracktro_h_

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "DX8Input.h"
#include "DX8Font.h"
#include "DX8Sprite.h"
#include "Globals.h"
#include "Menu.h"
#include "Timer.h"

// --------------------------------------------------------------------------------------
// Cracktro-Klasse
// --------------------------------------------------------------------------------------

class CCracktro
{
private:
    DirectGraphicsSprite Bars[3];
    DirectGraphicsSprite Logo[2];
    DirectGraphicsSprite Zahlen;
    DirectGraphicsSprite Star;
    DirectGraphicsFont	*pFont;
    float				ScrollOffset, ScrollOffset2;
    int					ScrollPos, ScrollPos2;
    float				SinPos;
    int					State;

    StarStruct	Stars[200];

public:
    bool	b_running;

    CCracktro(void);						// Cracktro initialisieren
    ~CCracktro(void);						// Cracktro freigeben

    void		Run(void);							// Cracktro laufen lassen
    void		Main(void);
    void		Load(void);
}; // CCracktro

#endif
