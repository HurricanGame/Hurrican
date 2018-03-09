#ifndef _GEGNER_SIDEROCKET_HPP_
#define _GEGNER_SIDEROCKET_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerSideRocket : public GegnerClass
{
public :

    GegnerSideRocket(int  Wert1, int Wert2,					// Konstruktor
                     bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
