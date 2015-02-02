#ifndef _TRIGGER_WARNING_H_
#define _TRIGGER_WARNING_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerWarning : public GegnerClass
{
public:

    GegnerWarning  (int Wert1,	int Wert2,		// Konstruktor
                    bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
