#ifndef _GEGNER_ROBOMAN1_H_
#define _GEGNER_ROBOMAN1_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerRoboMan1 : public GegnerClass
{
private:
    float ShotCount;								// Schuss Delay
    float SmokeCount;								// Rauch  Delay

public:

    GegnerRoboMan1(int  Wert1, int Wert2,				// Konstruktor
                   bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
