#ifndef _GEGNER_ROBOMAN1_HPP_
#define _GEGNER_ROBOMAN1_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

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
