// Datei : Outtro.h

// --------------------------------------------------------------------------------------
//
// Klasse für das Hurrican Outtro
//
// (c) 2006 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __OUTTRO_H_
#define __OUTTRO_H_

// --------------------------------------------------------------------------------------
// Inlcudces
// --------------------------------------------------------------------------------------

#include "Globals.h"
#include "DX8Font.h"
#include "DX8Sprite.h"
#include "Timer.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

//----- Zustände

#define OUTTRO_FADEIN			0
#define OUTTRO_TOWER_EXPLODE	1
#define OUTTRO_PLAYER_FLEES		2
#define OUTTRO_SCROLLER			3

// --------------------------------------------------------------------------------------
// Klassen Deklaration für das Outtro
// --------------------------------------------------------------------------------------

class OuttroClass
{
private:
    DirectGraphicsSprite	Background;
    DirectGraphicsSprite	Tower;
    DirectGraphicsSprite	Henry;
    DirectGraphicsSprite	Reiter[2];
    bool					finished;
    int						Zustand;
    float					Counter;
    int						TextOff;
    float					SmokeDelay;
    float					TowerOffset;
    float					Snow;
    float					PlayerSmoke;
    float					px[2], py[2], xs[2], ys[2];

    void	InitPlayerPos(void);

public:
    OuttroClass(void);							// Konstruktor
    ~OuttroClass(void);							// Destruktor
    void DoOuttro   (void);							// Intro ablaufen lassen

};

#endif
