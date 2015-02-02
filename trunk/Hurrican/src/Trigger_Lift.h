#ifndef _TRIGGER_LIFT_H_
#define _TRIGGER_LIFT_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerLift : public GegnerClass
{
private:
    float	SmokeDelay;

public:

    GegnerLift  (int Wert1,	int Wert2,		// Konstruktor
                 bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
