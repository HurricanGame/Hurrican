#ifndef _GEGNER_LAFASS_H_
#define _GEGNER_LAFASS_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerLaFass : public GegnerClass
{
public:

    GegnerLaFass  (int Wert1,	int Wert2,			// Konstruktor
                   bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
