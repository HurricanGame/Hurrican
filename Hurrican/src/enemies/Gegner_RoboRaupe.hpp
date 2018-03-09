#ifndef _GEGNER_ROBORAUPE_HPP_
#define _GEGNER_ROBORAUPE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerRoboRaupe : public GegnerClass
{
public:

    GegnerRoboRaupe(int  Wert1, int Wert2,				// Konstruktor
                    bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
