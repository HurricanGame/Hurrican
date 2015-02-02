#ifndef _GEGNER_SPIDERBOMB_H_
#define _GEGNER_SPIDERBOMB_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerSpiderBomb : public GegnerClass
{
public:

    GegnerSpiderBomb   (int Wert1, int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
