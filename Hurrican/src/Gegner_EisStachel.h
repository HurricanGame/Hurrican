#ifndef _GEGNER_EISSTACHEL_H_
#define _GEGNER_EISSTACHEL_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerEisStachel : public GegnerClass
{
public:

    GegnerEisStachel(int  Wert1, int Wert2,				// Konstruktor
                     bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
