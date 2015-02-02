#ifndef _TRIGGER_FLOATING_H_
#define _TRIGGER_FLOATING_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerFloating : public GegnerClass
{
private :
    bool	playeron;

public:

    GegnerFloating	   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
