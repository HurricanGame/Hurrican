#ifndef _GEGNER_STELZSACK_H_
#define _GEGNER_STELZSACK_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerStelzSack : public GegnerClass
{
public:

    GegnerStelzSack   (int  Wert1, int Wert2,			// Konstruktor
                       bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
