#ifndef _GEGNER_KRABBLERRECHTS_H_
#define _GEGNER_KRABBLERRECHTS_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerKrabblerRechts : public GegnerClass
{
public:

    float				ShotDelay;						// Schussverzögerung

    GegnerKrabblerRechts(int  Wert1, int Wert2,			// Konstruktor
                         bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
