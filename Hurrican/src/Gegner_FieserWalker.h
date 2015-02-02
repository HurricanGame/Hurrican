#ifndef _GEGNER_FIESERWALKER_H_
#define _GEGNER_FIESERWALKER_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

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
