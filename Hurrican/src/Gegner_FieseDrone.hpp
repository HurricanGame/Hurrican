#ifndef _GEGNER_FIESEDRONE_HPP_
#define _GEGNER_FIESEDRONE_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerFieseDrone : public GegnerClass
{
private :
    float ShotDelay;
    float SmokeDelay;

public :

    GegnerFieseDrone(int  Wert1, int Wert2,					// Konstruktor
                     bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
