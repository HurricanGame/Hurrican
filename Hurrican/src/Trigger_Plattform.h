#ifndef _TRIGGER_PLATTFORM_H_
#define _TRIGGER_PLATTFORM_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerPlattform : public GegnerClass
{
public:

    GegnerPlattform   (int Wert1,	int Wert2,			// Konstruktor
                       bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
