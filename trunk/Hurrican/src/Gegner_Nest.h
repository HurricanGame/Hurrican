#ifndef _GEGNER_NEST_H_
#define _GEGNER_NEST_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

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

