#ifndef _GEGNER_KRABBLEROBEN_H_
#define _GEGNER_KRABBLEROBEN_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerKrabblerOben : public GegnerClass
{
public:

    float				ShotDelay;						// Schussverzögerung

    GegnerKrabblerOben(int  Wert1, int Wert2,			// Konstruktor
                       bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
