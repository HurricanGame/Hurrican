#ifndef _TRIGGER_LIFT_HPP_
#define _TRIGGER_LIFT_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

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
