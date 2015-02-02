#ifndef _GEGNER_STARSMALL_H_
#define _GEGNER_STARSMALL_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerStarSmall : public GegnerClass
{
public :

    GegnerStarSmall (int  Wert1, int Wert2,			// Konstruktor
                     bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
