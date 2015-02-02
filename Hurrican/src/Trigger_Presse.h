#ifndef _TRIGGER_PRESSE_H_
#define _TRIGGER_PRESSE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerPresse : public GegnerClass
{
public:

    GegnerPresse	   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
