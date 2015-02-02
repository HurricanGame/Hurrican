#ifndef _TRIGGER_SECRET_H_
#define _TRIGGER_SECRET_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerSecret : public GegnerClass
{
public:

    GegnerSecret	   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
