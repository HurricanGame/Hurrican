#ifndef _GEGNER_WALKER_H
#define _GEGNER_WALKER_H

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerWalker : public GegnerClass
{
private:
    float ShotDelay;

public:

    GegnerWalker	   (int Wert1, int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
