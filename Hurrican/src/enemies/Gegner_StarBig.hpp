#ifndef _GEGNER_STARBIG_HPP_
#define _GEGNER_STARBIG_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerStarBig : public GegnerClass
{
private :
    int ShotCount;
public :

    GegnerStarBig (int  Wert1, int Wert2,			// Konstruktor
                   bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
