#ifndef _TRIGGER_FLOATING2_H_
#define _TRIGGER_FLOATING2_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerFloating2 : public GegnerClass
{
private :
    bool	playeron;

public:

    GegnerFloating2	   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
