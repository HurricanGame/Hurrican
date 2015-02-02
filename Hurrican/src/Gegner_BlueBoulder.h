#ifndef _GEGNER_BLUEBOULDER_H_
#define _GEGNER_BLUEBOULDER_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerBlueBoulder : public GegnerClass
{
public:

    GegnerBlueBoulder   (int  Wert1, int Wert2,			// Konstruktor
                         bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
