#ifndef _GEGNER_LAVABALL_H_
#define _GEGNER_LAVABALL_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerLavaBall : public GegnerClass
{
public:
    float	yStart;
    float	SmokeDelay, FlareDelay;
    bool	InLava;

    GegnerLavaBall (int  Wert1, int Wert2,				// Konstruktor
                    bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Eigene Drawroutine
};

#endif

