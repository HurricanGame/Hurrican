#ifndef _TRIGGER_SHRINE_H_
#define _TRIGGER_SHRINE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerShrine : public GegnerClass
{
public:

    GegnerShrine	   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
