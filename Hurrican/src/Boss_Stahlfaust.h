#ifndef _BOSS_STAHLFAUST_H_
#define _BOSS_STAHLFAUST_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerStahlfaust : public GegnerClass
{
public:
    float	SmokeDelay;

    GegnerStahlfaust   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
