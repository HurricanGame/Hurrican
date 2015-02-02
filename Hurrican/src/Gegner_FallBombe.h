#ifndef _GEGNER_FALLBOMBE_H_
#define _GEGNER_FALLBOMBE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerFallBombe : public GegnerClass
{
private:
    float	SmokeDelay;
public:

    GegnerFallBombe   (int  Wert1, int Wert2,			// Konstruktor
                       bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
