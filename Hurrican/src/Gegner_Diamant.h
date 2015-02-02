#ifndef _GEGNER_DIAMANT_H
#define _GEGNER_DIAMANT_H

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerDiamant : public GegnerClass
{
public:

    GegnerDiamant		(int Wert1, int Wert2,			// Konstruktor
                         bool			Light);
    void GegnerExplode  (void);							// Gegner explodiert
    void DoKI			(void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
