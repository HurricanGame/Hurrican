#ifndef _TRIGGER_SURFBRETT_H_
#define _TRIGGER_SURFBRETT_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerSurfBrett : public GegnerClass
{
private:
    float	SmokeCount;

public:

    GegnerSurfBrett	   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
