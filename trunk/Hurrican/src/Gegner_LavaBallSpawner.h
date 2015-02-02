#ifndef _GEGNER_LAVABALLSPAWNER_H_
#define _GEGNER_LAVABALLSPAWNER_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerLavaBallSpawner : public GegnerClass
{
public :
    float			Delay;

    GegnerLavaBallSpawner (int  Wert1, int Wert2,				// Konstruktor
                           bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif

