#ifndef _GEGNER_FIESEDRONE_H_
#define _GEGNER_FIESEDRONE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

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
