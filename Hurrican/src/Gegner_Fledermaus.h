#ifndef _GEGNER_FLEDERMAUS_H_
#define _GEGNER_FLEDERMAUS_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerFledermaus : public GegnerClass
{
public :

    GegnerFledermaus(int  Wert1, int Wert2,			// Konstruktor
                     bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
