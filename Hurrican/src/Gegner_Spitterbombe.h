#ifndef _GEGNER_SPITTERBOMBE_H_
#define _GEGNER_SPITTERBOMBE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerSpitterbombe : public GegnerClass
{
public :

    GegnerSpitterbombe (int  Wert1, int Wert2,			// Konstruktor
                        bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
