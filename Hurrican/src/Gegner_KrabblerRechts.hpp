#ifndef _GEGNER_KRABBLERRECHTS_HPP_
#define _GEGNER_KRABBLERRECHTS_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

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
