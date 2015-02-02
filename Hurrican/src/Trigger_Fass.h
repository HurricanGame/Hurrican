#ifndef _TRIGGER_FASS_H_
#define _TRIGGER_FASS_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerFass : public GegnerClass
{
public:

    GegnerFass		   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
