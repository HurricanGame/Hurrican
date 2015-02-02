#ifndef _GEGNER_FLUGSACK_H_
#define _GEGNER_FLUGSACK_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerFlugsack : public GegnerClass
{
private:
    float ShotCount;								// Schuss Delay
    float SmokeCount;								// Rauch  Delay

public:

    GegnerFlugsack(int  Wert1, int Wert2,				// Konstruktor
                   bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
