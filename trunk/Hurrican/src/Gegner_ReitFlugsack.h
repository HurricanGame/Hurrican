#ifndef _GEGNER_REITFLUGSACK_H_
#define _GEGNER_REITFLUGSACK_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerReitFlugsack : public GegnerClass
{
private:
    float ShotCount;								// Schuss Delay
    float SmokeCount;								// Rauch  Delay
    float ArrowCount;

public:

    GegnerReitFlugsack(int  Wert1, int Wert2,			// Konstruktor
                       bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);
};

#endif
