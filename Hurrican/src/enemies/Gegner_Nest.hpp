#ifndef _GEGNER_NEST_HPP_
#define _GEGNER_NEST_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerNest : public GegnerClass
{
public :

    GegnerNest (int  Wert1, int Wert2,					// Konstruktor
                bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif

