#ifndef _GEGNER_SIDEROCKET_H_
#define _GEGNER_SIDEROCKET_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

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
