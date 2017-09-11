#ifndef _BOSS_RIESENPIRANHA_H_
#define _BOSS_RIESENPIRANHA_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerRiesenPiranha : public GegnerClass
{
public:

    float				ShotDelay;						// Schussverzögerung

    GegnerRiesenPiranha(int  Wert1, int Wert2,			// Konstruktor
                        bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
