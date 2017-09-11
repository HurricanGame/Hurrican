#ifndef _GEGNER_FIESERWALKER_HPP_
#define _GEGNER_FIESERWALKER_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerFieserWalker : public GegnerClass
{
private :
    float	ShotDelay;

public :

    GegnerFieserWalker(int  Wert1, int Wert2, bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
